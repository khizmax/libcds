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

#ifndef CDSUNIT_TREE_TEST_INTRUSIVE_TREE_HP_H
#define CDSUNIT_TREE_TEST_INTRUSIVE_TREE_HP_H

#include "test_intrusive_tree.h"

// forward declaration
namespace cds { namespace intrusive {}}

namespace cds_test {

    namespace ci = cds::intrusive;

    class intrusive_tree_hp: public intrusive_tree
    {
        typedef intrusive_tree base_class;

    protected:

        template <class Tree>
        void test( Tree& t )
        {
            // Precondition: tree is empty
            // Postcondition: tree is empty

            base_class::test( t );

            ASSERT_TRUE( t.empty() );
            ASSERT_CONTAINER_SIZE( t, 0 );

            typedef typename Tree::value_type value_type;

            std::vector< value_type > data;
            std::vector< size_t> indices;
            data.reserve( kSize );
            indices.reserve( kSize );
            for ( size_t key = 0; key < kSize; ++key ) {
                data.push_back( value_type( static_cast<int>(key) ) );
                indices.push_back( key );
            }
            shuffle( indices.begin(), indices.end() );

            typename Tree::guarded_ptr gp;

            // get/extract from empty tree
            for ( auto idx : indices ) {
                auto& i = data[idx];

                gp = t.get( i );
                ASSERT_TRUE( !gp );
                gp = t.get( i.key() );
                ASSERT_TRUE( !gp );
                gp = t.get_with( other_item( i.key()), other_less());
                ASSERT_TRUE( !gp );

                gp = t.extract( i );
                ASSERT_TRUE( !gp );
                gp = t.extract( i.key());
                ASSERT_TRUE( !gp );
                gp = t.extract_with( other_item( i.key()), other_less());
                ASSERT_TRUE( !gp );

                gp = t.extract_min();
                ASSERT_TRUE( !gp );
                gp = t.extract_max();
                ASSERT_TRUE( !gp );
            }

            // fill tree
            for ( auto idx : indices ) {
                auto& i = data[idx];
                i.nDisposeCount = 0;
                ASSERT_TRUE( t.insert( i ) );
            }

            // get/extract
            for ( auto idx : indices ) {
                auto& i = data[idx];

                EXPECT_EQ( i.nFindCount, 0 );
                gp = t.get( i );
                ASSERT_FALSE( !gp );
                ++gp->nFindCount;
                EXPECT_EQ( i.nFindCount, 1 );

                gp = t.get( i.key() );
                ASSERT_FALSE( !gp );
                ++gp->nFindCount;
                EXPECT_EQ( i.nFindCount, 2 );

                gp = t.get_with( other_item( i.key()), other_less());
                ASSERT_FALSE( !gp );
                ++gp->nFindCount;
                EXPECT_EQ( i.nFindCount, 3 );

                EXPECT_EQ( i.nEraseCount, 0 );
                switch ( i.key() % 3 ) {
                case 0:
                    gp = t.extract( i.key());
                    break;
                case 1:
                    gp = t.extract( i );
                    break;
                case 2:
                    gp = t.extract_with( other_item( i.key() ), other_less() );
                    break;
                }
                ASSERT_FALSE( !gp );
                ++gp->nEraseCount;
                EXPECT_EQ( i.nEraseCount, 1 );

                gp = t.extract( i );
                ASSERT_TRUE( !gp );
                gp = t.extract( i.key() );
                ASSERT_TRUE( !gp );
                gp = t.extract_with( other_item( i.key() ), other_less() );
                ASSERT_TRUE( !gp );
            }

            gp.release();

            ASSERT_TRUE( t.empty() );
            ASSERT_CONTAINER_SIZE( t, 0 );

            // Force retiring cycle
            Tree::gc::force_dispose();
            for ( auto& i : data ) {
                EXPECT_EQ( i.nDisposeCount, 1 );
            }

            // extract_min
            for ( auto idx : indices ) {
                auto& i = data[idx];
                i.nDisposeCount = 0;
                ASSERT_TRUE( t.insert( i ) );
            }

            size_t nCount = 0;
            int nKey = -1;
            while ( !t.empty() ) {
                gp = t.extract_min();
                ASSERT_FALSE( !gp );
                EXPECT_EQ( gp->key(), nKey + 1 );
                ++nCount;
                nKey = gp->key();
            }
            gp.release();
            ASSERT_TRUE( t.empty() );
            ASSERT_CONTAINER_SIZE( t, 0 );
            EXPECT_EQ( nCount, data.size() );

            // Force retiring cycle
            Tree::gc::force_dispose();
            for ( auto& i : data ) {
                EXPECT_EQ( i.nDisposeCount, 1 );
            }

            // extract_max
            for ( auto idx : indices ) {
                auto& i = data[idx];
                i.nDisposeCount = 0;
                ASSERT_TRUE( t.insert( i ) );
            }

            nCount = 0;
            nKey = static_cast<int>( data.size());
            while ( !t.empty() ) {
                gp = t.extract_max();
                ASSERT_FALSE( !gp );
                EXPECT_EQ( gp->key(), nKey - 1 );
                ++nCount;
                nKey = gp->key();
            }
            gp.release();
            ASSERT_TRUE( t.empty() );
            ASSERT_CONTAINER_SIZE( t, 0 );
            EXPECT_EQ( nCount, data.size() );

            // Force retiring cycle
            Tree::gc::force_dispose();
            for ( auto& i : data ) {
                EXPECT_EQ( i.nDisposeCount, 1 );
            }

        }
    };

} // namespace cds_test

#endif // #ifndef CDSUNIT_TREE_TEST_INTRUSIVE_TREE_HP_H
