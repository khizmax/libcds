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

#include "tree/hdr_ellenbintree_set.h"
#include <cds/container/ellen_bintree_set_dhp.h>

#include "tree/hdr_intrusive_ellen_bintree_pool_dhp.h"
#include "unit/print_ellenbintree_stat.h"

namespace tree {
    namespace cc = cds::container;
    namespace co = cds::opt;
    namespace {
        typedef cds::gc::DHP     gc_type;

        typedef cc::ellen_bintree::node<gc_type, EllenBinTreeSetHdrTest::value_type>                    tree_leaf_node;
        typedef cc::ellen_bintree::internal_node< EllenBinTreeSetHdrTest::key_type, tree_leaf_node >    tree_internal_node;
        typedef cc::ellen_bintree::update_desc<tree_leaf_node, tree_internal_node>                      tree_update_desc;

        struct print_stat {
            template <typename Tree>
            void operator()( Tree const& t)
            {
                std::cout << t.statistics();
            }
        };

    }

    void EllenBinTreeSetHdrTest::EllenBinTree_dhp_less()
    {
        struct set_traits: public cc::ellen_bintree::traits{
            typedef EllenBinTreeSetHdrTest::key_extractor key_extractor;
            typedef EllenBinTreeSetHdrTest::less less;
        };
        typedef cc::EllenBinTreeSet< gc_type, key_type, value_type, set_traits > set_type;

        test<set_type, print_stat>();
    }

    void EllenBinTreeSetHdrTest::EllenBinTree_dhp_cmp()
    {
        typedef cc::EllenBinTreeSet< gc_type, key_type, value_type,
            cc::ellen_bintree::make_set_traits<
                cc::ellen_bintree::key_extractor< key_extractor >
                ,co::compare< compare >
            >::type
        > set_type;

        test<set_type, print_stat>();
    }

    void EllenBinTreeSetHdrTest::EllenBinTree_dhp_cmpless()
    {
        typedef cc::EllenBinTreeSet< gc_type, key_type, value_type,
            cc::ellen_bintree::make_set_traits<
                cc::ellen_bintree::key_extractor< key_extractor >
                ,co::compare< compare >
                ,co::less< less >
            >::type
        > set_type;

        test<set_type, print_stat>();
    }

    void EllenBinTreeSetHdrTest::EllenBinTree_dhp_less_ic()
    {
        typedef cc::EllenBinTreeSet< gc_type, key_type, value_type,
            cc::ellen_bintree::make_set_traits<
                cc::ellen_bintree::key_extractor< key_extractor >
                ,co::less< less >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > set_type;

        test<set_type, print_stat>();
    }

    void EllenBinTreeSetHdrTest::EllenBinTree_dhp_cmp_ic()
    {
        typedef cc::EllenBinTreeSet< gc_type, key_type, value_type,
            cc::ellen_bintree::make_set_traits<
                cc::ellen_bintree::key_extractor< key_extractor >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::compare< compare >
            >::type
        > set_type;

        test<set_type, print_stat>();
    }

    void EllenBinTreeSetHdrTest::EllenBinTree_dhp_less_stat()
    {
        typedef cc::EllenBinTreeSet< gc_type, key_type, value_type,
            cc::ellen_bintree::make_set_traits<
                cc::ellen_bintree::key_extractor< key_extractor >
                ,co::less< less >
                ,co::stat< cc::ellen_bintree::stat<> >
            >::type
        > set_type;

        test<set_type, print_stat>();
    }

    void EllenBinTreeSetHdrTest::EllenBinTree_dhp_cmp_ic_stat()
    {
        typedef cc::EllenBinTreeSet< gc_type, key_type, value_type,
            cc::ellen_bintree::make_set_traits<
                cc::ellen_bintree::key_extractor< key_extractor >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::stat< cc::ellen_bintree::stat<> >
                ,co::compare< compare >
            >::type
        > set_type;

        test<set_type, print_stat>();
    }

    void EllenBinTreeSetHdrTest::EllenBinTree_dhp_cmp_ic_stat_yield()
    {
        typedef cc::EllenBinTreeSet< gc_type, key_type, value_type,
            cc::ellen_bintree::make_set_traits<
                cc::ellen_bintree::key_extractor< key_extractor >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::stat< cc::ellen_bintree::stat<> >
                ,co::compare< compare >
                , co::back_off< cds::backoff::yield >
            >::type
        > set_type;

        test<set_type, print_stat>();
    }

    void EllenBinTreeSetHdrTest::EllenBinTree_dhp_less_pool()
    {
        typedef cc::EllenBinTreeSet< gc_type, key_type, value_type,
            cc::ellen_bintree::make_set_traits<
                cc::ellen_bintree::key_extractor< key_extractor >
                ,co::less< less >
                ,co::node_allocator< cds::memory::pool_allocator< tree_internal_node, ellen_bintree_dhp::internal_node_pool_accessor > >
                ,cc::ellen_bintree::update_desc_allocator< cds::memory::pool_allocator< tree_update_desc, ellen_bintree_dhp::update_desc_pool_accessor > >
            >::type
        > set_type;

        test<set_type, print_stat>();
    }

    void EllenBinTreeSetHdrTest::EllenBinTree_dhp_less_pool_ic_stat()
    {
        typedef cc::EllenBinTreeSet< gc_type, key_type, value_type,
            cc::ellen_bintree::make_set_traits<
                cc::ellen_bintree::key_extractor< key_extractor >
                ,co::less< less >
                ,co::node_allocator< cds::memory::pool_allocator< tree_internal_node, ellen_bintree_dhp::internal_node_pool_accessor > >
                ,cc::ellen_bintree::update_desc_allocator< cds::memory::pool_allocator< tree_update_desc, ellen_bintree_dhp::update_desc_pool_accessor > >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::stat< cc::ellen_bintree::stat<> >
            >::type
        > set_type;

        test<set_type, print_stat>();
    }

} // namespace tree
