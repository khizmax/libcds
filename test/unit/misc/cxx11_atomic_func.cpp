/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

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

#include <gtest/gtest.h>
#include <cds/algo/atomic.h>

#ifndef CDS_USE_BOOST_ATOMIC
// Skip this test for boost.atomic
// Boost.atomic has no free atomic functions implementation.

#include "cxx11_convert_memory_order.h"

namespace misc {

    class cxx11_atomic_func: public ::testing::Test
    {
    protected:
        template <typename AtomicFlag>
        void do_test_atomic_flag_mo( AtomicFlag& f, atomics::memory_order order )
        {
            atomics::memory_order mo_clear = convert_to_store_order(order);

            f.clear( convert_to_store_order(order));

            for ( int i = 0; i < 5; ++i ) {
                EXPECT_FALSE( atomics::atomic_flag_test_and_set_explicit( &f, order ));
                EXPECT_TRUE( atomics::atomic_flag_test_and_set_explicit( &f, order ));
                atomics::atomic_flag_clear_explicit( &f, mo_clear );
                atomics::atomic_flag_clear_explicit( &f, mo_clear );
            }
        }

        template <typename AtomicFlag>
        void do_test_atomic_flag( AtomicFlag& f )
        {
            f.clear();

            for ( int i = 0; i < 5; ++i ) {
                EXPECT_FALSE( atomics::atomic_flag_test_and_set( &f ));
                EXPECT_TRUE( atomics::atomic_flag_test_and_set( &f ));
                atomics::atomic_flag_clear(&f);
                atomics::atomic_flag_clear(&f);
            }

            do_test_atomic_flag_mo( f, atomics::memory_order_relaxed );
            do_test_atomic_flag_mo( f, atomics::memory_order_acquire );
            do_test_atomic_flag_mo( f, atomics::memory_order_release );
            do_test_atomic_flag_mo( f, atomics::memory_order_acq_rel );
            do_test_atomic_flag_mo( f, atomics::memory_order_seq_cst );
        }

        template <class Atomic, typename Integral>
        void do_test_atomic_type(Atomic& a )
        {
            typedef Integral    integral_type;

            EXPECT_TRUE( atomics::atomic_is_lock_free( &a ));
            atomics::atomic_store( &a, (integral_type) 0 );
            //EXPECT_EQ( a, integral_type( 0 ));
            EXPECT_EQ( atomics::atomic_load( &a ), integral_type( 0 ));

            for ( size_t nByte = 0; nByte < sizeof(Integral); ++nByte ) {
                integral_type n = integral_type(42) << (nByte * 8);
                EXPECT_EQ( atomics::atomic_exchange( &a, n ), (integral_type) 0 );
                EXPECT_EQ( atomics::atomic_load( &a ), n );
                EXPECT_EQ( atomics::atomic_exchange( &a, (integral_type) 0 ), n );
                EXPECT_EQ( atomics::atomic_load( &a ), (integral_type) 0 );
            }

            integral_type prev = atomics::atomic_load( &a );
            for ( size_t nByte = 0; nByte < sizeof(Integral); ++nByte ) {
                integral_type n = integral_type(42) << (nByte * 8);
                integral_type expected = prev;

                EXPECT_TRUE( atomics::atomic_compare_exchange_weak( &a, &expected, n));
                EXPECT_EQ( expected, prev );
                EXPECT_NE( expected, n );
                EXPECT_FALSE( atomics::atomic_compare_exchange_weak( &a, &expected, n));
                EXPECT_EQ( expected, n );

                prev = n;
                EXPECT_EQ( atomics::atomic_load( &a ), n );
            }

            atomics::atomic_store( &a, (integral_type) 0 );

            prev = atomics::atomic_load( &a );
            for ( size_t nByte = 0; nByte < sizeof(Integral); ++nByte ) {
                integral_type n = integral_type(42) << (nByte * 8);
                integral_type expected = prev;

                EXPECT_TRUE( atomics::atomic_compare_exchange_strong( &a, &expected, n));
                EXPECT_EQ( expected, prev );
                EXPECT_FALSE( atomics::atomic_compare_exchange_strong( &a, &expected, n));
                EXPECT_EQ( expected, n );

                prev = n;
                EXPECT_EQ( atomics::atomic_load( &a ), n );
            }

            EXPECT_EQ( atomics::atomic_exchange( &a, (integral_type) 0 ), prev );
        }

        template <class Atomic, typename Integral>
        void do_test_atomic_integral( Atomic& a )
        {
            do_test_atomic_type< Atomic, Integral >( a );

            typedef Integral    integral_type;

            // fetch_xxx testing
            atomics::atomic_store( &a, (integral_type) 0 );

            // fetch_add
            for ( size_t nByte = 0; nByte < sizeof(integral_type); ++nByte )
            {
                integral_type prev = atomics::atomic_load( &a );
                integral_type n = integral_type(42) << (nByte * 8);

                EXPECT_EQ( atomics::atomic_fetch_add( &a, n ), prev );
            }

            // fetch_sub
            for ( size_t nByte = sizeof(integral_type); nByte > 0; --nByte )
            {
                integral_type prev = atomics::atomic_load( &a );
                integral_type n = integral_type(42) << ((nByte - 1) * 8);

                EXPECT_EQ( atomics::atomic_fetch_sub( &a, n ), prev );
            }
            EXPECT_EQ( atomics::atomic_load( &a ), (integral_type) 0 );

            // fetch_or / fetc_xor / fetch_and
            for ( size_t nBit = 0; nBit < sizeof(integral_type) * 8; ++nBit )
            {
                integral_type prev = atomics::atomic_load( &a );
                integral_type mask = integral_type(1) << nBit;

                EXPECT_EQ( atomics::atomic_fetch_or( &a, mask ), prev );
                prev = atomics::atomic_load( &a );
                EXPECT_EQ( ( prev & mask ), mask );

                EXPECT_EQ( atomics::atomic_fetch_and( &a, (integral_type) ~mask ), prev );
                prev = atomics::atomic_load( &a );
                EXPECT_EQ( integral_type(prev & mask), integral_type(0));

                EXPECT_EQ( atomics::atomic_fetch_xor( &a, mask ), prev );
                prev = atomics::atomic_load( &a );
                EXPECT_EQ( ( prev & mask), mask);
            }
            EXPECT_EQ( atomics::atomic_load( &a ), (integral_type) -1 );
        }

        template <class Atomic, typename Integral>
        void do_test_atomic_type( Atomic& a, atomics::memory_order order )
        {
            typedef Integral    integral_type;

            const atomics::memory_order oLoad = convert_to_load_order( order );
            const atomics::memory_order oStore = convert_to_store_order( order );

            EXPECT_TRUE( atomics::atomic_is_lock_free( &a ));
            atomics::atomic_store_explicit( &a, (integral_type) 0, oStore );
            //EXPECT_EQ( a, integral_type( 0 ));
            EXPECT_EQ( atomics::atomic_load_explicit( &a, oLoad ), (integral_type) 0 );

            for ( size_t nByte = 0; nByte < sizeof(Integral); ++nByte ) {
                integral_type n = integral_type(42) << (nByte * 8);
                EXPECT_EQ( atomics::atomic_exchange_explicit( &a, n, order ), (integral_type) 0 );
                EXPECT_EQ( atomics::atomic_load_explicit( &a, oLoad ), n );
                EXPECT_EQ( atomics::atomic_exchange_explicit( &a, (integral_type) 0, order ), n );
                EXPECT_EQ( atomics::atomic_load_explicit( &a, oLoad ), (integral_type) 0 );
            }

            integral_type prev = atomics::atomic_load_explicit( &a, oLoad );
            for ( size_t nByte = 0; nByte < sizeof(Integral); ++nByte ) {
                integral_type n = integral_type(42) << (nByte * 8);
                integral_type expected = prev;

                EXPECT_TRUE( atomics::atomic_compare_exchange_weak_explicit( &a, &expected, n, order, atomics::memory_order_relaxed));
                EXPECT_EQ( expected, prev );
                EXPECT_FALSE( atomics::atomic_compare_exchange_weak_explicit( &a, &expected, n, order, atomics::memory_order_relaxed));
                EXPECT_EQ( expected, n );

                prev = n;
                EXPECT_EQ( atomics::atomic_load_explicit( &a, oLoad ), n );
            }

            atomics::atomic_store_explicit( &a, (integral_type) 0, oStore );

            prev = atomics::atomic_load_explicit( &a, oLoad );
            for ( size_t nByte = 0; nByte < sizeof(Integral); ++nByte ) {
                integral_type n = integral_type(42) << (nByte * 8);
                integral_type expected = prev;

                EXPECT_TRUE( atomics::atomic_compare_exchange_strong_explicit( &a, &expected, n, order, atomics::memory_order_relaxed));
                EXPECT_EQ( expected, prev );
                EXPECT_FALSE( atomics::atomic_compare_exchange_strong_explicit( &a, &expected, n, order, atomics::memory_order_relaxed));
                EXPECT_EQ( expected, n );

                prev = n;
                EXPECT_EQ( atomics::atomic_load_explicit( &a, oLoad ), n );
            }

            EXPECT_EQ( atomics::atomic_exchange_explicit( &a, (integral_type) 0, order ), prev );
        }

        template <class Atomic, typename Integral>
        void do_test_atomic_integral( Atomic& a, atomics::memory_order order )
        {
            do_test_atomic_type< Atomic, Integral >( a, order );
            typedef Integral    integral_type;

            const atomics::memory_order oLoad = convert_to_load_order( order );
            const atomics::memory_order oStore = convert_to_store_order( order );

            // fetch_xxx testing
            atomics::atomic_store_explicit( &a, (integral_type) 0, oStore );

            // fetch_add
            for ( size_t nByte = 0; nByte < sizeof(integral_type); ++nByte )
            {
                integral_type prev = atomics::atomic_load_explicit( &a, oLoad );
                integral_type n = integral_type(42) << (nByte * 8);

                EXPECT_EQ( atomics::atomic_fetch_add_explicit( &a, n, order), prev);
            }

            // fetch_sub
            for ( size_t nByte = sizeof(integral_type); nByte > 0; --nByte )
            {
                integral_type prev = atomics::atomic_load_explicit( &a, oLoad );
                integral_type n = integral_type(42) << ((nByte - 1) * 8);

                EXPECT_EQ( atomics::atomic_fetch_sub_explicit( &a, n, order ), prev);
            }
            EXPECT_EQ( atomics::atomic_load_explicit( &a, oLoad ), integral_type( 0 ));

            // fetch_or / fetc_xor / fetch_and
            for ( size_t nBit = 0; nBit < sizeof(integral_type) * 8; ++nBit )
            {
                integral_type prev = atomics::atomic_load_explicit( &a, oLoad )  ;;
                integral_type mask = integral_type(1) << nBit;

                EXPECT_EQ( atomics::atomic_fetch_or_explicit( &a, mask, order ), prev );
                prev = atomics::atomic_load_explicit( &a, oLoad );
                EXPECT_EQ( ( prev & mask), mask);

                EXPECT_EQ( atomics::atomic_fetch_and_explicit( &a, (integral_type) ~mask, order ), prev );
                prev = atomics::atomic_load_explicit( &a, oLoad );
                EXPECT_EQ( ( prev & mask), integral_type( 0 ));

                EXPECT_EQ( atomics::atomic_fetch_xor_explicit( &a, mask, order ), prev );
                prev = atomics::atomic_load_explicit( &a, oLoad );
                EXPECT_EQ( ( prev & mask), mask);
            }
            EXPECT_EQ( atomics::atomic_load_explicit( &a, oLoad ), (integral_type) -1 );
        }

        template <typename Atomic, typename Integral>
        void test_atomic_integral_(Atomic& a)
        {
            do_test_atomic_integral<Atomic, Integral >(a);

            do_test_atomic_integral<Atomic, Integral >( a, atomics::memory_order_relaxed );
            do_test_atomic_integral<Atomic, Integral >( a, atomics::memory_order_acquire );
            do_test_atomic_integral<Atomic, Integral >( a, atomics::memory_order_release );
            do_test_atomic_integral<Atomic, Integral >( a, atomics::memory_order_acq_rel );
            do_test_atomic_integral<Atomic, Integral >( a, atomics::memory_order_seq_cst );
        }

        template <typename Integral>
        void test_atomic_integral()
        {
            typedef atomics::atomic<Integral>    atomic_type;
            atomic_type a[8];
            for ( size_t i = 0; i < sizeof(a)/sizeof(a[0]); ++i ) {
                test_atomic_integral_<atomic_type, Integral>( a[i] );
            }
        }
        template <typename Integral>
        void test_atomic_integral_volatile()
        {
            typedef atomics::atomic<Integral> volatile atomic_type;
            atomic_type a[8];
            for ( size_t i = 0; i < sizeof(a)/sizeof(a[0]); ++i ) {
                test_atomic_integral_<atomic_type, Integral>( a[i] );
            }
        }

        template <class AtomicBool>
        void do_test_atomic_bool(AtomicBool& a)
        {
            EXPECT_TRUE( atomics::atomic_is_lock_free( &a ));
            atomics::atomic_store( &a, false );
            EXPECT_FALSE( a );
            EXPECT_FALSE( atomics::atomic_load( &a ));

            EXPECT_FALSE( atomics::atomic_exchange( &a, true ));
            EXPECT_TRUE( atomics::atomic_load( &a ));
            EXPECT_TRUE( atomics::atomic_exchange( &a, false ));
            EXPECT_FALSE( atomics::atomic_load( &a ));

            bool expected = false;
            EXPECT_TRUE( atomics::atomic_compare_exchange_weak( &a, &expected, true));
            EXPECT_FALSE( expected );
            EXPECT_FALSE( atomics::atomic_compare_exchange_weak( &a, &expected, false));
            EXPECT_TRUE( expected );
            EXPECT_TRUE( atomics::atomic_load( &a ));

            atomics::atomic_store( &a, false );

            expected = false;
            EXPECT_TRUE( atomics::atomic_compare_exchange_strong( &a, &expected, true));
            EXPECT_FALSE( expected );
            EXPECT_FALSE( atomics::atomic_compare_exchange_strong( &a, &expected, false));
            EXPECT_TRUE( expected );

            EXPECT_TRUE( atomics::atomic_load( &a ));

            EXPECT_TRUE( atomics::atomic_exchange( &a, false ));
        }

        template <class AtomicBool>
        void do_test_atomic_bool( AtomicBool& a, atomics::memory_order order )
        {
            const atomics::memory_order oLoad = convert_to_load_order( order );
            const atomics::memory_order oStore = convert_to_store_order( order );
            const atomics::memory_order oExchange = convert_to_exchange_order( order );

            EXPECT_TRUE( atomics::atomic_is_lock_free( &a ));
            atomics::atomic_store_explicit( &a, false, oStore );
            EXPECT_FALSE( a == false );
            EXPECT_FALSE( atomics::atomic_load_explicit( &a, oLoad ));

            EXPECT_FALSE( atomics::atomic_exchange_explicit( &a, true, oExchange ));
            EXPECT_TRUE( atomics::atomic_load_explicit( &a, oLoad ));
            EXPECT_TRUE( atomics::atomic_exchange_explicit( &a, false, oExchange ));
            EXPECT_FALSE( atomics::atomic_load_explicit( &a, oLoad ));

            bool expected = false;
            EXPECT_TRUE( atomics::atomic_compare_exchange_weak_explicit( &a, &expected, true, order, atomics::memory_order_relaxed));
            EXPECT_FALSE( expected );
            EXPECT_FALSE( atomics::atomic_compare_exchange_weak_explicit( &a, &expected, false, order, atomics::memory_order_relaxed));
            EXPECT_TRUE( expected );
            EXPECT_TRUE( atomics::atomic_load_explicit( &a, oLoad ));

            atomics::atomic_store( &a, false );

            expected = false;
            EXPECT_TRUE( atomics::atomic_compare_exchange_strong_explicit( &a, &expected, true, order, atomics::memory_order_relaxed));
            EXPECT_FALSE( expected );
            EXPECT_FALSE( atomics::atomic_compare_exchange_strong_explicit( &a, &expected, false, order, atomics::memory_order_relaxed));
            EXPECT_TRUE( expected );

            EXPECT_TRUE( atomics::atomic_load_explicit( &a, oLoad ));

            EXPECT_TRUE( atomics::atomic_exchange_explicit( &a, false, oExchange ));
        }

        template <typename Atomic, typename Integral>
        void test_atomic_pointer_for_( Atomic& a, Integral * arr, Integral aSize, atomics::memory_order order )
        {
            typedef Integral integral_type;
            atomics::memory_order oLoad = convert_to_load_order(order);
            atomics::memory_order oStore = convert_to_store_order(order);
            integral_type *  p;

            atomics::atomic_store_explicit( &a, arr, oStore );
            EXPECT_EQ( *atomics::atomic_load_explicit( &a, oLoad ), 1 );

            p = arr;
            EXPECT_TRUE( atomics::atomic_compare_exchange_weak_explicit( &a, &p, arr + 5, order, atomics::memory_order_relaxed ));
            EXPECT_EQ( p, arr + 0 );
            EXPECT_EQ( *p, 1 );
            EXPECT_FALSE( atomics::atomic_compare_exchange_weak_explicit( &a, &p, arr + 3, order, atomics::memory_order_relaxed ));
            EXPECT_EQ( p, arr + 5 );
            EXPECT_EQ( *p, 6 );

            EXPECT_TRUE( atomics::atomic_compare_exchange_strong_explicit( &a, &p, arr + 3, order, atomics::memory_order_relaxed ));
            EXPECT_EQ( p, arr + 5 );
            EXPECT_EQ( *p, 6 );
            EXPECT_FALSE( atomics::atomic_compare_exchange_strong_explicit( &a, &p, arr + 5, order, atomics::memory_order_relaxed ));
            EXPECT_EQ( p, arr + 3 );
            EXPECT_EQ( *p, 4 );

            EXPECT_EQ( atomics::atomic_exchange_explicit( &a, arr, order ), arr + 3 );
            EXPECT_EQ( atomics::atomic_load_explicit( &a, oLoad ), arr );
            EXPECT_EQ( *atomics::atomic_load_explicit( &a, oLoad ), 1 );

            for ( integral_type i = 1; i < aSize; ++i ) {
                integral_type * p = atomics::atomic_load_explicit( &a, oLoad );
                EXPECT_EQ( *p, i );
                EXPECT_EQ( atomics::atomic_fetch_add_explicit( &a, 1, order ), p );
                EXPECT_EQ( *atomics::atomic_load_explicit( &a, oLoad ), i + 1 );
            }

            for ( integral_type i = aSize; i > 1; --i ) {
                integral_type * p = atomics::atomic_load_explicit( &a, oLoad );
                EXPECT_EQ( *p, i  );
                EXPECT_EQ( atomics::atomic_fetch_sub_explicit( &a, 1, order ), p );
                EXPECT_EQ( *atomics::atomic_load_explicit( &a, oLoad ), i - 1 );
            }
        }

        template <typename Integral, bool Volatile>
        void test_atomic_pointer_for()
        {
            typedef Integral integral_type;
            typedef typename add_volatile<atomics::atomic< integral_type *>, Volatile>::type    atomic_pointer;

            integral_type   arr[8];
            const integral_type aSize = sizeof(arr)/sizeof(arr[0]);
            for ( integral_type i = 0; i < aSize; ++i ) {
                arr[size_t(i)] = i + 1;
            }

            atomic_pointer  a;
            integral_type *  p;

            atomics::atomic_store( &a, arr );
            EXPECT_EQ( *atomics::atomic_load( &a ), 1 );

            p = arr;
            EXPECT_TRUE( atomics::atomic_compare_exchange_weak( &a, &p, arr + 5 ));
            EXPECT_EQ( p, arr + 0 );
            EXPECT_FALSE( atomics::atomic_compare_exchange_weak( &a, &p, arr + 3 ));
            EXPECT_EQ( p, arr + 5 );

            EXPECT_TRUE( atomics::atomic_compare_exchange_strong( &a, &p, arr + 3 ));
            EXPECT_EQ( p, arr + 5 );
            EXPECT_FALSE( atomics::atomic_compare_exchange_strong( &a, &p, arr + 5 ));
            EXPECT_EQ( p, arr + 3 );

            EXPECT_EQ( atomics::atomic_exchange( &a, arr ), arr + 3 );
            EXPECT_EQ( atomics::atomic_load( &a ), arr );
            EXPECT_EQ( *atomics::atomic_load( &a ), 1 );

            for ( integral_type i = 1; i < aSize; ++i ) {
                integral_type * p = atomics::atomic_load( &a );
                EXPECT_EQ( *p, i );
                EXPECT_EQ( atomics::atomic_fetch_add( &a, 1 ), p );
                EXPECT_EQ( *atomics::atomic_load( &a ), i + 1 );
            }

            for ( integral_type i = aSize; i > 1; --i ) {
                integral_type * p = atomics::atomic_load( &a );
                EXPECT_EQ( *p, i );
                EXPECT_EQ( atomics::atomic_fetch_sub( &a, 1 ), p );
                EXPECT_EQ( *atomics::atomic_load( &a ), i - 1 );
            }

            test_atomic_pointer_for_( a, arr, aSize, atomics::memory_order_relaxed );
            test_atomic_pointer_for_( a, arr, aSize, atomics::memory_order_acquire );
            test_atomic_pointer_for_( a, arr, aSize, atomics::memory_order_release );
            test_atomic_pointer_for_( a, arr, aSize, atomics::memory_order_acq_rel );
            test_atomic_pointer_for_( a, arr, aSize, atomics::memory_order_seq_cst );

        }

        template <typename Atomic>
        void do_test_atomic_pointer_void_( Atomic& a, char * arr, char aSize, atomics::memory_order order )
        {
            atomics::memory_order oLoad = convert_to_load_order(order);
            atomics::memory_order oStore = convert_to_store_order(order);
            char *  p;

            atomics::atomic_store_explicit( &a, (void *) arr, oStore );
            EXPECT_EQ( *reinterpret_cast<char *>(atomics::atomic_load_explicit( &a, oLoad )), 1 );

            p = arr;
            EXPECT_TRUE( atomics::atomic_compare_exchange_weak_explicit( &a, (void **) &p, (void *)(arr + 5), order, atomics::memory_order_relaxed ));
            EXPECT_EQ( p, arr + 0 );
            EXPECT_EQ( *p, 1 );
            EXPECT_FALSE( atomics::atomic_compare_exchange_weak_explicit( &a, (void **) &p, (void *)(arr + 3), order, atomics::memory_order_relaxed ));
            EXPECT_EQ( p, arr + 5 );
            EXPECT_EQ( *p, 6 );

            EXPECT_TRUE( atomics::atomic_compare_exchange_strong_explicit( &a, (void **) &p, (void *)(arr + 3), order, atomics::memory_order_relaxed ));
            EXPECT_EQ( p, arr + 5 );
            EXPECT_EQ( *p, 6 );
            EXPECT_FALSE( atomics::atomic_compare_exchange_strong_explicit( &a, (void **) &p, (void *)(arr + 5), order, atomics::memory_order_relaxed ));
            EXPECT_EQ( p, arr + 3 );
            EXPECT_EQ( *p, 4 );

            EXPECT_EQ( reinterpret_cast<char *>(atomics::atomic_exchange_explicit( &a, (void *) arr, order )), arr + 3 );
            EXPECT_EQ( reinterpret_cast<char *>(atomics::atomic_load_explicit( &a, oLoad )), arr );
            EXPECT_EQ( *reinterpret_cast<char *>(atomics::atomic_load_explicit( &a, oLoad )), 1 );

            for ( char i = 1; i < aSize; ++i ) {
                EXPECT_EQ( *reinterpret_cast<char *>(atomics::atomic_load_explicit( &a, oLoad )), i );
                atomics::atomic_fetch_add_explicit( &a, 1, order );
                EXPECT_EQ( *reinterpret_cast<char *>(atomics::atomic_load_explicit( &a, oLoad )), i + 1 );
            }

            for ( char i = aSize; i > 1; --i ) {
                EXPECT_EQ( *reinterpret_cast<char *>(atomics::atomic_load_explicit( &a, oLoad )), i );
                atomics::atomic_fetch_sub_explicit( &a, 1, order );
                EXPECT_EQ( *reinterpret_cast<char *>(atomics::atomic_load_explicit( &a, oLoad )), i - 1 );
            }
        }

        template <bool Volatile>
        void do_test_atomic_pointer_void()
        {
            typedef typename add_volatile<atomics::atomic< void *>, Volatile>::type    atomic_pointer;

            char   arr[8];
            const char aSize = sizeof(arr)/sizeof(arr[0]);
            for ( char i = 0; i < aSize; ++i ) {
                arr[unsigned(i)] = i + 1;
            }

            atomic_pointer  a;
            char *  p;

            atomics::atomic_store( &a, (void *) arr );
            EXPECT_EQ( *reinterpret_cast<char *>(atomics::atomic_load( &a )), 1 );

            p = arr;
            EXPECT_TRUE( atomics::atomic_compare_exchange_weak( &a, (void **) &p, (void *)(arr + 5)));
            EXPECT_EQ( p, arr + 0 );
            EXPECT_FALSE( atomics::atomic_compare_exchange_weak( &a, (void **) &p, (void *)(arr + 3)));
            EXPECT_EQ( p, arr + 5 );

            EXPECT_TRUE( atomics::atomic_compare_exchange_strong( &a, (void **) &p, (void *)(arr + 3)));
            EXPECT_EQ( p, arr + 5 );
            EXPECT_FALSE( atomics::atomic_compare_exchange_strong( &a, (void **) &p, (void *)(arr + 5)));
            EXPECT_EQ( p, arr + 3 );

            EXPECT_EQ( reinterpret_cast<char *>( atomics::atomic_exchange( &a, (void *) arr )), arr + 3 );
            EXPECT_EQ( reinterpret_cast<char *>( atomics::atomic_load( &a )), arr );
            EXPECT_EQ( *reinterpret_cast<char *>(atomics::atomic_load( &a )), 1 );

            for ( char i = 1; i < aSize; ++i ) {
                EXPECT_EQ( *reinterpret_cast<char *>(atomics::atomic_load( &a )), i );
                atomics::atomic_fetch_add( &a, 1 );
                EXPECT_EQ( *reinterpret_cast<char *>(atomics::atomic_load( &a )), i + 1 );
            }

            for ( char i = aSize; i > 1; --i ) {
                EXPECT_EQ( *reinterpret_cast<char *>(atomics::atomic_load( &a )), i );
                atomics::atomic_fetch_sub( &a, 1 );
                EXPECT_EQ( *reinterpret_cast<char *>(atomics::atomic_load( &a )), i - 1 );
            }

            do_test_atomic_pointer_void_( a, arr, aSize, atomics::memory_order_relaxed );
            do_test_atomic_pointer_void_( a, arr, aSize, atomics::memory_order_acquire );
            do_test_atomic_pointer_void_( a, arr, aSize, atomics::memory_order_release );
            do_test_atomic_pointer_void_( a, arr, aSize, atomics::memory_order_acq_rel );
            do_test_atomic_pointer_void_( a, arr, aSize, atomics::memory_order_seq_cst );
        }

    public:
        void test_atomic_flag()
        {
            atomics::atomic_flag flags[8];
            for ( size_t i = 0; i < sizeof(flags)/sizeof(flags[0]); ++i )
                do_test_atomic_flag( flags[i] );
        }
        void test_atomic_flag_volatile()
        {
            atomics::atomic_flag volatile flags[8];
            for ( size_t i = 0; i < sizeof(flags)/sizeof(flags[0]); ++i )
                do_test_atomic_flag( flags[i] );
        }

        template <typename AtomicBool>
        void test_atomic_bool_()
        {
            AtomicBool a[8];
            for ( size_t i = 0; i < sizeof(a)/sizeof(a[0]); ++i ) {
                do_test_atomic_bool( a[i] );

                do_test_atomic_bool( a[i], atomics::memory_order_relaxed );
                do_test_atomic_bool( a[i], atomics::memory_order_acquire );
                do_test_atomic_bool( a[i], atomics::memory_order_release );
                do_test_atomic_bool( a[i], atomics::memory_order_acq_rel );
                do_test_atomic_bool( a[i], atomics::memory_order_seq_cst );
            }
        }

        void test_atomic_bool()
        {
            test_atomic_bool_<atomics::atomic<bool> >();
        }
        void test_atomic_bool_volatile()
        {
            test_atomic_bool_<atomics::atomic<bool> volatile >();
        }
    };

    TEST_F( cxx11_atomic_func, atomic_char )
    {
        test_atomic_integral<char>();
    }
    TEST_F( cxx11_atomic_func, atomic_char_volatile )
    {
        test_atomic_integral_volatile<char>();
    }
    TEST_F( cxx11_atomic_func, atomic_unsigned_char )
    {
        test_atomic_integral<unsigned char>();
    }
    TEST_F( cxx11_atomic_func, atomic_unsigned_char_volatile )
    {
        test_atomic_integral_volatile<unsigned char>();
    }
    TEST_F( cxx11_atomic_func, atomic_signed_char )
    {
        test_atomic_integral<signed char>();
    }
    TEST_F( cxx11_atomic_func, atomic_signed_char_volatile )
    {
        test_atomic_integral_volatile<signed char>();
    }
    TEST_F( cxx11_atomic_func, atomic_short_int )
    {
        test_atomic_integral<short int>();
    }
    TEST_F( cxx11_atomic_func, atomic_short_int_volatile )
    {
        test_atomic_integral_volatile<short int>();
    }
    TEST_F( cxx11_atomic_func, atomic_unsigned_short_int )
    {
        test_atomic_integral<unsigned short int>();
    }
    TEST_F( cxx11_atomic_func, atomic_unsigned_short_int_volatile )
    {
        test_atomic_integral_volatile<unsigned short int>();
    }
    TEST_F( cxx11_atomic_func, atomic_int )
    {
        test_atomic_integral<int>();
    }
    TEST_F( cxx11_atomic_func, atomic_int_volatile )
    {
        test_atomic_integral_volatile<int>();
    }
    TEST_F( cxx11_atomic_func, atomic_unsigned_int )
    {
        test_atomic_integral<unsigned int>();
    }
    TEST_F( cxx11_atomic_func, atomic_unsigned_int_volatile )
    {
        test_atomic_integral_volatile<unsigned int>();
    }
    TEST_F( cxx11_atomic_func, atomic_long )
    {
        test_atomic_integral<long>();
    }
    TEST_F( cxx11_atomic_func, atomic_long_volatile )
    {
        test_atomic_integral_volatile<long>();
    }
    TEST_F( cxx11_atomic_func, atomic_unsigned_long )
    {
        test_atomic_integral<unsigned long>();
    }
    TEST_F( cxx11_atomic_func, atomic_unsigned_long_volatile )
    {
        test_atomic_integral_volatile<unsigned long>();
    }
    TEST_F( cxx11_atomic_func, atomic_long_long )
    {
        test_atomic_integral<long long>();
    }
    TEST_F( cxx11_atomic_func, atomic_long_long_volatile )
    {
        test_atomic_integral_volatile<long long>();
    }
    TEST_F( cxx11_atomic_func, atomic_unsigned_long_long )
    {
        test_atomic_integral<unsigned long long>();
    }
    TEST_F( cxx11_atomic_func, atomic_unsigned_long_long_volatile )
    {
        test_atomic_integral_volatile<unsigned long long>();
    }

#if !( CDS_COMPILER == CDS_COMPILER_CLANG && CDS_COMPILER_VERSION < 40000 )
    //clang error with atomic<void*> fetch_add/fetch_sub
    TEST_F( cxx11_atomic_func, atomic_pointer_void )
    {
        do_test_atomic_pointer_void<false>();
    }
    TEST_F( cxx11_atomic_func, atomic_pointer_void_volatile )
    {
        do_test_atomic_pointer_void<true>();
    }
#endif

    TEST_F( cxx11_atomic_func, atomic_pointer_char )
    {
        test_atomic_pointer_for<char, false>();
    }
    TEST_F( cxx11_atomic_func, atomic_pointer_char_volatile )
    {
        test_atomic_pointer_for<char, true>();
    }
    TEST_F( cxx11_atomic_func, atomic_pointer_short )
    {
        test_atomic_pointer_for<short, false>();
    }
    TEST_F( cxx11_atomic_func, atomic_pointer_short_volatile )
    {
        test_atomic_pointer_for<short, true>();
    }
    TEST_F( cxx11_atomic_func, atomic_pointer_int )
    {
        test_atomic_pointer_for<int, false>();
    }
    TEST_F( cxx11_atomic_func, atomic_pointer_int_volatile )
    {
        test_atomic_pointer_for<int, true>();
    }
    TEST_F( cxx11_atomic_func, atomic_pointer_long )
    {
        test_atomic_pointer_for<long, false>();
    }
    TEST_F( cxx11_atomic_func, atomic_pointer_long_volatile )
    {
        test_atomic_pointer_for<long, true>();
    }
    TEST_F( cxx11_atomic_func, atomic_pointer_long_long )
    {
        test_atomic_pointer_for<long long, false>();
    }
    TEST_F( cxx11_atomic_func, atomic_pointer_long_long_volatile )
    {
        test_atomic_pointer_for<long long, true>();
    }

    TEST_F( cxx11_atomic_func, test_atomic_fence )
    {
        atomics::atomic_thread_fence(atomics::memory_order_relaxed );
        atomics::atomic_thread_fence(atomics::memory_order_acquire );
        atomics::atomic_thread_fence(atomics::memory_order_release );
        atomics::atomic_thread_fence(atomics::memory_order_acq_rel );
        atomics::atomic_thread_fence(atomics::memory_order_seq_cst );

        atomics::atomic_signal_fence(atomics::memory_order_relaxed );
        atomics::atomic_signal_fence(atomics::memory_order_acquire );
        atomics::atomic_signal_fence(atomics::memory_order_release );
        atomics::atomic_signal_fence(atomics::memory_order_acq_rel );
        atomics::atomic_signal_fence(atomics::memory_order_seq_cst );
    }
}   // namespace


#endif // #ifndef CDS_USE_BOOST_ATOMIC
