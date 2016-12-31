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

#include "test_intrusive_feldman_hashset_hp.h"

#include <cds/intrusive/feldman_hashset_hp.h>

namespace {
    namespace ci = cds::intrusive;
    typedef cds::gc::HP gc_type;

    class IntrusiveFeldmanHashSet_HP : public cds_test::intrusive_feldman_hashset_hp
    {
    protected:
        typedef cds_test::intrusive_feldman_hashset_hp base_class;

    protected:

        void SetUp()
        {
            typedef ci::FeldmanHashSet< gc_type, int_item,
                typename ci::feldman_hashset::make_traits<
                    ci::feldman_hashset::hash_accessor< hash_accessor >
                    ,ci::opt::less< std::less<int>>
                    ,ci::opt::disposer<mock_disposer>
                >::type
            > set_type;

            // +1 - for guarded_ptr
            cds::gc::hp::GarbageCollector::Construct( set_type::c_nHazardPtrCount + 1, 1, 16 );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::hp::GarbageCollector::Destruct( true );
        }
    };

    TEST_F( IntrusiveFeldmanHashSet_HP, compare )
    {
        struct traits : public ci::feldman_hashset::traits
        {
            typedef base_class::hash_accessor hash_accessor;
            typedef cmp compare;
            typedef mock_disposer disposer;
        };

        typedef ci::FeldmanHashSet< gc_type, int_item, traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( IntrusiveFeldmanHashSet_HP, less )
    {
        typedef ci::FeldmanHashSet< gc_type, int_item,
            typename ci::feldman_hashset::make_traits<
                ci::feldman_hashset::hash_accessor< hash_accessor >
                , ci::opt::less< std::less<int>>
                , ci::opt::disposer<mock_disposer>
            >::type
        > set_type;

        set_type s( 5, 2 );
        test( s );
    }

    TEST_F( IntrusiveFeldmanHashSet_HP, cmpmix )
    {
        struct traits : public ci::feldman_hashset::traits
        {
            typedef base_class::hash_accessor hash_accessor;
            typedef cmp compare;
            typedef std::less<int> less;
            typedef mock_disposer disposer;
            typedef simple_item_counter item_counter;
        };

        typedef ci::FeldmanHashSet< gc_type, int_item, traits > set_type;

        set_type s( 3, 4 );
        test( s );
    }

    TEST_F( IntrusiveFeldmanHashSet_HP, backoff )
    {
        struct traits : public ci::feldman_hashset::traits
        {
            typedef base_class::hash_accessor hash_accessor;
            typedef cmp compare;
            typedef mock_disposer disposer;
            typedef cds::backoff::empty back_off;
            typedef ci::opt::v::sequential_consistent memory_model;
        };

        typedef ci::FeldmanHashSet< gc_type, int_item, traits > set_type;

        set_type s( 8, 3 );
        test( s );
    }

    TEST_F( IntrusiveFeldmanHashSet_HP, stat )
    {
        struct traits : public ci::feldman_hashset::traits
        {
            typedef base_class::hash_accessor hash_accessor;
            typedef cmp compare;
            typedef mock_disposer disposer;
            typedef ci::feldman_hashset::stat<> stat;
        };

        typedef ci::FeldmanHashSet< gc_type, int_item, traits > set_type;

        set_type s( 8, 3 );
        test( s );
    }

    TEST_F( IntrusiveFeldmanHashSet_HP, explicit_hash_size )
    {
        struct traits: public ci::feldman_hashset::traits
        {
            typedef base_class::hash_accessor2 hash_accessor;
            enum: size_t {
                hash_size = sizeof( std::declval<key_val>().nKey )
            };
            typedef base_class::cmp2 compare;
            typedef mock_disposer disposer;
            typedef ci::feldman_hashset::stat<> stat;
        };

        typedef ci::FeldmanHashSet< gc_type, int_item2, traits > set_type;

        set_type s( 8, 3 );
        test( s );
    }

} // namespace
