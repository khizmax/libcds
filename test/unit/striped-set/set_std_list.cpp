// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds/container/striped_set/std_list.h>
#include "test_striped_set.h"

namespace {
    struct test_traits
    {
        typedef std::list< cds_test::container_set::int_item > container_type;

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

    INSTANTIATE_TYPED_TEST_CASE_P( StdList, StripedSet, test_traits );
    INSTANTIATE_TYPED_TEST_CASE_P( StdList, RefinableSet, test_traits );

} // namespace
