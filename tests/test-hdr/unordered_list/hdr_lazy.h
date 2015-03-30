//$$CDS-header$$

#ifndef CDSTEST_HDR_LAZY_H
#define CDSTEST_HDR_LAZY_H

#include "cppunit/cppunit_proxy.h"
#include <cds/container/details/lazy_list_base.h>

namespace unordlist {
    namespace cc = cds::container;
    namespace co = cds::container::opt;

    class UnorderedLazyListTestHeader: public CppUnitMini::TestCase
    {
    public:
        struct stat {
            int nEnsureExistsCall;
            int nEnsureNewCall;

            stat()
            {
                nEnsureExistsCall
                    = nEnsureNewCall
                    = 0;
            }
        };

        struct item {
            int     nKey;
            int     nVal;

            stat    s;

            item(int key)
                : nKey( key )
                , nVal( key * 2 )
                , s()
            {}

            item(int key, int val)
                : nKey( key )
                , nVal(val)
                , s()
            {}

            item( item const& v )
                : nKey( v.nKey )
                , nVal( v.nVal )
                , s()
            {}

            int key() const
            {
                return nKey;
            }
        };

        template <typename T>
        struct lt
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
            int operator ()(const T& v1, const T& v2 ) const
            {
                return v1.key() == v2.key();
            }

            template <typename Q>
            int operator ()(const T& v1, const Q& v2 ) const
            {
                return v1.key() == v2;
            }

            template <typename Q>
            int operator ()(const Q& v1, const T& v2 ) const
            {
                return v1 == v2.key();
            }
        };

    protected:
        template <class UnordList>
        void nogc_test()
        {
            typedef UnordList list;
            typedef typename list::value_type    value_type;
            typedef std::pair<typename list::iterator, bool> ensure_result;

            typename list::iterator it;

            list l;
            CPPUNIT_ASSERT( l.empty() );
            CPPUNIT_ASSERT( l.insert(50) != l.end() );
            CPPUNIT_ASSERT( !l.empty() );

            ensure_result eres = l.ensure( item(100, 33) );
            CPPUNIT_ASSERT( eres.second );
            CPPUNIT_ASSERT( eres.first != l.end() );
            CPPUNIT_ASSERT( l.insert( item(150) ) != l.end() );

            CPPUNIT_ASSERT( l.insert(100) == l.end() );
            eres = l.ensure( item(50, 33) );
            CPPUNIT_ASSERT( !eres.second );
            CPPUNIT_ASSERT( eres.first->nVal == eres.first->nKey * 2 );
            eres.first->nVal = 63;

            it = l.find( 33 );
            CPPUNIT_ASSERT( it == l.end() );

            it = l.find( 50 );
            CPPUNIT_ASSERT( it != l.end() );
            CPPUNIT_ASSERT( it->nKey == 50 );
            CPPUNIT_ASSERT( it->nVal == 63 );

            it = l.find( 100 );
            CPPUNIT_ASSERT( it != l.end() );
            CPPUNIT_ASSERT( it->nKey == 100 );
            CPPUNIT_ASSERT( it->nVal == 33 );

            it = l.find_with( 150, equal_to<value_type>() );
            CPPUNIT_ASSERT( it != l.end() );
            CPPUNIT_ASSERT( it->nKey == 150 );
            CPPUNIT_ASSERT( it->nVal == it->nKey * 2 );

            CPPUNIT_ASSERT( !l.empty() );
            l.clear();
            CPPUNIT_ASSERT( l.empty() );

            // insert test
            CPPUNIT_ASSERT( l.emplace( 501 ) != l.end());
            CPPUNIT_ASSERT( l.emplace( 251, 152 ) != l.end());
            CPPUNIT_ASSERT( l.emplace( item( 1001 )) != l.end());

            // insert failed - such key exists
            CPPUNIT_ASSERT( l.emplace( 501, 2 ) == l.end());
            CPPUNIT_ASSERT( l.emplace( 251, 10) == l.end());

            it = l.find( 501 );
            CPPUNIT_ASSERT( it != l.end() );
            CPPUNIT_ASSERT( it->nKey == 501 );
            CPPUNIT_ASSERT( it->nVal == 501 * 2 );

            it = l.find( 1001 );
            CPPUNIT_ASSERT( it != l.end() );
            CPPUNIT_ASSERT( it->nKey == 1001 );
            CPPUNIT_ASSERT( it->nVal == 1001 * 2 );

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


            l.clear();
            CPPUNIT_ASSERT( l.empty() );
        }

        void NOGC_cmp();
        void NOGC_less();
        void NOGC_equal_to();
        void NOGC_cmpmix();
        void NOGC_equal_to_mix();
        void NOGC_ic();

        CPPUNIT_TEST_SUITE(UnorderedLazyListTestHeader)
            CPPUNIT_TEST(NOGC_cmp)
            CPPUNIT_TEST(NOGC_less)
            CPPUNIT_TEST(NOGC_equal_to)
            CPPUNIT_TEST(NOGC_cmpmix)
            CPPUNIT_TEST(NOGC_equal_to_mix)
            CPPUNIT_TEST(NOGC_ic)
        CPPUNIT_TEST_SUITE_END()
    };

}   // namespace unordlist

#endif // #ifndef CDSTEST_HDR_LAZY_H
