// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_tree_set_hp.h"

#include <cds/container/ellen_bintree_set_hp.h>
#include "test_ellen_bintree_update_desc_pool.h"

namespace {
    namespace cc = cds::container;
    typedef cds::gc::HP gc_type;

    class EllenBinTreeSet_HP : public cds_test::container_tree_set_hp
    {
    protected:
        typedef cds_test::container_tree_set_hp base_class;
        typedef int key_type;

        struct generic_traits: public cc::ellen_bintree::traits
        {
            typedef base_class::key_extractor key_extractor;
        };

        void SetUp()
        {
            typedef cc::EllenBinTreeSet< gc_type, key_type, int_item > set_type;

            // +1 - for guarded_ptr
            cds::gc::hp::GarbageCollector::Construct( set_type::c_nHazardPtrCount + 1, 1, 16 );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::hp::GarbageCollector::Destruct( true );
        }
    };


    TEST_F( EllenBinTreeSet_HP, compare )
    {
        typedef cc::EllenBinTreeSet< gc_type, key_type, int_item,
            typename cc::ellen_bintree::make_set_traits<
                cc::opt::type_traits< generic_traits >
                ,cds::opt::compare< cmp >
            >::type
        > set_type;

        set_type s;
        test( s );
    }

    TEST_F( EllenBinTreeSet_HP, less )
    {
        typedef cc::EllenBinTreeSet< gc_type, key_type, int_item,
            typename cc::ellen_bintree::make_set_traits<
                cc::opt::type_traits< generic_traits >
                ,cds::opt::less< base_class::less >
            >::type
        > set_type;

        set_type s;
        test( s );
    }

    TEST_F( EllenBinTreeSet_HP, cmpmix )
    {
        typedef cc::EllenBinTreeSet< gc_type, key_type, int_item,
            typename cc::ellen_bintree::make_set_traits<
                cc::opt::type_traits< generic_traits >
                ,cds::opt::less< base_class::less >
                ,cds::opt::compare< cmp >
            >::type
        > set_type;

        set_type s;
        test( s );
    }

    TEST_F( EllenBinTreeSet_HP, update_desc_pool )
    {
        struct set_traits: public generic_traits
        {
            typedef cmp compare;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::pool_accessor> update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< gc_type, key_type, int_item, set_traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( EllenBinTreeSet_HP, update_desc_lazy_pool )
    {
        struct set_traits: public generic_traits
        {
            typedef cmp compare;
            typedef cds::memory::pool_allocator< cds_test::update_desc, cds_test::lazy_pool_accessor> update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< gc_type, key_type, int_item, set_traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( EllenBinTreeSet_HP, item_counting )
    {
        struct set_traits: public generic_traits
        {
            typedef cmp compare;
            typedef base_class::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::pool_accessor> update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< gc_type, key_type, int_item, set_traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( EllenBinTreeSet_HP, backoff )
    {
        struct set_traits: public generic_traits
        {
            typedef cmp compare;
            typedef base_class::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::lazy_pool_accessor> update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< gc_type, key_type, int_item, set_traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( EllenBinTreeSet_HP, stat )
    {
        struct set_traits: public generic_traits
        {
            typedef base_class::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
            typedef cc::ellen_bintree::stat<> stat;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::pool_accessor> update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< gc_type, key_type, int_item, set_traits > set_type;

        set_type s;
        test( s );
    }

} // namespace
