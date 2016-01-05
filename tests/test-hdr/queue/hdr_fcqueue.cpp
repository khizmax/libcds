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

#include <cds/container/fcqueue.h>
#include "queue/hdr_fcqueue.h"

#include <list>

namespace queue {

    void HdrFCQueue::FCQueue_deque()
    {
        typedef cds::container::FCQueue<int> queue_type;
        testFCQueue<queue_type>();
    }

    void HdrFCQueue::FCQueue_deque_elimination()
    {
        typedef cds::container::FCQueue<int, std::queue< int, std::deque<int> >,
            cds::container::fcqueue::make_traits<
                cds::opt::enable_elimination< true >
            >::type
        > queue_type;
        testFCQueue<queue_type>();
    }

    void HdrFCQueue::FCQueue_deque_mutex()
    {
        typedef cds::container::FCQueue<int, std::queue< int, std::deque<int> >,
            cds::container::fcqueue::make_traits<
                cds::opt::lock_type< std::mutex >
            >::type
        > queue_type;
        testFCQueue<queue_type>();
    }

    void HdrFCQueue::FCQueue_deque_stat()
    {
        typedef cds::container::FCQueue<int, std::queue< int, std::deque<int> >,
            cds::container::fcqueue::make_traits<
                cds::opt::stat< cds::container::fcqueue::stat<> >
            >::type
        > queue_type;
        testFCQueue<queue_type>();
    }

    //
    void HdrFCQueue::FCQueue_list()
    {
        typedef cds::container::FCQueue<int, std::queue< int, std::list<int> > > queue_type;
        testFCQueue<queue_type>();
    }

    void HdrFCQueue::FCQueue_list_elimination()
    {
        typedef cds::container::FCQueue<int, std::queue< int, std::list<int> >,
            cds::container::fcqueue::make_traits<
                cds::opt::enable_elimination< true >
            >::type
        > queue_type;
        testFCQueue<queue_type>();
    }

    void HdrFCQueue::FCQueue_list_mutex()
    {
        typedef cds::container::FCQueue<int, std::queue<int, std::list<int> >,
            cds::container::fcqueue::make_traits<
                cds::opt::lock_type< std::mutex >
            >::type
        > queue_type;
        testFCQueue<queue_type>();
    }

    void HdrFCQueue::FCQueue_list_stat()
    {
        struct queue_traits : public cds::container::fcqueue::traits
        {
            typedef cds::container::fcqueue::stat<> stat;
        };
        typedef cds::container::FCQueue<int, std::queue< int, std::list<int> >, queue_traits > queue_type;
        testFCQueue<queue_type>();
    }

} // namespace queue
