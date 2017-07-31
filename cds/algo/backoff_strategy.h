/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CDSLIB_BACKOFF_STRATEGY_H
#define CDSLIB_BACKOFF_STRATEGY_H

/*
    Filename: backoff_strategy.h
    Created 2007.03.01 by Maxim Khiszinsky

    Description:
         Generic back-off strategies

    Editions:
    2007.03.01  Maxim Khiszinsky    Created
    2008.10.02  Maxim Khiszinsky    Backoff action transfers from contructor to operator() for all backoff schemas
    2009.09.10  Maxim Khiszinsky    reset() function added
*/

#include <utility>      // declval
#include <thread>
#include <chrono>
#include <cds/compiler/backoff.h>

namespace cds {
    /// Different backoff schemes
    /**
        Back-off schema may be used in lock-free algorithms when the algorithm cannot perform some action because a conflict
        with the other concurrent operation is encountered. In this case current thread can do another work or can call
        processor's performance hint.

        The interface of back-off strategy is following:
        \code
            struct backoff_strategy {
                void operator()();
                template <typename Predicate> bool operator()( Predicate pr );
                void reset();
            };
        \endcode

        \p operator() operator calls back-off strategy's action. It is main part of back-off strategy.

        Interruptible back-off <tt>template < typename Predicate > bool operator()( Predicate pr )</tt>
        allows to interrupt back-off spinning if \p pr predicate returns \p true.
        \p Predicate is a functor with the following interface:
        \code
        struct predicate {
            bool operator()();
        };
        \endcode

        \p reset() function resets internal state of back-off strategy to initial state. It is required for some
        back-off strategies, for example, exponential back-off.
    */
    namespace backoff {

        /// Empty backoff strategy. Do nothing
        struct empty {
            //@cond
            void operator ()() const CDS_NOEXCEPT
            {}

            template <typename Predicate>
            bool operator()(Predicate pr) const CDS_NOEXCEPT_( noexcept(std::declval<Predicate>()()))
            {
                return pr();
            }

            static void reset() CDS_NOEXCEPT
            {}
            //@endcond
        };

        /// Switch to another thread (yield). Good for thread preemption architecture.
        struct yield {
            //@cond
            void operator ()() const CDS_NOEXCEPT
            {
                std::this_thread::yield();
            }

            template <typename Predicate>
            bool operator()(Predicate pr) const CDS_NOEXCEPT_( noexcept(std::declval<Predicate>()()))
            {
                if ( pr())
                    return true;
                operator()();
                return false;
            }

            static void reset() CDS_NOEXCEPT
            {}
            //@endcond
        };

        /// Random pause
        /**
            This back-off strategy calls processor-specific pause hint instruction
            if one is available for the processor architecture.
        */
        struct pause {
            //@cond
            void operator ()() const CDS_NOEXCEPT
            {
#            ifdef CDS_backoff_hint_defined
                platform::backoff_hint();
#            endif
            }

            template <typename Predicate>
            bool operator()(Predicate pr) const CDS_NOEXCEPT_( noexcept(std::declval<Predicate>()()))
            {
                if ( pr())
                    return true;
                operator()();
                return false;
            }

            static void reset() CDS_NOEXCEPT
            {}
            //@endcond
        };

        /// Processor hint back-off
        /**
            This back-off schema calls performance hint instruction if it is available for current processor.
            Otherwise, it calls \p nop.
        */
        struct hint
        {
        //@cond
            void operator ()() const CDS_NOEXCEPT
            {
#           if defined(CDS_backoff_hint_defined)
                platform::backoff_hint();
#           elif defined(CDS_backoff_nop_defined)
                platform::backoff_nop();
#           endif
            }

            template <typename Predicate>
            bool operator()(Predicate pr) const CDS_NOEXCEPT_(noexcept(std::declval<Predicate>()()))
            {
                if ( pr())
                    return true;
                operator()();
                return false;
            }

            static void reset() CDS_NOEXCEPT
            {}
        //@endcond
        };

        /// \p backoff::exponential const traits
        struct exponential_const_traits
        {
            typedef hint    fast_path_backoff;  ///< Fast-path back-off strategy
            typedef yield   slow_path_backoff;  ///< Slow-path back-off strategy

            enum: size_t {
                lower_bound = 16,         ///< Minimum spinning limit
                upper_bound = 16 * 1024   ///< Maximum spinning limit
            };
        };

        /// \p nackoff::exponential runtime traits
        struct exponential_runtime_traits
        {
            typedef hint    fast_path_backoff;  ///< Fast-path back-off strategy
            typedef yield   slow_path_backoff;  ///< Slow-path back-off strategy

            static size_t lower_bound;    ///< Minimum spinning limit, default is 16
            static size_t upper_bound;    ///< Maximum spinning limit, default is 16*1024
        };

        /// Exponential back-off
        /**
            This back-off strategy is composite. It consists of \p SpinBkoff and \p YieldBkoff
            back-off strategy. In first, the strategy tries to apply repeatedly \p SpinBkoff
            (spinning phase) until internal counter of failed attempts reaches its maximum
            spinning value. Then, the strategy transits to high-contention phase
            where it applies \p YieldBkoff until \p reset() is called.
            On each spinning iteration the internal spinning counter is doubled.

            Selecting the best value for maximum spinning limit is platform and application specific task.
            The limits are described by \p Traits template parameter.
            There are two types of \p Traits:
            - constant traits \p exponential_const_traits - specifies the lower and upper limits
              as a compile-time constants; to change the limits you should recompile your application
            - runtime traits \p exponential_runtime_traits - specifies the limits as \p s_nExpMin
              and \p s_nExpMax variables which can be changed at runtime to tune back-off strategy.

            The traits class must declare two data member:
            - \p lower_bound - the lower bound of spinning loop
            - \p upper_bound - the upper boudn of spinning loop

            You may use \p Traits template parameter to separate back-off implementations.
            For example, you may define two \p exponential back-offs that is the best for your task A and B:
            \code

            #include <cds/algo/backoff_strategy.h>
            namespace bkoff = cds::backoff;

            // the best bounds for task A
            struct traits_A: public bkoff::exponential_const_traits
            {
                static size_t lower_bound;
                static size_t upper_bound;
            };
            size_t traits_A::lower_bound = 1024;
            size_t traits_A::upper_bound = 8 * 1024;

            // the best bounds for task B
            struct traits_B: public bkoff::exponential_const_traits
            {
                static size_t lower_bound;
                static size_t upper_bound;
            };
            size_t traits_A::lower_bound = 16;
            size_t traits_A::upper_bound = 1024;

            // // define your back-off specialization
            typedef bkoff::exponential<traits_A> expBackOffA;
            typedef bkoff::exponential<traits_B> expBackOffB;
            \endcode
        */
        template <typename Traits = exponential_const_traits >
        class exponential
        {
        public:
            typedef Traits     traits;   ///< Traits

            typedef typename traits::fast_path_backoff  spin_backoff    ;   ///< spin (fast-path) back-off strategy
            typedef typename traits::slow_path_backoff  yield_backoff   ;   ///< yield (slow-path) back-off strategy

        protected:
            size_t  m_nExpCur   ;           ///< Current spin counter in range [traits::s_nExpMin, traits::s_nExpMax]

            spin_backoff    m_bkSpin    ;   ///< Spinning (fast-path) phase back-off strategy
            yield_backoff   m_bkYield   ;   ///< Yield phase back-off strategy

        public:
            /// Default ctor
            exponential() CDS_NOEXCEPT
                : m_nExpCur( traits::lower_bound )
            {}

            //@cond
            void operator ()() CDS_NOEXCEPT_(noexcept(std::declval<spin_backoff>()()) && noexcept(std::declval<yield_backoff>()()))
            {
                if ( m_nExpCur <= traits::upper_bound ) {
                    for ( size_t n = 0; n < m_nExpCur; ++n )
                        m_bkSpin();
                    m_nExpCur *= 2;
                }
                else
                    m_bkYield();
            }

            template <typename Predicate>
            bool operator()( Predicate pr ) CDS_NOEXCEPT_( noexcept(std::declval<Predicate>()()) && noexcept(std::declval<spin_backoff>()()) && noexcept(std::declval<yield_backoff>()()))
            {
                if ( m_nExpCur <= traits::upper_bound ) {
                    for ( size_t n = 0; n < m_nExpCur; ++n ) {
                        if ( m_bkSpin(pr))
                            return true;
                    }
                    m_nExpCur *= 2;
                }
                else
                    return m_bkYield(pr);
                return false;
            }

            void reset() CDS_NOEXCEPT_( noexcept( std::declval<spin_backoff>().reset()) && noexcept( std::declval<yield_backoff>().reset()))
            {
                m_nExpCur = traits::lower_bound;
                m_bkSpin.reset();
                m_bkYield.reset();
            }
            //@endcond
        };

        //@cond
        template <typename FastPathBkOff, typename SlowPathBkOff>
        struct make_exponential
        {
            struct traits: public exponential_const_traits
            {
                typedef FastPathBkOff   fast_path_backoff;
                typedef SlowPathBkOff   slow_path_backoff;
            };

            typedef exponential<traits> type;
        };

        template <typename FastPathBkOff, typename SlowPathBkOff>
        using make_exponential_t = typename make_exponential<FastPathBkOff, SlowPathBkOff>::type;
        //@endcond

        /// Constant traits for \ref delay back-off strategy
        struct delay_const_traits
        {
            typedef std::chrono::milliseconds duration_type;    ///< Timeout type
            enum: unsigned {
                timeout = 5                                     ///< Delay timeout
            };
        };

        /// Runtime traits for \ref delay back-off strategy
        struct delay_runtime_traits
        {
            typedef std::chrono::milliseconds duration_type;    ///< Timeout type
            static unsigned timeout;                            ///< Delay timeout, default 5
        };

        /// Delay back-off strategy
        /**
            Template arguments:
            - \p Duration - duration type, default is \p std::chrono::milliseconds
            - \p Traits - a class that defines default timeout.

            Choosing the best value for th timeout is platform and application specific task.
            The default values for timeout is provided by \p Traits class that should
            \p timeout data member. There are two predefined \p Traits implementation:
            - \p delay_const_traits - defines \p timeout as a constant (enum).
              To change timeout you should recompile your application.
            - \p delay_runtime_traits - specifies timeout as static data member that can be changed
              at runtime to tune the back-off strategy.

            You may use \p Traits template parameter to separate back-off implementations.
            For example, you may define two \p delay back-offs for 5 and 10 ms timeout:
            \code

            #include <cds/algo/backoff_strategy.h>
            namespace bkoff = cds::backoff;

            // 5ms delay
            struct ms5
            {
                typedef std::chrono::milliseconds duration_type;
                enum: unsigned { timeout = 5 };
            };

            // 10ms delay, runtime support
            struct ms10
            {
                typedef std::chrono::milliseconds duration_type;
                static unsigned timeout;
            };
            unsigned ms10::timeout = 10;

            // define your back-off specialization
            typedef bkoff::delay<std::chrono::milliseconds, ms5>  delay5;
            typedef bkoff::delay<std::chrono::milliseconds, ms10> delay10;

            \endcode
        */
        template <typename Traits = delay_const_traits>
        class delay
        {
        public:
            typedef Traits   traits;        ///< Traits
            typedef typename Traits::duration_type duration_type; ///< Duration type (default \p std::chrono::milliseconds)

        protected:
            ///@cond
            duration_type const timeout;
            ///@endcond

        public:
            /// Default ctor takes the timeout from \p traits::timeout
            delay() CDS_NOEXCEPT
                : timeout( traits::timeout )
            {}

            /// Initializes timeout from \p nTimeout
            CDS_CONSTEXPR explicit delay( unsigned int nTimeout ) CDS_NOEXCEPT
                : timeout( nTimeout )
            {}

            //@cond
            void operator()() const
            {
                std::this_thread::sleep_for( timeout );
            }

            template <typename Predicate>
            bool operator()(Predicate pr) const
            {
                for ( unsigned int i = 0; i < traits::timeout; i += 2 ) {
                    if ( pr())
                        return true;
                    std::this_thread::sleep_for( duration_type( 2 ));
                }
                return false;
            }

            static void reset() CDS_NOEXCEPT
            {}
            //@endcond
        };

        //@cond
        template <unsigned int Timeout, class Duration = std::chrono::milliseconds >
        struct make_delay_of
        {
            struct traits {
                typedef Duration duration_type;
                enum: unsigned { timeout = Timeout };
            };

            typedef delay<traits> type;
        };
        //@endcond

        /// Delay back-off strategy, template version
        /**
            This is a simplified version of \p backoff::delay class.
            Template parameter \p Timeout sets a delay timeout of \p Duration unit.
        */
        template <unsigned int Timeout, class Duration = std::chrono::milliseconds >
        using delay_of = typename make_delay_of< Timeout, Duration >::type;


        /// Default backoff strategy
        typedef exponential<exponential_const_traits>    Default;

        /// Default back-off strategy for lock primitives
        typedef exponential<exponential_const_traits>    LockDefault;

    } // namespace backoff
} // namespace cds


#endif // #ifndef CDSLIB_BACKOFF_STRATEGY_H
