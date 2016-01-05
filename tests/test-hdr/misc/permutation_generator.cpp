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
