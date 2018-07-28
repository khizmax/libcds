// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INIT_H
#define CDSLIB_INIT_H

#include <cds/details/defs.h>
#include <cds/os/topology.h>
#include <cds/threading/model.h>

namespace cds {

    //@cond
    namespace details {
        bool CDS_EXPORT_API init_first_call();
        bool CDS_EXPORT_API fini_last_call();

        void CDS_EXPORT_API check_hpstat_enabled( bool enabled );
    }   // namespace details
    //@endcond

    /// Initialize CDS library
    /**
        The function initializes \p CDS library framework.
        Before usage of \p CDS library features your application must initialize it
        by calling \p %Initialize() function, see \ref cds_how_to_use "how to use the library".

        You can call \p Initialize several times, only first call is significant others will be ignored.
        To terminate the \p CDS library correctly, each call to \p %Initialize() must be balanced
        by a corresponding \p Terminate() call.

        Note, that this function does not initialize garbage collectors. To use GC you need you should call
        GC-specific constructor function to initialize internal structures of GC.
        See \p cds::gc for details.
    */
    static inline void Initialize(
        unsigned int nFeatureFlags = 0  ///< for future use, must be zero.
    )
    {
        CDS_UNUSED( nFeatureFlags );

        details::check_hpstat_enabled(
#ifdef CDS_ENABLE_HPSTAT
            true
#else
            false
#endif
        );

        if ( cds::details::init_first_call())
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
        After \p %Terminate() calling many features of the library are unavailable.
        This call should be the last call of \p CDS library in your application,
        see \ref cds_how_to_use "how to use the library".
    */
    static inline void Terminate()
    {
        if ( cds::details::fini_last_call()) {
            cds::threading::Manager::fini();

            cds::OS::topology::fini();
        }
    }

}   // namespace cds

#endif // CDSLIB_INIT_H
