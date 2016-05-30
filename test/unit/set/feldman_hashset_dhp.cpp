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

#include "test_feldman_hashset_hp.h"

#include <cds/container/feldman_hashset_dhp.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::DHP gc_type;

    class FeldmanHashSet_DHP : public cds_test::feldman_hashset_hp
    {
    protected:
        typedef cds_test::feldman_hashset_hp base_class;

        void SetUp()
        {
            struct set_traits: public cc::feldman_hashset::traits
            {
                typedef get_hash hash_accessor;
            };
            typedef cc::FeldmanHashSet< gc_type, int_item, set_traits > set_type;

            cds::gc::dhp::GarbageCollector::Construct( 16, set_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::dhp::GarbageCollector::Destruct();
        }
    };

    TEST_F( FeldmanHashSet_DHP, defaulted )
    {
        typedef cc::FeldmanHashSet< gc_type, int_item,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash >
            >::type
        > set_type;

        set_type s;
        test( s );
    }

    TEST_F( FeldmanHashSet_DHP, compare )
    {
        typedef cc::FeldmanHashSet< gc_type, int_item,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash >
                ,cds::opt::compare< cmp >
            >::type
        > set_type;

        set_type s( 4, 5 );
        test( s );
    }

    TEST_F( FeldmanHashSet_DHP, less )
    {
        typedef cc::FeldmanHashSet< gc_type, int_item,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash >
                ,cds::opt::less< std::less<int> >
            >::type
        > set_type;

        set_type s( 3, 2 );
        test( s );
    }

    TEST_F( FeldmanHashSet_DHP, cmpmix )
    {
        typedef cc::FeldmanHashSet< gc_type, int_item,
            typename cc::feldman_hashset::make_traits<
                cc::feldman_hashset::hash_accessor< get_hash >
                , cds::opt::less< std::less<int> >
                ,cds::opt::compare< cmp >
            >::type
        > set_type;

        set_type s( 4, 4 );
        test( s );
    }

    TEST_F( FeldmanHashSet_DHP, item_counting )
    {
        struct set_traits: public cc::feldman_hashset::traits
        {
            typedef get_hash hash_accessor;
            typedef cmp compare;
            typedef std::less<int> less;
            typedef simple_item_counter item_counter;
        };
        typedef cc::FeldmanHashSet< gc_type, int_item, set_traits > set_type;

        set_type s( 3, 3 );
        test( s );
    }

    TEST_F( FeldmanHashSet_DHP, backoff )
    {
        struct set_traits: public cc::feldman_hashset::traits
        {
            typedef get_hash hash_accessor;
            typedef cmp compare;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
        };
        typedef cc::FeldmanHashSet< gc_type, int_item, set_traits > set_type;

        set_type s( 8, 2 );
        test( s );
    }

    TEST_F( FeldmanHashSet_DHP, stat )
    {
        struct set_traits: public cc::feldman_hashset::traits
        {
            typedef get_hash hash_accessor;
            typedef cds::backoff::yield back_off;
            typedef cc::feldman_hashset::stat<> stat;
        };
        typedef cc::FeldmanHashSet< gc_type, int_item, set_traits > set_type;

        set_type s( 1, 1 );
        test( s );
    }

} // namespace
