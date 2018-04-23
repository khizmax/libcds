// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSTEST_FIXTURE_H
#define CDSTEST_FIXTURE_H

#include <cds_test/ext_gtest.h>
#include <algorithm>
#include <random>

namespace cds_test {

    class fixture : public ::testing::Test
    {
    public:
        template <typename RandomIt>
        static void shuffle( RandomIt first, RandomIt last )
        {
            std::shuffle( first, last, random_gen_ );
        }

        static unsigned int rand( unsigned int nMax )
        {
            std::uniform_int_distribution<unsigned> dis(0, nMax - 1);
            return dis( random_gen_ );
        }

    private:
        static std::random_device random_dev_;
        static std::mt19937 random_gen_;
    };

} // namespace cds_test

#endif // CDSTEST_FIXTURE_H
