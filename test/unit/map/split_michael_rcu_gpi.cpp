// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds/urcu/general_instant.h>

#include "test_split_michael_rcu.h"

namespace {

    typedef cds::urcu::general_instant<>        rcu_implementation;
    typedef cds::urcu::general_instant_stripped rcu_implementation_stripped;

} // namespace

INSTANTIATE_TYPED_TEST_CASE_P( RCU_GPI,          SplitListMichaelMap, rcu_implementation );
INSTANTIATE_TYPED_TEST_CASE_P( RCU_GPI_stripped, SplitListMichaelMap, rcu_implementation_stripped );
