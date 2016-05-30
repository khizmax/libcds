/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

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

#include "test_map_hp.h"

#include <cds/container/lazy_list_dhp.h>
#include <cds/container/split_list_map.h>

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

            cds::gc::dhp::GarbageCollector::Construct( 16, map_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::dhp::GarbageCollector::Destruct();
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

} // namespace
