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

#ifndef CDSUNIT_MAP_TEST_SKIPLIST_HP_H
#define CDSUNIT_MAP_TEST_SKIPLIST_HP_H

#include "test_map_hp.h"

namespace cds_test {

    class skiplist_map_hp: public container_map_hp
    {
        typedef container_map_hp base_class;

    protected:
        template <class Map>
        void test( Map& m )
        {
            // Precondition: map is empty
            // Postcondition: map is empty

            base_class::test( m );

            ASSERT_TRUE( m.empty());
            ASSERT_CONTAINER_SIZE( m, 0 );

            typedef typename Map::value_type map_pair;
            size_t const kkSize = base_class::kSize;

            std::vector<key_type> arrKeys;
            for ( int i = 0; i < static_cast<int>(kkSize); ++i )
                arrKeys.push_back( key_type( i ));
            shuffle( arrKeys.begin(), arrKeys.end());

            for ( auto const& i : arrKeys )
                ASSERT_TRUE( m.insert( i ) );
            ASSERT_FALSE( m.empty() );
            ASSERT_CONTAINER_SIZE( m, kkSize );

            // extract_min
            typedef typename Map::guarded_ptr guarded_ptr;
            guarded_ptr gp;

            int nKey = -1;
            size_t nCount = 0;
            while ( !m.empty() ) {
                gp = m.extract_min();
                ASSERT_FALSE( !gp );
                EXPECT_EQ( gp->first.nKey, nKey + 1 );
                nKey = gp->first.nKey;
                ++nCount;
            }
            ASSERT_TRUE( m.empty() );
            ASSERT_CONTAINER_SIZE( m, 0 );
            EXPECT_EQ( nCount, kkSize );

            // extract_max
            for ( auto const& i : arrKeys )
                ASSERT_TRUE( m.insert( i ) );
            ASSERT_FALSE( m.empty() );
            ASSERT_CONTAINER_SIZE( m, kkSize );

            nKey = kkSize;
            nCount = 0;
            while ( !m.empty() ) {
                gp = m.extract_max();
                ASSERT_FALSE( !gp );
                EXPECT_EQ( gp->first.nKey, nKey - 1 );
                nKey = gp->first.nKey;
                ++nCount;
            }
            ASSERT_TRUE( m.empty() );
            ASSERT_CONTAINER_SIZE( m, 0 );
            EXPECT_EQ( nCount, kkSize );
        }
    };

} // namespace cds_test

#endif // #ifndef CDSUNIT_MAP_TEST_SKIPLIST_HP_H
