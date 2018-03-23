// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds/os/topology.h>

#if CDS_OS_TYPE == CDS_OS_LINUX

#include <thread>
/*
#include <unistd.h>
#include <fstream>
*/

namespace cds { namespace OS { inline namespace Linux {

    unsigned int topology::s_nProcessorCount = 0;

    void topology::init()
    {
        s_nProcessorCount = std::thread::hardware_concurrency();
/*
         long n = ::sysconf( _SC_NPROCESSORS_ONLN );
         if ( n > 0 )
            s_nProcessorCount = static_cast<unsigned>( n );
         else {
            try {
                std::ifstream cpuinfo("/proc/cpuinfo");
                std::string line;

                unsigned int nProcCount = 0;
                while ( !cpuinfo.eof()) {
                    std::getline(cpuinfo,line);
                    if (!line.size())
                        continue;
                    if ( line.find( "processor" ) !=0 )
                        continue;
                    ++nProcCount;
                }
                s_nProcessorCount = nProcCount;
            }
            catch ( std::exception& ex ) {
                s_nProcessorCount = 1;
            }
         }
*/
    }

    void topology::fini()
    {}
}}} // namespace cds::OS::Linux

#endif  // #if CDS_OS_TYPE == CDS_OS_LINUX
