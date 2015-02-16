//$$CDS-header$$

#include "tree/hdr_bronson_avltree_map.h"
#include <cds/urcu/signal_buffered.h>
#include <cds/container/bronson_avltree_map_rcu.h>

#include "unit/print_bronsonavltree_stat.h"

namespace tree {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    namespace cc = cds::container;
    namespace co = cds::opt;
    namespace {
        typedef cds::urcu::gc< cds::urcu::signal_buffered<> > rcu_type;

        struct print_stat {
            template <typename Tree>
            void operator()( Tree const& t )
            {
                std::cout << t.statistics();
            }
        };
    } // namespace
#endif

    void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_shb_less()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits: public 
            cc::bronson_avltree::make_traits<
                co::less< std::less<key_type> >
                ,cc::bronson_avltree::relaxed_insert< false >
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, traits > map_type;
        test<map_type, print_stat>();
#endif
    }

    void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_shb_less_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits: public 
            cc::bronson_avltree::make_traits<
                co::less< std::less<key_type> >
                ,co::stat< cc::bronson_avltree::stat<> >
                ,cc::bronson_avltree::relaxed_insert< false >
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, traits > map_type;
        test<map_type, print_stat>();
#endif
    }

    void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_shb_cmp()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits: public 
            cc::bronson_avltree::make_traits<
                co::compare< compare >
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, traits > map_type;
        test<map_type, print_stat>();
#endif
    }

    void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_shb_cmp_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits: public 
            cc::bronson_avltree::make_traits<
                co::compare< compare >
                ,co::stat< cc::bronson_avltree::stat<> >
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, traits > map_type;
        test<map_type, print_stat>();
#endif
    }

    void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_shb_cmpless()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits: public 
            cc::bronson_avltree::make_traits<
                co::compare< compare >
                ,co::less< std::less<key_type> >
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, traits > map_type;
        test<map_type, print_stat>();
#endif
    }

    void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_shb_less_ic()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits: public 
            cc::bronson_avltree::make_traits<
                co::less< std::less<key_type> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, traits > map_type;
        test<map_type, print_stat>();
#endif
    }

    void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_shb_cmp_ic()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits: public 
            cc::bronson_avltree::make_traits<
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, traits > map_type;
        test<map_type, print_stat>();
#endif
    }

    void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_shb_cmp_ic_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits: public 
            cc::bronson_avltree::make_traits<
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::stat< cc::bronson_avltree::stat<> >
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, traits > map_type;
        test<map_type, print_stat>();
#endif
    }

    void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_shb_cmp_ic_stat_yield()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits: public 
            cc::bronson_avltree::make_traits<
                co::compare< compare >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::stat< cc::bronson_avltree::stat<> >
                ,co::back_off< cds::backoff::yield >
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, traits > map_type;
        test<map_type, print_stat>();
#endif
    }

    void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_shb_less_relaxed_insert()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits: public 
            cc::bronson_avltree::make_traits<
                co::less< std::less<key_type> >
                ,cc::bronson_avltree::relaxed_insert< true >
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, traits > map_type;
        test<map_type, print_stat>();
#endif
    }

    void BronsonAVLTreeHdrTest::BronsonAVLTree_rcu_shb_less_relaxed_insert_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits: public 
            cc::bronson_avltree::make_traits<
                co::less< std::less<key_type> >
                ,co::stat< cc::bronson_avltree::stat<> >
                ,cc::bronson_avltree::relaxed_insert< true >
            >::type
        {};
        typedef cc::BronsonAVLTreeMap< rcu_type, key_type, value_type, traits > map_type;
        test<map_type, print_stat>();
#endif
    }

} // namespace tree
