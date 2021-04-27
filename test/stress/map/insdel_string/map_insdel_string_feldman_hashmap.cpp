// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "map_insdel_string.h"
#include "map_type_feldman_hashmap.h"

namespace map {

    CDSSTRESS_FeldmanHashMap_stdhash( Map_InsDel_string_stdhash, run_test, std::string, size_t )
#if CDS_BUILD_BITS == 64
        CDSSTRESS_FeldmanHashMap_city64( Map_InsDel_string_city64, run_test, std::string, size_t )
        CDSSTRESS_FeldmanHashMap_city128( Map_InsDel_string_city128, run_test, std::string, size_t )
#endif

} // namespace map
