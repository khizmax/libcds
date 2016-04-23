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

#include <gtest/gtest.h>
#include <cds/algo/atomic.h>
#include "cxx11_convert_memory_order.h"

namespace {
    class cxx11_atomic_class: public ::testing::Test
    {
    protected:
        template <typename AtomicFlag>
        void do_test_atomic_flag_mo( AtomicFlag& f, atomics::memory_order order )
        {
            atomics::memory_order mo_clear = convert_to_store_order(order);
            for ( int i = 0; i < 5; ++i ) {
                EXPECT_TRUE( !f.test_and_set( order ));
                EXPECT_TRUE( f.test_and_set( order ) );
                f.clear( mo_clear );
            }
        }

        template <typename AtomicFlag>
        void do_test_atomic_flag( AtomicFlag& f)
        {
            f.clear();

            for ( int i = 0; i < 5; ++i ) {
                EXPECT_TRUE( !f.test_and_set());
                EXPECT_TRUE( f.test_and_set() );
                f.clear();
            }

            do_test_atomic_flag_mo( f, atomics::memory_order_relaxed );
            //do_test_atomic_flag_mo( f, atomics::memory_order_consume );
            do_test_atomic_flag_mo( f, atomics::memory_order_acquire );
            do_test_atomic_flag_mo( f, atomics::memory_order_release );
            do_test_atomic_flag_mo( f, atomics::memory_order_acq_rel );
            do_test_atomic_flag_mo( f, atomics::memory_order_seq_cst );
        }

        template <class Atomic, typename Integral>
        void do_test_atomic_type(Atomic& a)
        {
            typedef Integral    integral_type;

            EXPECT_TRUE( a.is_lock_free() );
            a.store( (integral_type) 0 );
            EXPECT_EQ( a, 0 );
            EXPECT_EQ( a.load(), 0 );

            for ( size_t nByte = 0; nByte < sizeof(Integral); ++nByte ) {
                integral_type n = integral_type(42) << (nByte * 8);
                EXPECT_EQ( a.exchange( n ), 0 );
                EXPECT_EQ( a, n );
                EXPECT_EQ( a.exchange( (integral_type) 0 ), n );
                EXPECT_EQ( a.load(), 0 );
            }

            integral_type prev = a.load();
            for ( size_t nByte = 0; nByte < sizeof(Integral); ++nByte ) {
                integral_type n = integral_type(42) << (nByte * 8);
                integral_type expected = prev;

                EXPECT_TRUE( a.compare_exchange_weak( expected, n));
                EXPECT_EQ( expected, prev );
                EXPECT_FALSE( a.compare_exchange_weak( expected, n));
                EXPECT_EQ( expected, n );

                prev = n;
                EXPECT_EQ( a, n );
            }

            a = (integral_type) 0;

            prev = a;
            for ( size_t nByte = 0; nByte < sizeof(Integral); ++nByte ) {
                integral_type n = integral_type(42) << (nByte * 8);
                integral_type expected = prev;

                EXPECT_TRUE( a.compare_exchange_strong( expected, n));
                EXPECT_EQ( expected, prev );
                EXPECT_FALSE( a.compare_exchange_strong( expected, n));
                EXPECT_EQ( expected, n );

                prev = n;
                EXPECT_EQ( a.load(), n );
            }

            EXPECT_EQ( a.exchange( (integral_type) 0 ), prev );
        }

        template <class Atomic, typename Integral>
        void do_test_atomic_integral(Atomic& a)
        {
            do_test_atomic_type< Atomic, Integral >(a);

            typedef Integral    integral_type;

            // fetch_xxx testing
            a.store( (integral_type) 0 );

            // fetch_add
            for ( size_t nByte = 0; nByte < sizeof(integral_type); ++nByte )
            {
                integral_type prev = a.load();
                integral_type n = integral_type(42) << (nByte * 8);

                EXPECT_EQ( a.fetch_add(n), prev);
            }

            // fetch_sub
            for ( size_t nByte = sizeof(integral_type); nByte > 0; --nByte )
            {
                integral_type prev = a.load();
                integral_type n = integral_type(42) << ((nByte - 1) * 8);

                EXPECT_EQ( a.fetch_sub(n), prev);
            }
            EXPECT_EQ( a.load(), 0 );

            // fetch_or / fetc_xor / fetch_and
            for ( size_t nBit = 0; nBit < sizeof(integral_type) * 8; ++nBit )
            {
                integral_type prev = a.load()  ;;
                integral_type mask = integral_type(1) << nBit;

                EXPECT_EQ( a.fetch_or( mask ), prev );
                prev = a.load();
                EXPECT_EQ( ( prev & mask), mask);

                EXPECT_EQ( a.fetch_and( (integral_type) ~mask ), prev );
                prev = a.load();
                EXPECT_EQ( integral_type(prev & mask), integral_type(0));

                EXPECT_EQ( a.fetch_xor( mask ), prev );
                prev = a.load();
                EXPECT_EQ( integral_type( prev & mask), mask);
            }
            EXPECT_EQ( a.load(), (integral_type) -1 );


            // op= testing
            a = (integral_type) 0;

            // +=
            for ( size_t nByte = 0; nByte < sizeof(integral_type); ++nByte )
            {
                integral_type prev = a;
                integral_type n = integral_type(42) << (nByte * 8);

                EXPECT_EQ( (a += n), (prev + n));
            }

            // -=
            for ( size_t nByte = sizeof(integral_type); nByte > 0; --nByte )
            {
                integral_type prev = a;
                integral_type n = integral_type(42) << ((nByte - 1) * 8);

                EXPECT_EQ( (a -= n),  prev - n );
            }
            EXPECT_EQ( a.load(), 0 );

            // |= / ^= / &=
            for ( size_t nBit = 0; nBit < sizeof(integral_type) * 8; ++nBit )
            {
                integral_type prev = a;
                integral_type mask = integral_type(1) << nBit;

                EXPECT_EQ( (a |= mask ), (prev | mask ));
                prev = a;
                EXPECT_EQ( ( prev & mask), mask);

                EXPECT_EQ( (a &= (integral_type) ~mask ), ( prev & (integral_type) ~mask ));
                prev = a;
                EXPECT_EQ( ( prev & mask), 0);

                EXPECT_EQ( (a ^= mask ), (prev ^ mask ));
                prev = a;
                EXPECT_EQ( ( prev & mask), mask);
            }
            EXPECT_EQ( a, (integral_type) -1 );
        }

        template <class Atomic, typename Integral>
        void do_test_atomic_type( Atomic& a, atomics::memory_order order )
        {
            typedef Integral    integral_type;

            const atomics::memory_order oLoad = convert_to_load_order( order );
            const atomics::memory_order oStore = convert_to_store_order( order );

            EXPECT_TRUE( a.is_lock_free() );
            a.store((integral_type) 0, oStore );
            EXPECT_EQ( a, 0 );
            EXPECT_EQ( a.load( oLoad ), 0 );

            for ( size_t nByte = 0; nByte < sizeof(Integral); ++nByte ) {
                integral_type n = integral_type(42) << (nByte * 8);
                EXPECT_EQ( a.exchange( n, order ), 0 );
                EXPECT_EQ( a.load( oLoad ), n );
                EXPECT_EQ( a.exchange( (integral_type) 0, order ), n );
                EXPECT_EQ( a.load( oLoad ), 0 );
            }

            integral_type prev = a.load( oLoad );
            for ( size_t nByte = 0; nByte < sizeof(Integral); ++nByte ) {
                integral_type n = integral_type(42) << (nByte * 8);
                integral_type expected = prev;

                EXPECT_TRUE( a.compare_exchange_weak( expected, n, order, atomics::memory_order_relaxed));
                EXPECT_EQ( expected, prev );
                EXPECT_FALSE( a.compare_exchange_weak( expected, n, order, atomics::memory_order_relaxed));
                EXPECT_EQ( expected, n );

                prev = n;
                EXPECT_EQ( a.load( oLoad ), n );
            }

            a.store( (integral_type) 0, oStore );

            prev = a.load( oLoad );
            for ( size_t nByte = 0; nByte < sizeof(Integral); ++nByte ) {
                integral_type n = integral_type(42) << (nByte * 8);
                integral_type expected = prev;

                EXPECT_TRUE( a.compare_exchange_strong( expected, n, order, atomics::memory_order_relaxed));
                EXPECT_EQ( expected, prev );
                EXPECT_FALSE( a.compare_exchange_strong( expected, n, order, atomics::memory_order_relaxed));
                EXPECT_EQ( expected, n );

                prev = n;
                EXPECT_EQ( a.load( oLoad ), n );
            }

            EXPECT_EQ( a.exchange( (integral_type) 0, order ), prev );
        }

        template <class Atomic, typename Integral>
        void do_test_atomic_integral( Atomic& a, atomics::memory_order order )
        {
            do_test_atomic_type< Atomic, Integral >( a, order );

            typedef Integral    integral_type;

            const atomics::memory_order oLoad = convert_to_load_order( order );
            const atomics::memory_order oStore = convert_to_store_order( order );

            // fetch_xxx testing
            a.store( (integral_type) 0, oStore );

            // fetch_add
            for ( size_t nByte = 0; nByte < sizeof(integral_type); ++nByte )
            {
                integral_type prev = a.load( oLoad );
                integral_type n = integral_type(42) << (nByte * 8);

                EXPECT_EQ( a.fetch_add( n, order), prev);
            }

            // fetch_sub
            for ( size_t nByte = sizeof(integral_type); nByte > 0; --nByte )
            {
                integral_type prev = a.load( oLoad );
                integral_type n = integral_type(42) << ((nByte - 1) * 8);

                EXPECT_EQ( a.fetch_sub( n, order ), prev);
            }
            EXPECT_EQ( a.load( oLoad ), 0 );

            // fetch_or / fetc_xor / fetch_and
            for ( size_t nBit = 0; nBit < sizeof(integral_type) * 8; ++nBit )
            {
                integral_type prev = a.load( oLoad )  ;;
                integral_type mask = integral_type(1) << nBit;

                EXPECT_EQ( a.fetch_or( mask, order ), prev );
                prev = a.load( oLoad );
                EXPECT_EQ( ( prev & mask), mask);

                EXPECT_EQ( a.fetch_and( (integral_type) ~mask, order ), prev );
                prev = a.load( oLoad );
                EXPECT_EQ( ( prev & mask), 0);

                EXPECT_EQ( a.fetch_xor( mask, order ), prev );
                prev = a.load( oLoad );
                EXPECT_EQ( ( prev & mask), mask);
            }
            EXPECT_EQ( a.load( oLoad ), (integral_type) -1 );
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
            typedef atomics::atomic<Integral> atomic_type;

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
        void do_test_atomic_bool( AtomicBool& a )
        {
            EXPECT_TRUE( a.is_lock_free() );
            a.store( false );
            EXPECT_FALSE( a );
            EXPECT_FALSE( a.load());

            EXPECT_FALSE( a.exchange( true ));
            EXPECT_TRUE( a.load());
            EXPECT_TRUE( a.exchange( false ));
            EXPECT_FALSE( a.load());

            bool expected = false;
            EXPECT_TRUE( a.compare_exchange_weak( expected, true));
            EXPECT_FALSE( expected );
            EXPECT_FALSE( a.compare_exchange_weak( expected, false));
            EXPECT_TRUE( expected );
            EXPECT_TRUE( a.load() );

            a.store( false );

            expected = false;
            EXPECT_TRUE( a.compare_exchange_strong( expected, true));
            EXPECT_FALSE( expected );
            EXPECT_FALSE( a.compare_exchange_strong( expected, false));
            EXPECT_TRUE( expected );

            EXPECT_TRUE( a.load());

            EXPECT_TRUE( a.exchange( false ));
        }

        template <class AtomicBool>
        void do_test_atomic_bool( AtomicBool& a, atomics::memory_order order )
        {
            const atomics::memory_order oLoad = convert_to_load_order( order );
            const atomics::memory_order oStore = convert_to_store_order( order );
            const atomics::memory_order oExchange = convert_to_exchange_order( order );

            EXPECT_TRUE( a.is_lock_free() );
            a.store( false, oStore );
            EXPECT_FALSE( a );
            EXPECT_FALSE( a.load( oLoad ));

            EXPECT_FALSE( a.exchange( true, oExchange ));
            EXPECT_TRUE( a.load( oLoad ));
            EXPECT_TRUE( a.exchange( false, oExchange ));
            EXPECT_FALSE( a.load( oLoad ));

            bool expected = false;
            EXPECT_TRUE( a.compare_exchange_weak( expected, true, order, atomics::memory_order_relaxed));
            EXPECT_FALSE( expected );
            EXPECT_FALSE( a.compare_exchange_weak( expected, false, order, atomics::memory_order_relaxed));
            EXPECT_TRUE( expected );
            EXPECT_TRUE( a.load( oLoad ));

            //a = bool(false);
            a.store( false, oStore );

            expected = false;
            EXPECT_TRUE( a.compare_exchange_strong( expected, true, order, atomics::memory_order_relaxed));
            EXPECT_FALSE( expected );
            EXPECT_FALSE( a.compare_exchange_strong( expected, false, order, atomics::memory_order_relaxed));
            EXPECT_TRUE( expected );

            EXPECT_TRUE( a.load( oLoad ));

            EXPECT_TRUE( a.exchange( false, oExchange ));
        }


        template <typename Atomic>
        void do_test_atomic_pointer_void_( Atomic& a, char * arr, char aSize, atomics::memory_order order )
        {
            atomics::memory_order oLoad = convert_to_load_order(order);
            atomics::memory_order oStore = convert_to_store_order(order);
            void *  p;

            a.store( (void *) arr, oStore );
            EXPECT_EQ( *reinterpret_cast<char *>(a.load( oLoad )), 1 );

            p = arr;
            EXPECT_TRUE( a.compare_exchange_weak( p, (void *)(arr + 5), order, atomics::memory_order_relaxed ));
            EXPECT_EQ( p, arr + 0 );
            EXPECT_EQ( *reinterpret_cast<char *>(p), 1 );
            EXPECT_FALSE( a.compare_exchange_weak( p, (void *)(arr + 3), order, atomics::memory_order_relaxed ));
            EXPECT_EQ( p, arr + 5 );
            EXPECT_EQ( *reinterpret_cast<char *>(p), 6 );

            EXPECT_TRUE( a.compare_exchange_strong( p, (void *)(arr + 3), order, atomics::memory_order_relaxed ));
            EXPECT_EQ( p, arr + 5 );
            EXPECT_EQ( *reinterpret_cast<char *>(p), 6 );
            EXPECT_FALSE( a.compare_exchange_strong( p, (void *)(arr + 5), order, atomics::memory_order_relaxed ));
            EXPECT_EQ( p, arr + 3 );
            EXPECT_EQ( *reinterpret_cast<char *>(p), 4 );

            EXPECT_EQ( reinterpret_cast<char *>(a.exchange( (void *) arr, order )), arr + 3 );
            EXPECT_EQ( reinterpret_cast<char *>(a.load( oLoad )), arr );
            EXPECT_EQ( *reinterpret_cast<char *>(a.load( oLoad )), 1 );

            for ( char i = 1; i < aSize; ++i ) {
                EXPECT_EQ( *reinterpret_cast<char *>(a.load( oLoad )), i );
                a.fetch_add( 1, order );
                EXPECT_EQ( *reinterpret_cast<char *>(a.load( oLoad )), i + 1 );
            }

            for ( char i = aSize; i > 1; --i ) {
                EXPECT_EQ( *reinterpret_cast<char *>(a.load( oLoad )), i );
                a.fetch_sub( 1, order );
                EXPECT_EQ( *reinterpret_cast<char *>(a.load( oLoad )), i - 1 );
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
            void *  p;

            a.store( (void *) arr );
            EXPECT_EQ( *reinterpret_cast<char *>(a.load()), 1 );

            p = arr;
            EXPECT_TRUE( a.compare_exchange_weak( p, (void *)(arr + 5) ));
            EXPECT_EQ( p, arr + 0 );
            EXPECT_FALSE( a.compare_exchange_weak( p, (void *)(arr + 3) ));
            EXPECT_EQ( p, arr + 5 );

            EXPECT_TRUE( a.compare_exchange_strong( p, (void *)(arr + 3) ));
            EXPECT_EQ( p, arr + 5 );
            EXPECT_FALSE( a.compare_exchange_strong( p, (void *)(arr + 5) ));
            EXPECT_EQ( p, arr + 3 );

            EXPECT_EQ( reinterpret_cast<char *>( a.exchange( (void *) arr )), arr + 3 );
            EXPECT_EQ( reinterpret_cast<char *>( a.load()), arr );
            EXPECT_EQ( *reinterpret_cast<char *>( a.load()), 1 );

            for ( char i = 1; i < aSize; ++i ) {
                EXPECT_EQ( *reinterpret_cast<char *>(a.load()), i );
                a.fetch_add( 1 );
                EXPECT_EQ( *reinterpret_cast<char *>(a.load()), i + 1 );
            }

            for ( char i = aSize; i > 1; --i ) {
                EXPECT_EQ( *reinterpret_cast<char *>(a.load()), i );
                a.fetch_sub( 1 );
                EXPECT_EQ( *reinterpret_cast<char *>(a.load()), i - 1 );
            }

            do_test_atomic_pointer_void_( a, arr, aSize, atomics::memory_order_relaxed );
            do_test_atomic_pointer_void_( a, arr, aSize, atomics::memory_order_acquire );
            do_test_atomic_pointer_void_( a, arr, aSize, atomics::memory_order_release );
            do_test_atomic_pointer_void_( a, arr, aSize, atomics::memory_order_acq_rel );
            do_test_atomic_pointer_void_( a, arr, aSize, atomics::memory_order_seq_cst );
        }

        template <typename Atomic, typename Integral>
        void test_atomic_pointer_for_( Atomic& a, Integral * arr, Integral aSize, atomics::memory_order order )
        {
            typedef Integral integral_type;
            atomics::memory_order oLoad = convert_to_load_order(order);
            atomics::memory_order oStore = convert_to_store_order(order);
            integral_type *  p;

            a.store( arr, oStore );
            EXPECT_EQ( *a.load( oLoad ), 1 );

            p = arr;
            EXPECT_TRUE( a.compare_exchange_weak( p, arr + 5, order, atomics::memory_order_relaxed ));
            EXPECT_EQ( p, arr + 0 );
            EXPECT_EQ( *p, 1 );
            EXPECT_FALSE( a.compare_exchange_weak( p, arr + 3, order, atomics::memory_order_relaxed ));
            EXPECT_EQ( p, arr + 5 );
            EXPECT_EQ( *p, 6 );

            EXPECT_TRUE( a.compare_exchange_strong( p, arr + 3, order, atomics::memory_order_relaxed ));
            EXPECT_EQ( p, arr + 5 );
            EXPECT_EQ( *p, 6 );
            EXPECT_FALSE( a.compare_exchange_strong( p, arr + 5, order, atomics::memory_order_relaxed ));
            EXPECT_EQ( p, arr + 3 );
            EXPECT_EQ( *p, 4 );

            EXPECT_EQ( a.exchange( arr, order ), arr + 3 );
            EXPECT_EQ( a.load( oLoad ), arr );
            EXPECT_EQ( *a.load( oLoad ), 1 );

            for ( integral_type i = 1; i < aSize; ++i ) {
                integral_type * p = a.load();
                EXPECT_EQ( *p, i );
                EXPECT_EQ( a.fetch_add( 1, order ), p );
                EXPECT_EQ( *a.load( oLoad ), i + 1 );
            }

            for ( integral_type i = aSize; i > 1; --i ) {
                integral_type * p = a.load();
                EXPECT_EQ( *p, i  );
                EXPECT_EQ( a.fetch_sub( 1, order ), p );
                EXPECT_EQ( *a.load( oLoad ), i - 1 );
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

            a.store( arr );
            EXPECT_EQ( *a.load(), 1 );

            p = arr;
            EXPECT_TRUE( a.compare_exchange_weak( p, arr + 5 ));
            EXPECT_EQ( p, arr + 0 );
            EXPECT_EQ( *p, 1 );
            EXPECT_FALSE( a.compare_exchange_weak( p, arr + 3 ));
            EXPECT_EQ( p, arr + 5 );
            EXPECT_EQ( *p, 6 );

            EXPECT_TRUE( a.compare_exchange_strong( p, arr + 3 ));
            EXPECT_EQ( p, arr + 5 );
            EXPECT_EQ( *p, 6 );
            EXPECT_FALSE( a.compare_exchange_strong( p, arr + 5 ));
            EXPECT_EQ( p, arr + 3 );
            EXPECT_EQ( *p, 4 );

            EXPECT_EQ( a.exchange( arr ), arr + 3 );
            EXPECT_EQ( a.load(), arr );
            EXPECT_EQ( *a.load(), 1 );

            for ( integral_type i = 1; i < aSize; ++i ) {
                integral_type * p = a.load();
                EXPECT_EQ( *p, i );
                integral_type * pa = a.fetch_add( 1 );
                EXPECT_EQ( pa, p );
                EXPECT_EQ( *a.load(), i + 1 );
            }

            for ( integral_type i = aSize; i > 1; --i ) {
                integral_type * p = a.load();
                EXPECT_EQ( *p, i  );
                EXPECT_EQ( a.fetch_sub( 1 ), p );
                EXPECT_EQ( *a.load(), i - 1 );
            }

            test_atomic_pointer_for_( a, arr, aSize, atomics::memory_order_relaxed );
            test_atomic_pointer_for_( a, arr, aSize, atomics::memory_order_acquire );
            test_atomic_pointer_for_( a, arr, aSize, atomics::memory_order_release );
            test_atomic_pointer_for_( a, arr, aSize, atomics::memory_order_acq_rel );
            test_atomic_pointer_for_( a, arr, aSize, atomics::memory_order_seq_cst );
        }

    public:
        void test_atomic_flag()
        {
            // Array to test different alignment

            atomics::atomic_flag flags[8];
            for ( size_t i = 0; i < sizeof(flags)/sizeof(flags[0]); ++i )
                do_test_atomic_flag( flags[i] );
        }

        void test_atomic_flag_volatile()
        {
            // Array to test different alignment

            atomics::atomic_flag volatile flags[8];
            for ( size_t i = 0; i < sizeof(flags)/sizeof(flags[0]); ++i )
                do_test_atomic_flag( flags[i] );
        }

        template <typename AtomicBool>
        void test_atomic_bool_()
        {
            // Array to test different alignment
            AtomicBool  a[8];

            for ( size_t i = 0; i < sizeof(a)/sizeof(a[0]); ++i ) {
                do_test_atomic_bool( a[i] );

                do_test_atomic_bool( a[i], atomics::memory_order_relaxed );
                //do_test_atomic_bool( a[i], atomics::memory_order_consume );
                do_test_atomic_bool( a[i], atomics::memory_order_acquire );
                do_test_atomic_bool( a[i], atomics::memory_order_release );
                do_test_atomic_bool( a[i], atomics::memory_order_acq_rel );
                do_test_atomic_bool( a[i], atomics::memory_order_seq_cst );
            }
        }

        void test_atomic_bool()
        {
            test_atomic_bool_< atomics::atomic<bool> >();
        }
        void test_atomic_bool_volatile()
        {
            test_atomic_bool_< atomics::atomic<bool> volatile >();
        }
    };

    TEST_F( cxx11_atomic_class, atomic_char )
    {
        test_atomic_integral<char>();
    }

    TEST_F( cxx11_atomic_class, atomic_signed_char )
    {
        test_atomic_integral<signed char>();
    }

    TEST_F( cxx11_atomic_class, atomic_unsigned_char )
    {
        test_atomic_integral<unsigned char>();
    }

    TEST_F( cxx11_atomic_class, atomic_short_int )
    {
        test_atomic_integral<short int>();
    }

    TEST_F( cxx11_atomic_class, atomic_signed_short_int )
    {
        test_atomic_integral<signed short int>();
    }

    TEST_F( cxx11_atomic_class, atomic_unsigned_short_int )
    {
        test_atomic_integral<unsigned short int>();
    }

    TEST_F( cxx11_atomic_class, atomic_int )
    {
        test_atomic_integral<int>();
    }

    TEST_F( cxx11_atomic_class, atomic_unsigned_int )
    {
        test_atomic_integral<unsigned int>();
    }

    TEST_F( cxx11_atomic_class, atomic_long )
    {
        test_atomic_integral<long>();
    }

    TEST_F( cxx11_atomic_class, atomic_unsigned_long )
    {
        test_atomic_integral<unsigned long>();
    }

    TEST_F( cxx11_atomic_class, atomic_long_long )
    {
        test_atomic_integral<long long>();
    }

    TEST_F( cxx11_atomic_class, atomic_unsigned_long_long )
    {
        test_atomic_integral<unsigned long long>();
    }

    TEST_F( cxx11_atomic_class, atomic_char_volatile )
    {
        test_atomic_integral_volatile<char>();
    }

    TEST_F( cxx11_atomic_class, atomic_signed_char_volatile )
    {
        test_atomic_integral_volatile<signed char>();
    }

    TEST_F( cxx11_atomic_class, atomic_unsigned_char_volatile )
    {
        test_atomic_integral_volatile<unsigned char>();
    }

    TEST_F( cxx11_atomic_class, atomic_short_int_volatile )
    {
        test_atomic_integral_volatile<short int>();
    }

    TEST_F( cxx11_atomic_class, atomic_signed_short_int_volatile )
    {
        test_atomic_integral_volatile<signed short int>();
    }

    TEST_F( cxx11_atomic_class, atomic_unsigned_short_int_volatile )
    {
        test_atomic_integral_volatile<unsigned short int>();
    }

    TEST_F( cxx11_atomic_class, atomic_int_volatile )
    {
        test_atomic_integral_volatile<int>();
    }

    TEST_F( cxx11_atomic_class, atomic_unsigned_int_volatile )
    {
        test_atomic_integral_volatile<unsigned int>();
    }

    TEST_F( cxx11_atomic_class, atomic_long_volatile )
    {
        test_atomic_integral_volatile<long>();
    }

    TEST_F( cxx11_atomic_class, atomic_unsigned_long_volatile )
    {
        test_atomic_integral_volatile<unsigned long>();
    }

    TEST_F( cxx11_atomic_class, atomic_long_long_volatile )
    {
        test_atomic_integral_volatile<long long>();
    }

    TEST_F( cxx11_atomic_class, atomic_unsigned_long_long_volatile )
    {
        test_atomic_integral_volatile<unsigned long long>();
    }

    TEST_F( cxx11_atomic_class, atomic_pointer_void )
    {
        do_test_atomic_pointer_void<false>();
    }

    TEST_F( cxx11_atomic_class, atomic_pointer_void_volatile )
    {
        do_test_atomic_pointer_void<true>();
    }

    TEST_F( cxx11_atomic_class, atomic_pointer_char )
    {
        test_atomic_pointer_for<char, false>();
    }

    TEST_F( cxx11_atomic_class, atomic_pointer_char_volatile )
    {
        test_atomic_pointer_for<char, true>();
    }

    TEST_F( cxx11_atomic_class, atomic_pointer_short )
    {
        test_atomic_pointer_for<short int, false>();
    }

    TEST_F( cxx11_atomic_class, atomic_pointer_short_volatile )
    {
        test_atomic_pointer_for<short int, true>();
    }

    TEST_F( cxx11_atomic_class, atomic_pointer_int )
    {
        test_atomic_pointer_for<int, false>();
    }

    TEST_F( cxx11_atomic_class, atomic_pointer_int_volatile )
    {
        test_atomic_pointer_for<int, true>();
    }

    TEST_F( cxx11_atomic_class, atomic_pointer_long )
    {
        test_atomic_pointer_for<long, false>();
    }

    TEST_F( cxx11_atomic_class, atomic_pointer_long_volatile )
    {
        test_atomic_pointer_for<long, true>();
    }

    TEST_F( cxx11_atomic_class, atomic_pointer_long_long )
    {
        test_atomic_pointer_for<long long, false>();
    }

    TEST_F( cxx11_atomic_class, atomic_pointer_long_long_volatile )
    {
        test_atomic_pointer_for<long long, true>();
    }
}   // namespace 
