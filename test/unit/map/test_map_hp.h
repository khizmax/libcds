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

#ifndef CDSUNIT_MAP_TEST_MAP_HP_H
#define CDSUNIT_MAP_TEST_MAP_HP_H

#include "test_map.h"

namespace cds_test {

    class container_map_hp: public container_map
    {
        typedef container_map base_class;

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

            std::vector< value_type > arrVals;
            for ( size_t i = 0; i < kkSize; ++i ) {
                value_type val;
                val.nVal = static_cast<int>( i );
                val.strVal = std::to_string( i );
                arrVals.push_back( val );
            }

            for ( auto const& i : arrKeys )
                ASSERT_TRUE( m.insert( i ) );
            ASSERT_FALSE( m.empty() );
            ASSERT_CONTAINER_SIZE( m, kkSize );

            // iterators
            size_t nCount = 0;
            for ( auto it = m.begin(); it != m.end(); ++it ) {
                EXPECT_EQ( it->second.nVal, 0 );
                it->second.nVal = it->first.nKey * 2;
                ++nCount;
            }
            EXPECT_EQ( nCount, kkSize );

            nCount = 0;
            for ( auto it = m.cbegin(); it != m.cend(); ++it ) {
                EXPECT_EQ( it->second.nVal, it->first.nKey * 2 );
                ++nCount;
            }
            EXPECT_EQ( nCount, kkSize );

            // get/extract
            typedef typename Map::guarded_ptr guarded_ptr;
            guarded_ptr gp;

            for ( auto const& i : arrKeys ) {
                value_type const& val = arrVals.at( i.nKey );

                gp = m.get( i.nKey );
                ASSERT_FALSE( !gp );
                ASSERT_EQ( gp->first.nKey, i.nKey );
                gp.release();
                gp = m.get( i );
                ASSERT_FALSE( !gp );
                ASSERT_EQ( gp->first.nKey, i.nKey );
                gp.release();
                gp = m.get_with( other_item( i.nKey ), other_less());
                ASSERT_FALSE( !gp );
                ASSERT_EQ( gp->first.nKey, i.nKey );

                switch ( i.nKey % 4 ) {
                case 0:
                    gp = m.extract( i.nKey );
                    break;
                case 1:
                    gp = m.extract( i );
                    break;
                case 2:
                    gp = m.extract( val.strVal );
                    break;
                case 3:
                    gp = m.extract_with( other_item( i.nKey ), other_less());
                    break;
                }
                ASSERT_FALSE( !gp );
                ASSERT_EQ( gp->first.nKey, i.nKey );
                gp.release();

                gp = m.get( i.nKey );
                ASSERT_TRUE( !gp );
                gp = m.get( i );
                ASSERT_TRUE( !gp );
                gp = m.get_with( other_item( i.nKey ), other_less() );
                ASSERT_TRUE( !gp );
            }
            ASSERT_TRUE( m.empty() );
            ASSERT_CONTAINER_SIZE( m, 0 );
        }
    };

} // namespace cds_test

#endif // #ifndef CDSUNIT_MAP_TEST_MAP_H
