// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "set_iteration.h"
#include "set_type_feldman_hashset.h"

namespace set {

    CDSSTRESS_FeldmanHashSet_stdhash( Set_Iteration, run_test_extract, std::string, Set_Iteration::value_type )
    CDSSTRESS_FeldmanHashSet_city( Set_Iteration, run_test_extract, std::string, Set_Iteration::value_type )

} // namespace set
