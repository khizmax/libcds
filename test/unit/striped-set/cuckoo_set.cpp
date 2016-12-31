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

#include <cds/container/cuckoo_set.h>

namespace {
    namespace cc = cds::container;

    class CuckooSet : public cds_test::container_set
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

    struct store_hash_traits: public cc::cuckoo::traits
    {
        static bool const store_hash = true;
    };


//************************************************************
// striped set

    TEST_F( CuckooSet, striped_list_unordered )
    {
        struct set_traits: public cc::cuckoo::traits
        {
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef base_class::equal_to equal_to;
            typedef cc::cuckoo::list probeset_type;
        };
        typedef cc::CuckooSet< int_item, set_traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( CuckooSet, striped_vector_unordered )
    {
        struct set_traits: public cc::cuckoo::traits
        {
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef base_class::equal_to equal_to;
            typedef cc::cuckoo::vector<4> probeset_type;
        };
        typedef cc::CuckooSet< int_item, set_traits > set_type;

        set_type s( 32, 4 );
        test( s );
    }

    TEST_F( CuckooSet, striped_list_ordered_cmp )
    {
        typedef cc::CuckooSet< int_item
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::compare< cmp >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
            >::type
        > set_type;

        set_type s( 32, 6, 4 );
        test( s );
    }

    TEST_F( CuckooSet, striped_vector_ordered_cmp )
    {
        typedef cc::CuckooSet< int_item
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::compare< cmp >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<8>>
            >::type
        > set_type;

        typename set_type::hash_tuple_type ht;
        set_type s( ht );
        test( s );
    }

    TEST_F( CuckooSet, striped_list_ordered_less )
    {
        typedef cc::CuckooSet< int_item
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
            >::type
        > set_type;

        typename set_type::hash_tuple_type ht;
        set_type s( 32, 6, 4, ht );
        test( s );
    }

    TEST_F( CuckooSet, striped_vector_ordered_less )
    {
        typedef cc::CuckooSet< int_item
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<6>>
            >::type
        > set_type;

        typename set_type::hash_tuple_type ht;
        set_type s( std::move( ht ));
        test( s );
    }

    TEST_F( CuckooSet, striped_list_ordered_cmpmix )
    {
        typedef cc::CuckooSet< int_item
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
            >::type
        > set_type;

        typename set_type::hash_tuple_type ht;
        set_type s( 32, 6, 0, std::move( ht ));
        test( s );
    }

    TEST_F( CuckooSet, striped_vector_ordered_cmpmix )
    {
        typedef cc::CuckooSet< int_item
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<6>>
            >::type
        > set_type;

        typename set_type::hash_tuple_type ht;
        set_type s( std::move( ht ));
        test( s );
    }

    TEST_F( CuckooSet, striped_list_ordered_stat )
    {
        typedef cc::CuckooSet< int_item
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cds::opt::stat< cc::cuckoo::stat >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
            >::type
        > set_type;

        set_type s;
        test( s );
    }

    TEST_F( CuckooSet, striped_vector_ordered_stat )
    {
        typedef cc::CuckooSet< int_item
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cds::opt::stat< cc::cuckoo::stat >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<8>>
            >::type
        > set_type;

        set_type s;
        test( s );
    }

    TEST_F( CuckooSet, striped_list_unordered_storehash )
    {
        struct set_traits: public store_hash_traits
        {
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef base_class::equal_to equal_to;
            typedef cc::cuckoo::list     probeset_type;
            typedef cc::cuckoo::stat     stat;
        };
        typedef cc::CuckooSet< int_item, set_traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( CuckooSet, striped_vector_unordered_storehash )
    {
        struct set_traits: public store_hash_traits
        {
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef base_class::equal_to    equal_to;
            typedef cc::cuckoo::stat        stat;
            typedef cc::cuckoo::vector<4>   probeset_type;
        };
        typedef cc::CuckooSet< int_item, set_traits > set_type;

        set_type s( 32, 4 );
        test( s );
    }

    TEST_F( CuckooSet, striped_list_ordered_storehash )
    {
        typedef cc::CuckooSet< int_item
            ,cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                ,cc::cuckoo::store_hash< true >
            >::type
        > set_type;

        typename set_type::hash_tuple_type ht;
        set_type s( 32, 6, 0, std::move( ht ));
        test( s );
    }

    TEST_F( CuckooSet, striped_vector_ordered_storehash )
    {
        typedef cc::CuckooSet< int_item
            ,cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<6>>
                ,cc::cuckoo::store_hash< true >
            >::type
        > set_type;

        typename set_type::hash_tuple_type ht;
        set_type s( std::move( ht ));
        test( s );
    }


//************************************************************
// refinable set

    TEST_F( CuckooSet, refinable_list_unordered )
    {
        struct set_traits: public cc::cuckoo::traits
        {
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef base_class::equal_to equal_to;
            typedef cc::cuckoo::list probeset_type;
            typedef cc::cuckoo::refinable<> mutex_policy;
        };
        typedef cc::CuckooSet< int_item, set_traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( CuckooSet, refinable_vector_unordered )
    {
        struct set_traits: public cc::cuckoo::traits
        {
            typedef cc::cuckoo::refinable<> mutex_policy;
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef base_class::equal_to equal_to;
            typedef cc::cuckoo::vector<4> probeset_type;
        };
        typedef cc::CuckooSet< int_item, set_traits > set_type;

        set_type s( 32, 4 );
        test( s );
    }

    TEST_F( CuckooSet, refinable_list_ordered_cmp )
    {
        typedef cc::CuckooSet< int_item
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::cuckoo::refinable<>>
                ,cds::opt::compare< cmp >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
            >::type
        > set_type;

        set_type s( 32, 6, 4 );
        test( s );
    }

    TEST_F( CuckooSet, refinable_vector_ordered_cmp )
    {
        typedef cc::CuckooSet< int_item
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 >>
                ,cds::opt::mutex_policy< cc::cuckoo::refinable<>>
                ,cds::opt::compare< cmp >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<8>>
            >::type
        > set_type;

        typename set_type::hash_tuple_type ht;
        set_type s( ht );
        test( s );
    }

    TEST_F( CuckooSet, refinable_list_ordered_less )
    {
        typedef cc::CuckooSet< int_item
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::cuckoo::refinable<>>
                ,cds::opt::less< less >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
            >::type
        > set_type;

        typename set_type::hash_tuple_type ht;
        set_type s( 32, 6, 4, ht );
        test( s );
    }

    TEST_F( CuckooSet, refinable_vector_ordered_less )
    {
        typedef cc::CuckooSet< int_item
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::cuckoo::refinable<>>
                ,cds::opt::less< less >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<6>>
            >::type
        > set_type;

        typename set_type::hash_tuple_type ht;
        set_type s( std::move( ht ));
        test( s );
    }

    TEST_F( CuckooSet, refinable_list_ordered_cmpmix )
    {
        typedef cc::CuckooSet< int_item
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::cuckoo::refinable<>>
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
            >::type
        > set_type;

        typename set_type::hash_tuple_type ht;
        set_type s( 32, 6, 0, std::move( ht ));
        test( s );
    }

    TEST_F( CuckooSet, refinable_vector_ordered_cmpmix )
    {
        typedef cc::CuckooSet< int_item
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::cuckoo::refinable<>>
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<6>>
            >::type
        > set_type;

        typename set_type::hash_tuple_type ht;
        set_type s( std::move( ht ));
        test( s );
    }

    TEST_F( CuckooSet, refinable_list_ordered_stat )
    {
        typedef cc::CuckooSet< int_item
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::cuckoo::refinable<>>
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cds::opt::stat< cc::cuckoo::stat >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
            >::type
        > set_type;

        set_type s;
        test( s );
    }

    TEST_F( CuckooSet, refinable_vector_ordered_stat )
    {
        typedef cc::CuckooSet< int_item
            , cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::cuckoo::refinable<>>
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cds::opt::stat< cc::cuckoo::stat >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<8>>
            >::type
        > set_type;

        set_type s;
        test( s );
    }

    TEST_F( CuckooSet, refinable_list_unordered_storehash )
    {
        struct set_traits: public store_hash_traits
        {
            typedef cc::cuckoo::refinable<> mutex_policy;
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef base_class::equal_to equal_to;
            typedef cc::cuckoo::list     probeset_type;
            typedef cc::cuckoo::stat     stat;
        };
        typedef cc::CuckooSet< int_item, set_traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( CuckooSet, refinable_vector_unordered_storehash )
    {
        struct set_traits: public store_hash_traits
        {
            typedef cds::opt::hash_tuple< hash1, hash2 > hash;
            typedef cc::cuckoo::refinable<> mutex_policy;
            typedef base_class::equal_to    equal_to;
            typedef cc::cuckoo::stat        stat;
            typedef cc::cuckoo::vector<4>   probeset_type;
        };
        typedef cc::CuckooSet< int_item, set_traits > set_type;

        set_type s( 32, 4 );
        test( s );
    }

    TEST_F( CuckooSet, refinable_list_ordered_storehash )
    {
        typedef cc::CuckooSet< int_item
            ,cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::cuckoo::refinable<>>
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::cuckoo::probeset_type< cc::cuckoo::list >
                ,cc::cuckoo::store_hash< true >
            >::type
        > set_type;

        typename set_type::hash_tuple_type ht;
        set_type s( 32, 6, 0, std::move( ht ));
        test( s );
    }

    TEST_F( CuckooSet, refinable_vector_ordered_storehash )
    {
        typedef cc::CuckooSet< int_item
            ,cc::cuckoo::make_traits<
                cds::opt::hash< std::tuple< hash1, hash2 > >
                ,cds::opt::mutex_policy< cc::cuckoo::refinable<>>
                ,cds::opt::less< less >
                ,cds::opt::compare< cmp >
                ,cc::cuckoo::probeset_type< cc::cuckoo::vector<6>>
                ,cc::cuckoo::store_hash< true >
            >::type
        > set_type;

        typename set_type::hash_tuple_type ht;
        set_type s( std::move( ht ));
        test( s );
    }


} // namespace
