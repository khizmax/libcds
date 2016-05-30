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

#ifndef CDSUNIT_STRIPED_SET_TEST_INTRUSIVE_SET_H
#define CDSUNIT_STRIPED_SET_TEST_INTRUSIVE_SET_H

#include <cds_test/check_size.h>
#include <cds_test/fixture.h>

#include <cds/opt/hash.h>

// forward declaration
namespace cds { namespace intrusive {}}

namespace cds_test {

    namespace ci = cds::intrusive;
    namespace co = cds::opt;

    class intrusive_set: public fixture
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
            typedef Node member_type;

            int nKey;
            int nVal;

            Node hMember;

            stat s;

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

        struct hash_int {
            size_t operator()( int i ) const
            {
                return co::v::hash<int>()( i );
            }
            template <typename Item>
            size_t operator()( const Item& i ) const
            {
                return (*this)( i.key() );
            }
        };
        typedef hash_int hash1;

        struct hash2: private hash1
        {
            typedef hash1 base_class;

            size_t operator()( int i ) const
            {
                size_t h = ~(base_class::operator()( i ));
                return ~h + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            template <typename Item>
            size_t operator()( const Item& i ) const
            {
                size_t h = ~(base_class::operator()( i ));
                return ~h + 0x9e3779b9 + (h << 6) + (h >> 2);
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
            bool operator ()(const T& v1, const T& v2 ) const
            {
                return v1.key() < v2.key();
            }

            bool operator ()(const T& v1, int v2 ) const
            {
                return v1.key() < v2;
            }

            bool operator ()(int v1, const T& v2 ) const
            {
                return v1 < v2.key();
            }

            bool operator()( int v1, int v2 ) const
            {
                return v1 < v2;
            }
        };

        template <typename T>
        struct cmp {
            int operator ()(const T& v1, const T& v2 ) const
            {
                if ( v1.key() < v2.key() )
                    return -1;
                return v1.key() > v2.key() ? 1 : 0;
            }

            template <typename Q>
            int operator ()(const T& v1, const Q& v2 ) const
            {
                if ( v1.key() < v2 )
                    return -1;
                return v1.key() > v2 ? 1 : 0;
            }

            template <typename Q>
            int operator ()(const Q& v1, const T& v2 ) const
            {
                if ( v1 < v2.key() )
                    return -1;
                return v1 > v2.key() ? 1 : 0;
            }
        };

        template <typename T>
        struct equal_to {
            int operator ()( const T& v1, const T& v2 ) const
            {
                return v1.key() == v2.key();
            }

            int operator ()( const T& v1, int v2 ) const
            {
                return v1.key() == v2;
            }

            int operator ()( int v1, const T& v2 ) const
            {
                return v1 == v2.key();
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
            template <typename T>
            bool operator()( other_item const& lhs, T const& rhs ) const
            {
                return lhs.key() < rhs.key();
            }

            template <typename T>
            bool operator()( T const& lhs, other_item const& rhs ) const
            {
                return lhs.key() < rhs.key();
            }

            bool operator()( other_item const& lhs, int rhs ) const
            {
                return lhs.key() < rhs;
            }

            bool operator()( int lhs, other_item const& rhs ) const
            {
                return lhs < rhs.key();
            }
        };

        struct other_equal_to {
            template <typename Q, typename T>
            bool operator()( Q const& lhs, T const& rhs ) const
            {
                return lhs.key() == rhs.key();
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
        template <typename Set>
        void test( Set& s, std::vector< typename Set::value_type >& data )
        {
            test_< true >( s, data );
        }

        template <bool Sorted, class Set>
        void test_( Set& s, std::vector< typename Set::value_type >& data )
        {
            // Precondition: set is empty
            // Postcondition: set is empty

            ASSERT_TRUE( s.empty() );
            ASSERT_CONTAINER_SIZE( s, 0 );

            typedef typename Set::value_type value_type;
            typedef typename std::conditional< Sorted, other_less, other_equal_to >::type other_predicate;
            size_t const nSetSize = kSize;

            std::vector< size_t> indices;
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

                ASSERT_FALSE( s.contains( i.nKey ));
                ASSERT_FALSE( s.contains( i ));
                ASSERT_FALSE( s.contains( other_item( i.key()), other_predicate()));
                ASSERT_FALSE( s.find( i.nKey, []( value_type&, int ) {} ));
                ASSERT_FALSE( s.find_with( other_item( i.key()), other_predicate(), []( value_type&, other_item const& ) {} ));

                std::pair<bool, bool> updResult;

                updResult = s.update( i, []( bool bNew, value_type&, value_type& )
                {
                    ASSERT_TRUE( false );
                }, false );
                EXPECT_FALSE( updResult.first );
                EXPECT_FALSE( updResult.second );

                switch ( i.key() % 3 ) {
                case 0:
                    ASSERT_TRUE( s.insert( i ));
                    ASSERT_FALSE( s.insert( i ));
                    updResult = s.update( i, []( bool bNew, value_type& val, value_type& arg) 
                        {
                            EXPECT_FALSE( bNew );
                            EXPECT_EQ( &val, &arg );
                        }, false );
                    EXPECT_TRUE( updResult.first );
                    EXPECT_FALSE( updResult.second );
                    break;
                case 1:
                    EXPECT_EQ( i.nUpdateNewCount, 0 );
                    ASSERT_TRUE( s.insert( i, []( value_type& v ) { ++v.nUpdateNewCount;} ));
                    EXPECT_EQ( i.nUpdateNewCount, 1 );
                    ASSERT_FALSE( s.insert( i, []( value_type& v ) { ++v.nUpdateNewCount;} ) );
                    EXPECT_EQ( i.nUpdateNewCount, 1 );
                    i.nUpdateNewCount = 0;
                    break;
                case 2:
                    updResult = s.update( i, []( bool bNew, value_type& val, value_type& arg )
                    {
                        EXPECT_TRUE( bNew );
                        EXPECT_EQ( &val, &arg );
                    });
                    EXPECT_TRUE( updResult.first );
                    EXPECT_TRUE( updResult.second );
                    break;
                }

                ASSERT_TRUE( s.contains( i.nKey ) );
                ASSERT_TRUE( s.contains( i ) );
                ASSERT_TRUE( s.contains( other_item( i.key() ), other_predicate()));
                EXPECT_EQ( i.nFindCount, 0 );
                ASSERT_TRUE( s.find( i.nKey, []( value_type& v, int ) { ++v.nFindCount; } ));
                EXPECT_EQ( i.nFindCount, 1 );
                ASSERT_TRUE( s.find_with( other_item( i.key() ), other_predicate(), []( value_type& v, other_item const& ) { ++v.nFindCount; } ));
                EXPECT_EQ( i.nFindCount, 2 );
            }
            ASSERT_FALSE( s.empty() );
            ASSERT_CONTAINER_SIZE( s, nSetSize );

            std::for_each( data.begin(), data.end(), []( value_type& v ) { v.clear_stat(); });

            // erase
            shuffle( indices.begin(), indices.end() );
            for ( auto idx : indices ) {
                auto& i = data[ idx ];

                ASSERT_TRUE( s.contains( i.nKey ) );
                ASSERT_TRUE( s.contains( i ) );
                ASSERT_TRUE( s.contains( other_item( i.key() ), other_predicate() ) );
                EXPECT_EQ( i.nFindCount, 0 );
                ASSERT_TRUE( s.find( i.nKey, []( value_type& v, int ) { ++v.nFindCount; } ) );
                EXPECT_EQ( i.nFindCount, 1 );
                ASSERT_TRUE( s.find_with( other_item( i.key() ), other_predicate(), []( value_type& v, other_item const& ) { ++v.nFindCount; } ) );
                EXPECT_EQ( i.nFindCount, 2 );

                value_type v( i );
                switch ( i.key() % 6 ) {
                case 0:
                    ASSERT_FALSE( s.unlink( v ));
                    ASSERT_TRUE( s.unlink( i ));
                    ASSERT_FALSE( s.unlink( i ) );
                    break;
                case 1:
                    ASSERT_TRUE( s.erase( i.key()));
                    ASSERT_FALSE( s.erase( i.key() ) );
                    break;
                case 2:
                    ASSERT_TRUE( s.erase( v ));
                    ASSERT_FALSE( s.erase( v ) );
                    break;
                case 3:
                    ASSERT_TRUE( s.erase_with( other_item( i.key()), other_predicate()));
                    ASSERT_FALSE( s.erase_with( other_item( i.key() ), other_predicate() ) );
                    break;
                case 4:
                    EXPECT_EQ( i.nEraseCount, 0 );
                    ASSERT_TRUE( s.erase( v, []( value_type& val ) { ++val.nEraseCount; } ));
                    EXPECT_EQ( i.nEraseCount, 1 );
                    ASSERT_FALSE( s.erase( v, []( value_type& val ) { ++val.nEraseCount; } ));
                    EXPECT_EQ( i.nEraseCount, 1 );
                    break;
                case 5:
                    EXPECT_EQ( i.nEraseCount, 0 );
                    ASSERT_TRUE( s.erase_with( other_item( i.key() ), other_predicate(), []( value_type& val ) { ++val.nEraseCount; } ));
                    EXPECT_EQ( i.nEraseCount, 1 );
                    ASSERT_FALSE( s.erase_with( other_item( i.key() ), other_predicate(), []( value_type& val ) { ++val.nEraseCount; } ));
                    EXPECT_EQ( i.nEraseCount, 1 );
                    break;
                }

                ASSERT_FALSE( s.contains( i.nKey ));
                ASSERT_FALSE( s.contains( i ));
                ASSERT_FALSE( s.contains( other_item( i.key()), other_predicate()));
                ASSERT_FALSE( s.find( i.nKey, []( value_type&, int ) {} ));
                ASSERT_FALSE( s.find_with( other_item( i.key()), other_predicate(), []( value_type&, other_item const& ) {} ));
            }
            ASSERT_TRUE( s.empty() );
            ASSERT_CONTAINER_SIZE( s, 0 );

            // clear
            for ( auto& i : data ) {
                i.clear_stat();
                ASSERT_TRUE( s.insert( i ));
            }
            ASSERT_FALSE( s.empty() );
            ASSERT_CONTAINER_SIZE( s, nSetSize );

            s.clear();

            ASSERT_TRUE( s.empty() );
            ASSERT_CONTAINER_SIZE( s, 0 );

            // clear_and_dispose
            for ( auto& i : data ) {
                i.clear_stat();
                ASSERT_TRUE( s.insert( i ) );
            }
            ASSERT_FALSE( s.empty() );
            ASSERT_CONTAINER_SIZE( s, nSetSize );

            s.clear_and_dispose( mock_disposer() );

            ASSERT_TRUE( s.empty() );
            ASSERT_CONTAINER_SIZE( s, 0 );
            for ( auto& i : data ) {
                EXPECT_EQ( i.nDisposeCount, 1 );
            }

        }
    };

} // namespace cds_test

#endif // #ifndef CDSUNIT_STRIPED_SET_TEST_INTRUSIVE_SET_H
