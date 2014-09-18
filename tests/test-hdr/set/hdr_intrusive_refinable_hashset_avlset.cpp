//$$CDS-header$$

#include "set/hdr_intrusive_striped_set.h"
#include <cds/intrusive/striped_set/boost_avl_set.h>
#include <cds/intrusive/striped_set.h>

namespace set {
    namespace bi = boost::intrusive;

    namespace {
        typedef IntrusiveStripedSetHdrTest::base_item< bi::avl_set_base_hook<> > base_item_type;
        typedef IntrusiveStripedSetHdrTest::member_item< bi::avl_set_member_hook<> > member_item_type;
    }

    void IntrusiveStripedSetHdrTest::Refinable_avl_set_basehook()
    {
        typedef ci::StripedSet<
            bi::avl_set<base_item_type, bi::compare<IntrusiveStripedSetHdrTest::less<base_item_type> > >
            ,co::mutex_policy< ci::striped_set::refinable<> >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::back_off< cds::backoff::empty >
        > set_type;

        test<set_type>();
    }

    void IntrusiveStripedSetHdrTest::Refinable_avl_set_basehook_bucket_threshold()
    {
        typedef ci::StripedSet<
            bi::avl_set<base_item_type, bi::compare<IntrusiveStripedSetHdrTest::less<base_item_type> > >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::resizing_policy< ci::striped_set::single_bucket_size_threshold<64> >
            ,co::mutex_policy< ci::striped_set::refinable<> >
        > set_type;

        test<set_type>();
    }

    void IntrusiveStripedSetHdrTest::Refinable_avl_set_basehook_bucket_threshold_rt()
    {
        typedef ci::StripedSet<
            bi::avl_set<base_item_type, bi::compare<IntrusiveStripedSetHdrTest::less<base_item_type> > >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::resizing_policy< ci::striped_set::single_bucket_size_threshold<0> >
            ,co::mutex_policy< ci::striped_set::refinable<> >
        > set_type;

        set_type s( 64, ci::striped_set::single_bucket_size_threshold<0>(256) );
        test_with( s );
    }

    void IntrusiveStripedSetHdrTest::Refinable_avl_set_memberhook()
    {
        typedef ci::StripedSet<
            bi::avl_set<
                member_item_type
                , bi::member_hook< member_item_type, bi::avl_set_member_hook<>, &member_item_type::hMember>
                , bi::compare<IntrusiveStripedSetHdrTest::less<member_item_type> >
            >
            ,co::mutex_policy< ci::striped_set::refinable<> >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
        > set_type;

        test<set_type>();
    }

    void IntrusiveStripedSetHdrTest::Refinable_avl_set_memberhook_bucket_threshold()
    {
        typedef ci::StripedSet<
            bi::avl_set<
                member_item_type
                , bi::member_hook< member_item_type, bi::avl_set_member_hook<>, &member_item_type::hMember>
                , bi::compare<IntrusiveStripedSetHdrTest::less<member_item_type> >
            >
            ,co::mutex_policy< ci::striped_set::refinable<> >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::resizing_policy< ci::striped_set::single_bucket_size_threshold<256> >
        > set_type;

        test<set_type>();
    }

    void IntrusiveStripedSetHdrTest::Refinable_avl_set_memberhook_bucket_threshold_rt()
    {
        typedef ci::StripedSet<
            bi::avl_set<
                member_item_type
                , bi::member_hook< member_item_type, bi::avl_set_member_hook<>, &member_item_type::hMember>
                , bi::compare<IntrusiveStripedSetHdrTest::less<member_item_type> >
            >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::mutex_policy< ci::striped_set::refinable<> >
            ,co::resizing_policy< ci::striped_set::single_bucket_size_threshold<0> >
        > set_type;

        set_type s( 64, ci::striped_set::single_bucket_size_threshold<0>(256) );
        test_with( s );
    }
} // namespace set


