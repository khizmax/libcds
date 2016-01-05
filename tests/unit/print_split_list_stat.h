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

#ifndef CDSUNIT_PRINT_SPLIT_LIST_STAT_H
#define CDSUNIT_PRINT_SPLIT_LIST_STAT_H

#include <cds/intrusive/details/split_list_base.h>
#include <ostream>

namespace std {

    static inline ostream& operator <<( ostream& o, cds::intrusive::split_list::stat<> const& s )
    {
        return
        o << "Split-list stat [cds::intrusive::split_list::stat]\n"
            << "\t\t          m_nInsertSuccess: " << s.m_nInsertSuccess.get()           << "\n"
            << "\t\t           m_nInsertFailed: " << s.m_nInsertFailed.get()            << "\n"
            << "\t\t            m_nEnsureExist: " << s.m_nEnsureExist.get()             << "\n"
            << "\t\t              m_nEnsureNew: " << s.m_nEnsureNew.get()               << "\n"
            << "\t\t           m_nEraseSuccess: " << s.m_nEraseSuccess.get()            << "\n"
            << "\t\t            m_nEraseFailed: " << s.m_nEraseFailed.get()             << "\n"
            << "\t\t         m_nExtractSuccess: " << s.m_nExtractSuccess.get()          << "\n"
            << "\t\t          m_nExtractFailed: " << s.m_nExtractFailed.get()           << "\n"
            << "\t\t            m_nFindSuccess: " << s.m_nFindSuccess.get()             << "\n"
            << "\t\t      m_nHeadNodeAllocated: " << s.m_nHeadNodeAllocated.get()       << "\n"
            << "\t\t          m_nHeadNodeFreed: " << s.m_nHeadNodeFreed.get()           << "\n"
            << "\t\t            m_nBucketCount: " << s.m_nBucketCount.get()             << "\n"
            << "\t\t    m_nInitBucketRecursive: " << s.m_nInitBucketRecursive.get()     << "\n"
            << "\t\t   m_nInitBucketContention: " << s.m_nInitBucketContention.get()    << "\n"
            << "\t\t     m_nBusyWaitBucketInit: " << s.m_nBusyWaitBucketInit.get()      << "\n";
    }

    static inline ostream& operator <<( ostream& o, cds::intrusive::split_list::empty_stat const& /*s*/ )
    {
        return o;
    }

} // namespace std

#endif // #ifndef CDSUNIT_PRINT_SKIP_LIST_STAT_H
