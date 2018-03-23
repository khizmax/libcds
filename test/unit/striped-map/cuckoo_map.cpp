// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_map.h"

#include <cds/container/cuckoo_map.h>

namespace {
    namespace cc = cds::container;

    class CuckooMap : public cds_test::container_map
    {
    protected:
        typedef cds_test::container_map base_class;

        template <typename Set>
        void test( Set& m )
        {
            // Precondition: set is empty
            // Postcondition: set is empty

            base_class::test_< Set::c_isSorted>( m );
        }

        //void SetUp()
        //{}

        //void TearDown()
        //{}
    };

    struct store_hash_traits: public cc::cuckoo::traits
    {
        static bool const store_hash = true;
    };


//************************************************************
// striped set

    TEST_F( CuckooMap, striped_list_unordered )
    {
        struct map_traits: public cc::cuckoo::traits
        {
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef base_class::equal_to equal_to;
            typedef cc::cuckoo::list probeset_type;
        };
        typedef cc::CuckooMap< key_type, value_type, map_traits > map_type;

        map_type m;
        test( m );
    }

    TEST_F( CuckooMap, striped_vector_unordered )
    {
        struct map_traits: public cc::cuckoo::traits
        {
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef base_class::equal_to equal_to;
            typedef cc::cuckoo::vector<4> probeset_type;
        };
        typedef cc::CuckooMap< key_type, value_type, map_traits > map_type;

        map_type m( 32, 4 );
        test( m );
    }

    TEST_F( CuckooMap, striped_list_ordered_cmp )
    {
        typedef cc::CuckooMap< key_type, value_type
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::compare< cmp >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
            >::type
        > map_type;

        map_type m( 32, 6, 4 );
        test( m );
    }

    TEST_F( CuckooMap, striped_vector_ordered_cmp )
    {
        typedef cc::CuckooMap< key_type, value_type
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::compare< cmp >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<8>>
            >::type
        > map_type;

        typename map_type::hash_tuple_type ht;
        map_type m( ht );
        test( m );
    }

    TEST_F( CuckooMap, striped_list_ordered_less )
    {
        typedef cc::CuckooMap< key_type, value_type
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
            >::type
        > map_type;

        typename map_type::hash_tuple_type ht;
        map_type m( 32, 6, 4, ht );
        test( m );
    }

    TEST_F( CuckooMap, striped_vector_ordered_less )
    {
        typedef cc::CuckooMap< key_type, value_type
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<6>>
            >::type
        > map_type;

        typename map_type::hash_tuple_type ht;
        map_type m( std::move( ht ));
        test( m );
    }

    TEST_F( CuckooMap, striped_list_ordered_cmpmix )
    {
        typedef cc::CuckooMap< key_type, value_type
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
            >::type
        > map_type;

        typename map_type::hash_tuple_type ht;
        map_type m( 32, 6, 0, std::move( ht ));
        test( m );
    }

    TEST_F( CuckooMap, striped_vector_ordered_cmpmix )
    {
        typedef cc::CuckooMap< key_type, value_type
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<6>>
            >::type
        > map_type;

        typename map_type::hash_tuple_type ht;
        map_type m( std::move( ht ));
        test( m );
    }

    TEST_F( CuckooMap, striped_list_ordered_stat )
    {
        typedef cc::CuckooMap< key_type, value_type
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cds::opt::stat< cc::cuckoo::stat >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
            >::type
        > map_type;

        map_type m;
        test( m );
    }

    TEST_F( CuckooMap, striped_vector_ordered_stat )
    {
        typedef cc::CuckooMap< key_type, value_type
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cds::opt::stat< cc::cuckoo::stat >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<8>>
            >::type
        > map_type;

        map_type m;
        test( m );
    }

    TEST_F( CuckooMap, striped_list_unordered_storehash )
    {
        struct map_traits: public store_hash_traits
        {
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef base_class::equal_to equal_to;
            typedef cc::cuckoo::list     probeset_type;
            typedef cc::cuckoo::stat     stat;
        };
        typedef cc::CuckooMap< key_type, value_type, map_traits > map_type;

        map_type m;
        test( m );
    }

    TEST_F( CuckooMap, striped_vector_unordered_storehash )
    {
        struct map_traits: public store_hash_traits
        {
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef base_class::equal_to    equal_to;
            typedef cc::cuckoo::stat        stat;
            typedef cc::cuckoo::vector<4>   probeset_type;
        };
        typedef cc::CuckooMap< key_type, value_type, map_traits > map_type;

        map_type m( 32, 4 );
        test( m );
    }

    TEST_F( CuckooMap, striped_list_ordered_storehash )
    {
        typedef cc::CuckooMap< key_type, value_type
            ,cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                ,cc::cuckoo::store_hash< true >
            >::type
        > map_type;

        typename map_type::hash_tuple_type ht;
        map_type m( 32, 6, 0, std::move( ht ));
        test( m );
    }

    TEST_F( CuckooMap, striped_vector_ordered_storehash )
    {
        typedef cc::CuckooMap< key_type, value_type
            ,cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<6>>
                ,cc::cuckoo::store_hash< true >
            >::type
        > map_type;

        typename map_type::hash_tuple_type ht;
        map_type m( std::move( ht ));
        test( m );
    }


//************************************************************
// refinable set

    TEST_F( CuckooMap, refinable_list_unordered )
    {
        struct map_traits: public cc::cuckoo::traits
        {
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef base_class::equal_to equal_to;
            typedef cc::cuckoo::list probeset_type;
            typedef cc::cuckoo::refinable<> mutex_policy;
        };
        typedef cc::CuckooMap< key_type, value_type, map_traits > map_type;

        map_type m;
        test( m );
    }

    TEST_F( CuckooMap, refinable_vector_unordered )
    {
        struct map_traits: public cc::cuckoo::traits
        {
            typedef cc::cuckoo::refinable<> mutex_policy;
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef base_class::equal_to equal_to;
            typedef cc::cuckoo::vector<4> probeset_type;
        };
        typedef cc::CuckooMap< key_type, value_type, map_traits > map_type;

        map_type m( 32, 4 );
        test( m );
    }

    TEST_F( CuckooMap, refinable_list_ordered_cmp )
    {
        typedef cc::CuckooMap< key_type, value_type
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::cuckoo::refinable<>>
                ,cds::opt::compare< cmp >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
            >::type
        > map_type;

        map_type m( 32, 6, 4 );
        test( m );
    }

    TEST_F( CuckooMap, refinable_vector_ordered_cmp )
    {
        typedef cc::CuckooMap< key_type, value_type
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 >>
                ,cds::opt::mutex_policy< cc::cuckoo::refinable<>>
                ,cds::opt::compare< cmp >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<8>>
            >::type
        > map_type;

        typename map_type::hash_tuple_type ht;
        map_type m( ht );
        test( m );
    }

    TEST_F( CuckooMap, refinable_list_ordered_less )
    {
        typedef cc::CuckooMap< key_type, value_type
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::cuckoo::refinable<>>
                ,cds::opt::less< less >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
            >::type
        > map_type;

        typename map_type::hash_tuple_type ht;
        map_type m( 32, 6, 4, ht );
        test( m );
    }

    TEST_F( CuckooMap, refinable_vector_ordered_less )
    {
        typedef cc::CuckooMap< key_type, value_type
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::cuckoo::refinable<>>
                ,cds::opt::less< less >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<6>>
            >::type
        > map_type;

        typename map_type::hash_tuple_type ht;
        map_type m( std::move( ht ));
        test( m );
    }

    TEST_F( CuckooMap, refinable_list_ordered_cmpmix )
    {
        typedef cc::CuckooMap< key_type, value_type
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::cuckoo::refinable<>>
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
            >::type
        > map_type;

        typename map_type::hash_tuple_type ht;
        map_type m( 32, 6, 0, std::move( ht ));
        test( m );
    }

    TEST_F( CuckooMap, refinable_vector_ordered_cmpmix )
    {
        typedef cc::CuckooMap< key_type, value_type
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::cuckoo::refinable<>>
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<6>>
            >::type
        > map_type;

        typename map_type::hash_tuple_type ht;
        map_type m( std::move( ht ));
        test( m );
    }

    TEST_F( CuckooMap, refinable_list_ordered_stat )
    {
        typedef cc::CuckooMap< key_type, value_type
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::cuckoo::refinable<>>
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cds::opt::stat< cc::cuckoo::stat >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
            >::type
        > map_type;

        map_type m;
        test( m );
    }

    TEST_F( CuckooMap, refinable_vector_ordered_stat )
    {
        typedef cc::CuckooMap< key_type, value_type
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::cuckoo::refinable<>>
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cds::opt::stat< cc::cuckoo::stat >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<8>>
            >::type
        > map_type;

        map_type m;
        test( m );
    }

    TEST_F( CuckooMap, refinable_list_unordered_storehash )
    {
        struct map_traits: public store_hash_traits
        {
            typedef cc::cuckoo::refinable<> mutex_policy;
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef base_class::equal_to equal_to;
            typedef cc::cuckoo::list     probeset_type;
            typedef cc::cuckoo::stat     stat;
        };
        typedef cc::CuckooMap< key_type, value_type, map_traits > map_type;

        map_type m;
        test( m );
    }

    TEST_F( CuckooMap, refinable_vector_unordered_storehash )
    {
        struct map_traits: public store_hash_traits
        {
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef cc::cuckoo::refinable<> mutex_policy;
            typedef base_class::equal_to    equal_to;
            typedef cc::cuckoo::stat        stat;
            typedef cc::cuckoo::vector<4>   probeset_type;
        };
        typedef cc::CuckooMap< key_type, value_type, map_traits > map_type;

        map_type m( 32, 4 );
        test( m );
    }

    TEST_F( CuckooMap, refinable_list_ordered_storehash )
    {
        typedef cc::CuckooMap< key_type, value_type
            ,cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::cuckoo::refinable<>>
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                ,cc::cuckoo::store_hash< true >
            >::type
        > map_type;

        typename map_type::hash_tuple_type ht;
        map_type m( 32, 6, 0, std::move( ht ));
        test( m );
    }

    TEST_F( CuckooMap, refinable_vector_ordered_storehash )
    {
        typedef cc::CuckooMap< key_type, value_type
            ,cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::cuckoo::refinable<>>
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<6>>
                ,cc::cuckoo::store_hash< true >
            >::type
        > map_type;

        typename map_type::hash_tuple_type ht;
        map_type m( std::move( ht ));
        test( m );
    }


} // namespace
