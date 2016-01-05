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

#include "set2/set_delodd.h"

namespace set2 {
    CPPUNIT_TEST_SUITE_REGISTRATION( Set_DelOdd );

    void Set_DelOdd::setUpParams( const CppUnitMini::TestCfg& cfg )
    {
        c_nSetSize = cfg.getSizeT("MapSize", c_nSetSize );
        c_nInsThreadCount = cfg.getSizeT("InsThreadCount", c_nInsThreadCount);
        c_nDelThreadCount = cfg.getSizeT("DelThreadCount", c_nDelThreadCount);
        c_nExtractThreadCount = cfg.getSizeT("ExtractThreadCount", c_nExtractThreadCount);
        c_nMaxLoadFactor = cfg.getSizeT("MaxLoadFactor", c_nMaxLoadFactor);
        c_bPrintGCState = cfg.getBool("PrintGCStateFlag", true );

        c_nCuckooInitialSize = cfg.getSizeT("CuckooInitialSize", c_nCuckooInitialSize );
        c_nCuckooProbesetSize = cfg.getSizeT("CuckooProbesetSize", c_nCuckooProbesetSize );
        c_nCuckooProbesetThreshold = cfg.getSizeT("CuckooProbesetThreshold", c_nCuckooProbesetThreshold );

        c_nFeldmanSet_HeadBits = cfg.getSizeT("FeldmanMapHeadBits", c_nFeldmanSet_HeadBits);
        c_nFeldmanSet_ArrayBits = cfg.getSizeT("FeldmanMapArrayBits", c_nFeldmanSet_ArrayBits);

        if ( c_nInsThreadCount == 0 )
            c_nInsThreadCount = std::thread::hardware_concurrency();
        if ( c_nDelThreadCount == 0 && c_nExtractThreadCount == 0 ) {
            c_nExtractThreadCount = std::thread::hardware_concurrency() / 2;
            c_nDelThreadCount = std::thread::hardware_concurrency() - c_nExtractThreadCount;
        }

        m_arrData.resize( c_nSetSize );
        for ( size_t i = 0; i < c_nSetSize; ++i )
            m_arrData[i] = i;
        shuffle( m_arrData.begin(), m_arrData.end() );
    }

    void Set_DelOdd::endTestCase()
    {
        m_arrData.resize( 0 );
    }

} // namespace set2
