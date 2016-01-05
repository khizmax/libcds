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

#ifndef CDSLIB_THREADING_DETAILS_GCC_MANAGER_H
#define CDSLIB_THREADING_DETAILS_GCC_MANAGER_H

#if !( CDS_COMPILER == CDS_COMPILER_GCC || CDS_COMPILER == CDS_COMPILER_CLANG || CDS_COMPILER == CDS_COMPILER_INTEL)
#   error "threading/details/gcc_manager.h may be used only with GCC or Clang C++ compiler"
#endif

#include <cds/threading/details/_common.h>

//@cond
namespace cds { namespace threading {

    //@cond
    struct gcc_internal {
        typedef unsigned char  ThreadDataPlaceholder[ sizeof(ThreadData) ];
        static __thread ThreadDataPlaceholder CDS_DATA_ALIGNMENT(8) s_threadData;
        static __thread ThreadData * s_pThreadData;
    };
    //@endcond

    /// cds::threading::Manager implementation based on GCC __thread declaration
    CDS_CXX11_INLINE_NAMESPACE namespace gcc {

        /// Thread-specific data manager based on GCC __thread feature
        class Manager {
        private :
            //@cond

            static ThreadData * _threadData()
            {
                return gcc_internal::s_pThreadData;
            }

            static ThreadData * create_thread_data()
            {
                if ( !gcc_internal::s_pThreadData ) {
                    gcc_internal::s_pThreadData = new (gcc_internal::s_threadData) ThreadData();
                }
                return gcc_internal::s_pThreadData;
            }

            static void destroy_thread_data()
            {
                if ( gcc_internal::s_pThreadData ) {
                    ThreadData * p = gcc_internal::s_pThreadData;
                    gcc_internal::s_pThreadData = nullptr;
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
                assert( _threadData() );

                if ( _threadData()->fini() )
                    destroy_thread_data();
            }

            /// Returns ThreadData pointer for the current thread
            static ThreadData * thread_data()
            {
                ThreadData * p = _threadData();
                assert( p );
                return p;
            }

            /// Get gc::HP thread GC implementation for current thread
            /**
                The object returned may be uninitialized if you did not call attachThread in the beginning of thread execution
                or if you did not use gc::HP.
                To initialize gc::HP GC you must constuct cds::gc::HP object in the beginning of your application
            */
            static gc::HP::thread_gc_impl&   getHZPGC()
            {
                assert( _threadData()->m_hpManager );
                return *(_threadData()->m_hpManager);
            }

            /// Get gc::DHP thread GC implementation for current thread
            /**
                The object returned may be uninitialized if you did not call attachThread in the beginning of thread execution
                or if you did not use gc::DHP.
                To initialize gc::DHP GC you must constuct cds::gc::DHP object in the beginning of your application
            */
            static gc::DHP::thread_gc_impl&   getDHPGC()
            {
                assert( _threadData()->m_dhpManager );
                return *(_threadData()->m_dhpManager);
            }

            //@cond
            static size_t fake_current_processor()
            {
                return _threadData()->fake_current_processor();
            }
            //@endcond
        };

    } // namespace gcc

}} // namespace cds::threading
//@endcond

#endif // #ifndef CDSLIB_THREADING_DETAILS_GCC_MANAGER_H
