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

#include <cds/os/topology.h>

#if CDS_OS_TYPE == CDS_OS_HPUX

#include <cds/algo/atomic.h>
#include <limits>

namespace cds { namespace OS { CDS_CXX11_INLINE_NAMESPACE namespace Hpux {

    size_t topology::s_nProcMapSize = 0;
    topology::processor_map * topology::s_procMap = nullptr;

    void topology::make_processor_map()
    {
        spu_t nProc;
        spu_t nMaxProcNo;

        // Processor numbers are not sequential in HP-UX.

        // Calculate max processor number
        nProc = nMaxProcNo = ::mpctl( MPC_GETFIRSTSPU, 0, 0 );

        while ( (nProc = ::mpctl( MPC_GETNEXTSPU, nProc, 0 )) != -1 ) {
            if ( nMaxProcNo < nProc )
                nMaxProcNo = nProc;
        }

        // Allocate processor map array
        s_nProcMapSize = nMaxProcNo + 1;

        // We cannot use operator new or std::allocator in this code
        // since the initialization phase may be called from
        // our overloaded operator new that based on cds::mihcael::Heap
        // As a result, a deadlock may be occured
        s_procMap = reinterpret_cast<processor_map *>(::malloc( sizeof(s_procMap[0]) * s_nProcMapSize ));
        processor_map * pEnd = s_procMap + s_nProcMapSize;

        for ( processor_map * p = s_procMap; p != pEnd; ++p ) {
            p->nCell = 0;
            p->nNativeProcNo = -1;
            p->nProcNo = std::numeric_limits<unsigned int>::max();
        }

        // Fill processor map array
        unsigned int nProcNo = 0;
        nProc = ::mpctl( MPC_GETFIRSTSPU, 0, 0 );
        s_procMap[ nProc ].nNativeProcNo = nProc;
        s_procMap[ nProc ].nProcNo = nProcNo++;
        s_procMap[ nProc ].nCell = ::mpctl( MPC_SPUTOLDOM, nProc, 0 );

        while ( (nProc = ::mpctl( MPC_GETNEXTSPU, nProc, 0 )) != -1 ) {
            processor_map * p = s_procMap + nProc;
            p->nNativeProcNo = nProc;
            p->nProcNo = nProcNo++;
            p->nCell = ::mpctl( MPC_SPUTOLDOM, nProc, 0 );
        }
    }

    void topology::init()
    {
        assert( s_procMap == nullptr );
        make_processor_map();
    }

    void topology::fini()
    {
        assert( s_procMap );
        if ( s_procMap ) {
            ::free( s_procMap );

            s_procMap = nullptr;
        }
    }

}}} // namespace cds::OS::Hpux

#endif  // #if CDS_OS_TYPE == CDS_OS_HPUX
