// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_map_hp.h"

#include <cds/container/lazy_list_dhp.h>
#include <cds/container/split_list_map.h>
#include <cds/intrusive/free_list.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::DHP gc_type;

    class SplitListLazyMap_DHP : public cds_test::container_map_hp
    {
    protected:
        typedef cds_test::container_map_hp base_class;

        void SetUp()
        {
            struct map_traits: public cc::split_list::traits {
                typedef cc::lazy_list_tag ordered_list;
                typedef hash1 hash;
                struct ordered_list_traits: public cc::lazy_list::traits
                {
                    typedef cmp compare;
                };
            };
            typedef cc::SplitListMap< gc_type, key_type, value_type, map_traits > map_type;

            cds::gc::dhp::smr::construct( map_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::dhp::smr::destruct();
        }
    };

    TEST_F( SplitListLazyMap_DHP, compare )
    {
        typedef cc::SplitListMap< gc_type, key_type, value_type,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list< cc::lazy_list_tag >
                , cds::opt::hash< hash1 >
                , cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        cds::opt::compare< cmp >
                    >::type
                >
            >::type
        > map_type;

        map_type m( kSize, 2 );
        test( m );
    }

    TEST_F( SplitListLazyMap_DHP, less )
    {
        typedef cc::SplitListMap< gc_type, key_type, value_type,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list< cc::lazy_list_tag >
                , cds::opt::hash< hash1 >
                , cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        cds::opt::less< less >
                    >::type
                >
            >::type
        > map_type;

        map_type m( kSize, 1 );
        test( m );
    }

    TEST_F( SplitListLazyMap_DHP, cmpmix )
    {
        typedef cc::SplitListMap< gc_type, key_type, value_type,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list< cc::lazy_list_tag >
                , cds::opt::hash< hash1 >
                , cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        cds::opt::less< less >
                        , cds::opt::compare< cmp >
                    >::type
                >
            >::type
        > map_type;

        map_type m( kSize, 2 );
        test( m );
    }

    TEST_F( SplitListLazyMap_DHP, item_counting )
    {
        struct map_traits: public cc::split_list::traits
        {
            typedef cc::lazy_list_tag ordered_list;
            typedef hash1 hash;
            typedef cds::atomicity::item_counter item_counter;

            struct ordered_list_traits: public cc::lazy_list::traits
            {
                typedef cmp compare;
                typedef base_class::less less;
                typedef cds::backoff::empty back_off;
            };
        };
        typedef cc::SplitListMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m( kSize, 4 );
        test( m );
    }

    TEST_F( SplitListLazyMap_DHP, stat )
    {
        struct map_traits: public cc::split_list::traits
        {
            typedef cc::lazy_list_tag ordered_list;
            typedef hash1 hash;
            typedef cds::atomicity::item_counter item_counter;
            typedef cc::split_list::stat<> stat;

            struct ordered_list_traits: public cc::lazy_list::traits
            {
                typedef base_class::less less;
                typedef cds::opt::v::sequential_consistent memory_model;
            };
        };
        typedef cc::SplitListMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m( kSize, 2 );
        test( m );
    }

    TEST_F( SplitListLazyMap_DHP, back_off )
    {
        struct map_traits: public cc::split_list::traits
        {
            typedef cc::lazy_list_tag ordered_list;
            typedef hash1 hash;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
            typedef cds::opt::v::sequential_consistent memory_model;

            struct ordered_list_traits: public cc::lazy_list::traits
            {
                typedef cmp compare;
                typedef cds::backoff::empty back_off;
            };
        };
        typedef cc::SplitListMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m( kSize, 3 );
        test( m );
    }

    TEST_F( SplitListLazyMap_DHP, free_list )
    {
        struct map_traits: public cc::split_list::traits
        {
            typedef cc::lazy_list_tag ordered_list;
            typedef hash1 hash;
            typedef cds::intrusive::FreeList free_list;

            struct ordered_list_traits: public cc::lazy_list::traits
            {
                typedef cmp compare;
                typedef cds::backoff::empty back_off;
            };
        };
        typedef cc::SplitListMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m( kSize, 3 );
        test( m );
    }

    TEST_F( SplitListLazyMap_DHP, mutex )
    {
        struct map_traits: public cc::split_list::traits
        {
            typedef cc::lazy_list_tag ordered_list;
            typedef hash1 hash;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
            typedef cds::opt::v::sequential_consistent memory_model;

            struct ordered_list_traits: public cc::lazy_list::traits
            {
                typedef cmp compare;
                typedef cds::backoff::pause back_off;
                typedef std::mutex lock_type;
            };
        };
        typedef cc::SplitListMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m( kSize, 2 );
        test( m );
    }

    struct set_static_traits: public cc::split_list::traits
    {
        static bool const dynamic_bucket_table = false;
    };

    TEST_F( SplitListLazyMap_DHP, static_bucket_table )
    {
        struct map_traits: public set_static_traits
        {
            typedef cc::lazy_list_tag ordered_list;
            typedef hash1 hash;
            typedef cds::atomicity::item_counter item_counter;

            struct ordered_list_traits: public cc::lazy_list::traits
            {
                typedef cmp compare;
                typedef cds::backoff::pause back_off;
            };
        };
        typedef cc::SplitListMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m( kSize, 4 );
        test( m );
    }

    TEST_F( SplitListLazyMap_DHP, static_bucket_table_free_list )
    {
        struct map_traits: public set_static_traits
        {
            typedef cc::lazy_list_tag ordered_list;
            typedef hash1 hash;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::intrusive::FreeList free_list;

            struct ordered_list_traits: public cc::lazy_list::traits
            {
                typedef cmp compare;
                typedef cds::backoff::pause back_off;
            };
        };
        typedef cc::SplitListMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m( kSize, 4 );
        test( m );
    }

} // namespace
