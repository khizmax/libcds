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

#ifndef CDSLIB_ALGO_FLAT_COMBINING_WAIT_STRATEGY_H
#define CDSLIB_ALGO_FLAT_COMBINING_WAIT_STRATEGY_H

#include <cds/algo/flat_combining/defs.h>
#include <cds/algo/backoff_strategy.h>
#include <mutex>
#include <condition_variable>
#include <boost/thread/tss.hpp>  // thread_specific_ptr


namespace cds { namespace opt {

    /// Wait strategy option for \p flat_combining::kernel
    template <typename Strategy>
    struct wait_strategy {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef Strategy wait_strategy;
        };
        //@endcond
    };

}} // namespace cds::opt

namespace cds { namespace algo { namespace flat_combining {

    /// Wait strategies for \p flat_combining technique
    namespace wait_strategy {

        /// Empty wait strategy
        struct empty
        {
        //@cond
            template <typename PublicationRecord>
            struct make_publication_record {
                typedef PublicationRecord type;
            };

            template <typename PublicationRecord>
            void prepare( PublicationRecord& /*rec*/ )
            {}

            template <typename FCKernel, typename PublicationRecord>
            bool wait( FCKernel& /*fc*/, PublicationRecord& /*rec*/ )
            {
                return false;
            }

            template <typename FCKernel, typename PublicationRecord>
            void notify( FCKernel& /*fc*/, PublicationRecord& /*rec*/ )
            {}

            template <typename FCKernel>
            void wakeup( FCKernel& /*fc*/ )
            {}
        //@endcond
        };

        /// Back-off wait strategy
        template <typename BackOff = cds::backoff::delay_of<2>>
        struct backoff
        {
        //@cond
            typedef BackOff back_off;

            template <typename PublicationRecord>
            struct make_publication_record {
                struct type: public PublicationRecord
                {
                    back_off bkoff;
                };
            };

            template <typename PublicationRecord>
            void prepare( PublicationRecord& rec )
            {
                rec.bkoff.reset();
            }

            template <typename FCKernel, typename PublicationRecord>
            bool wait( FCKernel& /*fc*/, PublicationRecord& rec )
            {
                rec.bkoff();
                return false;
            }

            template <typename FCKernel, typename PublicationRecord>
            void notify( FCKernel& /*fc*/, PublicationRecord& /*rec*/ )
            {}

            template <typename FCKernel>
            void wakeup( FCKernel& )
            {}
        //@endcond
        };

        /// Wait strategy based on the single mutex and the condition variable
        /**
            The strategy shares the mutex and conditional variable for all thread.

            Template parameter \p Milliseconds specifies waiting duration;
            the minimal value is 1.
        */
        template <int Milliseconds = 2>
        class single_mutex_single_condvar
        {
        //@cond
            std::mutex m_mutex;
            std::condition_variable m_condvar;

            typedef std::unique_lock< std::mutex > unique_lock;

        public:
            enum {
                c_nWaitMilliseconds = Milliseconds < 1 ? 1 : Milliseconds
            };

            template <typename PublicationRecord>
            struct make_publication_record {
                typedef PublicationRecord type;
            };

            template <typename PublicationRecord>
            void prepare( PublicationRecord& /*rec*/ )
            {}

            template <typename FCKernel, typename PublicationRecord>
            bool wait( FCKernel& fc, PublicationRecord& rec )
            {
                if ( fc.get_operation( rec ) >= req_Operation ) {
                    unique_lock lock( m_mutex );
                    if ( fc.get_operation( rec ) >= req_Operation )
                        return m_condvar.wait_for( lock, std::chrono::milliseconds( c_nWaitMilliseconds )) == std::cv_status::no_timeout;
                }
                return false;
            }

            template <typename FCKernel, typename PublicationRecord>
            void notify( FCKernel& fc, PublicationRecord& rec )
            {
                m_condvar.notify_all();
            }

            template <typename FCKernel>
            void wakeup( FCKernel& /*fc*/ )
            {
                m_condvar.notify_all();
            }
        //@endcond
        };

        /// Wait strategy based on the single mutex and thread-local condition variables
        /**
            The strategy shares the mutex, but each thread has its own conditional variable

            Template parameter \p Milliseconds specifies waiting duration;
            the minimal value is 1.
        */
        template <int Milliseconds = 2>
        class single_mutex_multi_condvar
        {
        //@cond
            std::mutex m_mutex;

            typedef std::unique_lock< std::mutex > unique_lock;

        public:
            enum {
                c_nWaitMilliseconds = Milliseconds < 1 ? 1 : Milliseconds
            };

            template <typename PublicationRecord>
            struct make_publication_record {
                struct type: public PublicationRecord
                {
                    std::condition_variable m_condvar;
                };
            };

            template <typename PublicationRecord>
            void prepare( PublicationRecord& /*rec*/ )
            {}

            template <typename FCKernel, typename PublicationRecord>
            bool wait( FCKernel& fc, PublicationRecord& rec )
            {
                if ( fc.get_operation( rec ) >= req_Operation ) {
                    unique_lock lock( m_mutex );
                    if ( fc.get_operation( rec ) >= req_Operation )
                        return rec.m_condvar.wait_for( lock, std::chrono::milliseconds( c_nWaitMilliseconds )) == std::cv_status::no_timeout;
                }
                return false;
            }

            template <typename FCKernel, typename PublicationRecord>
            void notify( FCKernel& fc, PublicationRecord& rec )
            {
                rec.m_condvar.notify_one();
            }

            template <typename FCKernel>
            void wakeup( FCKernel& fc )
            {
                fc.wakeup_any();
            }
        //@endcond
        };

        /// Wait strategy where each thread has a mutex and a condition variable
        /**
            Template parameter \p Milliseconds specifies waiting duration;
            the minimal value is 1.
        */
        template <int Milliseconds = 10>
        class multi_mutex_multi_condvar
        {
        //@cond
            typedef std::unique_lock< std::mutex > unique_lock;

        public:
            enum {
                c_nWaitMilliseconds = Milliseconds < 1 ? 1 : Milliseconds
            };

            template <typename PublicationRecord>
            struct make_publication_record {
                struct type: public PublicationRecord
                {
                    std::mutex              m_mutex;
                    std::condition_variable m_condvar;
                };
            };

            template <typename PublicationRecord>
            void prepare( PublicationRecord& /*rec*/ )
            {}

            template <typename FCKernel, typename PublicationRecord>
            bool wait( FCKernel& fc, PublicationRecord& rec )
            {
                if ( fc.get_operation( rec ) >= req_Operation ) {
                    unique_lock lock( rec.m_mutex );
                    if ( fc.get_operation( rec ) >= req_Operation )
                        return rec.m_condvar.wait_for( lock, std::chrono::milliseconds( c_nWaitMilliseconds )) == std::cv_status::no_timeout;
                }
                return false;
            }

            template <typename FCKernel, typename PublicationRecord>
            void notify( FCKernel& /*fc*/, PublicationRecord& rec )
            {
                rec.m_condvar.notify_one();
            }

            template <typename FCKernel>
            void wakeup( FCKernel& fc )
            {
                fc.wakeup_any();
            }
        //@endcond
        };

    } // namespace wait_strategy
}}} // namespace cds::algo::flat_combining

#endif //CDSLIB_ALGO_FLAT_COMBINING_WAIT_STRATEGY_H
