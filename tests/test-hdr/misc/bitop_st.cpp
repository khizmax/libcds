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

#include "cppunit/cppunit_proxy.h"

#include <cds/algo/int_algo.h>
#include <cds/os/timer.h>

class bitop_ST : public CppUnitMini::TestCase
{
protected:
    void bitop32()
    {
        uint32_t    n;
        n = 0;
        CPPUNIT_ASSERT_EX( cds::bitop::MSB(n) == 0, "n=" << n );
        CPPUNIT_ASSERT_EX( cds::bitop::LSB(n) == 0, "n=" << n );
        CPPUNIT_ASSERT_EX( cds::bitop::SBC(n) == 0, "n=" << n );
        CPPUNIT_ASSERT_EX( cds::bitop::ZBC(n) == sizeof(n) * 8, "n=" << n );

        int nBit = 1;
        for ( n = 1; n != 0; n *= 2 ) {
            CPPUNIT_ASSERT_EX( cds::bitop::MSB(n) == nBit, "n=" << n );
            CPPUNIT_ASSERT_EX( cds::bitop::LSB(n) == nBit, "n=" << n );
            CPPUNIT_ASSERT_EX( cds::bitop::MSBnz(n) == nBit - 1, "n=" << n );
            CPPUNIT_ASSERT_EX( cds::bitop::LSBnz(n) == nBit - 1, "n=" << n );
            CPPUNIT_ASSERT_EX( cds::bitop::SBC(n) == 1, "n=" << n );
            CPPUNIT_ASSERT_EX( cds::bitop::ZBC(n) == sizeof(n) * 8 - 1, "n=" << n );

            ++nBit;
        }
    }

    void bitop64()
    {
        uint64_t n;
        n = 0;
        CPPUNIT_ASSERT_EX( cds::bitop::MSB(n) == 0, "n=" << n );
        CPPUNIT_ASSERT_EX( cds::bitop::LSB(n) == 0, "n=" << n );
        CPPUNIT_ASSERT_EX( cds::bitop::SBC(n) == 0, "n=" << n );
        CPPUNIT_ASSERT_EX( cds::bitop::ZBC(n) == sizeof(n) * 8, "n=" << n );

        int nBit = 1;
        for ( n = 1; n != 0; n *= 2 ) {
            CPPUNIT_ASSERT_EX( cds::bitop::MSB(n) == nBit, "n=" << n );
            CPPUNIT_ASSERT_EX( cds::bitop::LSB(n) == nBit, "n=" << n );
            CPPUNIT_ASSERT_EX( cds::bitop::MSBnz(n) == nBit - 1, "n=" << n );
            CPPUNIT_ASSERT_EX( cds::bitop::LSBnz(n) == nBit - 1, "n=" << n );
            CPPUNIT_ASSERT_EX( cds::bitop::SBC(n) == 1, "n=" << n );
            CPPUNIT_ASSERT_EX( cds::bitop::ZBC(n) == sizeof(n) * 8 - 1, "n=" << n );

            ++nBit;
        }
    }

    void floor_ceil_pow2()
    {
        CPPUNIT_CHECK_EX( cds::beans::floor2(0) == 1, "floor2(0) = " << cds::beans::floor2(0) << ", expected 1" );
        CPPUNIT_CHECK_EX( cds::beans::floor2(1) == 1, "floor2(1) = " << cds::beans::floor2(1) << ", expected 1" );
        CPPUNIT_CHECK_EX( cds::beans::floor2(2) == 2, "floor2(2) = " << cds::beans::floor2(2) << ", expected 2" );
        CPPUNIT_CHECK_EX( cds::beans::floor2(3) == 2, "floor2(3) = " << cds::beans::floor2(3) << ", expected 2" );
        CPPUNIT_CHECK_EX( cds::beans::floor2(4) == 4, "floor2(4) = " << cds::beans::floor2(4) << ", expected 4" );
        CPPUNIT_CHECK_EX( cds::beans::floor2(5) == 4, "floor2(5) = " << cds::beans::floor2(5) << ", expected 4" );
        CPPUNIT_CHECK_EX( cds::beans::floor2(7) == 4, "floor2(7) = " << cds::beans::floor2(7) << ", expected 4" );
        CPPUNIT_CHECK_EX( cds::beans::floor2(8) == 8, "floor2(8) = " << cds::beans::floor2(8) << ", expected 8" );
        CPPUNIT_CHECK_EX( cds::beans::floor2(9) == 8, "floor2(9) = " << cds::beans::floor2(9) << ", expected 8" );

        CPPUNIT_CHECK_EX( cds::beans::ceil2(0) == 1, "ceil2(0) = " << cds::beans::ceil2(0) << ", expected 1" );
        CPPUNIT_CHECK_EX( cds::beans::ceil2(1) == 1, "ceil2(1) = " << cds::beans::ceil2(1) << ", expected 1" );
        CPPUNIT_CHECK_EX( cds::beans::ceil2(2) == 2, "ceil2(2) = " << cds::beans::ceil2(2) << ", expected 2" );
        CPPUNIT_CHECK_EX( cds::beans::ceil2(3) == 4, "ceil2(3) = " << cds::beans::ceil2(3) << ", expected 4" );
        CPPUNIT_CHECK_EX( cds::beans::ceil2(4) == 4, "ceil2(4) = " << cds::beans::ceil2(4) << ", expected 4" );
        CPPUNIT_CHECK_EX( cds::beans::ceil2(5) == 8, "ceil2(5) = " << cds::beans::ceil2(5) << ", expected 8" );
        CPPUNIT_CHECK_EX( cds::beans::ceil2(7) == 8, "ceil2(7) = " << cds::beans::ceil2(7) << ", expected 8" );
        CPPUNIT_CHECK_EX( cds::beans::ceil2(8) == 8, "ceil2(8) = " << cds::beans::ceil2(8) << ", expected 8" );
        CPPUNIT_CHECK_EX( cds::beans::ceil2(9) == 16, "ceil2(9) = " << cds::beans::ceil2(16) << ", expected 16" );
    }

    CPPUNIT_TEST_SUITE(bitop_ST);
        CPPUNIT_TEST(bitop32)
        CPPUNIT_TEST(bitop64)
        CPPUNIT_TEST(floor_ceil_pow2)
    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(bitop_ST);
