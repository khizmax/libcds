// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

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
