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

#ifndef CDSUNIT_ELLEN_BINTREE_UPDATE_DESC_POOL_H
#define CDSUNIT_ELLEN_BINTREE_UPDATE_DESC_POOL_H

#include <cds/urcu/general_instant.h>
#include <cds/container/details/ellen_bintree_base.h>
#include <cds/memory/vyukov_queue_pool.h>
#include <cds/memory/pool_allocator.h>

namespace ellen_bintree_pool {
    typedef cds::container::ellen_bintree::node_types< cds::urcu::gc< cds::urcu::general_instant<> >, int > node_types; // fake
    typedef node_types::leaf_node_type     leaf_node;
    typedef node_types::internal_node_type internal_node;
    typedef node_types::update_desc_type   update_desc;

    // Update descriptor pool based on Vyukov's queue
    struct update_desc_pool_traits : public cds::memory::vyukov_queue_pool_traits
    {
        typedef cds::opt::v::initialized_static_buffer< cds::any_type, 4096 > buffer;
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

    // Update descriptor pool based on bounded Vyukov's queue
    struct bounded_update_desc_pool_traits : public cds::memory::vyukov_queue_pool_traits
    {
        typedef cds::opt::v::initialized_static_buffer< cds::any_type, 4096 > buffer;
    };
    typedef cds::memory::bounded_vyukov_queue_pool< update_desc, bounded_update_desc_pool_traits > bounded_update_desc_pool_type;
    extern bounded_update_desc_pool_type s_BoundedUpdateDescPool;

    struct bounded_update_desc_pool_accessor {
        typedef bounded_update_desc_pool_type::value_type     value_type;

        bounded_update_desc_pool_type& operator()() const
        {
            return s_BoundedUpdateDescPool;
        }
    };


    // Internal node allocator
    struct internal_node_counter
    {
        static cds::atomicity::event_counter   m_nAlloc;
        static cds::atomicity::event_counter   m_nFree;

        static void onAlloc()
        {
            ++m_nAlloc;
        }
        static void onFree()
        {
            ++m_nFree;
        }

        static void reset()
        {
            m_nAlloc.reset();
            m_nFree.reset();
        }
    };

    template <typename T, typename Alloc = CDS_DEFAULT_ALLOCATOR>
    class internal_node_allocator
        : public Alloc::template rebind< T >::other
        , internal_node_counter
    {
        typedef typename Alloc::template rebind< T >::other  base_class;
    public:
        template <typename Other>
        struct rebind {
            typedef internal_node_allocator< Other, Alloc > other;
        };

        T * allocate( size_t n, void const * pHint = nullptr )
        {
            internal_node_counter::onAlloc();
            T * p = base_class::allocate( n, pHint );
            return p;
        }

        void deallocate( T * p, size_t n )
        {
            internal_node_counter::onFree();
            return base_class::deallocate( p, n );
        }
    };

} // namespace ellen_bintree_pool

#endif // #ifndef CDSUNIT_ELLEN_BINTREE_UPDATE_DESC_POOL_H
