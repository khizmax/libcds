// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds/os/topology.h>

#if CDS_OS_TYPE == CDS_OS_OSX

#include <sys/types.h>
#include <sys/sysctl.h>

namespace cds { namespace OS { inline namespace OS_X {

    unsigned int topology::s_nProcessorCount = 0;

    void topology::init()
    {
        size_t len = sizeof( s_nProcessorCount );
        if ( sysctlbyname("hw.logicalcpu", &s_nProcessorCount, &len, nullptr, 0 ) != 0 )
            s_nProcessorCount = 1;
    }

    void topology::fini()
    {}
}}} // namespace cds::OS::Linux

#endif  // #if CDS_OS_TYPE == CDS_OS_OSX
