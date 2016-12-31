/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

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
