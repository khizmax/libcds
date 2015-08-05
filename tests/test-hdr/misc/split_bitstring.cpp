//$$CDS-header$$

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
