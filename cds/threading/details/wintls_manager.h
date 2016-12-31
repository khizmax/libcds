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

#ifndef CDSLIB_THREADING_DETAILS_WINTLS_MANAGER_H
#define CDSLIB_THREADING_DETAILS_WINTLS_MANAGER_H

#include <system_error>
#include <stdio.h>
#include <cds/threading/details/_common.h>

//@cond
namespace cds { namespace threading {

    /// cds::threading::Manager implementation based on Windows TLS API
    CDS_CXX11_INLINE_NAMESPACE namespace wintls {

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
                        if ( (m_key = ::TlsAlloc()) == TLS_OUT_OF_INDEXES )
                            throw api_exception( ::GetLastError(), "TlsAlloc" );
                    }
                }

                static void fini()
                {
                    if ( m_key != TLS_OUT_OF_INDEXES ) {
                        if ( ::TlsFree( m_key ) == 0 )
                            throw api_exception( ::GetLastError(), "TlsFree" );
                        m_key = TLS_OUT_OF_INDEXES;
                    }
                }

                static ThreadData *    get()
                {
                    api_error_code  nErr;
                    void * pData = ::TlsGetValue( m_key );
                    if ( pData == nullptr && (nErr = ::GetLastError()) != ERROR_SUCCESS )
                        throw api_exception( nErr, "TlsGetValue" );
                    return reinterpret_cast<ThreadData *>( pData );
                }

                static void alloc()
                {
                    ThreadData * pData = new ThreadData;
                    if ( !::TlsSetValue( m_key, pData ))
                        throw api_exception( ::GetLastError(), "TlsSetValue" );
                }
                static void free()
                {
                    ThreadData * p = get();
                    ::TlsSetValue( m_key, nullptr );
                    if ( p )
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

                if ( pData ) {
                    pData->init();
                }
                else
                    throw api_exception( api_error_code(-1), "cds::threading::wintls::Manager::attachThread" );
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
                    throw api_exception( api_error_code(-1), "cds::threading::winapi::Manager::detachThread" );
            }

            /// Returns ThreadData pointer for the current thread
            static ThreadData * thread_data()
            {
                return _threadData( do_getData );
            }

            /// Get gc::HP thread GC implementation for current thread
            /**
                The object returned may be uninitialized if you did not call attachThread in the beginning of thread execution
                or if you did not use gc::HP.
                To initialize gc::HP GC you must constuct cds::gc::HP object in the beginning of your application
            */
            static gc::HP::thread_gc_impl&   getHZPGC()
            {
                return *(_threadData( do_getData )->m_hpManager);
            }

            /// Get gc::DHP thread GC implementation for current thread
            /**
                The object returned may be uninitialized if you did not call attachThread in the beginning of thread execution
                or if you did not use gc::DHP.
                To initialize gc::DHP GC you must constuct cds::gc::DHP object in the beginning of your application
            */
            static gc::DHP::thread_gc_impl&   getDHPGC()
            {
                return *(_threadData( do_getData )->m_dhpManager);
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
