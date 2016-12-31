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
#ifndef CDSUNIT_TREE_TEST_ELLEN_BINTREE_SET_RCU_H
#define CDSUNIT_TREE_TEST_ELLEN_BINTREE_SET_RCU_H

#include "test_tree_set_rcu.h"
#include <cds/container/ellen_bintree_set_rcu.h>
#include "test_ellen_bintree_update_desc_pool.h"


namespace {
    namespace cc = cds::container;

    template <class RCU>
    class EllenBinTreeSet: public cds_test::container_tree_set_rcu
    {
        typedef cds_test::container_tree_set_rcu base_class;
    public:
        typedef cds::urcu::gc<RCU> rcu_type;

    protected:
        void SetUp()
        {
            RCU::Construct();
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            RCU::Destruct();
        }
    };

    TYPED_TEST_CASE_P( EllenBinTreeSet );

    TYPED_TEST_P( EllenBinTreeSet, compare )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::int_item int_item;
        typedef typename TestFixture::key_extractor key_extractor;

        typedef cc::EllenBinTreeSet< rcu_type, int, int_item,
            typename cc::ellen_bintree::make_set_traits<
                cds::opt::compare< typename TestFixture::cmp >
                ,cc::ellen_bintree::key_extractor< key_extractor >
            >::type
        > set_type;

        set_type s;
        this->test( s );
    }

    TYPED_TEST_P( EllenBinTreeSet, less )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::int_item int_item;
        typedef typename TestFixture::key_extractor key_extractor;

        typedef cc::EllenBinTreeSet< rcu_type, int, int_item,
            typename cc::ellen_bintree::make_set_traits<
                cds::opt::less< typename TestFixture::less >
                ,cc::ellen_bintree::key_extractor< key_extractor >
            >::type
        > set_type;

        set_type s;
        this->test( s );
    }

    TYPED_TEST_P( EllenBinTreeSet, cmpmix )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::int_item int_item;
        typedef typename TestFixture::key_extractor key_extractor;

        typedef cc::EllenBinTreeSet< rcu_type, int, int_item,
            typename cc::ellen_bintree::make_set_traits<
                cds::opt::less< typename TestFixture::less >
                ,cds::opt::compare< typename TestFixture::cmp >
                ,cc::ellen_bintree::key_extractor< key_extractor >
            >::type
        > set_type;

        set_type s;
        this->test( s );
    }

    TYPED_TEST_P( EllenBinTreeSet, update_desc_pool )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::int_item int_item;

        struct set_traits: public cc::ellen_bintree::traits
        {
            typedef typename TestFixture::key_extractor key_extractor;
            typedef typename TestFixture::cmp compare;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::pool_accessor> update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< rcu_type, int, int_item, set_traits > set_type;

        set_type s;
        this->test( s );
    }

    TYPED_TEST_P( EllenBinTreeSet, update_desc_lazy_pool )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::int_item int_item;

        struct set_traits: public cc::ellen_bintree::traits
        {
            typedef typename TestFixture::key_extractor key_extractor;
            typedef typename TestFixture::less less;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::lazy_pool_accessor> update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< rcu_type, int, int_item, set_traits > set_type;

        set_type s;
        this->test( s );
    }

    TYPED_TEST_P( EllenBinTreeSet, item_counting )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::int_item int_item;

        struct set_traits: public cc::ellen_bintree::traits
        {
            typedef typename TestFixture::key_extractor key_extractor;
            typedef typename TestFixture::cmp compare;
            typedef typename TestFixture::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::pool_accessor> update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< rcu_type, int, int_item, set_traits > set_type;

        set_type s;
        this->test( s );
    }

    TYPED_TEST_P( EllenBinTreeSet, backoff )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::int_item int_item;

        struct set_traits: public cc::ellen_bintree::traits
        {
            typedef typename TestFixture::key_extractor key_extractor;
            typedef typename TestFixture::cmp compare;
            typedef typename TestFixture::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::lazy_pool_accessor> update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< rcu_type, int, int_item, set_traits > set_type;

        set_type s;
        this->test( s );
    }

    TYPED_TEST_P( EllenBinTreeSet, stat )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::int_item int_item;

        struct set_traits: public cc::ellen_bintree::traits
        {
            typedef typename TestFixture::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
            typedef cc::ellen_bintree::stat<> stat;
            typedef typename TestFixture::key_extractor key_extractor;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::pool_accessor> update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< rcu_type, int, int_item, set_traits > set_type;

        set_type s;
        this->test( s );
    }

    TYPED_TEST_P( EllenBinTreeSet, seq_cst )
    {
        typedef typename TestFixture::rcu_type rcu_type;
        typedef typename TestFixture::int_item int_item;

        struct set_traits: public cc::ellen_bintree::traits
        {
            typedef typename TestFixture::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
            typedef cc::ellen_bintree::stat<> stat;
            typedef typename TestFixture::key_extractor key_extractor;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::pool_accessor> update_desc_allocator;
            typedef cds::opt::v::sequential_consistent memory_model;
        };
        typedef cc::EllenBinTreeSet< rcu_type, int, int_item, set_traits > set_type;

        set_type s;
        this->test( s );
    }


    // GCC 5: All this->test names should be written on single line, otherwise a runtime error will be encountered like as
    // "No this->test named <test_name> can be found in this this->test case"
    REGISTER_TYPED_TEST_CASE_P( EllenBinTreeSet,
        compare, less, cmpmix, update_desc_pool, update_desc_lazy_pool, item_counting, backoff, stat, seq_cst
    );
} // namespace

#endif // CDSUNIT_TREE_TEST_ELLEN_BINTREE_SET_RCU_H

