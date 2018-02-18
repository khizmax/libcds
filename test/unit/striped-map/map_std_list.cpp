// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds/container/striped_map/std_list.h>
#include "test_striped_map.h"

namespace {
    struct test_traits
    {
        typedef std::list< std::pair< cds_test::striped_map_fixture::key_type const, cds_test::striped_map_fixture::value_type >> container_type;

        struct copy_policy {
            typedef container_type::iterator iterator;

            void operator()( container_type& list, iterator itInsert, iterator itWhat )
            {
                list.insert( itInsert, *itWhat );
            }
        };

        static bool const c_hasFindWith = true;
        static bool const c_hasEraseWith = true;
    };

    INSTANTIATE_TYPED_TEST_CASE_P( StdList, StripedMap, test_traits );
    INSTANTIATE_TYPED_TEST_CASE_P( StdList, RefinableMap, test_traits );

} // namespace
