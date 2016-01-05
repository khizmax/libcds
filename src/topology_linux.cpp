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

#if CDS_OS_TYPE == CDS_OS_LINUX

#include <unistd.h>
#include <fstream>

namespace cds { namespace OS { CDS_CXX11_INLINE_NAMESPACE namespace Linux {

    unsigned int topology::s_nProcessorCount = 0;

    void topology::init()
    {
         long n = ::sysconf( _SC_NPROCESSORS_ONLN );
         if ( n > 0 )
            s_nProcessorCount = n;
         else {
            try {
                std::ifstream cpuinfo("/proc/cpuinfo");
                std::string line;

                unsigned int nProcCount = 0;
                while ( !cpuinfo.eof() ) {
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
    }

    void topology::fini()
    {}
}}} // namespace cds::OS::Linux

#endif  // #if CDS_OS_TYPE == CDS_OS_LINUX
