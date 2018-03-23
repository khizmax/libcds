// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSTEST_STAT_BRONSONAVLTREE_OUT_H
#define CDSTEST_STAT_BRONSONAVLTREE_OUT_H

#include <cds_test/stress_test.h>
#include <cds/container/details/bronson_avltree_base.h>

namespace cds_test {

    static inline property_stream& operator <<( property_stream& o, cds::container::bronson_avltree::empty_stat const& /*s*/ )
    {
        return o;
    }

    static inline property_stream& operator <<( property_stream& o, cds::container::bronson_avltree::stat<> const& s )
    {
        return o
            << CDSSTRESS_STAT_OUT( s, m_nFindSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nFindFailed )
            << CDSSTRESS_STAT_OUT( s, m_nFindRetry )
            << CDSSTRESS_STAT_OUT( s, m_nFindWaitShrinking )
            << CDSSTRESS_STAT_OUT( s, m_nInsertSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nInsertFailed )
            << CDSSTRESS_STAT_OUT( s, m_nRelaxedInsertFailed )
            << CDSSTRESS_STAT_OUT( s, m_nInsertRetry )
            << CDSSTRESS_STAT_OUT( s, m_nUpdateWaitShrinking )
            << CDSSTRESS_STAT_OUT( s, m_nUpdateRetry )
            << CDSSTRESS_STAT_OUT( s, m_nUpdateRootWaitShrinking )
            << CDSSTRESS_STAT_OUT( s, m_nUpdateSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nUpdateUnlinked )
            << CDSSTRESS_STAT_OUT( s, m_nRemoveSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nRemoveFailed )
            << CDSSTRESS_STAT_OUT( s, m_nRemoveRetry )
            << CDSSTRESS_STAT_OUT( s, m_nExtractSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nExtractFailed )
            << CDSSTRESS_STAT_OUT( s, m_nRemoveWaitShrinking )
            << CDSSTRESS_STAT_OUT( s, m_nRemoveRootWaitShrinking )
            << CDSSTRESS_STAT_OUT( s, m_nMakeRoutingNode )
            << CDSSTRESS_STAT_OUT( s, m_nDisposedValue )
            << CDSSTRESS_STAT_OUT( s, m_nDisposedNode )
            << CDSSTRESS_STAT_OUT( s, m_nExtractedValue )
            << CDSSTRESS_STAT_OUT( s, m_nRightRotation )
            << CDSSTRESS_STAT_OUT( s, m_nLeftRotation )
            << CDSSTRESS_STAT_OUT( s, m_nLeftRightRotation )
            << CDSSTRESS_STAT_OUT( s, m_nRightLeftRotation )
            << CDSSTRESS_STAT_OUT( s, m_nInsertRebalanceReq )
            << CDSSTRESS_STAT_OUT( s, m_nRemoveRebalanceReq )
            << CDSSTRESS_STAT_OUT( s, m_nRotateAfterRightRotation )
            << CDSSTRESS_STAT_OUT( s, m_nRemoveAfterRightRotation )
            << CDSSTRESS_STAT_OUT( s, m_nDamageAfterRightRotation )
            << CDSSTRESS_STAT_OUT( s, m_nRotateAfterLeftRotation )
            << CDSSTRESS_STAT_OUT( s, m_nRemoveAfterLeftRotation )
            << CDSSTRESS_STAT_OUT( s, m_nDamageAfterLeftRotation )
            << CDSSTRESS_STAT_OUT( s, m_nRotateAfterRLRotation )
            << CDSSTRESS_STAT_OUT( s, m_nRemoveAfterRLRotation )
            << CDSSTRESS_STAT_OUT( s, m_nRotateAfterLRRotation )
            << CDSSTRESS_STAT_OUT( s, m_nRemoveAfterLRRotation );
    }
} //namespace cds_test

#endif // #ifndef CDSTEST_STAT_BRONSONAVLTREE_OUT_H
