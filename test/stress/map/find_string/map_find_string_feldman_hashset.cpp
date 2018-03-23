// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "map_find_string.h"
#include "map_type_feldman_hashmap.h"

namespace map {

    CDSSTRESS_FeldmanHashMap_stdhash( Map_find_string_stdhash, run_test, std::string, Map_find_string::value_type )
#if CDS_BUILD_BITS == 64
    CDSSTRESS_FeldmanHashMap_city64(  Map_find_string_city64,  run_test, std::string, Map_find_string::value_type )
    CDSSTRESS_FeldmanHashMap_city128( Map_find_string_city128, run_test, std::string, Map_find_string::value_type )
#endif

} // namespace map
