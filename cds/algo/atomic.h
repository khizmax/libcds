// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CXX11_ATOMIC_H
#define CDSLIB_CXX11_ATOMIC_H

#include <cds/details/defs.h>
#include <cds/user_setup/cache_line.h>

namespace cds {

/// C++11 Atomic library support
/** @anchor cds_cxx11_atomic
    \p libcds can use the following implementations of the atomics:
    - STL \p &lt;atomic&gt;. This is used by default
    - \p boost.atomic for boost 1.54 and above. To use it you should define \p CDS_USE_BOOST_ATOMIC for
      your compiler invocation, for example, for gcc specify \p -DCDS_USE_BOOST_ATOMIC
      in command line
    - \p libcds implementation of atomic operation according to C++11 standard as
      specified in <a href="http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2011/n3242.pdf">N3242, p.29</a>.
      \p libcds implementation is not the full standard compliant, it provides only C++ part of standard,
      for example, \p libcds has no static initialization of the atomic variables and some other C features.
      However, that imlementation is enough for the library purposes. Supported architecture: x86, amd64,
      ia64 (Itanium) 64bit, 64bit Sparc. To use \p libcds atomic you should define \p CDS_USE_LIBCDS_ATOMIC
      in the compiler command line (\p -DCDS_USE_LIBCDS_ATOMIC for gcc/clang).

      @note For Clang compiler \p libcds doesn't use native \p libc++ \p &lt;atomic&gt; due some problems.
      Instead, \p libcds atomic is used by default, or you can try to use \p boost.atomic.

      The library defines \p atomics alias for atomic namespace:
      - <tt>namespace atomics = std</tt> for STL
      - <tt>namespace atomics = boost</tt> for \p boost.atomic
      - <tt>namespace atomics = cds::cxx11_atomic</tt> for library-provided atomic implementation
*/
namespace cxx11_atomic {
}} // namespace cds::cxx11_atomic

//@cond
#if defined(CDS_USE_BOOST_ATOMIC)
    // boost atomic
#   include <boost/version.hpp>
#   if BOOST_VERSION >= 105400
#       include <boost/atomic.hpp>
        namespace atomics = boost;
#       define CDS_CXX11_ATOMIC_BEGIN_NAMESPACE namespace boost {
#       define CDS_CXX11_ATOMIC_END_NAMESPACE }
#   else
#       error "Boost version 1.54 or above is needed for boost.atomic"
#   endif
#elif defined(CDS_USE_LIBCDS_ATOMIC)
    // libcds atomic
#   include <cds/compiler/cxx11_atomic.h>
    namespace atomics = cds::cxx11_atomic;
#   define CDS_CXX11_ATOMIC_BEGIN_NAMESPACE namespace cds { namespace cxx11_atomic {
#   define CDS_CXX11_ATOMIC_END_NAMESPACE }}
#else
    // Compiler provided C++11 atomic
#   include <atomic>
    namespace atomics = std;
#   define CDS_CXX11_ATOMIC_BEGIN_NAMESPACE namespace std {
#   define CDS_CXX11_ATOMIC_END_NAMESPACE }
#endif
//@endcond

namespace cds {

    /// Atomic primitives
    /**
        This namespace contains useful primitives derived from <tt>std::atomic</tt>.
    */
    namespace atomicity {

        /// Atomic event counter.
        /**
            This class is based on <tt>std::atomic_size_t</tt>.
            It uses relaxed memory ordering \p memory_order_relaxed and may be used as a statistic counter.
        */
        class event_counter
        {
            //@cond
            atomics::atomic_size_t   m_counter;
            //@endcond

        public:
            typedef size_t      value_type  ;       ///< Type of counter

        public:
            // Initializes event counter with zero
            event_counter() noexcept
                : m_counter(size_t(0))
            {}

            /// Assign operator
            /**
                Returns \p n.
            */
            value_type operator =(
                value_type n    ///< new value of the counter
            ) noexcept
            {
                m_counter.exchange( n, atomics::memory_order_relaxed );
                return n;
            }

            /// Addition
            /**
                Returns new value of the atomic counter.
            */
            size_t operator +=(
                size_t n    ///< addendum
            ) noexcept
            {
                return m_counter.fetch_add( n, atomics::memory_order_relaxed ) + n;
            }

            /// Substraction
            /**
                Returns new value of the atomic counter.
            */
            size_t operator -=(
                size_t n    ///< subtrahend
            ) noexcept
            {
                return m_counter.fetch_sub( n, atomics::memory_order_relaxed ) - n;
            }

            /// Get current value of the counter
            operator size_t () const noexcept
            {
                return m_counter.load( atomics::memory_order_relaxed );
            }

            /// Preincrement
            size_t operator ++() noexcept
            {
                return m_counter.fetch_add( 1, atomics::memory_order_relaxed ) + 1;
            }
            /// Postincrement
            size_t operator ++(int) noexcept
            {
                return m_counter.fetch_add( 1, atomics::memory_order_relaxed );
            }

            /// Predecrement
            size_t operator --() noexcept
            {
                return m_counter.fetch_sub( 1, atomics::memory_order_relaxed ) - 1;
            }
            /// Postdecrement
            size_t operator --(int) noexcept
            {
                return m_counter.fetch_sub( 1, atomics::memory_order_relaxed );
            }

            /// Get current value of the counter
            size_t get() const noexcept
            {
                return m_counter.load( atomics::memory_order_relaxed );
            }

            /// Resets the counter to 0
            void reset() noexcept
            {
                m_counter.store( 0, atomics::memory_order_release );
            }
        };

        /// Atomic item counter
        /**
            This class is simplified interface around \p std::atomic_size_t.
            The class supports getting current value of the counter and increment/decrement its value.

            See also: improved version that eliminates false sharing - \p cache_friendly_item_counter.
        */
        class item_counter
        {
        public:
            typedef atomics::atomic_size_t   atomic_type;   ///< atomic type used
            typedef size_t counter_type;                    ///< Integral item counter type (size_t)

        private:
            //@cond
            atomic_type     m_Counter;   ///< Atomic item counter
            //@endcond

        public:
            /// Default ctor initializes the counter to zero.
            item_counter()
                : m_Counter(counter_type(0))
            {}

            /// Returns current value of the counter
            counter_type value(atomics::memory_order order = atomics::memory_order_relaxed) const
            {
                return m_Counter.load( order );
            }

            /// Same as \ref value() with relaxed memory ordering
            operator counter_type() const
            {
                return value();
            }

            /// Returns underlying atomic interface
            atomic_type& getAtomic()
            {
                return m_Counter;
            }

            /// Returns underlying atomic interface (const)
            const atomic_type& getAtomic() const
            {
                return m_Counter;
            }

            /// Increments the counter. Semantics: postincrement
            counter_type inc(atomics::memory_order order = atomics::memory_order_relaxed )
            {
                return m_Counter.fetch_add( 1, order );
            }

            /// Increments the counter. Semantics: postincrement
            counter_type inc( counter_type count, atomics::memory_order order = atomics::memory_order_relaxed )
            {
                return m_Counter.fetch_add( count, order );
            }

            /// Decrements the counter. Semantics: postdecrement
            counter_type dec(atomics::memory_order order = atomics::memory_order_relaxed)
            {
                return m_Counter.fetch_sub( 1, order );
            }

            /// Decrements the counter. Semantics: postdecrement
            counter_type dec( counter_type count, atomics::memory_order order = atomics::memory_order_relaxed )
            {
                return m_Counter.fetch_sub( count, order );
            }

            /// Preincrement
            counter_type operator ++()
            {
                return inc() + 1;
            }
            /// Postincrement
            counter_type operator ++(int)
            {
                return inc();
            }

            /// Predecrement
            counter_type operator --()
            {
                return dec() - 1;
            }
            /// Postdecrement
            counter_type operator --(int)
            {
                return dec();
            }

            /// Increment by \p count
            counter_type operator +=( counter_type count )
            {
                return inc( count ) + count;
            }

            /// Decrement by \p count
            counter_type operator -=( counter_type count )
            {
                return dec( count ) - count;
            }

            /// Resets count to 0
            void reset(atomics::memory_order order = atomics::memory_order_relaxed)
            {
                m_Counter.store( 0, order );
            }
        };

#if CDS_COMPILER == CDS_COMPILER_CLANG
    // CLang unhappy: pad1_ and pad2_ - unused private field warning
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wunused-private-field"
#endif
        /// Atomic cache-friendly item counter
        /**
            Atomic item counter with cache-line padding to avoid false sharing.
            Adding cache-line padding before and after atomic counter eliminates the contention
            in read path of many containers and can notably improve search operations in sets/maps.
        */
        class cache_friendly_item_counter
        {
        public:
            typedef atomics::atomic_size_t   atomic_type;   ///< atomic type used
            typedef size_t counter_type;                    ///< Integral item counter type (size_t)

        private:
            //@cond
            char            pad1_[cds::c_nCacheLineSize];
            atomic_type     m_Counter;   ///< Atomic item counter
            char            pad2_[cds::c_nCacheLineSize - sizeof( atomic_type )];
            //@endcond

        public:
            /// Default ctor initializes the counter to zero.
            cache_friendly_item_counter()
                : m_Counter(counter_type(0))
            {}

            /// Returns current value of the counter
            counter_type value(atomics::memory_order order = atomics::memory_order_relaxed) const
            {
                return m_Counter.load( order );
            }

            /// Same as \ref value() with relaxed memory ordering
            operator counter_type() const
            {
                return value();
            }

            /// Returns underlying atomic interface
            atomic_type& getAtomic()
            {
                return m_Counter;
            }

            /// Returns underlying atomic interface (const)
            const atomic_type& getAtomic() const
            {
                return m_Counter;
            }

            /// Increments the counter. Semantics: postincrement
            counter_type inc(atomics::memory_order order = atomics::memory_order_relaxed )
            {
                return m_Counter.fetch_add( 1, order );
            }

            /// Increments the counter. Semantics: postincrement
            counter_type inc( counter_type count, atomics::memory_order order = atomics::memory_order_relaxed )
            {
                return m_Counter.fetch_add( count, order );
            }

            /// Decrements the counter. Semantics: postdecrement
            counter_type dec(atomics::memory_order order = atomics::memory_order_relaxed)
            {
                return m_Counter.fetch_sub( 1, order );
            }

            /// Decrements the counter. Semantics: postdecrement
            counter_type dec( counter_type count, atomics::memory_order order = atomics::memory_order_relaxed )
            {
                return m_Counter.fetch_sub( count, order );
            }

            /// Preincrement
            counter_type operator ++()
            {
                return inc() + 1;
            }
            /// Postincrement
            counter_type operator ++(int)
            {
                return inc();
            }

            /// Predecrement
            counter_type operator --()
            {
                return dec() - 1;
            }
            /// Postdecrement
            counter_type operator --(int)
            {
                return dec();
            }

            /// Increment by \p count
            counter_type operator +=( counter_type count )
            {
                return inc( count ) + count;
            }

            /// Decrement by \p count
            counter_type operator -=( counter_type count )
            {
                return dec( count ) - count;
            }

            /// Resets count to 0
            void reset(atomics::memory_order order = atomics::memory_order_relaxed)
            {
                m_Counter.store( 0, order );
            }
        };
#if CDS_COMPILER == CDS_COMPILER_CLANG
#   pragma GCC diagnostic pop
#endif

        /// Empty item counter
        /**
            This class may be used instead of \ref item_counter when you do not need full \ref item_counter interface.
            All methods of the class is empty and returns 0.

            The object of this class should not be used in data structure that behavior significantly depends on item counting
            (for example, in many hash map implementation).
        */
        class empty_item_counter {
        public:
            typedef size_t counter_type    ;  ///< Counter type
        public:
            /// Returns 0
            static counter_type value(atomics::memory_order /*order*/ = atomics::memory_order_relaxed)
            {
                return 0;
            }

            /// Same as \ref value(), always returns 0.
            operator counter_type() const
            {
                return value();
            }

            /// Dummy increment. Always returns 0
            static counter_type inc(atomics::memory_order /*order*/ = atomics::memory_order_relaxed)
            {
                return 0;
            }

            /// Dummy increment. Always returns 0
            static counter_type inc( counter_type /*count*/, atomics::memory_order /*order*/ = atomics::memory_order_relaxed )
            {
                return 0;
            }

            /// Dummy increment. Always returns 0
            static counter_type dec(atomics::memory_order /*order*/ = atomics::memory_order_relaxed)
            {
                return 0;
            }

            /// Dummy increment. Always returns 0
            static counter_type dec( counter_type /*count*/, atomics::memory_order /*order*/ = atomics::memory_order_relaxed )
            {
                return 0;
            }

            /// Dummy pre-increment. Always returns 0
            counter_type operator ++() const
            {
                return 0;
            }
            /// Dummy post-increment. Always returns 0
            counter_type operator ++(int) const
            {
                return 0;
            }

            /// Dummy pre-decrement. Always returns 0
            counter_type operator --() const
            {
                return 0;
            }
            /// Dummy post-decrement. Always returns 0
            counter_type operator --(int) const
            {
                return 0;
            }

            /// Dummy increment by \p count, always returns 0
            counter_type operator +=( counter_type count )
            {
                CDS_UNUSED( count );
                return 0;
            }

            /// Dummy decrement by \p count, always returns 0
            counter_type operator -=( counter_type count )
            {
                CDS_UNUSED( count );
                return 0;
            }

            /// Dummy function
            static void reset(atomics::memory_order /*order*/ = atomics::memory_order_relaxed)
            {}
        };
    }   // namespace atomicity
}   // namespace cds

#endif // #ifndef CDSLIB_CXX11_ATOMIC_H
