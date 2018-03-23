// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_intrusive_tree_hp.h"

#include <cds/intrusive/ellen_bintree_hp.h>
#include "test_ellen_bintree_update_desc_pool.h"

namespace {
    namespace ci = cds::intrusive;
    typedef cds::gc::HP gc_type;

    class IntrusiveEllenBinTree_HP : public cds_test::intrusive_tree_hp
    {
    protected:
        typedef cds_test::intrusive_tree_hp base_class;

    protected:
        typedef base_class::key_type key_type;

        typedef typename base_class::base_int_item< ci::ellen_bintree::node<gc_type>> base_item_type;
        typedef typename base_class::member_int_item< ci::ellen_bintree::node<gc_type>>  member_item_type;

        void SetUp()
        {
            typedef ci::EllenBinTree< gc_type, key_type, base_item_type > tree_type;

            // +1 - for guarded_ptr
            cds::gc::hp::GarbageCollector::Construct( tree_type::c_nHazardPtrCount + 1, 1, 16 );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::hp::GarbageCollector::Destruct( true );
        }

        struct generic_traits: public ci::ellen_bintree::traits
        {
            typedef base_class::key_extractor key_extractor;
            typedef mock_disposer disposer;
        };
    };


    TEST_F( IntrusiveEllenBinTree_HP, base_cmp )
    {
        typedef ci::EllenBinTree< gc_type, key_type, base_item_type,
            ci::ellen_bintree::make_traits<
                ci::opt::type_traits< generic_traits >
                ,ci::opt::hook< ci::ellen_bintree::base_hook< ci::opt::gc< gc_type >>>
                ,ci::opt::compare< cmp<base_item_type>>
            >::type
        > tree_type;

        tree_type t;
        test( t );
    }

    TEST_F( IntrusiveEllenBinTree_HP, base_less )
    {
        typedef ci::EllenBinTree< gc_type, key_type, base_item_type,
            ci::ellen_bintree::make_traits<
                ci::opt::type_traits< generic_traits >
                ,ci::opt::hook< ci::ellen_bintree::base_hook< ci::opt::gc< gc_type >>>
                ,ci::opt::less< less<base_item_type>>
            >::type
        > tree_type;

        tree_type t;
        test( t );
    }

    TEST_F( IntrusiveEllenBinTree_HP, base_item_counter )
    {
        typedef ci::EllenBinTree< gc_type, key_type, base_item_type,
            ci::ellen_bintree::make_traits<
                ci::opt::type_traits< generic_traits >
                ,ci::opt::hook< ci::ellen_bintree::base_hook< ci::opt::gc< gc_type >>>
                ,ci::opt::compare< cmp<base_item_type>>
                ,ci::opt::item_counter< simple_item_counter >
            >::type
        > tree_type;

        tree_type t;
        test( t );
    }

    TEST_F( IntrusiveEllenBinTree_HP, base_backoff )
    {
        struct tree_traits: public generic_traits
        {
            typedef ci::ellen_bintree::base_hook< ci::opt::gc< gc_type >> hook;
            typedef cmp<base_item_type> compare;
            typedef base_class::less<base_item_type> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
        };

        typedef ci::EllenBinTree< gc_type, key_type, base_item_type, tree_traits > tree_type;

        tree_type t;
        test( t );
    }

    TEST_F( IntrusiveEllenBinTree_HP, base_seq_cst )
    {
        struct tree_traits: public generic_traits
        {
            typedef ci::ellen_bintree::base_hook< ci::opt::gc< gc_type >> hook;
            typedef cmp<base_item_type> compare;
            typedef base_class::less<base_item_type> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::pause back_off;
            typedef ci::opt::v::sequential_consistent memory_model;
        };

        typedef ci::EllenBinTree< gc_type, key_type, base_item_type, tree_traits > tree_type;

        tree_type t;
        test( t );
    }

    TEST_F( IntrusiveEllenBinTree_HP, base_update_desc_pool )
    {
        struct tree_traits: public generic_traits
        {
            typedef ci::ellen_bintree::base_hook< ci::opt::gc< gc_type >> hook;
            typedef base_class::less<base_item_type> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::memory::pool_allocator< cds_test::update_desc, cds_test::pool_accessor> update_desc_allocator;
        };

        typedef ci::EllenBinTree< gc_type, key_type, base_item_type, tree_traits > tree_type;

        tree_type t;
        test( t );
    }

    TEST_F( IntrusiveEllenBinTree_HP, base_update_desc_lazy_pool )
    {
        struct tree_traits: public generic_traits
        {
            typedef ci::ellen_bintree::base_hook< ci::opt::gc< gc_type >> hook;
            typedef base_class::less<base_item_type> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::lazy_pool_accessor> update_desc_allocator;
        };

        typedef ci::EllenBinTree< gc_type, key_type, base_item_type, tree_traits > tree_type;

        tree_type t;
        test( t );
    }

    // member hook
    TEST_F( IntrusiveEllenBinTree_HP, member_cmp )
    {
        typedef ci::EllenBinTree< gc_type, key_type, member_item_type,
            ci::ellen_bintree::make_traits<
                ci::opt::type_traits< generic_traits >
                ,ci::opt::hook< ci::ellen_bintree::member_hook< offsetof( member_item_type, hMember), ci::opt::gc< gc_type >>>
                ,ci::opt::compare< cmp<member_item_type>>
            >::type
        > tree_type;

        tree_type t;
        test( t );
    }

    TEST_F( IntrusiveEllenBinTree_HP, member_less )
    {
        typedef ci::EllenBinTree< gc_type, key_type, member_item_type,
            ci::ellen_bintree::make_traits<
                ci::opt::type_traits< generic_traits >
                ,ci::opt::hook< ci::ellen_bintree::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< gc_type >>>
                ,ci::opt::less< less<member_item_type>>
            >::type
        > tree_type;

        tree_type t;
        test( t );
    }

    TEST_F( IntrusiveEllenBinTree_HP, member_item_counter )
    {
        typedef ci::EllenBinTree< gc_type, key_type, member_item_type,
            ci::ellen_bintree::make_traits<
                ci::opt::type_traits< generic_traits >
                ,ci::opt::hook< ci::ellen_bintree::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< gc_type >>>
                ,ci::opt::compare< cmp<member_item_type>>
                ,ci::opt::item_counter< simple_item_counter >
            >::type
        > tree_type;

        tree_type t;
        test( t );
    }

    TEST_F( IntrusiveEllenBinTree_HP, member_backoff )
    {
        struct tree_traits: public generic_traits
        {
            typedef ci::ellen_bintree::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< gc_type >> hook;
            typedef cmp<member_item_type> compare;
            typedef base_class::less<member_item_type> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
        };

        typedef ci::EllenBinTree< gc_type, key_type, member_item_type, tree_traits > tree_type;

        tree_type t;
        test( t );
    }

    TEST_F( IntrusiveEllenBinTree_HP, member_seq_cst )
    {
        struct tree_traits: public generic_traits
        {
            typedef ci::ellen_bintree::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< gc_type >> hook;
            typedef cmp<member_item_type> compare;
            typedef base_class::less<member_item_type> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::pause back_off;
            typedef ci::opt::v::sequential_consistent memory_model;
        };

        typedef ci::EllenBinTree< gc_type, key_type, member_item_type, tree_traits > tree_type;

        tree_type t;
        test( t );
    }

    TEST_F( IntrusiveEllenBinTree_HP, member_update_desc_pool )
    {
        struct tree_traits: public generic_traits
        {
            typedef ci::ellen_bintree::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< gc_type >> hook;
            typedef base_class::less<member_item_type> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::pool_accessor> update_desc_allocator;
        };

        typedef ci::EllenBinTree< gc_type, key_type, member_item_type, tree_traits > tree_type;

        tree_type t;
        test( t );
    }

    TEST_F( IntrusiveEllenBinTree_HP, member_update_desc_lazy_pool )
    {
        struct tree_traits: public generic_traits
        {
            typedef ci::ellen_bintree::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< gc_type >> hook;
            typedef base_class::less<member_item_type> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::lazy_pool_accessor> update_desc_allocator;
        };

        typedef ci::EllenBinTree< gc_type, key_type, member_item_type, tree_traits > tree_type;

        tree_type t;
        test( t );
    }

} // namespace
