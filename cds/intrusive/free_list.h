// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_FREE_LIST_H
#define CDSLIB_INTRUSIVE_FREE_LIST_H

#include <cds/algo/atomic.h>

namespace cds { namespace intrusive {

    /// Lock-free free list
    /** @ingroup cds_intrusive_freelist

        Free list is a helper class intended for reusing objects instead of freeing them completely;
        this avoids the overhead of \p malloc(), and also avoids its worst-case behavior of taking an operating system lock.
        So, the free list can be considered as a specialized allocator for objects of some type.

        The algorithm is taken from <a href="http://moodycamel.com/blog/2014/solving-the-aba-problem-for-lock-free-free-lists">this article</a>.
        The algo does not require any SMR like Hazard Pointer to prevent ABA problem.

        There is \ref TaggedFreeList "tagged pointers" variant of free list for processors with double-width CAS support.

        \b How to use
        \code
        #include <cds/intrusive/free_list.h>

        // Your struct should be derived from FreeList::node
        struct Foo: public cds::intrusive::FreeList::node
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
            typedef cds::intrusive::FreeList::node freelist_node;
            cds::intrusive::FreeList m_FreeList;
        };
        \endcode
    */
    class FreeList
    {
    public:
        /// Free list node
        struct node {
            //@cond
            atomics::atomic<uint32_t>   m_freeListRefs;
            atomics::atomic<node *>     m_freeListNext;

            node()
                : m_freeListRefs( 0 )
            {
                m_freeListNext.store( nullptr, atomics::memory_order_release );
            }
            //@endcond
        };

    public:
        /// Creates empty free list
        FreeList()
            : m_Head( nullptr )
        {}

        /// Destroys the free list. Free-list must be empty.
        /**
            @warning dtor does not free elements of the list.
            To free elements you should manually call \p clear() with an appropriate disposer.
        */
        ~FreeList()
        {
            assert( empty());
        }

        /// Puts \p pNode to the free list
        void put( node * pNode )
        {
            // We know that the should-be-on-freelist bit is 0 at this point, so it's safe to
            // set it using a fetch_add
            if ( pNode->m_freeListRefs.fetch_add( c_ShouldBeOnFreeList, atomics::memory_order_release ) == 0 ) {
                // Oh look! We were the last ones referencing this node, and we know
                // we want to add it to the free list, so let's do it!
                add_knowing_refcount_is_zero( pNode );
            }
        }

        /// Gets a node from the free list. If the list is empty, returns \p nullptr
        node * get()
        {
            auto head = m_Head.load( atomics::memory_order_acquire );
            while ( head != nullptr ) {
                auto prevHead = head;
                auto refs = head->m_freeListRefs.load( atomics::memory_order_relaxed );

                if ( cds_unlikely( (refs & c_RefsMask) == 0 || !head->m_freeListRefs.compare_exchange_strong( refs, refs + 1,
                    atomics::memory_order_acquire, atomics::memory_order_relaxed )))
                {
                    head = m_Head.load( atomics::memory_order_acquire );
                    continue;
                }

                // Good, reference count has been incremented (it wasn't at zero), which means
                // we can read the next and not worry about it changing between now and the time
                // we do the CAS
                node * next = head->m_freeListNext.load( atomics::memory_order_relaxed );
                if ( cds_likely( m_Head.compare_exchange_strong( head, next, atomics::memory_order_acquire, atomics::memory_order_relaxed ))) {
                    // Yay, got the node. This means it was on the list, which means
                    // shouldBeOnFreeList must be false no matter the refcount (because
                    // nobody else knows it's been taken off yet, it can't have been put back on).
                    assert( (head->m_freeListRefs.load( atomics::memory_order_relaxed ) & c_ShouldBeOnFreeList) == 0 );

                    // Decrease refcount twice, once for our ref, and once for the list's ref
                    head->m_freeListRefs.fetch_sub( 2, atomics::memory_order_relaxed );

                    return head;
                }

                // OK, the head must have changed on us, but we still need to decrease the refcount we
                // increased
                refs = prevHead->m_freeListRefs.fetch_sub( 1, atomics::memory_order_acq_rel );
                if ( refs == c_ShouldBeOnFreeList + 1 )
                    add_knowing_refcount_is_zero( prevHead );
            }

            return nullptr;
        }

        /// Checks whether the free list is empty
        bool empty() const
        {
            return m_Head.load( atomics::memory_order_relaxed ) == nullptr;
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
            node * head = m_Head.load( atomics::memory_order_relaxed );
            m_Head.store( nullptr, atomics::memory_order_relaxed );
            while ( head ) {
                node * next = head->m_freeListNext.load( atomics::memory_order_relaxed );
                disp( head );
                head = next;
            }
        }

    private:
        //@cond
        void add_knowing_refcount_is_zero( node * pNode )
        {
            // Since the refcount is zero, and nobody can increase it once it's zero (except us, and we
            // run only one copy of this method per node at a time, i.e. the single thread case), then we
            // know we can safely change the next pointer of the node; however, once the refcount is back
            // above zero, then other threads could increase it (happens under heavy contention, when the
            // refcount goes to zero in between a load and a refcount increment of a node in try_get, then
            // back up to something non-zero, then the refcount increment is done by the other thread) --
            // so, if the CAS to add the node to the actual list fails, decrease the refcount and leave
            // the add operation to the next thread who puts the refcount back at zero (which could be us,
            // hence the loop).
            node * head = m_Head.load( atomics::memory_order_relaxed );
            while ( true ) {
                pNode->m_freeListNext.store( head, atomics::memory_order_relaxed );
                pNode->m_freeListRefs.store( 1, atomics::memory_order_release );
                if ( cds_unlikely( !m_Head.compare_exchange_strong( head, pNode, atomics::memory_order_release, atomics::memory_order_relaxed ))) {
                    // Hmm, the add failed, but we can only try again when the refcount goes back to zero
                    if ( pNode->m_freeListRefs.fetch_add( c_ShouldBeOnFreeList - 1, atomics::memory_order_release ) == 1 )
                        continue;
                }
                return;
            }
        }
        //@endcond

    private:
        //@cond
        static constexpr uint32_t const c_RefsMask = 0x7FFFFFFF;
        static constexpr uint32_t const c_ShouldBeOnFreeList = 0x80000000;

        // Implemented like a stack, but where node order doesn't matter (nodes are
        // inserted out of order under contention)
        atomics::atomic<node *>  m_Head;
        //@endcond
    };

}} // namespace cds::intrusive

#endif // CDSLIB_INTRUSIVE_FREE_LIST_H
