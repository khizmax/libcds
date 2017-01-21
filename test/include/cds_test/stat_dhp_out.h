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
