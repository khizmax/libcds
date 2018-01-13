/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "test_map.h"

#include <cds/container/hopscotch_hashmap.h>

namespace {
    namespace cc = cds::container;

    class HopscotchHashmap : public cds_test::container_map
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

    struct store_hash_traits: public cc::hopscotch_hashmap::traits
    {
        static bool const store_hash = true;
    };


//************************************************************
// striped set

    TEST_F( HopscotchHashmap, striped_list_unordered )
    {
        struct map_traits: public cc::hopscotch_hashmap::traits
        {
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef base_class::equal_to equal_to;
            typedef cc::hopscotch_hashmap::list probeset_type;
        };
        typedef cc::HopscotchHashmap< key_type, value_type, map_traits > map_type;

        map_type m;
        test( m );
    }

    TEST_F( HopscotchHashmap, striped_vector_unordered )
    {
        struct map_traits: public cc::hopscotch_hashmap::traits
        {
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef base_class::equal_to equal_to;
            typedef cc::hopscotch_hashmap::vector<4> probeset_type;
        };
        typedef cc::HopscotchHashmap< key_type, value_type, map_traits > map_type;

        map_type m( 32, 4 );
        test( m );
    }

    TEST_F( HopscotchHashmap, striped_list_ordered_cmp )
    {
        typedef cc::HopscotchHashmap< key_type, value_type
            , cc::hopscotch_hashmap::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::compare< cmp >
                ,cc::hopscotch_hashmap::probeset_type< cc::hopscotch_hashmap::list >
            >::type
        > map_type;

        map_type m( 32, 6, 4 );
        test( m );
    }

    TEST_F( HopscotchHashmap, striped_vector_ordered_cmp )
    {
        typedef cc::HopscotchHashmap< key_type, value_type
            , cc::hopscotch_hashmap::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::compare< cmp >
                ,cc::hopscotch_hashmap::probeset_type< cc::hopscotch_hashmap::vector<8>>
            >::type
        > map_type;

        typename map_type::hash_tuple_type ht;
        map_type m( ht );
        test( m );
    }

    TEST_F( HopscotchHashmap, striped_list_ordered_less )
    {
        typedef cc::HopscotchHashmap< key_type, value_type
            , cc::hopscotch_hashmap::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cc::hopscotch_hashmap::probeset_type< cc::hopscotch_hashmap::list >
            >::type
        > map_type;

        typename map_type::hash_tuple_type ht;
        map_type m( 32, 6, 4, ht );
        test( m );
    }

    TEST_F( HopscotchHashmap, striped_vector_ordered_less )
    {
        typedef cc::HopscotchHashmap< key_type, value_type
            , cc::hopscotch_hashmap::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cc::hopscotch_hashmap::probeset_type< cc::hopscotch_hashmap::vector<6>>
            >::type
        > map_type;

        typename map_type::hash_tuple_type ht;
        map_type m( std::move( ht ));
        test( m );
    }

    TEST_F( HopscotchHashmap, striped_list_ordered_cmpmix )
    {
        typedef cc::HopscotchHashmap< key_type, value_type
            , cc::hopscotch_hashmap::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::hopscotch_hashmap::probeset_type< cc::hopscotch_hashmap::list >
            >::type
        > map_type;

        typename map_type::hash_tuple_type ht;
        map_type m( 32, 6, 0, std::move( ht ));
        test( m );
    }

    TEST_F( HopscotchHashmap, striped_vector_ordered_cmpmix )
    {
        typedef cc::HopscotchHashmap< key_type, value_type
            , cc::hopscotch_hashmap::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::hopscotch_hashmap::probeset_type< cc::hopscotch_hashmap::vector<6>>
            >::type
        > map_type;

        typename map_type::hash_tuple_type ht;
        map_type m( std::move( ht ));
        test( m );
    }

    TEST_F( HopscotchHashmap, striped_list_ordered_stat )
    {
        typedef cc::HopscotchHashmap< key_type, value_type
            , cc::hopscotch_hashmap::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cds::opt::stat< cc::hopscotch_hashmap::stat >
                ,cc::hopscotch_hashmap::probeset_type< cc::hopscotch_hashmap::list >
            >::type
        > map_type;

        map_type m;
        test( m );
    }

    TEST_F( HopscotchHashmap, striped_vector_ordered_stat )
    {
        typedef cc::HopscotchHashmap< key_type, value_type
            , cc::hopscotch_hashmap::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cds::opt::stat< cc::hopscotch_hashmap::stat >
                ,cc::hopscotch_hashmap::probeset_type< cc::hopscotch_hashmap::vector<8>>
            >::type
        > map_type;

        map_type m;
        test( m );
    }

    TEST_F( HopscotchHashmap, striped_list_unordered_storehash )
    {
        struct map_traits: public store_hash_traits
        {
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef base_class::equal_to equal_to;
            typedef cc::hopscotch_hashmap::list     probeset_type;
            typedef cc::hopscotch_hashmap::stat     stat;
        };
        typedef cc::HopscotchHashmap< key_type, value_type, map_traits > map_type;

        map_type m;
        test( m );
    }

    TEST_F( HopscotchHashmap, striped_vector_unordered_storehash )
    {
        struct map_traits: public store_hash_traits
        {
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef base_class::equal_to    equal_to;
            typedef cc::hopscotch_hashmap::stat        stat;
            typedef cc::hopscotch_hashmap::vector<4>   probeset_type;
        };
        typedef cc::HopscotchHashmap< key_type, value_type, map_traits > map_type;

        map_type m( 32, 4 );
        test( m );
    }

    TEST_F( HopscotchHashmap, striped_list_ordered_storehash )
    {
        typedef cc::HopscotchHashmap< key_type, value_type
            ,cc::hopscotch_hashmap::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::hopscotch_hashmap::probeset_type< cc::hopscotch_hashmap::list >
                ,cc::hopscotch_hashmap::store_hash< true >
            >::type
        > map_type;

        typename map_type::hash_tuple_type ht;
        map_type m( 32, 6, 0, std::move( ht ));
        test( m );
    }

    TEST_F( HopscotchHashmap, striped_vector_ordered_storehash )
    {
        typedef cc::HopscotchHashmap< key_type, value_type
            ,cc::hopscotch_hashmap::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::hopscotch_hashmap::probeset_type< cc::hopscotch_hashmap::vector<6>>
                ,cc::hopscotch_hashmap::store_hash< true >
            >::type
        > map_type;

        typename map_type::hash_tuple_type ht;
        map_type m( std::move( ht ));
        test( m );
    }


//************************************************************
// refinable set

    TEST_F( HopscotchHashmap, refinable_list_unordered )
    {
        struct map_traits: public cc::hopscotch_hashmap::traits
        {
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef base_class::equal_to equal_to;
            typedef cc::hopscotch_hashmap::list probeset_type;
            typedef cc::hopscotch_hashmap::refinable<> mutex_policy;
        };
        typedef cc::HopscotchHashmap< key_type, value_type, map_traits > map_type;

        map_type m;
        test( m );
    }

    TEST_F( HopscotchHashmap, refinable_vector_unordered )
    {
        struct map_traits: public cc::hopscotch_hashmap::traits
        {
            typedef cc::hopscotch_hashmap::refinable<> mutex_policy;
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef base_class::equal_to equal_to;
            typedef cc::hopscotch_hashmap::vector<4> probeset_type;
        };
        typedef cc::HopscotchHashmap< key_type, value_type, map_traits > map_type;

        map_type m( 32, 4 );
        test( m );
    }

    TEST_F( HopscotchHashmap, refinable_list_ordered_cmp )
    {
        typedef cc::HopscotchHashmap< key_type, value_type
            , cc::hopscotch_hashmap::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::hopscotch_hashmap::refinable<>>
                ,cds::opt::compare< cmp >
                ,cc::hopscotch_hashmap::probeset_type< cc::hopscotch_hashmap::list >
            >::type
        > map_type;

        map_type m( 32, 6, 4 );
        test( m );
    }

    TEST_F( HopscotchHashmap, refinable_vector_ordered_cmp )
    {
        typedef cc::HopscotchHashmap< key_type, value_type
            , cc::hopscotch_hashmap::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 >>
                ,cds::opt::mutex_policy< cc::hopscotch_hashmap::refinable<>>
                ,cds::opt::compare< cmp >
                ,cc::hopscotch_hashmap::probeset_type< cc::hopscotch_hashmap::vector<8>>
            >::type
        > map_type;

        typename map_type::hash_tuple_type ht;
        map_type m( ht );
        test( m );
    }

    TEST_F( HopscotchHashmap, refinable_list_ordered_less )
    {
        typedef cc::HopscotchHashmap< key_type, value_type
            , cc::hopscotch_hashmap::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::hopscotch_hashmap::refinable<>>
                ,cds::opt::less< less >
                ,cc::hopscotch_hashmap::probeset_type< cc::hopscotch_hashmap::list >
            >::type
        > map_type;

        typename map_type::hash_tuple_type ht;
        map_type m( 32, 6, 4, ht );
        test( m );
    }

    TEST_F( HopscotchHashmap, refinable_vector_ordered_less )
    {
        typedef cc::HopscotchHashmap< key_type, value_type
            , cc::hopscotch_hashmap::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::hopscotch_hashmap::refinable<>>
                ,cds::opt::less< less >
                ,cc::hopscotch_hashmap::probeset_type< cc::hopscotch_hashmap::vector<6>>
            >::type
        > map_type;

        typename map_type::hash_tuple_type ht;
        map_type m( std::move( ht ));
        test( m );
    }

    TEST_F( HopscotchHashmap, refinable_list_ordered_cmpmix )
    {
        typedef cc::HopscotchHashmap< key_type, value_type
            , cc::hopscotch_hashmap::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::hopscotch_hashmap::refinable<>>
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::hopscotch_hashmap::probeset_type< cc::hopscotch_hashmap::list >
            >::type
        > map_type;

        typename map_type::hash_tuple_type ht;
        map_type m( 32, 6, 0, std::move( ht ));
        test( m );
    }

    TEST_F( HopscotchHashmap, refinable_vector_ordered_cmpmix )
    {
        typedef cc::HopscotchHashmap< key_type, value_type
            , cc::hopscotch_hashmap::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::hopscotch_hashmap::refinable<>>
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::hopscotch_hashmap::probeset_type< cc::hopscotch_hashmap::vector<6>>
            >::type
        > map_type;

        typename map_type::hash_tuple_type ht;
        map_type m( std::move( ht ));
        test( m );
    }

    TEST_F( HopscotchHashmap, refinable_list_ordered_stat )
    {
        typedef cc::HopscotchHashmap< key_type, value_type
            , cc::hopscotch_hashmap::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::hopscotch_hashmap::refinable<>>
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cds::opt::stat< cc::hopscotch_hashmap::stat >
                ,cc::hopscotch_hashmap::probeset_type< cc::hopscotch_hashmap::list >
            >::type
        > map_type;

        map_type m;
        test( m );
    }

    TEST_F( HopscotchHashmap, refinable_vector_ordered_stat )
    {
        typedef cc::HopscotchHashmap< key_type, value_type
            , cc::hopscotch_hashmap::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::hopscotch_hashmap::refinable<>>
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cds::opt::stat< cc::hopscotch_hashmap::stat >
                ,cc::hopscotch_hashmap::probeset_type< cc::hopscotch_hashmap::vector<8>>
            >::type
        > map_type;

        map_type m;
        test( m );
    }

    TEST_F( HopscotchHashmap, refinable_list_unordered_storehash )
    {
        struct map_traits: public store_hash_traits
        {
            typedef cc::hopscotch_hashmap::refinable<> mutex_policy;
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef base_class::equal_to equal_to;
            typedef cc::hopscotch_hashmap::list     probeset_type;
            typedef cc::hopscotch_hashmap::stat     stat;
        };
        typedef cc::HopscotchHashmap< key_type, value_type, map_traits > map_type;

        map_type m;
        test( m );
    }

    TEST_F( HopscotchHashmap, refinable_vector_unordered_storehash )
    {
        struct map_traits: public store_hash_traits
        {
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef cc::hopscotch_hashmap::refinable<> mutex_policy;
            typedef base_class::equal_to    equal_to;
            typedef cc::hopscotch_hashmap::stat        stat;
            typedef cc::hopscotch_hashmap::vector<4>   probeset_type;
        };
        typedef cc::HopscotchHashmap< key_type, value_type, map_traits > map_type;

        map_type m( 32, 4 );
        test( m );
    }

    TEST_F( HopscotchHashmap, refinable_list_ordered_storehash )
    {
        typedef cc::HopscotchHashmap< key_type, value_type
            ,cc::hopscotch_hashmap::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::hopscotch_hashmap::refinable<>>
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::hopscotch_hashmap::probeset_type< cc::hopscotch_hashmap::list >
                ,cc::hopscotch_hashmap::store_hash< true >
            >::type
        > map_type;

        typename map_type::hash_tuple_type ht;
        map_type m( 32, 6, 0, std::move( ht ));
        test( m );
    }

    TEST_F( HopscotchHashmap, refinable_vector_ordered_storehash )
    {
        typedef cc::HopscotchHashmap< key_type, value_type
            ,cc::hopscotch_hashmap::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::hopscotch_hashmap::refinable<>>
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::hopscotch_hashmap::probeset_type< cc::hopscotch_hashmap::vector<6>>
                ,cc::hopscotch_hashmap::store_hash< true >
            >::type
        > map_type;

        typename map_type::hash_tuple_type ht;
        map_type m( std::move( ht ));
        test( m );
    }


} // namespace
