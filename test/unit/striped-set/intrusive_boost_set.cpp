// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef CDSUNIT_ENABLE_BOOST_CONTAINER

//#include <boost/version.hpp>
//#if BOOST_VERSION >= 105900 && BOOST_VERSION < 106100
//#   define CDSTEST_REQUIRES_IMPLICIT_CONVERSION_WORKAROUND
//#endif

#include "test_intrusive_striped_set.h"
#include <cds/intrusive/striped_set/boost_set.h>

#if 0 //BOOST_VERSION >= 105900
#   define CDS_BOOST_INTRUSIVE_KEY_OF_VALUE_OPTION( type ) ,bi::key_of_value< get_key< type >>
#else
#   define CDS_BOOST_INTRUSIVE_KEY_OF_VALUE_OPTION( type )
#endif

namespace {
    namespace ci = cds::intrusive;
    namespace bi = boost::intrusive;

    template <typename Node>
    struct get_key
    {
        typedef int type;

        int operator()( Node const& v ) const
        {
            return v.key();
        }
    };

    struct test_traits
    {
        typedef cds_test::intrusive_set::base_int_item< bi::set_base_hook<> > base_item;
        typedef cds_test::intrusive_set::member_int_item< bi::set_member_hook<> > member_item;

        typedef bi::set< base_item,
            bi::compare< cds_test::intrusive_set::less< base_item >>
            CDS_BOOST_INTRUSIVE_KEY_OF_VALUE_OPTION( base_item )
        > base_hook_container;

        typedef bi::set< member_item,
            bi::member_hook< member_item, bi::set_member_hook<>, &member_item::hMember>,
            bi::compare< cds_test::intrusive_set::less< member_item >>
            CDS_BOOST_INTRUSIVE_KEY_OF_VALUE_OPTION( member_item )
        > member_hook_container;
    };

    INSTANTIATE_TYPED_TEST_CASE_P( BoostSet, IntrusiveStripedSet, test_traits );

} // namespace

#endif // CDSUNIT_ENABLE_BOOST_CONTAINER
