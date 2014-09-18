//$$CDS-header$$

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
