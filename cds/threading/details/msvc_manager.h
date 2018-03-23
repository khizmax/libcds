// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_THREADING_DETAILS_MSVC_MANAGER_H
#define CDSLIB_THREADING_DETAILS_MSVC_MANAGER_H

#if !( CDS_COMPILER == CDS_COMPILER_MSVC || (CDS_COMPILER == CDS_COMPILER_INTEL && CDS_OS_INTERFACE == CDS_OSI_WINDOWS))
#   error "threading/details/msvc_manager.h may be used only with Microsoft Visual C++ / Intel C++ compiler"
#endif

#include <cds/threading/details/_common.h>

//@cond
namespace cds { namespace threading {

    //@cond
    struct msvc_internal {
        typedef unsigned char  ThreadDataPlaceholder[ sizeof(ThreadData) ];
        __declspec( thread ) static ThreadDataPlaceholder s_threadData;
        __declspec( thread ) static ThreadData * s_pThreadData;
    };
    //@endcond

    /// cds::threading::Manager implementation based on Microsoft Visual C++ __declspec( thread ) construction
    inline namespace msvc {

        /// Thread-specific data manager based on MSVC __declspec( thread ) feature
        class Manager {
        private :
            //@cond
            static ThreadData * _threadData()
            {
                return msvc_internal::s_pThreadData;
            }

            static ThreadData * create_thread_data()
            {
                if ( !msvc_internal::s_pThreadData ) {
                    msvc_internal::s_pThreadData = new (msvc_internal::s_threadData) ThreadData();
                }
                return msvc_internal::s_pThreadData;
            }

            static void destroy_thread_data()
            {
                if ( msvc_internal::s_pThreadData ) {
                    msvc_internal::s_pThreadData->ThreadData::~ThreadData();
                    msvc_internal::s_pThreadData = nullptr;
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
                ThreadData * pData = _threadData();
                return pData != nullptr;
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

            /// Returns internal ThreadData pointer for the current thread
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

    } // namespace msvc
}} // namespace cds::threading
//@endcond

#endif // #ifndef CDSLIB_THREADING_DETAILS_MSVC_MANAGER_H
