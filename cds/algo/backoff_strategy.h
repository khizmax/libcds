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
#            ifdef CDS_backoff_pause_defined
                platform::backoff_pause();
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

        /// Exponential back-off
        /**
            This back-off strategy is composite. It consists of \p SpinBkoff and \p YieldBkoff
            back-off strategy. In first, the strategy tries to apply repeatedly \p SpinBkoff
            (spinning phase) until internal counter of failed attempts reaches its maximum
            spinning value. Then, the strategy transits to high-contention phase
            where it applies \p YieldBkoff until \p reset() is called.
            On each spinning iteration the internal spinning counter is doubled.

            Choosing the best value for maximum spinning bound is platform and task specific.
            In this implementation, the default values for maximum and minimum spinning is statically
            declared so you can set its value globally for your platform.
            The third template argument, \p Tag, is used to separate implementation. For
            example, you may define two \p exponential back-offs that is the best for your task A and B:
            \code

            #include <cds/algo/backoff_strategy.h>
            namespace bkoff = cds::backoff;

            struct tagA ;   // tag to select task A implementation
            struct tagB ;   // tag to select task B implementation

            // // define your back-off specialization
            typedef bkoff::exponential<bkoff::hint, bkoff::yield, tagA> expBackOffA;
            typedef bkoff::exponential<bkoff::hint, bkoff::yield, tagB> expBackOffB;

            // // set up the best bounds for task A
            expBackOffA::s_nExpMin = 32;
            expBackOffA::s_nExpMax = 1024;

            // // set up the best bounds for task B
            expBackOffB::s_nExpMin = 2;
            expBackOffB::s_nExpMax = 512;

            \endcode

            Another way of solving this problem is subclassing \p exponential back-off class:
            \code
            #include <cds/algo/backoff_strategy.h>
            namespace bkoff = cds::backoff;
            typedef bkoff::exponential<bkoff::hint, bkoff::yield>   base_bkoff;

            class expBackOffA: public base_bkoff
            {
            public:
                expBackOffA()
                    : base_bkoff( 32, 1024 )
                    {}
            };

            class expBackOffB: public base_bkoff
            {
            public:
                expBackOffB()
                    : base_bkoff( 2, 512 )
                    {}
            };
            \endcode
        */
        template <typename SpinBkoff, typename YieldBkoff, typename Tag=void>
        class exponential
        {
        public:
            typedef SpinBkoff  spin_backoff    ;   ///< spin back-off strategy
            typedef YieldBkoff yield_backoff   ;   ///< yield back-off strategy
            typedef Tag        impl_tag        ;   ///< implementation separation tag

            static size_t s_nExpMin ;   ///< Default minimum spinning bound (16)
            static size_t s_nExpMax ;   ///< Default maximum spinning bound (16384)

        protected:
            size_t  m_nExpCur   ;   ///< Current spinning
            size_t  m_nExpMin   ;   ///< Minimum spinning bound
            size_t  m_nExpMax   ;   ///< Maximum spinning bound

            spin_backoff    m_bkSpin    ;   ///< Spinning (fast-path) phase back-off strategy
            yield_backoff   m_bkYield   ;   ///< Yield phase back-off strategy

        public:
            /// Initializes m_nExpMin and m_nExpMax from default s_nExpMin and s_nExpMax respectively
            exponential() CDS_NOEXCEPT
                : m_nExpMin( s_nExpMin )
                , m_nExpMax( s_nExpMax )
            {
                m_nExpCur = m_nExpMin;
            }

            /// Explicitly defined bounds of spinning
            /**
                The \p libcds library never calls this ctor.
            */
            exponential(
                size_t nExpMin,     ///< Minimum spinning
                size_t nExpMax      ///< Maximum spinning
                ) CDS_NOEXCEPT
                : m_nExpMin( nExpMin )
                , m_nExpMax( nExpMax )
            {
                m_nExpCur = m_nExpMin;
            }

            //@cond
            void operator ()() CDS_NOEXCEPT_(noexcept(std::declval<spin_backoff>()()) && noexcept(std::declval<yield_backoff>()()))
            {
                if ( m_nExpCur <= m_nExpMax ) {
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
                if ( m_nExpCur <= m_nExpMax ) {
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
                m_nExpCur = m_nExpMin;
                m_bkSpin.reset();
                m_bkYield.reset();
            }
            //@endcond
        };

        //@cond
        template <typename SpinBkoff, typename YieldBkoff, typename Tag>
        size_t exponential<SpinBkoff, YieldBkoff, Tag>::s_nExpMin = 16;

        template <typename SpinBkoff, typename YieldBkoff, typename Tag>
        size_t exponential<SpinBkoff, YieldBkoff, Tag>::s_nExpMax = 16 * 1024;
        //@endcond

        /// Delay back-off strategy
        /**
            Template arguments:
            - \p Duration - duration type, default is \p std::chrono::milliseconds
            - \p Tag - a selector tag

            Choosing the best value for th timeout is platform and task specific.
            In this implementation, the default values for timeout is statically
            declared so you can set its value globally for your platform.
            The second template argument, \p Tag, is used to separate implementation. For
            example, you may define two \p delay back-offs for 5 and 10 ms timeout:
            \code

            #include <cds/algo/backoff_strategy.h>
            namespace bkoff = cds::backoff;

            struct ms5  ;   // tag to select 5ms
            struct ms10 ;   // tag to select 10ms

            // // define your back-off specialization
            typedef bkoff::delay<std::chrono::milliseconds, ms5> delay5;
            typedef bkoff::delay<std::chrono::milliseconds, ms10> delay10;

            // // set up the timeouts
            delay5::s_nTimeout = 5;
            delay10::s_nTimeout = 10;
            \endcode

            Another way of solving this problem is subclassing \p delay back-off class:
            \code
            #include <cds/algo/backoff_strategy.h>
            namespace bkoff = cds::backoff;
            typedef bkoff::delay<> delay_bkoff;

            class delay5: public delay_bkoff {
            public:
                delay5(): delay_bkoff( 5 ) {}
            };

            class delay10: public delay_bkoff {
            public:
                delay10(): delay_bkoff( 10 ) {}
            };
            \endcode

        */
        template <class Duration = std::chrono::milliseconds, typename Tag=void >
        class delay
        {
        public:
            typedef Duration duration_type; ///< Duration type (default \p std::chrono::milliseconds)
            static unsigned int s_nTimeout; ///< default timeout, =5

        protected:
            ///@cond
            unsigned int const m_nTimeout;
            ///@endcond

        public:
            /// Default ctor takes the timeout from s_nTimeout
            delay() CDS_NOEXCEPT
                : m_nTimeout( s_nTimeout )
            {}

            /// Initializes timeout from \p nTimeout
            CDS_CONSTEXPR explicit delay( unsigned int nTimeout ) CDS_NOEXCEPT
                : m_nTimeout( nTimeout )
            {}

            //@cond
            void operator()() const
            {
                std::this_thread::sleep_for( duration_type( m_nTimeout ));
            }

            template <typename Predicate>
            bool operator()(Predicate pr) const
            {
                for ( unsigned int i = 0; i < m_nTimeout; i += 2 ) {
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
        template <class Duration, typename Tag>
        unsigned int delay<Duration, Tag>::s_nTimeout = 5;
        //@endcond


        /// Delay back-off strategy, template version
        /**
            This is a template version of backoff::delay class.
            Template parameter \p Timeout sets a delay timeout.
            The declaration <tt>cds::backoff::delay_of< 5 > bkoff</tt> is equal for
            <tt>cds::backoff::delay<> bkoff(5)</tt>.
        */
        template <unsigned int Timeout, class Duration = std::chrono::milliseconds >
        class delay_of: public delay<Duration>
        {
        //@cond
            typedef delay<Duration> base_class;
        public:
            delay_of() CDS_NOEXCEPT
                : base_class( Timeout )
            {}
        //@endcond
        };


        /// Default backoff strategy
        typedef exponential<hint, yield>    Default;

        /// Default back-off strategy for lock primitives
        typedef exponential<hint, yield>    LockDefault;

    } // namespace backoff
} // namespace cds


#endif // #ifndef CDSLIB_BACKOFF_STRATEGY_H
