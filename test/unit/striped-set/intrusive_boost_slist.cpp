// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef CDSUNIT_ENABLE_BOOST_CONTAINER

#include <cds/intrusive/striped_set/boost_slist.h>
#include "test_intrusive_striped_set.h"

namespace {
    namespace ci = cds::intrusive;
    namespace bi = boost::intrusive;

    struct test_traits
    {
        typedef cds_test::intrusive_set::base_int_item< bi::slist_base_hook<> > base_item;
        typedef cds_test::intrusive_set::member_int_item< bi::slist_member_hook<> > member_item;

        typedef bi::slist< base_item, bi::constant_time_size<true>> base_hook_container;

        typedef bi::slist< member_item,
            bi::member_hook< member_item, bi::slist_member_hook<>, &member_item::hMember>,
            bi::constant_time_size<true>
        > member_hook_container;
    };

    INSTANTIATE_TYPED_TEST_CASE_P( BoostSList, IntrusiveStripedSet, test_traits );

} // namespace

#endif // CDSUNIT_ENABLE_BOOST_CONTAINER
