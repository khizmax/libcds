/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

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

#include <cds/details/defs.h>

#if CDS_OS_TYPE == CDS_OS_WIN32 || CDS_OS_TYPE == CDS_OS_WIN64 || CDS_OS_TYPE == CDS_OS_MINGW

#include <cds/os/thread.h>

// Visual leak detector (see http://vld.codeplex.com/)
#if defined(CDS_USE_VLD) && CDS_COMPILER == CDS_COMPILER_MSVC
#   ifdef _DEBUG
#       include <vld.h>
#   endif
#endif

static cds::OS::ThreadId    s_MainThreadId = 0;
static HINSTANCE            s_DllInstance = nullptr;

#if _WIN32_WINNT < 0x0601
// For Windows below Windows 7

#include <cds/os/topology.h>
#include <cds/algo/bitop.h>

static unsigned int     s_nProcessorCount = 1;
static unsigned int     s_nProcessorGroupCount = 1;

// Array of processor - cell relationship
// Array size is s_nProcessorCount
// s_arrProcessorCellRelationship[i] is the cell (the processor group) number for i-th processor
// static unsigned int *   s_arrProcessorCellRelationship = nullptr;

static void discover_topology()
{
    // From MSDN: http://msdn.microsoft.com/en-us/library/ms683194%28v=VS.85%29.aspx

    typedef BOOL (WINAPI *LPFN_GLPI)( PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD);

    LPFN_GLPI glpi;
    bool bDone = false;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = nullptr;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = nullptr;
    DWORD returnLength = 0;
    DWORD logicalProcessorCount = 0;
    DWORD numaNodeCount = 0;
    DWORD processorCoreCount = 0;
    DWORD processorPackageCount = 0;
    DWORD byteOffset = 0;

    s_nProcessorCount = 1;
    s_nProcessorGroupCount = 1;

    glpi = (LPFN_GLPI) GetProcAddress( GetModuleHandle("kernel32"), "GetLogicalProcessorInformation" );
    if ( glpi == nullptr ) {
        return;
    }

    while (!bDone)
    {
        DWORD rc = glpi(buffer, &returnLength);

        if (FALSE == rc) {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
                if (buffer)
                    free(buffer);

                buffer = reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION>( ::malloc( returnLength ));

                if ( buffer == nullptr ) {
                    // allocation failed
                    return;
                }
            }
            else {
                // System error
                // _tprintf(TEXT("\nError %d\n"), GetLastError());
                return;
            }
        }
        else
            bDone = true;
    }

    ptr = buffer;

    while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength)
    {
        switch (ptr->Relationship)
        {
        case RelationNumaNode:
            // Non-NUMA systems report a single record of this type.
            numaNodeCount++;
            break;

        case RelationProcessorCore:
            processorCoreCount++;

            // A hyperthreaded core supplies more than one logical processor.
            logicalProcessorCount += cds::bitop::SBC( ptr->ProcessorMask );
            break;

        case RelationCache:
            break;

        case RelationProcessorPackage:
            // Logical processors share a physical package.
            processorPackageCount++;
            break;

        default:
            // Unsupported LOGICAL_PROCESSOR_RELATIONSHIP value
            break;
        }
        byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
        ptr++;
    }

    s_nProcessorCount = logicalProcessorCount;
    s_nProcessorGroupCount = numaNodeCount;

    // Build relationship processor -> cell
    /*
    s_arrProcessorCellRelationship = new unsigned int[s_nProcessorCount];
    memset( s_arrProcessorCellRelationship, 0, s_nProcessorCount * sizeof(s_arrProcessorCellRelationship[0]));
    byteOffset = 0;
    ptr = buffer;
    while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength)
    {
        switch (ptr->Relationship)
        {
        case RelationNumaNode:
            // Non-NUMA systems report a single record of this type.
            for ( unsigned int i = 0; i < sizeof(ptr->ProcessorMask) * 8; ++i ) {
                if ( ptr->ProcessorMask & (1 << i)) {
                    assert( i < s_nProcessorCount );
                    assert( ptr->NumaNode.NodeNumber < s_nProcessorGroupCount );
                    if ( i < s_nProcessorCount )
                        s_arrProcessorCellRelationship[i] = ptr->NumaNode.NodeNumber;
                }
            }
            break;
        }
        byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
        ptr++;
    }
    */

    free(buffer);

    return;
}

namespace cds { namespace OS { CDS_CXX11_INLINE_NAMESPACE namespace Win32 {
    unsigned int    topology::processor_count()
    {
        return s_nProcessorCount;
    }
}}} // namespace cds::OS::Win32

#endif  // #if _WIN32_WINNT < 0x0601

#if _WIN32_WINNT < 0x0600
#   include <cds/os/win/topology.h>
    typedef DWORD (WINAPI * fnGetCurrentProcessorNumber)();
    static fnGetCurrentProcessorNumber s_fnGetCurrentProcessorNumber;

    static void prepare_current_processor_call()
    {
        s_fnGetCurrentProcessorNumber = (fnGetCurrentProcessorNumber) GetProcAddress( GetModuleHandle("kernel32"), "GetCurrentProcessorNumber" );
        if ( s_fnGetCurrentProcessorNumber == nullptr )
            s_fnGetCurrentProcessorNumber = (fnGetCurrentProcessorNumber) GetProcAddress( GetModuleHandle("ntdll"), "NtGetCurrentProcessorNumber" );
    }

    namespace cds { namespace OS { CDS_CXX11_INLINE_NAMESPACE namespace Win32 {
        unsigned int topology::current_processor()
        {
            if ( s_fnGetCurrentProcessorNumber != nullptr )
                return s_fnGetCurrentProcessorNumber();
            return 0;
        }
    }}} // namespace cds::OS::Win32
#endif

extern "C" __declspec(dllexport)
BOOL WINAPI DllMain(
                HINSTANCE hinstDLL,
                DWORD fdwReason,
                LPVOID /*lpvReserved*/
)
{
    switch ( fdwReason ) {
        case DLL_PROCESS_ATTACH:
            s_DllInstance = hinstDLL;
            s_MainThreadId = cds::OS::get_current_thread_id();
#if _WIN32_WINNT < 0x0601
            discover_topology();
#endif
#   if _WIN32_WINNT < 0x0600
            prepare_current_processor_call();
#endif
            break;

        case DLL_PROCESS_DETACH:
/*
#if _WIN32_WINNT < 0x0601
            if ( s_arrProcessorCellRelationship != nullptr ) {
                delete [] s_arrProcessorCellRelationship;
                s_arrProcessorCellRelationship = nullptr;
            }
#endif
*/
            break;
    }
    return TRUE;
}

#endif  // #if CDS_OS_TYPE == CDS_OS_WIN32


