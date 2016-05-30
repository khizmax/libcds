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

#ifndef CDSUNIT_TREE_TEST_INTRUSIVE_TREE_H
#define CDSUNIT_TREE_TEST_INTRUSIVE_TREE_H

#include <cds_test/check_size.h>
#include <cds_test/fixture.h>

#include <cds/opt/hash.h>
// forward declaration
namespace cds { namespace intrusive {}}

namespace cds_test {

    namespace ci = cds::intrusive;

    class intrusive_tree: public fixture
    {
    public:
        static size_t const kSize = 1000;

        struct stat
        {
            unsigned int nDisposeCount  ;   // count of disposer calling
            unsigned int nFindCount     ;   // count of find-functor calling
            unsigned int nUpdateNewCount;
            unsigned int nUpdateCount;
            mutable unsigned int nEraseCount;

            stat()
            {
                clear_stat();
            }

            void clear_stat()
            {
                memset( this, 0, sizeof( *this ) );
            }
        };

        typedef int key_type;

        template <typename Node>
        struct base_int_item
            : public Node
            , public stat

        {
            int nKey;
            int nVal;

            base_int_item()
            {}

            explicit base_int_item( int key )
                : nKey( key )
                , nVal( key )
            {}

            base_int_item(int key, int val)
                : nKey( key )
                , nVal(val)
            {}

            base_int_item( base_int_item const& v )
                : Node()
                , stat()
                , nKey( v.nKey )
                , nVal( v.nVal )
            {}

            int key() const
            {
                return nKey;
            }
        };

        template <typename Node>
        struct member_int_item: public stat
        {
            int nKey;
            int nVal;

            Node hMember;

            stat t;

            member_int_item()
            {}

            explicit member_int_item( int key )
                : nKey( key )
                , nVal( key )
            {}

            member_int_item(int key, int val)
                : nKey( key )
                , nVal(val)
            {}

            member_int_item(member_int_item const& v )
                : stat()
                , nKey( v.nKey )
                , nVal( v.nVal )
            {}

            int key() const
            {
                return nKey;
            }
        };

        struct key_extractor
        {
            template <typename T>
            void operator()( key_type& key, T const& val ) const
            {
                key = val.key();
            }
        };

        struct simple_item_counter {
            size_t  m_nCount;

            simple_item_counter()
                : m_nCount(0)
            {}

            size_t operator ++()
            {
                return ++m_nCount;
            }

            size_t operator --()
            {
                return --m_nCount;
            }

            void reset()
            {
                m_nCount = 0;
            }

            operator size_t() const
            {
                return m_nCount;
            }
        };


        template <typename T>
        struct less
        {
            bool operator ()(T const& v1, T const& v2 ) const
            {
                return v1.key() < v2.key();
            }

            bool operator()( T const& lhs, int rhs ) const
            {
                return lhs.key() < rhs;
            }

            bool operator()( int lhs, T const& rhs ) const
            {
                return lhs < rhs.key();
            }

            bool operator()( int lhs, int rhs ) const
            {
                return lhs < rhs;
            }
        };

        template <typename T>
        struct cmp 
        {
            int operator ()(T const& v1, T const& v2 ) const
            {
                if ( v1.key() < v2.key() )
                    return -1;
                return v1.key() > v2.key() ? 1 : 0;
            }

            int operator()( T const& lhs, int rhs ) const
            {
                if ( lhs.key() < rhs )
                    return -1;
                return lhs.key() > rhs ? 1 : 0;
            }

            int operator()( int lhs, T const& rhs ) const
            {
                if ( lhs < rhs.key() )
                    return -1;
                return lhs > rhs.key() ? 1 : 0;
            }

            int operator()( int lhs, int rhs ) const
            {
                if ( lhs < rhs )
                    return -1;
                return lhs > rhs ? 1 : 0;
            }
        };

        struct other_item {
            int nKey;

            explicit other_item( int k )
                : nKey( k )
            {}

            int key() const
            {
                return nKey;
            }
        };

        struct other_less {
            template <typename Q, typename T>
            bool operator()( Q const& lhs, T const& rhs ) const
            {
                return lhs.key() < rhs.key();
            }

            template <typename Q>
            bool operator()( Q const& lhs, int rhs ) const
            {
                return lhs.key() < rhs;
            }

            template <typename T>
            bool operator()( int lhs, T const& rhs ) const
            {
                return lhs < rhs.key();
            }
        };

        struct mock_disposer
        {
            template <typename T>
            void operator ()( T * p )
            {
                ++p->nDisposeCount;
            }
        };

    protected:
        template <class Tree>
        void test( Tree& t )
        {
            // Precondition: tree is empty
            // Postcondition: tree is empty

            ASSERT_TRUE( t.empty() );
            ASSERT_CONTAINER_SIZE( t, 0 );
            size_t const nTreeSize = kSize;

            typedef typename Tree::value_type value_type;

            std::vector< value_type > data;
            std::vector< size_t > indices;
            data.reserve( kSize );
            indices.reserve( kSize );
            for ( size_t key = 0; key < kSize; ++key ) {
                data.push_back( value_type( static_cast<int>( key )));
                indices.push_back( key );
            }
            shuffle( indices.begin(), indices.end() );

            // insert/find
            for ( auto idx : indices ) {
                auto& i = data[ idx ];

                ASSERT_FALSE( t.contains( i.nKey ));
                ASSERT_FALSE( t.contains( i ));
                ASSERT_FALSE( t.contains( other_item( i.key()), other_less()));
                ASSERT_FALSE( t.find( i.nKey, []( value_type&, int ) {} ));
                ASSERT_FALSE( t.find_with( other_item( i.key()), other_less(), []( value_type&, other_item const& ) {} ));

                std::pair<bool, bool> updResult;

                updResult = t.update( i, []( bool bNew, value_type&, value_type& )
                {
                    ASSERT_TRUE( false );
                }, false );
                EXPECT_FALSE( updResult.first );
                EXPECT_FALSE( updResult.second );

                switch ( i.key() % 3 ) {
                case 0:
                    ASSERT_TRUE( t.insert( i ));
                    ASSERT_FALSE( t.insert( i ));
                    updResult = t.update( i, []( bool bNew, value_type& val, value_type& arg) 
                        {
                            EXPECT_FALSE( bNew );
                            EXPECT_EQ( &val, &arg );
                        }, false );
                    EXPECT_TRUE( updResult.first );
                    EXPECT_FALSE( updResult.second );
                    break;
                case 1:
                    EXPECT_EQ( i.nUpdateNewCount, 0 );
                    ASSERT_TRUE( t.insert( i, []( value_type& v ) { ++v.nUpdateNewCount;} ));
                    EXPECT_EQ( i.nUpdateNewCount, 1 );
                    ASSERT_FALSE( t.insert( i, []( value_type& v ) { ++v.nUpdateNewCount;} ) );
                    EXPECT_EQ( i.nUpdateNewCount, 1 );
                    i.nUpdateNewCount = 0;
                    break;
                case 2:
                    updResult = t.update( i, []( bool bNew, value_type& val, value_type& arg )
                    {
                        EXPECT_TRUE( false );
                    }, false );
                    EXPECT_FALSE( updResult.first );
                    EXPECT_FALSE( updResult.second );

                    EXPECT_EQ( i.nUpdateNewCount, 0 );
                    updResult = t.update( i, []( bool bNew, value_type& val, value_type& arg )
                    {
                        EXPECT_TRUE( bNew );
                        EXPECT_EQ( &val, &arg );
                        ++val.nUpdateNewCount;
                    });
                    EXPECT_TRUE( updResult.first );
                    EXPECT_TRUE( updResult.second );
                    EXPECT_EQ( i.nUpdateNewCount, 1 );
                    i.nUpdateNewCount = 0;

                    EXPECT_EQ( i.nUpdateCount, 0 );
                    updResult = t.update( i, []( bool bNew, value_type& val, value_type& arg )
                    {
                        EXPECT_FALSE( bNew );
                        EXPECT_EQ( &val, &arg );
                        ++val.nUpdateCount;
                    }, false );
                    EXPECT_TRUE( updResult.first );
                    EXPECT_FALSE( updResult.second );
                    EXPECT_EQ( i.nUpdateCount, 1 );
                    i.nUpdateCount = 0;

                    break;
                }

                ASSERT_TRUE( t.contains( i.nKey ) );
                ASSERT_TRUE( t.contains( i ) );
                ASSERT_TRUE( t.contains( other_item( i.key() ), other_less()));
                EXPECT_EQ( i.nFindCount, 0 );
                ASSERT_TRUE( t.find( i.nKey, []( value_type& v, int ) { ++v.nFindCount; } ));
                EXPECT_EQ( i.nFindCount, 1 );
                ASSERT_TRUE( t.find_with( other_item( i.key() ), other_less(), []( value_type& v, other_item const& ) { ++v.nFindCount; } ));
                EXPECT_EQ( i.nFindCount, 2 );
                ASSERT_TRUE( t.find( i, []( value_type& v, value_type& ) { ++v.nFindCount; } ) );
                EXPECT_EQ( i.nFindCount, 3 );
            }
            ASSERT_FALSE( t.empty() );
            ASSERT_CONTAINER_SIZE( t, nTreeSize );

            std::for_each( data.begin(), data.end(), []( value_type& v ) { v.clear_stat(); });

            // erase
            shuffle( indices.begin(), indices.end() );
            for ( auto idx : indices ) {
                auto& i = data[ idx ];

                ASSERT_TRUE( t.contains( i.nKey ) );
                ASSERT_TRUE( t.contains( i ) );
                ASSERT_TRUE( t.contains( other_item( i.key() ), other_less() ) );
                EXPECT_EQ( i.nFindCount, 0 );
                ASSERT_TRUE( t.find( i.nKey, []( value_type& v, int ) { ++v.nFindCount; } ) );
                EXPECT_EQ( i.nFindCount, 1 );
                ASSERT_TRUE( t.find_with( other_item( i.key() ), other_less(), []( value_type& v, other_item const& ) { ++v.nFindCount; } ) );
                EXPECT_EQ( i.nFindCount, 2 );

                value_type v( i );
                switch ( i.key() % 6 ) {
                case 0:
                    ASSERT_FALSE( t.unlink( v ));
                    ASSERT_TRUE( t.unlink( i ));
                    ASSERT_FALSE( t.unlink( i ) );
                    break;
                case 1:
                    ASSERT_TRUE( t.erase( i.key()));
                    ASSERT_FALSE( t.erase( i.key() ) );
                    break;
                case 2:
                    ASSERT_TRUE( t.erase( v ));
                    ASSERT_FALSE( t.erase( v ) );
                    break;
                case 3:
                    ASSERT_TRUE( t.erase_with( other_item( i.key()), other_less()));
                    ASSERT_FALSE( t.erase_with( other_item( i.key() ), other_less() ) );
                    break;
                case 4:
                    EXPECT_EQ( i.nEraseCount, 0 );
                    ASSERT_TRUE( t.erase( v, []( value_type& val ) { ++val.nEraseCount; } ));
                    EXPECT_EQ( i.nEraseCount, 1 );
                    ASSERT_FALSE( t.erase( v, []( value_type& val ) { ++val.nEraseCount; } ));
                    EXPECT_EQ( i.nEraseCount, 1 );
                    break;
                case 5:
                    EXPECT_EQ( i.nEraseCount, 0 );
                    ASSERT_TRUE( t.erase_with( other_item( i.key() ), other_less(), []( value_type& val ) { ++val.nEraseCount; } ));
                    EXPECT_EQ( i.nEraseCount, 1 );
                    ASSERT_FALSE( t.erase_with( other_item( i.key() ), other_less(), []( value_type& val ) { ++val.nEraseCount; } ));
                    EXPECT_EQ( i.nEraseCount, 1 );
                    break;
                }

                ASSERT_FALSE( t.contains( i.nKey ));
                ASSERT_FALSE( t.contains( i ));
                ASSERT_FALSE( t.contains( other_item( i.key()), other_less()));
                ASSERT_FALSE( t.find( i.nKey, []( value_type&, int ) {} ));
                ASSERT_FALSE( t.find( i,      []( value_type&, value_type const& ) {} ));
                ASSERT_FALSE( t.find_with( other_item( i.key()), other_less(), []( value_type&, other_item const& ) {} ));
            }
            ASSERT_TRUE( t.empty() );
            ASSERT_CONTAINER_SIZE( t, 0 );

            // Force retiring cycle
            Tree::gc::force_dispose();
            for ( auto& i : data ) {
                EXPECT_EQ( i.nDisposeCount, 1 );
            }

            // clear
            for ( auto idx : indices ) {
                auto& i = data[idx];
                i.clear_stat();
                ASSERT_TRUE( t.insert( i ));
            }
            ASSERT_FALSE( t.empty() );
            ASSERT_CONTAINER_SIZE( t, nTreeSize );

            // clear test
            t.clear();

            ASSERT_TRUE( t.empty());
            ASSERT_CONTAINER_SIZE( t, 0 );

            // Force retiring cycle
            Tree::gc::force_dispose();
            for ( auto& i : data ) {
                EXPECT_EQ( i.nDisposeCount, 1 );
            }
        }
    };

} // namespace cds_test

#endif // #ifndef CDSUNIT_TREE_TEST_INTRUSIVE_TREE_H
