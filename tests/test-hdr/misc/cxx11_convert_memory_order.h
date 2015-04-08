/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

// This header should be included AFTER <cds/algo/atomic.h> if needed

namespace misc {

    static inline atomics::memory_order convert_to_store_order( atomics::memory_order order )
    {
        switch ( order ) {
            case atomics::memory_order_acquire:
            case atomics::memory_order_consume:
                return atomics::memory_order_relaxed;
            case atomics::memory_order_acq_rel:
                return atomics::memory_order_release;
            default:
                return order;
        }
    }

    static inline atomics::memory_order convert_to_load_order( atomics::memory_order order )
    {
        switch ( order ) {
            case atomics::memory_order_release:
                return atomics::memory_order_relaxed;
            case atomics::memory_order_acq_rel:
                return atomics::memory_order_acquire;
            default:
                return order;
        }
    }

#if CDS_COMPILER == CDS_COMPILER_INTEL
    static inline atomics::memory_order convert_to_exchange_order( atomics::memory_order order )
    {
        return order == atomics::memory_order_consume ? atomics::memory_order_relaxed : order;
    }
#else
    static inline atomics::memory_order convert_to_exchange_order( atomics::memory_order order )
    {
        return order;
    }
#endif

    template <typename T, bool Volatile>
    struct add_volatile;

    template <typename T>
    struct add_volatile<T, false>
    {
        typedef T   type;
    };

    template <typename T>
    struct add_volatile<T, true>
    {
        typedef T volatile   type;
    };

} // namespace misc
