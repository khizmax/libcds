// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds/urcu/general_threaded.h>

#include "test_split_michael_rcu.h"

namespace {

    typedef cds::urcu::general_threaded<>        rcu_implementation;
    typedef cds::urcu::general_threaded_stripped rcu_implementation_stripped;

} // namespace

INSTANTIATE_TYPED_TEST_CASE_P( RCU_GPT,          SplitListMichaelSet, rcu_implementation );
INSTANTIATE_TYPED_TEST_CASE_P( RCU_GPT_stripped, SplitListMichaelSet, rcu_implementation_stripped );
