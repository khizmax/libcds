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

#include "map/hdr_map.h"
#include <cds/container/michael_kvlist_hp.h>
#include <cds/container/michael_map.h>

namespace map {
    namespace {
        struct map_traits: public cc::michael_map::traits
        {
            typedef HashMapHdrTest::hash_int            hash;
            typedef HashMapHdrTest::simple_item_counter item_counter;
        };

        struct HP_cmp_traits: public cc::michael_list::traits
        {
            typedef HashMapHdrTest::cmp   compare;
        };

        struct HP_less_traits: public cc::michael_list::traits
        {
            typedef HashMapHdrTest::less  less;
        };

        struct HP_cmpmix_traits: public cc::michael_list::traits
        {
            typedef HashMapHdrTest::cmp   compare;
            typedef HashMapHdrTest::less  less;
        };
    }

    void HashMapHdrTest::Michael_HP_cmp()
    {
        typedef cc::MichaelKVList< cds::gc::HP, int, HashMapHdrTest::value_type, HP_cmp_traits > list;

        // traits-based version
        typedef cc::MichaelHashMap< cds::gc::HP, list, map_traits > map;
        test_int< map >();

        // option-based version
        typedef cc::MichaelHashMap< cds::gc::HP, list,
            cc::michael_map::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_map;
        test_int< opt_map >();
    }

    void HashMapHdrTest::Michael_HP_less()
    {
        typedef cc::MichaelKVList< cds::gc::HP, int, HashMapHdrTest::value_type, HP_less_traits > list;

        // traits-based version
        typedef cc::MichaelHashMap< cds::gc::HP, list, map_traits > map;
        test_int< map >();

        // option-based version
        typedef cc::MichaelHashMap< cds::gc::HP, list,
            cc::michael_map::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_map;
        test_int< opt_map >();
    }

    void HashMapHdrTest::Michael_HP_cmpmix()
    {
        typedef cc::MichaelKVList< cds::gc::HP, int, HashMapHdrTest::value_type, HP_cmpmix_traits > list;

        // traits-based version
        typedef cc::MichaelHashMap< cds::gc::HP, list, map_traits > map;
        test_int< map >();

        // option-based version
        typedef cc::MichaelHashMap< cds::gc::HP, list,
            cc::michael_map::make_traits<
                cc::opt::hash< hash_int >
                ,cc::opt::item_counter< simple_item_counter >
            >::type
        > opt_map;
        test_int< opt_map >();
    }


} // namespace map

CPPUNIT_TEST_SUITE_REGISTRATION(map::HashMapHdrTest);

