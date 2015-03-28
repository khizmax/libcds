//$$CDS-header$$

#ifndef CDSTEST_HDR_INTRUSIVE_LAZY_H
#define CDSTEST_HDR_INTRUSIVE_LAZY_H

#include "cppunit/cppunit_proxy.h"
#include <cds/intrusive/details/lazy_list_base.h>

namespace unordlist {
    namespace ci = cds::intrusive;
    namespace co = cds::opt;

    struct stat {
        int nDisposeCount;
        int nEnsureExistsCall;
        int nEnsureNewCall;
        int nFindCall;
        int nEraseCall;

        stat()
            : nDisposeCount(0)
            , nEnsureExistsCall(0)
            , nEnsureNewCall(0)
            , nFindCall(0)
            , nEraseCall(0)
        {}

        stat( const stat& s )
        {
            *this = s;
        }

        stat& operator =(const stat& s)
        {
            memcpy( this, &s, sizeof(s));
            return *this;
        }
    };

    template <typename GC>
    struct base_int_item: public ci::lazy_list::node< GC >
    {
        int nKey;
        int nVal;

        mutable stat    s;

        base_int_item()
        {}

        base_int_item(int key, int val)
            : nKey( key )
            , nVal(val)
            , s()
        {}

        base_int_item(const base_int_item& v )
            : nKey( v.nKey )
            , nVal( v.nVal )
            , s()
        {}

        const int& key() const
        {
            return nKey;
        }

        operator int() const
        { return nKey; }
    };

    template <typename GC>
    struct member_int_item
    {
        int nKey;
        int nVal;

        ci::lazy_list::node< GC > hMember;

        mutable stat s;

        member_int_item()
        {}

        member_int_item(int key, int val)
            : nKey( key )
            , nVal(val)
            , s()
        {}

        member_int_item(const member_int_item& v )
            : nKey( v.nKey )
            , nVal( v.nVal )
            , s()
        {}

        const int& key() const
        {
            return nKey;
        }

        operator int() const
        { return nKey; }
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
        bool operator()( T const& l, T const& r ) const
        {
            return l.key() == r.key();
        }

        template <typename Q>
        bool operator()( Q const& l, T const& r ) const
        {
            return l == r.key();
        }

        template <typename Q>
        bool operator()( T const& l, Q const& r ) const
        {
            return l.key() == r;
        }
    };

    struct faked_disposer
    {
        template <typename T>
        void operator ()( T * p )
        {
            ++p->s.nDisposeCount;
        }
    };

    struct ensure_functor
    {
        template <typename T>
        void operator ()(bool bNew, T& item, T& /*val*/ )
        {
            if ( bNew )
                ++item.s.nEnsureNewCall;
            else
                ++item.s.nEnsureExistsCall;
        }
    };

    struct find_functor
    {
        template <typename T, typename Q>
        void operator ()( T& item, Q& /*val*/ )
        {
            ++item.s.nFindCall;
        }
    };

    class UnorderedIntrusiveLazyListHeaderTest: public CppUnitMini::TestCase
    {
    public:
        template <class UnordList>
        void test_nogc_int()
        {
            typedef typename UnordList::value_type    value_type;
            {
                value_type v1( 10, 50 );
                value_type v2( 5, 25  );
                value_type v3( 20, 100 );
                {
                    UnordList l;
                    CPPUNIT_ASSERT( l.empty() );

                    CPPUNIT_ASSERT( l.insert( v1 ));   // true
                    CPPUNIT_ASSERT( l.find( v1.key() ) == &v1 );

                    CPPUNIT_ASSERT( v1.s.nFindCall == 0 );
                    CPPUNIT_ASSERT( l.find( v1.key(), find_functor() ));
                    CPPUNIT_ASSERT( v1.s.nFindCall == 1 );

                    CPPUNIT_ASSERT( l.find_with( v2.key(), equal_to<value_type>() ) == nullptr );
                    CPPUNIT_ASSERT( l.find( v3.key() ) == nullptr );
                    CPPUNIT_ASSERT( !l.empty() );

                    //CPPUNIT_ASSERT( !l.insert( v1 ))    ;   // assertion "is_empty" is raised

                    {
                        value_type v( v1 );
                        CPPUNIT_ASSERT( !l.insert( v )) ;   // false
                    }

                    std::pair<bool, bool> ret = l.ensure( v2, ensure_functor() );
                    CPPUNIT_ASSERT( ret.first );
                    CPPUNIT_ASSERT( ret.second );
                    CPPUNIT_ASSERT( v2.s.nEnsureNewCall == 1 );
                    CPPUNIT_ASSERT( v2.s.nEnsureExistsCall == 0 );

                    //CPPUNIT_ASSERT( !l.insert( v2 ))    ;   // assertion "is_empty"

                    CPPUNIT_ASSERT( l.find( v1.key() ) == &v1 ) ;   // true

                    CPPUNIT_ASSERT( v1.s.nFindCall == 1 );
                    CPPUNIT_ASSERT( l.find( v1.key(), find_functor() ));
                    CPPUNIT_ASSERT( v1.s.nFindCall == 2 );

                    CPPUNIT_ASSERT( l.find_with( v2.key(), equal_to<value_type>() ) == &v2 );

                    CPPUNIT_ASSERT( v2.s.nFindCall == 0 );
                    CPPUNIT_ASSERT( l.find_with( v2.key(), equal_to<value_type>(), find_functor() ));
                    CPPUNIT_ASSERT( v2.s.nFindCall == 1 );

                    CPPUNIT_ASSERT( !l.find( v3.key() ));

                    {
                        value_type v( v2 );
                        ret = l.ensure( v, ensure_functor() );

                        CPPUNIT_ASSERT( ret.first );
                        CPPUNIT_ASSERT( !ret.second );
                        CPPUNIT_ASSERT( v2.s.nEnsureExistsCall == 1 );
                        CPPUNIT_ASSERT( v.s.nEnsureExistsCall == 0 && v.s.nEnsureNewCall == 0 );
                    }

                    CPPUNIT_ASSERT( !l.empty() );

                    CPPUNIT_ASSERT( l.insert( v3 ))     ;   // true
                    CPPUNIT_ASSERT( l.find( v3.key() ) == &v3 );

                    CPPUNIT_ASSERT( v3.s.nFindCall == 0 );
                    CPPUNIT_ASSERT( l.find( v3.key(), find_functor() ));
                    CPPUNIT_ASSERT( v3.s.nFindCall == 1 );

                    {
                        typename UnordList::iterator it = l.begin();
                        typename UnordList::const_iterator cit = l.cbegin();
                        CPPUNIT_ASSERT( it != l.end() );
                        CPPUNIT_ASSERT( it != l.cend() );
                        CPPUNIT_ASSERT( cit != l.end() );
                        CPPUNIT_ASSERT( cit != l.cend() );
                        CPPUNIT_ASSERT( cit == it );

                        CPPUNIT_ASSERT( it->nKey == v1.nKey );
                        CPPUNIT_ASSERT( it->nVal == v1.nVal );
                        CPPUNIT_ASSERT( ++it != l.end() );
                        CPPUNIT_ASSERT( it->nKey == v2.nKey );
                        CPPUNIT_ASSERT( it->nVal == v2.nVal );
                        CPPUNIT_ASSERT( it++ != l.end() );
                        CPPUNIT_ASSERT( it->nKey == v3.nKey );
                        CPPUNIT_ASSERT( it->nVal == v3.nVal );
                        CPPUNIT_ASSERT( it++ != l.end() );
                        CPPUNIT_ASSERT( it == l.end() );
                    }

                    {
                        UnordList const & lref = l;
                        typename UnordList::const_iterator it = lref.begin();
                        CPPUNIT_ASSERT( it != l.end() );
                        CPPUNIT_ASSERT( it->nKey == v1.nKey );
                        CPPUNIT_ASSERT( it->nVal == v1.nVal );
                        CPPUNIT_ASSERT( ++it != lref.end() );
                        CPPUNIT_ASSERT( it->nKey == v2.nKey );
                        CPPUNIT_ASSERT( it->nVal == v2.nVal );
                        CPPUNIT_ASSERT( it++ != l.end() );
                        CPPUNIT_ASSERT( it->nKey == v3.nKey );
                        CPPUNIT_ASSERT( it->nVal == v3.nVal );
                        CPPUNIT_ASSERT( it++ != lref.end() );
                        CPPUNIT_ASSERT( it == l.end() );
                    }
                }

                // Disposer called on list destruction
                CPPUNIT_ASSERT( v1.s.nDisposeCount == 1 );
                CPPUNIT_ASSERT( v2.s.nDisposeCount == 1 );
                CPPUNIT_ASSERT( v3.s.nDisposeCount == 1 );
            }
        }

        void nogc_base_cmp();
        void nogc_base_less();
        void nogc_base_equal_to();
        void nogc_base_cmpmix();
        void nogc_base_equal_to_mix();
        void nogc_base_ic();
        void nogc_member_cmp();
        void nogc_member_less();
        void nogc_member_equal_to();
        void nogc_member_cmpmix();
        void nogc_member_equal_to_mix();
        void nogc_member_ic();

        CPPUNIT_TEST_SUITE(UnorderedIntrusiveLazyListHeaderTest)

            CPPUNIT_TEST(nogc_base_cmp)
            CPPUNIT_TEST(nogc_base_less)
            CPPUNIT_TEST(nogc_base_equal_to)
            CPPUNIT_TEST(nogc_base_cmpmix)
            CPPUNIT_TEST(nogc_base_equal_to_mix)
            CPPUNIT_TEST(nogc_base_ic)
            CPPUNIT_TEST(nogc_member_cmp)
            CPPUNIT_TEST(nogc_member_less)
            CPPUNIT_TEST(nogc_member_equal_to)
            CPPUNIT_TEST(nogc_member_cmpmix)
            CPPUNIT_TEST(nogc_member_equal_to_mix)
            CPPUNIT_TEST(nogc_member_ic)

        CPPUNIT_TEST_SUITE_END()
    };
}   // namespace unordlist

#endif // #ifndef CDSTEST_HDR_INTRUSIVE_LAZY_H
