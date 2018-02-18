// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds/urcu/signal_buffered.h>

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED

#include "test_michael_michael_rcu.h"

namespace {

    typedef cds::urcu::signal_buffered<>        rcu_implementation;
    typedef cds::urcu::signal_buffered_stripped rcu_implementation_stripped;

} // namespace

INSTANTIATE_TYPED_TEST_CASE_P( RCU_SHB,          MichaelSet, rcu_implementation );
INSTANTIATE_TYPED_TEST_CASE_P( RCU_SHB_stripped, MichaelSet, rcu_implementation_stripped );

#endif // CDS_URCU_SIGNAL_HANDLING_ENABLED
