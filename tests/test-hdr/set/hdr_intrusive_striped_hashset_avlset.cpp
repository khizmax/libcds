/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

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

#include "set/hdr_intrusive_striped_set.h"
#include <cds/intrusive/striped_set/boost_avl_set.h>
#include <cds/intrusive/striped_set.h>

namespace set {
    namespace bi = boost::intrusive;

    namespace {
        typedef IntrusiveStripedSetHdrTest::base_item< bi::avl_set_base_hook<> > base_item_type;
        typedef IntrusiveStripedSetHdrTest::member_item< bi::avl_set_member_hook<> > member_item_type;

        template <typename T, typename Node>
        struct get_key
        {
            typedef T type;

            type const& operator()( Node const& v ) const
            {
                return v.nKey;
            }
        };
    }

#if BOOST_VERSION >= 105900
#   define CDS_BOOST_INTRUSIVE_KEY_OF_VALUE_OPTION( type ) ,bi::key_of_value< get_key< int, type>>
#else
#   define CDS_BOOST_INTRUSIVE_KEY_OF_VALUE_OPTION( type )
#endif

    void IntrusiveStripedSetHdrTest::Striped_avl_set_basehook()
    {
        typedef ci::StripedSet<
            bi::avl_set<base_item_type
                , bi::compare<IntrusiveStripedSetHdrTest::less<base_item_type>>
                CDS_BOOST_INTRUSIVE_KEY_OF_VALUE_OPTION( base_item_type )
            >
            ,co::mutex_policy< ci::striped_set::striping<> >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
        > set_type;

        test<set_type>();
    }

    void IntrusiveStripedSetHdrTest::Striped_avl_set_basehook_bucket_threshold()
    {
        typedef ci::StripedSet<
            bi::avl_set<base_item_type
                , bi::compare<IntrusiveStripedSetHdrTest::less<base_item_type>>
                CDS_BOOST_INTRUSIVE_KEY_OF_VALUE_OPTION( base_item_type )
            >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::resizing_policy< ci::striped_set::single_bucket_size_threshold<64> >
        > set_type;

        test<set_type>();
    }

    void IntrusiveStripedSetHdrTest::Striped_avl_set_basehook_bucket_threshold_rt()
    {
        typedef ci::StripedSet<
            bi::avl_set<base_item_type
                , bi::compare<IntrusiveStripedSetHdrTest::less<base_item_type> >
                CDS_BOOST_INTRUSIVE_KEY_OF_VALUE_OPTION( base_item_type )
            >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::resizing_policy< ci::striped_set::single_bucket_size_threshold<0> >
        > set_type;

        set_type s( 64, ci::striped_set::single_bucket_size_threshold<0>(256) );
        test_with( s );
    }

    void IntrusiveStripedSetHdrTest::Striped_avl_set_memberhook()
    {
        typedef ci::StripedSet<
            bi::avl_set<
                member_item_type
                , bi::member_hook< member_item_type, bi::avl_set_member_hook<>, &member_item_type::hMember>
                , bi::compare<IntrusiveStripedSetHdrTest::less<member_item_type> >
                CDS_BOOST_INTRUSIVE_KEY_OF_VALUE_OPTION( member_item_type )
            >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
        > set_type;

        test<set_type>();
    }

    void IntrusiveStripedSetHdrTest::Striped_avl_set_memberhook_bucket_threshold()
    {
        typedef ci::StripedSet<
            bi::avl_set<
                member_item_type
                , bi::member_hook< member_item_type, bi::avl_set_member_hook<>, &member_item_type::hMember>
                , bi::compare<IntrusiveStripedSetHdrTest::less<member_item_type> >
                CDS_BOOST_INTRUSIVE_KEY_OF_VALUE_OPTION( member_item_type )
            >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::resizing_policy< ci::striped_set::single_bucket_size_threshold<256> >
        > set_type;

        test<set_type>();
    }

    void IntrusiveStripedSetHdrTest::Striped_avl_set_memberhook_bucket_threshold_rt()
    {
        typedef ci::StripedSet<
            bi::avl_set<
                member_item_type
                , bi::member_hook< member_item_type, bi::avl_set_member_hook<>, &member_item_type::hMember>
                , bi::compare<IntrusiveStripedSetHdrTest::less<member_item_type> >
                CDS_BOOST_INTRUSIVE_KEY_OF_VALUE_OPTION( member_item_type )
            >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::resizing_policy< ci::striped_set::single_bucket_size_threshold<0> >
        > set_type;

        set_type s( 64, ci::striped_set::single_bucket_size_threshold<0>(256) );
        test_with( s );
    }

} // namespace set


