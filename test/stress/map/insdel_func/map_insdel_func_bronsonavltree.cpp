// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "map_insdel_func.h"
#include "map_type_bronson_avltree.h"

namespace map {

    CDSSTRESS_BronsonAVLTreeMap( Map_InsDel_func, run_test, size_t, Map_InsDel_func::value_type )

} // namespace map
