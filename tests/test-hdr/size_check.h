//$$CDS-header$$

#ifndef CDSTEST_SIZE_CHECK_H
#define CDSTEST_SIZE_CHECK_H

#include <cds/algo/atomic.h>

namespace misc {

    template <typename ItemCounter>
    struct size_checker {
        template <typename Container>
        bool operator()( Container const& c, size_t nSize ) const
        {
            return c.size() == nSize;
        }
    };

    template<>
    struct size_checker< cds::atomicity::empty_item_counter > {
        template <typename Container>
        bool operator()( Container const& /*c*/, size_t /*nSize*/ ) const
        {
            return true;
        }
    };

    template <class Container>
    static inline bool check_size( Container const& cont, size_t nSize )
    {
        return size_checker<typename Container::item_counter>()( cont, nSize );
    }

} // namespace misc

#endif // #ifndef CDSTEST_SIZE_CHECK_H
