// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSTEST_STAT_FLAT_COMBINING_OUT_H
#define CDSTEST_STAT_FLAT_COMBINING_OUT_H

#include <cds/algo/flat_combining.h>

namespace cds_test {

    static inline property_stream& operator <<( property_stream& o, cds::algo::flat_combining::empty_stat const& /*s*/ )
    {
        return o;
    }

    static inline property_stream& operator <<( property_stream& o, cds::algo::flat_combining::stat<> const& s )
    {
        return o
            << CDSSTRESS_STAT_OUT_( "combining_factor", s.combining_factor())
            << CDSSTRESS_STAT_OUT( s, m_nOperationCount )
            << CDSSTRESS_STAT_OUT( s, m_nCombiningCount )
            << CDSSTRESS_STAT_OUT( s, m_nCompactPublicationList )
            << CDSSTRESS_STAT_OUT( s, m_nDeactivatePubRecord )
            << CDSSTRESS_STAT_OUT( s, m_nActivatePubRecord )
            << CDSSTRESS_STAT_OUT( s, m_nPubRecordCreated )
            << CDSSTRESS_STAT_OUT( s, m_nPubRecordDeleted )
            << CDSSTRESS_STAT_OUT( s, m_nPassiveWaitCall )
            << CDSSTRESS_STAT_OUT( s, m_nPassiveWaitIteration )
            << CDSSTRESS_STAT_OUT( s, m_nPassiveWaitWakeup )
            << CDSSTRESS_STAT_OUT( s, m_nInvokeExclusive )
            << CDSSTRESS_STAT_OUT( s, m_nWakeupByNotifying )
            << CDSSTRESS_STAT_OUT( s, m_nPassiveToCombiner );
    }

} // namespace cds_test

#endif // #ifndef CDSTEST_STAT_SPLITLIST_OUT_H
