//$$CDS-header$$

#ifndef __UNIT_PRINT_DEQUE_STAT_H
#define __UNIT_PRINT_DEQUE_STAT_H

#include <cds/intrusive/michael_deque.h>
#include <ostream>

namespace std {

    inline ostream& operator <<( ostream& o, cds::intrusive::deque_stat<> const& s )
    {
        return o << "\tStatistics:\n"
            << "\t\t           Push front: " << s.m_PushFrontCount.get()              << "\n"
            << "\t\t            Push back: " << s.m_PushBackCount.get()               << "\n"
            << "\t\t            Pop front: " << s.m_PopFrontCount.get()               << "\n"
            << "\t\t             Pop back: " << s.m_PopBackCount.get()                << "\n"
            << "\t\tPush front contention: " << s.m_PushFrontContentionCount.get()    << "\n"
            << "\t\t Push back contention: " << s.m_PushBackContentionCount.get()     << "\n"
            << "\t\t Pop front contention: " << s.m_PopFrontContentionCount.get()     << "\n"
            << "\t\t  Pop back contention: " << s.m_PopBackContentionCount.get()      << "\n"
            << "\t\t            Pop empty: " << s.m_PopEmptyCount.get()               << "\n"
;
    }

    inline ostream& operator <<( ostream& o, cds::intrusive::michael_deque::stat<> const& s )
    {
        return o << static_cast<cds::intrusive::deque_stat<> const&>( s )
            << "\t\t      Stabilize front: " << s.m_StabilizeFrontCount.get() << "\n"
            << "\t\t       Stabilize back: " << s.m_StabilizeBackCount.get()  << "\n"
;
    }

    inline ostream& operator <<( ostream& o, cds::intrusive::michael_deque::dummy_stat const&  )
    {
        return o;
    }

}   // namespace std

#endif // #ifndef __UNIT_PRINT_DEQUE_STAT_H
