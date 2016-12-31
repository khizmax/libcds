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

#include "test_intrusive_tree_hp.h"

#include <cds/intrusive/ellen_bintree_dhp.h>
#include "test_ellen_bintree_update_desc_pool.h"

namespace {
    namespace ci = cds::intrusive;
    typedef cds::gc::DHP gc_type;

    class IntrusiveEllenBinTree_DHP : public cds_test::intrusive_tree_hp
    {
    protected:
        typedef cds_test::intrusive_tree_hp base_class;

    protected:
        typedef base_class::key_type key_type;

        typedef typename base_class::base_int_item< ci::ellen_bintree::node<gc_type>> base_item_type;
        typedef typename base_class::member_int_item< ci::ellen_bintree::node<gc_type>>  member_item_type;

        void SetUp()
        {
            typedef ci::EllenBinTree< gc_type, key_type, base_item_type > tree_type;

            cds::gc::dhp::GarbageCollector::Construct( 16, tree_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::dhp::GarbageCollector::Destruct();
        }

        struct generic_traits: public ci::ellen_bintree::traits
        {
            typedef base_class::key_extractor key_extractor;
            typedef mock_disposer disposer;
        };
    };

    TEST_F( IntrusiveEllenBinTree_DHP, base_cmp )
    {
        typedef ci::EllenBinTree< gc_type, key_type, base_item_type,
            ci::ellen_bintree::make_traits<
                ci::opt::type_traits< generic_traits >
                ,ci::opt::hook< ci::ellen_bintree::base_hook< ci::opt::gc< gc_type >>>
                ,ci::opt::compare< cmp<base_item_type>>
            >::type
        > tree_type;

        tree_type t;
        test( t );
    }

    TEST_F( IntrusiveEllenBinTree_DHP, base_less )
    {
        typedef ci::EllenBinTree< gc_type, key_type, base_item_type,
            ci::ellen_bintree::make_traits<
                ci::opt::type_traits< generic_traits >
                ,ci::opt::hook< ci::ellen_bintree::base_hook< ci::opt::gc< gc_type >>>
                ,ci::opt::less< less<base_item_type>>
            >::type
        > tree_type;

        tree_type t;
        test( t );
    }

    TEST_F( IntrusiveEllenBinTree_DHP, base_item_counter )
    {
        typedef ci::EllenBinTree< gc_type, key_type, base_item_type,
            ci::ellen_bintree::make_traits<
                ci::opt::type_traits< generic_traits >
                ,ci::opt::hook< ci::ellen_bintree::base_hook< ci::opt::gc< gc_type >>>
                ,ci::opt::compare< cmp<base_item_type>>
                ,ci::opt::item_counter< simple_item_counter >
            >::type
        > tree_type;

        tree_type t;
        test( t );
    }

    TEST_F( IntrusiveEllenBinTree_DHP, base_backoff )
    {
        struct tree_traits: public generic_traits
        {
            typedef ci::ellen_bintree::base_hook< ci::opt::gc< gc_type >> hook;
            typedef cmp<base_item_type> compare;
            typedef base_class::less<base_item_type> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
        };

        typedef ci::EllenBinTree< gc_type, key_type, base_item_type, tree_traits > tree_type;

        tree_type t;
        test( t );
    }

    TEST_F( IntrusiveEllenBinTree_DHP, base_seq_cst )
    {
        struct tree_traits: public generic_traits
        {
            typedef ci::ellen_bintree::base_hook< ci::opt::gc< gc_type >> hook;
            typedef cmp<base_item_type> compare;
            typedef base_class::less<base_item_type> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::pause back_off;
            typedef ci::opt::v::sequential_consistent memory_model;
        };

        typedef ci::EllenBinTree< gc_type, key_type, base_item_type, tree_traits > tree_type;

        tree_type t;
        test( t );
    }

    TEST_F( IntrusiveEllenBinTree_DHP, base_update_desc_pool )
    {
        struct tree_traits: public generic_traits
        {
            typedef ci::ellen_bintree::base_hook< ci::opt::gc< gc_type >> hook;
            typedef base_class::less<base_item_type> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::pool_accessor> update_desc_allocator;
        };

        typedef ci::EllenBinTree< gc_type, key_type, base_item_type, tree_traits > tree_type;

        tree_type t;
        test( t );
    }

    TEST_F( IntrusiveEllenBinTree_DHP, base_update_desc_lazy_pool )
    {
        struct tree_traits: public generic_traits
        {
            typedef ci::ellen_bintree::base_hook< ci::opt::gc< gc_type >> hook;
            typedef base_class::less<base_item_type> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::lazy_pool_accessor> update_desc_allocator;
        };

        typedef ci::EllenBinTree< gc_type, key_type, base_item_type, tree_traits > tree_type;

        tree_type t;
        test( t );
    }

    // member hook
    TEST_F( IntrusiveEllenBinTree_DHP, member_cmp )
    {
        typedef ci::EllenBinTree< gc_type, key_type, member_item_type,
            ci::ellen_bintree::make_traits<
                ci::opt::type_traits< generic_traits >
                ,ci::opt::hook< ci::ellen_bintree::member_hook< offsetof( member_item_type, hMember), ci::opt::gc< gc_type >>>
                ,ci::opt::compare< cmp<member_item_type>>
            >::type
        > tree_type;

        tree_type t;
        test( t );
    }

    TEST_F( IntrusiveEllenBinTree_DHP, member_less )
    {
        typedef ci::EllenBinTree< gc_type, key_type, member_item_type,
            ci::ellen_bintree::make_traits<
                ci::opt::type_traits< generic_traits >
                ,ci::opt::hook< ci::ellen_bintree::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< gc_type >>>
                ,ci::opt::less< less<member_item_type>>
            >::type
        > tree_type;

        tree_type t;
        test( t );
    }

    TEST_F( IntrusiveEllenBinTree_DHP, member_item_counter )
    {
        typedef ci::EllenBinTree< gc_type, key_type, member_item_type,
            ci::ellen_bintree::make_traits<
                ci::opt::type_traits< generic_traits >
                ,ci::opt::hook< ci::ellen_bintree::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< gc_type >>>
                ,ci::opt::compare< cmp<member_item_type>>
                ,ci::opt::item_counter< simple_item_counter >
            >::type
        > tree_type;

        tree_type t;
        test( t );
    }

    TEST_F( IntrusiveEllenBinTree_DHP, member_backoff )
    {
        struct tree_traits: public generic_traits
        {
            typedef ci::ellen_bintree::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< gc_type >> hook;
            typedef cmp<member_item_type> compare;
            typedef base_class::less<member_item_type> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::yield back_off;
        };

        typedef ci::EllenBinTree< gc_type, key_type, member_item_type, tree_traits > tree_type;

        tree_type t;
        test( t );
    }

    TEST_F( IntrusiveEllenBinTree_DHP, member_seq_cst )
    {
        struct tree_traits: public generic_traits
        {
            typedef ci::ellen_bintree::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< gc_type >> hook;
            typedef cmp<member_item_type> compare;
            typedef base_class::less<member_item_type> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::backoff::pause back_off;
            typedef ci::opt::v::sequential_consistent memory_model;
        };

        typedef ci::EllenBinTree< gc_type, key_type, member_item_type, tree_traits > tree_type;

        tree_type t;
        test( t );
    }

    TEST_F( IntrusiveEllenBinTree_DHP, member_update_desc_pool )
    {
        struct tree_traits: public generic_traits
        {
            typedef ci::ellen_bintree::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< gc_type >> hook;
            typedef base_class::less<member_item_type> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::pool_accessor> update_desc_allocator;
        };

        typedef ci::EllenBinTree< gc_type, key_type, member_item_type, tree_traits > tree_type;

        tree_type t;
        test( t );
    }

    TEST_F( IntrusiveEllenBinTree_DHP, member_update_desc_lazy_pool )
    {
        struct tree_traits: public generic_traits
        {
            typedef ci::ellen_bintree::member_hook< offsetof( member_item_type, hMember ), ci::opt::gc< gc_type >> hook;
            typedef base_class::less<member_item_type> less;
            typedef cds::atomicity::item_counter item_counter;
            typedef cds::memory::pool_allocator<cds_test::update_desc, cds_test::lazy_pool_accessor> update_desc_allocator;
        };

        typedef ci::EllenBinTree< gc_type, key_type, member_item_type, tree_traits > tree_type;

        tree_type t;
        test( t );
    }

} // namespace
