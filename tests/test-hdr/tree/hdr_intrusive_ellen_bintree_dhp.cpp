//$$CDS-header$$

#include "tree/hdr_intrusive_bintree.h"
#include <cds/intrusive/ellen_bintree_dhp.h>

#include "tree/hdr_intrusive_ellen_bintree_pool_dhp.h"
#include "unit/print_ellenbintree_stat.h"

namespace tree {
    namespace ci = cds::intrusive;
    namespace co = cds::opt;
    namespace {
        typedef ci::ellen_bintree::node<cds::gc::DHP>                       leaf_node;
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

    void IntrusiveBinTreeHdrTest::EllenBinTree_dhp_base_less()
    {
        struct tree_traits : public ci::ellen_bintree::traits
        {
            typedef ci::ellen_bintree::base_hook< co::gc< cds::gc::DHP >> hook;
            typedef IntrusiveBinTreeHdrTest::key_extractor< base_value > key_extractor;
            typedef IntrusiveBinTreeHdrTest::less< base_value > less;
            typedef IntrusiveBinTreeHdrTest::disposer< base_value > disposer;
        };
        typedef ci::EllenBinTree< cds::gc::DHP, key_type, base_value, tree_traits > tree_type;

        test<tree_type, print_stat>();
    }

    void IntrusiveBinTreeHdrTest::EllenBinTree_dhp_base_less_stat()
    {
        typedef ci::EllenBinTree< cds::gc::DHP, key_type, base_value,
            ci::ellen_bintree::make_traits<
                ci::opt::hook< ci::ellen_bintree::base_hook< co::gc< cds::gc::DHP > > >
                ,ci::ellen_bintree::key_extractor< key_extractor< base_value > >
                ,co::less< less< base_value > >
                ,ci::opt::disposer< disposer< base_value > >
                ,co::stat< ci::ellen_bintree::stat<> >
            >::type
        > tree_type;

        test<tree_type, print_stat>();
    }

    void IntrusiveBinTreeHdrTest::EllenBinTree_dhp_base_cmp()
    {
        typedef ci::EllenBinTree< cds::gc::DHP, key_type, base_value,
            ci::ellen_bintree::make_traits<
                ci::opt::hook< ci::ellen_bintree::base_hook< co::gc< cds::gc::DHP > > >
                ,ci::ellen_bintree::key_extractor< key_extractor< base_value > >
                ,co::compare< compare< base_value > >
                ,ci::opt::disposer< disposer< base_value > >
            >::type
        > tree_type;

        test<tree_type, print_stat>();
    }

    void IntrusiveBinTreeHdrTest::EllenBinTree_dhp_base_cmpless()
    {
        typedef ci::EllenBinTree< cds::gc::DHP, key_type, base_value,
            ci::ellen_bintree::make_traits<
                ci::opt::hook< ci::ellen_bintree::base_hook< co::gc< cds::gc::DHP > > >
                ,ci::ellen_bintree::key_extractor< key_extractor< base_value > >
                ,co::compare< compare< base_value > >
                ,co::less< less< base_value > >
                ,ci::opt::disposer< disposer< base_value > >
            >::type
        > tree_type;

        test<tree_type, print_stat>();
    }

    void IntrusiveBinTreeHdrTest::EllenBinTree_dhp_base_less_ic()
    {
        typedef ci::EllenBinTree< cds::gc::DHP, key_type, base_value,
            ci::ellen_bintree::make_traits<
                ci::opt::hook< ci::ellen_bintree::base_hook< co::gc< cds::gc::DHP > > >
                ,ci::ellen_bintree::key_extractor< key_extractor< base_value > >
                ,co::less< less< base_value > >
                ,ci::opt::disposer< disposer< base_value > >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > tree_type;

        test<tree_type, print_stat>();
    }

    void IntrusiveBinTreeHdrTest::EllenBinTree_dhp_base_cmp_ic()
    {
        typedef ci::EllenBinTree< cds::gc::DHP, key_type, base_value,
            ci::ellen_bintree::make_traits<
                ci::opt::hook< ci::ellen_bintree::base_hook< co::gc< cds::gc::DHP > > >
                ,ci::ellen_bintree::key_extractor< key_extractor< base_value > >
                ,co::compare< compare< base_value > >
                ,ci::opt::disposer< disposer< base_value > >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > tree_type;

        test<tree_type, print_stat>();
    }

    void IntrusiveBinTreeHdrTest::EllenBinTree_dhp_base_cmp_ic_stat()
    {
        typedef ci::EllenBinTree< cds::gc::DHP, key_type, base_value,
            ci::ellen_bintree::make_traits<
                ci::opt::hook< ci::ellen_bintree::base_hook< co::gc< cds::gc::DHP > > >
                ,ci::ellen_bintree::key_extractor< key_extractor< base_value > >
                ,co::compare< compare< base_value > >
                ,ci::opt::disposer< disposer< base_value > >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::stat< ci::ellen_bintree::stat<> >
            >::type
        > tree_type;

        test<tree_type, print_stat>();
    }

    void IntrusiveBinTreeHdrTest::EllenBinTree_dhp_base_cmp_ic_stat_yield()
    {
        typedef ci::EllenBinTree< cds::gc::DHP, key_type, base_value,
            ci::ellen_bintree::make_traits<
                ci::opt::hook< ci::ellen_bintree::base_hook< co::gc< cds::gc::DHP > > >
                , ci::ellen_bintree::key_extractor< key_extractor< base_value > >
                , co::compare< compare< base_value > >
                , ci::opt::disposer< disposer< base_value > >
                , co::item_counter< cds::atomicity::item_counter >
                , co::stat< ci::ellen_bintree::stat<> >
                , co::back_off< cds::backoff::yield >
            >::type
        > tree_type;

        test<tree_type, print_stat>();
    }

    void IntrusiveBinTreeHdrTest::EllenBinTree_dhp_base_less_pool()
    {
        typedef ci::EllenBinTree< cds::gc::DHP, key_type, base_value,
            ci::ellen_bintree::make_traits<
                ci::opt::hook< ci::ellen_bintree::base_hook< co::gc< cds::gc::DHP > > >
                ,ci::ellen_bintree::key_extractor< key_extractor< base_value > >
                ,co::less< less< base_value > >
                ,ci::opt::disposer< disposer< base_value > >
                ,co::node_allocator< cds::memory::pool_allocator< internal_node, ellen_bintree_dhp::internal_node_pool_accessor > >
                ,ci::ellen_bintree::update_desc_allocator< cds::memory::pool_allocator< update_desc, ellen_bintree_dhp::update_desc_pool_accessor > >
            >::type
        > tree_type;

        test<tree_type, print_stat>();
    }

    void IntrusiveBinTreeHdrTest::EllenBinTree_dhp_base_less_pool_ic_stat()
    {
        typedef ci::EllenBinTree< cds::gc::DHP, key_type, base_value,
            ci::ellen_bintree::make_traits<
                ci::opt::hook< ci::ellen_bintree::base_hook< co::gc< cds::gc::DHP > > >
                ,ci::ellen_bintree::key_extractor< key_extractor< base_value > >
                ,co::less< less< base_value > >
                ,ci::opt::disposer< disposer< base_value > >
                ,co::node_allocator< cds::memory::pool_allocator< internal_node, ellen_bintree_dhp::internal_node_pool_accessor > >
                ,ci::ellen_bintree::update_desc_allocator< cds::memory::pool_allocator< update_desc, ellen_bintree_dhp::update_desc_pool_accessor > >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::stat< ci::ellen_bintree::stat<> >
            >::type
        > tree_type;

        test<tree_type, print_stat>();
    }


} //namespace tree
