//$$CDS-header$$

#include "cppunit/cppunit_proxy.h"

#include <cds/cxx11_atomic.h>

#ifndef CDS_USE_BOOST_ATOMIC
// Skip this test for boost.atomic
// Boost.atomic has no free atomic functions implementation.

#include "misc/cxx11_convert_memory_order.h"

namespace misc {

    class cxx11_atomic_func: public CppUnitMini::TestCase
    {
        template <typename AtomicFlag>
        void do_test_atomic_flag_mo( AtomicFlag& f, CDS_ATOMIC::memory_order order )
        {
            CDS_ATOMIC::memory_order mo_clear = convert_to_store_order(order);

            f.clear( convert_to_store_order(order) );

            for ( int i = 0; i < 5; ++i ) {
                CPPUNIT_ASSERT( !CDS_ATOMIC::atomic_flag_test_and_set_explicit( &f, order ));
                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_flag_test_and_set_explicit( &f, order ) );
                CDS_ATOMIC::atomic_flag_clear_explicit( &f, mo_clear );
                CDS_ATOMIC::atomic_flag_clear_explicit( &f, mo_clear );
            }
            //CPPUNIT_ASSERT( f.m_Flag == 0 );
        }

        template <typename AtomicFlag>
        void do_test_atomic_flag( AtomicFlag& f )
        {
            f.clear();

            for ( int i = 0; i < 5; ++i ) {
                //CPPUNIT_ASSERT( f.m_Flag == 0 );
                CPPUNIT_ASSERT( !CDS_ATOMIC::atomic_flag_test_and_set( &f ));
                //CPPUNIT_ASSERT( f.m_Flag != 0 );
                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_flag_test_and_set( &f ) );
                //CPPUNIT_ASSERT( f.m_Flag != 0 );
                CDS_ATOMIC::atomic_flag_clear(&f);
                //CPPUNIT_ASSERT( f.m_Flag == 0 );
                CDS_ATOMIC::atomic_flag_clear(&f);
            }
            //CPPUNIT_ASSERT( f.m_Flag == 0 );

            do_test_atomic_flag_mo( f, CDS_ATOMIC::memory_order_relaxed );
            do_test_atomic_flag_mo( f, CDS_ATOMIC::memory_order_consume );
            do_test_atomic_flag_mo( f, CDS_ATOMIC::memory_order_acquire );
            do_test_atomic_flag_mo( f, CDS_ATOMIC::memory_order_release );
            do_test_atomic_flag_mo( f, CDS_ATOMIC::memory_order_acq_rel );
            do_test_atomic_flag_mo( f, CDS_ATOMIC::memory_order_seq_cst );
        }

        template <class Atomic, typename Integral>
        void do_test_atomic_type(Atomic& a )
        {
            typedef Integral    integral_type;

            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_is_lock_free( &a ) );
            CDS_ATOMIC::atomic_store( &a, (integral_type) 0 );
            CPPUNIT_ASSERT( a == 0 );
            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_load( &a ) == 0 );

            for ( size_t nByte = 0; nByte < sizeof(Integral); ++nByte ) {
                integral_type n = integral_type(42) << (nByte * 8);
                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_exchange( &a, n ) == 0 );
                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_load( &a ) == n );
                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_exchange( &a, (integral_type) 0 ) == n );
                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_load( &a ) == 0 );
            }

            integral_type prev = CDS_ATOMIC::atomic_load( &a );
            for ( size_t nByte = 0; nByte < sizeof(Integral); ++nByte ) {
                integral_type n = integral_type(42) << (nByte * 8);
                integral_type expected = prev;

                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_compare_exchange_weak( &a, &expected, n));
                CPPUNIT_ASSERT( expected  == prev );
                CPPUNIT_ASSERT( expected  != n );
                CPPUNIT_ASSERT( !CDS_ATOMIC::atomic_compare_exchange_weak( &a, &expected, n) );
                CPPUNIT_ASSERT( expected  == n );

                prev = n;
                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_load( &a ) == n );
            }

            CDS_ATOMIC::atomic_store( &a, (integral_type) 0 );

            prev = CDS_ATOMIC::atomic_load( &a );
            for ( size_t nByte = 0; nByte < sizeof(Integral); ++nByte ) {
                integral_type n = integral_type(42) << (nByte * 8);
                integral_type expected = prev;

                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_compare_exchange_strong( &a, &expected, n));
                CPPUNIT_ASSERT( expected  == prev );
                CPPUNIT_ASSERT( !CDS_ATOMIC::atomic_compare_exchange_strong( &a, &expected, n));
                CPPUNIT_ASSERT( expected  == n );

                prev = n;
                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_load( &a ) == n );
            }

            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_exchange( &a, (integral_type) 0 ) == prev );
        }

        template <class Atomic, typename Integral>
        void do_test_atomic_integral( Atomic& a )
        {
            do_test_atomic_type< Atomic, Integral >( a );

            typedef Integral    integral_type;

            // fetch_xxx testing
            CDS_ATOMIC::atomic_store( &a, (integral_type) 0 );

            // fetch_add
            for ( size_t nByte = 0; nByte < sizeof(integral_type); ++nByte )
            {
                integral_type prev = CDS_ATOMIC::atomic_load( &a );
                integral_type n = integral_type(42) << (nByte * 8);

                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_fetch_add( &a, n) == prev);
            }

            // fetch_sub
            for ( size_t nByte = sizeof(integral_type); nByte > 0; --nByte )
            {
                integral_type prev = CDS_ATOMIC::atomic_load( &a );
                integral_type n = integral_type(42) << ((nByte - 1) * 8);

                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_fetch_sub( &a, n) == prev);
            }
            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_load( &a ) == 0 );

            // fetch_or / fetc_xor / fetch_and
            for ( size_t nBit = 0; nBit < sizeof(integral_type) * 8; ++nBit )
            {
                integral_type prev = CDS_ATOMIC::atomic_load( &a );
                integral_type mask = 1 << nBit;

                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_fetch_or( &a, mask ) == prev );
                prev = CDS_ATOMIC::atomic_load( &a );
                CPPUNIT_ASSERT( ( prev & mask)  == mask);

                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_fetch_and( &a, (integral_type) ~mask ) == prev );
                prev = CDS_ATOMIC::atomic_load( &a );
                CPPUNIT_ASSERT_EX( integral_type(prev & mask) == integral_type(0), "prev=" << std::hex << prev << ", mask=" << std::hex << mask);

                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_fetch_xor( &a, mask ) == prev );
                prev = CDS_ATOMIC::atomic_load( &a );
                CPPUNIT_ASSERT( ( prev & mask)  == mask);
            }
            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_load( &a ) == (integral_type) -1 );
        }

        template <class Atomic, typename Integral>
        void do_test_atomic_type( Atomic& a, CDS_ATOMIC::memory_order order )
        {
            typedef Integral    integral_type;

            const CDS_ATOMIC::memory_order oLoad = convert_to_load_order( order );
            const CDS_ATOMIC::memory_order oStore = convert_to_store_order( order );

            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_is_lock_free( &a ) );
            CDS_ATOMIC::atomic_store_explicit( &a, (integral_type) 0, oStore );
            CPPUNIT_ASSERT( a == 0 );
            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_load_explicit( &a, oLoad ) == 0 );

            for ( size_t nByte = 0; nByte < sizeof(Integral); ++nByte ) {
                integral_type n = integral_type(42) << (nByte * 8);
                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_exchange_explicit( &a, n, order ) == 0 );
                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_load_explicit( &a, oLoad ) == n );
                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_exchange_explicit( &a, (integral_type) 0, order ) == n );
                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_load_explicit( &a, oLoad ) == 0 );
            }

            integral_type prev = CDS_ATOMIC::atomic_load_explicit( &a, oLoad );
            for ( size_t nByte = 0; nByte < sizeof(Integral); ++nByte ) {
                integral_type n = integral_type(42) << (nByte * 8);
                integral_type expected = prev;

                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_compare_exchange_weak_explicit( &a, &expected, n, order, CDS_ATOMIC::memory_order_relaxed));
                CPPUNIT_ASSERT( expected  == prev );
                CPPUNIT_ASSERT( !CDS_ATOMIC::atomic_compare_exchange_weak_explicit( &a, &expected, n, order, CDS_ATOMIC::memory_order_relaxed));
                CPPUNIT_ASSERT( expected  == n );

                prev = n;
                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_load_explicit( &a, oLoad ) == n );
            }

            CDS_ATOMIC::atomic_store_explicit( &a, (integral_type) 0, oStore );

            prev = CDS_ATOMIC::atomic_load_explicit( &a, oLoad );
            for ( size_t nByte = 0; nByte < sizeof(Integral); ++nByte ) {
                integral_type n = integral_type(42) << (nByte * 8);
                integral_type expected = prev;

                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_compare_exchange_strong_explicit( &a, &expected, n, order, CDS_ATOMIC::memory_order_relaxed));
                CPPUNIT_ASSERT( expected  == prev );
                CPPUNIT_ASSERT( !CDS_ATOMIC::atomic_compare_exchange_strong_explicit( &a, &expected, n, order, CDS_ATOMIC::memory_order_relaxed));
                CPPUNIT_ASSERT( expected  == n );

                prev = n;
                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_load_explicit( &a, oLoad ) == n );
            }

            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_exchange_explicit( &a, (integral_type) 0, order ) == prev );
        }

        template <class Atomic, typename Integral>
        void do_test_atomic_integral( Atomic& a, CDS_ATOMIC::memory_order order )
        {
            do_test_atomic_type< Atomic, Integral >( a, order );
            typedef Integral    integral_type;

            const CDS_ATOMIC::memory_order oLoad = convert_to_load_order( order );
            const CDS_ATOMIC::memory_order oStore = convert_to_store_order( order );

            // fetch_xxx testing
            CDS_ATOMIC::atomic_store_explicit( &a, (integral_type) 0, oStore );

            // fetch_add
            for ( size_t nByte = 0; nByte < sizeof(integral_type); ++nByte )
            {
                integral_type prev = CDS_ATOMIC::atomic_load_explicit( &a, oLoad );
                integral_type n = integral_type(42) << (nByte * 8);

                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_fetch_add_explicit( &a, n, order) == prev);
            }

            // fetch_sub
            for ( size_t nByte = sizeof(integral_type); nByte > 0; --nByte )
            {
                integral_type prev = CDS_ATOMIC::atomic_load_explicit( &a, oLoad );
                integral_type n = integral_type(42) << ((nByte - 1) * 8);

                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_fetch_sub_explicit( &a, n, order ) == prev);
            }
            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_load_explicit( &a, oLoad ) == 0 );

            // fetch_or / fetc_xor / fetch_and
            for ( size_t nBit = 0; nBit < sizeof(integral_type) * 8; ++nBit )
            {
                integral_type prev = CDS_ATOMIC::atomic_load_explicit( &a, oLoad )  ;;
                integral_type mask = 1 << nBit;

                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_fetch_or_explicit( &a, mask, order ) == prev );
                prev = CDS_ATOMIC::atomic_load_explicit( &a, oLoad );
                CPPUNIT_ASSERT( ( prev & mask)  == mask);

                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_fetch_and_explicit( &a, (integral_type) ~mask, order ) == prev );
                prev = CDS_ATOMIC::atomic_load_explicit( &a, oLoad );
                CPPUNIT_ASSERT( ( prev & mask)  == 0);

                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_fetch_xor_explicit( &a, mask, order ) == prev );
                prev = CDS_ATOMIC::atomic_load_explicit( &a, oLoad );
                CPPUNIT_ASSERT( ( prev & mask)  == mask);
            }
            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_load_explicit( &a, oLoad ) == (integral_type) -1 );
        }

        template <typename Atomic, typename Integral>
        void test_atomic_integral_(Atomic& a)
        {
            do_test_atomic_integral<Atomic, Integral >(a);

            do_test_atomic_integral<Atomic, Integral >( a, CDS_ATOMIC::memory_order_relaxed );
            do_test_atomic_integral<Atomic, Integral >( a, CDS_ATOMIC::memory_order_consume );
            do_test_atomic_integral<Atomic, Integral >( a, CDS_ATOMIC::memory_order_acquire );
            do_test_atomic_integral<Atomic, Integral >( a, CDS_ATOMIC::memory_order_release );
            do_test_atomic_integral<Atomic, Integral >( a, CDS_ATOMIC::memory_order_acq_rel );
            do_test_atomic_integral<Atomic, Integral >( a, CDS_ATOMIC::memory_order_seq_cst );
        }

        template <typename Integral>
        void test_atomic_integral()
        {
            typedef CDS_ATOMIC::atomic<Integral>    atomic_type;
            atomic_type a[8];
            for ( size_t i = 0; i < sizeof(a)/sizeof(a[0]); ++i ) {
                test_atomic_integral_<atomic_type, Integral>( a[i] );
            }
        }
        template <typename Integral>
        void test_atomic_integral_volatile()
        {
            typedef CDS_ATOMIC::atomic<Integral> volatile atomic_type;
            atomic_type a[8];
            for ( size_t i = 0; i < sizeof(a)/sizeof(a[0]); ++i ) {
                test_atomic_integral_<atomic_type, Integral>( a[i] );
            }
        }

        template <class AtomicBool>
        void do_test_atomic_bool(AtomicBool& a)
        {
            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_is_lock_free( &a ) );
            CDS_ATOMIC::atomic_store( &a, false );
            CPPUNIT_ASSERT( a == false );
            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_load( &a ) == false );

            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_exchange( &a, true ) == false );
            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_load( &a ) == true );
            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_exchange( &a, false ) == true );
            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_load( &a ) == false );

            bool expected = false;
            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_compare_exchange_weak( &a, &expected, true));
            CPPUNIT_ASSERT( expected  == false );
            CPPUNIT_ASSERT( !CDS_ATOMIC::atomic_compare_exchange_weak( &a, &expected, false));
            CPPUNIT_ASSERT( expected  == true );
            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_load( &a ) == true );

            CDS_ATOMIC::atomic_store( &a, false );

            expected = false;
            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_compare_exchange_strong( &a, &expected, true));
            CPPUNIT_ASSERT( expected  == false );
            CPPUNIT_ASSERT( !CDS_ATOMIC::atomic_compare_exchange_strong( &a, &expected, false));
            CPPUNIT_ASSERT( expected  == true );

            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_load( &a ) == true );

            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_exchange( &a, false ) == true );
        }

        template <class AtomicBool>
        void do_test_atomic_bool( AtomicBool& a, CDS_ATOMIC::memory_order order )
        {
            const CDS_ATOMIC::memory_order oLoad = convert_to_load_order( order );
            const CDS_ATOMIC::memory_order oStore = convert_to_store_order( order );

            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_is_lock_free( &a ) );
            CDS_ATOMIC::atomic_store_explicit( &a, false, oStore );
            CPPUNIT_ASSERT( a == false );
            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_load_explicit( &a, oLoad ) == false );

            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_exchange_explicit( &a, true, order ) == false );
            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_load_explicit( &a, oLoad ) == true );
            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_exchange_explicit( &a, false, order ) == true );
            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_load_explicit( &a, oLoad ) == false );

            bool expected = false;
            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_compare_exchange_weak_explicit( &a, &expected, true, order, CDS_ATOMIC::memory_order_relaxed));
            CPPUNIT_ASSERT( expected  == false );
            CPPUNIT_ASSERT( !CDS_ATOMIC::atomic_compare_exchange_weak_explicit( &a, &expected, false, order, CDS_ATOMIC::memory_order_relaxed));
            CPPUNIT_ASSERT( expected  == true );
            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_load_explicit( &a, oLoad ) == true );

            CDS_ATOMIC::atomic_store( &a, false );

            expected = false;
            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_compare_exchange_strong_explicit( &a, &expected, true, order, CDS_ATOMIC::memory_order_relaxed));
            CPPUNIT_ASSERT( expected  == false );
            CPPUNIT_ASSERT( !CDS_ATOMIC::atomic_compare_exchange_strong_explicit( &a, &expected, false, order, CDS_ATOMIC::memory_order_relaxed));
            CPPUNIT_ASSERT( expected  == true );

            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_load_explicit( &a, oLoad ) == true );

            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_exchange_explicit( &a, false, order ) == true );
        }

        template <typename Atomic, typename Integral>
        void test_atomic_pointer_for_( Atomic& a, Integral * arr, Integral aSize, CDS_ATOMIC::memory_order order )
        {
            typedef Integral integral_type;
            CDS_ATOMIC::memory_order oLoad = convert_to_load_order(order);
            CDS_ATOMIC::memory_order oStore = convert_to_store_order(order);
            integral_type *  p;

            CDS_ATOMIC::atomic_store_explicit( &a, arr, oStore );
            CPPUNIT_ASSERT( *CDS_ATOMIC::atomic_load_explicit( &a, oLoad ) == 1 );

            p = arr;
            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_compare_exchange_weak_explicit( &a, &p, arr + 5, order, CDS_ATOMIC::memory_order_relaxed ));
            CPPUNIT_ASSERT( p == arr + 0 );
            CPPUNIT_ASSERT( *p == 1 );
            CPPUNIT_ASSERT( !CDS_ATOMIC::atomic_compare_exchange_weak_explicit( &a, &p, arr + 3, order, CDS_ATOMIC::memory_order_relaxed ));
            CPPUNIT_ASSERT( p == arr + 5 );
            CPPUNIT_ASSERT( *p == 6 );

            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_compare_exchange_strong_explicit( &a, &p, arr + 3, order, CDS_ATOMIC::memory_order_relaxed ));
            CPPUNIT_ASSERT( p == arr + 5 );
            CPPUNIT_ASSERT( *p == 6 );
            CPPUNIT_ASSERT( !CDS_ATOMIC::atomic_compare_exchange_strong_explicit( &a, &p, arr + 5, order, CDS_ATOMIC::memory_order_relaxed ));
            CPPUNIT_ASSERT( p == arr + 3 );
            CPPUNIT_ASSERT( *p == 4 );

            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_exchange_explicit( &a, arr, order ) == arr + 3 );
            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_load_explicit( &a, oLoad ) == arr );
            CPPUNIT_ASSERT( *CDS_ATOMIC::atomic_load_explicit( &a, oLoad ) == 1 );

            for ( integral_type i = 1; i < aSize; ++i ) {
                integral_type * p = CDS_ATOMIC::atomic_load_explicit( &a, oLoad );
                CPPUNIT_ASSERT( *p == i );
                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_fetch_add_explicit( &a, 1, order ) == p );
                CPPUNIT_ASSERT( *CDS_ATOMIC::atomic_load_explicit( &a, oLoad ) == i + 1 );
            }

            for ( integral_type i = aSize; i > 1; --i ) {
                integral_type * p = CDS_ATOMIC::atomic_load_explicit( &a, oLoad );
                CPPUNIT_ASSERT( *p == i  );
                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_fetch_sub_explicit( &a, 1, order ) == p );
                CPPUNIT_ASSERT( *CDS_ATOMIC::atomic_load_explicit( &a, oLoad ) == i - 1 );
            }
        }

        template <typename Integral, bool Volatile>
        void test_atomic_pointer_for()
        {
            typedef Integral integral_type;
            typedef typename add_volatile<CDS_ATOMIC::atomic< integral_type *>, Volatile>::type    atomic_pointer;

            integral_type   arr[8];
            const integral_type aSize = sizeof(arr)/sizeof(arr[0]);
            for ( integral_type i = 0; i < aSize; ++i ) {
                arr[size_t(i)] = i + 1;
            }

            atomic_pointer  a;
            integral_type *  p;

            CDS_ATOMIC::atomic_store( &a, arr );
            CPPUNIT_ASSERT( *CDS_ATOMIC::atomic_load( &a ) == 1 );

            p = arr;
            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_compare_exchange_weak( &a, &p, arr + 5 ));
            CPPUNIT_ASSERT( p == arr + 0 );
            CPPUNIT_ASSERT( !CDS_ATOMIC::atomic_compare_exchange_weak( &a, &p, arr + 3 ));
            CPPUNIT_ASSERT( p == arr + 5 );

            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_compare_exchange_strong( &a, &p, arr + 3 ));
            CPPUNIT_ASSERT( p == arr + 5 );
            CPPUNIT_ASSERT( !CDS_ATOMIC::atomic_compare_exchange_strong( &a, &p, arr + 5 ));
            CPPUNIT_ASSERT( p == arr + 3 );

            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_exchange( &a, arr ) == arr + 3 );
            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_load( &a ) == arr );
            CPPUNIT_ASSERT( *CDS_ATOMIC::atomic_load( &a ) == 1 );

            for ( integral_type i = 1; i < aSize; ++i ) {
                integral_type * p = CDS_ATOMIC::atomic_load( &a );
                CPPUNIT_ASSERT( *p == i );
                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_fetch_add( &a, 1 ) == p );
                CPPUNIT_ASSERT( *CDS_ATOMIC::atomic_load( &a ) == i + 1 );
            }

            for ( integral_type i = aSize; i > 1; --i ) {
                integral_type * p = CDS_ATOMIC::atomic_load( &a );
                CPPUNIT_ASSERT( *p == i  );
                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_fetch_sub( &a, 1 ) == p );
                CPPUNIT_ASSERT( *CDS_ATOMIC::atomic_load( &a ) == i - 1 );
            }

            test_atomic_pointer_for_( a, arr, aSize, CDS_ATOMIC::memory_order_relaxed );
            test_atomic_pointer_for_( a, arr, aSize, CDS_ATOMIC::memory_order_consume );
            test_atomic_pointer_for_( a, arr, aSize, CDS_ATOMIC::memory_order_acquire );
            test_atomic_pointer_for_( a, arr, aSize, CDS_ATOMIC::memory_order_release );
            test_atomic_pointer_for_( a, arr, aSize, CDS_ATOMIC::memory_order_acq_rel );
            test_atomic_pointer_for_( a, arr, aSize, CDS_ATOMIC::memory_order_seq_cst );

        }

        template <typename Atomic>
        void do_test_atomic_pointer_void_( Atomic& a, char * arr, char aSize, CDS_ATOMIC::memory_order order )
        {
            CDS_ATOMIC::memory_order oLoad = convert_to_load_order(order);
            CDS_ATOMIC::memory_order oStore = convert_to_store_order(order);
            char *  p;

            CDS_ATOMIC::atomic_store_explicit( &a, (void *) arr, oStore );
            CPPUNIT_ASSERT( *reinterpret_cast<char *>(CDS_ATOMIC::atomic_load_explicit( &a, oLoad )) == 1 );

            p = arr;
            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_compare_exchange_weak_explicit( &a, (void **) &p, (void *)(arr + 5), order, CDS_ATOMIC::memory_order_relaxed ));
            CPPUNIT_ASSERT( p == arr + 0 );
            CPPUNIT_ASSERT( *p == 1 );
            CPPUNIT_ASSERT( !CDS_ATOMIC::atomic_compare_exchange_weak_explicit( &a, (void **) &p, (void *)(arr + 3), order, CDS_ATOMIC::memory_order_relaxed ));
            CPPUNIT_ASSERT( p == arr + 5 );
            CPPUNIT_ASSERT( *p == 6 );

            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_compare_exchange_strong_explicit( &a, (void **) &p, (void *)(arr + 3), order, CDS_ATOMIC::memory_order_relaxed ));
            CPPUNIT_ASSERT( p == arr + 5 );
            CPPUNIT_ASSERT( *p == 6 );
            CPPUNIT_ASSERT( !CDS_ATOMIC::atomic_compare_exchange_strong_explicit( &a, (void **) &p, (void *)(arr + 5), order, CDS_ATOMIC::memory_order_relaxed ));
            CPPUNIT_ASSERT( p == arr + 3 );
            CPPUNIT_ASSERT( *p == 4 );

            CPPUNIT_ASSERT( reinterpret_cast<char *>(CDS_ATOMIC::atomic_exchange_explicit( &a, (void *) arr, order )) == arr + 3 );
            CPPUNIT_ASSERT( reinterpret_cast<char *>(CDS_ATOMIC::atomic_load_explicit( &a, oLoad )) == arr );
            CPPUNIT_ASSERT( *reinterpret_cast<char *>(CDS_ATOMIC::atomic_load_explicit( &a, oLoad )) == 1 );

            for ( char i = 1; i < aSize; ++i ) {
                CPPUNIT_ASSERT( *reinterpret_cast<char *>(CDS_ATOMIC::atomic_load_explicit( &a, oLoad )) == i );
                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_fetch_add_explicit( &a, 1, order ));
                CPPUNIT_ASSERT( *reinterpret_cast<char *>(CDS_ATOMIC::atomic_load_explicit( &a, oLoad )) == i + 1 );
            }

            for ( char i = aSize; i > 1; --i ) {
                CPPUNIT_ASSERT( *reinterpret_cast<char *>(CDS_ATOMIC::atomic_load_explicit( &a, oLoad )) == i  );
                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_fetch_sub_explicit( &a, 1, order ));
                CPPUNIT_ASSERT( *reinterpret_cast<char *>(CDS_ATOMIC::atomic_load_explicit( &a, oLoad )) == i - 1 );
            }
        }

        template <bool Volatile>
        void do_test_atomic_pointer_void()
        {
            typedef typename add_volatile<CDS_ATOMIC::atomic< void *>, Volatile>::type    atomic_pointer;

            char   arr[8];
            const char aSize = sizeof(arr)/sizeof(arr[0]);
            for ( char i = 0; i < aSize; ++i ) {
                arr[unsigned(i)] = i + 1;
            }

            atomic_pointer  a;
            char *  p;

            CDS_ATOMIC::atomic_store( &a, (void *) arr );
            CPPUNIT_ASSERT( *reinterpret_cast<char *>(CDS_ATOMIC::atomic_load( &a )) == 1 );

            p = arr;
            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_compare_exchange_weak( &a, (void **) &p, (void *)(arr + 5) ));
            CPPUNIT_ASSERT( p == arr + 0 );
            CPPUNIT_ASSERT( !CDS_ATOMIC::atomic_compare_exchange_weak( &a, (void **) &p, (void *)(arr + 3) ));
            CPPUNIT_ASSERT( p == arr + 5 );

            CPPUNIT_ASSERT( CDS_ATOMIC::atomic_compare_exchange_strong( &a, (void **) &p, (void *)(arr + 3) ));
            CPPUNIT_ASSERT( p == arr + 5 );
            CPPUNIT_ASSERT( !CDS_ATOMIC::atomic_compare_exchange_strong( &a, (void **) &p, (void *)(arr + 5) ));
            CPPUNIT_ASSERT( p == arr + 3 );

            CPPUNIT_ASSERT( reinterpret_cast<char *>( CDS_ATOMIC::atomic_exchange( &a, (void *) arr )) == arr + 3 );
            CPPUNIT_ASSERT( reinterpret_cast<char *>( CDS_ATOMIC::atomic_load( &a )) == arr );
            CPPUNIT_ASSERT( *reinterpret_cast<char *>(CDS_ATOMIC::atomic_load( &a )) == 1 );

            for ( char i = 1; i < aSize; ++i ) {
                CPPUNIT_ASSERT( *reinterpret_cast<char *>(CDS_ATOMIC::atomic_load( &a )) == i );
                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_fetch_add( &a, 1 ));
                CPPUNIT_ASSERT( *reinterpret_cast<char *>(CDS_ATOMIC::atomic_load( &a )) == i + 1 );
            }

            for ( char i = aSize; i > 1; --i ) {
                CPPUNIT_ASSERT( *reinterpret_cast<char *>(CDS_ATOMIC::atomic_load( &a )) == i  );
                CPPUNIT_ASSERT( CDS_ATOMIC::atomic_fetch_sub( &a, 1 ));
                CPPUNIT_ASSERT( *reinterpret_cast<char *>(CDS_ATOMIC::atomic_load( &a )) == i - 1 );
            }

            do_test_atomic_pointer_void_( a, arr, aSize, CDS_ATOMIC::memory_order_relaxed );
            do_test_atomic_pointer_void_( a, arr, aSize, CDS_ATOMIC::memory_order_consume );
            do_test_atomic_pointer_void_( a, arr, aSize, CDS_ATOMIC::memory_order_acquire );
            do_test_atomic_pointer_void_( a, arr, aSize, CDS_ATOMIC::memory_order_release );
            do_test_atomic_pointer_void_( a, arr, aSize, CDS_ATOMIC::memory_order_acq_rel );
            do_test_atomic_pointer_void_( a, arr, aSize, CDS_ATOMIC::memory_order_seq_cst );
        }

    public:
        void test_atomic_flag()
        {
            CDS_ATOMIC::atomic_flag flags[8];
            for ( size_t i = 0; i < sizeof(flags)/sizeof(flags[0]); ++i )
                do_test_atomic_flag( flags[i] );
        }
        void test_atomic_flag_volatile()
        {
            CDS_ATOMIC::atomic_flag volatile flags[8];
            for ( size_t i = 0; i < sizeof(flags)/sizeof(flags[0]); ++i )
                do_test_atomic_flag( flags[i] );
        }

        template <typename AtomicBool>
        void test_atomic_bool_()
        {
            AtomicBool a[8];
            for ( size_t i = 0; i < sizeof(a)/sizeof(a[0]); ++i ) {
                do_test_atomic_bool( a[i] );

                do_test_atomic_bool( a[i], CDS_ATOMIC::memory_order_relaxed );
                do_test_atomic_bool( a[i], CDS_ATOMIC::memory_order_consume );
                do_test_atomic_bool( a[i], CDS_ATOMIC::memory_order_acquire );
                do_test_atomic_bool( a[i], CDS_ATOMIC::memory_order_release );
                do_test_atomic_bool( a[i], CDS_ATOMIC::memory_order_acq_rel );
                do_test_atomic_bool( a[i], CDS_ATOMIC::memory_order_seq_cst );
            }
        }

        void test_atomic_bool()
        {
            test_atomic_bool_<CDS_ATOMIC::atomic<bool> >();
        }
        void test_atomic_bool_volatile()
        {
            test_atomic_bool_<CDS_ATOMIC::atomic<bool> volatile >();
        }

        void test_atomic_char()                 { test_atomic_integral<char>(); }
        void test_atomic_char_volatile()        { test_atomic_integral_volatile<char>(); }
        void test_atomic_signed_char()          { test_atomic_integral<signed char>(); }
        void test_atomic_signed_char_volatile() { test_atomic_integral_volatile<signed char>(); }
        void test_atomic_unsigned_char()        { test_atomic_integral<unsigned char>(); }
        void test_atomic_unsigned_char_volatile(){ test_atomic_integral_volatile<unsigned char>(); }
        void test_atomic_short_int()            { test_atomic_integral<short int>(); }
        void test_atomic_short_int_volatile()   { test_atomic_integral_volatile<short int>(); }
        void test_atomic_unsigned_short_int()   { test_atomic_integral<unsigned short int>(); }
        void test_atomic_unsigned_short_int_volatile() { test_atomic_integral_volatile<unsigned short int>(); }
        void test_atomic_int()                  { test_atomic_integral<int>(); }
        void test_atomic_int_volatile()         { test_atomic_integral_volatile<int>(); }
        void test_atomic_unsigned_int()         { test_atomic_integral<unsigned int>(); }
        void test_atomic_unsigned_int_volatile(){ test_atomic_integral_volatile<unsigned int>(); }
        void test_atomic_long()                 { test_atomic_integral<long>(); }
        void test_atomic_long_volatile()        { test_atomic_integral_volatile<long>(); }
        void test_atomic_unsigned_long()        { test_atomic_integral<unsigned long>(); }
        void test_atomic_unsigned_long_volatile() { test_atomic_integral_volatile<unsigned long>(); }
        void test_atomic_long_long()            { test_atomic_integral<long long>(); }
        void test_atomic_long_long_volatile()   { test_atomic_integral_volatile<long long>(); }
        void test_atomic_unsigned_long_long()   { test_atomic_integral<unsigned long long>(); }
        void test_atomic_unsigned_long_long_volatile() { test_atomic_integral_volatile<unsigned long long>(); }
//#if CDS_COMPILER == CDS_COMPILER_GCC && CDS_COMPILER_VERSION >= 40400
//        void test_atomic_char16_t()             { test_atomic_integral<char16_t>(); }
//        void test_atomic_char16_t_volatile()    { test_atomic_integral_volatile<char16_t>(); }
//        void test_atomic_char32_t()             { test_atomic_integral<char32_t>(); }
//        void test_atomic_char32_t_volatile()    { test_atomic_integral_volatile<char32_t>(); }
//#endif
//        void test_atomic_wchar_t()
//        {
//#if CDS_OS_TYPE != CDS_OS_HPUX
//            test_atomic_integral<wchar_t>();
//#endif
//        }
//        void test_atomic_wchar_t_volatile()
//        {
//#if CDS_OS_TYPE != CDS_OS_HPUX
//            test_atomic_integral_volatile<wchar_t>();
//#endif
//        }

        void test_atomic_pointer_void()         { do_test_atomic_pointer_void<false>() ;}
        void test_atomic_pointer_void_volatile(){ do_test_atomic_pointer_void<true>() ;}

        void test_atomic_pointer_char()         { test_atomic_pointer_for<char, false>() ;}
        void test_atomic_pointer_short()        { test_atomic_pointer_for<short int, false>() ;}
        void test_atomic_pointer_int()          { test_atomic_pointer_for<int, false>() ;}
        void test_atomic_pointer_long()         { test_atomic_pointer_for<long, false>() ;}
        void test_atomic_pointer_long_long()    { test_atomic_pointer_for<long long, false>() ;}

        void test_atomic_pointer_char_volatile()        { test_atomic_pointer_for<char, true>() ;}
        void test_atomic_pointer_short_volatile()       { test_atomic_pointer_for<unsigned short int, true>() ;}
        void test_atomic_pointer_int_volatile()          { test_atomic_pointer_for<int, true>() ;}
        void test_atomic_pointer_long_volatile()         { test_atomic_pointer_for<long, true>() ;}
        void test_atomic_pointer_long_long_volatile()    { test_atomic_pointer_for<long long, true>() ;}

        void test_atomic_fence()
        {
            CDS_ATOMIC::atomic_thread_fence(CDS_ATOMIC::memory_order_relaxed );
            CDS_ATOMIC::atomic_thread_fence(CDS_ATOMIC::memory_order_consume );
            CDS_ATOMIC::atomic_thread_fence(CDS_ATOMIC::memory_order_acquire );
            CDS_ATOMIC::atomic_thread_fence(CDS_ATOMIC::memory_order_release );
            CDS_ATOMIC::atomic_thread_fence(CDS_ATOMIC::memory_order_acq_rel );
            CDS_ATOMIC::atomic_thread_fence(CDS_ATOMIC::memory_order_seq_cst );

            CDS_ATOMIC::atomic_signal_fence(CDS_ATOMIC::memory_order_relaxed );
            CDS_ATOMIC::atomic_signal_fence(CDS_ATOMIC::memory_order_consume );
            CDS_ATOMIC::atomic_signal_fence(CDS_ATOMIC::memory_order_acquire );
            CDS_ATOMIC::atomic_signal_fence(CDS_ATOMIC::memory_order_release );
            CDS_ATOMIC::atomic_signal_fence(CDS_ATOMIC::memory_order_acq_rel );
            CDS_ATOMIC::atomic_signal_fence(CDS_ATOMIC::memory_order_seq_cst );
        }

    public:
        CPPUNIT_TEST_SUITE(cxx11_atomic_func)
            CPPUNIT_TEST( test_atomic_flag )
            CPPUNIT_TEST( test_atomic_flag_volatile )

            CPPUNIT_TEST( test_atomic_bool )
            CPPUNIT_TEST( test_atomic_char )
            CPPUNIT_TEST( test_atomic_signed_char)
            CPPUNIT_TEST( test_atomic_unsigned_char)
            CPPUNIT_TEST( test_atomic_short_int)
            CPPUNIT_TEST( test_atomic_unsigned_short_int)
            CPPUNIT_TEST( test_atomic_int)
            CPPUNIT_TEST( test_atomic_unsigned_int)
            CPPUNIT_TEST( test_atomic_long)
            CPPUNIT_TEST( test_atomic_unsigned_long)
            CPPUNIT_TEST( test_atomic_long_long)
            CPPUNIT_TEST( test_atomic_unsigned_long_long)
//#if CDS_COMPILER == CDS_COMPILER_GCC && CDS_COMPILER_VERSION >= 40400
//            CPPUNIT_TEST( test_atomic_char16_t )
//            CPPUNIT_TEST( test_atomic_char32_t )
//#endif
//            CPPUNIT_TEST( test_atomic_wchar_t)

            CPPUNIT_TEST( test_atomic_bool_volatile )
            CPPUNIT_TEST( test_atomic_char_volatile )
            CPPUNIT_TEST( test_atomic_signed_char_volatile)
            CPPUNIT_TEST( test_atomic_unsigned_char_volatile)
            CPPUNIT_TEST( test_atomic_short_int_volatile)
            CPPUNIT_TEST( test_atomic_unsigned_short_int_volatile)
            CPPUNIT_TEST( test_atomic_int_volatile)
            CPPUNIT_TEST( test_atomic_unsigned_int_volatile)
            CPPUNIT_TEST( test_atomic_long_volatile)
            CPPUNIT_TEST( test_atomic_unsigned_long_volatile)
            CPPUNIT_TEST( test_atomic_long_long_volatile)
            CPPUNIT_TEST( test_atomic_unsigned_long_long_volatile)
//#if CDS_COMPILER == CDS_COMPILER_GCC && CDS_COMPILER_VERSION >= 40400
//            CPPUNIT_TEST( test_atomic_char16_t_volatile )
//            CPPUNIT_TEST( test_atomic_char32_t_volatile )
//#endif
//            CPPUNIT_TEST( test_atomic_wchar_t_volatile)

            CPPUNIT_TEST( test_atomic_pointer_void)
            CPPUNIT_TEST( test_atomic_pointer_void_volatile)

            CPPUNIT_TEST( test_atomic_pointer_char)
            CPPUNIT_TEST( test_atomic_pointer_short)
            CPPUNIT_TEST( test_atomic_pointer_int)
            CPPUNIT_TEST( test_atomic_pointer_long)
            CPPUNIT_TEST( test_atomic_pointer_long_long)

            CPPUNIT_TEST( test_atomic_pointer_char_volatile)
            CPPUNIT_TEST( test_atomic_pointer_short_volatile)
            CPPUNIT_TEST( test_atomic_pointer_int_volatile)
            CPPUNIT_TEST( test_atomic_pointer_long_volatile)
            CPPUNIT_TEST( test_atomic_pointer_long_long_volatile)

            CPPUNIT_TEST( test_atomic_fence)

        CPPUNIT_TEST_SUITE_END()
    };
}   // namespace misc

CPPUNIT_TEST_SUITE_REGISTRATION(misc::cxx11_atomic_func);

#endif // #ifndef CDS_USE_BOOST_ATOMIC
