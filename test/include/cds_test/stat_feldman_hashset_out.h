// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

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
