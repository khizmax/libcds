// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds/urcu/general_buffered.h>

#include "test_ellen_bintree_map_rcu.h"

namespace {

    typedef cds::urcu::general_buffered<>        rcu_implementation;
    typedef cds::urcu::general_buffered_stripped rcu_implementation_stripped;

} // namespace

INSTANTIATE_TYPED_TEST_CASE_P( RCU_GPB,          EllenBinTreeMap, rcu_implementation );
INSTANTIATE_TYPED_TEST_CASE_P( RCU_GPB_stripped, EllenBinTreeMap, rcu_implementation_stripped );
