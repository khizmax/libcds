// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

// This header should be included AFTER <cds/algo/atomic.h> if needed

namespace {

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

} // namespace
