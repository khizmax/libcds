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
