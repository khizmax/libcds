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

#include "test_feldman_hashmap_hp.h"

#include <cds/container/feldman_hashmap_dhp.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::DHP gc_type;

    class FeldmanHashMap_DHP : public cds_test::feldman_hashmap_hp
    {
    protected:
        typedef cds_test::feldman_hashmap_hp base_class;

        void SetUp()
        {
            typedef cc::FeldmanHashMap< gc_type, key_type, value_type > map_type;

            // +1 - for guarded_ptr
            cds::gc::dhp::GarbageCollector::Construct( 16, map_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::dhp::GarbageCollector::Destruct();
        }
    };

    TEST_F( FeldmanHashMap_DHP, defaulted )
    {
        typedef cc::FeldmanHashMap< gc_type, key_type, value_type > map_type;

        map_type m;
        test( m );
    }

    TEST_F( FeldmanHashMap_DHP, compare )
    {
        typedef cc::FeldmanHashMap< gc_type, key_type, value_type,
            typename cc::feldman_hashmap::make_traits<
                cds::opt::compare< cmp >
            >::type
        > map_type;

        map_type m( 4, 5 );
        test( m );
    }

    TEST_F( FeldmanHashMap_DHP, less )
    {
        typedef cc::FeldmanHashMap< gc_type, key_type, value_type,
            typename cc::feldman_hashmap::make_traits<
                cds::opt::less< less >
            >::type
        > map_type;

        map_type m( 3, 2 );
        test( m );
    }

    TEST_F( FeldmanHashMap_DHP, cmpmix )
    {
        typedef cc::FeldmanHashMap< gc_type, key_type, value_type,
            typename cc::feldman_hashmap::make_traits<
                cds::opt::less< less >
                ,cds::opt::compare< cmp >
            >::type
        > map_type;

        map_type m( 4, 4 );
        test( m );
    }

    TEST_F( FeldmanHashMap_DHP, backoff )
    {
        struct map_traits: public cc::feldman_hashmap::traits
        {
            typedef cmp compare;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
        };
        typedef cc::FeldmanHashMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m( 8, 2 );
        test( m );
    }

    TEST_F( FeldmanHashMap_DHP, stat )
    {
        struct map_traits: public cc::feldman_hashmap::traits
        {
            typedef cds::backoff::yield back_off;
            typedef cc::feldman_hashmap::stat<> stat;
        };
        typedef cc::FeldmanHashMap< gc_type, key_type, value_type, map_traits > map_type;

        map_type m( 1, 1 );
        test( m );
    }

    TEST_F( FeldmanHashMap_DHP, explicit_key_size )
    {
        struct map_traits: public cc::feldman_hashmap::traits
        {
            enum: size_t {
                hash_size = sizeof( int ) + sizeof( uint16_t )
            };
            typedef hash2 hash;
            typedef less2 less;
            typedef cc::feldman_hashmap::stat<> stat;
        };
        typedef cc::FeldmanHashMap< gc_type, key_type2, value_type, map_traits > map_type;

        map_type m( 5, 3 );
        EXPECT_EQ( m.head_size(), static_cast<size_t>(1 << 6));
        EXPECT_EQ( m.array_node_size(), static_cast<size_t>(1 << 3));
        test( m );
    }

} // namespace
