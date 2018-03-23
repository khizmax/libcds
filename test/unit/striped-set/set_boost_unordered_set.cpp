// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef CDSUNIT_ENABLE_BOOST_CONTAINER

#include <cds/container/striped_set/boost_unordered_set.h>
#include "test_striped_set.h"

namespace {
    struct test_traits
    {
        typedef boost::unordered_set< cds_test::container_set::int_item, cds_test::container_set::hash2, cds_test::container_set::equal_to > container_type;

        struct copy_policy {
            typedef container_type::iterator iterator;

            void operator()( container_type& set, iterator itWhat )
            {
                set.insert( *itWhat );
            }
        };

        static bool const c_hasFindWith = false;
        static bool const c_hasEraseWith = false;
    };

    INSTANTIATE_TYPED_TEST_CASE_P( BoostUnorderedSet, StripedSet, test_traits );
    INSTANTIATE_TYPED_TEST_CASE_P( BoostUnorderedSet, RefinableSet, test_traits );

} // namespace

#endif // CDSUNIT_ENABLE_BOOST_CONTAINER
