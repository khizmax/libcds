//$$CDS-header$$

#ifndef CDSTEST_HDR_INTRUSIVE_BINTREE_H
#define CDSTEST_HDR_INTRUSIVE_BINTREE_H

#include "cppunit/cppunit_proxy.h"
#include "size_check.h"
#include <algorithm>

namespace tree {

    class IntrusiveBinTreeHdrTest: public CppUnitMini::TestCase
    {
    public:
        typedef int     key_type;

        struct stat_data {
            size_t  nDisposeCount;
            size_t  nWaitingDispCount;
            size_t  nInsertFuncCall;
            size_t  nEnsureExistFuncCall;
            size_t  nEnsureNewFuncCall;
            size_t  nEraseFuncCall;
            size_t  nFindFuncCall;
            size_t  nFindConstFuncCall;

            stat_data()
                : nDisposeCount(0)
                , nWaitingDispCount(0)
                , nInsertFuncCall(0)
                , nEnsureExistFuncCall(0)
                , nEnsureNewFuncCall(0)
                , nEraseFuncCall(0)
                , nFindFuncCall(0)
                , nFindConstFuncCall(0)
            {}
        };

        template <typename Hook>
        struct base_hook_value: public Hook
        {
            int     nKey;
            int     nValue;
            mutable stat_data   stat;

            base_hook_value()
            {}

            base_hook_value( int key )
                : nKey(key)
                , nValue(key * 2)
            {}

            base_hook_value( int key, int val )
                : nKey(key)
                , nValue(val)
            {}

#   ifdef _DEBUG
            base_hook_value( base_hook_value&& s )
                : Hook()
                , nKey(s.nKey)
                , nValue(s.nValue)
            {}
            base_hook_value& operator=( base_hook_value const& s )
            {
                nKey = s.nKey;
                nValue = s.nValue;
                return *this;
            }
#   endif
        };

        template <typename Hook>
        struct member_hook_value
        {
            int     nKey;
            int     nValue;
            Hook    hook;
            mutable stat_data   stat;

            member_hook_value()
            {}

            member_hook_value( int key )
                : nKey(key)
                , nValue(key * 2)
            {}

            member_hook_value( int key, int val )
                : nKey(key)
                , nValue(val)
            {}
#   ifdef _DEBUG
            member_hook_value( member_hook_value&& s )
                : nKey(s.nKey)
                , nValue(s.nValue)
                , hook()
            {}
            member_hook_value& operator=( member_hook_value const& s )
            {
                nKey = s.nKey;
                nValue = s.nValue;
                return *this;
            }
#   endif
        };

        template <typename ValueType>
        struct less {
            typedef ValueType value_type;

            bool operator()( int k1, int k2 ) const
            {
                return k1 < k2;
            }
            bool operator()( value_type const& v1, value_type const& v2 ) const
            {
                return v1.nKey < v2.nKey;
            }
            bool operator()( value_type const& v, int k ) const
            {
                return v.nKey < k;
            }
            bool operator()( int k, value_type const& v ) const
            {
                return k < v.nKey;
            }
        };

        template <typename ValueType>
        struct compare {
            typedef ValueType value_type;

            int cmp( int k1, int k2 ) const
            {
                return k1 < k2 ? -1 : (k1 > k2 ? 1 : 0);
            }
            int operator()( int k1, int k2 ) const
            {
                return cmp( k1, k2 );
            }
            int operator()( value_type const& v1, value_type const& v2 ) const
            {
                return cmp( v1.nKey, v2.nKey );
            }
            int operator()( value_type const& v, int k ) const
            {
                return cmp( v.nKey, k );
            }
            int operator()( int k, value_type const& v ) const
            {
                return cmp( k, v.nKey );
            }
        };

        struct wrapped_int {
            int  nKey;

            wrapped_int( int n )
                : nKey(n)
            {}
        };

        template <typename T>
        struct wrapped_less
        {
            bool operator()( wrapped_int const& w, int n ) const
            {
                return w.nKey < n;
            }
            bool operator()( int n, wrapped_int const& w ) const
            {
                return n < w.nKey;
            }
            bool operator()( wrapped_int const& w, T const& v ) const
            {
                return w.nKey < v.nKey;
            }
            bool operator()( T const& v, wrapped_int const& w ) const
            {
                return v.nKey < w.nKey;
            }
        };

        template <typename ValueType>
        struct key_extractor {
            void operator()( int& dest, ValueType const& src ) const
            {
                dest = src.nKey;
            }
        };

        template <typename ValueType>
        struct disposer {
            void operator()( ValueType * v ) const
            {
                ++v->stat.nDisposeCount;
            }
        };

        struct insert_functor {
            template <typename T>
            void operator()( T& v ) const
            {
                ++v.stat.nInsertFuncCall;
            }
        };

        struct ensure_functor {
            template <typename T>
            void operator()( bool bNew, T& dest, T& src) const
            {
                if ( bNew )
                    ++dest.stat.nEnsureNewFuncCall;
                else {
                    dest.nValue *= 2;
                    ++src.stat.nEnsureExistFuncCall;
                }
            }
        };

        struct erase_functor {
            template <typename T>
            void operator()( T const& v ) const
            {
                ++v.stat.nEraseFuncCall;
            }
        };

        struct find_functor {
            template <typename T, typename Q>
            void operator()( T const& v, Q& /*q*/ ) const
            {
                ++v.stat.nFindFuncCall;
            }
            template <typename T, typename Q>
            void operator()( T const& v, Q const& /*q*/ ) const
            {
                ++v.stat.nFindConstFuncCall;
            }
        };

    protected:
        static const size_t c_nItemCount = 10000;

        template <typename T>
        class data_array
        {
            T *     pFirst;
            T *     pLast;

        public:
            data_array()
                : pFirst( new T[c_nItemCount] )
                , pLast( pFirst + c_nItemCount )
            {
                int i = 0;
                for ( T * p = pFirst; p != pLast; ++p, ++i ) {
                    p->nKey = i;
                    p->nValue = i * 2;
                }

                shuffle( pFirst, pLast );
            }

            ~data_array()
            {
                delete [] pFirst;
            }

            T * begin() { return pFirst; }
            T * end()   { return pLast ; }
        };

    protected:
        template <typename Tree>
        void test_common( Tree& t )
        {
            typedef Tree tree_type;
            typedef typename tree_type::key_type     key_type;
            typedef typename tree_type::value_type   value_type;

            {
                value_type v1( 10, 100 );
                value_type v2( 20, 200 );
                value_type v3( 30, 300 );
                value_type v4( 25, 250 );
                value_type v5( -50, -500 );

                // insert/ensure
                CPPUNIT_ASSERT( t.empty() );
                CPPUNIT_ASSERT( misc::check_size( t, 0 ));
                CPPUNIT_CHECK( !t.find( v1.nKey ));
                CPPUNIT_CHECK( !t.find( v1 ));
                CPPUNIT_CHECK( !t.find( v2.nKey ));
                CPPUNIT_CHECK( !t.find( v2 ));
                CPPUNIT_CHECK( !t.find( v3.nKey ));
                CPPUNIT_CHECK( !t.find( v3 ));
                CPPUNIT_CHECK( !t.find( v4.nKey ));
                CPPUNIT_CHECK( !t.find( v4 ));
                CPPUNIT_CHECK( !t.find( v5.nKey ));
                CPPUNIT_CHECK( !t.find( v5 ));

                CPPUNIT_ASSERT( t.insert( v1 ));
                CPPUNIT_ASSERT( t.check_consistency() );
                CPPUNIT_ASSERT( !t.empty() );
                CPPUNIT_ASSERT( misc::check_size( t, 1 ));
                CPPUNIT_CHECK( t.find( v1.nKey ));
                CPPUNIT_CHECK( t.find( v1 ));
                CPPUNIT_CHECK( !t.find( v2.nKey ));
                CPPUNIT_CHECK( !t.find( v2 ));
                CPPUNIT_CHECK( !t.find( v3.nKey ));
                CPPUNIT_CHECK( !t.find( v3 ));
                CPPUNIT_CHECK( !t.find( v4.nKey ));
                CPPUNIT_CHECK( !t.find( v4 ));
                CPPUNIT_CHECK( !t.find( v5.nKey ));
                CPPUNIT_CHECK( !t.find( v5 ));

                CPPUNIT_ASSERT( v2.stat.nInsertFuncCall == 0 );
                CPPUNIT_ASSERT( t.insert( v2, insert_functor() ));
                CPPUNIT_ASSERT( t.check_consistency() );
                CPPUNIT_ASSERT( v2.stat.nInsertFuncCall == 1 );
                CPPUNIT_ASSERT( t.find( v1.nKey ));
                CPPUNIT_ASSERT( t.find( v1 ));
                CPPUNIT_ASSERT( t.find( v2.nKey ));
                CPPUNIT_ASSERT( t.find( v2 ));
                CPPUNIT_ASSERT( !t.find( v3.nKey ));
                CPPUNIT_ASSERT( !t.find( v3 ));
                CPPUNIT_ASSERT( !t.find( v4.nKey ));
                CPPUNIT_ASSERT( !t.find( v4 ));
                CPPUNIT_ASSERT( !t.find( v5.nKey ));
                CPPUNIT_ASSERT( !t.find( v5 ));
                CPPUNIT_ASSERT( !t.empty() );
                CPPUNIT_ASSERT( misc::check_size( t, 2 ));

                CPPUNIT_ASSERT( v3.stat.nEnsureNewFuncCall == 0 );
                CPPUNIT_ASSERT( v3.stat.nEnsureExistFuncCall == 0 );
                CPPUNIT_ASSERT( t.ensure( v3, ensure_functor() ).second );
                CPPUNIT_ASSERT( t.check_consistency() );
                CPPUNIT_ASSERT( v3.stat.nEnsureNewFuncCall == 1 );
                CPPUNIT_ASSERT( v3.stat.nEnsureExistFuncCall == 0 );
                CPPUNIT_ASSERT( v3.nValue == 300 );
                CPPUNIT_ASSERT( !t.ensure( v3, ensure_functor() ).second );
                CPPUNIT_ASSERT( v3.stat.nEnsureNewFuncCall == 1 );
                CPPUNIT_ASSERT( v3.stat.nEnsureExistFuncCall == 1 );
                CPPUNIT_ASSERT( v3.nValue == 600 );
                CPPUNIT_ASSERT( t.find( v1.nKey ));
                CPPUNIT_ASSERT( t.find( v1 ));
                CPPUNIT_ASSERT( t.find( v2.nKey ));
                CPPUNIT_ASSERT( t.find( v2 ));
                CPPUNIT_ASSERT( t.find( v3.nKey ));
                CPPUNIT_ASSERT( t.find( v3 ));
                CPPUNIT_ASSERT( !t.find( v4.nKey ));
                CPPUNIT_ASSERT( !t.find( v4 ));
                CPPUNIT_ASSERT( !t.find( v5.nKey ));
                CPPUNIT_ASSERT( !t.find( v5 ));
                CPPUNIT_ASSERT( !t.empty() );
                CPPUNIT_ASSERT( misc::check_size( t, 3 ));

                {
                    value_type v( v3.nKey, v3.nValue );
                    CPPUNIT_ASSERT( v.stat.nEnsureExistFuncCall == 0 );
                    CPPUNIT_ASSERT( v3.stat.nEnsureNewFuncCall == 1 );
                    CPPUNIT_ASSERT( v3.stat.nEnsureExistFuncCall == 1 );
                    CPPUNIT_ASSERT( v3.nValue == 600 );
                    CPPUNIT_ASSERT( !t.ensure( v, ensure_functor() ).second );
                    CPPUNIT_ASSERT( v3.stat.nEnsureNewFuncCall == 1 );
                    CPPUNIT_ASSERT( v.stat.nEnsureExistFuncCall == 1 );
                    CPPUNIT_ASSERT( v3.nValue == 1200 );

                }
                v3.nValue = 300;
                CPPUNIT_ASSERT( !t.insert( v3 ));

                CPPUNIT_ASSERT( t.insert( v4 ));
                CPPUNIT_ASSERT( t.check_consistency() );
                CPPUNIT_ASSERT( t.find( v1.nKey ));
                CPPUNIT_ASSERT( t.find( v1 ));
                CPPUNIT_ASSERT( t.find( v2.nKey ));
                CPPUNIT_ASSERT( t.find( v2 ));
                CPPUNIT_ASSERT( t.find( v3.nKey ));
                CPPUNIT_ASSERT( t.find( v3 ));
                CPPUNIT_ASSERT( t.find( v4.nKey ));
                CPPUNIT_ASSERT( t.find( v4 ));
                CPPUNIT_ASSERT( !t.find( v5.nKey ));
                CPPUNIT_ASSERT( !t.find( v5 ));
                CPPUNIT_ASSERT( !t.empty() );
                CPPUNIT_ASSERT( misc::check_size( t, 4 ));

                CPPUNIT_ASSERT( t.insert( v5 ));
                CPPUNIT_ASSERT( t.check_consistency() );
                CPPUNIT_ASSERT( t.find( v1.nKey ));
                CPPUNIT_ASSERT( t.find( v1 ));
                CPPUNIT_ASSERT( t.find( v2.nKey ));
                CPPUNIT_ASSERT( t.find( v2 ));
                CPPUNIT_ASSERT( t.find( v3.nKey ));
                CPPUNIT_ASSERT( t.find( v3 ));
                CPPUNIT_ASSERT( t.find( v4.nKey ));
                CPPUNIT_ASSERT( t.find( v4 ));
                CPPUNIT_ASSERT( t.find( v5.nKey ));
                CPPUNIT_ASSERT( t.find( v5 ));
                CPPUNIT_ASSERT( !t.empty() );
                CPPUNIT_ASSERT( misc::check_size( t, 5 ));

                //unlink/erase
                ++v1.stat.nWaitingDispCount;
                CPPUNIT_ASSERT( t.unlink(v1));
                CPPUNIT_ASSERT( t.check_consistency() );
                CPPUNIT_ASSERT( !t.find( v1.nKey ));
                CPPUNIT_ASSERT( !t.find( v1 ));
                CPPUNIT_ASSERT( !t.unlink(v1));
                CPPUNIT_ASSERT( t.find( v2.nKey ));
                CPPUNIT_ASSERT( t.find( v2 ));
                CPPUNIT_ASSERT( t.find( v3.nKey ));
                CPPUNIT_ASSERT( t.find( v3 ));
                CPPUNIT_ASSERT( t.find( v4.nKey ));
                CPPUNIT_ASSERT( t.find( v4 ));
                CPPUNIT_ASSERT( t.find( v5.nKey ));
                CPPUNIT_ASSERT( t.find( v5 ));
                CPPUNIT_ASSERT( !t.empty() );
                CPPUNIT_ASSERT( misc::check_size( t, 4 ));

                ++v2.stat.nWaitingDispCount;
                CPPUNIT_ASSERT( t.erase( v2.nKey ));
                CPPUNIT_ASSERT( t.check_consistency() );
                CPPUNIT_ASSERT( !t.find( v1.nKey ));
                CPPUNIT_ASSERT( !t.find( v1 ));
                CPPUNIT_ASSERT( !t.find( v2.nKey ));
                CPPUNIT_ASSERT( !t.find( v2 ));
                CPPUNIT_ASSERT( !t.erase(v2));
                CPPUNIT_ASSERT( t.find( v3.nKey ));
                CPPUNIT_ASSERT( t.find( v3 ));
                CPPUNIT_ASSERT( t.find( v4.nKey ));
                CPPUNIT_ASSERT( t.find( v4 ));
                CPPUNIT_ASSERT( t.find( v5.nKey ));
                CPPUNIT_ASSERT( t.find( v5 ));
                CPPUNIT_ASSERT( !t.empty() );
                CPPUNIT_ASSERT( misc::check_size( t, 3 ));

                ++v3.stat.nWaitingDispCount;
                CPPUNIT_ASSERT( t.erase_with( v3.nKey, less<value_type>() ));
                CPPUNIT_ASSERT( t.check_consistency() );
                CPPUNIT_ASSERT( !t.find( v1.nKey ));
                CPPUNIT_ASSERT( !t.find( v1 ));
                CPPUNIT_ASSERT( !t.find( v2.nKey ));
                CPPUNIT_ASSERT( !t.find( v2 ));
                CPPUNIT_ASSERT( !t.find( v3.nKey ));
                CPPUNIT_ASSERT( !t.find( v3 ));
                CPPUNIT_ASSERT( !t.erase_with(v3, less<value_type>() ));
                CPPUNIT_ASSERT( t.find( v4.nKey ));
                CPPUNIT_ASSERT( t.find( v4 ));
                CPPUNIT_ASSERT( t.find( v5.nKey ));
                CPPUNIT_ASSERT( t.find( v5 ));
                CPPUNIT_ASSERT( !t.empty() );
                CPPUNIT_ASSERT( misc::check_size( t, 2 ));

                ++v4.stat.nWaitingDispCount;
                CPPUNIT_ASSERT( v4.stat.nEraseFuncCall == 0 );
                CPPUNIT_ASSERT( t.erase( v4.nKey, erase_functor() ));
                CPPUNIT_ASSERT( t.check_consistency() );
                CPPUNIT_ASSERT( v4.stat.nEraseFuncCall == 1 );
                CPPUNIT_ASSERT( !t.find( v1.nKey ));
                CPPUNIT_ASSERT( !t.find( v1 ));
                CPPUNIT_ASSERT( !t.find( v2.nKey ));
                CPPUNIT_ASSERT( !t.find( v2 ));
                CPPUNIT_ASSERT( !t.find( v3.nKey ));
                CPPUNIT_ASSERT( !t.find( v3 ));
                CPPUNIT_ASSERT( !t.find( v4.nKey ));
                CPPUNIT_ASSERT( !t.find( v4 ));
                CPPUNIT_ASSERT( !t.erase( v4.nKey, erase_functor() ));
                CPPUNIT_ASSERT( v4.stat.nEraseFuncCall == 1 );
                CPPUNIT_ASSERT( t.find( v5.nKey ));
                CPPUNIT_ASSERT( t.find( v5 ));
                CPPUNIT_ASSERT( !t.empty() );
                CPPUNIT_ASSERT( misc::check_size( t, 1 ));

                ++v5.stat.nWaitingDispCount;
                CPPUNIT_ASSERT( t.erase_with( v5.nKey, less<value_type>(), erase_functor() ));
                CPPUNIT_ASSERT( t.check_consistency() );
                CPPUNIT_ASSERT( v5.stat.nEraseFuncCall == 1 );
                CPPUNIT_ASSERT( !t.find( v1.nKey ));
                CPPUNIT_ASSERT( !t.find( v1 ));
                CPPUNIT_ASSERT( !t.find( v2.nKey ));
                CPPUNIT_ASSERT( !t.find( v2 ));
                CPPUNIT_ASSERT( !t.find( v3.nKey ));
                CPPUNIT_ASSERT( !t.find( v3 ));
                CPPUNIT_ASSERT( !t.erase_with(v5, less<value_type>(), erase_functor() ));
                CPPUNIT_ASSERT( !t.find( v4.nKey ));
                CPPUNIT_ASSERT( !t.find( v4 ));
                CPPUNIT_ASSERT( !t.find( v5.nKey ));
                CPPUNIT_ASSERT( !t.find( v5 ));
                CPPUNIT_ASSERT( t.empty() );
                CPPUNIT_ASSERT( misc::check_size( t, 0 ));

                tree_type::gc::force_dispose();

                // find
                CPPUNIT_ASSERT( t.insert( v1 ));
                CPPUNIT_ASSERT( t.insert( v2 ));
                CPPUNIT_ASSERT( t.insert( v3 ));
                CPPUNIT_ASSERT( t.insert( v4 ));
                CPPUNIT_ASSERT( t.insert( v5 ));
                CPPUNIT_ASSERT( t.check_consistency() );
                CPPUNIT_ASSERT( !t.empty() );
                CPPUNIT_ASSERT( misc::check_size( t, 5 ));

                CPPUNIT_ASSERT( t.find( 10 ));
                CPPUNIT_ASSERT( !t.find( 11 ));
                CPPUNIT_ASSERT( t.find( v1 ));
                CPPUNIT_ASSERT( t.find( v2.nKey ));
                CPPUNIT_ASSERT( t.find( v3 ));
                CPPUNIT_ASSERT( t.find( v4.nKey ));
                CPPUNIT_ASSERT( t.find( v5.nKey ));

                // find_with
                CPPUNIT_ASSERT( t.find_with( 10, less<value_type>() ));
                CPPUNIT_ASSERT( !t.find_with( wrapped_int(11), wrapped_less<value_type>() ));
                CPPUNIT_ASSERT( t.find_with( v1, less<value_type>() ));
                CPPUNIT_ASSERT( t.find_with( wrapped_int(v2.nKey), wrapped_less<value_type>() ));
                CPPUNIT_ASSERT( t.find_with( v3, less<value_type>() ));
                CPPUNIT_ASSERT( t.find_with( v4.nKey, less<value_type>() ));
                CPPUNIT_ASSERT( t.find_with( v5.nKey, less<value_type>() ));

                // find<Func>
                CPPUNIT_ASSERT( v1.stat.nFindFuncCall == 0 );
                CPPUNIT_ASSERT( v1.stat.nFindConstFuncCall == 0 );
                CPPUNIT_ASSERT( t.find( 10, find_functor() ));
                CPPUNIT_ASSERT( v1.stat.nFindFuncCall == 0 );
                CPPUNIT_ASSERT( v1.stat.nFindConstFuncCall == 1 );

                CPPUNIT_ASSERT( !t.find( 11, find_functor() ));

                CPPUNIT_ASSERT( t.find( v1, find_functor() ));
                CPPUNIT_ASSERT( v1.stat.nFindFuncCall == 1 );
                CPPUNIT_ASSERT( v1.stat.nFindConstFuncCall == 1 );

                CPPUNIT_ASSERT( v2.stat.nFindFuncCall == 0 );
                CPPUNIT_ASSERT( v2.stat.nFindConstFuncCall == 0 );
                CPPUNIT_ASSERT( t.find( v2.nKey, find_functor() ));
                CPPUNIT_ASSERT( v2.stat.nFindFuncCall == 1 );
                CPPUNIT_ASSERT( v2.stat.nFindConstFuncCall == 0 );

                CPPUNIT_ASSERT( v3.stat.nFindFuncCall == 0 );
                CPPUNIT_ASSERT( v3.stat.nFindConstFuncCall == 0 );
                CPPUNIT_ASSERT( t.find( v3, find_functor() ));
                CPPUNIT_ASSERT( v3.stat.nFindFuncCall == 1 );
                CPPUNIT_ASSERT( v3.stat.nFindConstFuncCall == 0 );

                CPPUNIT_ASSERT( v4.stat.nFindFuncCall == 0 );
                CPPUNIT_ASSERT( v4.stat.nFindConstFuncCall == 0 );
                CPPUNIT_ASSERT( t.find( (value_type const&) v4, find_functor() ));
                CPPUNIT_ASSERT( v4.stat.nFindFuncCall == 0 );
                CPPUNIT_ASSERT( v4.stat.nFindConstFuncCall == 1 );

                CPPUNIT_ASSERT( v5.stat.nFindFuncCall == 0 );
                CPPUNIT_ASSERT( v5.stat.nFindConstFuncCall == 0 );
                CPPUNIT_ASSERT( t.find( v5.nKey, find_functor() ));
                CPPUNIT_ASSERT( v5.stat.nFindFuncCall == 1 );
                CPPUNIT_ASSERT( v5.stat.nFindConstFuncCall == 0 );

                // find_with<Func>
                CPPUNIT_ASSERT( v1.stat.nFindFuncCall == 1 );
                CPPUNIT_ASSERT( v1.stat.nFindConstFuncCall == 1 );
                CPPUNIT_ASSERT( t.find_with( 10, less<value_type>(), find_functor() ));
                CPPUNIT_ASSERT( v1.stat.nFindFuncCall == 1 );
                CPPUNIT_ASSERT( v1.stat.nFindConstFuncCall == 2 );

                CPPUNIT_ASSERT( !t.find_with( 11, less<value_type>(), find_functor() ));

                CPPUNIT_ASSERT( t.find_with( v1, less<value_type>(), find_functor() ));
                CPPUNIT_ASSERT( v1.stat.nFindFuncCall == 2 );
                CPPUNIT_ASSERT( v1.stat.nFindConstFuncCall == 2 );

                CPPUNIT_ASSERT( v2.stat.nFindFuncCall == 1 );
                CPPUNIT_ASSERT( v2.stat.nFindConstFuncCall == 0 );
                CPPUNIT_ASSERT( t.find_with( v2.nKey, less<value_type>(), find_functor() ));
                CPPUNIT_ASSERT( v2.stat.nFindFuncCall == 2 );
                CPPUNIT_ASSERT( v2.stat.nFindConstFuncCall == 0 );

                CPPUNIT_ASSERT( v3.stat.nFindFuncCall == 1 );
                CPPUNIT_ASSERT( v3.stat.nFindConstFuncCall == 0 );
                CPPUNIT_ASSERT( t.find_with( wrapped_int(v3.nKey), wrapped_less<value_type>(), find_functor() ));
                CPPUNIT_ASSERT( v3.stat.nFindFuncCall == 1 );
                CPPUNIT_ASSERT( v3.stat.nFindConstFuncCall == 1 );

                CPPUNIT_ASSERT( v4.stat.nFindFuncCall == 0 );
                CPPUNIT_ASSERT( v4.stat.nFindConstFuncCall == 1 );
                CPPUNIT_ASSERT( t.find_with( (value_type const&) v4, less<value_type>(), find_functor() ));
                CPPUNIT_ASSERT( v4.stat.nFindFuncCall == 0 );
                CPPUNIT_ASSERT( v4.stat.nFindConstFuncCall == 2 );

                CPPUNIT_ASSERT( v5.stat.nFindFuncCall == 1 );
                CPPUNIT_ASSERT( v5.stat.nFindConstFuncCall == 0 );
                CPPUNIT_ASSERT( t.find_with( v5.nKey, less<value_type>(), find_functor() ));
                CPPUNIT_ASSERT( v5.stat.nFindFuncCall == 2 );
                CPPUNIT_ASSERT( v5.stat.nFindConstFuncCall == 0 );

                CPPUNIT_ASSERT( t.check_consistency() );
                t.clear();
                CPPUNIT_ASSERT( t.check_consistency() );
                CPPUNIT_ASSERT( t.empty() );
                CPPUNIT_ASSERT( misc::check_size( t, 0 ));

                tree_type::gc::force_dispose();
                CPPUNIT_CHECK_EX( v1.stat.nWaitingDispCount + 1 == v1.stat.nDisposeCount,
                    "v1.stat.nWaitingDispCount=" << v1.stat.nWaitingDispCount << ", v1.stat.nDisposeCount=" << v1.stat.nDisposeCount );
                CPPUNIT_CHECK_EX( v2.stat.nWaitingDispCount + 1 == v2.stat.nDisposeCount,
                    "v2.stat.nWaitingDispCount=" << v2.stat.nWaitingDispCount << ", v2.stat.nDisposeCount=" << v2.stat.nDisposeCount );
                CPPUNIT_CHECK_EX( v3.stat.nWaitingDispCount + 1 == v3.stat.nDisposeCount,
                    "v3.stat.nWaitingDispCount=" << v3.stat.nWaitingDispCount << ", v3.stat.nDisposeCount=" << v3.stat.nDisposeCount );
                CPPUNIT_CHECK_EX( v4.stat.nWaitingDispCount + 1 == v4.stat.nDisposeCount,
                    "v4.stat.nWaitingDispCount=" << v4.stat.nWaitingDispCount << ", v4.stat.nDisposeCount=" << v4.stat.nDisposeCount );
                CPPUNIT_CHECK_EX( v5.stat.nWaitingDispCount + 1 == v5.stat.nDisposeCount,
                    "v5.stat.nWaitingDispCount=" << v5.stat.nWaitingDispCount << ", v5.stat.nDisposeCount=" << v5.stat.nDisposeCount );
            }

            {
                data_array< value_type> arr;
                value_type * pFirst = arr.begin();
                value_type * pLast  = arr.end();

                for ( value_type * p = pFirst; p != pLast; ++p ) {
                    CPPUNIT_ASSERT( t.insert( *p ) );
                    CPPUNIT_ASSERT( !t.insert( *p ));
                }
                CPPUNIT_ASSERT( !t.empty() );
                CPPUNIT_ASSERT( misc::check_size( t, c_nItemCount ));
                CPPUNIT_ASSERT( t.check_consistency() );

                for ( int n = 0; n < (int) c_nItemCount; ++n ) {
                    CPPUNIT_ASSERT_MSG( t.find( n ), n );
                }
                for ( value_type * p = pFirst; p != pLast; ++p ) {
                    CPPUNIT_ASSERT( t.find( *p ));
                    CPPUNIT_ASSERT( t.find( p->nKey ));
                    CPPUNIT_ASSERT( t.unlink( *p ) );
                    CPPUNIT_ASSERT( !t.unlink( *p ) );
                    CPPUNIT_ASSERT( !t.find( p->nKey ));
                }

                tree_type::gc::force_dispose();
            }
        }

        template <class Tree, class PrintStat>
        void test()
        {
            typedef Tree tree_type;
            typedef typename tree_type::key_type     key_type;
            typedef typename tree_type::value_type   value_type;

            tree_type t;
            test_common(t);

            {
                value_type v1( 10, 100 );
                value_type v2( 20, 200 );
                value_type v3( 30, 300 );
                value_type v4( 25, 250 );
                value_type v5( -50, -500 );

                // extract/extract_min/extract_max
                CPPUNIT_ASSERT( t.insert( v1 ));
                CPPUNIT_ASSERT( t.insert( v2 ));
                CPPUNIT_ASSERT( t.insert( v3 ));
                CPPUNIT_ASSERT( t.insert( v4 ));
                CPPUNIT_ASSERT( t.insert( v5 ));
                CPPUNIT_ASSERT( t.check_consistency() );
                CPPUNIT_ASSERT( !t.empty() );
                CPPUNIT_ASSERT( misc::check_size( t, 5 ));

                {
                    typename tree_type::guarded_ptr gp;

                    gp = t.get( v2.nKey );
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->nKey == v2.nKey );
                    gp = t.extract( v2.nKey );
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_ASSERT( t.check_consistency() );
                    CPPUNIT_ASSERT( !t.empty() );
                    CPPUNIT_ASSERT( misc::check_size( t, 4 ));
                    CPPUNIT_ASSERT( gp->nKey == v2.nKey );
                    gp = t.extract( v2.nKey );
                    CPPUNIT_ASSERT( !gp );
                    gp = t.get( v2.nKey );
                    CPPUNIT_CHECK( !gp );
                    CPPUNIT_ASSERT( t.check_consistency() );
                    CPPUNIT_ASSERT( !t.empty() );
                    CPPUNIT_ASSERT( misc::check_size( t, 4 ));

                    gp = t.extract_min();
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_ASSERT( t.check_consistency() );
                    CPPUNIT_ASSERT( !t.empty() );
                    CPPUNIT_ASSERT( misc::check_size( t, 3 ));
                    CPPUNIT_ASSERT( gp->nKey == v5.nKey );

                    gp = t.extract_min();
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( t.check_consistency() );
                    CPPUNIT_ASSERT( !t.empty() );
                    CPPUNIT_ASSERT( misc::check_size( t, 2 ));
                    CPPUNIT_ASSERT( gp->nKey == v1.nKey );

                    gp = t.extract_min();
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( t.check_consistency() );
                    CPPUNIT_ASSERT( !t.empty() );
                    CPPUNIT_ASSERT( misc::check_size( t, 1 ));
                    CPPUNIT_ASSERT( gp->nKey == v4.nKey );

                    gp = t.extract_min();
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( t.check_consistency() );
                    CPPUNIT_ASSERT( t.empty() );
                    CPPUNIT_ASSERT( misc::check_size( t, 0 ));
                    CPPUNIT_ASSERT( gp->nKey == v3.nKey );

                    gp = t.extract_min();
                    CPPUNIT_ASSERT( !gp );
                    CPPUNIT_ASSERT( !t.extract_max());
                    CPPUNIT_ASSERT( !t.extract( v1.nKey ));
                }

                tree_type::gc::force_dispose();

                {
                    typename tree_type::guarded_ptr gp;

                    CPPUNIT_ASSERT( t.insert( v1 ));
                    CPPUNIT_ASSERT( t.insert( v2 ));
                    CPPUNIT_ASSERT( t.insert( v3 ));
                    CPPUNIT_ASSERT( t.insert( v4 ));
                    CPPUNIT_ASSERT( t.insert( v5 ));
                    CPPUNIT_ASSERT( t.check_consistency() );
                    CPPUNIT_ASSERT( !t.empty() );
                    CPPUNIT_ASSERT( misc::check_size( t, 5 ));

                    gp = t.get_with( wrapped_int( v4.nKey ), wrapped_less<value_type>());
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->nKey == v4.nKey );
                    gp = t.extract_with( wrapped_int( v4.nKey ), wrapped_less<value_type>());
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( t.check_consistency() );
                    CPPUNIT_ASSERT( !t.empty() );
                    CPPUNIT_ASSERT( misc::check_size( t, 4 ));
                    CPPUNIT_ASSERT( gp->nKey == v4.nKey );

                    gp = t.extract_with( v4.nKey, less<value_type>());
                    CPPUNIT_ASSERT( !gp );
                    CPPUNIT_ASSERT( t.check_consistency() );
                    CPPUNIT_ASSERT( !t.get_with( v4.nKey, less<value_type>() ) );

                    gp = t.extract_max();
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( t.check_consistency() );
                    CPPUNIT_ASSERT( !t.empty() );
                    CPPUNIT_ASSERT( misc::check_size( t, 3 ));
                    CPPUNIT_ASSERT( gp->nKey == v3.nKey );

                    gp = t.extract_max();
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( t.check_consistency() );
                    CPPUNIT_ASSERT( !t.empty() );
                    CPPUNIT_ASSERT( misc::check_size( t, 2 ));
                    CPPUNIT_ASSERT( gp->nKey == v2.nKey );

                    gp = t.extract_max();
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( t.check_consistency() );
                    CPPUNIT_ASSERT( !t.empty() );
                    CPPUNIT_ASSERT( misc::check_size( t, 1 ));
                    CPPUNIT_ASSERT( gp->nKey == v1.nKey );

                    gp = t.extract_max();
                    CPPUNIT_ASSERT( gp );
                    CPPUNIT_ASSERT( t.check_consistency() );
                    CPPUNIT_ASSERT( t.empty() );
                    CPPUNIT_ASSERT( misc::check_size( t, 0 ));
                    CPPUNIT_ASSERT( gp->nKey == v5.nKey );
                }

                tree_type::gc::force_dispose();
            }

            {
                data_array< value_type> arr;
                value_type * pFirst = arr.begin();
                value_type * pLast  = arr.end();

                for ( value_type * p = pFirst; p != pLast; ++p ) {
                    CPPUNIT_ASSERT( t.ensure( *p, ensure_functor()).second );
                }
                for ( int n = 0; n < (int) c_nItemCount; ++n ) {
                    typename tree_type::guarded_ptr gp( t.extract_min() );
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->nKey == n );
                }

                for ( value_type * p = pFirst; p != pLast; ++p ) {
                    CPPUNIT_ASSERT( t.insert( *p ) );
                }
                for ( int n = (int) c_nItemCount - 1; n >= 0; --n ) {
                    typename tree_type::guarded_ptr gp( t.extract_max());
                    CPPUNIT_ASSERT( !gp.empty());
                    CPPUNIT_CHECK( gp->nKey == n );
                }

                tree_type::gc::force_dispose();
            }

            PrintStat()( t );
        }

        template <class Tree, class PrintStat>
        void test_rcu()
        {
            typedef Tree tree_type;
            typedef typename tree_type::key_type     key_type;
            typedef typename tree_type::value_type   value_type;

            tree_type t;
            test_common(t);

            // extract/get
            {
                value_type v1( 10, 100 );
                value_type v2( 20, 200 );
                value_type v3( 30, 300 );
                value_type v4( 25, 250 );
                value_type v5( -50, -500 );

                // extract/extract_min/extract_max
                CPPUNIT_ASSERT( t.insert( v1 ));
                CPPUNIT_ASSERT( t.insert( v2 ));
                CPPUNIT_ASSERT( t.insert( v3 ));
                CPPUNIT_ASSERT( t.insert( v4 ));
                CPPUNIT_ASSERT( t.insert( v5 ));
                CPPUNIT_ASSERT( t.check_consistency() );
                CPPUNIT_ASSERT( !t.empty() );
                CPPUNIT_ASSERT( misc::check_size( t, 5 ));

                typename tree_type::exempt_ptr  ep;
                typename tree_type::value_type * pVal;
                {
                    typename tree_type::rcu_lock l;
                    pVal = t.get( v1.nKey );
                    CPPUNIT_ASSERT( pVal != nullptr );
                    CPPUNIT_CHECK( pVal == &v1 );
                }
                ep = t.extract( v1.nKey );
                CPPUNIT_ASSERT( !ep.empty());
                CPPUNIT_CHECK( ep->nKey == v1.nKey );
                {
                    typename tree_type::rcu_lock l;
                    CPPUNIT_CHECK( t.get( v1.nKey ) == nullptr );
                }
                ep.release();
                ep = t.extract( v1.nKey );
                CPPUNIT_ASSERT( ep.empty());

                ep = t.extract_min();
                CPPUNIT_ASSERT( !ep.empty() );
                CPPUNIT_CHECK( ep->nKey == v5.nKey );
                {
                    typename tree_type::rcu_lock l;
                    CPPUNIT_CHECK( t.get( v5.nKey ) == nullptr );
                }

                ep = t.extract( v5.nKey );
                CPPUNIT_ASSERT( ep.empty() );

                ep = t.extract_max();
                CPPUNIT_ASSERT( !ep.empty());
                CPPUNIT_CHECK( ep->nKey == v3.nKey );
                {
                    typename tree_type::rcu_lock l;
                    CPPUNIT_CHECK( t.get( v3.nKey ) == nullptr );
                }
                ep.release();

                {
                    typename tree_type::rcu_lock l;
                    pVal = t.get_with( wrapped_int(v2.nKey), wrapped_less<value_type>() );
                    CPPUNIT_ASSERT( pVal != nullptr );
                    CPPUNIT_CHECK( pVal == &v2 );
                }
                ep = t.extract_with( wrapped_int( v2.nKey ), wrapped_less<value_type>() );
                CPPUNIT_ASSERT( !ep.empty() );
                CPPUNIT_CHECK( ep->nKey == v2.nKey );
                {
                    typename tree_type::rcu_lock l;
                    CPPUNIT_CHECK( t.get_with( wrapped_int( v2.nKey ), wrapped_less<value_type>() ) == nullptr );
                }
                //ep.release();
                ep = t.extract_with( wrapped_int( v2.nKey ), wrapped_less<value_type>() );
                CPPUNIT_CHECK( ep.empty());

                ep = t.extract( v4.nKey );
                CPPUNIT_ASSERT( ep );
                CPPUNIT_CHECK( ep->nKey == v4.nKey );
                ep.release();

                tree_type::gc::force_dispose();

                CPPUNIT_CHECK( t.empty());
                CPPUNIT_ASSERT( misc::check_size( t, 0 ));

                {
                    typename tree_type::rcu_lock l;
                    CPPUNIT_CHECK( t.get( v1.nKey ) == nullptr );
                    CPPUNIT_CHECK( t.get( v2.nKey ) == nullptr );
                    CPPUNIT_CHECK( t.get( v3.nKey ) == nullptr );
                    CPPUNIT_CHECK( t.get( v4.nKey ) == nullptr );
                    CPPUNIT_CHECK( t.get( v5.nKey ) == nullptr );
                }

                CPPUNIT_CHECK( !t.extract(v1.nKey));
                CPPUNIT_CHECK( !t.extract(v2.nKey));
                CPPUNIT_CHECK( !t.extract(v3.nKey));
                CPPUNIT_CHECK( !t.extract(v4.nKey));
                CPPUNIT_CHECK( !t.extract(v5.nKey));

                tree_type::gc::force_dispose();
            }

            PrintStat()( t );
        }

        void EllenBinTree_hp_base_less();
        void EllenBinTree_hp_base_cmp();
        void EllenBinTree_hp_base_cmpless();
        void EllenBinTree_hp_base_less_ic();
        void EllenBinTree_hp_base_cmp_ic();
        void EllenBinTree_hp_base_less_stat();
        void EllenBinTree_hp_base_cmp_ic_stat();
        void EllenBinTree_hp_base_cmp_ic_stat_yield();
        void EllenBinTree_hp_base_less_pool();
        void EllenBinTree_hp_base_less_pool_ic_stat();

        void EllenBinTree_hp_member_less();
        void EllenBinTree_hp_member_cmp();
        void EllenBinTree_hp_member_cmpless();
        void EllenBinTree_hp_member_less_ic();
        void EllenBinTree_hp_member_cmp_ic();
        void EllenBinTree_hp_member_less_stat();
        void EllenBinTree_hp_member_cmp_ic_stat();
        void EllenBinTree_hp_member_cmp_ic_stat_yield();
        void EllenBinTree_hp_member_less_pool();
        void EllenBinTree_hp_member_less_pool_ic_stat();

        void EllenBinTree_dhp_base_less();
        void EllenBinTree_dhp_base_cmp();
        void EllenBinTree_dhp_base_cmpless();
        void EllenBinTree_dhp_base_less_ic();
        void EllenBinTree_dhp_base_cmp_ic();
        void EllenBinTree_dhp_base_less_stat();
        void EllenBinTree_dhp_base_cmp_ic_stat();
        void EllenBinTree_dhp_base_cmp_ic_stat_yield();
        void EllenBinTree_dhp_base_less_pool();
        void EllenBinTree_dhp_base_less_pool_ic_stat();

        void EllenBinTree_dhp_member_less();
        void EllenBinTree_dhp_member_cmp();
        void EllenBinTree_dhp_member_cmpless();
        void EllenBinTree_dhp_member_less_ic();
        void EllenBinTree_dhp_member_cmp_ic();
        void EllenBinTree_dhp_member_less_stat();
        void EllenBinTree_dhp_member_cmp_ic_stat();
        void EllenBinTree_dhp_member_cmp_ic_stat_yield();
        void EllenBinTree_dhp_member_less_pool();
        void EllenBinTree_dhp_member_less_pool_ic_stat();

        void EllenBinTree_rcu_gpi_base_less();
        void EllenBinTree_rcu_gpi_base_cmp();
        void EllenBinTree_rcu_gpi_base_cmpless();
        void EllenBinTree_rcu_gpi_base_less_ic();
        void EllenBinTree_rcu_gpi_base_cmp_ic();
        void EllenBinTree_rcu_gpi_base_less_stat();
        void EllenBinTree_rcu_gpi_base_cmp_ic_stat();
        void EllenBinTree_rcu_gpi_base_cmp_ic_stat_yield();
        void EllenBinTree_rcu_gpi_base_less_pool();
        void EllenBinTree_rcu_gpi_base_less_pool_ic_stat();

        void EllenBinTree_rcu_gpi_member_less();
        void EllenBinTree_rcu_gpi_member_cmp();
        void EllenBinTree_rcu_gpi_member_cmpless();
        void EllenBinTree_rcu_gpi_member_less_ic();
        void EllenBinTree_rcu_gpi_member_cmp_ic();
        void EllenBinTree_rcu_gpi_member_less_stat();
        void EllenBinTree_rcu_gpi_member_cmp_ic_stat();
        void EllenBinTree_rcu_gpi_member_cmp_ic_stat_yield();
        void EllenBinTree_rcu_gpi_member_less_pool();
        void EllenBinTree_rcu_gpi_member_less_pool_ic_stat();

        void EllenBinTree_rcu_gpb_base_less();
        void EllenBinTree_rcu_gpb_base_cmp();
        void EllenBinTree_rcu_gpb_base_cmpless();
        void EllenBinTree_rcu_gpb_base_less_ic();
        void EllenBinTree_rcu_gpb_base_cmp_ic();
        void EllenBinTree_rcu_gpb_base_less_stat();
        void EllenBinTree_rcu_gpb_base_cmp_ic_stat();
        void EllenBinTree_rcu_gpb_base_cmp_ic_stat_yield();
        void EllenBinTree_rcu_gpb_base_less_pool();
        void EllenBinTree_rcu_gpb_base_less_pool_ic_stat();

        void EllenBinTree_rcu_gpb_member_less();
        void EllenBinTree_rcu_gpb_member_cmp();
        void EllenBinTree_rcu_gpb_member_cmpless();
        void EllenBinTree_rcu_gpb_member_less_ic();
        void EllenBinTree_rcu_gpb_member_cmp_ic();
        void EllenBinTree_rcu_gpb_member_less_stat();
        void EllenBinTree_rcu_gpb_member_cmp_ic_stat();
        void EllenBinTree_rcu_gpb_member_cmp_ic_stat_yield();
        void EllenBinTree_rcu_gpb_member_less_pool();
        void EllenBinTree_rcu_gpb_member_less_pool_ic_stat();

        void EllenBinTree_rcu_gpt_base_less();
        void EllenBinTree_rcu_gpt_base_cmp();
        void EllenBinTree_rcu_gpt_base_cmpless();
        void EllenBinTree_rcu_gpt_base_less_ic();
        void EllenBinTree_rcu_gpt_base_cmp_ic();
        void EllenBinTree_rcu_gpt_base_less_stat();
        void EllenBinTree_rcu_gpt_base_cmp_ic_stat();
        void EllenBinTree_rcu_gpt_base_cmp_ic_stat_yield();
        void EllenBinTree_rcu_gpt_base_less_pool();
        void EllenBinTree_rcu_gpt_base_less_pool_ic_stat();

        void EllenBinTree_rcu_gpt_member_less();
        void EllenBinTree_rcu_gpt_member_cmp();
        void EllenBinTree_rcu_gpt_member_cmpless();
        void EllenBinTree_rcu_gpt_member_less_ic();
        void EllenBinTree_rcu_gpt_member_cmp_ic();
        void EllenBinTree_rcu_gpt_member_less_stat();
        void EllenBinTree_rcu_gpt_member_cmp_ic_stat();
        void EllenBinTree_rcu_gpt_member_cmp_ic_stat_yield();
        void EllenBinTree_rcu_gpt_member_less_pool();
        void EllenBinTree_rcu_gpt_member_less_pool_ic_stat();

        void EllenBinTree_rcu_shb_base_less();
        void EllenBinTree_rcu_shb_base_cmp();
        void EllenBinTree_rcu_shb_base_cmpless();
        void EllenBinTree_rcu_shb_base_less_ic();
        void EllenBinTree_rcu_shb_base_cmp_ic();
        void EllenBinTree_rcu_shb_base_less_stat();
        void EllenBinTree_rcu_shb_base_cmp_ic_stat();
        void EllenBinTree_rcu_shb_base_cmp_ic_stat_yield();
        void EllenBinTree_rcu_shb_base_less_pool();
        void EllenBinTree_rcu_shb_base_less_pool_ic_stat();

        void EllenBinTree_rcu_shb_member_less();
        void EllenBinTree_rcu_shb_member_cmp();
        void EllenBinTree_rcu_shb_member_cmpless();
        void EllenBinTree_rcu_shb_member_less_ic();
        void EllenBinTree_rcu_shb_member_cmp_ic();
        void EllenBinTree_rcu_shb_member_less_stat();
        void EllenBinTree_rcu_shb_member_cmp_ic_stat();
        void EllenBinTree_rcu_shb_member_cmp_ic_stat_yield();
        void EllenBinTree_rcu_shb_member_less_pool();
        void EllenBinTree_rcu_shb_member_less_pool_ic_stat();

        void EllenBinTree_rcu_sht_base_less();
        void EllenBinTree_rcu_sht_base_cmp();
        void EllenBinTree_rcu_sht_base_cmpless();
        void EllenBinTree_rcu_sht_base_less_ic();
        void EllenBinTree_rcu_sht_base_cmp_ic();
        void EllenBinTree_rcu_sht_base_less_stat();
        void EllenBinTree_rcu_sht_base_cmp_ic_stat();
        void EllenBinTree_rcu_sht_base_cmp_ic_stat_yield();
        void EllenBinTree_rcu_sht_base_less_pool();
        void EllenBinTree_rcu_sht_base_less_pool_ic_stat();

        void EllenBinTree_rcu_sht_member_less();
        void EllenBinTree_rcu_sht_member_cmp();
        void EllenBinTree_rcu_sht_member_cmpless();
        void EllenBinTree_rcu_sht_member_less_ic();
        void EllenBinTree_rcu_sht_member_cmp_ic();
        void EllenBinTree_rcu_sht_member_less_stat();
        void EllenBinTree_rcu_sht_member_cmp_ic_stat();
        void EllenBinTree_rcu_sht_member_cmp_ic_stat_yield();
        void EllenBinTree_rcu_sht_member_less_pool();
        void EllenBinTree_rcu_sht_member_less_pool_ic_stat();

        CPPUNIT_TEST_SUITE(IntrusiveBinTreeHdrTest)
            CPPUNIT_TEST(EllenBinTree_hp_base_less)
            CPPUNIT_TEST(EllenBinTree_hp_base_cmp)
            CPPUNIT_TEST(EllenBinTree_hp_base_less_stat)
            CPPUNIT_TEST(EllenBinTree_hp_base_cmpless)
            CPPUNIT_TEST(EllenBinTree_hp_base_less_ic)
            CPPUNIT_TEST(EllenBinTree_hp_base_cmp_ic)
            CPPUNIT_TEST(EllenBinTree_hp_base_cmp_ic_stat)
            CPPUNIT_TEST( EllenBinTree_hp_base_cmp_ic_stat_yield )
            CPPUNIT_TEST( EllenBinTree_hp_base_less_pool )
            CPPUNIT_TEST(EllenBinTree_hp_base_less_pool_ic_stat)

            CPPUNIT_TEST(EllenBinTree_hp_member_less)
            CPPUNIT_TEST(EllenBinTree_hp_member_cmp)
            CPPUNIT_TEST(EllenBinTree_hp_member_less_stat)
            CPPUNIT_TEST(EllenBinTree_hp_member_cmpless)
            CPPUNIT_TEST(EllenBinTree_hp_member_less_ic)
            CPPUNIT_TEST(EllenBinTree_hp_member_cmp_ic)
            CPPUNIT_TEST( EllenBinTree_hp_member_cmp_ic_stat )
            CPPUNIT_TEST( EllenBinTree_hp_member_cmp_ic_stat_yield )
            CPPUNIT_TEST(EllenBinTree_hp_member_less_pool)
            CPPUNIT_TEST(EllenBinTree_hp_member_less_pool_ic_stat)

            CPPUNIT_TEST(EllenBinTree_dhp_base_less)
            CPPUNIT_TEST(EllenBinTree_dhp_base_cmp)
            CPPUNIT_TEST(EllenBinTree_dhp_base_less_stat)
            CPPUNIT_TEST(EllenBinTree_dhp_base_cmpless)
            CPPUNIT_TEST(EllenBinTree_dhp_base_less_ic)
            CPPUNIT_TEST(EllenBinTree_dhp_base_cmp_ic)
            CPPUNIT_TEST(EllenBinTree_dhp_base_cmp_ic_stat)
            CPPUNIT_TEST( EllenBinTree_dhp_base_cmp_ic_stat_yield )
            CPPUNIT_TEST( EllenBinTree_dhp_base_less_pool )
            CPPUNIT_TEST(EllenBinTree_dhp_base_less_pool_ic_stat)

            CPPUNIT_TEST(EllenBinTree_dhp_member_less)
            CPPUNIT_TEST(EllenBinTree_dhp_member_cmp)
            CPPUNIT_TEST(EllenBinTree_dhp_member_less_stat)
            CPPUNIT_TEST(EllenBinTree_dhp_member_cmpless)
            CPPUNIT_TEST(EllenBinTree_dhp_member_less_ic)
            CPPUNIT_TEST(EllenBinTree_dhp_member_cmp_ic)
            CPPUNIT_TEST(EllenBinTree_dhp_member_cmp_ic_stat)
            CPPUNIT_TEST( EllenBinTree_dhp_member_cmp_ic_stat_yield )
            CPPUNIT_TEST( EllenBinTree_dhp_member_less_pool )
            CPPUNIT_TEST(EllenBinTree_dhp_member_less_pool_ic_stat)

            CPPUNIT_TEST(EllenBinTree_rcu_gpi_base_less)
            CPPUNIT_TEST(EllenBinTree_rcu_gpi_base_cmp)
            CPPUNIT_TEST(EllenBinTree_rcu_gpi_base_less_stat)
            CPPUNIT_TEST(EllenBinTree_rcu_gpi_base_cmpless)
            CPPUNIT_TEST(EllenBinTree_rcu_gpi_base_less_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_gpi_base_cmp_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_gpi_base_cmp_ic_stat)
            CPPUNIT_TEST( EllenBinTree_rcu_gpi_base_cmp_ic_stat_yield )
            CPPUNIT_TEST( EllenBinTree_rcu_gpi_base_less_pool )
            CPPUNIT_TEST(EllenBinTree_rcu_gpi_base_less_pool_ic_stat)

            CPPUNIT_TEST(EllenBinTree_rcu_gpi_member_less)
            CPPUNIT_TEST(EllenBinTree_rcu_gpi_member_cmp)
            CPPUNIT_TEST(EllenBinTree_rcu_gpi_member_less_stat)
            CPPUNIT_TEST(EllenBinTree_rcu_gpi_member_cmpless)
            CPPUNIT_TEST(EllenBinTree_rcu_gpi_member_less_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_gpi_member_cmp_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_gpi_member_cmp_ic_stat)
            CPPUNIT_TEST( EllenBinTree_rcu_gpi_member_cmp_ic_stat_yield )
            CPPUNIT_TEST( EllenBinTree_rcu_gpi_member_less_pool )
            CPPUNIT_TEST(EllenBinTree_rcu_gpi_member_less_pool_ic_stat)

            CPPUNIT_TEST(EllenBinTree_rcu_gpb_base_less)
            CPPUNIT_TEST(EllenBinTree_rcu_gpb_base_cmp)
            CPPUNIT_TEST(EllenBinTree_rcu_gpb_base_less_stat)
            CPPUNIT_TEST(EllenBinTree_rcu_gpb_base_cmpless)
            CPPUNIT_TEST(EllenBinTree_rcu_gpb_base_less_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_gpb_base_cmp_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_gpb_base_cmp_ic_stat)
            CPPUNIT_TEST( EllenBinTree_rcu_gpb_base_cmp_ic_stat_yield )
            CPPUNIT_TEST( EllenBinTree_rcu_gpb_base_less_pool )
            CPPUNIT_TEST(EllenBinTree_rcu_gpb_base_less_pool_ic_stat)

            CPPUNIT_TEST(EllenBinTree_rcu_gpb_member_less)
            CPPUNIT_TEST(EllenBinTree_rcu_gpb_member_cmp)
            CPPUNIT_TEST(EllenBinTree_rcu_gpb_member_less_stat)
            CPPUNIT_TEST(EllenBinTree_rcu_gpb_member_cmpless)
            CPPUNIT_TEST(EllenBinTree_rcu_gpb_member_less_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_gpb_member_cmp_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_gpb_member_cmp_ic_stat)
            CPPUNIT_TEST( EllenBinTree_rcu_gpb_member_cmp_ic_stat_yield )
            CPPUNIT_TEST( EllenBinTree_rcu_gpb_member_less_pool )
            CPPUNIT_TEST(EllenBinTree_rcu_gpb_member_less_pool_ic_stat)

            CPPUNIT_TEST(EllenBinTree_rcu_gpt_base_less)
            CPPUNIT_TEST(EllenBinTree_rcu_gpt_base_cmp)
            CPPUNIT_TEST(EllenBinTree_rcu_gpt_base_less_stat)
            CPPUNIT_TEST(EllenBinTree_rcu_gpt_base_cmpless)
            CPPUNIT_TEST(EllenBinTree_rcu_gpt_base_less_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_gpt_base_cmp_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_gpt_base_cmp_ic_stat)
            CPPUNIT_TEST( EllenBinTree_rcu_gpt_base_cmp_ic_stat_yield )
            CPPUNIT_TEST( EllenBinTree_rcu_gpt_base_less_pool )
            CPPUNIT_TEST(EllenBinTree_rcu_gpt_base_less_pool_ic_stat)

            CPPUNIT_TEST(EllenBinTree_rcu_gpt_member_less)
            CPPUNIT_TEST(EllenBinTree_rcu_gpt_member_cmp)
            CPPUNIT_TEST(EllenBinTree_rcu_gpt_member_less_stat)
            CPPUNIT_TEST(EllenBinTree_rcu_gpt_member_cmpless)
            CPPUNIT_TEST(EllenBinTree_rcu_gpt_member_less_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_gpt_member_cmp_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_gpt_member_cmp_ic_stat)
            CPPUNIT_TEST( EllenBinTree_rcu_gpt_member_cmp_ic_stat_yield )
            CPPUNIT_TEST( EllenBinTree_rcu_gpt_member_less_pool )
            CPPUNIT_TEST(EllenBinTree_rcu_gpt_member_less_pool_ic_stat)

            CPPUNIT_TEST(EllenBinTree_rcu_shb_base_less)
            CPPUNIT_TEST(EllenBinTree_rcu_shb_base_cmp)
            CPPUNIT_TEST(EllenBinTree_rcu_shb_base_less_stat)
            CPPUNIT_TEST(EllenBinTree_rcu_shb_base_cmpless)
            CPPUNIT_TEST(EllenBinTree_rcu_shb_base_less_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_shb_base_cmp_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_shb_base_cmp_ic_stat)
            CPPUNIT_TEST( EllenBinTree_rcu_shb_base_cmp_ic_stat_yield )
            CPPUNIT_TEST( EllenBinTree_rcu_shb_base_less_pool )
            CPPUNIT_TEST(EllenBinTree_rcu_shb_base_less_pool_ic_stat)

            CPPUNIT_TEST(EllenBinTree_rcu_shb_member_less)
            CPPUNIT_TEST(EllenBinTree_rcu_shb_member_cmp)
            CPPUNIT_TEST(EllenBinTree_rcu_shb_member_less_stat)
            CPPUNIT_TEST(EllenBinTree_rcu_shb_member_cmpless)
            CPPUNIT_TEST(EllenBinTree_rcu_shb_member_less_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_shb_member_cmp_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_shb_member_cmp_ic_stat)
            CPPUNIT_TEST( EllenBinTree_rcu_shb_member_cmp_ic_stat_yield )
            CPPUNIT_TEST( EllenBinTree_rcu_shb_member_less_pool )
            CPPUNIT_TEST(EllenBinTree_rcu_shb_member_less_pool_ic_stat)

            CPPUNIT_TEST(EllenBinTree_rcu_sht_base_less)
            CPPUNIT_TEST(EllenBinTree_rcu_sht_base_cmp)
            CPPUNIT_TEST(EllenBinTree_rcu_sht_base_less_stat)
            CPPUNIT_TEST(EllenBinTree_rcu_sht_base_cmpless)
            CPPUNIT_TEST(EllenBinTree_rcu_sht_base_less_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_sht_base_cmp_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_sht_base_cmp_ic_stat)
            CPPUNIT_TEST( EllenBinTree_rcu_sht_base_cmp_ic_stat_yield )
            CPPUNIT_TEST( EllenBinTree_rcu_sht_base_less_pool )
            CPPUNIT_TEST(EllenBinTree_rcu_sht_base_less_pool_ic_stat)

            CPPUNIT_TEST(EllenBinTree_rcu_sht_member_less)
            CPPUNIT_TEST(EllenBinTree_rcu_sht_member_cmp)
            CPPUNIT_TEST(EllenBinTree_rcu_sht_member_less_stat)
            CPPUNIT_TEST(EllenBinTree_rcu_sht_member_cmpless)
            CPPUNIT_TEST(EllenBinTree_rcu_sht_member_less_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_sht_member_cmp_ic)
            CPPUNIT_TEST(EllenBinTree_rcu_sht_member_cmp_ic_stat)
            CPPUNIT_TEST( EllenBinTree_rcu_sht_member_cmp_ic_stat_yield )
            CPPUNIT_TEST( EllenBinTree_rcu_sht_member_less_pool )
            CPPUNIT_TEST(EllenBinTree_rcu_sht_member_less_pool_ic_stat)

        CPPUNIT_TEST_SUITE_END()
    };
} // namespace tree

#endif // #ifndef CDSTEST_HDR_INTRUSIVE_BINTREE_H
