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

#include <cds/init.h>
#include <cds/algo/atomic.h>

#if CDS_OS_INTERFACE == CDS_OSI_WINDOWS && CDS_OS_TYPE != CDS_OS_MINGW
#   if CDS_COMPILER == CDS_COMPILER_MSVC || CDS_COMPILER == CDS_COMPILER_INTEL
#       include <cds/threading/details/msvc_manager.h>
#   endif
#   include <cds/threading/details/wintls_manager.h>
#else   // CDS_OS_INTERFACE != CDS_OSI_WINDOWS
#   if CDS_COMPILER == CDS_COMPILER_GCC || CDS_COMPILER == CDS_COMPILER_CLANG || CDS_COMPILER == CDS_COMPILER_INTEL
#       include <cds/threading/details/gcc_manager.h>
#   endif
#   include <cds/threading/details/pthread_manager.h>
#endif

#ifdef CDS_CXX11_THREAD_LOCAL_SUPPORT
#   include <cds/threading/details/cxx11_manager.h>
#endif

namespace cds {

    CDS_EXPORT_API atomics::atomic<size_t> threading::ThreadData::s_nLastUsedProcNo(0);
    CDS_EXPORT_API size_t threading::ThreadData::s_nProcCount = 1;

#if CDS_OS_INTERFACE == CDS_OSI_WINDOWS
    CDS_EXPORT_API DWORD cds::threading::wintls::Manager::Holder::m_key = TLS_OUT_OF_INDEXES;
#   if CDS_COMPILER == CDS_COMPILER_MSVC || CDS_COMPILER == CDS_COMPILER_INTEL
        __declspec( thread ) threading::msvc_internal::ThreadDataPlaceholder threading::msvc_internal::s_threadData;
        __declspec(thread) threading::ThreadData * threading::msvc_internal::s_pThreadData = nullptr;
#   endif
#else
    pthread_key_t threading::pthread::Manager::Holder::m_key;

#   if CDS_COMPILER == CDS_COMPILER_GCC || CDS_COMPILER == CDS_COMPILER_CLANG
        __thread threading::gcc_internal::ThreadDataPlaceholder CDS_DATA_ALIGNMENT(8) threading::gcc_internal::s_threadData;
        __thread threading::ThreadData * threading::gcc_internal::s_pThreadData = nullptr;
#   endif
#endif

#ifdef CDS_CXX11_THREAD_LOCAL_SUPPORT
    thread_local threading::cxx11_internal::ThreadDataPlaceholder CDS_DATA_ALIGNMENT(8) threading::cxx11_internal::s_threadData;
    thread_local threading::ThreadData * threading::cxx11_internal::s_pThreadData = nullptr;
#endif

    namespace details {
        static atomics::atomic<size_t> s_nInitCallCount(0);

        bool CDS_EXPORT_API init_first_call()
        {
            return s_nInitCallCount.fetch_add(1, atomics::memory_order_relaxed) == 0;
        }

        bool CDS_EXPORT_API fini_last_call()
        {
            if ( s_nInitCallCount.fetch_sub( 1, atomics::memory_order_relaxed ) == 1 ) {
                atomics::atomic_thread_fence( atomics::memory_order_release );
                return true;
            }
            return false;
        }
    } // namespace details

}   // namespace cds
