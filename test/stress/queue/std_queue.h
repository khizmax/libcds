// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_QUEUE_STD_QUEUE_H
#define CDSUNIT_QUEUE_STD_QUEUE_H

#include <mutex>    //unique_lock
#include <queue>
#include <list>
#include <cds/sync/spinlock.h>

namespace queue {

    template <typename T, class Container, class Lock = cds::sync::spin >
    class StdQueue: public std::queue<T, Container >
    {
        typedef std::queue<T, Container >   base_class;
        mutable Lock m_Locker;

    public:
        bool enqueue( const T& data )
        {
            std::unique_lock<Lock> a(m_Locker);

            base_class::push( data );
            return true;
        }

        bool push( const T& data )
        {
            return enqueue( data );
        }

        bool dequeue( T& data )
        {
            std::unique_lock<Lock> a(m_Locker);
            if ( base_class::empty())
                return false;

            data = base_class::front();
            base_class::pop();
            return true;
        }

        bool pop( T& data )
        {
            return dequeue( data );
        }

        bool empty() const
        {
            std::unique_lock<Lock> a( m_Locker );
            return base_class::empty();
        }

        size_t size() const
        {
            std::unique_lock<Lock> a( m_Locker );
            return base_class::size();
        }

        cds::opt::none statistics() const
        {
            return cds::opt::none();
        }
    };

    template <typename T, class Lock = cds::sync::spin >
    using StdQueue_deque = StdQueue<T, std::deque<T>, Lock >;

    template <typename T, class Lock = cds::sync::spin >
    using StdQueue_list = StdQueue<T, std::list<T>, Lock >;
}

#endif // #ifndef CDSUNIT_QUEUE_STD_QUEUE_H
