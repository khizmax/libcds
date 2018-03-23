// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef CDSUNIT_ENABLE_BOOST_CONTAINER

#include "test_intrusive_striped_set.h"
#include <cds/intrusive/striped_set/boost_avl_set.h>

namespace {
    namespace ci = cds::intrusive;
    namespace bi = boost::intrusive;

    struct test_traits
    {
        typedef cds_test::intrusive_set::base_int_item< bi::avl_set_base_hook<> > base_item;
        typedef cds_test::intrusive_set::member_int_item< bi::avl_set_member_hook<> > member_item;

        typedef bi::avl_set< base_item,
            bi::compare< cds_test::intrusive_set::less< base_item >>
        > base_hook_container;

        typedef bi::avl_set< member_item,
            bi::member_hook< member_item, bi::avl_set_member_hook<>, &member_item::hMember>,
            bi::compare< cds_test::intrusive_set::less< member_item >>
        > member_hook_container;
    };

    INSTANTIATE_TYPED_TEST_CASE_P( BoostAvlSet, IntrusiveStripedSet, test_traits );

} // namespace

#endif // CDSUNIT_ENABLE_BOOST_CONTAINER
