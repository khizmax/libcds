/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#include "set/hdr_intrusive_striped_set.h"
#include <cds/intrusive/striped_set/boost_splay_set.h>
#include <cds/intrusive/striped_set.h>
#include <boost/version.hpp>

namespace set {
    namespace bi = boost::intrusive;

    namespace {
#   if BOOST_VERSION < 105600
        typedef bi::splay_set_member_hook<> splay_set_member_hook;
        typedef bi::splay_set_base_hook<> splay_set_base_hook;
#else
        typedef bi::bs_set_base_hook<> splay_set_base_hook;
        typedef bi::bs_set_member_hook<> splay_set_member_hook;
#endif
        typedef IntrusiveStripedSetHdrTest::base_item< splay_set_base_hook > base_item_type;
        typedef IntrusiveStripedSetHdrTest::member_item< splay_set_member_hook > member_item_type;
    }

    void IntrusiveStripedSetHdrTest::Refinable_splay_set_basehook()
    {
        typedef ci::StripedSet<
            bi::splay_set<base_item_type, bi::compare<IntrusiveStripedSetHdrTest::less<base_item_type> > >
            ,co::mutex_policy< ci::striped_set::refinable<> >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::back_off< cds::backoff::empty >
        > set_type;

        test<set_type>();
    }

    void IntrusiveStripedSetHdrTest::Refinable_splay_set_basehook_bucket_threshold()
    {
        typedef ci::StripedSet<
            bi::splay_set<base_item_type, bi::compare<IntrusiveStripedSetHdrTest::less<base_item_type> > >
            ,co::mutex_policy< ci::striped_set::refinable<> >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::resizing_policy< ci::striped_set::single_bucket_size_threshold<64> >
        > set_type;

        test<set_type>();
    }

    void IntrusiveStripedSetHdrTest::Refinable_splay_set_basehook_bucket_threshold_rt()
    {
        typedef ci::StripedSet<
            bi::splay_set<base_item_type, bi::compare<IntrusiveStripedSetHdrTest::less<base_item_type> > >
            ,co::mutex_policy< ci::striped_set::refinable<> >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::resizing_policy< ci::striped_set::single_bucket_size_threshold<0> >
        > set_type;

        set_type s( 64, ci::striped_set::single_bucket_size_threshold<0>(256) );
        test_with( s );
    }

    void IntrusiveStripedSetHdrTest::Refinable_splay_set_memberhook()
    {
        typedef ci::StripedSet<
            bi::splay_set<
                member_item_type
                , bi::member_hook< member_item_type, splay_set_member_hook, &member_item_type::hMember>
                , bi::compare<IntrusiveStripedSetHdrTest::less<member_item_type> >
            >
            ,co::mutex_policy< ci::striped_set::refinable<> >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
        > set_type;

        test<set_type>();
    }

    void IntrusiveStripedSetHdrTest::Refinable_splay_set_memberhook_bucket_threshold()
    {
        typedef ci::StripedSet<
            bi::splay_set<
                member_item_type
                , bi::member_hook< member_item_type, splay_set_member_hook, &member_item_type::hMember>
                , bi::compare<IntrusiveStripedSetHdrTest::less<member_item_type> >
            >
            ,co::mutex_policy< ci::striped_set::refinable<> >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::resizing_policy< ci::striped_set::single_bucket_size_threshold<256> >
        > set_type;

        test<set_type>();
    }

    void IntrusiveStripedSetHdrTest::Refinable_splay_set_memberhook_bucket_threshold_rt()
    {
        typedef ci::StripedSet<
            bi::splay_set<
                member_item_type
                , bi::member_hook< member_item_type, splay_set_member_hook, &member_item_type::hMember>
                , bi::compare<IntrusiveStripedSetHdrTest::less<member_item_type> >
            >
            ,co::mutex_policy< ci::striped_set::refinable<> >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::resizing_policy< ci::striped_set::single_bucket_size_threshold<0> >
        > set_type;

        set_type s( 64, ci::striped_set::single_bucket_size_threshold<0>(256) );
        test_with( s );
    }
} // namespace set


