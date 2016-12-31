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

#include "test_tree_set_hp.h"

#include <cds/container/ellen_bintree_set_dhp.h>
#include "test_ellen_bintree_update_desc_pool.h"

namespace {
    namespace cc = cds::container;
    typedef cds::gc::DHP gc_type;

    class EllenBinTreeSet_DHP : public cds_test::container_tree_set_hp
    {
    protected:
        typedef cds_test::container_tree_set_hp base_class;
        typedef int key_type;

        struct generic_traits: public cc::ellen_bintree::traits
        {
            typedef base_class::key_extractor key_extractor;
        };

        void SetUp()
        {
            typedef cc::EllenBinTreeSet< gc_type, key_type, int_item > set_type;

            cds::gc::dhp::GarbageCollector::Construct( 16, set_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::hp::GarbageCollector::Destruct();
        }
    };


    TEST_F( EllenBinTreeSet_DHP, compare )
    {
        typedef cc::EllenBinTreeSet< gc_type, key_type, int_item,
            typename cc::ellen_bintree::make_set_traits<
                cc::opt::type_traits< generic_traits >
                ,cds::opt::compare< cmp >
            >::type
        > set_type;

        set_type s;
        test( s );
    }

    TEST_F( EllenBinTreeSet_DHP, less )
    {
        typedef cc::EllenBinTreeSet< gc_type, key_type, int_item,
            typename cc::ellen_bintree::make_set_traits<
                cc::opt::type_traits< generic_traits >
                ,cds::opt::less< base_class::less >
            >::type
        > set_type;

        set_type s;
        test( s );
    }

    TEST_F( EllenBinTreeSet_DHP, cmpmix )
    {
        typedef cc::EllenBinTreeSet< gc_type, key_type, int_item,
            typename cc::ellen_bintree::make_set_traits<
                cc::opt::type_traits< generic_traits >
                ,cds::opt::less< base_class::less >
                ,cds::opt::compare< cmp >
            >::type
        > set_type;

        set_type s;
        test( s );
    }

    TEST_F( EllenBinTreeSet_DHP, update_desc_pool )
    {
        struct set_traits: public generic_traits
        {
            typedef cmp compare;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::pool_accessor> update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< gc_type, key_type, int_item, set_traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( EllenBinTreeSet_DHP, update_desc_lazy_pool )
    {
        struct set_traits: public generic_traits
        {
            typedef cmp compare;
            typedef cds::memory::pool_allocator< cds_test::update_desc, cds_test::lazy_pool_accessor> update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< gc_type, key_type, int_item, set_traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( EllenBinTreeSet_DHP, item_counting )
    {
        struct set_traits: public generic_traits
        {
            typedef cmp compare;
            typedef base_class::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::pool_accessor> update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< gc_type, key_type, int_item, set_traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( EllenBinTreeSet_DHP, backoff )
    {
        struct set_traits: public generic_traits
        {
            typedef cmp compare;
            typedef base_class::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::lazy_pool_accessor> update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< gc_type, key_type, int_item, set_traits > set_type;

        set_type s;
        test( s );
    }

    TEST_F( EllenBinTreeSet_DHP, stat )
    {
        struct set_traits: public generic_traits
        {
            typedef base_class::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
            typedef cc::ellen_bintree::stat<> stat;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::pool_accessor> update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< gc_type, key_type, int_item, set_traits > set_type;

        set_type s;
        test( s );
    }

} // namespace
