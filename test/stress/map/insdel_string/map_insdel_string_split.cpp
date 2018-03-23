// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "map_insdel_string.h"
#include "map_type_split_list.h"

namespace map {

    CDSSTRESS_SplitListMap( Map_InsDel_string_LF, run_test, std::string, size_t )
    CDSSTRESS_SplitListIterableMap( Map_InsDel_string_LF, run_test, std::string, size_t )

} // namespace map
