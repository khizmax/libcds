// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_OS_OSX_TIMER_H
#define CDSLIB_OS_OSX_TIMER_H

#ifndef CDSLIB_OS_TIMER_H
#   error "<cds/os/timer.h> must be included"
#endif

#include <time.h>
#include <sys/time.h>
#include <mach/clock.h>
#include <mach/mach.h>

// From http://stackoverflow.com/questions/11680461/monotonic-clock-on-osx

//@cond none
namespace cds { namespace OS {
    inline namespace OS_X {

        // High resolution timer
        class Timer {
        public:
            typedef mach_timespec_t native_timer_type;
            typedef long long       native_duration_type;

        private:
            native_timer_type    m_tmStart;
            clock_serv_t         m_Service;

        public:

            Timer()
            {
                host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &m_Service);
                current( m_tmStart );
            }
            ~Timer()
            {
                mach_port_deallocate(mach_task_self(), m_Service);
            }

            void current( native_timer_type& tmr )
            {
                clock_get_time(m_Service, &tmr);
            }

            native_timer_type    current()
            {
                native_timer_type    tmr;
                current(tmr);
                return tmr;
            }

            double reset()
            {
                native_timer_type ts;
                current( ts );
                double dblRet = ( ts.tv_sec - m_tmStart.tv_sec ) + ( ts.tv_nsec - m_tmStart.tv_nsec ) / 1.0E9;
                m_tmStart = ts;
                return dblRet;
            }

            double duration( native_duration_type dur )
            {
                return double( dur ) / 1.0E9;
            }

            double duration()
            {
                return duration( native_duration());
            }

            native_duration_type    native_duration()
            {
                native_timer_type ts;
                current( ts );
                return native_duration( m_tmStart, ts );
            }

            static native_duration_type    native_duration( const native_timer_type& nStart, const native_timer_type& nEnd )
            {
                return native_duration_type( nEnd.tv_sec - nStart.tv_sec ) * 1000000000 + ( nEnd.tv_nsec - nStart.tv_nsec);
            }

            static unsigned long long random_seed()
            {
                native_timer_type tmr;
                Timer tm;
                tm.current( tmr );
                return ( ((unsigned long long)(tmr.tv_sec)) << 32 ) + tmr.tv_nsec;
            }
        };
    }    // namespace OS_X

}}    // namespace cds::OS
//@endcond

#endif // #ifndef CDSLIB_OS_LINUX_TIMER_H
