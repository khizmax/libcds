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

#ifndef CDSUNIT_PRINT_SEGMENTEDQUEUE_STAT_H
#define CDSUNIT_PRINT_SEGMENTEDQUEUE_STAT_H

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

#endif // #ifndef CDSUNIT_PRINT_SEGMENTEDQUEUE_STAT_H
