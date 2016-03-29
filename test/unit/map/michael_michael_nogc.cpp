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

#include "test_map_nogc.h"

#include <cds/container/michael_kvlist_nogc.h>
#include <cds/container/michael_map_nogc.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::nogc gc_type;

    class MichaelMap_NoGC : public cds_test::container_map_nogc
    {
    protected:
        typedef cds_test::container_map_nogc base_class;

        //void SetUp()
        //{}

        //void TearDown()
        //{}
    };

    TEST_F( MichaelMap_NoGC, compare )
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

    TEST_F( MichaelMap_NoGC, less )
    {
        typedef cc::MichaelKVList< gc_type, key_type, value_type,
            typename cc::michael_list::make_traits<
                cds::opt::less< base_class::less >
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

    TEST_F( MichaelMap_NoGC, cmpmix )
    {
        struct list_traits : public cc::michael_list::traits
        {
            typedef base_class::less less;
            typedef cmp compare;
        };
        typedef cc::MichaelKVList< gc_type, key_type, value_type, list_traits > list_type;

        typedef cc::MichaelHashMap< gc_type, list_type, 
            typename cc::michael_map::make_traits<
                cds::opt::hash< hash1 >
            >::type
        > map_type;

        map_type m( kSize, 2 );
        test( m );
    }

    TEST_F( MichaelMap_NoGC, backoff )
    {
        struct list_traits : public cc::michael_list::traits
        {
            typedef cmp compare;
            typedef cds::backoff::exponential<cds::backoff::pause, cds::backoff::yield> back_off;
        };
        typedef cc::MichaelKVList< gc_type, key_type, value_type, list_traits > list_type;

        struct map_traits : public cc::michael_map::traits
        {
            typedef hash1 hash;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::MichaelHashMap< gc_type, list_type, map_traits > map_type;

        map_type m( kSize, 4 );
        test( m );
    }

    TEST_F( MichaelMap_NoGC, seq_cst )
    {
        struct list_traits : public cc::michael_list::traits
        {
            typedef base_class::less less;
            typedef cds::backoff::pause back_off;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef cc::MichaelKVList< gc_type, key_type, value_type, list_traits > list_type;

        struct map_traits : public cc::michael_map::traits
        {
            typedef hash1 hash;
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef cc::MichaelHashMap< gc_type, list_type, map_traits > map_type;

        map_type m( kSize, 4 );
        test( m );
    }

} // namespace
