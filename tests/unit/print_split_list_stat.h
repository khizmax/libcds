//$$CDS-header$$

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
