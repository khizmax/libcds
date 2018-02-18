// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_FREE_LIST_TAGGED_H
#define CDSLIB_INTRUSIVE_FREE_LIST_TAGGED_H

#include <cds/algo/atomic.h>

namespace cds { namespace intrusive {

    /// Lock-free free list based on tagged pointers (required double-width CAS)
    /** @ingroup cds_intrusive_freelist

        This variant of \p FreeList is intended for processor architectures that support double-width CAS.
        It uses <a href="https://en.wikipedia.org/wiki/Tagged_pointer">tagged pointer</a> technique to solve ABA problem.

        \b How to use
        \code
        #include <cds/intrusive/free_list_tagged.h>

        // Your struct should be derived from TaggedFreeList::node
        struct Foo: public cds::intrusive::TaggedFreeList::node
        {
            // Foo fields
        };

        // Simplified Foo allocator
        class FooAllocator
        {
        public:
            // free-list clear() must be explicitly called before destroying the free-list object
            ~FooAllocator()
            {
                m_FreeList.clear( []( freelist_node * p ) { delete static_cast<Foo *>( p ); });
            }

            Foo * alloc()
            {
                freelist_node * p = m_FreeList.get();
                if ( p )
                    return static_cast<Foo *>( p );
                return new Foo;
            };

            void dealloc( Foo * p )
            {
                m_FreeList.put( static_cast<freelist_node *>( p ));
            };

        private:
            typedef cds::intrusive::TaggedFreeList::node freelist_node;
            cds::intrusive::TaggedFreeList m_FreeList;
        };
        \endcode
    */
    class TaggedFreeList
    {
    public:
        struct node {
            //@cond
            atomics::atomic<node *> m_freeListNext;

            node()
            {
                m_freeListNext.store( nullptr, atomics::memory_order_release );
            }
            //@endcond
        };

    private:
        //@cond
        struct tagged_ptr
        {
            node *    ptr;
            uintptr_t tag;

            tagged_ptr()
                : ptr( nullptr )
                , tag( 0 )
            {}

            tagged_ptr( node* p )
                : ptr( p )
                , tag( 0 )
            {}
        };

        static_assert(sizeof( tagged_ptr ) == sizeof( void * ) * 2, "sizeof( tagged_ptr ) violation");
        //@endcond

    public:
        /// Creates empty free-list
        TaggedFreeList()
            : m_Head( tagged_ptr())
        {
            // Your platform must support double-width CAS
            assert( m_Head.is_lock_free());
        }

        /// Destroys the free list. Free-list must be empty.
        /**
            @warning dtor does not free elements of the list.
            To free elements you should manually call \p clear() with an appropriate disposer.
        */
        ~TaggedFreeList()
        {
            assert( empty());
        }

        /// Puts \p pNode to the free list
        void put( node * pNode )
        {
            assert( m_Head.is_lock_free());

            tagged_ptr currentHead = m_Head.load( atomics::memory_order_relaxed );
            tagged_ptr newHead = { pNode };
            do {
                newHead.tag = currentHead.tag + 1;
                pNode->m_freeListNext.store( currentHead.ptr, atomics::memory_order_relaxed );
                CDS_TSAN_ANNOTATE_HAPPENS_BEFORE( &pNode->m_freeListNext );
            } while ( cds_unlikely( !m_Head.compare_exchange_weak( currentHead, newHead, atomics::memory_order_release, atomics::memory_order_acquire )));
        }

        /// Gets a node from the free list. If the list is empty, returns \p nullptr
        node * get()
        {
            tagged_ptr currentHead = m_Head.load( atomics::memory_order_acquire );
            tagged_ptr newHead;
            while ( currentHead.ptr != nullptr ) {
                CDS_TSAN_ANNOTATE_HAPPENS_AFTER( &currentHead.ptr->m_freeListNext );
                newHead.ptr = currentHead.ptr->m_freeListNext.load( atomics::memory_order_relaxed );
                newHead.tag = currentHead.tag + 1;
                if ( cds_likely( m_Head.compare_exchange_weak( currentHead, newHead, atomics::memory_order_release, atomics::memory_order_acquire )))
                    break;
            }
            return currentHead.ptr;
        }

        /// Checks whether the free list is empty
        bool empty() const
        {
            return m_Head.load( atomics::memory_order_relaxed ).ptr == nullptr;
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
            node * head = m_Head.load( atomics::memory_order_relaxed ).ptr;
            m_Head.store( { nullptr }, atomics::memory_order_relaxed );
            while ( head ) {
                node * next = head->m_freeListNext.load( atomics::memory_order_relaxed );
                disp( head );
                head = next;
            }
        }

    private:
        //@cond
        atomics::atomic<tagged_ptr> m_Head;
        //@endcond
    };

}} // namespace cds::intrusive

#endif // CDSLIB_INTRUSIVE_FREE_LIST_TAGGED_H
