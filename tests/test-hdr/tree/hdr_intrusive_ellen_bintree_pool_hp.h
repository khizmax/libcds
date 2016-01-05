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

#ifndef CDSTEST_HDR_INTRUSIVE_ELLEN_BINTREE_POOL_HP_H
#define CDSTEST_HDR_INTRUSIVE_ELLEN_BINTREE_POOL_HP_H

#include "tree/hdr_intrusive_bintree.h"
#include <cds/intrusive/ellen_bintree_hp.h>

#include <cds/memory/vyukov_queue_pool.h>
#include <cds/memory/pool_allocator.h>

namespace tree {

    namespace ellen_bintree_hp {
        typedef cds::intrusive::ellen_bintree::node_types<cds::gc::HP, IntrusiveBinTreeHdrTest::key_type>   node_types;
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

    } // namespace ellen_bintree_hp

} // namespace tree

#endif // #ifndef CDSTEST_HDR_INTRUSIVE_ELLEN_BINTREE_POOL_HP_H
