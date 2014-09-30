//$$CDS-header$$

#include <cds/opt/hash.h>

#include "cppunit/cppunit_proxy.h"

namespace misc {
    namespace {
        typedef cds::opt::v::hash_selector< cds::opt::none >::type hashing;

#define HASHING(_n) \
        struct hash##_n: public hashing { \
            template <typename T> size_t operator()( T const& v ) const { return hashing::operator()(v) + _n ; } \
        };

        HASHING(2)
        HASHING(3)
        HASHING(4)
        HASHING(5)
        HASHING(6)
        HASHING(7)
        HASHING(8)
        HASHING(9)
        HASHING(10)
#undef HASHING
    }

    class HashTuple: public CppUnitMini::TestCase
    {
        void test()
        {
            int nVal = 5;
            size_t nHash = hashing()(nVal);

            size_t val[16];

            cds::opt::hash< std::tuple< hashing, hash2 > >::pack<cds::opt::none>::hash  h2;
            h2( val, nVal );
            CPPUNIT_ASSERT( val[0] == nHash );
            CPPUNIT_ASSERT( val[1] == nHash + 2 );

            cds::opt::hash< std::tuple< hashing, hash2, hash3 > >::pack<cds::opt::none>::hash  h3;
            h3( val, nVal );
            CPPUNIT_ASSERT( val[0] == nHash );
            CPPUNIT_ASSERT( val[1] == nHash + 2 );
            CPPUNIT_ASSERT( val[2] == nHash + 3 );

            cds::opt::hash< std::tuple< hashing, hash2, hash3, hash4 > >::pack<cds::opt::none>::hash  h4;
            h4( val, nVal );
            CPPUNIT_ASSERT( val[0] == nHash );
            CPPUNIT_ASSERT( val[1] == nHash + 2 );
            CPPUNIT_ASSERT( val[2] == nHash + 3 );
            CPPUNIT_ASSERT( val[3] == nHash + 4 );

            cds::opt::hash< std::tuple< hashing, hash2, hash3, hash4, hash5 > >::pack<cds::opt::none>::hash  h5;
            h5( val, nVal );
            CPPUNIT_ASSERT( val[0] == nHash );
            CPPUNIT_ASSERT( val[1] == nHash + 2 );
            CPPUNIT_ASSERT( val[2] == nHash + 3 );
            CPPUNIT_ASSERT( val[3] == nHash + 4 );
            CPPUNIT_ASSERT( val[4] == nHash + 5 );

#if !((CDS_COMPILER == CDS_COMPILER_MSVC || (CDS_COMPILER == CDS_COMPILER_INTEL && CDS_OS_INTERFACE == CDS_OSI_WINDOWS)) && _MSC_VER == 1700)
            // MS VC 11: std::tuple suports up to 5 template params only

            cds::opt::hash< std::tuple< hashing, hash2, hash3, hash4, hash5, hash6 > >::pack<cds::opt::none>::hash  h6;
            h6( val, nVal );
            CPPUNIT_ASSERT( val[0] == nHash );
            CPPUNIT_ASSERT( val[1] == nHash + 2 );
            CPPUNIT_ASSERT( val[2] == nHash + 3 );
            CPPUNIT_ASSERT( val[3] == nHash + 4 );
            CPPUNIT_ASSERT( val[4] == nHash + 5 );
            CPPUNIT_ASSERT( val[5] == nHash + 6 );

            cds::opt::hash< std::tuple< hashing, hash2, hash3, hash4, hash5, hash6, hash7 > >::pack<cds::opt::none>::hash  h7;
            h7( val, nVal );
            CPPUNIT_ASSERT( val[0] == nHash );
            CPPUNIT_ASSERT( val[1] == nHash + 2 );
            CPPUNIT_ASSERT( val[2] == nHash + 3 );
            CPPUNIT_ASSERT( val[3] == nHash + 4 );
            CPPUNIT_ASSERT( val[4] == nHash + 5 );
            CPPUNIT_ASSERT( val[5] == nHash + 6 );
            CPPUNIT_ASSERT( val[6] == nHash + 7 );

            cds::opt::hash< std::tuple< hashing, hash2, hash3, hash4, hash5, hash6, hash7, hash8 > >::pack<cds::opt::none>::hash  h8;
            h8( val, nVal );
            CPPUNIT_ASSERT( val[0] == nHash );
            CPPUNIT_ASSERT( val[1] == nHash + 2 );
            CPPUNIT_ASSERT( val[2] == nHash + 3 );
            CPPUNIT_ASSERT( val[3] == nHash + 4 );
            CPPUNIT_ASSERT( val[4] == nHash + 5 );
            CPPUNIT_ASSERT( val[5] == nHash + 6 );
            CPPUNIT_ASSERT( val[6] == nHash + 7 );
            CPPUNIT_ASSERT( val[7] == nHash + 8 );

            cds::opt::hash< std::tuple< hashing, hash2, hash3, hash4, hash5, hash6, hash7, hash8, hash9 > >::pack<cds::opt::none>::hash  h9;
            h9( val, nVal );
            CPPUNIT_ASSERT( val[0] == nHash );
            CPPUNIT_ASSERT( val[1] == nHash + 2 );
            CPPUNIT_ASSERT( val[2] == nHash + 3 );
            CPPUNIT_ASSERT( val[3] == nHash + 4 );
            CPPUNIT_ASSERT( val[4] == nHash + 5 );
            CPPUNIT_ASSERT( val[5] == nHash + 6 );
            CPPUNIT_ASSERT( val[6] == nHash + 7 );
            CPPUNIT_ASSERT( val[7] == nHash + 8 );
            CPPUNIT_ASSERT( val[8] == nHash + 9 );

            cds::opt::hash< std::tuple< hashing, hash2, hash3, hash4, hash5, hash6, hash7, hash8, hash9, hash10 > >::pack<cds::opt::none>::hash  h10;
            h10( val, nVal );
            CPPUNIT_ASSERT( val[0] == nHash );
            CPPUNIT_ASSERT( val[1] == nHash + 2 );
            CPPUNIT_ASSERT( val[2] == nHash + 3 );
            CPPUNIT_ASSERT( val[3] == nHash + 4 );
            CPPUNIT_ASSERT( val[4] == nHash + 5 );
            CPPUNIT_ASSERT( val[5] == nHash + 6 );
            CPPUNIT_ASSERT( val[6] == nHash + 7 );
            CPPUNIT_ASSERT( val[7] == nHash + 8 );
            CPPUNIT_ASSERT( val[8] == nHash + 9 );
            CPPUNIT_ASSERT( val[9] == nHash + 10 );
#endif
        }

    public:
        CPPUNIT_TEST_SUITE(HashTuple)
            CPPUNIT_TEST( test )
        CPPUNIT_TEST_SUITE_END()

    };
} // namespace misc

CPPUNIT_TEST_SUITE_REGISTRATION(misc::HashTuple);
