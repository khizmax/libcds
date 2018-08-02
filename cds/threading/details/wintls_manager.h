// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_THREADING_DETAILS_WINTLS_MANAGER_H
#define CDSLIB_THREADING_DETAILS_WINTLS_MANAGER_H

#include <system_error>
#include <stdio.h>
#include <cds/threading/details/_common.h>
#include <cds/details/throw_exception.h>

//@cond
namespace cds { namespace threading {

    /// cds::threading::Manager implementation based on Windows TLS API
    inline namespace wintls {

        /// Thread-specific data manager based on Windows TLS API
        /**
            Manager throws an exception of Manager::api_exception class if an error occurs
        */
        class Manager {
        private :
            /// Windows TLS API error code type
            typedef DWORD api_error_code;

            /// TLS API exception
            class api_exception : public std::system_error
            {
            public:
                /// Exception constructor
                api_exception( api_error_code nCode, const char * pszFunction )
                    : std::system_error( static_cast<int>(nCode), std::system_category(), pszFunction )
                {}
            };

            //@cond
            enum EThreadAction {
                do_getData,
                do_attachThread,
                do_detachThread,
                do_checkData
            };
            //@endcond

            //@cond
            /// TLS key holder
            struct Holder {
                static CDS_EXPORT_API DWORD m_key;

                static void init()
                {
                    if ( m_key == TLS_OUT_OF_INDEXES ) {
                        if ( ( m_key = ::TlsAlloc()) == TLS_OUT_OF_INDEXES )
                            CDS_THROW_EXCEPTION( api_exception( ::GetLastError(), "TlsAlloc" ));
                    }
                }

                static void fini()
                {
                    if ( m_key != TLS_OUT_OF_INDEXES ) {
                        if ( ::TlsFree( m_key ) == 0 )
                            CDS_THROW_EXCEPTION( api_exception( ::GetLastError(), "TlsFree" ));
                        m_key = TLS_OUT_OF_INDEXES;
                    }
                }

                static ThreadData *    get()
                {
                    api_error_code  nErr;
                    void * pData = ::TlsGetValue( m_key );
                    if ( pData == nullptr && ( nErr = ::GetLastError()) != ERROR_SUCCESS )
                        CDS_THROW_EXCEPTION( api_exception( nErr, "TlsGetValue" ));
                    return reinterpret_cast<ThreadData *>( pData );
                }

                static void alloc()
                {
                    ThreadData * pData = new ThreadData;
                    if ( !::TlsSetValue( m_key, pData ))
                        CDS_THROW_EXCEPTION( api_exception( ::GetLastError(), "TlsSetValue" ));
                }
                static void free()
                {
                    ThreadData * p = get();
                    ::TlsSetValue( m_key, nullptr );
                    delete p;
                }
            };
            //@endcond

            //@cond
            static ThreadData * _threadData( EThreadAction nAction )
            {
                switch ( nAction ) {
                    case do_getData:
#           ifdef _DEBUG
                        {
                            ThreadData * p = Holder::get();
                            assert( p );
                            return p;
                        }
#           else
                        return Holder::get();
#           endif
                    case do_checkData:
                        return Holder::get();
                    case do_attachThread:
                        if ( Holder::get() == nullptr )
                            Holder::alloc();
                        return Holder::get();
                    case do_detachThread:
                        Holder::free();
                        return nullptr;
                    default:
                        assert( false ) ;   // anything forgotten?..
                }
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
                If TLS pointer to manager's data is \p nullptr, api_exception is thrown
                with code = -1.
                If an error occurs in call of Win TLS API function, api_exception is thrown
                with Windows error code.
            */
            static void attachThread()
            {
                ThreadData * pData = _threadData( do_attachThread );
                assert( pData );

                if ( pData )
                    pData->init();
                else
                    CDS_THROW_EXCEPTION( api_exception( api_error_code(-1), "cds::threading::wintls::Manager::attachThread" ));
            }

            /// This method must be called in end of thread execution
            /**
                If TLS pointer to manager's data is \p nullptr, api_exception is thrown
                with code = -1.
                If an error occurs in call of Win TLS API function, api_exception is thrown
                with Windows error code.
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
                    CDS_THROW_EXCEPTION( api_exception( api_error_code(-1), "cds::threading::winapi::Manager::detachThread" ));
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

    } // namespace wintls
}} // namespace cds::threading
//@endcond

#endif // #ifndef CDSLIB_THREADING_DETAILS_WINTLS_MANAGER_H
