// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef CDSUNIT_ENABLE_BOOST_CONTAINER

#include <boost/version.hpp>

#include "test_intrusive_striped_set.h"
#include <cds/intrusive/striped_set/boost_splay_set.h>

namespace {
    namespace ci = cds::intrusive;
    namespace bi = boost::intrusive;

    struct test_traits
    {
#if BOOST_VERSION < 105600
        typedef bi::splay_set_member_hook<> splay_set_member_hook;
        typedef bi::splay_set_base_hook<> splay_set_base_hook;
#else
        typedef bi::bs_set_base_hook<> splay_set_base_hook;
        typedef bi::bs_set_member_hook<> splay_set_member_hook;
#endif
        typedef cds_test::intrusive_set::base_int_item< splay_set_base_hook > base_item;
        typedef cds_test::intrusive_set::member_int_item< splay_set_member_hook > member_item;

        typedef bi::splay_set< base_item,
            bi::compare< cds_test::intrusive_set::less< base_item >>
        > base_hook_container;

        typedef bi::splay_set< member_item,
            bi::member_hook< member_item, splay_set_member_hook, &member_item::hMember>,
            bi::compare< cds_test::intrusive_set::less< member_item >>
        > member_hook_container;
    };

    INSTANTIATE_TYPED_TEST_CASE_P( BoostSplaySet, IntrusiveStripedSet, test_traits );

} // namespace

#endif // CDSUNIT_ENABLE_BOOST_CONTAINER
