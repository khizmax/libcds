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

#include "set_insdelfind.h"

namespace set {

    size_t Set_InsDelFind::s_nSetSize = 500000;      // initial set size
    size_t Set_InsDelFind::s_nThreadCount = 8;       // thread count
    size_t Set_InsDelFind::s_nMaxLoadFactor = 8;     // maximum load factor
    unsigned int Set_InsDelFind::s_nInsertPercentage = 5;
    unsigned int Set_InsDelFind::s_nDeletePercentage = 5;
    unsigned int Set_InsDelFind::s_nDuration = 30;   // test duration, seconds

    size_t Set_InsDelFind::s_nCuckooInitialSize = 1024;// initial size for CuckooSet
    size_t Set_InsDelFind::s_nCuckooProbesetSize = 16; // CuckooSet probeset size (only for list-based probeset)
    size_t Set_InsDelFind::s_nCuckooProbesetThreshold = 0; // CUckooSet probeset threshold (0 - use default)

    size_t Set_InsDelFind::s_nFeldmanSet_HeadBits = 10;
    size_t Set_InsDelFind::s_nFeldmanSet_ArrayBits = 4;

    size_t Set_InsDelFind::s_nLoadFactor = 2;

    void Set_InsDelFind::SetUpTestCase()
    {
        cds_test::config const& cfg = get_config( "map_insdelfind" );

        s_nSetSize = cfg.get_size_t( "InitialMapSize", s_nSetSize );
        if ( s_nSetSize < 1000 )
            s_nSetSize = 1000;

        s_nThreadCount = cfg.get_size_t( "ThreadCount", s_nThreadCount );
        if ( s_nThreadCount == 0 )
            s_nThreadCount = 2;

        s_nMaxLoadFactor = cfg.get_size_t( "MaxLoadFactor", s_nMaxLoadFactor );
        if ( s_nMaxLoadFactor == 0 )
            s_nMaxLoadFactor = 1;

        s_nInsertPercentage = cfg.get_uint( "InsertPercentage", s_nInsertPercentage );
        if ( s_nInsertPercentage == 0 || s_nInsertPercentage > 90 )
            s_nInsertPercentage = 5;

        s_nDeletePercentage = cfg.get_uint( "DeletePercentage", s_nDeletePercentage );
        if ( s_nDeletePercentage == 0 || s_nDeletePercentage > 90 )
            s_nDeletePercentage = 5;

        s_nDuration = cfg.get_uint( "Duration", s_nDuration );
        if ( s_nDuration == 0 )
            s_nDuration = 5;

        s_nCuckooInitialSize = cfg.get_size_t( "CuckooInitialSize", s_nCuckooInitialSize );
        if ( s_nCuckooInitialSize < 256 )
            s_nCuckooInitialSize = 256;

        s_nCuckooProbesetSize = cfg.get_size_t( "CuckooProbesetSize", s_nCuckooProbesetSize );
        if ( s_nCuckooProbesetSize < 8 )
            s_nCuckooProbesetSize = 8;

        s_nCuckooProbesetThreshold = cfg.get_size_t( "CuckooProbesetThreshold", s_nCuckooProbesetThreshold );

        s_nFeldmanSet_HeadBits = cfg.get_size_t( "FeldmanMapHeadBits", s_nFeldmanSet_HeadBits );
        if ( s_nFeldmanSet_HeadBits == 0 )
            s_nFeldmanSet_HeadBits = 2;

        s_nFeldmanSet_ArrayBits = cfg.get_size_t( "FeldmanMapArrayBits", s_nFeldmanSet_ArrayBits );
        if ( s_nFeldmanSet_ArrayBits == 0 )
            s_nFeldmanSet_ArrayBits = 2;
    }

    std::vector<size_t> Set_InsDelFind_LF::get_load_factors()
    {
        cds_test::config const& cfg = get_config( "map_delodd" );

        s_nMaxLoadFactor = cfg.get_size_t( "MaxLoadFactor", s_nMaxLoadFactor );
        if ( s_nMaxLoadFactor == 0 )
            s_nMaxLoadFactor = 1;

        std::vector<size_t> lf;
        for ( size_t n = 1; n <= s_nMaxLoadFactor; n *= 2 )
            lf.push_back( n );

        return lf;
    }

    INSTANTIATE_TEST_CASE_P( a, Set_InsDelFind_LF, ::testing::ValuesIn( Set_InsDelFind_LF::get_load_factors()));
} // namespace set
