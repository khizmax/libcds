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

#include "test_set_nogc.h"

#include <cds/container/lazy_list_nogc.h>
#include <cds/container/split_list_set_nogc.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::nogc gc_type;

    class SplitListLazySet_NoGC : public cds_test::container_set_nogc
    {
    protected:
        typedef cds_test::container_set_nogc base_class;

        //void SetUp()
        //{}

        //void TearDown()
        //{}
    };

    TEST_F( SplitListLazySet_NoGC, compare )
    {
        typedef cc::SplitListSet< gc_type, int_item, 
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list< cc::lazy_list_tag >
                , cds::opt::hash< hash_int >
                , cc::split_list::ordered_list_traits< 
                    typename cc::lazy_list::make_traits<
                        cds::opt::compare< cmp >
                    >::type
                >
            >::type
        > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( SplitListLazySet_NoGC, less )
    {
        typedef cc::SplitListSet< gc_type, int_item, 
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list< cc::lazy_list_tag >
                , cds::opt::hash< hash_int >
                , cc::split_list::ordered_list_traits< 
                    typename cc::lazy_list::make_traits<
                        cds::opt::less< less >
                    >::type
                >
            >::type
        > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( SplitListLazySet_NoGC, cmpmix )
    {
        typedef cc::SplitListSet< gc_type, int_item, 
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list< cc::lazy_list_tag >
                , cds::opt::hash< hash_int >
                , cc::split_list::ordered_list_traits< 
                    typename cc::lazy_list::make_traits<
                        cds::opt::less< less >
                        , cds::opt::compare< cmp >
                    >::type
                >
            >::type
        > set_type;

        set_type s( kSize, 1 );
        test( s );
    }

    TEST_F( SplitListLazySet_NoGC, item_counting )
    {
        struct set_traits: public cc::split_list::traits
        {
            typedef cc::lazy_list_tag ordered_list;
            typedef hash_int hash;
            typedef cds::atomicity::item_counter item_counter;

            struct ordered_list_traits: public cc::lazy_list::traits
            {
                typedef cmp compare;
                typedef base_class::less less;
                typedef cds::backoff::empty back_off;
            };
        };
        typedef cc::SplitListSet< gc_type, int_item, set_traits > set_type;

        set_type s( kSize, 3 );
        test( s );
    }

    TEST_F( SplitListLazySet_NoGC, stat )
    {
        struct set_traits: public cc::split_list::traits
        {
            typedef cc::lazy_list_tag ordered_list;
            typedef hash_int hash;
            typedef cds::atomicity::item_counter item_counter;
            typedef cc::split_list::stat<> stat;

            struct ordered_list_traits: public cc::lazy_list::traits
            {
                typedef base_class::less less;
                typedef cds::opt::v::sequential_consistent memory_model;
            };
        };
        typedef cc::SplitListSet< gc_type, int_item, set_traits > set_type;

        set_type s( kSize, 4 );
        test( s );
    }

    TEST_F( SplitListLazySet_NoGC, back_off )
    {
        struct set_traits: public cc::split_list::traits
        {
            typedef cc::lazy_list_tag ordered_list;
            typedef hash_int hash;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
            typedef cds::opt::v::sequential_consistent memory_model;

            struct ordered_list_traits: public cc::lazy_list::traits
            {
                typedef cmp compare;
                typedef cds::backoff::empty back_off;
            };
        };
        typedef cc::SplitListSet< gc_type, int_item, set_traits > set_type;

        set_type s( kSize, 2 );
        test( s );
    }

    TEST_F( SplitListLazySet_NoGC, mutex )
    {
        struct set_traits: public cc::split_list::traits
        {
            typedef cc::lazy_list_tag ordered_list;
            typedef hash_int hash;
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
        typedef cc::SplitListSet< gc_type, int_item, set_traits > set_type;

        set_type s( kSize, 3 );
        test( s );
    }

    struct set_static_traits: public cc::split_list::traits
    {
        static bool const dynamic_bucket_table = false;
    };

    TEST_F( SplitListLazySet_NoGC, static_bucket_table )
    {
        struct set_traits: public set_static_traits
        {
            typedef cc::lazy_list_tag ordered_list;
            typedef hash_int hash;
            typedef cds::atomicity::item_counter item_counter;

            struct ordered_list_traits: public cc::lazy_list::traits
            {
                typedef cmp compare;
                typedef cds::backoff::pause back_off;
            };
        };
        typedef cc::SplitListSet< gc_type, int_item, set_traits > set_type;

        set_type s( kSize, 4 );
        test( s );
    }


} // namespace
