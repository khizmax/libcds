// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSTEST_STAT_CUCKOO_OUT_H
#define CDSTEST_STAT_CUCKOO_OUT_H

#include <cds/intrusive/cuckoo_set.h>

namespace cds_test {

    static inline property_stream& operator <<( property_stream& o, cds::intrusive::cuckoo::striping_stat const& s )
    {
        return o
            << CDSSTRESS_STAT_OUT( s, m_nCellLockCount )
            << CDSSTRESS_STAT_OUT( s, m_nCellLockCount )
            << CDSSTRESS_STAT_OUT( s, m_nCellTryLockCount )
            << CDSSTRESS_STAT_OUT( s, m_nFullLockCount )
            << CDSSTRESS_STAT_OUT( s, m_nResizeLockCount )
            << CDSSTRESS_STAT_OUT( s, m_nResizeCount );
    }

    static inline property_stream& operator <<( property_stream& o, cds::intrusive::cuckoo::empty_striping_stat const& /*s*/ )
    {
        return o;
    }

    static inline property_stream& operator <<( property_stream& o, cds::intrusive::cuckoo::refinable_stat const& s )
    {
        return o
            << CDSSTRESS_STAT_OUT( s, m_nCellLockCount )
            << CDSSTRESS_STAT_OUT( s, m_nCellLockWaitResizing )
            << CDSSTRESS_STAT_OUT( s, m_nCellLockArrayChanged )
            << CDSSTRESS_STAT_OUT( s, m_nCellLockFailed )
            << CDSSTRESS_STAT_OUT( s, m_nSecondCellLockCount )
            << CDSSTRESS_STAT_OUT( s, m_nSecondCellLockFailed )
            << CDSSTRESS_STAT_OUT( s, m_nFullLockCount )
            << CDSSTRESS_STAT_OUT( s, m_nFullLockIter )
            << CDSSTRESS_STAT_OUT( s, m_nResizeLockCount )
            << CDSSTRESS_STAT_OUT( s, m_nResizeLockIter )
            << CDSSTRESS_STAT_OUT( s, m_nResizeLockArrayChanged )
            << CDSSTRESS_STAT_OUT( s, m_nResizeCount );
    }

    static inline property_stream& operator <<( property_stream& o, cds::intrusive::cuckoo::empty_refinable_stat const& /*s*/ )
    {
        return o;
    }

    static inline property_stream& operator <<( property_stream& o, cds::intrusive::cuckoo::stat const& s )
    {
        return o
            << CDSSTRESS_STAT_OUT( s, m_nRelocateCallCount )
            << CDSSTRESS_STAT_OUT( s, m_nRelocateRoundCount )
            << CDSSTRESS_STAT_OUT( s, m_nFalseRelocateCount )
            << CDSSTRESS_STAT_OUT( s, m_nSuccessRelocateCount )
            << CDSSTRESS_STAT_OUT( s, m_nRelocateAboveThresholdCount )
            << CDSSTRESS_STAT_OUT( s, m_nFailedRelocateCount )
            << CDSSTRESS_STAT_OUT( s, m_nResizeCallCount )
            << CDSSTRESS_STAT_OUT( s, m_nFalseResizeCount )
            << CDSSTRESS_STAT_OUT( s, m_nResizeSuccessNodeMove )
            << CDSSTRESS_STAT_OUT( s, m_nResizeRelocateCall )
            << CDSSTRESS_STAT_OUT( s, m_nInsertSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nInsertFailed )
            << CDSSTRESS_STAT_OUT( s, m_nInsertResizeCount )
            << CDSSTRESS_STAT_OUT( s, m_nInsertRelocateCount )
            << CDSSTRESS_STAT_OUT( s, m_nInsertRelocateFault )
            << CDSSTRESS_STAT_OUT( s, m_nUpdateExistCount )
            << CDSSTRESS_STAT_OUT( s, m_nUpdateSuccessCount )
            << CDSSTRESS_STAT_OUT( s, m_nUpdateResizeCount )
            << CDSSTRESS_STAT_OUT( s, m_nUpdateRelocateCount )
            << CDSSTRESS_STAT_OUT( s, m_nUpdateRelocateFault )
            << CDSSTRESS_STAT_OUT( s, m_nUnlinkSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nUnlinkFailed )
            << CDSSTRESS_STAT_OUT( s, m_nEraseSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nEraseFailed )
            << CDSSTRESS_STAT_OUT( s, m_nFindSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nFindFailed )
            << CDSSTRESS_STAT_OUT( s, m_nFindWithSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nFindWithFailed );
    }

    static inline property_stream& operator <<( property_stream& o, cds::intrusive::cuckoo::empty_stat const& /*s*/ )
    {
        return o;
    }

} // namespace cds_test

#endif // #ifndef CDSTEST_STAT_CUCKOO_OUT_H
