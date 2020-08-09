// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_THREADING__COMMON_H
#define CDSLIB_THREADING__COMMON_H

#include <cds/urcu/details/gp_decl.h>
#include <cds/urcu/details/sh_decl.h>
#include <cds/algo/elimination_tls.h>

namespace cds {
    /// Threading support
    /** \anchor cds_threading
        The \p CDS library requires support from the threads.
        Each garbage collector manages a control structure on the per-thread basis.
        The library does not dictate any thread model. To embed the library to your application you should choose
        appropriate implementation of \p cds::threading::Manager interface
        or should provide yourself.
        The \p %cds::threading::Manager interface manages \p cds::threading::ThreadData structure that contains GC's thread specific data.

        Any \p cds::threading::Manager implementation is a singleton and it must be accessible from any thread and from any point of
        your application. Note that you should not mix different implementation of the \p cds::threading::Manager in your application.

        Before compiling of your application you may define one of \p CDS_THREADING_xxx macro in cds/user_setup/threading.h file:
            - \p CDS_THREADING_AUTODETECT - auto-detect appropriate threading model for your platform and compiler. This is
                predefined value of threading model in <tt>cds/user_setup/threading.h</tt>.
            - \p CDS_THREADING_WIN_TLS - use <tt>cds::threading::wintls::Manager</tt> implementation based on Windows TLS API.
                Intended for Windows and Microsoft Visual C++ only. This is default threading model for Windows and MS Visual C++.
            - \p CDS_THREADING_MSVC - use <tt>cds::threading::msvc::Manager</tt> implementation based on Microsoft Visual C++ <tt>__declspec(thread)</tt>
                declaration. Intended for Windows and Microsoft Visual C++ only.
                This macro should be explicitly specified if you want to use <tt>__declspec(thread)</tt> keyword.
            - \p CDS_THREADING_PTHREAD - use <tt>cds::threading::pthread::Manager</tt> implementation based on pthread thread-specific
                data functions \p pthread_getspecific / \p pthread_setspecific. Intended for GCC and clang compilers.
                This is default threading model for GCC and clang.
            - \p CDS_THREADING_GCC - use <tt>cds::threading::gcc::Manager</tt> implementation based on GCC \p __thread
                keyword. Intended for GCC compiler only. Note, that GCC compiler supports \p __thread keyword properly
                not for all platforms and even not for all GCC version.
                This macro should be explicitly specified if you want to use \p __thread keyword.
            - \p CDS_THREADING_CXX11 - use <tt>cds::threading::cxx11::Manager</tt> implementation based on \p thread_local
                keyword introduced in C++11 standard. May be used only if your compiler supports C++11 thread-local storage.
            - \p CDS_THREADING_USER_DEFINED - use user-provided threading model.

        These macros select appropriate implementation of \p cds::threading::Manager class. The child namespaces of cds::threading
        provide suitable implementation and import it to cds::threading by using \p using directive (or by using inline namespace if the compiler
        supports it). So, if you need to call threading manager functions directly you should refer to \p cds::threading::Manager class.

        @note Usually, you should not use \p cds::threading::Manager instance directly.
        You may specify \p CDS_THREADING_xxx macro when building, everything else will setup automatically when you initialize the library,
        see \ref cds_how_to_use "How to use libcds".

        The interface of \p cds::threading::Manager class is the following:
        \code
        class Manager {
        public:
            // Initialize manager (called by cds::Initialize())
            static void init();

            // Terminate manager (called by cds::Terminate())
            static void fini();

            // Checks whether current thread is attached to \p libcds feature or not.
            static bool isThreadAttached();

            // This method must be called in beginning of thread execution
            // (called by ctor of GC thread object, for example, by ctor of cds::gc::HP::thread_gc)
            static void attachThread();

            // This method must be called in end of thread execution
            // (called by dtor of GC thread object, for example, by dtor of cds::gc::HP::thread_gc)
            static void detachThread();

            // Get cds::gc::DHP thread GC implementation for current thread;
            static gc::DHP::thread_gc_impl&   getDHPGC();
        };
        \endcode

        The library's core (dynamic linked library) is free of usage of user-supplied \p cds::threading::Manager code.
        \p cds::threading::Manager is necessary for header-only part of \p CDS library.


        Each thread that uses \p libcds data structures should be attached to threading::Manager before using
        lock-free data structs.
        See \ref cds_how_to_use "How to use" section for details

        <b>Note for Windows</b>

        When you use Garbage Collectors (GC) provided by \p libcds in your dll that dynamically loaded by \p LoadLibrary then there is no way
        to use \p __declspec(thread) declaration to support threading model for \p libcds. MSDN says:

        \li If a DLL declares any nonlocal data or object as __declspec( thread ), it can cause a protection fault if dynamically loaded.
            After the DLL is loaded with \p LoadLibrary, it causes system failure whenever the code references the nonlocal __declspec( thread ) data.
            Because the global variable space for a thread is allocated at run time, the size of this space is based on a calculation of the requirements
            of the application plus the requirements of all the DLLs that are statically linked. When you use \p LoadLibrary, there is no way to extend
            this space to allow for the thread local variables declared with __declspec( thread ). Use the TLS APIs, such as TlsAlloc, in your
            DLL to allocate TLS if the DLL might be loaded with LoadLibrary.

        Thus, in case when \p libcds or a dll that depends on \p libcds is loaded dynamically by calling \p LoadLibrary explicitly,
        you should not use \p CDS_THREADING_MSVC macro. Instead, you should build your dll projects with \p CDS_THREADING_WIN_TLS only.
    */
    namespace threading {

        //@cond
        /// Thread-specific data
        struct ThreadData {

            //@cond
            cds::urcu::details::thread_data< cds::urcu::general_instant_tag > *     m_pGPIRCU;
            cds::urcu::details::thread_data< cds::urcu::general_buffered_tag > *    m_pGPBRCU;
            cds::urcu::details::thread_data< cds::urcu::general_threaded_tag > *    m_pGPTRCU;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
            cds::urcu::details::thread_data< cds::urcu::signal_buffered_tag > *    m_pSHBRCU;
#endif

            //@endcond

            size_t  m_nFakeProcessorNumber  ;   ///< fake "current processor" number

            //@cond
            size_t  m_nAttachCount;
            //@endcond

            //@cond
            static CDS_EXPORT_API atomics::atomic<size_t> s_nLastUsedProcNo;
            static CDS_EXPORT_API size_t                  s_nProcCount;
            //@endcond

            //@cond
            ThreadData()
                : m_pGPIRCU( nullptr )
                , m_pGPBRCU( nullptr )
                , m_pGPTRCU( nullptr )
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
                , m_pSHBRCU( nullptr )
#endif
                , m_nFakeProcessorNumber( s_nLastUsedProcNo.fetch_add(1, atomics::memory_order_relaxed) % s_nProcCount )
                , m_nAttachCount(0)
            {}

            ~ThreadData()
            {
                assert( m_pGPIRCU == nullptr );
                assert( m_pGPBRCU == nullptr );
                assert( m_pGPTRCU == nullptr );
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
                assert( m_pSHBRCU == nullptr );
#endif
            }

            CDS_EXPORT_API void init();
            CDS_EXPORT_API bool fini();

            size_t fake_current_processor()
            {
                return m_nFakeProcessorNumber;
            }
            //@endcond
        };
        //@endcond

    } // namespace threading
} // namespace cds

#endif // #ifndef CDSLIB_THREADING__COMMON_H
