//$$CDS-header$$

#ifndef CDSUNIT_PRINT_MSPRIORITYQUEUE_STAT_H
#define CDSUNIT_PRINT_MSPRIORITYQUEUE_STAT_H

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

#endif // #ifndef CDSUNIT_PRINT_MSPRIORITYQUEUE_STAT_H
