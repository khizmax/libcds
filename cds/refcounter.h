//$$CDS-header$$

#ifndef __CDS_REFCOUNTER_H
#define __CDS_REFCOUNTER_H

/*
    Refernce counting primitives

    Editions:
*/

#include <cds/cxx11_atomic.h>

namespace cds {

    /// Simple reference counter
    /**
        Thread-safe reference counter build on atomic primitives.
        \p T is one of integer types
    */
    template <typename T>
    class ref_counter
    {
        CDS_ATOMIC::atomic<T>   m_nRefCount    ;        ///< The reference counter

    public:
        typedef T   ref_counter_type  ; ///< The reference counter type

    public:
        /// Construct reference counter with specified value \p initValue
        ref_counter( T initValue = 0 ) CDS_NOEXCEPT
            : m_nRefCount(initValue)
        {}

        /// Get current value of reference counter.
        T   value() const CDS_NOEXCEPT
        {
            return m_nRefCount.load( CDS_ATOMIC::memory_order_relaxed );
        }

        /// Current value of reference counter
        operator T() const CDS_NOEXCEPT
        {
            return value();
        }

        /// Atomic increment
        void    inc() CDS_NOEXCEPT
        {
            m_nRefCount.fetch_add( 1, CDS_ATOMIC::memory_order_relaxed );
        }

        /// Atomic decrement. Return \p true if reference counter is 0, otherwise \p false
        bool    dec() CDS_NOEXCEPT
        {
            if ( m_nRefCount.fetch_sub( 1, CDS_ATOMIC::memory_order_relaxed ) == 1 ) {
                CDS_ATOMIC::atomic_thread_fence( CDS_ATOMIC::memory_order_release );
                return true;
            }
            return false;
        }

        /// Atomic increment
        void operator ++() CDS_NOEXCEPT
        {
            inc();
        }

        /// Atomic decrement
        bool operator--() CDS_NOEXCEPT
        {
            return dec();
        }
    };

    /// Signed 32bit reference counter
    typedef ref_counter<cds::int32_t>       signed_ref_counter;

    /// Unsigned 32bit reference counter
    typedef ref_counter<cds::uint32_t>      unsigned_ref_counter;

} // namespace cds

#endif    // #ifndef __CDS_REFCOUNTER_H
