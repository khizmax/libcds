/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.     
*/

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
            << "\t\t            m_nUpdateExist: " << s.m_nUpdateExist.get()           << "\n"
            << "\t\t              m_nUpdateNew: " << s.m_nUpdateNew.get()             << "\n"
            << "\t\t          m_nUpdateRetries: " << s.m_nUpdateRetries.get()         << "\n"
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
