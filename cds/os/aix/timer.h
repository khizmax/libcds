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

#ifndef CDSLIB_OS_AIX_TIMER_H
#define CDSLIB_OS_AIX_TIMER_H

// Source: http://publib16.boulder.ibm.com/doc_link/en_US/a_doc_lib/libs/basetrf2/read_real_time.htm

#ifndef CDSLIB_OS_TIMER_H
#   error "<cds/os/timer.h> must be included"
#endif

#include <sys/time.h>
#include <sys/systemcfg.h>

//@cond none
namespace cds { namespace OS {
    CDS_CXX11_INLINE_NAMESPACE namespace Aix {

        // High resolution timer
        class Timer {
            timebasestruct_t    m_tmStart;

        protected:
            static unsigned long long nano( native_timer_type const& nStart, native_timer_type const& nEnd )
            {
                return (((unsigned long long) (nEnd.tb_high - nStart.tb_high)) << 32) + (nEnd.tb_low - nStart.tb_low);
            }

        public:
            typedef timebasestruct_t    native_timer_type;
            typedef long long            native_duration_type;

            Timer() : m_tmStart( current() ) {}

            static native_timer_type    current()
            {
                native_timer_type tm;
                current( &tm );
                return tm;
            }
            static void current( native_timer_type& tmr )
            {
                read_real_time( &tmr, sizeof(tmr) );
                time_base_to_time( &tmr, sizeof(tmr ));
            }

            double reset()
            {
                native_timer_type tt;
                current( tt );
                double ret = nano( m_tmStart, tt ) / 1.0E9;
                m_tmStart = tt;
                return ret;
            }

            double duration( native_duration_type dur )
            {
                return double( dur ) / 1.0E9;
            }

            double duration()
            {
                return duration( native_duration() );
            }

            native_duration_type    native_duration()
            {
                return native_duration( m_tmStart, current() );
            }

            static native_duration_type    native_duration( native_timer_type const & nStart, native_timer_type const & nEnd )
            {
                return nano( nStart, nEnd );
            }

            static unsigned long long random_seed()
            {
                native_timer_type tmr;
                read_real_time( &tmr, sizeof(tmr) );
                return ( ((unsigned long long)(tmr.tb_hight)) << 32 ) + tmr.tb_low;
            }
        };
    }    // namespace Aix

#ifndef CDS_CXX11_INLINE_NAMESPACE_SUPPORT
    typedef Aix::Timer    Timer;
#endif

}}   // namespace cds::OS
//@endcond

#endif // #ifndef CDSLIB_OS_AIX_TIMER_H
