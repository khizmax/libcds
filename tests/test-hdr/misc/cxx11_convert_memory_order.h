//$$CDS-header$$

// This header should be included AFTER <cds/cxx11_atomic.h> if needed

namespace misc {

    static inline CDS_ATOMIC::memory_order convert_to_store_order( CDS_ATOMIC::memory_order order )
    {
        switch ( order ) {
            case CDS_ATOMIC::memory_order_acquire:
            case CDS_ATOMIC::memory_order_consume:
                return CDS_ATOMIC::memory_order_relaxed;
            case CDS_ATOMIC::memory_order_acq_rel:
                return CDS_ATOMIC::memory_order_release;
            default:
                return order;
        }
    }

    static inline CDS_ATOMIC::memory_order convert_to_load_order( CDS_ATOMIC::memory_order order )
    {
        switch ( order ) {
            case CDS_ATOMIC::memory_order_release:
                return CDS_ATOMIC::memory_order_relaxed;
            case CDS_ATOMIC::memory_order_acq_rel:
                return CDS_ATOMIC::memory_order_acquire;
            default:
                return order;
        }
    }

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
