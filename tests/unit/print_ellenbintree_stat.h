//$$CDS-header$$

#ifndef CDSUNIT_PRINT_ELLENBINTREE_STAT_H
#define CDSUNIT_PRINT_ELLENBINTREE_STAT_H

#include "ellen_bintree_update_desc_pool.h"
#include <ostream>

namespace std {
    static inline ostream& operator <<( ostream& o, cds::intrusive::ellen_bintree::empty_stat const& /*s*/ )
    {
        return o;
    }

    static inline ostream& operator <<( ostream& o, cds::intrusive::ellen_bintree::stat<> const& s )
    {
        return o << "\nEllenBinTree statistics [cds::intrusive::ellen_bintree::stat]:\n"
            << "\t\t   Internal node allocated: " << ellen_bintree_pool::internal_node_counter::m_nAlloc.get() << "\n"
            << "\t\t       Internal node freed: " << ellen_bintree_pool::internal_node_counter::m_nFree.get() << "\n"
            << "\t\t    m_nInternalNodeCreated: " << s.m_nInternalNodeCreated.get()   << "\n"
            << "\t\t    m_nInternalNodeDeleted: " << s.m_nInternalNodeDeleted.get()   << "\n"
            << "\t\t      m_nUpdateDescCreated: " << s.m_nUpdateDescCreated.get()     << "\n"
            << "\t\t      m_nUpdateDescDeleted: " << s.m_nUpdateDescDeleted.get()     << "\n"
            << "\t\t          m_nInsertSuccess: " << s.m_nInsertSuccess.get()         << "\n"
            << "\t\t           m_nInsertFailed: " << s.m_nInsertFailed.get()          << "\n"
            << "\t\t          m_nInsertRetries: " << s.m_nInsertRetries.get()         << "\n"
            << "\t\t            m_nEnsureExist: " << s.m_nEnsureExist.get()           << "\n"
            << "\t\t              m_nEnsureNew: " << s.m_nEnsureNew.get()             << "\n"
            << "\t\t          m_nEnsureRetries: " << s.m_nEnsureRetries.get()         << "\n"
            << "\t\t           m_nEraseSuccess: " << s.m_nEraseSuccess.get()          << "\n"
            << "\t\t            m_nEraseFailed: " << s.m_nEraseFailed.get()           << "\n"
            << "\t\t           m_nEraseRetries: " << s.m_nEraseRetries.get()          << "\n"
            << "\t\t            m_nFindSuccess: " << s.m_nFindSuccess.get()           << "\n"
            << "\t\t             m_nFindFailed: " << s.m_nFindFailed.get()            << "\n"
            << "\t\t      m_nExtractMinSuccess: " << s.m_nExtractMinSuccess.get()     << "\n"
            << "\t\t       m_nExtractMinFailed: " << s.m_nExtractMinFailed.get()      << "\n"
            << "\t\t      m_nExtractMinRetries: " << s.m_nExtractMinRetries.get()     << "\n"
            << "\t\t      m_nExtractMaxSuccess: " << s.m_nExtractMaxSuccess.get()     << "\n"
            << "\t\t       m_nExtractMaxFailed: " << s.m_nExtractMaxFailed.get()      << "\n"
            << "\t\t      m_nExtractMaxRetries: " << s.m_nExtractMaxRetries.get()     << "\n"
            << "\t\t            m_nSearchRetry: " << s.m_nSearchRetry.get()           << "\n"
            << "\t\t             m_nHelpInsert: " << s.m_nHelpInsert.get()            << "\n"
            << "\t\t             m_nHelpDelete: " << s.m_nHelpDelete.get()            << "\n"
            << "\t\t               m_nHelpMark: " << s.m_nHelpMark.get()              << "\n"
            << "\t\t       m_nHelpGuardSuccess: " << s.m_nHelpGuardSuccess.get()      << "\n"
            << "\t\t        m_nHelpGuardFailed: " << s.m_nHelpGuardFailed.get()       << "\n";
    }
}

#endif // #ifndef CDSUNIT_PRINT_ELLENBINTREE_STAT_H
