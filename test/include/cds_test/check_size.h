// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSTEST_CHECK_SIZE_H
#define CDSTEST_CHECK_SIZE_H

#include <cds_test/ext_gtest.h>
#include <cds/algo/atomic.h>

namespace cds_test {

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

} // namespace cds_test

#define ASSERT_CONTAINER_SIZE( container, expected ) \
    ASSERT_TRUE( cds_test::check_size(container, expected)) << "container size=" << container.size() << ", expected=" << expected

#define EXPECT_CONTAINER_SIZE( container, expected ) \
    EXPECT_TRUE( cds_test::check_size(container, expected)) << "container size=" << container.size() << ", expected=" << expected

#endif // #ifndef CDSTEST_CHECK_SIZE_H
