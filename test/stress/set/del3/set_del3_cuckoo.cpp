// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "set_del3.h"
#include "set_type_cuckoo.h"

namespace set {

    CDSSTRESS_CuckooSet( Set_Del3, run_test, key_thread, size_t )

} // namespace set
