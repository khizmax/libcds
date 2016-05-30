/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

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

#ifndef CDSTEST_FIXTURE_H
#define CDSTEST_FIXTURE_H

#include <gtest/gtest.h>
#include <algorithm>
#include <random>

// earlier version of gtest do not support 4th parameter in INSTANTIATE_TEST_CASE_P macro
//TODO: how to known gtest version?..
//#define CDSTEST_GTEST_INSTANTIATE_TEST_CASE_P_HAS_4TH_ARG

namespace cds_test {

    class fixture : public ::testing::Test
    {
    protected:
        template <typename RandomIt>
        static void shuffle( RandomIt first, RandomIt last )
        {
            static std::random_device random_dev;
            static std::mt19937       random_gen( random_dev() );

            std::shuffle( first, last, random_gen );
        }

        static inline unsigned int rand( unsigned int nMax )
        {
            double rnd = double( std::rand() ) / double( RAND_MAX );
            unsigned int n = (unsigned int)(rnd * nMax);
            return n < nMax ? n : (n - 1);
        }
    };

} // namespace cds_test

#endif // CDSTEST_FIXTURE_H
