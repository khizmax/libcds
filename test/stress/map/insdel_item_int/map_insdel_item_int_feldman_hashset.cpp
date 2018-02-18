// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "map_insdel_item_int.h"
#include "map_type_feldman_hashmap.h"

namespace map {

    CDSSTRESS_FeldmanHashMap_fixed( Map_InsDel_item_int, run_test, size_t, size_t )

} // namespace map
