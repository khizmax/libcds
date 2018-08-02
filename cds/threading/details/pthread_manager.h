// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_THREADING_DETAILS_PTHREAD_MANAGER_H
#define CDSLIB_THREADING_DETAILS_PTHREAD_MANAGER_H

#include <system_error>
#include <stdio.h>
#include <pthread.h>
#include <cds/threading/details/_common.h>
#include <cds/details/throw_exception.h>

//@cond
namespace cds { namespace threading {

    /// cds::threading::Manager implementation based on pthread thread-specific data functions
    inline namespace pthread {

        /// Thread-specific data manager based on pthread thread-specific data functions
        /**
            Manager throws an exception of Manager::pthread_exception class if an error occurs
        */
        class Manager {
        private :
            /// pthread error code type
            typedef int pthread_error_code;

            /// pthread exception
            class pthread_exception: public std::system_error
            {
            public:
                /// Exception constructor
                pthread_exception( int nCode, const char * pszFunction )
                    : std::system_error( nCode, std::system_category(), pszFunction )
                {}
            };

            /// pthread TLS key holder
            struct Holder {
            //@cond
                static pthread_key_t   m_key;

                static void key_destructor(void * p)
                {
                    if ( p ) {
                        reinterpret_cast<ThreadData *>(p)->fini();
                        delete reinterpret_cast<ThreadData *>(p);
                    }
                }

                static void init()
                {
                    pthread_error_code  nErr;
                    if ( ( nErr = pthread_key_create( &m_key, key_destructor )) != 0 )
                        CDS_THROW_EXCEPTION( pthread_exception( nErr, "pthread_key_create" ));
                }

                static void fini()
                {
                    pthread_error_code  nErr;
                    if ( ( nErr = pthread_key_delete( m_key )) != 0 )
                        CDS_THROW_EXCEPTION( pthread_exception( nErr, "pthread_key_delete" ));
                }

                static ThreadData *    get()
                {
                    return reinterpret_cast<ThreadData *>( pthread_getspecific( m_key ));
                }

                static void alloc()
                {
                    pthread_error_code  nErr;
                    ThreadData * pData = new ThreadData;
                    if ( ( nErr = pthread_setspecific( m_key, pData )) != 0 )
                        CDS_THROW_EXCEPTION( pthread_exception( nErr, "pthread_setspecific" ));
                }
                static void free()
                {
                    ThreadData * p = get();
                    pthread_setspecific( m_key, nullptr );
                    delete p;
                }
            //@endcond
            };

            //@cond
            enum EThreadAction {
                do_getData,
                do_attachThread,
                do_detachThread,
                do_checkData,
                init_holder,
                fini_holder
            };
            //@endcond

            //@cond
            static ThreadData * _threadData( EThreadAction nAction )
            {
                switch ( nAction ) {
                    case do_getData:
                        return Holder::get();
                    case do_checkData:
                        return Holder::get();
                    case do_attachThread:
                        if ( Holder::get() == nullptr )
                            Holder::alloc();
                        return Holder::get();
                    case do_detachThread:
                        Holder::free();
                        return nullptr;
                    case init_holder:
                    case fini_holder:
                        break;
                    default:
                        assert( false ) ;   // anything forgotten?..
                }
                assert(false)   ;   // how did we get here?
                return nullptr;
            }
            //@endcond

        public:
            /// Initialize manager
            /**
                This function is automatically called by cds::Initialize
            */
            static void init()
            {
                Holder::init();
            }

            /// Terminate manager
            /**
                This function is automatically called by cds::Terminate
            */
            static void fini()
            {
                Holder::fini();
            }

            /// Checks whether current thread is attached to \p libcds feature or not.
            static bool isThreadAttached()
            {
                return _threadData( do_checkData ) != nullptr;
            }

            /// This method must be called in beginning of thread execution
            /**
                If TLS pointer to manager's data is \p nullptr, pthread_exception is thrown
                with code = -1.
                If an error occurs in call of pthread API function, pthread_exception is thrown
                with pthread error code.
            */
            static void attachThread()
            {
                ThreadData * pData = _threadData( do_attachThread );
                assert( pData );

                if ( pData ) {
                    pData->init();
                }
                else
                    CDS_THROW_EXCEPTION( pthread_exception( -1, "cds::threading::pthread::Manager::attachThread" ));
            }

            /// This method must be called in end of thread execution
            /**
                If TLS pointer to manager's data is \p nullptr, pthread_exception is thrown
                with code = -1.
                If an error occurs in call of pthread API function, pthread_exception is thrown
                with pthread error code.
            */
            static void detachThread()
            {
                ThreadData * pData = _threadData( do_getData );
                assert( pData );

                if ( pData ) {
                    if ( pData->fini())
                        _threadData( do_detachThread );
                }
                else
                    CDS_THROW_EXCEPTION( pthread_exception( -1, "cds::threading::pthread::Manager::detachThread" ));
            }

            /// Returns ThreadData pointer for the current thread
            static ThreadData * thread_data()
            {
                return _threadData( do_getData );
            }

            //@cond
            static size_t fake_current_processor()
            {
                return _threadData( do_getData )->fake_current_processor();
            }
            //@endcond

        };

    } // namespace pthread
}} // namespace cds::threading
//@endcond

#endif // #ifndef CDSLIB_THREADING_DETAILS_PTHREAD_MANAGER_H
