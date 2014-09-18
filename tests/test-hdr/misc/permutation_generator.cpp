//$$CDS-header$$

#include <cds/opt/permutation.h>
#include "cppunit/cppunit_proxy.h"

namespace misc {

    class Permutations: public CppUnitMini::TestCase
    {
        static const size_t c_nMax = 1024;

        template <typename Gen>
        void test_with( Gen& gen, size_t nLen )
        {
            unsigned int arr[c_nMax];
            for ( size_t nPass = 0; nPass < 10; ++nPass ) {
                for ( size_t i = 0; i < c_nMax; ++i )
                    arr[i] = 0;

                do {
                    typename Gen::integer_type i = gen;
                    ++arr[ i ];
                } while ( gen.next() );

                for ( size_t i = 0; i < nLen; ++i )
                    CPPUNIT_CHECK_EX( arr[i] == 1, "arr[" << i << "]=" << arr[i] );
                for ( size_t i = nLen; i < c_nMax; ++i )
                    CPPUNIT_CHECK_EX( arr[i] == 0, "arr[" << i << "]=" << arr[i] );

                gen.reset();
            }
        }

        template <typename Gen>
        void test()
        {
            for ( size_t nLen = 2; nLen <= c_nMax; ++nLen ) {
                Gen gen( nLen );
                test_with( gen, nLen );
            }
        }

        template <typename Gen>
        void test2()
        {
            for ( size_t nLen = 2; nLen <= c_nMax; nLen *= 2 ) {
                Gen gen( nLen );
                test_with( gen, nLen );
            }
        }

        void test_random_permutation()
        {
            test< cds::opt::v::random_permutation<> >();
        }

        void test_random2_permutation()
        {
            test2< cds::opt::v::random2_permutation<> >();
        }

        void test_random_shuffle_permutation()
        {
            test< cds::opt::v::random_shuffle_permutation<> >();
        }

    public:
        CPPUNIT_TEST_SUITE(Permutations)
            CPPUNIT_TEST( test_random_permutation )
            CPPUNIT_TEST( test_random2_permutation )
            CPPUNIT_TEST( test_random_shuffle_permutation )
        CPPUNIT_TEST_SUITE_END()
    };

} // namespace misc

CPPUNIT_TEST_SUITE_REGISTRATION(misc::Permutations);
