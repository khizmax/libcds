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
#ifndef CDSUNIT_TREE_TEST_ELLEN_BINTREE_MAP_RCU_H
#define CDSUNIT_TREE_TEST_ELLEN_BINTREE_MAP_RCU_H

#include "test_tree_map_rcu.h"
#include <cds/container/ellen_bintree_map_rcu.h>
#include "test_ellen_bintree_update_desc_pool.h"

namespace {
    namespace cc = cds::container;

    template <class RCU>
    class EllenBinTreeMap: public cds_test::container_tree_map_rcu
    {
        typedef cds_test::container_tree_map_rcu base_class;
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

    TYPED_TEST_CASE_P( EllenBinTreeMap );

    TYPED_TEST_P( EllenBinTreeMap, compare )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::value_type value_type;

        typedef cc::EllenBinTreeMap< rcu_type, key_type, value_type,
            typename cc::ellen_bintree::make_map_traits<
                cds::opt::compare< typename TestFixture::cmp >
            >::type
        > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( EllenBinTreeMap, less )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::value_type value_type;

        typedef cc::EllenBinTreeMap< rcu_type, key_type, value_type,
            typename cc::ellen_bintree::make_map_traits<
                cds::opt::less< typename TestFixture::less >
            >::type
        > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( EllenBinTreeMap, update_desc_pool )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::value_type value_type;

        typedef cc::EllenBinTreeMap< rcu_type, key_type, value_type,
            typename cc::ellen_bintree::make_map_traits<
                cds::opt::less< typename TestFixture::less >
                ,cc::ellen_bintree::update_desc_allocator< cds::memory::pool_allocator<cds_test::update_desc, cds_test::pool_accessor>>
            >::type
        > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( EllenBinTreeMap, update_desc_lazy_pool )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::value_type value_type;

        typedef cc::EllenBinTreeMap< rcu_type, key_type, value_type,
            typename cc::ellen_bintree::make_map_traits<
                cds::opt::less< typename TestFixture::less >
                , cc::ellen_bintree::update_desc_allocator< cds::memory::pool_allocator<cds_test::update_desc, cds_test::lazy_pool_accessor>>
            >::type
        > map_type;

            map_type m;
            this->test( m );
    }

    TYPED_TEST_P( EllenBinTreeMap, cmpmix )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::value_type value_type;

        typedef cc::EllenBinTreeMap< rcu_type, key_type, value_type,
            typename cc::ellen_bintree::make_map_traits<
                cds::opt::less< typename TestFixture::less >
                ,cds::opt::compare< typename TestFixture::cmp >
            >::type
        > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( EllenBinTreeMap, item_counting )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::value_type value_type;

        struct map_traits: public cc::ellen_bintree::traits
        {
            typedef typename TestFixture::cmp compare;
            typedef typename TestFixture::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::pool_accessor> update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< rcu_type, key_type, value_type, map_traits > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( EllenBinTreeMap, backoff )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::value_type value_type;

        struct map_traits: public cc::ellen_bintree::traits
        {
            typedef typename TestFixture::cmp compare;
            typedef typename TestFixture::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::lazy_pool_accessor> update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< rcu_type, key_type, value_type, map_traits > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( EllenBinTreeMap, stat )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::value_type value_type;

        struct map_traits: public cc::ellen_bintree::traits
        {
            typedef typename TestFixture::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
            typedef cc::ellen_bintree::stat<> stat;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::pool_accessor> update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< rcu_type, key_type, value_type, map_traits > map_type;

        map_type m;
        this->test( m );
    }

    TYPED_TEST_P( EllenBinTreeMap, copy_policy )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::value_type value_type;

        struct map_traits: public cc::ellen_bintree::traits
        {
            typedef typename TestFixture::less less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
            typedef cc::ellen_bintree::stat<> stat;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::lazy_pool_accessor> update_desc_allocator;
            typedef typename TestFixture::copy_key copy_policy;
        };
        typedef cc::EllenBinTreeMap< rcu_type, key_type, value_type, map_traits > map_type;

        map_type m;
        this->test( m );
    }

    REGISTER_TYPED_TEST_CASE_P( EllenBinTreeMap,
        compare, less, update_desc_pool, update_desc_lazy_pool, cmpmix, item_counting, backoff, stat, copy_policy
    );

} // namespace

#endif // CDSUNIT_TREE_TEST_ELLEN_BINTREE_MAP_RCU_H
 