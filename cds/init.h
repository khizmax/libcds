//$$CDS-header$$

#ifndef __CDS_INIT_H
#define __CDS_INIT_H

#include <cds/details/defs.h>
#include <cds/os/topology.h>
#include <cds/threading/model.h>
#include <cds/details/lib.h>

namespace cds {

    //@cond
    namespace details {
        bool CDS_EXPORT_API init_first_call();
        bool CDS_EXPORT_API fini_last_call();
    }   // namespace details
    //@endcond

    /// Initialize CDS library
    /**
        The function initializes \p CDS library framework.
        Before usage of \p CDS library features your application must initialize it
        by calling \p Initialize function:
        @code
        #include <cds/init.h>
        #include <cds/gc/hp.h>

        int main()
        {
            // // Initialize CDS library
            cds::Initialize( 0 );

            {
                // // Initialize Hazard Pointer GC (if it is needed for you)
                cds::gc::HP();

                // // Now you can use CDS library containers with Hazard Pointer GC
                ...

            }
            // // Teminate CDS library
            cds::Terminate();

            return 0;
        }
        @endcode

        You may call \p Initialize several times, only first call is significant others will be ignored.
        To terminate the \p CDS library correctly, each call to \p Initialize must be balanced by a corresponding call to \ref Terminate.

        Note, that this function does not initialize garbage collectors. To use GC you need you should call
        GC-specific constructor function to initialize internal structures of GC. See cds::gc and its subnamespace for details.
    */
    static inline void Initialize(
        unsigned int nFeatureFlags = 0  ///< for future use, must be zero.
    )
    {
        CDS_UNUSED( nFeatureFlags );

        if ( cds::details::init_first_call() )
        {
            cds::OS::topology::init();
            cds::threading::ThreadData::s_nProcCount = cds::OS::topology::processor_count();
            if ( cds::threading::ThreadData::s_nProcCount == 0 )
                cds::threading::ThreadData::s_nProcCount = 1;

            cds::threading::Manager::init();
        }
    }

    /// Terminate CDS library
    /**
        This function terminates \p CDS library.
        After \p Terminate calling many features of the library are unavailable.
        This call should be the last call of \p CDS library in your application.
    */
    static inline void Terminate()
    {
        if ( cds::details::fini_last_call() ) {
            cds::threading::Manager::fini();

            cds::OS::topology::fini();
        }
    }

}   // namespace cds

#endif // __CDS_INIT_H
