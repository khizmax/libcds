//$$CDS-header$$

#include "cppunit/cppunit_proxy.h"

#include <cds/algo/split_bitstring.h>

class Split_bitstrig : public CppUnitMini::TestCase
{
protected:

    void cut_uint()
    {
        union {
            uint32_t ui;
            uint8_t  ch;
        } byte_order;
        byte_order.ui = 0xFF000001;

        if ( byte_order.ch == 0x01 )
            cut_uint_le();
        else
            cut_uint_be();

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

    CPPUNIT_TEST_SUITE(Split_bitstrig);
        CPPUNIT_TEST(cut_uint)
    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(Split_bitstrig);
