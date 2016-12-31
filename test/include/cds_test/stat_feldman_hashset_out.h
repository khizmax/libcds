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

#ifndef CDSTEST_STAT_FELDMAN_HASHSET_OUT_H
#define CDSTEST_STAT_FELDMAN_HASHSET_OUT_H

#include <cds_test/stress_test.h>
#include <cds/intrusive/details/feldman_hashset_base.h>

namespace cds_test {

    static inline property_stream& operator <<( property_stream& o, cds::intrusive::feldman_hashset::empty_stat const& /*s*/ )
    {
        return o;
    }

    static inline property_stream& operator <<( property_stream& o, cds::intrusive::feldman_hashset::stat<> const& s )
    {
        return o
            << CDSSTRESS_STAT_OUT( s, m_nInsertSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nInsertFailed )
            << CDSSTRESS_STAT_OUT( s, m_nInsertRetry )
            << CDSSTRESS_STAT_OUT( s, m_nUpdateNew )
            << CDSSTRESS_STAT_OUT( s, m_nUpdateExisting )
            << CDSSTRESS_STAT_OUT( s, m_nUpdateFailed )
            << CDSSTRESS_STAT_OUT( s, m_nUpdateRetry )
            << CDSSTRESS_STAT_OUT( s, m_nEraseSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nEraseFailed )
            << CDSSTRESS_STAT_OUT( s, m_nEraseRetry )
            << CDSSTRESS_STAT_OUT( s, m_nFindSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nFindFailed )
            << CDSSTRESS_STAT_OUT( s, m_nExpandNodeSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nExpandNodeFailed )
            << CDSSTRESS_STAT_OUT( s, m_nSlotChanged )
            << CDSSTRESS_STAT_OUT( s, m_nSlotConverting )
            << CDSSTRESS_STAT_OUT( s, m_nArrayNodeCount )
            << CDSSTRESS_STAT_OUT( s, m_nHeight );
    }

    static inline property_stream& operator<<( property_stream& o, std::vector< cds::intrusive::feldman_hashset::level_statistics > const& level_stat )
    {
#       define CDSSTRESS_LEVELSTAT_OUT( level, field ) \
            CDSSTRESS_STAT_OUT_( "level_stat." + std::to_string(level) + "." + #field, it->field )

        o << CDSSTRESS_STAT_OUT_( "stat.level_count", level_stat.size());
        size_t i = 0;
        for ( auto it = level_stat.begin(); it != level_stat.end(); ++it, ++i ) {
            o << CDSSTRESS_LEVELSTAT_OUT( i, array_node_count )
              << CDSSTRESS_LEVELSTAT_OUT( i, node_capacity )
              << CDSSTRESS_LEVELSTAT_OUT( i, data_cell_count )
              << CDSSTRESS_LEVELSTAT_OUT( i, array_cell_count )
              << CDSSTRESS_LEVELSTAT_OUT( i, empty_cell_count );
        }
        return o;
#       undef CDSSTRESS_LEVELSTAT_OUT
    }

} // namespace std

#endif // #ifndef CDSTEST_STAT_FELDMAN_HASHSET_OUT_H
