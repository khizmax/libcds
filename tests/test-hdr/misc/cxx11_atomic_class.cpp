//$$CDS-header$$

#include "cppunit/cppunit_proxy.h"

#include <cds/algo/atomic.h>

#include "misc/cxx11_convert_memory_order.h"

namespace misc {
    class cxx11_atomic_class: public CppUnitMini::TestCase
    {
        template <typename AtomicFlag>
        void do_test_atomic_flag_mo( AtomicFlag& f, atomics::memory_order order )
        {
            atomics::memory_order mo_clear = convert_to_store_order(order);
            for ( int i = 0; i < 5; ++i ) {
                CPPUNIT_ASSERT( !f.test_and_set( order ));
                CPPUNIT_ASSERT( f.test_and_set( order ) );
                f.clear( mo_clear );
            }
        }

        template <typename AtomicFlag>
        void do_test_atomic_flag( AtomicFlag& f)
        {
            f.clear();

            for ( int i = 0; i < 5; ++i ) {
                CPPUNIT_ASSERT( !f.test_and_set());
                CPPUNIT_ASSERT( f.test_and_set() );
                f.clear();
            }

            do_test_atomic_flag_mo( f, atomics::memory_order_relaxed );
            do_test_atomic_flag_mo( f, atomics::memory_order_consume );
            do_test_atomic_flag_mo( f, atomics::memory_order_acquire );
            do_test_atomic_flag_mo( f, atomics::memory_order_release );
            do_test_atomic_flag_mo( f, atomics::memory_order_acq_rel );
            do_test_atomic_flag_mo( f, atomics::memory_order_seq_cst );
        }

        template <class Atomic, typename Integral>
        void do_test_atomic_type(Atomic& a)
        {
            typedef Integral    integral_type;

            CPPUNIT_ASSERT( a.is_lock_free() );
            a.store( (integral_type) 0 );
            CPPUNIT_ASSERT( a == 0 );
            CPPUNIT_ASSERT( a.load() == 0 );

            for ( size_t nByte = 0; nByte < sizeof(Integral); ++nByte ) {
                integral_type n = integral_type(42) << (nByte * 8);
                CPPUNIT_ASSERT( a.exchange( n ) == 0 );
                CPPUNIT_ASSERT( a == n );
                CPPUNIT_ASSERT( a.exchange( (integral_type) 0 ) == n );
                CPPUNIT_ASSERT( a.load() == 0 );
            }

            integral_type prev = a.load();
            for ( size_t nByte = 0; nByte < sizeof(Integral); ++nByte ) {
                integral_type n = integral_type(42) << (nByte * 8);
                integral_type expected = prev;

                CPPUNIT_ASSERT( a.compare_exchange_weak( expected, n));
                CPPUNIT_ASSERT( expected  == prev );
                CPPUNIT_ASSERT( !a.compare_exchange_weak( expected, n));
                CPPUNIT_ASSERT( expected  == n );

                prev = n;
                CPPUNIT_ASSERT( a == n );
            }

            a = (integral_type) 0;

            prev = a;
            for ( size_t nByte = 0; nByte < sizeof(Integral); ++nByte ) {
                integral_type n = integral_type(42) << (nByte * 8);
                integral_type expected = prev;

                CPPUNIT_ASSERT( a.compare_exchange_strong( expected, n));
                CPPUNIT_ASSERT( expected  == prev );
                CPPUNIT_ASSERT( !a.compare_exchange_strong( expected, n));
                CPPUNIT_ASSERT( expected  == n );

                prev = n;
                CPPUNIT_ASSERT( a.load() == n );
            }

            CPPUNIT_ASSERT( a.exchange( (integral_type) 0 ) == prev );
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

                CPPUNIT_ASSERT( a.fetch_add(n) == prev);
            }

            // fetch_sub
            for ( size_t nByte = sizeof(integral_type); nByte > 0; --nByte )
            {
                integral_type prev = a.load();
                integral_type n = integral_type(42) << ((nByte - 1) * 8);

                CPPUNIT_ASSERT( a.fetch_sub(n) == prev);
            }
            CPPUNIT_ASSERT( a.load() == 0 );

            // fetch_or / fetc_xor / fetch_and
            for ( size_t nBit = 0; nBit < sizeof(integral_type) * 8; ++nBit )
            {
                integral_type prev = a.load()  ;;
                integral_type mask = 1 << nBit;

                CPPUNIT_ASSERT( a.fetch_or( mask ) == prev );
                prev = a.load();
                CPPUNIT_ASSERT( ( prev & mask)  == mask);

                CPPUNIT_ASSERT( a.fetch_and( (integral_type) ~mask ) == prev );
                prev = a.load();
                CPPUNIT_ASSERT( integral_type(prev & mask) == integral_type(0));

                CPPUNIT_ASSERT( a.fetch_xor( mask ) == prev );
                prev = a.load();
                CPPUNIT_ASSERT( integral_type( prev & mask)  == mask);
            }
            CPPUNIT_ASSERT( a.load() == (integral_type) -1 );


            // op= testing
            a = (integral_type) 0;

            // +=
            for ( size_t nByte = 0; nByte < sizeof(integral_type); ++nByte )
            {
                integral_type prev = a;
                integral_type n = integral_type(42) << (nByte * 8);

                CPPUNIT_ASSERT( (a += n) == (prev + n));
            }

            // -=
            for ( size_t nByte = sizeof(integral_type); nByte > 0; --nByte )
            {
                integral_type prev = a;
                integral_type n = integral_type(42) << ((nByte - 1) * 8);

                CPPUNIT_ASSERT( (a -= n) == prev - n );
            }
            CPPUNIT_ASSERT( a.load() == 0 );

            // |= / ^= / &=
            for ( size_t nBit = 0; nBit < sizeof(integral_type) * 8; ++nBit )
            {
                integral_type prev = a;
                integral_type mask = integral_type(1) << nBit;

                CPPUNIT_ASSERT( (a |= mask ) == (prev | mask ));
                prev = a;
                CPPUNIT_ASSERT( ( prev & mask)  == mask);

                CPPUNIT_ASSERT( (a &= (integral_type) ~mask ) == ( prev & (integral_type) ~mask ));
                prev = a;
                CPPUNIT_ASSERT( ( prev & mask)  == 0);

                CPPUNIT_ASSERT( (a ^= mask ) == (prev ^ mask ));
                prev = a;
                CPPUNIT_ASSERT( ( prev & mask)  == mask);
            }
            CPPUNIT_ASSERT( a == (integral_type) -1 );
        }

        template <class Atomic, typename Integral>
        void do_test_atomic_type( Atomic& a, atomics::memory_order order )
        {
            typedef Integral    integral_type;

            const atomics::memory_order oLoad = convert_to_load_order( order );
            const atomics::memory_order oStore = convert_to_store_order( order );

            CPPUNIT_ASSERT( a.is_lock_free() );
            a.store((integral_type) 0, oStore );
            CPPUNIT_ASSERT( a == 0 );
            CPPUNIT_ASSERT( a.load( oLoad ) == 0 );

            for ( size_t nByte = 0; nByte < sizeof(Integral); ++nByte ) {
                integral_type n = integral_type(42) << (nByte * 8);
                CPPUNIT_ASSERT( a.exchange( n, order ) == 0 );
                CPPUNIT_ASSERT( a.load( oLoad ) == n );
                CPPUNIT_ASSERT( a.exchange( (integral_type) 0, order ) == n );
                CPPUNIT_ASSERT( a.load( oLoad ) == 0 );
            }

            integral_type prev = a.load( oLoad );
            for ( size_t nByte = 0; nByte < sizeof(Integral); ++nByte ) {
                integral_type n = integral_type(42) << (nByte * 8);
                integral_type expected = prev;

                CPPUNIT_ASSERT( a.compare_exchange_weak( expected, n, order, atomics::memory_order_relaxed));
                CPPUNIT_ASSERT( expected  == prev );
                CPPUNIT_ASSERT( !a.compare_exchange_weak( expected, n, order, atomics::memory_order_relaxed));
                CPPUNIT_ASSERT( expected  == n );

                prev = n;
                CPPUNIT_ASSERT( a.load( oLoad ) == n );
            }

            a.store( (integral_type) 0, oStore );

            prev = a.load( oLoad );
            for ( size_t nByte = 0; nByte < sizeof(Integral); ++nByte ) {
                integral_type n = integral_type(42) << (nByte * 8);
                integral_type expected = prev;

                CPPUNIT_ASSERT( a.compare_exchange_strong( expected, n, order, atomics::memory_order_relaxed));
                CPPUNIT_ASSERT( expected  == prev );
                CPPUNIT_ASSERT( !a.compare_exchange_strong( expected, n, order, atomics::memory_order_relaxed));
                CPPUNIT_ASSERT( expected  == n );

                prev = n;
                CPPUNIT_ASSERT( a.load( oLoad ) == n );
            }

            CPPUNIT_ASSERT( a.exchange( (integral_type) 0, order ) == prev );
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

                CPPUNIT_ASSERT( a.fetch_add( n, order) == prev);
            }

            // fetch_sub
            for ( size_t nByte = sizeof(integral_type); nByte > 0; --nByte )
            {
                integral_type prev = a.load( oLoad );
                integral_type n = integral_type(42) << ((nByte - 1) * 8);

                CPPUNIT_ASSERT( a.fetch_sub( n, order ) == prev);
            }
            CPPUNIT_ASSERT( a.load( oLoad ) == 0 );

            // fetch_or / fetc_xor / fetch_and
            for ( size_t nBit = 0; nBit < sizeof(integral_type) * 8; ++nBit )
            {
                integral_type prev = a.load( oLoad )  ;;
                integral_type mask = 1 << nBit;

                CPPUNIT_ASSERT( a.fetch_or( mask, order ) == prev );
                prev = a.load( oLoad );
                CPPUNIT_ASSERT( ( prev & mask)  == mask);

                CPPUNIT_ASSERT( a.fetch_and( (integral_type) ~mask, order ) == prev );
                prev = a.load( oLoad );
                CPPUNIT_ASSERT( ( prev & mask)  == 0);

                CPPUNIT_ASSERT( a.fetch_xor( mask, order ) == prev );
                prev = a.load( oLoad );
                CPPUNIT_ASSERT( ( prev & mask)  == mask);
            }
            CPPUNIT_ASSERT( a.load( oLoad ) == (integral_type) -1 );
        }



        template <typename Atomic, typename Integral>
        void test_atomic_integral_(Atomic& a)
        {
            do_test_atomic_integral<Atomic, Integral >(a);

            do_test_atomic_integral<Atomic, Integral >( a, atomics::memory_order_relaxed );
#if !(CDS_COMPILER == CDS_COMPILER_GCC && CDS_COMPILER_VERSION < 40900)
            do_test_atomic_integral<Atomic, Integral >( a, atomics::memory_order_consume );
#endif
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
            CPPUNIT_ASSERT( a.is_lock_free() );
            a.store( false );
            CPPUNIT_ASSERT( a == false );
            CPPUNIT_ASSERT( a.load() == false );

            CPPUNIT_ASSERT( a.exchange( true ) == false );
            CPPUNIT_ASSERT( a.load() == true );
            CPPUNIT_ASSERT( a.exchange( false ) == true );
            CPPUNIT_ASSERT( a.load() == false );

            bool expected = false;
            CPPUNIT_ASSERT( a.compare_exchange_weak( expected, true));
            CPPUNIT_ASSERT( expected  == false );
            CPPUNIT_ASSERT( !a.compare_exchange_weak( expected, false));
            CPPUNIT_ASSERT( expected  == true );
            CPPUNIT_ASSERT( a.load() == true );

            a.store( false );

            expected = false;
            CPPUNIT_ASSERT( a.compare_exchange_strong( expected, true));
            CPPUNIT_ASSERT( expected  == false );
            CPPUNIT_ASSERT( !a.compare_exchange_strong( expected, false));
            CPPUNIT_ASSERT( expected  == true );

            CPPUNIT_ASSERT( a.load() == true );

            CPPUNIT_ASSERT( a.exchange( false ) == true );
        }

        template <class AtomicBool>
        void do_test_atomic_bool( AtomicBool& a, atomics::memory_order order )
        {
            const atomics::memory_order oLoad = convert_to_load_order( order );
            const atomics::memory_order oStore = convert_to_store_order( order );
            const atomics::memory_order oExchange = convert_to_exchange_order( order );

            CPPUNIT_ASSERT( a.is_lock_free() );
            a.store( false, oStore );
            CPPUNIT_ASSERT( a == false );
            CPPUNIT_ASSERT( a.load( oLoad ) == false );

            CPPUNIT_ASSERT( a.exchange( true, oExchange ) == false );
            CPPUNIT_ASSERT( a.load( oLoad ) == true );
            CPPUNIT_ASSERT( a.exchange( false, oExchange ) == true );
            CPPUNIT_ASSERT( a.load( oLoad ) == false );

            bool expected = false;
            CPPUNIT_ASSERT( a.compare_exchange_weak( expected, true, order, atomics::memory_order_relaxed));
            CPPUNIT_ASSERT( expected  == false );
            CPPUNIT_ASSERT( !a.compare_exchange_weak( expected, false, order, atomics::memory_order_relaxed));
            CPPUNIT_ASSERT( expected  == true );
            CPPUNIT_ASSERT( a.load( oLoad ) == true );

            //a = bool(false);
            a.store( false, oStore );

            expected = false;
            CPPUNIT_ASSERT( a.compare_exchange_strong( expected, true, order, atomics::memory_order_relaxed));
            CPPUNIT_ASSERT( expected  == false );
            CPPUNIT_ASSERT( !a.compare_exchange_strong( expected, false, order, atomics::memory_order_relaxed));
            CPPUNIT_ASSERT( expected  == true );

            CPPUNIT_ASSERT( a.load( oLoad ) == true );

            CPPUNIT_ASSERT( a.exchange( false, oExchange ) == true );
        }


        template <typename Atomic>
        void do_test_atomic_pointer_void_( Atomic& a, char * arr, char aSize, atomics::memory_order order )
        {
            atomics::memory_order oLoad = convert_to_load_order(order);
            atomics::memory_order oStore = convert_to_store_order(order);
            void *  p;

            a.store( (void *) arr, oStore );
            CPPUNIT_ASSERT( *reinterpret_cast<char *>(a.load( oLoad )) == 1 );

            p = arr;
            CPPUNIT_ASSERT( a.compare_exchange_weak( p, (void *)(arr + 5), order, atomics::memory_order_relaxed ));
            CPPUNIT_ASSERT( p == arr + 0 );
            CPPUNIT_ASSERT( *reinterpret_cast<char *>(p) == 1 );
            CPPUNIT_ASSERT( !a.compare_exchange_weak( p, (void *)(arr + 3), order, atomics::memory_order_relaxed ));
            CPPUNIT_ASSERT( p == arr + 5 );
            CPPUNIT_ASSERT( *reinterpret_cast<char *>(p) == 6 );

            CPPUNIT_ASSERT( a.compare_exchange_strong( p, (void *)(arr + 3), order, atomics::memory_order_relaxed ));
            CPPUNIT_ASSERT( p == arr + 5 );
            CPPUNIT_ASSERT( *reinterpret_cast<char *>(p) == 6 );
            CPPUNIT_ASSERT( !a.compare_exchange_strong( p, (void *)(arr + 5), order, atomics::memory_order_relaxed ));
            CPPUNIT_ASSERT( p == arr + 3 );
            CPPUNIT_ASSERT( *reinterpret_cast<char *>(p) == 4 );

            CPPUNIT_ASSERT( reinterpret_cast<char *>(a.exchange( (void *) arr, order )) == arr + 3 );
            CPPUNIT_ASSERT( reinterpret_cast<char *>(a.load( oLoad )) == arr );
            CPPUNIT_ASSERT( *reinterpret_cast<char *>(a.load( oLoad )) == 1 );

            for ( char i = 1; i < aSize; ++i ) {
                CPPUNIT_ASSERT( *reinterpret_cast<char *>(a.load( oLoad )) == i );
                CPPUNIT_ASSERT( a.fetch_add( 1, order ));
                CPPUNIT_ASSERT( *reinterpret_cast<char *>(a.load( oLoad )) == i + 1 );
            }

            for ( char i = aSize; i > 1; --i ) {
                CPPUNIT_ASSERT( *reinterpret_cast<char *>(a.load( oLoad )) == i  );
                CPPUNIT_ASSERT( a.fetch_sub( 1, order ));
                CPPUNIT_ASSERT( *reinterpret_cast<char *>(a.load( oLoad )) == i - 1 );
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

#if CDS_BUILD_BITS == 32 && !( CDS_COMPILER == CDS_COMPILER_GCC && CDS_COMPILER_VERSION == 40700 )
            /* GCC 4.7.0 has an linktime error in 32bit x86 mode:

            ../tests/test-hdr/misc/cxx11_atomic_class.o: In function `std::__atomic_base<void*>::is_lock_free() const':
            /usr/local/lib/gcc/x86_64-unknown-linux-gnu/4.7.0/../../../../include/c++/4.7.0/bits/atomic_base.h:719: undefined reference to `__atomic_is_lock_free'

            ../tests/test-hdr/misc/cxx11_atomic_class.o: In function `std::__atomic_base<void*>::is_lock_free() const volatile':
            /usr/local/lib/gcc/x86_64-unknown-linux-gnu/4.7.0/../../../../include/c++/4.7.0/bits/atomic_base.h:723: undefined reference to `__atomic_is_lock_free'

            */
            CPPUNIT_ASSERT( a.is_lock_free() );
#endif

            a.store( (void *) arr );
            CPPUNIT_ASSERT( *reinterpret_cast<char *>(a.load()) == 1 );

            p = arr;
            CPPUNIT_ASSERT( a.compare_exchange_weak( p, (void *)(arr + 5) ));
            CPPUNIT_ASSERT( p == arr + 0 );
            CPPUNIT_ASSERT( !a.compare_exchange_weak( p, (void *)(arr + 3) ));
            CPPUNIT_ASSERT( p == arr + 5 );

            CPPUNIT_ASSERT( a.compare_exchange_strong( p, (void *)(arr + 3) ));
            CPPUNIT_ASSERT( p == arr + 5 );
            CPPUNIT_ASSERT( !a.compare_exchange_strong( p, (void *)(arr + 5) ));
            CPPUNIT_ASSERT( p == arr + 3 );

            CPPUNIT_ASSERT( reinterpret_cast<char *>( a.exchange( (void *) arr )) == arr + 3 );
            CPPUNIT_ASSERT( reinterpret_cast<char *>( a.load()) == arr );
            CPPUNIT_ASSERT( *reinterpret_cast<char *>( a.load()) == 1 );

            for ( char i = 1; i < aSize; ++i ) {
                CPPUNIT_ASSERT( *reinterpret_cast<char *>(a.load()) == i );
                CPPUNIT_ASSERT( a.fetch_add( 1 ));
                CPPUNIT_ASSERT( *reinterpret_cast<char *>(a.load()) == i + 1 );
            }

            for ( char i = aSize; i > 1; --i ) {
                CPPUNIT_ASSERT( *reinterpret_cast<char *>(a.load()) == i  );
                CPPUNIT_ASSERT( a.fetch_sub( 1 ));
                CPPUNIT_ASSERT( *reinterpret_cast<char *>(a.load()) == i - 1 );
            }

            do_test_atomic_pointer_void_( a, arr, aSize, atomics::memory_order_relaxed );
            do_test_atomic_pointer_void_( a, arr, aSize, atomics::memory_order_consume );
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
            CPPUNIT_ASSERT( *a.load( oLoad ) == 1 );

            p = arr;
            CPPUNIT_ASSERT( a.compare_exchange_weak( p, arr + 5, order, atomics::memory_order_relaxed ));
            CPPUNIT_ASSERT( p == arr + 0 );
            CPPUNIT_ASSERT( *p == 1 );
            CPPUNIT_ASSERT( !a.compare_exchange_weak( p, arr + 3, order, atomics::memory_order_relaxed ));
            CPPUNIT_ASSERT( p == arr + 5 );
            CPPUNIT_ASSERT( *p == 6 );

            CPPUNIT_ASSERT( a.compare_exchange_strong( p, arr + 3, order, atomics::memory_order_relaxed ));
            CPPUNIT_ASSERT( p == arr + 5 );
            CPPUNIT_ASSERT( *p == 6 );
            CPPUNIT_ASSERT( !a.compare_exchange_strong( p, arr + 5, order, atomics::memory_order_relaxed ));
            CPPUNIT_ASSERT( p == arr + 3 );
            CPPUNIT_ASSERT( *p == 4 );

            CPPUNIT_ASSERT( a.exchange( arr, order ) == arr + 3 );
            CPPUNIT_ASSERT( a.load( oLoad ) == arr );
            CPPUNIT_ASSERT( *a.load( oLoad ) == 1 );

            for ( integral_type i = 1; i < aSize; ++i ) {
                integral_type * p = a.load();
                CPPUNIT_ASSERT( *p == i );
                CPPUNIT_ASSERT( a.fetch_add( 1, order ) == p  );
                CPPUNIT_ASSERT( *a.load( oLoad ) == i + 1 );
            }

            for ( integral_type i = aSize; i > 1; --i ) {
                integral_type * p = a.load();
                CPPUNIT_ASSERT( *p == i  );
                CPPUNIT_ASSERT( a.fetch_sub( 1, order ) == p );
                CPPUNIT_ASSERT( *a.load( oLoad ) == i - 1 );
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
            CPPUNIT_ASSERT( *a.load() == 1 );

            p = arr;
            CPPUNIT_ASSERT( a.compare_exchange_weak( p, arr + 5 ));
            CPPUNIT_ASSERT( p == arr + 0 );
            CPPUNIT_ASSERT( *p == 1 );
            CPPUNIT_ASSERT( !a.compare_exchange_weak( p, arr + 3 ));
            CPPUNIT_ASSERT( p == arr + 5 );
            CPPUNIT_ASSERT( *p == 6 );

            CPPUNIT_ASSERT( a.compare_exchange_strong( p, arr + 3 ));
            CPPUNIT_ASSERT( p == arr + 5 );
            CPPUNIT_ASSERT( *p == 6 );
            CPPUNIT_ASSERT( !a.compare_exchange_strong( p, arr + 5 ));
            CPPUNIT_ASSERT( p == arr + 3 );
            CPPUNIT_ASSERT( *p == 4 );

            CPPUNIT_ASSERT( a.exchange( arr ) == arr + 3 );
            CPPUNIT_ASSERT( a.load() == arr );
            CPPUNIT_ASSERT( *a.load() == 1 );

            for ( integral_type i = 1; i < aSize; ++i ) {
                integral_type * p = a.load();
                CPPUNIT_ASSERT( *p == i );
                integral_type * pa = a.fetch_add( 1 );
                CPPUNIT_ASSERT_EX( pa == p, "pa=" << ((uintptr_t) pa) << " p=" << ((uintptr_t) p) );
                CPPUNIT_ASSERT( *a.load() == i + 1 );
            }

            for ( integral_type i = aSize; i > 1; --i ) {
                integral_type * p = a.load();
                CPPUNIT_ASSERT( *p == i  );
                CPPUNIT_ASSERT( a.fetch_sub( 1 ) == p );
                CPPUNIT_ASSERT( *a.load() == i - 1 );
            }

            test_atomic_pointer_for_( a, arr, aSize, atomics::memory_order_relaxed );
            test_atomic_pointer_for_( a, arr, aSize, atomics::memory_order_consume );
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
                do_test_atomic_bool( a[i], atomics::memory_order_consume );
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

        void test_atomic_char()                 { test_atomic_integral<char>(); }
        void test_atomic_signed_char()          { test_atomic_integral<signed char>(); }
        void test_atomic_unsigned_char()        { test_atomic_integral<unsigned char>(); }
        void test_atomic_short_int()            { test_atomic_integral<short int>(); }
        void test_atomic_unsigned_short_int()   { test_atomic_integral<unsigned short int>(); }
        void test_atomic_int()                  { test_atomic_integral<int>(); }
        void test_atomic_unsigned_int()         { test_atomic_integral<unsigned int>(); }
        void test_atomic_long()                 { test_atomic_integral<long>(); }
        void test_atomic_unsigned_long()        { test_atomic_integral<unsigned long>(); }
        void test_atomic_long_long()            { test_atomic_integral<long long>(); }
        void test_atomic_unsigned_long_long()   { test_atomic_integral<unsigned long long>(); }

        void test_atomic_char_volatile()                 { test_atomic_integral_volatile<char>(); }
        void test_atomic_signed_char_volatile()          { test_atomic_integral_volatile<signed char>(); }
        void test_atomic_unsigned_char_volatile()        { test_atomic_integral_volatile<unsigned char>(); }
        void test_atomic_short_int_volatile()            { test_atomic_integral_volatile<short int>(); }
        void test_atomic_unsigned_short_int_volatile()   { test_atomic_integral_volatile<unsigned short int>(); }
        void test_atomic_int_volatile()                  { test_atomic_integral_volatile<int>(); }
        void test_atomic_unsigned_int_volatile()         { test_atomic_integral_volatile<unsigned int>(); }
        void test_atomic_long_volatile()                 { test_atomic_integral_volatile<long>(); }
        void test_atomic_unsigned_long_volatile()        { test_atomic_integral_volatile<unsigned long>(); }
        void test_atomic_long_long_volatile()            { test_atomic_integral_volatile<long long>(); }
        void test_atomic_unsigned_long_long_volatile()   { test_atomic_integral_volatile<unsigned long long>(); }

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

    public:
        CPPUNIT_TEST_SUITE(cxx11_atomic_class)
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

        CPPUNIT_TEST_SUITE_END()
    };
}   // namespace misc

CPPUNIT_TEST_SUITE_REGISTRATION(misc::cxx11_atomic_class);
