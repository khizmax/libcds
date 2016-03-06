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

    template <typename T, class Lock = cds::sync::spin >
    using StdQueue_deque = StdQueue<T, std::deque<T>, Lock >;

    template <typename T, class Lock = cds::sync::spin >
    using StdQueue_list = StdQueue<T, std::list<T>, Lock >;
}

#endif // #ifndef CDSUNIT_QUEUE_STD_QUEUE_H
