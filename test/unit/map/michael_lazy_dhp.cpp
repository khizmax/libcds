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

#include <cds/container/lazy_kvlist_dhp.h>
#include <cds/container/michael_map.h>

namespace {

    namespace cc = cds::container;
    typedef cds::gc::DHP gc_type;

    class MichaelLazyMap_DHP: public cds_test::container_map_hp
    {
    protected:
        typedef cds_test::container_map_hp base_class;

        void SetUp()
        {
            typedef cc::LazyKVList< gc_type, key_type, value_type > list_type;
            typedef cc::MichaelHashMap< gc_type, list_type >   map_type;

            cds::gc::dhp::GarbageCollector::Construct( 16, map_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::dhp::GarbageCollector::Destruct();
        }
    };

    TEST_F( MichaelLazyMap_DHP, compare )
    {
        typedef cc::LazyKVList< gc_type, key_type, value_type,
            typename cc::lazy_list::make_traits<
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

    TEST_F( MichaelLazyMap_DHP, less )
    {
        typedef cc::LazyKVList< gc_type, key_type, value_type,
            typename cc::lazy_list::make_traits<
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

    TEST_F( MichaelLazyMap_DHP, cmpmix )
    {
        typedef cc::LazyKVList< gc_type, key_type, value_type,
            typename cc::lazy_list::make_traits<
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

    TEST_F( MichaelLazyMap_DHP, backoff )
    {
        struct list_traits: public cc::lazy_list::traits
        {
            typedef cmp compare;
            typedef cds::backoff::exponential<cds::backoff::pause, cds::backoff::yield> back_off;
        };
        typedef cc::LazyKVList< gc_type, key_type, value_type, list_traits > list_type;

        struct map_traits: public cc::michael_map::traits
        {
            typedef hash1 hash;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::MichaelHashMap< gc_type, list_type, map_traits > map_type;

        map_type m( kSize, 4 );
        test( m );
    }

    TEST_F( MichaelLazyMap_DHP, seq_cst )
    {
        struct list_traits: public cc::lazy_list::traits
        {
            typedef cmp compare;
            typedef cds::backoff::yield back_off;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef cc::LazyKVList< gc_type, key_type, value_type, list_traits > list_type;

        struct map_traits: public cc::michael_map::traits
        {
            typedef hash1 hash;
        };
        typedef cc::MichaelHashMap< gc_type, list_type, map_traits > map_type;

        map_type m( kSize, 8 );
        test( m );
    }

    TEST_F( MichaelLazyMap_DHP, mutex )
    {
        struct list_traits: public cc::lazy_list::traits
        {
            typedef cmp                 compare;
            typedef cds::backoff::yield back_off;
            typedef std::mutex          lock_type;
        };
        typedef cc::LazyKVList< gc_type, key_type, value_type, list_traits > list_type;

        struct map_traits: public cc::michael_map::traits
        {
            typedef hash1 hash;
        };
        typedef cc::MichaelHashMap< gc_type, list_type, map_traits > map_type;

        map_type m( kSize, 2 );
        test( m );
    }

} // namespace

