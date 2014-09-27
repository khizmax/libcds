//$$CDS-header$$

#ifndef __CDS_INTRUSIVE_MICHAEL_LIST_HRC_H
#define __CDS_INTRUSIVE_MICHAEL_LIST_HRC_H

#include <cds/intrusive/impl/michael_list.h>
#include <cds/gc/hrc.h>

namespace cds { namespace intrusive { namespace michael_list {
    //@cond
    // Specialization for HRC GC
    template <typename Tag>
    struct node< gc::HRC, Tag>: public gc::HRC::container_node
    {
        typedef gc::HRC gc  ;   ///< Garbage collector
        typedef Tag     tag ;   ///< tag

        typedef cds::details::marked_ptr<node, 1>   marked_ptr         ;   ///< marked pointer
        typedef typename gc::atomic_marked_ptr< marked_ptr>     atomic_marked_ptr   ;   ///< atomic marked pointer
        atomic_marked_ptr m_pNext ; ///< pointer to the next node in the stack

        node()
            : m_pNext( nullptr )
        {}

    protected:
        virtual void cleanUp( cds::gc::hrc::ThreadGC * pGC )
        {
            assert( pGC );
            typename gc::GuardArray<2> aGuards( *pGC );

            while ( true ) {
                marked_ptr pNextMarked( aGuards.protect( 0, m_pNext ));
                node * pNext = pNextMarked.ptr();
                if ( pNext && pNext->m_bDeleted.load(atomics::memory_order_acquire) ) {
                    marked_ptr p = aGuards.protect( 1, pNext->m_pNext );
                    m_pNext.compare_exchange_strong( pNextMarked, p, atomics::memory_order_acquire, atomics::memory_order_relaxed );
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
                marked_ptr pNext = m_pNext.load(atomics::memory_order_acquire);
                do {} while ( !m_pNext.compare_exchange_weak( pNext, marked_ptr(), atomics::memory_order_release, atomics::memory_order_relaxed ) );
            }
            else {
                m_pNext.store( marked_ptr(), atomics::memory_order_relaxed );
            }
        }
    };
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

}}}   // namespace cds::intrusive::michael_list

#endif // #ifndef __CDS_INTRUSIVE_MICHAEL_LIST_HP_H
