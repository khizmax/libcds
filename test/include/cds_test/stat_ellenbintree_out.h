// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSTEST_STAT_ELLENBINTREE_OUT_H
#define CDSTEST_STAT_ELLENBINTREE_OUT_H

//#include "ellen_bintree_update_desc_pool.h"
#include <cds_test/stress_test.h>
#include <cds/intrusive/details/ellen_bintree_base.h>

namespace cds_test {
    static inline property_stream& operator <<( property_stream& o, cds::intrusive::ellen_bintree::empty_stat const& /*s*/ )
    {
        return o;
    }

    static inline property_stream& operator <<( property_stream& o, cds::intrusive::ellen_bintree::stat<> const& s )
    {
        return o
            // << "\t\t   Internal node allocated: " << ellen_bintree_pool::internal_node_counter::m_nAlloc.get() << "\n"
            // << "\t\t       Internal node freed: " << ellen_bintree_pool::internal_node_counter::m_nFree.get() << "\n"
            << CDSSTRESS_STAT_OUT( s, m_nInternalNodeCreated )
            << CDSSTRESS_STAT_OUT( s, m_nInternalNodeDeleted )
            << CDSSTRESS_STAT_OUT( s, m_nUpdateDescCreated )
            << CDSSTRESS_STAT_OUT( s, m_nUpdateDescDeleted )
            << CDSSTRESS_STAT_OUT( s, m_nInsertSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nInsertFailed )
            << CDSSTRESS_STAT_OUT( s, m_nInsertRetries )
            << CDSSTRESS_STAT_OUT( s, m_nUpdateExist )
            << CDSSTRESS_STAT_OUT( s, m_nUpdateNew )
            << CDSSTRESS_STAT_OUT( s, m_nUpdateRetries )
            << CDSSTRESS_STAT_OUT( s, m_nEraseSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nEraseFailed )
            << CDSSTRESS_STAT_OUT( s, m_nEraseRetries )
            << CDSSTRESS_STAT_OUT( s, m_nFindSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nFindFailed )
            << CDSSTRESS_STAT_OUT( s, m_nExtractMinSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nExtractMinFailed )
            << CDSSTRESS_STAT_OUT( s, m_nExtractMinRetries )
            << CDSSTRESS_STAT_OUT( s, m_nExtractMaxSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nExtractMaxFailed )
            << CDSSTRESS_STAT_OUT( s, m_nExtractMaxRetries )
            << CDSSTRESS_STAT_OUT( s, m_nSearchRetry )
            << CDSSTRESS_STAT_OUT( s, m_nHelpInsert )
            << CDSSTRESS_STAT_OUT( s, m_nHelpDelete )
            << CDSSTRESS_STAT_OUT( s, m_nHelpMark )
            << CDSSTRESS_STAT_OUT( s, m_nHelpGuardSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nHelpGuardFailed );
    }
} // namespace cds_test

#endif // #ifndef CDSTEST_STAT_ELLENBINTREE_OUT_H
