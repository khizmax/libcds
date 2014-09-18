//$$CDS-header$$

#ifndef __CDS_INTRUSIVE_LAZY_LIST_HRC_H
#define __CDS_INTRUSIVE_LAZY_LIST_HRC_H

#include <cds/intrusive/lazy_list_impl.h>
#include <cds/gc/hrc.h>
#include <cds/details/allocator.h>

namespace cds { namespace intrusive { namespace lazy_list {
    //@cond
    // Specialization for HRC GC
    template <typename Lock, typename Tag>
    struct node< gc::HRC, Lock, Tag>: public gc::HRC::container_node
    {
        typedef gc::HRC gc          ;   ///< Garbage collector
        typedef Lock    lock_type   ;   ///< Lock type
        typedef Tag     tag         ;   ///< tag

        typedef cds::details::marked_ptr<node, 1>   marked_ptr         ;   ///< marked pointer
        typedef typename gc::template atomic_marked_ptr< marked_ptr>     atomic_marked_ptr   ;   ///< atomic marked pointer specific for GC

        atomic_marked_ptr   m_pNext ; ///< pointer to the next node in the list + logical deletion mark
        mutable lock_type   m_Lock  ; ///< Node lock

        /// Checks if node is marked
        bool is_marked() const
        {
            return m_pNext.load(CDS_ATOMIC::memory_order_relaxed).bits() != 0;
        }

        node()
            : m_pNext( nullptr )
        {}

    protected:
        virtual void cleanUp( cds::gc::hrc::ThreadGC * pGC )
        {
            assert( pGC != nullptr );
            typename gc::GuardArray<2> aGuards( *pGC );

            while ( true ) {
                marked_ptr pNextMarked( aGuards.protect( 0, m_pNext ));
                node * pNext = pNextMarked.ptr();
                if ( pNext != nullptr && pNext->m_bDeleted.load( CDS_ATOMIC::memory_order_acquire ) ) {
                    marked_ptr p = aGuards.protect( 1, pNext->m_pNext );
                    m_pNext.compare_exchange_weak( pNextMarked, p, CDS_ATOMIC::memory_order_acquire, CDS_ATOMIC::memory_order_relaxed );
                    continue;
                }
                else {
                    break;
                }
            }
        }

        virtual void terminate( cds::gc::hrc::ThreadGC * pGC, bool bConcurrent )
        {
            if ( bConcurrent ) {
                marked_ptr pNext( m_pNext.load(CDS_ATOMIC::memory_order_relaxed));
                do {} while ( !m_pNext.compare_exchange_weak( pNext, marked_ptr(), CDS_ATOMIC::memory_order_release, CDS_ATOMIC::memory_order_relaxed ) );
            }
            else {
                m_pNext.store( marked_ptr(), CDS_ATOMIC::memory_order_relaxed );
            }
        }
    };
    //@endcond

    //@cond
    template <typename NodeType, typename Alloc >
    class boundary_nodes< gc::HRC, NodeType, Alloc >
    {
        typedef NodeType node_type;
        typedef cds::details::Allocator< node_type, Alloc> cxx_allocator   ;   ///< allocator for the tail node

        struct boundary_disposer
        {
            void operator()( node_type * p )
            {
                cxx_allocator().Delete( p );
            }
        };


        node_type *  m_pHead;
        node_type *  m_pTail;

    public:
        boundary_nodes()
        {
            m_pHead = cxx_allocator().New();
            m_pTail = cxx_allocator().New();
        }

        ~boundary_nodes()
        {
            cds::gc::HRC::template retire<boundary_disposer>( m_pHead );
            cds::gc::HRC::template retire<boundary_disposer>( m_pTail );
        }

    public:
        node_type * head()
        {
            return m_pHead;
        }
        node_type const * head() const
        {
            return m_pHead;
        }
        node_type * tail()
        {
            return m_pTail;
        }
        node_type const * tail() const
        {
            return m_pTail;
        }
    };
    //@endcond

    //@cond
    /*
    template <typename Node, typename MemoryModel>
    struct node_cleaner< gc::HRC, Node, MemoryModel> {
        void operator()( Node * p )
        {
            typedef typename Node::marked_ptr marked_ptr;
            p->m_pNext.store( marked_ptr(), MemoryModel::memory_order_release );
            //p->clean( MemoryModel::memory_order_release );
        }
    };
    */
    //@endcond


    //@cond
    template <typename NODE>
    struct link_checker_selector< gc::HRC, NODE, opt::never_check_link >
    {
        typedef link_checker<NODE>  type;
    };

    template <typename NODE>
    struct link_checker_selector< gc::HRC, NODE, opt::debug_check_link >
    {
        typedef link_checker<NODE>  type;
    };
    //@endcond

}}}   // namespace cds::intrusive::lazy_list

#endif // #ifndef __CDS_INTRUSIVE_LAZY_LIST_HP_H
