// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_tree_map_hp.h"

#include <cds/container/ellen_bintree_map_dhp.h>
#include "test_ellen_bintree_update_desc_pool.h"

namespace {
    namespace cc = cds::container;
    typedef cds::gc::DHP gc_type;

    class EllenBinTreeMap_DHP : public cds_test::container_tree_map_hp
    {
    protected:
        typedef cds_test::container_tree_map_hp base_class;

        void SetUp()
        {
            typedef cc::EllenBinTreeMap< gc_type, key_type, value_type > map_type;

            cds::gc::dhp::smr::construct( map_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::dhp::smr::destruct();
        }
    };


    TEST_F( EllenBinTreeMap_DHP, compare )
    {
        typedef cc::EllenBinTreeMap< gc_type, key_type, value_type,
            typename cc::ellen_bintree::make_map_traits<
                cds::opt::compare< cmp >
            >::type
        > map_type;

        map_type m;
        test( m );
    }

    TEST_F( EllenBinTreeMap_DHP, less )
    {
        typedef cc::EllenBinTreeMap< gc_type, key_type, value_type,
            typename cc::ellen_bintree::make_map_traits<
                cds::opt::less< base_class::less >
            >::type
        > map_type;

        map_type m;
        test( m );
    }

    TEST_F( EllenBinTreeMap_DHP, cmpmix )
    {
        typedef cc::EllenBinTreeMap< gc_type, key_type, value_type,
            typename cc::ellen_bintree::make_map_traits<
                cds::opt::less< base_class::less >
                ,cds::opt::compare< cmp >
            >::type
        > map_type;

        map_type m;
        test( m );
    }

    TEST_F( EllenBinTreeMap_DHP, update_desc_pool )
    {
        struct map_traits: public cc::ellen_bintree::traits
        {
            typedef cmp compare;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::pool_accessor> update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m;
        test( m );
    }

    TEST_F( EllenBinTreeMap_DHP, update_desc_lazy_pool )
    {
        struct map_traits: public cc::ellen_bintree::traits
        {
            typedef cmp compare;
            typedef cds::memory::pool_allocator< cds_test::update_desc, cds_test::lazy_pool_accessor> update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m;
        test( m );
    }

    TEST_F( EllenBinTreeMap_DHP, item_counting )
    {
        struct map_traits: public cc::ellen_bintree::traits
        {
            typedef cmp compare;
            typedef base_class::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::pool_accessor> update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m;
        test( m );
    }

    TEST_F( EllenBinTreeMap_DHP, backoff )
    {
        struct map_traits: public cc::ellen_bintree::traits
        {
            typedef cmp compare;
            typedef base_class::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::lazy_pool_accessor> update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m;
        test( m );
    }

    TEST_F( EllenBinTreeMap_DHP, stat )
    {
        struct map_traits: public cc::ellen_bintree::traits
        {
            typedef base_class::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
            typedef cc::ellen_bintree::stat<> stat;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::pool_accessor> update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m;
        test( m );
    }

    TEST_F( EllenBinTreeMap_DHP, copy_policy )
    {
        struct map_traits: public cc::ellen_bintree::traits
        {
            typedef base_class::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::pool_accessor> update_desc_allocator;
            typedef copy_key copy_policy;
        };
        typedef cc::EllenBinTreeMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m;
        test( m );
    }

    TEST_F( EllenBinTreeMap_DHP, seq_cst )
    {
        struct map_traits: public cc::ellen_bintree::traits
        {
            typedef cmp compare;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::lazy_pool_accessor> update_desc_allocator;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef cc::EllenBinTreeMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m;
        test( m );
    }

} // namespace
