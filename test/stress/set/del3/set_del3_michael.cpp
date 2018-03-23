// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "set_del3.h"
#include "set_type_michael.h"

namespace set {

    CDSSTRESS_MichaelSet( Set_Del3_LF, run_test_extract, key_thread, size_t )
    CDSSTRESS_MichaelIterableSet( Set_Del3_LF, run_test_extract, key_thread, size_t )

} // namespace set
