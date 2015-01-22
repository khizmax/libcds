//$$CDS-header$$

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
            << "\t\t            m_nEnsureExistCount: " << s.m_nEnsureExistCount.get()               << "\n"
            << "\t\t          m_nEnsureSuccessCount: " << s.m_nEnsureSuccessCount.get()             << "\n"
            << "\t\t           m_nEnsureResizeCount: " << s.m_nEnsureResizeCount.get()              << "\n"
            << "\t\t         m_nEnsureRelocateCount: " << s.m_nEnsureRelocateCount.get()            << "\n"
            << "\t\t         m_nEnsureRelocateFault: " << s.m_nEnsureRelocateFault.get()            << "\n"
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
