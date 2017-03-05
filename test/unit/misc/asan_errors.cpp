/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifdef CDS_ADDRESS_SANITIZER_ENABLED

#include <gtest/gtest.h>

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
