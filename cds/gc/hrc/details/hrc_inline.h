//$$CDS-header$$

#ifndef __CDS_GC_HRC_SCHEMA_INLINE_H
#define __CDS_GC_HRC_SCHEMA_INLINE_H

//@cond
namespace cds { namespace gc { namespace hrc {

    //-------------------------------------------------------------------
    // Inlines
    //-------------------------------------------------------------------

    namespace details {
        inline retired_vector::retired_vector( const GarbageCollector& gc )
        : m_nFreeList(0)
        , m_arr( gc.getMaxRetiredPtrCount() )
        {
            for ( size_t i = 0; i < m_arr.capacity(); ++i )
                m_arr[i].m_nNextFree = i + 1;
            m_arr[ m_arr.capacity() - 1 ].m_nNextFree = m_nEndFreeList;
        }

        inline thread_descriptor::thread_descriptor( const GarbageCollector& gc )
        : m_hzp( gc.getHazardPointerCount() )
        , m_arrRetired( gc )
        {}

    } // namespace details

    inline ContainerNode::ContainerNode()
        : m_bTrace( false )
        , m_bDeleted( false )
    {
        CDS_DEBUG_DO( GarbageCollector::instance().dbgNodeConstructed() ; )
    }

    inline ContainerNode::~ContainerNode()
    {
        assert( m_RC == 0 );
        CDS_DEBUG_DO( GarbageCollector::instance().dbgNodeDestructed() ; )
    }

    inline void GarbageCollector::try_retire( ThreadGC * pThreadGC )
    {
        CDS_DEBUG_DO( unsigned int nAttempt = 0 );

        do {
            pThreadGC->cleanUpLocal();
            Scan( pThreadGC );
            HelpScan( pThreadGC );

            if ( pThreadGC->m_pDesc->m_arrRetired.isFull() )
                CleanUpAll( pThreadGC );

            // infinite loop?
            assert( ++nAttempt <= 3 );
        } while ( pThreadGC->m_pDesc->m_arrRetired.isFull() );
    }


} } } // namespace cds::gc::hrc
//@endcond

#endif // #ifndef __CDS_GC_HRC_SCHEMA_INLINE_H
