#ifndef CDSUNIT_SET_TEST_INTRUSIVE_NC_FELDMAN_HASHSET_H
#define CDSUNIT_SET_TEST_INTRUSIVE_NC_FELDMAN_HASHSET_H

#include <cds_test/check_size.h>
#include <cds_test/fixture.h>

#include <cds/opt/hash.h>
#include <functional>   // ref

// forward declaration
namespace cds { namespace intrusive {}}
namespace ci = cds::intrusive;

namespace cds_test {
    class intrusive_nc_feldman_hashset: public fixture
    {
    public:
        struct stat
        {
            unsigned int nDisposeCount;   // count of disposer calling
            unsigned int nFindCount;   // count of find-functor calling
            unsigned int nInsertCount;
            mutable unsigned int nEraseCount;

            stat()
            {
                clear_stat();
            }

            void clear_stat()
            {
                memset( this, 0, sizeof( *this ));
            }
        };

        struct int_item: public stat
        {
            int nKey;
            int nVal;

            int_item()
            {}

            explicit int_item( int key )
                : nKey( key )
                , nVal( key )
            {}

            int_item( int key, int val )
                : nKey( key )
                , nVal( val )
            {}

            int_item( int_item const& v )
                : stat()
                , nKey( v.nKey )
                , nVal( v.nVal )
            {}

            int key() const
            {
                return nKey;
            }
            
        };

        struct equal {
            bool operator()( int_item const &lhs, int_item const& rhs )
            {
                return  lhs.nVal == rhs.nVal;
            }
        };

        struct less {
            bool operator()( int_item const &lhs, int_item const& rhs )
            {
                return  lhs.nVal < rhs.nVal;
            }
        };

        struct hash_accessor {
            int operator()( int_item const& v ) const
            {
                return v.key() % 12;
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

        struct cmp {
            int operator ()( int lhs, int rhs ) const
            {
                if ( lhs < rhs )
                    return -1;
                return lhs > rhs ? 1 : 0;
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
        template <class Set>
        void test( Set& s )
        {
  
            // Precondition: set is empty
            // Postcondition: set is empty

            ASSERT_TRUE( s.empty());
            ASSERT_CONTAINER_SIZE( s, 0 );

            size_t const nSetSize = std::max( s.head_size() * 2, static_cast<size_t>(100));

            typedef typename Set::value_type value_type;

            std::vector< value_type > data;
            std::vector< size_t> indices;
            data.reserve( nSetSize );
            indices.reserve( nSetSize );
            for ( size_t key = 0; key < nSetSize; ++key ) {
                data.push_back( value_type( static_cast<int>( key )));
                indices.push_back( key );
            }
            std::random_shuffle( indices.begin(), indices.end());


            // insert/find
            for ( auto idx : indices ) {
            
                auto& i = data[ idx ];

                ASSERT_FALSE( s.contains( i ));
                ASSERT_FALSE( s.find( i, []( value_type& ) {} ));
                        
                switch ( i.key() % 2 ) {
                case 0:
                    ASSERT_TRUE( s.insert( i ));
                    ASSERT_FALSE( s.insert( i ));
                    break;
                case 1:
                    EXPECT_EQ( i.nInsertCount, 0u );
                    ASSERT_TRUE( s.insert( i, []( value_type& v ) { ++v.nInsertCount;} ));
                    EXPECT_EQ( i.nInsertCount, 1u );
                    ASSERT_FALSE( s.insert( i, []( value_type& v ) { ++v.nInsertCount;} ));
                    EXPECT_EQ( i.nInsertCount, 1u );
                    i.nInsertCount = 0;
                    break;
                }

                ASSERT_TRUE( s.contains( i ));
                EXPECT_EQ( i.nFindCount, 0u );
                ASSERT_TRUE( s.find( i, []( value_type& v ) { ++v.nFindCount; } ));
                EXPECT_EQ( i.nFindCount, 1u );
            }
            
            ASSERT_FALSE( s.empty());
            ASSERT_CONTAINER_SIZE( s, nSetSize );

            std::for_each( data.begin(), data.end(), []( value_type& v ) { v.clear_stat(); });

            // get_level_statistics
            {
                std::vector< typename Set::level_statistics > level_stat;
                s.get_level_statistics( level_stat );
                EXPECT_GT( level_stat.size(), 0u );
            }
              
            // erase
            std::random_shuffle( indices.begin(), indices.end());
            for ( auto idx : indices ) {
                auto& i = data[ idx ];

                ASSERT_TRUE( s.contains( i ));
                EXPECT_EQ( i.nFindCount, 0u );
                ASSERT_TRUE( s.find( i, []( value_type& v ) { ++v.nFindCount; } ));
                EXPECT_EQ( i.nFindCount, 1u );

                value_type v( i );
                switch ( i.key() % 3 ) {
                case 0:
                    ASSERT_FALSE( s.unlink( v ));
                    ASSERT_TRUE( s.unlink( i ));
                    ASSERT_FALSE( s.unlink( i ));
                    break;
                case 1:
                    ASSERT_TRUE( s.erase( i ));
                    ASSERT_FALSE( s.erase( i ));
                    break;
                case 2:
                    EXPECT_EQ( i.nEraseCount, 0u );
                    ASSERT_TRUE( s.erase( v, []( value_type& val ) { ++val.nEraseCount; } ));
                    EXPECT_EQ( i.nEraseCount, 1u );
                    ASSERT_FALSE( s.erase( v, []( value_type& val ) { ++val.nEraseCount; } ));
                    EXPECT_EQ( i.nEraseCount, 1u );
                    break;
                }

                ASSERT_FALSE( s.contains( i ));
                ASSERT_FALSE( s.find( i, []( value_type const& ) {} ));
            }
            ASSERT_TRUE( s.empty());
            ASSERT_CONTAINER_SIZE( s, 0 );
            
            // clear test
            s.clear();

            ASSERT_TRUE( s.empty());
            ASSERT_CONTAINER_SIZE( s, 0u );
            
        }
    };

} // namespace cds_test

#endif // #ifndef CDSUNIT_SET_TEST_INTRUSIVE_NC_FELDMAN_HASHSET_H
