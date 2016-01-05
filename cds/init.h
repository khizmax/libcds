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

#ifndef CDSLIB_INIT_H
#define CDSLIB_INIT_H

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
        After \p %Terminate() calling many features of the library are unavailable.
        This call should be the last call of \p CDS library in your application,
        see \ref cds_how_to_use "how to use the library".
    */
    static inline void Terminate()
    {
        if ( cds::details::fini_last_call() ) {
            cds::threading::Manager::fini();

            cds::OS::topology::fini();
        }
    }

}   // namespace cds

#endif // CDSLIB_INIT_H
