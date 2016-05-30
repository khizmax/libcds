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

#ifndef CDSSTRESS_QUEUE_PRINT_STAT_H
#define CDSSTRESS_QUEUE_PRINT_STAT_H

namespace cds_test {

    template <typename Counter>
    static inline property_stream& operator <<( property_stream& o, cds::intrusive::basket_queue::stat<Counter> const& s )
    {
        return o
            << CDSSTRESS_STAT_OUT( s, m_EnqueueCount )
            << CDSSTRESS_STAT_OUT( s, m_EnqueueRace )
            << CDSSTRESS_STAT_OUT( s, m_DequeueCount )
            << CDSSTRESS_STAT_OUT( s, m_EmptyDequeue )
            << CDSSTRESS_STAT_OUT( s, m_DequeueRace )
            << CDSSTRESS_STAT_OUT( s, m_AdvanceTailError )
            << CDSSTRESS_STAT_OUT( s, m_BadTail )
            << CDSSTRESS_STAT_OUT( s, m_TryAddBasket )
            << CDSSTRESS_STAT_OUT( s, m_AddBasketCount );
    }

    static inline property_stream& operator <<( property_stream& o, cds::intrusive::basket_queue::empty_stat const& /*s*/ )
    {
        return o;
    }

    template <typename Counter>
    static inline property_stream& operator <<( property_stream& o, cds::intrusive::msqueue::stat<Counter> const& s )
    {
        return o
            << CDSSTRESS_STAT_OUT( s, m_EnqueueCount )
            << CDSSTRESS_STAT_OUT( s, m_EnqueueRace )
            << CDSSTRESS_STAT_OUT( s, m_DequeueCount )
            << CDSSTRESS_STAT_OUT( s, m_EmptyDequeue )
            << CDSSTRESS_STAT_OUT( s, m_DequeueRace )
            << CDSSTRESS_STAT_OUT( s, m_AdvanceTailError )
            << CDSSTRESS_STAT_OUT( s, m_BadTail );
    }

    static inline property_stream& operator <<( property_stream& o, cds::intrusive::msqueue::empty_stat const& /*s*/ )
    {
        return o;
    }

    static inline property_stream& operator <<( property_stream& o, cds::opt::none )
    {
        return o;
    }

    template <typename Counter>
    static inline property_stream& operator <<( property_stream& o, cds::intrusive::optimistic_queue::stat<Counter> const& s )
    {
        return o
            << CDSSTRESS_STAT_OUT( s, m_EnqueueCount )
            << CDSSTRESS_STAT_OUT( s, m_EnqueueRace )
            << CDSSTRESS_STAT_OUT( s, m_DequeueCount )
            << CDSSTRESS_STAT_OUT( s, m_EmptyDequeue )
            << CDSSTRESS_STAT_OUT( s, m_DequeueRace )
            << CDSSTRESS_STAT_OUT( s, m_AdvanceTailError )
            << CDSSTRESS_STAT_OUT( s, m_BadTail )
            << CDSSTRESS_STAT_OUT( s, m_FixListCount );
    }

    static inline property_stream& operator <<( property_stream& o, cds::intrusive::optimistic_queue::empty_stat const& /*s*/ )
    {
        return o;
    }

    static inline property_stream& operator <<( property_stream& o, std::nullptr_t /*s*/ )
    {
        return o;
    }

    static inline property_stream& operator <<( property_stream& o, cds::intrusive::segmented_queue::stat<> const& s )
    {
        return o
            << CDSSTRESS_STAT_OUT( s, m_nPush )
            << CDSSTRESS_STAT_OUT( s, m_nPushPopulated )
            << CDSSTRESS_STAT_OUT( s, m_nPushContended )
            << CDSSTRESS_STAT_OUT( s, m_nPop )
            << CDSSTRESS_STAT_OUT( s, m_nPopEmpty )
            << CDSSTRESS_STAT_OUT( s, m_nPopContended )
            << CDSSTRESS_STAT_OUT( s, m_nCreateSegmentReq )
            << CDSSTRESS_STAT_OUT( s, m_nDeleteSegmentReq )
            << CDSSTRESS_STAT_OUT( s, m_nSegmentCreated )
            << CDSSTRESS_STAT_OUT( s, m_nSegmentDeleted );
    }

    static inline property_stream& operator <<( property_stream& o, cds::intrusive::segmented_queue::empty_stat const& /*s*/ )
    {
        return o;
    }

} // namespace cds_test

#endif // CDSSTRESS_QUEUE_PRINT_STAT_H
