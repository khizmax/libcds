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

#include "set2/set_insdelfind.h"

namespace set2 {
    CPPUNIT_TEST_SUITE_REGISTRATION( Set_InsDelFind );

    void Set_InsDelFind::setUpParams( const CppUnitMini::TestCfg& cfg )
    {
        c_nSetSize = cfg.getSizeT("InitialMapSize", c_nSetSize );
        c_nThreadCount = cfg.getSizeT("ThreadCount", c_nThreadCount );
        c_nMaxLoadFactor = cfg.getSizeT("MaxLoadFactor", c_nMaxLoadFactor );
        c_nInsertPercentage = cfg.getUInt("InsertPercentage", c_nInsertPercentage );
        c_nDeletePercentage = cfg.getUInt("DeletePercentage", c_nDeletePercentage );
        c_nDuration = cfg.getUInt("Duration", c_nDuration );
        c_bPrintGCState = cfg.getBool("PrintGCStateFlag", c_bPrintGCState );

        c_nCuckooInitialSize = cfg.getSizeT("CuckooInitialSize", c_nCuckooInitialSize );
        c_nCuckooProbesetSize = cfg.getSizeT("CuckooProbesetSize", c_nCuckooProbesetSize );
        c_nCuckooProbesetThreshold = cfg.getSizeT("CuckooProbesetThreshold", c_nCuckooProbesetThreshold );

        c_nFeldmanSet_HeadBits = cfg.getSizeT("FeldmanMapHeadBits", c_nFeldmanSet_HeadBits);
        c_nFeldmanSet_ArrayBits = cfg.getSizeT("FeldmanMapArrayBits", c_nFeldmanSet_ArrayBits);

        if ( c_nThreadCount == 0 )
            c_nThreadCount = std::thread::hardware_concurrency();

        CPPUNIT_ASSERT( c_nInsertPercentage + c_nDeletePercentage <= 100 );

        actions * pFirst = m_arrShuffle;
        actions * pLast = m_arrShuffle + c_nInsertPercentage;
        std::fill( pFirst, pLast, do_insert );
        pFirst = pLast;
        pLast += c_nDeletePercentage;
        std::fill( pFirst, pLast, do_delete );
        pFirst = pLast;
        pLast = m_arrShuffle + sizeof(m_arrShuffle)/sizeof(m_arrShuffle[0]);
        std::fill( pFirst, pLast, do_find );
        shuffle( m_arrShuffle, pLast );
    }
} // namespace set2
