/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
