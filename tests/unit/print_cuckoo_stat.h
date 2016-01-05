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

#ifndef CDSUNIT_PRINT_CUCKOO_STAT_H
#define CDSUNIT_PRINT_CUCKOO_STAT_H

#include <cds/intrusive/cuckoo_set.h>
#include <ostream>

namespace std {

    static inline ostream& operator <<( ostream& o, cds::intrusive::cuckoo::striping_stat const& s )
    {
        return o << "\tStriping statistis [cds::intrusive::cuckoo::striping_stat]:\n"
            << "\t\t    m_nCellLockCount: " << s.m_nCellLockCount.get()     << "\n"
            << "\t\t m_nCellTryLockCount: " << s.m_nCellTryLockCount.get()  << "\n"
            << "\t\t    m_nFullLockCount: " << s.m_nFullLockCount.get()     << "\n"
            << "\t\t  m_nResizeLockCount: " << s.m_nResizeLockCount.get()   << "\n"
            << "\t\t      m_nResizeCount: " << s.m_nResizeCount.get()       << "\n"
;
    }
    static inline ostream& operator <<( ostream& o, cds::intrusive::cuckoo::empty_striping_stat const& /*s*/ )
    {
        return o;
    }

    static inline ostream& operator <<( ostream& o, cds::intrusive::cuckoo::refinable_stat const& s )
    {
        return o << "\tRefinable statistics [cds::intrusive::cuckoo::refinable_stat]:\n"
            << "\t\t          m_nCellLockCount: " << s.m_nCellLockCount.get()           << "\n"
            << "\t\t   m_nCellLockWaitResizing: " << s.m_nCellLockWaitResizing.get()    << "\n"
            << "\t\t   m_nCellLockArrayChanged: " << s.m_nCellLockArrayChanged.get()    << "\n"
            << "\t\t         m_nCellLockFailed: " << s.m_nCellLockFailed.get()          << "\n"
            << "\t\t    m_nSecondCellLockCount: " << s.m_nSecondCellLockCount.get()     << "\n"
            << "\t\t   m_nSecondCellLockFailed: " << s.m_nSecondCellLockFailed.get()    << "\n"
            << "\t\t          m_nFullLockCount: " << s.m_nFullLockCount.get()           << "\n"
            << "\t\t           m_nFullLockIter: " << s.m_nFullLockIter.get()            << "\n"
            << "\t\t        m_nResizeLockCount: " << s.m_nResizeLockCount.get()         << "\n"
            << "\t\t         m_nResizeLockIter: " << s.m_nResizeLockIter.get()          << "\n"
            << "\t\t m_nResizeLockArrayChanged: " << s.m_nResizeLockArrayChanged.get()  << "\n"
            << "\t\t            m_nResizeCount: " << s.m_nResizeCount.get()             << "\n"
;
    }
    static inline ostream& operator <<( ostream& o, cds::intrusive::cuckoo::empty_refinable_stat const& /*s*/ )
    {
        return o;
    }

    static inline ostream& operator <<( ostream& o, cds::intrusive::cuckoo::stat const& s )
    {
        return o << "\tCuckoo stat [cds::intrusive::cuckoo::stat]:\n"
            << "\t\t           m_nRelocateCallCount: " << s.m_nRelocateCallCount.get()              << "\n"
            << "\t\t          m_nRelocateRoundCount: " << s.m_nRelocateRoundCount.get()             << "\n"
            << "\t\t          m_nFalseRelocateCount: " << s.m_nFalseRelocateCount.get()             << "\n"
            << "\t\t        m_nSuccessRelocateCount: " << s.m_nSuccessRelocateCount.get()           << "\n"
            << "\t\t m_nRelocateAboveThresholdCount: " << s.m_nRelocateAboveThresholdCount.get()    << "\n"
            << "\t\t         m_nFailedRelocateCount: " << s.m_nFailedRelocateCount.get()            << "\n"
            << "\t\t             m_nResizeCallCount: " << s.m_nResizeCallCount.get()                << "\n"
            << "\t\t            m_nFalseResizeCount: " << s.m_nFalseResizeCount.get()               << "\n"
            << "\t\t       m_nResizeSuccessNodeMove: " << s.m_nResizeSuccessNodeMove.get()          << "\n"
            << "\t\t          m_nResizeRelocateCall: " << s.m_nResizeRelocateCall.get()             << "\n"
            << "\t\t               m_nInsertSuccess: " << s.m_nInsertSuccess.get()                  << "\n"
            << "\t\t                m_nInsertFailed: " << s.m_nInsertFailed.get()                   << "\n"
            << "\t\t           m_nInsertResizeCount: " << s.m_nInsertResizeCount.get()              << "\n"
            << "\t\t         m_nInsertRelocateCount: " << s.m_nInsertRelocateCount.get()            << "\n"
            << "\t\t         m_nInsertRelocateFault: " << s.m_nInsertRelocateFault.get()            << "\n"
            << "\t\t            m_nUpdateExistCount: " << s.m_nUpdateExistCount.get()               << "\n"
            << "\t\t          m_nUpdateSuccessCount: " << s.m_nUpdateSuccessCount.get()             << "\n"
            << "\t\t           m_nUpdateResizeCount: " << s.m_nUpdateResizeCount.get()              << "\n"
            << "\t\t         m_nUpdateRelocateCount: " << s.m_nUpdateRelocateCount.get()            << "\n"
            << "\t\t         m_nUpdateRelocateFault: " << s.m_nUpdateRelocateFault.get()            << "\n"
            << "\t\t               m_nUnlinkSuccess: " << s.m_nUnlinkSuccess.get()                  << "\n"
            << "\t\t                m_nUnlinkFailed: " << s.m_nUnlinkFailed.get()                   << "\n"
            << "\t\t                m_nEraseSuccess: " << s.m_nEraseSuccess.get()                   << "\n"
            << "\t\t                 m_nEraseFailed: " << s.m_nEraseFailed.get()                    << "\n"
            << "\t\t                 m_nFindSuccess: " << s.m_nFindSuccess.get()                    << "\n"
            << "\t\t                  m_nFindFailed: " << s.m_nFindFailed.get()                     << "\n"
            << "\t\t             m_nFindWithSuccess: " << s.m_nFindWithSuccess.get()                << "\n"
            << "\t\t              m_nFindWithFailed: " << s.m_nFindWithFailed.get()                 << "\n"
;
    }

    static inline ostream& operator <<( ostream& o, cds::intrusive::cuckoo::empty_stat const& /*s*/ )
    {
        return o;
    }

} // namespace std

#endif // #ifndef CDSUNIT_PRINT_CUCKOO_STAT_H
