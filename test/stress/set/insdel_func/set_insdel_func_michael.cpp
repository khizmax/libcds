// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "set_insdel_func.h"
#include "set_type_michael.h"

namespace set {

    CDSSTRESS_MichaelSet( Set_InsDel_func_LF, run_test2, size_t, value )
    CDSSTRESS_MichaelIterableSet( Set_InsDel_func_LF, run_test2, size_t, value )

} // namespace set
