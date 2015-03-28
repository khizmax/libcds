//$$CDS-header$$

#ifndef CDSTEST_HDR_LAZY_KV_H
#define CDSTEST_HDR_LAZY_KV_H

#include "cppunit/cppunit_proxy.h"
#include <cds/container/details/lazy_list_base.h>

namespace unordlist {
    namespace cc = cds::container;
    namespace co = cds::container::opt;

    class UnorderedLazyKVListTestHeader: public CppUnitMini::TestCase
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
        };

        template <typename T>
        struct lt
        {
            bool operator ()(const T& v1, const T& v2 ) const
            {
                return v1 < v2;
            }
        };

        template <typename T>
        struct cmp {
            int operator ()(const T& v1, const T& v2 ) const
            {
                if ( v1 < v2 )
                    return -1;
                return v1 > v2 ? 1 : 0;
            }
        };

        template <typename T>
        struct eq {
            bool operator ()(const T& v1, const T& v2 ) const
            {
                return v1 == v2;
            }
        };

        struct insert_functor {
            template <typename T>
            void operator()( T& pair )
            {
                pair.second.m_val = pair.first * 10;
            }
        };

    protected:
        template <class UnordList>
        void nogc_test()
        {
            typedef typename UnordList::value_type    value_type;
            typedef typename UnordList::iterator      iterator;

            {
                UnordList l;
                iterator it;

                CPPUNIT_ASSERT( l.empty() );

                // insert / find test
                CPPUNIT_ASSERT( l.find( 100 ) == l.end() );
                CPPUNIT_ASSERT( l.insert( 100 ) != l.end() );
                CPPUNIT_ASSERT( !l.empty() );
                it = l.find( 100 );
                CPPUNIT_ASSERT( it != l.end() );
                CPPUNIT_ASSERT( it.key() == 100 );
                CPPUNIT_ASSERT( it.val().m_val == 0 );

                CPPUNIT_ASSERT( l.insert( 50, 500 ) != l.end());
                it = l.find( 50 );
                CPPUNIT_ASSERT( it != l.end() );
                CPPUNIT_ASSERT( it.key() == 50 );
                CPPUNIT_ASSERT( it.val().m_val == 500 );

                CPPUNIT_ASSERT( l.insert( 50, 5 ) == l.end() );
                it = l.find( 50 );
                CPPUNIT_ASSERT( it != l.end() );
                CPPUNIT_ASSERT( it.key() == 50 );
                CPPUNIT_ASSERT( it.val().m_val == 500 );
                CPPUNIT_ASSERT( !l.empty() );

                CPPUNIT_ASSERT( l.find( 150 ) == l.end() );
                CPPUNIT_ASSERT( l.insert_with( 150, insert_functor() ) != l.end() );
                it = l.find( 150 );
                CPPUNIT_ASSERT( it != l.end() );
                CPPUNIT_ASSERT( it.key() == 150 );
                CPPUNIT_ASSERT( it.val().m_val == 1500 );
                it = l.find( 100 );
                CPPUNIT_ASSERT( it != l.end() );
                CPPUNIT_ASSERT( it.key() == 100 );
                CPPUNIT_ASSERT( it.val().m_val == 0 );
                it = l.find( 50 );
                CPPUNIT_ASSERT( it != l.end() );
                CPPUNIT_ASSERT( it.key() == 50 );
                CPPUNIT_ASSERT( it.val().m_val == 500 );
                it.val().m_val = 25;
                it = l.find( 50 );
                CPPUNIT_ASSERT( it != l.end() );
                CPPUNIT_ASSERT( it.key() == 50 );
                CPPUNIT_ASSERT( it.val().m_val == 25 );
                CPPUNIT_ASSERT( !l.empty() );

                // ensure existing item
                std::pair<iterator, bool> ensureResult;
                ensureResult = l.ensure( 100 );
                CPPUNIT_ASSERT( !ensureResult.second );
                CPPUNIT_ASSERT( ensureResult.first.key() == 100 );
                CPPUNIT_ASSERT( ensureResult.first.val().m_val == 0   );
                ensureResult.first.val().m_val = 5;
                it = l.find( 100 );
                CPPUNIT_ASSERT( it != l.end() );
                CPPUNIT_ASSERT( it.key() == 100 );
                CPPUNIT_ASSERT( it.val().m_val == 5 );

                CPPUNIT_ASSERT( !l.empty() );

                // ensure new item
                ensureResult = l.ensure( 1000 );
                CPPUNIT_ASSERT( ensureResult.second );
                CPPUNIT_ASSERT( ensureResult.first.key() == 1000 );
                CPPUNIT_ASSERT( ensureResult.first.val().m_val == 0   );
                ensureResult.first.val().m_val = 33;
                ensureResult = l.ensure( 1000 );
                CPPUNIT_ASSERT( !ensureResult.second );
                CPPUNIT_ASSERT( ensureResult.first.key() == 1000 );
                CPPUNIT_ASSERT( ensureResult.first.val().m_val == 33   );

                // clear test
                l.clear();
                CPPUNIT_ASSERT( l.empty() );

                // insert test
                CPPUNIT_ASSERT( l.emplace( 501 ) != l.end());
                CPPUNIT_ASSERT( l.emplace( 251, 152 ) != l.end());

                // insert failed - such key exists
                CPPUNIT_ASSERT( l.emplace( 501, 2 ) == l.end());
                CPPUNIT_ASSERT( l.emplace( 251, 10) == l.end());

                it = l.find(501);
                CPPUNIT_ASSERT( it != l.end() );
                CPPUNIT_ASSERT( it.key() == 501 );
                CPPUNIT_ASSERT( it.val().m_val == 0 );

                it = l.find(251);
                CPPUNIT_ASSERT( it != l.end() );
                CPPUNIT_ASSERT( it.key() == 251 );
                CPPUNIT_ASSERT( it.val().m_val == 152 );

                l.clear();
                CPPUNIT_ASSERT( l.empty() );

                // Iterator test
                {
                    int nCount = 100;
                    for ( int i = 0; i < nCount; ++i )
                        CPPUNIT_ASSERT( l.insert(i, i * 2 ) != l.end() );

                    {
                        typename UnordList::iterator it( l.begin() );
                        typename UnordList::const_iterator cit( l.cbegin() );
                        CPPUNIT_CHECK( it == cit );
                        CPPUNIT_CHECK( it != l.end() );
                        CPPUNIT_CHECK( it != l.cend() );
                        CPPUNIT_CHECK( cit != l.end() );
                        CPPUNIT_CHECK( cit != l.cend() );
                        ++it;
                        CPPUNIT_CHECK( it != cit );
                        CPPUNIT_CHECK( it != l.end() );
                        CPPUNIT_CHECK( it != l.cend() );
                        CPPUNIT_CHECK( cit != l.end() );
                        CPPUNIT_CHECK( cit != l.cend() );
                        ++cit;
                        CPPUNIT_CHECK( it == cit );
                        CPPUNIT_CHECK( it != l.end() );
                        CPPUNIT_CHECK( it != l.cend() );
                        CPPUNIT_CHECK( cit != l.end() );
                        CPPUNIT_CHECK( cit != l.cend() );
                    }

                    int i = 0;
                    for ( typename UnordList::iterator iter = l.begin(), itEnd = l.end(); iter != itEnd; ++iter, ++i ) {
                        CPPUNIT_ASSERT( iter.key() == i );
                        CPPUNIT_ASSERT( iter->first == i );
                        CPPUNIT_ASSERT( (*iter).first == i );

                        CPPUNIT_ASSERT( iter.val().m_val == i * 2 );
                        CPPUNIT_ASSERT( iter->second.m_val == i * 2 );
                        CPPUNIT_ASSERT( (*iter).second.m_val == i * 2 );

                        iter.val().m_val = i * 3;
                    }

                    // Check that we have visited all items
                    for ( int i = 0; i < nCount; ++i ) {
                        it = l.find( i );
                        CPPUNIT_ASSERT( it != l.end() );
                        CPPUNIT_ASSERT( it.key() == i );
                        CPPUNIT_ASSERT( it.val().m_val == i * 3 );
                    }

                    l.clear();
                    CPPUNIT_ASSERT( l.empty() );

                    // Const iterator
                    for ( int i = 0; i < nCount; ++i )
                        CPPUNIT_ASSERT( l.insert(i, i * 7) != l.end() );

                    i = 0;
                    const UnordList& rl = l;
                    for ( typename UnordList::const_iterator iter = rl.begin(), itEnd = rl.end(); iter != itEnd; ++iter, ++i ) {
                        CPPUNIT_ASSERT( iter.key() == i );
                        CPPUNIT_ASSERT( iter->first == i );
                        CPPUNIT_ASSERT( (*iter).first == i );

                        CPPUNIT_ASSERT( iter.val().m_val == i * 7 );
                        CPPUNIT_ASSERT( iter->second.m_val == i * 7 );
                        CPPUNIT_ASSERT( (*iter).second.m_val == i * 7 );
                        // it.val().m_val = i * 3    ; // error: const-iterator
                    }

                    l.clear();
                    CPPUNIT_ASSERT( l.empty() );
                }

            }
        }

        void NOGC_cmp();
        void NOGC_less();
        void NOGC_equal_to();
        void NOGC_cmpmix();
        void NOGC_ic();

        CPPUNIT_TEST_SUITE(UnorderedLazyKVListTestHeader)
            CPPUNIT_TEST(NOGC_cmp)
            CPPUNIT_TEST(NOGC_less)
            CPPUNIT_TEST(NOGC_equal_to)
            CPPUNIT_TEST(NOGC_cmpmix)
            CPPUNIT_TEST(NOGC_ic)
        CPPUNIT_TEST_SUITE_END()
    };

}   // namespace unordlist

#endif // #ifndef CDSTEST_HDR_LAZY_KV_H
