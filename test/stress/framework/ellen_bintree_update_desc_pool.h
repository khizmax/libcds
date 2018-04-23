// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_ELLEN_BINTREE_UPDATE_DESC_POOL_H
#define CDSUNIT_ELLEN_BINTREE_UPDATE_DESC_POOL_H

#include <cds/urcu/general_instant.h>
#include <cds/container/details/ellen_bintree_base.h>
#include <cds/memory/vyukov_queue_pool.h>
#include <cds/memory/pool_allocator.h>
#include <memory>

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
        : public std::allocator_traits<Alloc>::template rebind_alloc< T >
        , internal_node_counter
    {
        typedef typename std::allocator_traits<Alloc>::template rebind_alloc< T > base_class;
    public:
        template <typename Other>
        struct rebind {
            typedef internal_node_allocator< Other, Alloc > other;
        };

        T * allocate( size_t n, void const * pHint = nullptr )
        {
            internal_node_counter::onAlloc();
            T * p = std::allocator_traits<base_class>::allocate( *static_cast<base_class*>( this ), n, pHint );
            return p;
        }

        void deallocate( T * p, size_t n )
        {
            internal_node_counter::onFree();
            std::allocator_traits<base_class>::deallocate( *static_cast<base_class*>( this ), p, n );
        }
    };

} // namespace ellen_bintree_pool

#endif // #ifndef CDSUNIT_ELLEN_BINTREE_UPDATE_DESC_POOL_H
