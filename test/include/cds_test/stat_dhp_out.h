// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSTEST_STAT_DHP_OUT_H
#define CDSTEST_STAT_DHP_OUT_H

#include <cds/gc/dhp.h>
#include <ostream>

namespace cds_test {

    static inline property_stream& operator <<( property_stream& o, cds::gc::DHP::stat const& s )
    {
#ifdef CDS_ENABLE_HPSTAT
#   define CDS_HPSTAT_OUT( stat, fld ) std::make_pair( "dhp_" + property_stream::stat_prefix() + "." #fld, stat.fld )
        return o
            << CDS_HPSTAT_OUT( s, guard_allocated )
            << CDS_HPSTAT_OUT( s, guard_freed )
            << CDS_HPSTAT_OUT( s, retired_count )
            << CDS_HPSTAT_OUT( s, free_count )
            << CDS_HPSTAT_OUT( s, scan_count )
            << CDS_HPSTAT_OUT( s, help_scan_count )
            << CDS_HPSTAT_OUT( s, thread_rec_count )
            << CDS_HPSTAT_OUT( s, hp_block_count )
            << CDS_HPSTAT_OUT( s, retired_block_count )
            << CDS_HPSTAT_OUT( s, hp_extend_count )
            << CDS_HPSTAT_OUT( s, retired_extend_count );
#   undef CDS_HPSTAT_OUT
#else
        return o;
#endif
    }

} // namespace cds_test

static inline std::ostream& operator <<( std::ostream& o, cds::gc::DHP::stat const& s )
{
#ifdef CDS_ENABLE_HPSTAT
#   define CDS_HPSTAT_OUT( stat, fld ) "\t" << #fld << "=" << stat.fld << "\n"
    return o
        << "DHP post-mortem statistics:\n"
        << CDS_HPSTAT_OUT( s, guard_allocated )
        << CDS_HPSTAT_OUT( s, guard_freed )
        << CDS_HPSTAT_OUT( s, retired_count )
        << CDS_HPSTAT_OUT( s, free_count )
        << CDS_HPSTAT_OUT( s, scan_count )
        << CDS_HPSTAT_OUT( s, help_scan_count )
        << CDS_HPSTAT_OUT( s, thread_rec_count )
        << CDS_HPSTAT_OUT( s, hp_block_count )
        << CDS_HPSTAT_OUT( s, retired_block_count )
        << CDS_HPSTAT_OUT( s, hp_extend_count )
        << CDS_HPSTAT_OUT( s, retired_extend_count );
#   undef CDS_HPSTAT_OUT
#else
    return o;
#endif
}


#endif // #ifndef CDSTEST_STAT_DHP_OUT_H
