// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef CDSUNIT_ENABLE_BOOST_CONTAINER

#include <boost/version.hpp>
#include <cds/details/defs.h>

#if BOOST_VERSION >= 104800

#include <cds/container/striped_map/boost_slist.h>
#include "test_striped_map.h"

namespace {
    struct test_traits
    {
        typedef boost::container::slist< std::pair< cds_test::striped_map_fixture::key_type const, cds_test::striped_map_fixture::value_type >> container_type;

        struct copy_policy {
            typedef container_type::iterator iterator;

            void operator()( container_type& list, iterator itInsert, iterator itWhat )
            {
                list.insert_after( itInsert, *itWhat );
            }
        };

        static bool const c_hasFindWith = true;
        static bool const c_hasEraseWith = true;
    };

    INSTANTIATE_TYPED_TEST_CASE_P( BoostSList, StripedMap, test_traits );
    INSTANTIATE_TYPED_TEST_CASE_P( BoostSList, RefinableMap, test_traits );

} // namespace

#else // BOOST_VERSION < 104800
// Skipped; for boost::container::slist you should use boost version 1.48 or above
#endif  // BOOST_VERSION

#endif // #ifdef CDSUNIT_ENABLE_BOOST_CONTAINER
