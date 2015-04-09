//$$CDS-header$$

#ifndef CDSTEST_HDR_STRIPED_SET_H
#define CDSTEST_HDR_STRIPED_SET_H

#include "cppunit/cppunit_proxy.h"
#include "size_check.h"

#include <cds/opt/hash.h>
#include <cds/os/timer.h>
#include <functional>   // ref

// forward namespace declaration
namespace cds {
    namespace container {}
    namespace opt {}
}

namespace set {
    using misc::check_size;

    namespace cc = cds::container;
    namespace co = cds::opt;


    class StripedSetHdrTest: public CppUnitMini::TestCase
    {
    public:
        struct stat
        {
            unsigned int nFindCount     ;   // count of find-functor calling
            unsigned int nEnsureNewCount;
            unsigned int nEnsureCount;

            stat()
            {
                memset( this, 0, sizeof(*this));
            }

            void copy( stat const& s )
            {
                nFindCount = s.nFindCount;
                nEnsureCount = s.nEnsureCount;
                nEnsureNewCount = s.nEnsureNewCount;
            }
        };

        struct item: public stat
        {
            int nKey;
            int nVal;

            item()
            {}

            item( int key )
                : nKey( key )
                , nVal( key )
            {}

            item (int key, int val )
                : nKey(key)
                , nVal( val )
            {}

            item( std::pair<int,int> const& p )
                : nKey( p.first )
                , nVal( p.second )
            {}

            item( item const& i )
                : nKey( i.nKey )
                , nVal( i.nVal )
            {}

            item& operator=(item const& i)
            {
                nKey = i.nKey;
                nVal = i.nVal;
                stat::copy(i);

                return *this;
            }

            item( item&& i )
                : nKey( i.nKey )
                , nVal( i.nVal )
            {}

            //item& operator=(item&& i)
            //{
            //    nKey = i.nKey;
            //    nVal = i.nVal;
            //    return *this;
            //}

            int key() const
            {
                return nKey;
            }

            int val() const
            {
                return nVal;
            }
        };

        struct hash_int {
            size_t operator()( int i ) const
            {
                return co::v::hash<int>()( i );
            }

            size_t operator()( std::pair<int,int> const& i ) const
            {
                return co::v::hash<int>()( i.first );
            }

            template <typename Item>
            size_t operator()( Item const& i ) const
            {
                return (*this)( i.key() );
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

            template <typename Q>
            bool operator ()(const T& v1, const Q& v2 ) const
            {
                return v1.key() < v2;
            }

            template <typename Q>
            bool operator ()(const Q& v1, const T& v2 ) const
            {
                return v1 < v2.key();
            }

            bool operator ()( std::pair<int, int> const& v1, const T& v2 ) const
            {
                return v1.first < v2.key();
            }

            bool operator ()(const T& v1, std::pair<int, int> const& v2 ) const
            {
                return v1.key() < v2.first;
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

            int operator()( std::pair<int,int> const& v1, T const& v2 ) const
            {
                if ( v1.first < v2.key() )
                    return -1;
                return v1.first > v2.key() ? 1 : 0;
            }

            int operator()( T const& v1, std::pair<int,int> const& v2 ) const
            {
                if ( v1.key() < v2.first )
                    return -1;
                return v1.key() > v2.first ? 1 : 0;
            }
        };

        template <typename T>
        struct equal
        {
            bool operator ()(const T& v1, const T& v2 ) const
            {
                return v1.key() == v2.key();
            }

            template <typename Q>
            bool operator ()(const T& v1, const Q& v2 ) const
            {
                return v1.key() == v2;
            }

            template <typename Q>
            bool operator ()(const Q& v1, const T& v2 ) const
            {
                return v1 == v2.key();
            }

            bool operator ()( std::pair<int, int> const& v1, const T& v2 ) const
            {
                return v1.first == v2.key();
            }

            bool operator ()(const T& v1, std::pair<int, int> const& v2 ) const
            {
                return v1.key() == v2.first;
            }
        };

        struct find_functor
        {
            template <typename Item, typename T>
            void operator()( Item& i, T& /*val*/ ) const
            {
                ++i.nFindCount;
            }
            template <typename Item, typename T>
            void operator()( Item& i, T const& /*val*/ ) const
            {
                ++i.nFindCount;
            }
        };

        template <typename Item>
        struct copy_found
        {
            Item    m_found;

            template <typename T>
            void operator()( Item& i, T& /*val*/ )
            {
                m_found = i;
            }

            void operator()( Item const& i )
            {
                m_found = i;
            }
        };

        struct insert_functor
        {
            template <typename Item>
            void operator()(Item& i )
            {
                i.nVal = i.nKey * 100;
            }
        };

        template <typename Item, typename Q>
        static void ensure_func( bool bNew, Item& i, Q& /*val*/ )
        {
            if ( bNew )
                ++i.nEnsureNewCount;
            else
                ++i.nEnsureCount;
        }

        struct ensure_functor
        {
            template <typename Item, typename Q>
            void operator()( bool bNew, Item& i, Q& val )
            {
                ensure_func( bNew, i, val );
            }
        };

    public:
        template <class Set>
        void test_striped()
        {
            Set s( 30 );
            CPPUNIT_ASSERT( s.bucket_count() == 32 );
            CPPUNIT_ASSERT( s.lock_count() == 32 );

            test_striped_with( s );
        }

        template <class Set>
        void test_striped_with( Set& s )
        {
            cds::OS::Timer    timer;

            test_int_with( s );

            // Resizing test
            for ( int i = 0; i < 10000; i++ ) {
                s.insert( i );
            }

            CPPUNIT_MSG( "   Duration=" << timer.duration() );
        }

        template <class Set>
        void test_int_with( Set& s)
        {
            typedef typename Set::value_type    value_type;

            item itm;
            int key;

            // insert/find test
            CPPUNIT_ASSERT( !s.find( 10 ) );
            CPPUNIT_ASSERT( s.insert( 10 ));
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 1 ));
            CPPUNIT_ASSERT( s.find( 10 ) );

            CPPUNIT_ASSERT( !s.insert( 10 ));
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 1 ));

            CPPUNIT_ASSERT( !s.find( 20 ) );
            CPPUNIT_ASSERT( s.insert( std::make_pair(20, 25) ));
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 2 ));
            CPPUNIT_ASSERT( s.find( 10 ) );
            CPPUNIT_ASSERT( s.find( key = 20 ) );
            CPPUNIT_ASSERT( s.find( key, find_functor() ) );
            {
                copy_found<item> f;
                key = 20;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 20 );
                CPPUNIT_ASSERT( f.m_found.nVal == 25 );
                CPPUNIT_ASSERT( f.m_found.nFindCount == 1 );
            }
            {
                copy_found<item> f;
                key = 20;
                CPPUNIT_ASSERT( s.find( key, find_functor() ) );
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 20 );
                CPPUNIT_ASSERT( f.m_found.nVal == 25 );
                CPPUNIT_ASSERT( f.m_found.nFindCount == 2 );
            }
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 2 ));

            CPPUNIT_ASSERT( !s.find( 25 ) );
            CPPUNIT_ASSERT( s.insert( std::make_pair(25, -1), insert_functor() ));
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 3 ));
            {
                copy_found<item> f;
                key = 25;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 25 );
                CPPUNIT_ASSERT( f.m_found.nVal == 2500 );
            }

            // ensure test
            key = 10;
            {
                copy_found<item> f;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 10 );
                CPPUNIT_ASSERT( f.m_found.nVal == 10 );
                CPPUNIT_ASSERT( f.m_found.nEnsureCount == 0 );
                CPPUNIT_ASSERT( f.m_found.nEnsureNewCount == 0 );
            }
            std::pair<bool, bool> ensureResult = s.ensure( key, ensure_functor() );
            CPPUNIT_ASSERT( ensureResult.first && !ensureResult.second );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 3 ));
            {
                copy_found<item> f;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 10 );
                CPPUNIT_ASSERT( f.m_found.nVal == 10 );
                CPPUNIT_ASSERT( f.m_found.nEnsureCount == 1 );
                CPPUNIT_ASSERT( f.m_found.nEnsureNewCount == 0 );
            }

            ensureResult = s.ensure( std::make_pair(13, 1300), ensure_functor() );
            CPPUNIT_ASSERT( ensureResult.first && ensureResult.second );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 4 ));
            {
                copy_found<item> f;
                key = 13;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 13 );
                CPPUNIT_ASSERT( f.m_found.nVal == 1300 );
                CPPUNIT_ASSERT( f.m_found.nEnsureCount == 0 );
                CPPUNIT_ASSERT( f.m_found.nEnsureNewCount == 1 );
            }

            // erase test
            CPPUNIT_ASSERT( s.erase(13) );
            CPPUNIT_ASSERT( !s.find( 13 ));
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 3 ));
            CPPUNIT_ASSERT( !s.erase(13) );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 3 ));

            CPPUNIT_ASSERT( s.find( 10 ));
            CPPUNIT_ASSERT( s.erase( 10 ));
            CPPUNIT_ASSERT( !s.find( 10 ));
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 2 ));
            CPPUNIT_ASSERT( !s.erase(10) );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 2 ));

            CPPUNIT_ASSERT( s.find(20) );
            {
                copy_found<item> f;
                CPPUNIT_ASSERT( s.erase( 20, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 20 );
                CPPUNIT_ASSERT( f.m_found.nVal == 25 );

                CPPUNIT_ASSERT( s.insert(235))
                    CPPUNIT_ASSERT( s.erase( 235, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 235 );
                CPPUNIT_ASSERT( f.m_found.nVal == 235 );
            }
            CPPUNIT_ASSERT( !s.find( 20 ));
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 1 ));

            s.clear();
            CPPUNIT_ASSERT( s.empty() );
            CPPUNIT_ASSERT( check_size( s, 0 ));

            // emplace test
            CPPUNIT_ASSERT( s.emplace( 151 )) ;  // key = 151,  val = 151
            CPPUNIT_ASSERT( s.emplace( 174, 471 )) ;    // key = 174, val = 471
            CPPUNIT_ASSERT( s.emplace( std::make_pair( 190, 91 ) )) ; // key == 190, val = 91
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 3 ));

            CPPUNIT_ASSERT( s.find(151));
            CPPUNIT_ASSERT( s.find(174));
            CPPUNIT_ASSERT( s.find(190));

            {
                copy_found<item> f;
                key = 151;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 151 );
                CPPUNIT_ASSERT( f.m_found.nVal == 151 );

                key = 174;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 174 );
                CPPUNIT_ASSERT( f.m_found.nVal == 471 );

                key = 190;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 190 );
                CPPUNIT_ASSERT( f.m_found.nVal == 91 );
            }

            s.clear();
            CPPUNIT_ASSERT( s.empty() );
            CPPUNIT_ASSERT( check_size( s, 0 ));
        }

        template <class Set>
        void test_striped2()
        {
            Set s( 30 );
            CPPUNIT_ASSERT( s.bucket_count() == 32 );
            CPPUNIT_ASSERT( s.lock_count() == 32 );

            test_striped_with2( s );
        }

        template <class Set>
        void test_striped_with2( Set& s )
        {
            cds::OS::Timer    timer;

            test_int_with2( s );

            // Resizing test
            for ( int i = 0; i < 10000; i++ ) {
                s.insert( i );
            }

            CPPUNIT_MSG( "   Duration=" << timer.duration() );
        }

        template <class Set>
        void test_int_with2( Set& s)
        {
            typedef typename Set::value_type    value_type;

            item itm;
            int key;

            // insert/find test
            CPPUNIT_ASSERT( !s.find( 10 ) );
            CPPUNIT_ASSERT( s.insert( 10 ));
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 1 ));
            CPPUNIT_ASSERT( s.find( 10 ) );

            CPPUNIT_ASSERT( !s.insert( 10 ));
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 1 ));

            CPPUNIT_ASSERT( !s.find_with( 20, less<value_type>() ) );
            CPPUNIT_ASSERT( s.insert( std::make_pair(20, 25) ));
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 2 ));
            CPPUNIT_ASSERT( s.find( 10 ) );
            CPPUNIT_ASSERT( s.find_with( key = 20, less<value_type>() ) );
            CPPUNIT_ASSERT( s.find_with( key, less<value_type>(), find_functor() ) );
            {
                copy_found<item> f;
                key = 20;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 20 );
                CPPUNIT_ASSERT( f.m_found.nVal == 25 );
                CPPUNIT_ASSERT( f.m_found.nFindCount == 1 );
            }
            {
                copy_found<item> f;
                key = 20;
                CPPUNIT_ASSERT( s.find_with( 20, less<value_type>(), find_functor() ) );
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 20 );
                CPPUNIT_ASSERT( f.m_found.nVal == 25 );
                CPPUNIT_ASSERT( f.m_found.nFindCount == 2 );
            }
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 2 ));

            CPPUNIT_ASSERT( !s.find( 25 ) );
            CPPUNIT_ASSERT( s.insert( std::make_pair(25, -1), insert_functor() ));
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 3 ));
            {
                copy_found<item> f;
                key = 25;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 25 );
                CPPUNIT_ASSERT( f.m_found.nVal == 2500 );
            }

            // ensure test
            key = 10;
            {
                copy_found<item> f;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 10 );
                CPPUNIT_ASSERT( f.m_found.nVal == 10 );
                CPPUNIT_ASSERT( f.m_found.nEnsureCount == 0 );
                CPPUNIT_ASSERT( f.m_found.nEnsureNewCount == 0 );
            }
            std::pair<bool, bool> ensureResult = s.ensure( key, ensure_functor() );
            CPPUNIT_ASSERT( ensureResult.first && !ensureResult.second );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 3 ));
            {
                copy_found<item> f;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 10 );
                CPPUNIT_ASSERT( f.m_found.nVal == 10 );
                CPPUNIT_ASSERT( f.m_found.nEnsureCount == 1 );
                CPPUNIT_ASSERT( f.m_found.nEnsureNewCount == 0 );
            }

            ensureResult = s.ensure( std::make_pair(13, 1300), ensure_functor() );
            CPPUNIT_ASSERT( ensureResult.first && ensureResult.second );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 4 ));
            {
                copy_found<item> f;
                key = 13;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 13 );
                CPPUNIT_ASSERT( f.m_found.nVal == 1300 );
                CPPUNIT_ASSERT( f.m_found.nEnsureCount == 0 );
                CPPUNIT_ASSERT( f.m_found.nEnsureNewCount == 1 );
            }

            // erase test
            CPPUNIT_ASSERT( s.erase(13) );
            CPPUNIT_ASSERT( !s.find( 13 ));
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 3 ));
            CPPUNIT_ASSERT( !s.erase(13) );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 3 ));

            CPPUNIT_ASSERT( s.find( 10 ));
            CPPUNIT_ASSERT( s.erase_with( 10, less<value_type>() ));
            CPPUNIT_ASSERT( !s.find( 10 ));
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 2 ));
            CPPUNIT_ASSERT( !s.erase_with( 10, less<value_type>() ) );
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 2 ));

            CPPUNIT_ASSERT( s.find(20) );
            {
                copy_found<item> f;
                CPPUNIT_ASSERT( s.erase( 20, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 20 );
                CPPUNIT_ASSERT( f.m_found.nVal == 25 );

                CPPUNIT_ASSERT( s.insert(235))
                    CPPUNIT_ASSERT( s.erase_with( 235, less<value_type>(), std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 235 );
                CPPUNIT_ASSERT( f.m_found.nVal == 235 );
            }
            CPPUNIT_ASSERT( !s.find( 20 ));
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 1 ));

            s.clear();
            CPPUNIT_ASSERT( s.empty() );
            CPPUNIT_ASSERT( check_size( s, 0 ));

            // emplace test
            CPPUNIT_ASSERT( s.emplace( 151 )) ;  // key = 151,  val = 151
            CPPUNIT_ASSERT( s.emplace( 174, 471 )) ;    // key = 174, val = 471
            CPPUNIT_ASSERT( s.emplace( std::make_pair( 190, 91 ) )) ; // key == 190, val = 91
            CPPUNIT_ASSERT( !s.empty() );
            CPPUNIT_ASSERT( check_size( s, 3 ));

            CPPUNIT_ASSERT( s.find(151));
            CPPUNIT_ASSERT( s.find(174));
            CPPUNIT_ASSERT( s.find(190));

            {
                copy_found<item> f;
                key = 151;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 151 );
                CPPUNIT_ASSERT( f.m_found.nVal == 151 );

                key = 174;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 174 );
                CPPUNIT_ASSERT( f.m_found.nVal == 471 );

                key = 190;
                CPPUNIT_ASSERT( s.find( key, std::ref( f ) ) );
                CPPUNIT_ASSERT( f.m_found.nKey == 190 );
                CPPUNIT_ASSERT( f.m_found.nVal == 91 );
            }

            s.clear();
            CPPUNIT_ASSERT( s.empty() );
            CPPUNIT_ASSERT( check_size( s, 0 ));
        }

        void Striped_list();
        void Striped_vector();
        void Striped_set();
        void Striped_hashset();
        void Striped_slist();
        void Striped_boost_list();
        void Striped_boost_vector();
        void Striped_boost_stable_vector();
        void Striped_boost_flat_set();
        void Striped_boost_set();
        void Striped_boost_unordered_set();

        void Refinable_list();
        void Refinable_vector();
        void Refinable_set();
        void Refinable_hashset();
        void Refinable_slist();
        void Refinable_boost_list();
        void Refinable_boost_vector();
        void Refinable_boost_stable_vector();
        void Refinable_boost_flat_set();
        void Refinable_boost_set();
        void Refinable_boost_unordered_set();

        CPPUNIT_TEST_SUITE(StripedSetHdrTest)
            CPPUNIT_TEST(Striped_list)
            CPPUNIT_TEST(Striped_vector)
            CPPUNIT_TEST(Striped_set)
            CPPUNIT_TEST(Striped_hashset)
            CPPUNIT_TEST(Striped_slist)
            CPPUNIT_TEST(Striped_boost_list)
            CPPUNIT_TEST(Striped_boost_vector)
            CPPUNIT_TEST(Striped_boost_stable_vector)
            CPPUNIT_TEST(Striped_boost_flat_set)
            CPPUNIT_TEST(Striped_boost_set)
            CPPUNIT_TEST(Striped_boost_unordered_set)

            CPPUNIT_TEST(Refinable_list)
            CPPUNIT_TEST(Refinable_vector)
            CPPUNIT_TEST(Refinable_set)
            CPPUNIT_TEST(Refinable_hashset)
            CPPUNIT_TEST(Refinable_slist)
            CPPUNIT_TEST(Refinable_boost_list)
            CPPUNIT_TEST(Refinable_boost_vector)
            CPPUNIT_TEST(Refinable_boost_stable_vector)
            CPPUNIT_TEST(Refinable_boost_flat_set)
            CPPUNIT_TEST(Refinable_boost_set)
            CPPUNIT_TEST(Refinable_boost_unordered_set)

        CPPUNIT_TEST_SUITE_END()
    };
} // namespace set

#endif // #ifndef CDSTEST_HDR_STRIPED_SET_H
