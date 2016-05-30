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

#   define CDSSTRESS_POOLMONITOR_STAT_OUT( s, field ) CDSSTRESS_STAT_OUT_( "pool_monitor." #field, s.field.get() )

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
