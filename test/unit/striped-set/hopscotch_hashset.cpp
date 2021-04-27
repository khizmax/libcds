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

#include "test_set.h"

#include <cds/container/hopscotch_hashset.h>

namespace {
    namespace cc = cds::container;

    class HopscotchHashset : public cds_test::container_set
    {
    protected:
        typedef cds_test::container_set base_class;

        template <typename Set>
        void test( Set& s )
        {
            // Precondition: set is empty
            // Postcondition: set is empty

            base_class::test_< Set::c_isSorted>( s );
        }

        //void SetUp()
        //{}

        //void TearDown()
        //{}
    };

    struct store_hash_traits: public cc::hopscotch_hashset::traits
    {
        static bool const store_hash = true;
    };


//************************************************************
// striped set

    TEST_F( HopscotchHashset, striped_list_unordered )
    {
        struct set_traits: public cc::hopscotch_hashset::traits
        {
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef base_class::equal_to equal_to;
            typedef cc::hopscotch_hashset::list probeset_type;
        };
        typedef cc::HopscotchHashset< int_item, set_traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( HopscotchHashset, striped_vector_unordered )
    {
        struct set_traits: public cc::hopscotch_hashset::traits
        {
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef base_class::equal_to equal_to;
            typedef cc::hopscotch_hashset::vector<4> probeset_type;
        };
        typedef cc::HopscotchHashset< int_item, set_traits > set_type;

        set_type s( 32, 4 );
        test( s );
    }

    TEST_F( HopscotchHashset, striped_list_ordered_cmp )
    {
        typedef cc::HopscotchHashset< int_item
            , cc::hopscotch_hashset::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::compare< cmp >
                ,cc::hopscotch_hashset::probeset_type< cc::hopscotch_hashset::list >
            >::type
        > set_type;

        set_type s( 32, 6, 4 );
        test( s );
    }

    TEST_F( HopscotchHashset, striped_vector_ordered_cmp )
    {
        typedef cc::HopscotchHashset< int_item
            , cc::hopscotch_hashset::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::compare< cmp >
                ,cc::hopscotch_hashset::probeset_type< cc::hopscotch_hashset::vector<8>>
            >::type
        > set_type;

        typename set_type::hash_tuple_type ht;
        set_type s( ht );
        test( s );
    }

    TEST_F( HopscotchHashset, striped_list_ordered_less )
    {
        typedef cc::HopscotchHashset< int_item
            , cc::hopscotch_hashset::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cc::hopscotch_hashset::probeset_type< cc::hopscotch_hashset::list >
            >::type
        > set_type;

        typename set_type::hash_tuple_type ht;
        set_type s( 32, 6, 4, ht );
        test( s );
    }

    TEST_F( HopscotchHashset, striped_vector_ordered_less )
    {
        typedef cc::HopscotchHashset< int_item
            , cc::hopscotch_hashset::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cc::hopscotch_hashset::probeset_type< cc::hopscotch_hashset::vector<6>>
            >::type
        > set_type;

        typename set_type::hash_tuple_type ht;
        set_type s( std::move( ht ));
        test( s );
    }

    TEST_F( HopscotchHashset, striped_list_ordered_cmpmix )
    {
        typedef cc::HopscotchHashset< int_item
            , cc::hopscotch_hashset::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::hopscotch_hashset::probeset_type< cc::hopscotch_hashset::list >
            >::type
        > set_type;

        typename set_type::hash_tuple_type ht;
        set_type s( 32, 6, 0, std::move( ht ));
        test( s );
    }

    TEST_F( HopscotchHashset, striped_vector_ordered_cmpmix )
    {
        typedef cc::HopscotchHashset< int_item
            , cc::hopscotch_hashset::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::hopscotch_hashset::probeset_type< cc::hopscotch_hashset::vector<6>>
            >::type
        > set_type;

        typename set_type::hash_tuple_type ht;
        set_type s( std::move( ht ));
        test( s );
    }

    TEST_F( HopscotchHashset, striped_list_ordered_stat )
    {
        typedef cc::HopscotchHashset< int_item
            , cc::hopscotch_hashset::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cds::opt::stat< cc::hopscotch_hashset::stat >
                ,cc::hopscotch_hashset::probeset_type< cc::hopscotch_hashset::list >
            >::type
        > set_type;

        set_type s;
        test( s );
    }

    TEST_F( HopscotchHashset, striped_vector_ordered_stat )
    {
        typedef cc::HopscotchHashset< int_item
            , cc::hopscotch_hashset::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cds::opt::stat< cc::hopscotch_hashset::stat >
                ,cc::hopscotch_hashset::probeset_type< cc::hopscotch_hashset::vector<8>>
            >::type
        > set_type;

        set_type s;
        test( s );
    }

    TEST_F( HopscotchHashset, striped_list_unordered_storehash )
    {
        struct set_traits: public store_hash_traits
        {
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef base_class::equal_to equal_to;
            typedef cc::hopscotch_hashset::list     probeset_type;
            typedef cc::hopscotch_hashset::stat     stat;
        };
        typedef cc::HopscotchHashset< int_item, set_traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( HopscotchHashset, striped_vector_unordered_storehash )
    {
        struct set_traits: public store_hash_traits
        {
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef base_class::equal_to    equal_to;
            typedef cc::hopscotch_hashset::stat        stat;
            typedef cc::hopscotch_hashset::vector<4>   probeset_type;
        };
        typedef cc::HopscotchHashset< int_item, set_traits > set_type;

        set_type s( 32, 4 );
        test( s );
    }

    TEST_F( HopscotchHashset, striped_list_ordered_storehash )
    {
        typedef cc::HopscotchHashset< int_item
            ,cc::hopscotch_hashset::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::hopscotch_hashset::probeset_type< cc::hopscotch_hashset::list >
                ,cc::hopscotch_hashset::store_hash< true >
            >::type
        > set_type;

        typename set_type::hash_tuple_type ht;
        set_type s( 32, 6, 0, std::move( ht ));
        test( s );
    }

    TEST_F( HopscotchHashset, striped_vector_ordered_storehash )
    {
        typedef cc::HopscotchHashset< int_item
            ,cc::hopscotch_hashset::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::hopscotch_hashset::probeset_type< cc::hopscotch_hashset::vector<6>>
                ,cc::hopscotch_hashset::store_hash< true >
            >::type
        > set_type;

        typename set_type::hash_tuple_type ht;
        set_type s( std::move( ht ));
        test( s );
    }


//************************************************************
// refinable set

    TEST_F( HopscotchHashset, refinable_list_unordered )
    {
        struct set_traits: public cc::hopscotch_hashset::traits
        {
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef base_class::equal_to equal_to;
            typedef cc::hopscotch_hashset::list probeset_type;
            typedef cc::hopscotch_hashset::refinable<> mutex_policy;
        };
        typedef cc::HopscotchHashset< int_item, set_traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( HopscotchHashset, refinable_vector_unordered )
    {
        struct set_traits: public cc::hopscotch_hashset::traits
        {
            typedef cc::hopscotch_hashset::refinable<> mutex_policy;
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef base_class::equal_to equal_to;
            typedef cc::hopscotch_hashset::vector<4> probeset_type;
        };
        typedef cc::HopscotchHashset< int_item, set_traits > set_type;

        set_type s( 32, 4 );
        test( s );
    }

    TEST_F( HopscotchHashset, refinable_list_ordered_cmp )
    {
        typedef cc::HopscotchHashset< int_item
            , cc::hopscotch_hashset::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::hopscotch_hashset::refinable<>>
                ,cds::opt::compare< cmp >
                ,cc::hopscotch_hashset::probeset_type< cc::hopscotch_hashset::list >
            >::type
        > set_type;

        set_type s( 32, 6, 4 );
        test( s );
    }

    TEST_F( HopscotchHashset, refinable_vector_ordered_cmp )
    {
        typedef cc::HopscotchHashset< int_item
            , cc::hopscotch_hashset::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 >>
                ,cds::opt::mutex_policy< cc::hopscotch_hashset::refinable<>>
                ,cds::opt::compare< cmp >
                ,cc::hopscotch_hashset::probeset_type< cc::hopscotch_hashset::vector<8>>
            >::type
        > set_type;

        typename set_type::hash_tuple_type ht;
        set_type s( ht );
        test( s );
    }

    TEST_F( HopscotchHashset, refinable_list_ordered_less )
    {
        typedef cc::HopscotchHashset< int_item
            , cc::hopscotch_hashset::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::hopscotch_hashset::refinable<>>
                ,cds::opt::less< less >
                ,cc::hopscotch_hashset::probeset_type< cc::hopscotch_hashset::list >
            >::type
        > set_type;

        typename set_type::hash_tuple_type ht;
        set_type s( 32, 6, 4, ht );
        test( s );
    }

    TEST_F( HopscotchHashset, refinable_vector_ordered_less )
    {
        typedef cc::HopscotchHashset< int_item
            , cc::hopscotch_hashset::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::hopscotch_hashset::refinable<>>
                ,cds::opt::less< less >
                ,cc::hopscotch_hashset::probeset_type< cc::hopscotch_hashset::vector<6>>
            >::type
        > set_type;

        typename set_type::hash_tuple_type ht;
        set_type s( std::move( ht ));
        test( s );
    }

    TEST_F( HopscotchHashset, refinable_list_ordered_cmpmix )
    {
        typedef cc::HopscotchHashset< int_item
            , cc::hopscotch_hashset::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::hopscotch_hashset::refinable<>>
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::hopscotch_hashset::probeset_type< cc::hopscotch_hashset::list >
            >::type
        > set_type;

        typename set_type::hash_tuple_type ht;
        set_type s( 32, 6, 0, std::move( ht ));
        test( s );
    }

    TEST_F( HopscotchHashset, refinable_vector_ordered_cmpmix )
    {
        typedef cc::HopscotchHashset< int_item
            , cc::hopscotch_hashset::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::hopscotch_hashset::refinable<>>
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::hopscotch_hashset::probeset_type< cc::hopscotch_hashset::vector<6>>
            >::type
        > set_type;

        typename set_type::hash_tuple_type ht;
        set_type s( std::move( ht ));
        test( s );
    }

    TEST_F( HopscotchHashset, refinable_list_ordered_stat )
    {
        typedef cc::HopscotchHashset< int_item
            , cc::hopscotch_hashset::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::hopscotch_hashset::refinable<>>
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cds::opt::stat< cc::hopscotch_hashset::stat >
                ,cc::hopscotch_hashset::probeset_type< cc::hopscotch_hashset::list >
            >::type
        > set_type;

        set_type s;
        test( s );
    }

    TEST_F( HopscotchHashset, refinable_vector_ordered_stat )
    {
        typedef cc::HopscotchHashset< int_item
            , cc::hopscotch_hashset::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::hopscotch_hashset::refinable<>>
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cds::opt::stat< cc::hopscotch_hashset::stat >
                ,cc::hopscotch_hashset::probeset_type< cc::hopscotch_hashset::vector<8>>
            >::type
        > set_type;

        set_type s;
        test( s );
    }

    TEST_F( HopscotchHashset, refinable_list_unordered_storehash )
    {
        struct set_traits: public store_hash_traits
        {
            typedef cc::hopscotch_hashset::refinable<> mutex_policy;
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef base_class::equal_to equal_to;
            typedef cc::hopscotch_hashset::list     probeset_type;
            typedef cc::hopscotch_hashset::stat     stat;
        };
        typedef cc::HopscotchHashset< int_item, set_traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( HopscotchHashset, refinable_vector_unordered_storehash )
    {
        struct set_traits: public store_hash_traits
        {
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef cc::hopscotch_hashset::refinable<> mutex_policy;
            typedef base_class::equal_to    equal_to;
            typedef cc::hopscotch_hashset::stat        stat;
            typedef cc::hopscotch_hashset::vector<4>   probeset_type;
        };
        typedef cc::HopscotchHashset< int_item, set_traits > set_type;

        set_type s( 32, 4 );
        test( s );
    }

    TEST_F( HopscotchHashset, refinable_list_ordered_storehash )
    {
        typedef cc::HopscotchHashset< int_item
            ,cc::hopscotch_hashset::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::hopscotch_hashset::refinable<>>
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::hopscotch_hashset::probeset_type< cc::hopscotch_hashset::list >
                ,cc::hopscotch_hashset::store_hash< true >
            >::type
        > set_type;

        typename set_type::hash_tuple_type ht;
        set_type s( 32, 6, 0, std::move( ht ));
        test( s );
    }

    TEST_F( HopscotchHashset, refinable_vector_ordered_storehash )
    {
        typedef cc::HopscotchHashset< int_item
            ,cc::hopscotch_hashset::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::hopscotch_hashset::refinable<>>
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::hopscotch_hashset::probeset_type< cc::hopscotch_hashset::vector<6>>
                ,cc::hopscotch_hashset::store_hash< true >
            >::type
        > set_type;

        typename set_type::hash_tuple_type ht;
        set_type s( std::move( ht ));
        test( s );
    }


} // namespace
