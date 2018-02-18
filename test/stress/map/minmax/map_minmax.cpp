// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "map_minmax.h"

namespace map {

    size_t  Map_MinMax::s_nMapSize = 50000;
    size_t  Map_MinMax::s_nInsThreadCount = 4;
    size_t  Map_MinMax::s_nExtractThreadCount = 4;
    size_t  Map_MinMax::s_nPassCount = 1000;

    size_t Map_MinMax::s_nFeldmanMap_HeadBits = 8;
    size_t Map_MinMax::s_nFeldmanMap_ArrayBits = 8;

    void Map_MinMax::SetUpTestCase()
    {
        cds_test::config const& cfg = get_config( "map_minmax" );

        s_nMapSize = cfg.get_size_t( "MapSize", s_nMapSize );
        if ( s_nMapSize < 1000 )
            s_nMapSize = 1000;

        s_nInsThreadCount = cfg.get_size_t( "InsThreadCount", s_nInsThreadCount );
        if ( s_nInsThreadCount == 0 )
            s_nInsThreadCount = 1;

        s_nExtractThreadCount = cfg.get_size_t( "ExtractThreadCount", s_nExtractThreadCount );
        if ( s_nExtractThreadCount )
            s_nExtractThreadCount = 1;

        s_nPassCount = cfg.get_size_t( "PassCount", s_nPassCount );
        if ( s_nPassCount == 0 )
            s_nPassCount = 100;

        s_nFeldmanMap_HeadBits = cfg.get_size_t( "FeldmanMapHeadBits", s_nFeldmanMap_HeadBits );
        if ( s_nFeldmanMap_HeadBits == 0 )
            s_nFeldmanMap_HeadBits = 4;

        s_nFeldmanMap_ArrayBits = cfg.get_size_t( "FeldmanMapArrayBits", s_nFeldmanMap_ArrayBits );
        if ( s_nFeldmanMap_ArrayBits == 0 )
            s_nFeldmanMap_ArrayBits = 4;
    }

} // namespace map
