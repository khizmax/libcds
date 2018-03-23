// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSTEST_EXT_GTEST_H
#define CDSTEST_EXT_GTEST_H


#if defined( __GCC__ ) && !defined(__clang__) && __GNUC__ >= 7
#   pragma GCC diagnostic push
    // Unfortunately, seems, -Wduplicated-branches cannot be controlled via this pragma
#   pragma GCC diagnostic ignored "-Wduplicated-branches"
#endif

#include <gtest/gtest.h>

#if defined( __GCC__ ) && !defined(__clang__) && __GNUC__ >= 7
#   pragma GCC diagnostic pop
#endif

#endif // #ifndef CDSTEST_EXT_GTEST_H
