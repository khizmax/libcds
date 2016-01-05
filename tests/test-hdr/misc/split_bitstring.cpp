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

#include <cds/algo/split_bitstring.h>

class Split_bitstrig : public CppUnitMini::TestCase
{
private:
    bool is_big_endian()
    {
        union {
            uint32_t ui;
            uint8_t  ch;
        } byte_order;
        byte_order.ui = 0xFF000001;

        return byte_order.ch != 0x01;
    }
protected:

    void cut_uint()
    {
        if ( is_big_endian() )
            cut_uint_be();
        else
            cut_uint_le();
    }

    void cut_uint16()
    {
        if ( is_big_endian() )
            cut_small_be<uint16_t>();
        else
            cut_small_le<uint16_t>();
    }

    void cut_uint_le()
    {
        CPPUNIT_MSG("little-endian byte order");

        typedef cds::algo::split_bitstring< size_t > split_bitstring;

        size_t src = sizeof(src) == 8 ? 0xFEDCBA9876543210 : 0x76543210;
        split_bitstring splitter(src);
        size_t res;

        // Trivial case
        CPPUNIT_ASSERT( !splitter.eos() );
        CPPUNIT_ASSERT( splitter );
        res = splitter.cut(sizeof(src) * 8);
        CPPUNIT_ASSERT_EX( res == src, "src=" << src << ", result=" << res );
        CPPUNIT_ASSERT( splitter.eos() );
        CPPUNIT_ASSERT( !splitter );
        CPPUNIT_ASSERT(splitter.safe_cut(sizeof(src) * 8) == 0 );
        CPPUNIT_ASSERT( splitter.eos() );
        CPPUNIT_ASSERT( !splitter );
        splitter.reset();
        CPPUNIT_ASSERT( !splitter.eos() );
        CPPUNIT_ASSERT( splitter );
        res = splitter.cut(sizeof(src) * 8);
        CPPUNIT_ASSERT_EX( res == src, "src=" << src << ", result=" << res );
        CPPUNIT_ASSERT( splitter.eos() );
        CPPUNIT_ASSERT( !splitter );
        CPPUNIT_ASSERT(splitter.safe_cut(sizeof(src) * 8) == 0 );
        CPPUNIT_ASSERT( splitter.eos() );
        CPPUNIT_ASSERT( !splitter );

        // Cut each hex digit
        splitter.reset();
        for ( size_t i = 0; i < sizeof(size_t) * 2; ++i ) {
            CPPUNIT_ASSERT( !splitter.eos() );
            CPPUNIT_ASSERT( splitter );
            CPPUNIT_ASSERT( splitter.cut( 4 ) == i );
        }
        CPPUNIT_ASSERT( splitter.eos() );
        CPPUNIT_ASSERT( !splitter );

        // by one bit
        {
            splitter.reset();
            res = 0;
            for ( size_t i = 0; i < sizeof(size_t) * 8; ++i ) {
                CPPUNIT_ASSERT( !splitter.eos() );
                CPPUNIT_ASSERT( splitter );
                res = res + (splitter.cut( 1 ) << i);
            }
            CPPUNIT_ASSERT( splitter.eos() );
            CPPUNIT_ASSERT( !splitter );
            CPPUNIT_ASSERT( res == src );
        }

        // random cut
        {
            for ( size_t k = 0; k < 100; ++k ) {
                splitter.reset();
                res = 0;
                size_t shift = 0;
                while ( splitter ) {
                    CPPUNIT_ASSERT( !splitter.eos() );
                    CPPUNIT_ASSERT( splitter );
                    int bits = rand() % 16;
                    res = res + ( splitter.safe_cut( bits ) << shift );
                    shift += bits;
                }
                CPPUNIT_ASSERT( splitter.eos() );
                CPPUNIT_ASSERT( !splitter );
                CPPUNIT_ASSERT( res == src );
            }
        }
    }

    void cut_uint_be()
    {
        CPPUNIT_MSG("big-endian byte order");

        typedef cds::algo::split_bitstring< size_t > split_bitstring;

        size_t src = sizeof(src) == 8 ? 0xFEDCBA9876543210 : 0x76543210;
        split_bitstring splitter(src);
        size_t res;

        // Trivial case
        CPPUNIT_ASSERT( !splitter.eos() );
        CPPUNIT_ASSERT( splitter );
        res = splitter.cut(sizeof(src) * 8);
        CPPUNIT_ASSERT_EX( res == src, "src=" << src << ", result=" << res );
        CPPUNIT_ASSERT( splitter.eos() );
        CPPUNIT_ASSERT( !splitter );
        CPPUNIT_ASSERT(splitter.safe_cut(sizeof(src) * 8) == 0 );
        CPPUNIT_ASSERT( splitter.eos() );
        CPPUNIT_ASSERT( !splitter );
        splitter.reset();
        CPPUNIT_ASSERT( !splitter.eos() );
        CPPUNIT_ASSERT( splitter );
        res = splitter.cut(sizeof(src) * 8);
        CPPUNIT_ASSERT_EX( res == src, "src=" << src << ", result=" << res );
        CPPUNIT_ASSERT( splitter.eos() );
        CPPUNIT_ASSERT( !splitter );
        CPPUNIT_ASSERT(splitter.safe_cut(sizeof(src) * 8) == 0 );
        CPPUNIT_ASSERT( splitter.eos() );
        CPPUNIT_ASSERT( !splitter );

        // Cut each hex digit
        splitter.reset();
        for ( size_t i = 0; i < sizeof(size_t) * 2; ++i ) {
            CPPUNIT_ASSERT( !splitter.eos() );
            CPPUNIT_ASSERT( splitter );
            CPPUNIT_ASSERT( splitter.cut( 4 ) == 0x0F - i );
        }
        CPPUNIT_ASSERT( splitter.eos() );
        CPPUNIT_ASSERT( !splitter );

        // by one bit
        {
            splitter.reset();
            res = 0;
            for ( size_t i = 0; i < sizeof(size_t) * 8; ++i ) {
                CPPUNIT_ASSERT( !splitter.eos() );
                CPPUNIT_ASSERT( splitter );
                res = (res << 1) + splitter.cut( 1 );
            }
            CPPUNIT_ASSERT( splitter.eos() );
            CPPUNIT_ASSERT( !splitter );
            CPPUNIT_ASSERT( res == src );
        }

        // random cut
        {
            for ( size_t k = 0; k < 100; ++k ) {
                splitter.reset();
                res = 0;
                while ( splitter ) {
                    CPPUNIT_ASSERT( !splitter.eos() );
                    CPPUNIT_ASSERT( splitter );
                    int bits = rand() % 16;
                    res = (res << bits) + splitter.safe_cut( bits );
                }
                CPPUNIT_ASSERT( splitter.eos() );
                CPPUNIT_ASSERT( !splitter );
                CPPUNIT_ASSERT( res == src );
            }
        }
    }


private:
    template <typename PartUInt>
    void cut_small_le()
    {
        CPPUNIT_MSG("little-endian byte order");
        typedef PartUInt part_uint;

        typedef cds::algo::split_bitstring< uint64_t, part_uint > split_bitstring;

        uint64_t src = 0xFEDCBA9876543210;
        split_bitstring splitter(src);
        uint64_t res;

        // Cut each hex digit
        splitter.reset();
        for ( size_t i = 0; i < sizeof(size_t) * 2; ++i ) {
            CPPUNIT_ASSERT( !splitter.eos() );
            CPPUNIT_ASSERT( splitter );
            CPPUNIT_ASSERT( static_cast<size_t>(splitter.cut( 4 )) == i );
        }
        CPPUNIT_ASSERT( splitter.eos() );
        CPPUNIT_ASSERT( !splitter );

        // by one bit
        {
            splitter.reset();
            res = 0;
            for ( size_t i = 0; i < sizeof(size_t) * 8; ++i ) {
                CPPUNIT_ASSERT( !splitter.eos() );
                CPPUNIT_ASSERT( splitter );
                res = res + ( static_cast<uint64_t>(splitter.cut( 1 )) << i);
            }
            CPPUNIT_ASSERT( splitter.eos() );
            CPPUNIT_ASSERT( !splitter );
            CPPUNIT_ASSERT( res == src );
        }

        // random cut
        {
            for ( size_t k = 0; k < 100; ++k ) {
                splitter.reset();
                res = 0;
                size_t shift = 0;
                while ( splitter ) {
                    CPPUNIT_ASSERT( !splitter.eos() );
                    CPPUNIT_ASSERT( splitter );
                    int bits = rand() % 16;
                    res = res + ( static_cast<uint64_t>(splitter.safe_cut( bits )) << shift );
                    shift += bits;
                }
                CPPUNIT_ASSERT( splitter.eos() );
                CPPUNIT_ASSERT( !splitter );
                CPPUNIT_ASSERT( res == src );
            }
        }
    }

    template <typename PartUInt>
    void cut_small_be()
    {
        CPPUNIT_MSG("big-endian byte order");
        typedef PartUInt part_uint;

        typedef cds::algo::split_bitstring< uint64_t, part_uint > split_bitstring;

        uint64_t src = 0xFEDCBA9876543210;
        split_bitstring splitter(src);
        uint64_t res;

        // Cut each hex digit
        splitter.reset();
        for ( size_t i = 0; i < sizeof(size_t) * 2; ++i ) {
            CPPUNIT_ASSERT( !splitter.eos() );
            CPPUNIT_ASSERT( splitter );
            CPPUNIT_ASSERT( splitter.cut( 4 ) == 0x0F - i );
        }
        CPPUNIT_ASSERT( splitter.eos() );
        CPPUNIT_ASSERT( !splitter );

        // by one bit
        {
            splitter.reset();
            res = 0;
            for ( size_t i = 0; i < sizeof(size_t) * 8; ++i ) {
                CPPUNIT_ASSERT( !splitter.eos() );
                CPPUNIT_ASSERT( splitter );
                res = (res << 1) + splitter.cut( 1 );
            }
            CPPUNIT_ASSERT( splitter.eos() );
            CPPUNIT_ASSERT( !splitter );
            CPPUNIT_ASSERT( res == src );
        }

        // random cut
        {
            for ( size_t k = 0; k < 100; ++k ) {
                splitter.reset();
                res = 0;
                while ( splitter ) {
                    CPPUNIT_ASSERT( !splitter.eos() );
                    CPPUNIT_ASSERT( splitter );
                    int bits = rand() % 16;
                    res = (res << bits) + splitter.safe_cut( bits );
                }
                CPPUNIT_ASSERT( splitter.eos() );
                CPPUNIT_ASSERT( !splitter );
                CPPUNIT_ASSERT( res == src );
            }
        }
    }


    CPPUNIT_TEST_SUITE(Split_bitstrig);
        CPPUNIT_TEST(cut_uint)
        CPPUNIT_TEST(cut_uint16)
    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(Split_bitstrig);
