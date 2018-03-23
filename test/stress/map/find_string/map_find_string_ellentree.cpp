// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "map_find_string.h"
#include "map_type_ellen_bintree.h"

namespace map {

    CDSSTRESS_EllenBinTreeMap( Map_find_string, run_test, std::string, Map_find_string::value_type )

} // namespace map
