//$$CDS-header$$

#ifndef CDSUNIT_PRINT_BRONSONAVLTREE_STAT_H
#define CDSUNIT_PRINT_BRONSONAVLTREE_STAT_H

#include <ostream>

namespace std {

    static inline ostream& operator <<( ostream& o, cds::container::bronson_avltree::empty_stat const& /*s*/ )
    {
        return o;
    }

    static inline ostream& operator <<(ostream& o, cds::container::bronson_avltree::stat<> const& s)
    {
        return o << "\nBronsonAVLTree statistics [cds::container::bronson_avltree::stat]:\n"
            << "\t\t            m_nFindSuccess: " << s.m_nFindSuccess.get()         << "\n"
            << "\t\t             m_nFindFailed: " << s.m_nFindFailed.get()          << "\n"
            << "\t\t              m_nFindRetry: " << s.m_nFindRetry.get()           << "\n"
            << "\t\t      m_nFindWaitShrinking: " << s.m_nFindWaitShrinking.get()   << "\n"
            << "\t\t          m_nInsertSuccess: " << s.m_nInsertSuccess.get()       << "\n"
            << "\t\t    m_nRelaxedInsertFailed: " << s.m_nRelaxedInsertFailed.get() << "\n"
            << "\t\t            m_nInsertRetry: " << s.m_nInsertRetry.get()         << "\n"
            << "\t\t    m_nUpdateWaitShrinking: " << s.m_nUpdateWaitShrinking.get() << "\n"
            << "\t\t            m_nUpdateRetry: " << s.m_nUpdateRetry.get()         << "\n"
            << "\t\tm_nUpdateRootWaitShrinking: " << s.m_nUpdateRootWaitShrinking.get() << "\n"
            << "\t\t          m_nUpdateSuccess: " << s.m_nUpdateSuccess.get()       << "\n"
            << "\t\t         m_nUpdateUnlinked: " << s.m_nUpdateUnlinked.get()      << "\n"
            << "\t\t            m_nRemoveRetry: " << s.m_nRemoveRetry.get()         << "\n"
            << "\t\t    m_nRemoveWaitShrinking: " << s.m_nRemoveWaitShrinking.get() << "\n"
            << "\t\tm_nRemoveRootWaitShrinking: " << s.m_nRemoveRootWaitShrinking.get() << "\n"
            << "\t\t          m_nDisposedValue: " << s.m_nDisposedValue.get()       << "\n"
            << "\t\t           m_nDisposedNode: " << s.m_nDisposedNode.get()        << "\n"
            << "\t\t         m_nExtractedValue: " << s.m_nExtractedValue.get()      << "\n"
            << "\t\t          m_nRightRotation: " << s.m_nRightRotation.get()       << "\n"
            << "\t\t           m_nLeftRotation: " << s.m_nLeftRotation.get()        << "\n"
            << "\t\t      m_nLeftRightRotation: " << s.m_nLeftRightRotation.get()   << "\n"
            << "\t\t      m_nRightLeftRotation: " << s.m_nRightLeftRotation.get()   << "\n"
            << "\t\t     m_nInsertRebalanceReq: " << s.m_nInsertRebalanceReq.get()  << "\n"
            << "\t\t     m_nRemoveRebalanceReq: " << s.m_nRemoveRebalanceReq.get()  << "\n"
            ;
    }
} //namespace std

#endif // #ifndef CDSUNIT_PRINT_ELLENBINTREE_STAT_H
