// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_map_hp.h"

#include <cds/container/michael_kvlist_dhp.h>
#include <cds/container/michael_map.h>

namespace {

    namespace cc = cds::container;
    typedef cds::gc::DHP gc_type;

    class MichaelMap_DHP: public cds_test::container_map_hp
    {
    protected:
        typedef cds_test::container_map_hp base_class;

        void SetUp()
        {
            typedef cc::MichaelKVList< gc_type, key_type, value_type > list_type;
            typedef cc::MichaelHashMap< gc_type, list_type >   map_type;

            cds::gc::dhp::smr::construct( map_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::dhp::smr::destruct();
        }
    };

    TEST_F( MichaelMap_DHP, compare )
    {
        typedef cc::MichaelKVList< gc_type, key_type, value_type,
            typename cc::michael_list::make_traits<
                cds::opt::compare< cmp >
            >::type
        > list_type;

        typedef cc::MichaelHashMap< gc_type, list_type,
            typename cc::michael_map::make_traits<
                cds::opt::hash< hash1 >
            >::type
        > map_type;

        map_type m( kSize, 2 );
        test( m );
    }

    TEST_F( MichaelMap_DHP, less )
    {
        typedef cc::MichaelKVList< gc_type, key_type, value_type,
            typename cc::michael_list::make_traits<
                cds::opt::less< less >
            >::type
        > list_type;

        typedef cc::MichaelHashMap< gc_type, list_type,
            typename cc::michael_map::make_traits<
                cds::opt::hash< hash1 >
            >::type
        > map_type;

        map_type m( kSize, 1 );
        test( m );
    }

    TEST_F( MichaelMap_DHP, cmpmix )
    {
        typedef cc::MichaelKVList< gc_type, key_type, value_type,
            typename cc::michael_list::make_traits<
                cds::opt::less< less >
                ,cds::opt::compare< cmp >
            >::type
        > list_type;

        typedef cc::MichaelHashMap< gc_type, list_type,
            typename cc::michael_map::make_traits<
                cds::opt::hash< hash1 >
            >::type
        > map_type;

        map_type m( kSize, 2 );
        test( m );
    }

    TEST_F( MichaelMap_DHP, backoff )
    {
        struct list_traits: public cc::michael_list::traits
        {
            typedef cmp compare;
            typedef cds::backoff::make_exponential_t<cds::backoff::pause, cds::backoff::yield> back_off;
        };
        typedef cc::MichaelKVList< gc_type, key_type, value_type, list_traits > list_type;

        struct map_traits: public cc::michael_map::traits
        {
            typedef hash1 hash;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::MichaelHashMap< gc_type, list_type, map_traits > map_type;

        map_type m( kSize, 4 );
        test( m );
    }

    TEST_F( MichaelMap_DHP, seq_cst )
    {
        struct list_traits: public cc::michael_list::traits
        {
            typedef cmp compare;
            typedef cds::backoff::yield back_off;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef cc::MichaelKVList< gc_type, key_type, value_type, list_traits > list_type;

        struct map_traits: public cc::michael_map::traits
        {
            typedef hash1 hash;
        };
        typedef cc::MichaelHashMap< gc_type, list_type, map_traits > map_type;

        map_type s( kSize, 8 );
        test( s );
    }

    TEST_F( MichaelMap_DHP, stat )
    {
        struct list_traits: public cc::michael_list::traits
        {
            typedef cmp compare;
            typedef cds::backoff::yield back_off;
            typedef cc::michael_list::stat<> stat;
        };
        typedef cc::MichaelKVList< gc_type, key_type, value_type, list_traits > list_type;

        struct map_traits: public cc::michael_map::traits
        {
            typedef hash1 hash;
        };
        typedef cc::MichaelHashMap< gc_type, list_type, map_traits > map_type;

        map_type m( kSize, 8 );
        test( m );
        EXPECT_GE( m.statistics().m_nInsertSuccess, 0u );
    }

    TEST_F( MichaelMap_DHP, wrapped_stat )
    {
        struct list_traits: public cc::michael_list::traits
        {
            typedef cmp compare;
            typedef cc::michael_list::wrapped_stat<> stat;
        };
        typedef cc::MichaelKVList< gc_type, key_type, value_type, list_traits > list_type;

        struct map_traits: public cc::michael_map::traits
        {
            typedef hash1 hash;
        };
        typedef cc::MichaelHashMap< gc_type, list_type, map_traits > map_type;

        map_type m( kSize, 8 );
        test( m );
        EXPECT_GE( m.statistics().m_nInsertSuccess, 0u );
    }

} // namespace

