/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#ifndef CDSHDRTEST_INTRUSIVE_ELLEN_BINTREE_POOL_DHP_H
#define CDSHDRTEST_INTRUSIVE_ELLEN_BINTREE_POOL_DHP_H

#include "tree/hdr_intrusive_bintree.h"
#include <cds/intrusive/ellen_bintree_dhp.h>

#include <cds/memory/vyukov_queue_pool.h>
#include <cds/memory/pool_allocator.h>

namespace tree {

    namespace ellen_bintree_dhp {
        typedef cds::intrusive::ellen_bintree::node_types<cds::gc::DHP, IntrusiveBinTreeHdrTest::key_type>   node_types;
        typedef node_types::leaf_node_type                                  leaf_node;
        typedef IntrusiveBinTreeHdrTest::base_hook_value< leaf_node >       base_value;
        typedef node_types::internal_node_type                              internal_node;
        typedef node_types::update_desc_type                                update_desc;


        // Internal node pool based on Vyukov's queue
        struct internal_node_pool_traits : public cds::memory::vyukov_queue_pool_traits
        {
            typedef cds::opt::v::dynamic_buffer< cds::any_type > buffer;
        };
        typedef cds::memory::lazy_vyukov_queue_pool< internal_node, internal_node_pool_traits > internal_node_pool_type;
        extern internal_node_pool_type s_InternalNodePool;

        struct internal_node_pool_accessor {
            typedef internal_node_pool_type::value_type     value_type;

            internal_node_pool_type& operator()() const
            {
                return s_InternalNodePool;
            }
        };

        // Update descriptor pool based on Vyukov's queue
        struct update_desc_pool_traits : public cds::memory::vyukov_queue_pool_traits
        {
            typedef cds::opt::v::static_buffer< cds::any_type, 16 > buffer;
        };
        typedef cds::memory::vyukov_queue_pool< update_desc, update_desc_pool_traits > update_desc_pool_type;
        extern update_desc_pool_type s_UpdateDescPool;

        struct update_desc_pool_accessor {
            typedef update_desc_pool_type::value_type     value_type;

            update_desc_pool_type& operator()() const
            {
                return s_UpdateDescPool;
            }
        };

    } // namespace ellen_bintree_dhp

} // namespace tree

#endif // #ifndef CDSHDRTEST_INTRUSIVE_ELLEN_BINTREE_POOL_DHP_H
