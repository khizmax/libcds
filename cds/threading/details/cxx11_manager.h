/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

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

#ifndef CDSLIB_THREADING_DETAILS_CXX11_MANAGER_H
#define CDSLIB_THREADING_DETAILS_CXX11_MANAGER_H

#include <cds/threading/details/_common.h>

#ifndef CDS_CXX11_THREAD_LOCAL_SUPPORT
#   error "The compiler does not support C++11 thread_local keyword. You cannot use CDS_THREADING_CXX11 threading model."
#endif

//@cond
namespace cds { namespace threading {

    //@cond
    struct cxx11_internal {
        typedef unsigned char  ThreadDataPlaceholder[ sizeof(ThreadData) ];
        static thread_local ThreadDataPlaceholder CDS_DATA_ALIGNMENT(8) s_threadData;
        static thread_local ThreadData * s_pThreadData;
    };
    //@endcond

    /// cds::threading::Manager implementation based on c++11 thread_local declaration
    CDS_CXX11_INLINE_NAMESPACE namespace cxx11 {

        /// Thread-specific data manager based on c++11 thread_local feature
        class Manager {
        private :
            //@cond

            static ThreadData * _threadData()
            {
                return cxx11_internal::s_pThreadData;
            }

            static ThreadData * create_thread_data()
            {
                if ( !cxx11_internal::s_pThreadData ) {
                    cxx11_internal::s_pThreadData = new (cxx11_internal::s_threadData) ThreadData();
                }
                return cxx11_internal::s_pThreadData;
            }

            static void destroy_thread_data()
            {
                if ( cxx11_internal::s_pThreadData ) {
                    ThreadData * p = cxx11_internal::s_pThreadData;
                    cxx11_internal::s_pThreadData = nullptr;
                    p->ThreadData::~ThreadData();
                }
            }
            //@endcond

        public:
            /// Initialize manager (empty function)
            /**
                This function is automatically called by cds::Initialize
            */
            static void init()
            {}

            /// Terminate manager (empty function)
            /**
                This function is automatically called by cds::Terminate
            */
            static void fini()
            {}

            /// Checks whether current thread is attached to \p libcds feature or not.
            static bool isThreadAttached()
            {
                return _threadData() != nullptr;
            }

            /// This method must be called in beginning of thread execution
            static void attachThread()
            {
                create_thread_data()->init();
            }

            /// This method must be called in end of thread execution
            static void detachThread()
            {
                assert( _threadData());

                if ( _threadData()->fini())
                    destroy_thread_data();
            }

            /// Returns ThreadData pointer for the current thread
            static ThreadData * thread_data()
            {
                ThreadData * p = _threadData();
                assert( p );
                return p;
            }

            //@cond
            static size_t fake_current_processor()
            {
                return _threadData()->fake_current_processor();
            }
            //@endcond
        };

    } // namespace cxx11

}} // namespace cds::threading
//@endcond

#endif // #ifndef CDSLIB_THREADING_DETAILS_CXX11_MANAGER_H
