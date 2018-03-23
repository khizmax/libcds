// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds/os/topology.h>

#if CDS_OS_TYPE == CDS_OS_HPUX

#include <cds/algo/atomic.h>
#include <limits>

namespace cds { namespace OS { inline namespace Hpux {

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
