// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds/container/striped_map/std_map.h>
#include "test_striped_map.h"

namespace {
    struct test_traits
    {
        typedef std::map< cds_test::striped_map_fixture::key_type, cds_test::striped_map_fixture::value_type, cds_test::striped_map_fixture::less > container_type;

        struct copy_policy {
            typedef container_type::iterator iterator;

            void operator()( container_type& m, iterator /*itInsert*/, iterator itWhat )
            {
                m.insert( *itWhat );
            }
        };

        static bool const c_hasFindWith = false;
        static bool const c_hasEraseWith = false;
    };

    INSTANTIATE_TYPED_TEST_CASE_P( StdMap, StripedMap, test_traits );
    INSTANTIATE_TYPED_TEST_CASE_P( StdMap, RefinableMap, test_traits );

} // namespace
