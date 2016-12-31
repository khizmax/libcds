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

#ifndef CDSTEST_STAT_LAZY_LIST_OUT_H
#define CDSTEST_STAT_LAZY_LIST_OUT_H

#include <cds/intrusive/details/lazy_list_base.h>

namespace cds_test {

    static inline property_stream& operator <<( property_stream& o, cds::intrusive::lazy_list::empty_stat const& /*s*/ )
    {
        return o;
    }

    static inline property_stream& operator <<( property_stream& o, cds::intrusive::lazy_list::stat<> const& s )
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
            << CDSSTRESS_STAT_OUT( s, m_nValidationSuccess )
            << CDSSTRESS_STAT_OUT( s, m_nValidationFailed );
    }

    template <typename Stat>
    static inline property_stream& operator <<( property_stream& o, cds::intrusive::lazy_list::wrapped_stat<Stat> const& s )
    {
        return o << s.m_stat;
    }

} // namespace cds_test

#endif // #ifndef CDSTEST_STAT_LAZY_LIST_OUT_H
