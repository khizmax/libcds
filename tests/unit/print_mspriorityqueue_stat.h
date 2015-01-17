/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#ifndef __UNIT_PRINT_MSPRIORITYQUEUE_STAT_H
#define __UNIT_PRINT_MSPRIORITYQUEUE_STAT_H

#include <cds/container/mspriority_queue.h>

namespace std {
    static inline ostream& operator <<( ostream& o, cds::container::mspriority_queue::empty_stat const& /*s*/ )
    {
        return o;
    }

    static inline ostream& operator <<( ostream& o, cds::container::mspriority_queue::stat<> const& s )
    {
        return o << "\nMSPriorityQueue statistis [cds::container::mspriority_queue::stat]:\n"
            << "\t\t            Success push count: " << s.m_nPushCount.get() << "\n"
            << "\t\t             Success pop count: " << s.m_nPopCount.get() << "\n"
            << "\t\tFailed push count (full queue): " << s.m_nPushFailCount.get() << "\n"
            << "\t\tFailed pop count (empty queue): " << s.m_nPopFailCount.get() << "\n"
            << "\t\t          Heapify swap on push: " << s.m_nPushHeapifySwapCount.get() << "\n"
            << "\t\t           Heapify swap on pop: " << s.m_nPopHeapifySwapCount.get() << "\n";
    }
}

#endif // #ifndef __UNIT_PRINT_MSPRIORITYQUEUE_STAT_H
