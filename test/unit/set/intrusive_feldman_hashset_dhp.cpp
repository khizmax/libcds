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

#include "test_intrusive_feldman_hashset_hp.h"

#include <cds/intrusive/feldman_hashset_dhp.h>

namespace {
    namespace ci = cds::intrusive;
    typedef cds::gc::DHP gc_type;

    class IntrusiveFeldmanHashSet_DHP : public cds_test::intrusive_feldman_hashset_hp
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

            cds::gc::dhp::GarbageCollector::Construct( 16, set_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::dhp::GarbageCollector::Destruct();
        }
    };

    TEST_F( IntrusiveFeldmanHashSet_DHP, compare )
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

    TEST_F( IntrusiveFeldmanHashSet_DHP, less )
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

    TEST_F( IntrusiveFeldmanHashSet_DHP, cmpmix )
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

    TEST_F( IntrusiveFeldmanHashSet_DHP, backoff )
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

    TEST_F( IntrusiveFeldmanHashSet_DHP, stat )
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

} // namespace
