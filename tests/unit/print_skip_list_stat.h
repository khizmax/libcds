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

#ifndef CDSUNIT_PRINT_SKIP_LIST_STAT_H
#define CDSUNIT_PRINT_SKIP_LIST_STAT_H

#include <cds/intrusive/details/skip_list_base.h>
#include <ostream>

namespace std {

    static inline ostream& operator <<( ostream& o, cds::intrusive::skip_list::stat<> const& s )
    {
        o << "Skip-list stat [cds::intrusive::skip_list::stat]\n"
            << "\t\t node height (add/delete):\n";

        for ( unsigned int i = 0; i < 9; ++i )
            o << "\t\t\t[ " << (i + 1) << "]: " << s.m_nNodeHeightAdd[i].get() << '/' << s.m_nNodeHeightDel[i].get() << "\n";
        for ( unsigned int i = 9; i < sizeof(s.m_nNodeHeightAdd) / sizeof(s.m_nNodeHeightAdd[0] ); ++i )
            o << "\t\t\t[" << (i + 1) << "]: " << s.m_nNodeHeightAdd[i].get() << '/' << s.m_nNodeHeightDel[i].get() << "\n";

        return o
            << "\t\t          m_nInsertSuccess: " << s.m_nInsertSuccess.get()           << "\n"
            << "\t\t           m_nInsertFailed: " << s.m_nInsertFailed.get()            << "\n"
            << "\t\t          m_nInsertRetries: " << s.m_nInsertRetries.get()           << "\n"
            << "\t\t            m_nUpdateExist: " << s.m_nUpdateExist.get()             << "\n"
            << "\t\t              m_nUpdateNew: " << s.m_nUpdateNew.get()               << "\n"
            << "\t\t          m_nUnlinkSuccess: " << s.m_nUnlinkSuccess.get()           << "\n"
            << "\t\t           m_nUnlinkFailed: " << s.m_nUnlinkFailed.get()            << "\n"
            << "\t\t         m_nExtractSuccess: " << s.m_nExtractSuccess.get()          << "\n"
            << "\t\t          m_nExtractFailed: " << s.m_nExtractFailed.get()           << "\n"
            << "\t\t         m_nExtractRetries: " << s.m_nExtractRetries.get()          << "\n"
            << "\t\t      m_nExtractMinSuccess: " << s.m_nExtractMinSuccess.get()       << "\n"
            << "\t\t       m_nExtractMinFailed: " << s.m_nExtractMinFailed.get()        << "\n"
            << "\t\t      m_nExtractMinRetries: " << s.m_nExtractMinRetries.get()       << "\n"
            << "\t\t      m_nExtractMaxSuccess: " << s.m_nExtractMaxSuccess.get()       << "\n"
            << "\t\t       m_nExtractMaxFailed: " << s.m_nExtractMaxFailed.get()        << "\n"
            << "\t\t      m_nExtractMaxRetries: " << s.m_nExtractMaxRetries.get()       << "\n"
            << "\t\t           m_nEraseSuccess: " << s.m_nEraseSuccess.get()            << "\n"
            << "\t\t            m_nEraseFailed: " << s.m_nEraseFailed.get()             << "\n"
            << "\t\t             m_nEraseRetry: " << s.m_nEraseRetry.get()              << "\n"
            << "\t\t        m_nFindFastSuccess: " << s.m_nFindFastSuccess.get()         << "\n"
            << "\t\t         m_nFindFastFailed: " << s.m_nFindFastFailed.get()          << "\n"
            << "\t\t        m_nFindSlowSuccess: " << s.m_nFindSlowSuccess.get()         << "\n"
            << "\t\t         m_nFindSlowFailed: " << s.m_nFindSlowFailed.get()          << "\n"
            << "\t\t    m_nRenewInsertPosition: " << s.m_nRenewInsertPosition.get()     << "\n"
            << "\t\t m_nLogicDeleteWhileInsert: " << s.m_nLogicDeleteWhileInsert.get()  << "\n"
            << "\t\t    m_nNotFoundWhileInsert: " << s.m_nNotFoundWhileInsert.get()     << "\n"
            << "\t\t              m_nFastErase: " << s.m_nFastErase.get()               << "\n"
            << "\t\t              m_nSlowErase: " << s.m_nSlowErase.get()               << "\n"
            << "\t\t            m_nFastExtract: " << s.m_nFastExtract.get()             << "\n"
            << "\t\t            m_nSlowExtract: " << s.m_nSlowExtract.get()             << "\n"
            << "\t\t         m_nEraseWhileFind: " << s.m_nEraseWhileFind.get()          << "\n"
            << "\t\t       m_nExtractWhileFind: " << s.m_nExtractWhileFind.get()        << "\n";
    }

    static inline ostream& operator <<( ostream& o, cds::intrusive::skip_list::empty_stat const& /*s*/ )
    {
        return o;
    }

} // namespace std

#endif // #ifndef CDSUNIT_PRINT_SKIP_LIST_STAT_H
