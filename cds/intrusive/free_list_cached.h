// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_FREE_LIST_CACHED_H
#define CDSLIB_INTRUSIVE_FREE_LIST_CACHED_H

#include <cds/algo/atomic.h>
#include <cds/opt/options.h>
#include <cds/user_setup/cache_line.h>
#include <cds/details/type_padding.h>

#include <thread>
#include <functional>

namespace cds { namespace intrusive {

    /// Cached free list
    /** @ingroup cds_intrusive_freelist

        The class that is a wrapper over other \p FreeList contains a small cache of free elements.
        Before placing a new item into underlying \p FreeList the cached free-list tryes
        to put that item into the cache if its corresponding slot is empty. The slot is calculated by
        current thread id:
        \code
        int slot = std::hash<std::thread::id>()( std::this_thread::get_id()) & (CacheSize - 1);
        \endcode

        When getting the free-list checks the corresponding cache slot. If it is not empty, its
        contents is returned.

        In some cases such simple algorithm significantly reduces \p FreeList contention.

        Template parameters:
        - \p FreeList - a free-list implementation: \p FreeList, \p TaggedFreeList
        - \p CacheSize - size of cache, a small power-of-two number, default is 16
        - \p Padding - padding of cache elements for solving false sharing, default is \p cds::c_nCacheLineSize
    */
    template <typename FreeList, size_t CacheSize = 16, unsigned Padding = cds::c_nCacheLineSize >
    class CachedFreeList
    {
    public:
        typedef FreeList free_list_type;    ///< Undelying free-list type
        typedef typename free_list_type::node node; ///< Free-list node

        static size_t const c_cache_size = CacheSize;   ///< Cache size
        static unsigned const c_padding = Padding;      ///< Cache element padding

        static_assert( c_cache_size >= 4, "Cache size is too small" );
        static_assert( (c_cache_size & (c_cache_size - 1)) == 0, "CacheSize must be power of two" );
        static_assert( (c_padding & (c_padding - 1)) == 0, "Padding must be power-of-two");

    public:
        /// Creates empty free list
        CachedFreeList()
        {
            for ( auto& i: m_cache )
                i.store( nullptr, atomics::memory_order_relaxed );
        }

        /// Destroys the free list. Free-list must be empty.
        /**
            @warning dtor does not free elements of the list.
            To free elements you should manually call \p clear() with an appropriate disposer.
        */
        ~CachedFreeList()
        {
            assert( empty());
        }

        /// Puts \p pNode to the free list
        void put( node* pNode )
        {
            // try to put into free cell of cache
            node* expect = nullptr;
            if ( m_cache[ get_hash() ].compare_exchange_weak( expect, pNode, atomics::memory_order_release, atomics::memory_order_relaxed ))
                return;

            // cache cell is not empty - use free-list
            m_freeList.put( pNode );
        }

        /// Gets a node from the free list. If the list is empty, returns \p nullptr
        node * get()
        {
            // try get from cache
            atomics::atomic<node*>& cell = m_cache[ get_hash() ];
            node* p = cell.load( atomics::memory_order_relaxed );
            if ( p && cell.compare_exchange_weak( p, nullptr, atomics::memory_order_acquire, atomics::memory_order_relaxed ))
                return p;

            // try read from free-list
            p = m_freeList.get();
            if ( p )
                return p;

            // iterate the cache
            for ( auto& item : m_cache ) {
                p = item.load( atomics::memory_order_relaxed );
                if ( p && item.compare_exchange_weak( p, nullptr, atomics::memory_order_acquire, atomics::memory_order_relaxed ))
                    return p;
            }

            return m_freeList.get();
        }

        /// Checks whether the free list is empty
        bool empty() const
        {
            if ( !m_freeList.empty())
                return false;

            for ( auto& cell : m_cache ) {
                node* p = cell.load( atomics::memory_order_relaxed );
                if ( p )
                    return false;
            }

            return true;
        }

        /// Clears the free list (not atomic)
        /**
            For each element \p disp disposer is called to free memory.
            The \p Disposer interface:
            \code
            struct disposer
            {
                void operator()( FreeList::node * node );
            };
            \endcode

            This method must be explicitly called before the free list destructor.
        */
        template <typename Disposer>
        void clear( Disposer disp )
        {
            m_freeList.clear( disp );
            for ( auto& cell : m_cache ) {
                node* p = cell.load( atomics::memory_order_relaxed );
                if ( p ) {
                    disp( p );
                    cell.store( nullptr, atomics::memory_order_relaxed );
                }
            }
        }

    private:
        //@cond
        size_t get_hash()
        {
            return std::hash<std::thread::id>()( std::this_thread::get_id()) & (c_cache_size - 1);
        }
        //@endcond
    private:
        //@cond
        typedef typename cds::details::type_padding< atomics::atomic<node*>, c_padding >::type array_item;
        array_item m_cache[ c_cache_size ];
        free_list_type  m_freeList;
        //@endcond
    };

}} // namespace cds::intrusive
//@endcond

#endif // CDSLIB_INTRUSIVE_FREE_LIST_CACHED_H
