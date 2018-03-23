// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef CDS_ADDRESS_SANITIZER_ENABLED

#include <cds_test/ext_gtest.h>

namespace {

    class asan: public ::testing::Test
    {};

    TEST_F( asan, memory_leak )
    {
        constexpr size_t size = 100;

        size_t* arr = new size_t[size];
        for ( size_t i = 0; i < size; ++i ) {
            arr[i] = i;
        }
    }

    TEST_F( asan, array_bound )
    {
        constexpr size_t size = 100;

        size_t arr[size];
        size_t protect = 0;

        for ( size_t i = 0; i <= size; ++i ) {
            arr[i] = i;
        }

        protect = 0;
    }

    TEST_F( asan, uninit_var )
    {
        constexpr size_t size = 100;

        size_t arr[size];

        for ( size_t i = 0; i < size; ++i ) {
            arr[i] = i;
        }

        size_t n = arr[n];
        n = arr[n / 2];
    }
} // namespace

#endif // #ifdef CDS_ADDRESS_SANITIZER_ENABLED
