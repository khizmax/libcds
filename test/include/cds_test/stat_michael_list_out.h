// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSTEST_STAT_MICHAEL_LIST_OUT_H
#define CDSTEST_STAT_MICHAEL_LIST_OUT_H

#include <cds/intrusive/details/michael_list_base.h>

namespace cds_test {

    static inline property_stream& operator <<( property_stream& o, cds::intrusive::michael_list::empty_stat const& /*s*/ )
    {
        return o;
    }

    static inline property_stream& operator <<( property_stream& o, cds::intrusive::michael_list::stat<> const& s )
    {
        return o
            << CDSSTRESS_STAT_OUT( s, m_nInsertSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nInsertFailed )
            << CDSSTRESS_STAT_OUT( s, m_nInsertRetry )
            << CDSSTRESS_STAT_OUT( s, m_nUpdateNew )
            << CDSSTRESS_STAT_OUT( s, m_nUpdateExisting )
            << CDSSTRESS_STAT_OUT( s, m_nUpdateFailed )
            << CDSSTRESS_STAT_OUT( s, m_nUpdateRetry )
            << CDSSTRESS_STAT_OUT( s, m_nUpdateMarked )
            << CDSSTRESS_STAT_OUT( s, m_nEraseSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nEraseFailed )
            << CDSSTRESS_STAT_OUT( s, m_nEraseRetry )
            << CDSSTRESS_STAT_OUT( s, m_nFindSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nFindFailed )
            << CDSSTRESS_STAT_OUT( s, m_nHelpingSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nHelpingFailed );
    }

    template <typename Stat>
    static inline property_stream& operator <<( property_stream& o, cds::intrusive::michael_list::wrapped_stat<Stat> const& s )
    {
        return o << s.m_stat;
    }

} // namespace cds_test

#endif // #ifndef CDSTEST_STAT_MICHAEL_LIST_OUT_H
