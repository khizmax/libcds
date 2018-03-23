// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_tree_set_hp.h"

#include <cds/container/ellen_bintree_set_dhp.h>
#include "test_ellen_bintree_update_desc_pool.h"

namespace {
    namespace cc = cds::container;
    typedef cds::gc::DHP gc_type;

    class EllenBinTreeSet_DHP : public cds_test::container_tree_set_hp
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

            cds::gc::dhp::smr::construct( set_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::dhp::smr::destruct();
        }
    };


    TEST_F( EllenBinTreeSet_DHP, compare )
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

    TEST_F( EllenBinTreeSet_DHP, less )
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

    TEST_F( EllenBinTreeSet_DHP, cmpmix )
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

    TEST_F( EllenBinTreeSet_DHP, update_desc_pool )
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

    TEST_F( EllenBinTreeSet_DHP, update_desc_lazy_pool )
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

    TEST_F( EllenBinTreeSet_DHP, item_counting )
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

    TEST_F( EllenBinTreeSet_DHP, backoff )
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

    TEST_F( EllenBinTreeSet_DHP, stat )
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
