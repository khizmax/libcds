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
