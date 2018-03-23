// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#if defined(CDSLIB_SYNC_INJECTING_MONITOR_H) && !defined(CDSUNIT_PRINT_INJECTING_MONITOR_STAT_H)
#define CDSUNIT_PRINT_INJECTING_MONITOR_STAT_H

#include <cds_test/stress_test.h>

namespace cds_test {
    static inline property_stream& operator <<( property_stream& o, cds::sync::injecting_monitor_traits::empty_stat const& /*s*/ )
    {
        return o;
    }
} // namespace cds_test
#endif

#if defined(CDSLIB_SYNC_POOL_MONITOR_H) && !defined(CDSUNIT_PRINT_POOL_MONITOR_STAT_H)
#define CDSUNIT_PRINT_POOL_MONITOR_STAT_H

#include <cds_test/stress_test.h>

namespace cds_test {
    static inline property_stream& operator <<( property_stream& o, cds::sync::pool_monitor_traits::empty_stat const& /*s*/ )
    {
        return o;
    }

#   define CDSSTRESS_POOLMONITOR_STAT_OUT( s, field ) CDSSTRESS_STAT_OUT_( "pool_monitor." #field, s.field.get())

    static inline property_stream& operator <<( property_stream& o, cds::sync::pool_monitor_traits::stat<> const& s )
    {
        return o
            << CDSSTRESS_POOLMONITOR_STAT_OUT( s, m_nLockCount )
            << CDSSTRESS_POOLMONITOR_STAT_OUT( s, m_nUnlockCount )
            << CDSSTRESS_POOLMONITOR_STAT_OUT( s, m_nMaxLocked )
            << CDSSTRESS_POOLMONITOR_STAT_OUT( s, m_nLockContention )
            << CDSSTRESS_POOLMONITOR_STAT_OUT( s, m_nUnlockContention )
            << CDSSTRESS_POOLMONITOR_STAT_OUT( s, m_nLockAllocation )
            << CDSSTRESS_POOLMONITOR_STAT_OUT( s, m_nLockDeallocation )
            << CDSSTRESS_POOLMONITOR_STAT_OUT( s, m_nMaxAllocated );
    }

#   undef CDSSTRESS_POOLMONITOR_STAT_OUT

} // namespace cds_test
#endif
