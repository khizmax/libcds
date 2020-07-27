// Copyright (c) 2020 Weile Wei
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_THREADING_DETAILS_HPX_MANAGER_H
#define CDSLIB_THREADING_DETAILS_HPX_MANAGER_H

#include <system_error>
#include <stdio.h>
#include <cds/threading/details/_common.h>
#include <cds/details/throw_exception.h>
#include <hpx/modules/threading.hpp>

#include <iostream>

//@cond
namespace cds { namespace threading {

    /// cds::threading::Manager implementation based on hpxthread thread-specific data functions
    inline namespace hpxthread {

        /// Thread-specific data manager based on hpxthread thread-specific data functions
        /**
            Manager throws an exception of Manager::hpxthread_exception class if an error occurs
        */
        class Manager {
        public:
            /// Initialize manager
            /**
                This function is automatically called by cds::Initialize
            */
            static void init()
            {}

            /// Terminate manager
            /**
                This function is automatically called by cds::Terminate
            */
            static void fini()
            {}

            /// Checks whether current thread is attached to \p libcds feature or not.
            static bool isThreadAttached()
            {
                std::size_t hpx_thread_data = hpx::threads::get_libcds_data(hpx::threads::get_self_id());
                ThreadData * pData = reinterpret_cast<ThreadData*> (hpx_thread_data);
                return pData != nullptr;
            }

            /// This method must be called in beginning of thread execution
            static void attachThread()
            {
                std::size_t hpx_thread_data = hpx::threads::get_libcds_data(hpx::threads::get_self_id());
                ThreadData * pData = reinterpret_cast<ThreadData*> (hpx_thread_data);
                if(pData == nullptr)
                {
                    pData = new ThreadData;
                    hpx_thread_data = reinterpret_cast<std::size_t>(pData);
                    hpx::threads::set_libcds_data(hpx::threads::get_self_id(), hpx_thread_data);
                }
                assert( pData );
                pData->init();
            }

            /// This method must be called in end of thread execution
            static void detachThread()
            {
                std::size_t hpx_thread_data = hpx::threads::get_libcds_data(hpx::threads::get_self_id());
                ThreadData * pData = reinterpret_cast<ThreadData*> (hpx_thread_data);
                assert( pData );

                if ( pData->fini())
                {
                    hpx_thread_data = reinterpret_cast<std::size_t>(nullptr);
                    hpx::threads::set_libcds_data(hpx::threads::get_self_id(), hpx_thread_data);
                    delete pData;
                }

            }

            /// Returns ThreadData pointer for the current thread
            static ThreadData * thread_data()
            {
                std::size_t hpx_thread_data = hpx::threads::get_libcds_data(hpx::threads::get_self_id());
                return reinterpret_cast<ThreadData*> (hpx_thread_data);
            }

            //@cond
            static size_t fake_current_processor()
            {
                std::size_t hpx_thread_data = hpx::threads::get_libcds_data(hpx::threads::get_self_id());
                ThreadData * pData = reinterpret_cast<ThreadData*> (hpx_thread_data);
                return pData->fake_current_processor();
            }
            //@endcond
        };

    } // namespace pthread
}} // namespace cds::threading
//@endcond

#endif // #ifndef CDSLIB_THREADING_DETAILS_HPX_MANAGER_H
