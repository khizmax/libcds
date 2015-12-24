//$$CDS-header$$

#include "set/hdr_intrusive_striped_set.h"
#include <cds/intrusive/striped_set/boost_set.h>
#include <cds/intrusive/striped_set.h>

namespace set {
    namespace bi = boost::intrusive;

    namespace {
        typedef IntrusiveStripedSetHdrTest::base_item< bi::set_base_hook<> > base_item_type;
        typedef IntrusiveStripedSetHdrTest::member_item< bi::set_member_hook<> > member_item_type;

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

    void IntrusiveStripedSetHdrTest::Refinable_set_basehook()
    {
        typedef ci::StripedSet<
            bi::set<base_item_type
                , bi::compare<IntrusiveStripedSetHdrTest::less<base_item_type>>
                CDS_BOOST_INTRUSIVE_KEY_OF_VALUE_OPTION( base_item_type )
            >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::mutex_policy< ci::striped_set::refinable<> >
        > set_type;

        test<set_type>();
    }

    void IntrusiveStripedSetHdrTest::Refinable_set_basehook_bucket_threshold()
    {
        typedef ci::StripedSet<
            bi::set<base_item_type
                , bi::compare<IntrusiveStripedSetHdrTest::less<base_item_type>>
                CDS_BOOST_INTRUSIVE_KEY_OF_VALUE_OPTION( base_item_type )
            >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::resizing_policy< ci::striped_set::single_bucket_size_threshold<64> >
            ,co::mutex_policy< ci::striped_set::refinable<> >
        > set_type;

        test<set_type>();
    }

    void IntrusiveStripedSetHdrTest::Refinable_set_basehook_bucket_threshold_rt()
    {
        typedef ci::StripedSet<
            bi::set<base_item_type
                , bi::compare<IntrusiveStripedSetHdrTest::less<base_item_type>>
                CDS_BOOST_INTRUSIVE_KEY_OF_VALUE_OPTION( base_item_type )
            >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::resizing_policy< ci::striped_set::single_bucket_size_threshold<0> >
            ,co::mutex_policy< ci::striped_set::refinable<> >
        > set_type;

        set_type s( 64, ci::striped_set::single_bucket_size_threshold<0>(256) );
        test_with( s );
    }

    void IntrusiveStripedSetHdrTest::Refinable_set_memberhook()
    {
        typedef ci::StripedSet<
            bi::set<
                member_item_type
                , bi::member_hook< member_item_type, bi::set_member_hook<>, &member_item_type::hMember>
                , bi::compare<IntrusiveStripedSetHdrTest::less<member_item_type> >
                CDS_BOOST_INTRUSIVE_KEY_OF_VALUE_OPTION( member_item_type )
            >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::mutex_policy< ci::striped_set::refinable<> >
        > set_type;

        test<set_type>();
    }

    void IntrusiveStripedSetHdrTest::Refinable_set_memberhook_bucket_threshold()
    {
        typedef ci::StripedSet<
            bi::set<
                member_item_type
                , bi::member_hook< member_item_type, bi::set_member_hook<>, &member_item_type::hMember>
                , bi::compare<IntrusiveStripedSetHdrTest::less<member_item_type>>
                CDS_BOOST_INTRUSIVE_KEY_OF_VALUE_OPTION( member_item_type )
            >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::resizing_policy< ci::striped_set::single_bucket_size_threshold<256> >
            ,co::mutex_policy< ci::striped_set::refinable<> >
        > set_type;

        test<set_type>();
    }

    void IntrusiveStripedSetHdrTest::Refinable_set_memberhook_bucket_threshold_rt()
    {
        typedef ci::StripedSet<
            bi::set<
                member_item_type
                , bi::member_hook< member_item_type, bi::set_member_hook<>, &member_item_type::hMember>
                , bi::compare<IntrusiveStripedSetHdrTest::less<member_item_type>>
                CDS_BOOST_INTRUSIVE_KEY_OF_VALUE_OPTION( member_item_type )
            >
            ,co::hash< IntrusiveStripedSetHdrTest::hash_int >
            ,co::resizing_policy< ci::striped_set::single_bucket_size_threshold<0> >
            ,co::mutex_policy< ci::striped_set::refinable<> >
        > set_type;

        set_type s( 64, ci::striped_set::single_bucket_size_threshold<0>(256) );
        test_with( s );
    }

} // namespace set


