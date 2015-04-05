//$$CDS-header$$

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
