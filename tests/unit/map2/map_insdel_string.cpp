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

#include "map2/map_insdel_string.h"

namespace map2 {
    CPPUNIT_TEST_SUITE_REGISTRATION( Map_InsDel_string );

    void Map_InsDel_string::setUpParams( const CppUnitMini::TestCfg& cfg )
    {
        c_nInsertThreadCount = cfg.getSizeT("InsertThreadCount", c_nInsertThreadCount );
        c_nDeleteThreadCount = cfg.getSizeT("DeleteThreadCount", c_nDeleteThreadCount );
        c_nThreadPassCount = cfg.getSizeT("ThreadPassCount", c_nThreadPassCount );
        c_nMapSize = cfg.getSizeT("MapSize", c_nMapSize );
        c_nMaxLoadFactor = cfg.getSizeT("MaxLoadFactor", c_nMaxLoadFactor );
        c_bPrintGCState = cfg.getBool("PrintGCStateFlag", c_bPrintGCState );

        c_nCuckooInitialSize = cfg.getSizeT("CuckooInitialSize", c_nCuckooInitialSize);
        c_nCuckooProbesetSize = cfg.getSizeT("CuckooProbesetSize", c_nCuckooProbesetSize);
        c_nCuckooProbesetThreshold = cfg.getSizeT("CuckooProbesetThreshold", c_nCuckooProbesetThreshold);

        c_nFeldmanMap_HeadBits = cfg.getSizeT("FeldmanMapHeadBits", c_nFeldmanMap_HeadBits);
        c_nFeldmanMap_ArrayBits = cfg.getSizeT("FeldmanMapArrayBits", c_nFeldmanMap_ArrayBits);

        if ( c_nInsertThreadCount == 0 )
            c_nInsertThreadCount = std::thread::hardware_concurrency();
        if ( c_nDeleteThreadCount == 0 )
            c_nDeleteThreadCount = std::thread::hardware_concurrency();
    }
} // namespace map2
