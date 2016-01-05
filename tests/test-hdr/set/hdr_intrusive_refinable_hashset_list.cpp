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
#include <cds/intrusive/striped_set/boost_list.h>
#include <cds/intrusive/striped_set.h>

#include <type_traits> // std::is_same

namespace set {
    namespace bi = boost::intrusive;

    namespace {
        typedef IntrusiveStripedSetHdrTest::base_item< bi::list_base_hook<> > base_item_type;
        typedef IntrusiveStripedSetHdrTest::member_item< bi::list_member_hook<> > member_item_type;
    }

    void IntrusiveStripedSetHdrTest::Refinable_list_basehook_cmp()
    {
        typedef ci::StripedSet<
            bi::list<base_item_type>
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::compare< IntrusiveStripedSetHdrTest::cmp<base_item_type> >
            ,co::mutex_policy< ci::striped_set::refinable<> >
        > set_type;

        static_assert( (std::is_same<
            IntrusiveStripedSetHdrTest::cmp<base_item_type>
            ,set_type::bucket_type::key_comparator
        >::value), "Key compare function selection error" );

        test<set_type>();
    }

    void IntrusiveStripedSetHdrTest::Refinable_list_basehook_less()
    {
        typedef ci::StripedSet<
            bi::list<base_item_type>
            ,co::mutex_policy< ci::striped_set::refinable<> >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::less< IntrusiveStripedSetHdrTest::less<base_item_type> >
        > set_type;

        test<set_type>();
    }

    void IntrusiveStripedSetHdrTest::Refinable_list_basehook_cmpmix()
    {
        typedef ci::StripedSet<
            bi::list<base_item_type>
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::mutex_policy< ci::striped_set::refinable<> >
            ,co::less< IntrusiveStripedSetHdrTest::less<base_item_type> >
            ,co::compare< IntrusiveStripedSetHdrTest::cmp<base_item_type> >
        > set_type;

        static_assert( (std::is_same<
            IntrusiveStripedSetHdrTest::cmp<base_item_type>
            ,set_type::bucket_type::key_comparator
        >::value), "Key compare function selection error" );

        test<set_type>();
    }

    void IntrusiveStripedSetHdrTest::Refinable_list_basehook_bucket_threshold()
    {
        typedef ci::StripedSet<
            bi::list<base_item_type>
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::less< IntrusiveStripedSetHdrTest::less<base_item_type> >
            ,co::compare< IntrusiveStripedSetHdrTest::cmp<base_item_type> >
            ,co::mutex_policy< ci::striped_set::refinable<> >
            ,co::resizing_policy< ci::striped_set::single_bucket_size_threshold<8> >
        > set_type;

        static_assert( (std::is_same<
            IntrusiveStripedSetHdrTest::cmp<base_item_type>
            ,set_type::bucket_type::key_comparator
        >::value), "Key compare function selection error" );

        test<set_type>();
    }

    void IntrusiveStripedSetHdrTest::Refinable_list_basehook_bucket_threshold_rt()
    {
        typedef ci::StripedSet<
            bi::list<base_item_type>
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::less< IntrusiveStripedSetHdrTest::less<base_item_type> >
            ,co::compare< IntrusiveStripedSetHdrTest::cmp<base_item_type> >
            ,co::resizing_policy< ci::striped_set::single_bucket_size_threshold<0> >
            ,co::mutex_policy< ci::striped_set::refinable<> >
        > set_type;

        static_assert( (std::is_same<
            IntrusiveStripedSetHdrTest::cmp<base_item_type>
            ,set_type::bucket_type::key_comparator
        >::value), "Key compare function selection error" );

        set_type s( 128, ci::striped_set::single_bucket_size_threshold<0>(4) );
        test_with( s );
    }

    void IntrusiveStripedSetHdrTest::Refinable_list_memberhook_cmp()
    {
        typedef ci::StripedSet<
            bi::list<
                member_item_type
                , bi::member_hook< member_item_type, bi::list_member_hook<>, &member_item_type::hMember>
            >
            ,co::mutex_policy< ci::striped_set::refinable<> >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::compare< IntrusiveStripedSetHdrTest::cmp<member_item_type> >
        > set_type;

        test<set_type>();
    }

    void IntrusiveStripedSetHdrTest::Refinable_list_memberhook_less()
    {
        typedef ci::StripedSet<
            bi::list<
                member_item_type
                , bi::member_hook< member_item_type, bi::list_member_hook<>, &member_item_type::hMember>
            >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::less< IntrusiveStripedSetHdrTest::less<member_item_type> >
            ,co::mutex_policy< ci::striped_set::refinable<> >
        > set_type;

        test<set_type>();
    }

    void IntrusiveStripedSetHdrTest::Refinable_list_memberhook_cmpmix()
    {
        typedef ci::StripedSet<
            bi::list<
                member_item_type
                , bi::member_hook< member_item_type, bi::list_member_hook<>, &member_item_type::hMember>
            >
            ,co::mutex_policy< ci::striped_set::refinable<> >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::less< IntrusiveStripedSetHdrTest::less<member_item_type> >
            ,co::compare< IntrusiveStripedSetHdrTest::cmp<member_item_type> >
        > set_type;

        test<set_type>();
    }

    void IntrusiveStripedSetHdrTest::Refinable_list_memberhook_bucket_threshold()
    {
        typedef ci::StripedSet<
            bi::list<
                member_item_type
                , bi::member_hook< member_item_type, bi::list_member_hook<>, &member_item_type::hMember>
            >
            ,co::mutex_policy< ci::striped_set::refinable<> >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::less< IntrusiveStripedSetHdrTest::less<member_item_type> >
            ,co::compare< IntrusiveStripedSetHdrTest::cmp<member_item_type> >
            ,co::resizing_policy< ci::striped_set::single_bucket_size_threshold<8> >
        > set_type;

        static_assert( (std::is_same<
            IntrusiveStripedSetHdrTest::cmp<member_item_type>
            ,set_type::bucket_type::key_comparator
        >::value), "Key compare function selection error" );

        test<set_type>();
    }

    void IntrusiveStripedSetHdrTest::Refinable_list_memberhook_bucket_threshold_rt()
    {
        typedef ci::StripedSet<
            bi::list<
            member_item_type
            , bi::member_hook< member_item_type, bi::list_member_hook<>, &member_item_type::hMember>
            >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::less< IntrusiveStripedSetHdrTest::less<member_item_type> >
            ,co::compare< IntrusiveStripedSetHdrTest::cmp<member_item_type> >
            ,co::resizing_policy< ci::striped_set::single_bucket_size_threshold<0> >
            ,co::mutex_policy< ci::striped_set::refinable<> >
        > set_type;

        static_assert( (std::is_same<
            IntrusiveStripedSetHdrTest::cmp<member_item_type>
            ,set_type::bucket_type::key_comparator
        >::value), "Key compare function selection error" );

        set_type s( 128, ci::striped_set::single_bucket_size_threshold<0>(4) );
        test_with( s );
    }

} // namespace set


