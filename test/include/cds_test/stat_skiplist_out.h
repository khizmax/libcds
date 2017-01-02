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

#ifndef CDSTEST_STAT_SKIPLIST_OUT_H
#define CDSTEST_STAT_SKIPLIST_OUT_H

#include <cds/intrusive/details/skip_list_base.h>

namespace cds_test {

    static inline property_stream& operator <<( property_stream& o, cds::intrusive::skip_list::empty_stat const& /*s*/ )
    {
        return o;
    }

    static inline property_stream& operator <<( property_stream& o, cds::intrusive::skip_list::stat<> const& s )
    {
        {
            std::stringstream stm;
            for ( unsigned int i = 0; i < sizeof( s.m_nNodeHeightAdd ) / sizeof( s.m_nNodeHeightAdd[0] ); ++i )
                stm << " +" << s.m_nNodeHeightAdd[i].get() << "/-" << s.m_nNodeHeightDel[i].get();
            o << CDSSTRESS_STAT_OUT_( "stat.level_ins_del", stm.str().substr( 1 ).c_str());
        }

        return o
            << CDSSTRESS_STAT_OUT( s, m_nInsertSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nInsertFailed )
            << CDSSTRESS_STAT_OUT( s, m_nInsertRetries )
            << CDSSTRESS_STAT_OUT( s, m_nUpdateExist )
            << CDSSTRESS_STAT_OUT( s, m_nUpdateNew )
            << CDSSTRESS_STAT_OUT( s, m_nUnlinkSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nUnlinkFailed )
            << CDSSTRESS_STAT_OUT( s, m_nExtractSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nExtractFailed )
            << CDSSTRESS_STAT_OUT( s, m_nExtractRetries )
            << CDSSTRESS_STAT_OUT( s, m_nExtractMinSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nExtractMinFailed )
            << CDSSTRESS_STAT_OUT( s, m_nExtractMinRetries )
            << CDSSTRESS_STAT_OUT( s, m_nExtractMaxSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nExtractMaxFailed )
            << CDSSTRESS_STAT_OUT( s, m_nExtractMaxRetries )
            << CDSSTRESS_STAT_OUT( s, m_nEraseSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nEraseFailed )
            << CDSSTRESS_STAT_OUT( s, m_nEraseRetry )
            << CDSSTRESS_STAT_OUT( s, m_nFindFastSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nFindFastFailed )
            << CDSSTRESS_STAT_OUT( s, m_nFindSlowSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nFindSlowFailed )
            << CDSSTRESS_STAT_OUT( s, m_nRenewInsertPosition )
            << CDSSTRESS_STAT_OUT( s, m_nLogicDeleteWhileInsert )
            << CDSSTRESS_STAT_OUT( s, m_nRemoveWhileInsert )
            << CDSSTRESS_STAT_OUT( s, m_nFastErase )
            << CDSSTRESS_STAT_OUT( s, m_nSlowErase )
            << CDSSTRESS_STAT_OUT( s, m_nFastExtract )
            << CDSSTRESS_STAT_OUT( s, m_nSlowExtract )
            << CDSSTRESS_STAT_OUT( s, m_nEraseWhileFind )
            << CDSSTRESS_STAT_OUT( s, m_nExtractWhileFind )
            << CDSSTRESS_STAT_OUT( s, m_nMarkFailed )
            << CDSSTRESS_STAT_OUT( s, m_nEraseContention );
    }

} // namespace cds_test

#endif // #ifndef CDSTEST_STAT_SKIPLIST_OUT_H
