//$$CDS-header$$

#ifndef CDSTEST_HDR_STRIPED_MAP_H
#define CDSTEST_HDR_STRIPED_MAP_H
#include "size_check.h"

#include "cppunit/cppunit_proxy.h"
#include <cds/os/timer.h>
#include <cds/opt/hash.h>
#include <functional>   // ref

namespace cds { namespace container {}}

namespace map {
    using misc::check_size;

    namespace cc = cds::container;
    namespace co = cds::opt;

    class StripedMapHdrTest: public CppUnitMini::TestCase
    {
    public:
        typedef int key_type;

        struct value_type {
            int m_val;

            value_type()
                : m_val(0)
            {}

            value_type( int n )
                : m_val( n )
            {}

            value_type( value_type&& v )
                : m_val( v.m_val )
            {}

            value_type( value_type const& v )
                : m_val( v.m_val )
            {}

            value_type& operator=( value_type const& v )
            {
                m_val = v.m_val;
                return *this;
            }
        };

        typedef std::pair<key_type const, value_type> pair_type;

        struct less
        {
            bool operator ()(int v1, int v2 ) const
            {
                return v1 < v2;
            }
        };

        struct cmp {
            int operator ()(int v1, int v2 ) const
            {
                if ( v1 < v2 )
                    return -1;
                return v1 > v2 ? 1 : 0;
            }
        };

        struct equal {
            bool operator ()(int v1, int v2 ) const
            {
                return v1 == v2;
            }
        };

        struct hash_int {
            size_t operator()( int i ) const
            {
                return co::v::hash<int>()( i );
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

        template <typename Map>
        struct insert_functor
        {
            typedef typename Map::value_type pair_type;

            // insert ftor
            void operator()( pair_type& item )
            {
                item.second.m_val = item.first * 3;
            }

            // ensure ftor
            void operator()( bool bNew, pair_type& item )
            {
                if ( bNew )
                    item.second.m_val = item.first * 2;
                else
                    item.second.m_val = item.first * 5;
            }
        };

        struct check_value {
            int     m_nExpected;

            check_value( int nExpected )
                : m_nExpected( nExpected )
            {}

            template <typename T>
            void operator ()( T& pair )
            {
                CPPUNIT_ASSERT_CURRENT( pair.second.m_val == m_nExpected );
            }
            template <typename T, typename Q>
            void operator ()( T& pair, Q )
            {
                CPPUNIT_ASSERT_CURRENT( pair.second.m_val == m_nExpected );
            }
        };

        struct extract_functor
        {
            int *   m_pVal;
            void operator()( pair_type const& val )
            {
                *m_pVal = val.second.m_val;
            }
        };

        template <class Map>
        void test_int_with( Map& m )
        {
            std::pair<bool, bool> ensureResult;

            // insert
            CPPUNIT_ASSERT( m.empty() );
            CPPUNIT_ASSERT( check_size( m, 0 ));
            CPPUNIT_ASSERT( !m.find(25) );
            CPPUNIT_ASSERT( m.insert( 25 ) )    ;   // value = 0
            CPPUNIT_ASSERT( m.find(25) );
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 1 ));
            CPPUNIT_ASSERT( m.find(25) );

            CPPUNIT_ASSERT( !m.insert( 25 ) );
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 1 ));

            CPPUNIT_ASSERT( !m.find(10) );
            CPPUNIT_ASSERT( m.insert( 10, 10 ) );
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 2 ));
            CPPUNIT_ASSERT( m.find(10) );

            CPPUNIT_ASSERT( !m.insert( 10, 20 ) );
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 2 ));

            CPPUNIT_ASSERT( !m.find(30) );
            CPPUNIT_ASSERT( m.insert_with( 30, insert_functor<Map>() ) )    ; // value = 90
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 3 ));
            CPPUNIT_ASSERT( m.find(30) );

            CPPUNIT_ASSERT( !m.insert_with( 10, insert_functor<Map>() ) );
            CPPUNIT_ASSERT( !m.insert_with( 25, insert_functor<Map>() ) );
            CPPUNIT_ASSERT( !m.insert_with( 30, insert_functor<Map>() ) );

            // ensure (new key)
            CPPUNIT_ASSERT( !m.find(27) );
            ensureResult = m.ensure( 27, insert_functor<Map>() ) ;   // value = 54
            CPPUNIT_ASSERT( ensureResult.first );
            CPPUNIT_ASSERT( ensureResult.second );
            CPPUNIT_ASSERT( check_size( m, 4 ));
            CPPUNIT_ASSERT( m.find(27) );

            // find test
            check_value chk(10);
            CPPUNIT_ASSERT( m.find( 10, std::ref(chk) ));
            chk.m_nExpected = 0;
            CPPUNIT_ASSERT( m.find( 25, std::ref( chk ) ) );
            chk.m_nExpected = 90;
            CPPUNIT_ASSERT( m.find( 30, std::ref( chk ) ) );
            chk.m_nExpected = 54;
            CPPUNIT_ASSERT( m.find( 27, std::ref( chk ) ) );

            ensureResult = m.ensure( 10, insert_functor<Map>() ) ;   // value = 50
            CPPUNIT_ASSERT( ensureResult.first );
            CPPUNIT_ASSERT( !ensureResult.second );
            chk.m_nExpected = 50;
            CPPUNIT_ASSERT( m.find( 10, std::ref( chk ) ) );

            // erase test
            CPPUNIT_ASSERT( !m.find(100) );
            CPPUNIT_ASSERT( !m.erase( 100 )) ;  // not found

            CPPUNIT_ASSERT( m.find(25) );
            CPPUNIT_ASSERT( check_size( m, 4 ));
            CPPUNIT_ASSERT( m.erase( 25 ));
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 3 ));
            CPPUNIT_ASSERT( !m.find(25) );
            CPPUNIT_ASSERT( !m.erase( 25 ));

            CPPUNIT_ASSERT( !m.find(258) );
            CPPUNIT_ASSERT( m.insert(258))
            CPPUNIT_ASSERT( check_size( m, 4 ));
            CPPUNIT_ASSERT( m.find(258) );
            CPPUNIT_ASSERT( m.erase( 258 ));
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 3 ));
            CPPUNIT_ASSERT( !m.find(258) );
            CPPUNIT_ASSERT( !m.erase( 258 ));

            int nVal;
            extract_functor ext;
            ext.m_pVal = &nVal;

            CPPUNIT_ASSERT( !m.find(29) );
            CPPUNIT_ASSERT( m.insert(29, 290));
            CPPUNIT_ASSERT( check_size( m, 4 ));
            CPPUNIT_ASSERT( m.erase( 29, std::ref( ext ) ) );
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 3 ));
            CPPUNIT_ASSERT( nVal == 290 );
            nVal = -1;
            CPPUNIT_ASSERT( !m.erase( 29, std::ref( ext ) ) );
            CPPUNIT_ASSERT( nVal == -1 );

            CPPUNIT_ASSERT( m.erase( 30, std::ref( ext ) ) );
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 2 ));
            CPPUNIT_ASSERT( nVal == 90 );
            nVal = -1;
            CPPUNIT_ASSERT( !m.erase( 30, std::ref( ext ) ) );
            CPPUNIT_ASSERT( nVal == -1 );

            m.clear();
            CPPUNIT_ASSERT( m.empty() );
            CPPUNIT_ASSERT( check_size( m, 0 ));

            // emplace test
            CPPUNIT_ASSERT( m.emplace(126) ) ; // key = 126, val = 0
            CPPUNIT_ASSERT( m.emplace(137, 731))    ;   // key = 137, val = 731
            CPPUNIT_ASSERT( m.emplace( 149, value_type(941) ))   ;   // key = 149, val = 941

            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 3 ));

            chk.m_nExpected = 0;
            CPPUNIT_ASSERT( m.find( 126, std::ref(chk) ));
            chk.m_nExpected = 731;
            CPPUNIT_ASSERT( m.find( 137, std::ref(chk) ));
            chk.m_nExpected = 941;
            CPPUNIT_ASSERT( m.find( 149, std::ref(chk) ));

            CPPUNIT_ASSERT( !m.emplace(126, 621)) ; // already in map
            chk.m_nExpected = 0;
            CPPUNIT_ASSERT( m.find( 126, std::ref(chk) ));
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 3 ));

            m.clear();
            CPPUNIT_ASSERT( m.empty() );
            CPPUNIT_ASSERT( check_size( m, 0 ));
        }

        template <class Map>
        void test_iter( Map& s)
        {
            typedef typename Map::iterator          iterator;
            typedef typename Map::const_iterator    const_iterator;

            const int nMaxCount = 500;
            for ( int i = 0; i < nMaxCount; ++i ) {
                CPPUNIT_ASSERT( s.insert( i, i * 2 ));
            }

            int nCount = 0;
            for ( iterator it = s.begin(), itEnd = s.end(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( it->first * 2 == it->second.m_val );
                CPPUNIT_ASSERT( (*it).first * 2 == (*it).second.m_val );
                it->second.m_val = it->first;
                ++nCount;
            }
            CPPUNIT_ASSERT( nCount == nMaxCount );

            Map const& refSet = s;
            nCount = 0;
            for ( const_iterator it = refSet.begin(), itEnd = refSet.end(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( it->first == it->second.m_val );
                CPPUNIT_ASSERT( (*it).first == (*it).second.m_val );
                ++nCount;
            }
            CPPUNIT_ASSERT( nCount == nMaxCount );
        }

        template <class Map>
        void test_striped()
        {
            Map m( 30 );
            CPPUNIT_ASSERT( m.bucket_count() == 32 );
            CPPUNIT_ASSERT( m.lock_count() == 32 );

            test_striped_with(m);
        }

        template <class Map>
        void test_striped_with(Map& m)
        {
            cds::OS::Timer    timer;

            test_int_with( m );

            // Iterators is not yet supported
            //m.clear();
            //CPPUNIT_ASSERT( m.empty() );
            //CPPUNIT_ASSERT( check_size( m, 0 ));
            //test_iter(m);

            m.clear();
            CPPUNIT_ASSERT( m.empty() );
            CPPUNIT_ASSERT( check_size( m, 0 ));

            // Resizing test
            for ( int i = 0; i < 40000; i++ ) {
                m.insert( i );
            }

            CPPUNIT_MSG( "   Duration=" << timer.duration() );
        }

        //*******************************************
        // If erase_with && find_with are supported
        template <class Map>
        void test_int_with2( Map& m )
        {
            std::pair<bool, bool> ensureResult;

            // insert
            CPPUNIT_ASSERT( m.empty() );
            CPPUNIT_ASSERT( check_size( m, 0 ));
            CPPUNIT_ASSERT( !m.find(25) );
            CPPUNIT_ASSERT( m.insert( 25 ) )    ;   // value = 0
            CPPUNIT_ASSERT( m.find(25) );
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 1 ));
            CPPUNIT_ASSERT( m.find(25) );

            CPPUNIT_ASSERT( !m.insert( 25 ) );
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 1 ));

            CPPUNIT_ASSERT( !m.find_with(10, less()) );
            CPPUNIT_ASSERT( m.insert( 10, 10 ) );
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 2 ));
            CPPUNIT_ASSERT( m.find_with(10, less()) );

            CPPUNIT_ASSERT( !m.insert( 10, 20 ) );
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 2 ));

            CPPUNIT_ASSERT( !m.find(30) );
            CPPUNIT_ASSERT( m.insert_with( 30, insert_functor<Map>() ) )    ; // value = 90
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 3 ));
            CPPUNIT_ASSERT( m.find(30) );

            CPPUNIT_ASSERT( !m.insert_with( 10, insert_functor<Map>() ) );
            CPPUNIT_ASSERT( !m.insert_with( 25, insert_functor<Map>() ) );
            CPPUNIT_ASSERT( !m.insert_with( 30, insert_functor<Map>() ) );

            // ensure (new key)
            CPPUNIT_ASSERT( !m.find(27) );
            ensureResult = m.ensure( 27, insert_functor<Map>() ) ;   // value = 54
            CPPUNIT_ASSERT( ensureResult.first );
            CPPUNIT_ASSERT( ensureResult.second );
            CPPUNIT_ASSERT( m.find(27) );

            // find test
            check_value chk(10);
            CPPUNIT_ASSERT( m.find( 10, std::ref(chk) ));
            chk.m_nExpected = 0;
            CPPUNIT_ASSERT( m.find_with( 25, less(), std::ref( chk ) ) );
            chk.m_nExpected = 90;
            CPPUNIT_ASSERT( m.find( 30, std::ref( chk ) ) );
            chk.m_nExpected = 54;
            CPPUNIT_ASSERT( m.find( 27, std::ref( chk ) ) );

            ensureResult = m.ensure( 10, insert_functor<Map>() ) ;   // value = 50
            CPPUNIT_ASSERT( ensureResult.first );
            CPPUNIT_ASSERT( !ensureResult.second );
            chk.m_nExpected = 50;
            CPPUNIT_ASSERT( m.find( 10, std::ref( chk ) ) );

            // erase test
            CPPUNIT_ASSERT( !m.find(100) );
            CPPUNIT_ASSERT( !m.erase( 100 )) ;  // not found

            CPPUNIT_ASSERT( m.find(25) );
            CPPUNIT_ASSERT( check_size( m, 4 ));
            CPPUNIT_ASSERT( m.erase( 25 ));
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 3 ));
            CPPUNIT_ASSERT( !m.find(25) );
            CPPUNIT_ASSERT( !m.erase( 25 ));

            CPPUNIT_ASSERT( !m.find(258) );
            CPPUNIT_ASSERT( m.insert(258))
            CPPUNIT_ASSERT( check_size( m, 4 ));
            CPPUNIT_ASSERT( m.find_with(258, less()) );
            CPPUNIT_ASSERT( m.erase_with( 258, less() ));
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 3 ));
            CPPUNIT_ASSERT( !m.find(258) );
            CPPUNIT_ASSERT( !m.erase_with( 258, less() ));

            int nVal;
            extract_functor ext;
            ext.m_pVal = &nVal;

            CPPUNIT_ASSERT( !m.find(29) );
            CPPUNIT_ASSERT( m.insert(29, 290))
                CPPUNIT_ASSERT( m.erase_with( 29, less(), std::ref( ext ) ) );
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 3 ));
            CPPUNIT_ASSERT( nVal == 290 );
            nVal = -1;
            CPPUNIT_ASSERT( !m.erase_with( 29, less(), std::ref( ext ) ) );
            CPPUNIT_ASSERT( nVal == -1 );

            CPPUNIT_ASSERT( m.erase( 30, std::ref( ext ) ) );
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 2 ));
            CPPUNIT_ASSERT( nVal == 90 );
            nVal = -1;
            CPPUNIT_ASSERT( !m.erase( 30, std::ref( ext ) ) );
            CPPUNIT_ASSERT( nVal == -1 );

            m.clear();
            CPPUNIT_ASSERT( m.empty() );
            CPPUNIT_ASSERT( check_size( m, 0 ));

            // emplace test
            CPPUNIT_ASSERT( m.emplace(126) ) ; // key = 126, val = 0
            CPPUNIT_ASSERT( m.emplace(137, 731))    ;   // key = 137, val = 731
            CPPUNIT_ASSERT( m.emplace( 149, value_type(941) ))   ;   // key = 149, val = 941

            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 3 ));

            chk.m_nExpected = 0;
            CPPUNIT_ASSERT( m.find( 126, std::ref(chk) ));
            chk.m_nExpected = 731;
            CPPUNIT_ASSERT( m.find_with( 137, less(), std::ref(chk) ));
            chk.m_nExpected = 941;
            CPPUNIT_ASSERT( m.find( 149, std::ref(chk) ));

            CPPUNIT_ASSERT( !m.emplace(126, 621)) ; // already in map
            chk.m_nExpected = 0;
            CPPUNIT_ASSERT( m.find( 126, std::ref(chk) ));
            CPPUNIT_ASSERT( !m.empty() );
            CPPUNIT_ASSERT( check_size( m, 3 ));

            m.clear();
            CPPUNIT_ASSERT( m.empty() );
            CPPUNIT_ASSERT( check_size( m, 0 ));
        }

        template <class Map>
        void test_striped_with2(Map& m)
        {
            cds::OS::Timer    timer;

            test_int_with2( m );

            // Iterators is not yet supported
            //m.clear();
            //CPPUNIT_ASSERT( m.empty() );
            //CPPUNIT_ASSERT( check_size( m, 0 ));
            //test_iter(m);

            m.clear();
            CPPUNIT_ASSERT( m.empty() );
            CPPUNIT_ASSERT( check_size( m, 0 ));

            // Resizing test
            for ( int i = 0; i < 40000; i++ ) {
                m.insert( i );
            }

            CPPUNIT_MSG( "   Duration=" << timer.duration() );
        }

        template <class Map>
        void test_striped2()
        {
            Map m( 30 );
            CPPUNIT_ASSERT( m.bucket_count() == 32 );
            CPPUNIT_ASSERT( m.lock_count() == 32 );

            test_striped_with2(m);
        }

        void Striped_hashmap();
        void Striped_list();
        void Striped_map();
        void Striped_slist();
        void Striped_boost_list();
        void Striped_boost_flat_map();
        void Striped_boost_map();
        void Striped_boost_unordered_map();

        void Refinable_hashmap();
        void Refinable_list();
        void Refinable_map();
        void Refinable_slist();
        void Refinable_boost_list();
        void Refinable_boost_flat_map();
        void Refinable_boost_map();
        void Refinable_boost_unordered_map();

        CPPUNIT_TEST_SUITE(StripedMapHdrTest)
            CPPUNIT_TEST(Striped_hashmap)
            CPPUNIT_TEST(Striped_list)
            CPPUNIT_TEST(Striped_map)
            CPPUNIT_TEST(Striped_slist)
            CPPUNIT_TEST(Striped_boost_list)
            CPPUNIT_TEST(Striped_boost_flat_map)
            CPPUNIT_TEST(Striped_boost_map)
            CPPUNIT_TEST(Striped_boost_unordered_map)

            CPPUNIT_TEST(Refinable_hashmap)
            CPPUNIT_TEST(Refinable_list)
            CPPUNIT_TEST(Refinable_map)
            CPPUNIT_TEST(Refinable_slist)
            CPPUNIT_TEST(Refinable_boost_list)
            CPPUNIT_TEST(Refinable_boost_flat_map)
            CPPUNIT_TEST(Refinable_boost_map)
            CPPUNIT_TEST(Refinable_boost_unordered_map)
        CPPUNIT_TEST_SUITE_END()

    };
}   // namespace map

#endif // #ifndef CDSTEST_HDR_STRIPED_MAP_H
