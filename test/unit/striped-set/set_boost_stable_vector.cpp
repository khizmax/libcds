// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef CDSUNIT_ENABLE_BOOST_CONTAINER

#include <boost/version.hpp>
#include <cds/details/defs.h>

#if BOOST_VERSION >= 104800

#include <cds/container/striped_set/boost_stable_vector.h>
#include "test_striped_set.h"

namespace {
    struct test_traits
    {
        typedef boost::container::stable_vector< cds_test::container_set::int_item > container_type;

        struct copy_policy {
            typedef container_type::iterator iterator;

            void operator()( container_type& vec, iterator itInsert, iterator itWhat )
            {
                vec.insert( itInsert, *itWhat );
            }
        };

        static bool const c_hasFindWith = true;
        static bool const c_hasEraseWith = true;
    };

    INSTANTIATE_TYPED_TEST_CASE_P( BoostStableVector, StripedSet, test_traits );
    INSTANTIATE_TYPED_TEST_CASE_P( BoostStableVector, RefinableSet, test_traits );

} // namespace

#else // BOOST_VERSION < 104800
    // Skipped; for boost::container::stable_vector you should use boost version 1.48 or above
#endif  // BOOST_VERSION

#endif // CDSUNIT_ENABLE_BOOST_CONTAINER
