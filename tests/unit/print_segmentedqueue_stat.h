/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#ifndef __UNIT_PRINT_SEGMENTEDQUEUE_STAT_H
#define __UNIT_PRINT_SEGMENTEDQUEUE_STAT_H

#include <ostream>

namespace std {
    static inline std::ostream& operator <<( std::ostream& o, cds::intrusive::segmented_queue::stat<> const& s )
    {
        return o << "\tStatistics:\n"
            << "\t                    Push: " << s.m_nPush.get()           << "\n"
            << "\t          Push populated: " << s.m_nPushPopulated.get()  << "\n"
            << "\tPush failed (contention): " << s.m_nPushContended.get()  << "\n"
            << "\t                     Pop: " << s.m_nPop.get()            << "\n"
            << "\t               Pop empty: " << s.m_nPopEmpty.get()       << "\n"
            << "\t Pop failed (contention): " << s.m_nPopContended.get()      << "\n"
            << "\t Segment create requests: " << s.m_nCreateSegmentReq.get()  << "\n"
            << "\t Segment delete requests: " << s.m_nDeleteSegmentReq.get()  << "\n"
            << "\t         Segment created: " << s.m_nSegmentCreated.get()    << "\n"
            << "\t         Segment deleted: " << s.m_nSegmentDeleted.get()    << "\n";
    }

    static inline ostream& operator <<( ostream& o, cds::intrusive::segmented_queue::empty_stat const& /*s*/ )
    {
        return o;
    }

} // namespace std

#endif // #ifndef __UNIT_PRINT_SEGMENTEDQUEUE_STAT_H
