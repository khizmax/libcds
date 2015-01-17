/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#ifndef __UNIT_QUEUE_STD_QUEUE_H
#define __UNIT_QUEUE_STD_QUEUE_H

#include <mutex>    //unique_lock
#include <queue>
#include <cds/lock/spinlock.h>

namespace queue {

    template <typename T, class Container, class Lock = cds::lock::Spin >
    class StdQueue: public std::queue<T, Container >
    {
        typedef std::queue<T, Container >   base_class;
        Lock    m_Locker;

    public:
        bool enqueue( const T& data )
        {
            std::unique_lock<Lock> a(m_Locker);

            base_class::push( data );
            return true;
        }
        bool push( const T& data )  { return enqueue( data ) ; }
        bool dequeue( T& data )
        {
            std::unique_lock<Lock> a(m_Locker);
            if ( base_class::empty() )
                return false;

            data = base_class::front();
            base_class::pop();
            return true;
        }
        bool pop( T& data )         { return dequeue( data ) ; }

        cds::opt::none statistics() const
        {
            return cds::opt::none();
        }
    };

    template <typename T, class Lock = cds::lock::Spin >
    class StdQueue_deque: public StdQueue<T, std::deque<T>, Lock >
    {};

    template <typename T, class Lock = cds::lock::Spin >
    class StdQueue_list: public StdQueue<T, std::list<T>, Lock >
    {};
}

#endif // #ifndef __UNIT_QUEUE_STD_QUEUE_H
