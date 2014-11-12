//$$CDS-header$$

#include "tree/hdr_intrusive_bintree.h"
#include <cds/urcu/general_threaded.h>
#include <cds/intrusive/ellen_bintree_rcu.h>

#include "tree/hdr_intrusive_ellen_bintree_pool_rcu.h"
#include "unit/print_ellenbintree_stat.h"

namespace tree {
    namespace ci = cds::intrusive;
    namespace co = cds::opt;
    namespace {
        typedef cds::urcu::gc< cds::urcu::general_threaded<> > rcu_type;

        typedef ci::ellen_bintree::node<rcu_type>                           leaf_node;
        typedef IntrusiveBinTreeHdrTest::base_hook_value< leaf_node >       base_value;

        struct print_stat {
            template <typename Tree>
            void operator()( Tree const& t)
            {
                std::cout << t.statistics();
            }
        };

        typedef ci::ellen_bintree::internal_node< IntrusiveBinTreeHdrTest::key_type, leaf_node > internal_node;
        typedef ci::ellen_bintree::update_desc< leaf_node, internal_node >   update_desc;
}

    void IntrusiveBinTreeHdrTest::EllenBinTree_rcu_gpt_base_less()
    {
        typedef ci::EllenBinTree< rcu_type, key_type, base_value,
            ci::ellen_bintree::make_traits<
                ci::opt::hook< ci::ellen_bintree::base_hook< co::gc< rcu_type > > >
                ,ci::ellen_bintree::key_extractor< key_extractor< base_value > >
                ,co::less< less< base_value > >
                ,ci::opt::disposer< disposer< base_value > >
            >::type
        > tree_type;

        test_rcu<tree_type, print_stat>();
    }

    void IntrusiveBinTreeHdrTest::EllenBinTree_rcu_gpt_base_less_stat()
    {
        typedef ci::EllenBinTree< rcu_type, key_type, base_value,
            ci::ellen_bintree::make_traits<
                ci::opt::hook< ci::ellen_bintree::base_hook< co::gc< rcu_type > > >
                ,ci::ellen_bintree::key_extractor< key_extractor< base_value > >
                ,co::less< less< base_value > >
                ,ci::opt::disposer< disposer< base_value > >
                ,co::stat< ci::ellen_bintree::stat<> >
            >::type
        > tree_type;

        test_rcu<tree_type, print_stat>();
    }

    void IntrusiveBinTreeHdrTest::EllenBinTree_rcu_gpt_base_cmp()
    {
        typedef ci::EllenBinTree< rcu_type, key_type, base_value,
            ci::ellen_bintree::make_traits<
                ci::opt::hook< ci::ellen_bintree::base_hook< co::gc< rcu_type > > >
                ,ci::ellen_bintree::key_extractor< key_extractor< base_value > >
                ,co::compare< compare< base_value > >
                ,ci::opt::disposer< disposer< base_value > >
            >::type
        > tree_type;

        test_rcu<tree_type, print_stat>();
    }

    void IntrusiveBinTreeHdrTest::EllenBinTree_rcu_gpt_base_cmpless()
    {
        typedef ci::EllenBinTree< rcu_type, key_type, base_value,
            ci::ellen_bintree::make_traits<
                ci::opt::hook< ci::ellen_bintree::base_hook< co::gc< rcu_type > > >
                ,ci::ellen_bintree::key_extractor< key_extractor< base_value > >
                ,co::compare< compare< base_value > >
                ,co::less< less< base_value > >
                ,ci::opt::disposer< disposer< base_value > >
            >::type
        > tree_type;

        test_rcu<tree_type, print_stat>();
    }

    void IntrusiveBinTreeHdrTest::EllenBinTree_rcu_gpt_base_less_ic()
    {
        typedef ci::EllenBinTree< rcu_type, key_type, base_value,
            ci::ellen_bintree::make_traits<
                ci::opt::hook< ci::ellen_bintree::base_hook< co::gc< rcu_type > > >
                ,ci::ellen_bintree::key_extractor< key_extractor< base_value > >
                ,co::less< less< base_value > >
                ,ci::opt::disposer< disposer< base_value > >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > tree_type;

        test_rcu<tree_type, print_stat>();
    }

    void IntrusiveBinTreeHdrTest::EllenBinTree_rcu_gpt_base_cmp_ic()
    {
        typedef ci::EllenBinTree< rcu_type, key_type, base_value,
            ci::ellen_bintree::make_traits<
                ci::opt::hook< ci::ellen_bintree::base_hook< co::gc< rcu_type > > >
                ,ci::ellen_bintree::key_extractor< key_extractor< base_value > >
                ,co::compare< compare< base_value > >
                ,ci::opt::disposer< disposer< base_value > >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > tree_type;

        test_rcu<tree_type, print_stat>();
    }

    void IntrusiveBinTreeHdrTest::EllenBinTree_rcu_gpt_base_cmp_ic_stat()
    {
        typedef ci::EllenBinTree< rcu_type, key_type, base_value,
            ci::ellen_bintree::make_traits<
                ci::opt::hook< ci::ellen_bintree::base_hook< co::gc< rcu_type > > >
                ,ci::ellen_bintree::key_extractor< key_extractor< base_value > >
                ,co::compare< compare< base_value > >
                ,ci::opt::disposer< disposer< base_value > >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::stat< ci::ellen_bintree::stat<> >
            >::type
        > tree_type;

        test_rcu<tree_type, print_stat>();
    }

    void IntrusiveBinTreeHdrTest::EllenBinTree_rcu_gpt_base_cmp_ic_stat_yield()
    {
        typedef ci::EllenBinTree< rcu_type, key_type, base_value,
            ci::ellen_bintree::make_traits<
                ci::opt::hook< ci::ellen_bintree::base_hook< co::gc< rcu_type > > >
                ,ci::ellen_bintree::key_extractor< key_extractor< base_value > >
                ,co::compare< compare< base_value > >
                ,ci::opt::disposer< disposer< base_value > >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::stat< ci::ellen_bintree::stat<> >
                , co::back_off< cds::backoff::yield >
            >::type
        > tree_type;

        test_rcu<tree_type, print_stat>();
    }

    void IntrusiveBinTreeHdrTest::EllenBinTree_rcu_gpt_base_less_pool()
    {
        typedef ci::EllenBinTree< rcu_type, key_type, base_value,
            ci::ellen_bintree::make_traits<
                ci::opt::hook< ci::ellen_bintree::base_hook< co::gc< rcu_type > > >
                ,ci::ellen_bintree::key_extractor< key_extractor< base_value > >
                ,co::less< less< base_value > >
                ,ci::opt::disposer< disposer< base_value > >
                ,co::node_allocator< cds::memory::pool_allocator< internal_node, ellen_bintree_rcu::internal_node_pool_accessor > >
                ,ci::ellen_bintree::update_desc_allocator< cds::memory::pool_allocator< update_desc, ellen_bintree_rcu::update_desc_pool_accessor > >
            >::type
        > tree_type;

        test_rcu<tree_type, print_stat>();
    }

    void IntrusiveBinTreeHdrTest::EllenBinTree_rcu_gpt_base_less_pool_ic_stat()
    {
        typedef ci::EllenBinTree< rcu_type, key_type, base_value,
            ci::ellen_bintree::make_traits<
                ci::opt::hook< ci::ellen_bintree::base_hook< co::gc< rcu_type > > >
                ,ci::ellen_bintree::key_extractor< key_extractor< base_value > >
                ,co::less< less< base_value > >
                ,ci::opt::disposer< disposer< base_value > >
                ,co::node_allocator< cds::memory::pool_allocator< internal_node, ellen_bintree_rcu::internal_node_pool_accessor > >
                ,ci::ellen_bintree::update_desc_allocator< cds::memory::pool_allocator< update_desc, ellen_bintree_rcu::update_desc_pool_accessor > >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::stat< ci::ellen_bintree::stat<> >
            >::type
        > tree_type;

        test_rcu<tree_type, print_stat>();
    }


} //namespace tree
