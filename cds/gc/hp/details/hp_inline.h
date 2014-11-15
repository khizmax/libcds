//$$CDS-header$$

#ifndef __CDS_GC_HP_DETAILS_HP_INLINE_H
#define __CDS_GC_HP_DETAILS_HP_INLINE_H

namespace cds {
    namespace gc{ namespace hp { namespace details {

        /************************************************************************/
        /* INLINES                                                              */
        /************************************************************************/
        inline retired_vector::retired_vector( const cds::gc::hp::GarbageCollector& HzpMgr ) CDS_NOEXCEPT
            : m_arr( HzpMgr.getMaxRetiredPtrCount() ),
            m_nSize(0)
        {}

        inline hp_record::hp_record( const cds::gc::hp::GarbageCollector& HzpMgr )
            : m_hzp( HzpMgr.getHazardPointerCount() ),
            m_arrRetired( HzpMgr )
        {}

    } } }    // namespace gc::hp::details
}    // namespace cds


#endif // #ifndef __CDS_GC_HP_DETAILS_HP_INLINE_H
