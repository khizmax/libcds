//$$CDS-header$$

#include "tree/hdr_ellenbintree_map.h"
#include <cds/urcu/signal_threaded.h>
#include <cds/container/ellen_bintree_map_rcu.h>

#include "tree/hdr_intrusive_ellen_bintree_pool_rcu.h"
#include "unit/print_ellenbintree_stat.h"

namespace tree {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    namespace cc = cds::container;
    namespace co = cds::opt;
    namespace {
        typedef cds::urcu::gc< cds::urcu::signal_threaded<> > rcu_type;

        typedef cc::ellen_bintree::map_node<rcu_type, EllenBinTreeMapHdrTest::key_type, EllenBinTreeMapHdrTest::value_type> tree_leaf_node;
        typedef cc::ellen_bintree::internal_node< EllenBinTreeMapHdrTest::key_type, tree_leaf_node >                        tree_internal_node;
        typedef cc::ellen_bintree::update_desc<tree_leaf_node, tree_internal_node>                                          tree_update_desc;

        struct print_stat {
            template <typename Tree>
            void operator()( Tree const& t)
            {
                std::cout << t.statistics();
            }
        };
    }
#endif

    void EllenBinTreeMapHdrTest::EllenBinTree_rcu_sht_less()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::EllenBinTreeMap< rcu_type, key_type, value_type,
            cc::ellen_bintree::make_map_traits<
                co::less< less >
            >::type
        > set_type;

        test_rcu<set_type, print_stat>();
#endif
    }

    void EllenBinTreeMapHdrTest::EllenBinTree_rcu_sht_cmp()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::EllenBinTreeMap< rcu_type, key_type, value_type,
            cc::ellen_bintree::make_map_traits<
                co::compare< compare >
            >::type
        > set_type;

        test_rcu<set_type, print_stat>();
#endif
    }

    void EllenBinTreeMapHdrTest::EllenBinTree_rcu_sht_cmpless()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::EllenBinTreeMap< rcu_type, key_type, value_type,
            cc::ellen_bintree::make_map_traits<
                co::compare< compare >
                ,co::less< less >
            >::type
        > set_type;

        test_rcu<set_type, print_stat>();
#endif
    }

    void EllenBinTreeMapHdrTest::EllenBinTree_rcu_sht_less_ic()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::EllenBinTreeMap< rcu_type, key_type, value_type,
            cc::ellen_bintree::make_map_traits<
                co::less< less >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > set_type;

        test_rcu<set_type, print_stat>();
#endif
    }

    void EllenBinTreeMapHdrTest::EllenBinTree_rcu_sht_cmp_ic()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::EllenBinTreeMap< rcu_type, key_type, value_type,
            cc::ellen_bintree::make_map_traits<
                co::item_counter< cds::atomicity::item_counter >
                ,co::compare< compare >
            >::type
        > set_type;

        test_rcu<set_type, print_stat>();
#endif
    }

    void EllenBinTreeMapHdrTest::EllenBinTree_rcu_sht_less_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::EllenBinTreeMap< rcu_type, key_type, value_type,
            cc::ellen_bintree::make_map_traits<
                co::less< less >
                ,co::stat< cc::ellen_bintree::stat<> >
            >::type
        > set_type;

        test_rcu<set_type, print_stat>();
#endif
    }

    void EllenBinTreeMapHdrTest::EllenBinTree_rcu_sht_cmp_ic_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::EllenBinTreeMap< rcu_type, key_type, value_type,
            cc::ellen_bintree::make_map_traits<
                co::item_counter< cds::atomicity::item_counter >
                ,co::stat< cc::ellen_bintree::stat<> >
                ,co::compare< compare >
            >::type
        > set_type;

        test_rcu<set_type, print_stat>();
#endif
    }

    void EllenBinTreeMapHdrTest::EllenBinTree_rcu_sht_cmp_ic_stat_yield()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::EllenBinTreeMap< rcu_type, key_type, value_type,
            cc::ellen_bintree::make_map_traits<
                co::item_counter< cds::atomicity::item_counter >
                ,co::stat< cc::ellen_bintree::stat<> >
                ,co::compare< compare >
                , co::back_off< cds::backoff::yield >
            >::type
        > set_type;

        test_rcu<set_type, print_stat>();
#endif
    }

    void EllenBinTreeMapHdrTest::EllenBinTree_rcu_sht_less_pool()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::EllenBinTreeMap< rcu_type, key_type, value_type,
            cc::ellen_bintree::make_map_traits<
                co::less< less >
                ,co::node_allocator< cds::memory::pool_allocator< tree_internal_node, ellen_bintree_rcu::internal_node_pool_accessor > >
                ,cc::ellen_bintree::update_desc_allocator< cds::memory::pool_allocator< tree_update_desc, ellen_bintree_rcu::update_desc_pool_accessor > >
            >::type
        > set_type;

        test_rcu<set_type, print_stat>();
#endif
    }

    void EllenBinTreeMapHdrTest::EllenBinTree_rcu_sht_less_pool_ic_stat()
    {
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::EllenBinTreeMap< rcu_type, key_type, value_type,
            cc::ellen_bintree::make_map_traits<
                co::less< less >
                ,co::node_allocator< cds::memory::pool_allocator< tree_internal_node, ellen_bintree_rcu::internal_node_pool_accessor > >
                ,cc::ellen_bintree::update_desc_allocator< cds::memory::pool_allocator< tree_update_desc, ellen_bintree_rcu::update_desc_pool_accessor > >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::stat< cc::ellen_bintree::stat<> >
            >::type
        > set_type;

        test_rcu<set_type, print_stat>();
#endif
    }

} // namespace tree
