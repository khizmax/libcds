// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_TREE_TEST_INTRUSIVE_ELLEN_BINTREE_RCU_H
#define CDSUNIT_TREE_TEST_INTRUSIVE_ELLEN_BINTREE_RCU_H

#include "test_intrusive_tree_rcu.h"

#include <cds/intrusive/ellen_bintree_rcu.h>
#include "test_ellen_bintree_update_desc_pool.h"

// forward declaration
namespace cds { namespace intrusive {}}

namespace {

    namespace ci = cds::intrusive;

    template <class RCU>
    class IntrusiveEllenBinTree: public cds_test::intrusive_tree_rcu
    {
        typedef cds_test::intrusive_tree_rcu base_class;

    public:
        typedef cds::urcu::gc<RCU> rcu_type;

        typedef base_class::key_type key_type;

        typedef typename base_class::base_int_item< ci::ellen_bintree::node<rcu_type>> base_item_type;
        typedef typename base_class::member_int_item< ci::ellen_bintree::node<rcu_type>> member_item_type;

        struct generic_traits: public ci::ellen_bintree::traits
        {
            typedef base_class::key_extractor key_extractor;
            typedef mock_disposer disposer;
        };

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

    TYPED_TEST_CASE_P( IntrusiveEllenBinTree );


    TYPED_TEST_P( IntrusiveEllenBinTree, base_cmp )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::base_item_type base_item_type;
        typedef typename TestFixture::generic_traits generic_traits;

        typedef ci::EllenBinTree< rcu_type, key_type, base_item_type,
            typename ci::ellen_bintree::make_traits<
                ci::opt::type_traits< generic_traits >
                , ci::opt::hook< ci::ellen_bintree::base_hook< ci::opt::gc< rcu_type >>>
                , ci::opt::compare< typename TestFixture::template cmp<base_item_type>>
            >::type
        > tree_type;

        tree_type t;
        this->test( t );
    }

    TYPED_TEST_P( IntrusiveEllenBinTree, base_less )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::base_item_type base_item_type;
        typedef typename TestFixture::generic_traits generic_traits;

        typedef ci::EllenBinTree< rcu_type, key_type, base_item_type,
            typename ci::ellen_bintree::make_traits<
                ci::opt::type_traits< generic_traits >
                , ci::opt::hook< ci::ellen_bintree::base_hook< ci::opt::gc< rcu_type >>>
                , ci::opt::less< typename TestFixture::template less<base_item_type>>
            >::type
        > tree_type;

        tree_type t;
        this->test( t );
    }

    TYPED_TEST_P( IntrusiveEllenBinTree, base_item_counter )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::base_item_type base_item_type;
        typedef typename TestFixture::generic_traits generic_traits;

        typedef ci::EllenBinTree< rcu_type, key_type, base_item_type,
            typename ci::ellen_bintree::make_traits<
                ci::opt::type_traits< generic_traits >
                , ci::opt::hook< ci::ellen_bintree::base_hook< ci::opt::gc< rcu_type >>>
                , ci::opt::compare< typename TestFixture::template cmp<base_item_type>>
                , ci::opt::item_counter< typename TestFixture::simple_item_counter >
            >::type
        > tree_type;

        tree_type t;
        this->test( t );
    }

    TYPED_TEST_P( IntrusiveEllenBinTree, base_backoff )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::base_item_type base_item_type;
        typedef typename TestFixture::generic_traits generic_traits;

        struct tree_traits: public generic_traits
        {
            typedef ci::ellen_bintree::base_hook< ci::opt::gc< rcu_type >> hook;
            typedef typename TestFixture::template cmp<base_item_type> compare;
            typedef typename TestFixture::template less<base_item_type> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
        };

        typedef ci::EllenBinTree< rcu_type, key_type, base_item_type, tree_traits > tree_type;

        tree_type t;
        this->test( t );
    }

    TYPED_TEST_P( IntrusiveEllenBinTree, base_seq_cst )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::base_item_type base_item_type;
        typedef typename TestFixture::generic_traits generic_traits;

        struct tree_traits: public generic_traits
        {
            typedef ci::ellen_bintree::base_hook< ci::opt::gc< rcu_type >> hook;
            typedef typename TestFixture::template cmp<base_item_type> compare;
            typedef typename TestFixture::template less<base_item_type> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::pause back_off;
            typedef ci::opt::v::sequential_consistent memory_model;
        };

        typedef ci::EllenBinTree< rcu_type, key_type, base_item_type, tree_traits > tree_type;

        tree_type t;
        this->test( t );
    }

    TYPED_TEST_P( IntrusiveEllenBinTree, base_update_desc_pool )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::base_item_type base_item_type;
        typedef typename TestFixture::generic_traits generic_traits;

        struct tree_traits: public generic_traits
        {
            typedef ci::ellen_bintree::base_hook< ci::opt::gc< rcu_type >> hook;
            typedef typename TestFixture::template less<base_item_type> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::memory::pool_allocator< cds_test::update_desc, cds_test::pool_accessor> update_desc_allocator;
        };

        typedef ci::EllenBinTree< rcu_type, key_type, base_item_type, tree_traits > tree_type;

        tree_type t;
        this->test( t );
    }

    TYPED_TEST_P( IntrusiveEllenBinTree, base_update_desc_lazy_pool )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::base_item_type base_item_type;
        typedef typename TestFixture::generic_traits generic_traits;

        struct tree_traits: public generic_traits
        {
            typedef ci::ellen_bintree::base_hook< ci::opt::gc< rcu_type >> hook;
            typedef typename TestFixture::template less<base_item_type> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::memory::pool_allocator< cds_test::update_desc, cds_test::lazy_pool_accessor> update_desc_allocator;
        };

        typedef ci::EllenBinTree< rcu_type, key_type, base_item_type, tree_traits > tree_type;

        tree_type t;
        this->test( t );
    }

    // member hook
    TYPED_TEST_P( IntrusiveEllenBinTree, member_cmp )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::member_item_type member_item_type;
        typedef typename TestFixture::generic_traits   generic_traits;

        typedef ci::EllenBinTree< rcu_type, key_type, member_item_type,
            typename ci::ellen_bintree::make_traits<
                ci::opt::type_traits< generic_traits >
                , ci::opt::hook< ci::ellen_bintree::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< rcu_type >>>
                , ci::opt::compare< typename TestFixture::template cmp<member_item_type>>
            >::type
        > tree_type;

        tree_type t;
        this->test( t );
    }

    TYPED_TEST_P( IntrusiveEllenBinTree, member_less )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::member_item_type member_item_type;
        typedef typename TestFixture::generic_traits   generic_traits;

        typedef ci::EllenBinTree< rcu_type, key_type, member_item_type,
            typename ci::ellen_bintree::make_traits<
                ci::opt::type_traits< generic_traits >
                , ci::opt::hook< ci::ellen_bintree::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< rcu_type >>>
                , ci::opt::less< typename TestFixture::template less<member_item_type>>
            >::type
        > tree_type;

        tree_type t;
        this->test( t );
    }

    TYPED_TEST_P( IntrusiveEllenBinTree, member_item_counter )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::member_item_type member_item_type;
        typedef typename TestFixture::generic_traits   generic_traits;

        typedef ci::EllenBinTree< rcu_type, key_type, member_item_type,
            typename ci::ellen_bintree::make_traits<
                ci::opt::type_traits< generic_traits >
                , ci::opt::hook< ci::ellen_bintree::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< rcu_type >>>
                , ci::opt::compare< typename TestFixture::template cmp<member_item_type>>
                , ci::opt::item_counter< typename TestFixture::simple_item_counter >
            >::type
        > tree_type;

        tree_type t;
        this->test( t );
    }

    TYPED_TEST_P( IntrusiveEllenBinTree, member_backoff )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::member_item_type member_item_type;
        typedef typename TestFixture::generic_traits   generic_traits;

        struct tree_traits: public generic_traits
        {
            typedef ci::ellen_bintree::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< rcu_type >> hook;
            typedef typename TestFixture::template cmp<member_item_type> compare;
            typedef typename TestFixture::template less<member_item_type> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
        };

        typedef ci::EllenBinTree< rcu_type, key_type, member_item_type, tree_traits > tree_type;

        tree_type t;
        this->test( t );
    }

    TYPED_TEST_P( IntrusiveEllenBinTree, member_seq_cst )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::member_item_type member_item_type;
        typedef typename TestFixture::generic_traits   generic_traits;

        struct tree_traits: public generic_traits
        {
            typedef ci::ellen_bintree::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< rcu_type >> hook;
            typedef typename TestFixture::template cmp<member_item_type> compare;
            typedef typename TestFixture::template less<member_item_type> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::pause back_off;
            typedef ci::opt::v::sequential_consistent memory_model;
        };

        typedef ci::EllenBinTree< rcu_type, key_type, member_item_type, tree_traits > tree_type;

        tree_type t;
        this->test( t );
    }

    TYPED_TEST_P( IntrusiveEllenBinTree, member_update_desc_pool )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::member_item_type member_item_type;
        typedef typename TestFixture::generic_traits   generic_traits;

        struct tree_traits: public generic_traits
        {
            typedef ci::ellen_bintree::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< rcu_type >> hook;
            typedef typename TestFixture::template less<member_item_type> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::memory::pool_allocator< cds_test::update_desc, cds_test::pool_accessor> update_desc_allocator;
        };

        typedef ci::EllenBinTree< rcu_type, key_type, member_item_type, tree_traits > tree_type;

        tree_type t;
        this->test( t );
    }

    TYPED_TEST_P( IntrusiveEllenBinTree, member_update_desc_lazy_pool )
    {
        typedef typename TestFixture::rcu_type   rcu_type;
        typedef typename TestFixture::key_type   key_type;
        typedef typename TestFixture::member_item_type member_item_type;
        typedef typename TestFixture::generic_traits   generic_traits;

        struct tree_traits: public generic_traits
        {
            typedef ci::ellen_bintree::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< rcu_type >> hook;
            typedef typename TestFixture::template less<member_item_type> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::memory::pool_allocator< cds_test::update_desc, cds_test::lazy_pool_accessor> update_desc_allocator;
        };

        typedef ci::EllenBinTree< rcu_type, key_type, member_item_type, tree_traits > tree_type;

        tree_type t;
        this->test( t );
    }

    REGISTER_TYPED_TEST_CASE_P( IntrusiveEllenBinTree,
        base_cmp, base_less, base_item_counter, base_backoff, base_seq_cst, base_update_desc_pool, base_update_desc_lazy_pool, member_cmp, member_less, member_item_counter, member_backoff, member_seq_cst, member_update_desc_pool, member_update_desc_lazy_pool
        );

} // namespace

#endif // #ifndef CDSUNIT_TREE_TEST_INTRUSIVE_ELLEN_BINTREE_RCU_H
