/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#ifndef __CDSUNIT_STD_PQUEUE_H
#define __CDSUNIT_STD_PQUEUE_H

#include <queue>
#include <mutex>    //unique_lock

namespace pqueue {

    struct dummy_stat {};

    template <typename T, typename Container, typename Lock, typename Less=std::less<typename Container::value_type> >
    class StdPQueue
    {
        typedef T value_type;
        typedef std::priority_queue<value_type, Container, Less> pqueue_type;

        pqueue_type     m_PQueue;
        mutable Lock    m_Lock;

        typedef std::unique_lock<Lock> scoped_lock;

    public:
        bool push( value_type const& val )
        {
            scoped_lock l( m_Lock );
            m_PQueue.push( val );
            return true;
        }

        bool pop( value_type& dest )
        {
            scoped_lock l( m_Lock );
            if ( !m_PQueue.empty() ) {
                dest = m_PQueue.top();
                m_PQueue.pop();
                return true;
            }
            return false;
        }

        template <typename Q, typename MoveFunc>
        bool pop_with( Q& dest, MoveFunc f )
        {
            scoped_lock l( m_Lock );
            if ( !m_PQueue.empty() ) {
                f( dest, m_PQueue.top());
                m_PQueue.pop();
                return true;
            }
            return false;
        }

        void clear()
        {
            scoped_lock l( m_Lock );
            while ( !m_PQueue.empty() )
                m_PQueue.pop();
        }

        template <typename Func>
        void clear_with( Func f )
        {
            scoped_lock l( m_Lock );
            while ( !m_PQueue.empty() ) {
                f( m_PQueue.top() );
                m_PQueue.pop();
            }
        }

        bool empty() const
        {
            return m_PQueue.empty();
        }

        size_t size() const
        {
            return m_PQueue.size();
        }

        dummy_stat statistics() const
        {
            return dummy_stat();
        }
    };

} // namespace pqueue

namespace std {
    static inline ostream& operator <<( ostream& o, pqueue::dummy_stat )
    {
        return o;
    }
}

#endif // #ifndef __CDSUNIT_STD_PQUEUE_H
