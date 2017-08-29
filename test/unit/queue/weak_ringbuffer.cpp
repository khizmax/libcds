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

#include "test_bounded_queue.h"

#include <cds/container/weak_ringbuffer.h>
#include <cds_test/fixture.h>

namespace {
    namespace cc = cds::container;

    class WeakRingBuffer: public cds_test::bounded_queue
    {
    public:
        template <typename Queue>
        void test_array( Queue& q )
        {
            typedef typename Queue::value_type value_type;

            const size_t nSize = q.capacity();
            static const size_t nArrSize = 16;
            const size_t nArrCount = nSize / nArrSize;

            {
                value_type el[nArrSize];

                for ( unsigned pass = 0; pass < 3; ++pass ) {
                    // batch push
                    for ( size_t i = 0; i < nSize; i += nArrSize ) {
                        for ( size_t k = 0; k < nArrSize; ++k )
                            el[k] = static_cast<value_type>( i + k );

                        if ( i + nArrSize <= nSize ) {
                            ASSERT_TRUE( q.push( el, nArrSize ));
                        }
                        else {
                            ASSERT_FALSE( q.push( el, nArrSize ));
                        }
                    }

                    ASSERT_TRUE( !q.empty());
                    if ( nSize % nArrSize != 0 ) {
                        ASSERT_FALSE( q.full());
                        ASSERT_CONTAINER_SIZE( q, nArrCount * nArrSize );
                        for ( size_t i = nArrCount * nArrSize; i < nSize; ++i ) {
                            ASSERT_TRUE( q.enqueue( static_cast<value_type>( i )));
                        }
                    }
                    ASSERT_TRUE( q.full());
                    ASSERT_CONTAINER_SIZE( q, nSize );

                    // batch pop
                    value_type expected = 0;
                    while ( q.pop( el, nArrSize )) {
                        for ( size_t i = 0; i < nArrSize; ++i ) {
                            ASSERT_EQ( el[i], expected );
                            ++expected;
                        }
                    }

                    if ( nSize % nArrSize == 0 ) {
                        ASSERT_TRUE( q.empty());
                    }
                    else {
                        ASSERT_FALSE( q.empty());
                        ASSERT_CONTAINER_SIZE( q, nSize % nArrSize );
                        q.clear();
                    }
                    ASSERT_TRUE( q.empty());
                    ASSERT_FALSE( q.full());
                    ASSERT_CONTAINER_SIZE( q, 0u );
                }
            }

            {
                // batch push with functor
                size_t el[nArrSize];

                auto func_push = []( value_type& dest, size_t src ) { dest = static_cast<value_type>( src * 10 ); };

                for ( unsigned pass = 0; pass < 3; ++pass ) {
                    for ( size_t i = 0; i < nSize; i += nArrSize ) {
                        for ( size_t k = 0; k < nArrSize; ++k )
                            el[k] = i + k;

                        if ( i + nArrSize <= nSize ) {
                            ASSERT_TRUE( q.push( el, nArrSize, func_push ));
                        }
                        else {
                            ASSERT_FALSE( q.push( el, nArrSize, func_push ));
                        }
                    }

                    ASSERT_TRUE( !q.empty());
                    if ( nSize % nArrSize != 0 ) {
                        ASSERT_FALSE( q.full());
                        ASSERT_CONTAINER_SIZE( q, nArrCount * nArrSize );
                        for ( size_t i = nArrCount * nArrSize; i < nSize; ++i ) {
                            ASSERT_TRUE( q.push( &i, 1, func_push ));
                        }
                    }
                    ASSERT_TRUE( q.full());
                    ASSERT_CONTAINER_SIZE( q, nSize );

                    // batch pop with functor
                    auto func_pop = []( size_t& dest, value_type src ) { dest = static_cast<size_t>( src / 10 ); };
                    size_t expected = 0;
                    while ( q.pop( el, nArrSize, func_pop )) {
                        for ( size_t i = 0; i < nArrSize; ++i ) {
                            ASSERT_EQ( el[i], expected );
                            ++expected;
                        }
                    }

                    if ( nSize % nArrSize == 0 ) {
                        ASSERT_TRUE( q.empty());
                    }
                    else {
                        ASSERT_FALSE( q.empty());
                        ASSERT_CONTAINER_SIZE( q, nSize % nArrSize );
                        size_t v;
                        while ( q.pop( &v, 1, func_pop )) {
                            ASSERT_EQ( v, expected );
                            ++expected;
                        }
                    }
                    ASSERT_TRUE( q.empty());
                    ASSERT_FALSE( q.full());
                    ASSERT_CONTAINER_SIZE( q, 0u );
                }

                // front/pop_front
                for ( unsigned pass = 0; pass < 3; ++pass ) {
                    for ( size_t i = 0; i < nSize; i += nArrSize ) {
                        for ( size_t k = 0; k < nArrSize; ++k )
                            el[k] = i + k;

                        if ( i + nArrSize <= nSize ) {
                            ASSERT_TRUE( q.push( el, nArrSize, func_push ));
                        }
                        else {
                            ASSERT_FALSE( q.push( el, nArrSize, func_push ));
                        }
                    }

                    ASSERT_TRUE( !q.empty());
                    if ( nSize % nArrSize != 0 ) {
                        ASSERT_FALSE( q.full());
                        ASSERT_CONTAINER_SIZE( q, nArrCount * nArrSize );
                        for ( size_t i = nArrCount * nArrSize; i < nSize; ++i ) {
                            ASSERT_TRUE( q.push( &i, 1, func_push ));
                        }
                    }
                    ASSERT_TRUE( q.full());
                    ASSERT_CONTAINER_SIZE( q, nSize );

                    value_type cur = 0;
                    while ( !q.empty()) {
                        value_type* front = q.front();
                        ASSERT_TRUE( front != nullptr );
                        ASSERT_EQ( cur, *front );
                        ASSERT_TRUE( q.pop_front());
                        cur += 10;
                    }

                    ASSERT_TRUE( q.empty());
                    ASSERT_TRUE( q.front() == nullptr );
                    ASSERT_FALSE( q.pop_front());
                }
            }
        }

        template <typename Queue>
        void test_varsize_buffer( Queue& q )
        {
            size_t const capacity = q.capacity();

            ASSERT_TRUE( q.empty());
            ASSERT_EQ( q.size(), 0u );
            ASSERT_TRUE( q.front().first == nullptr );
            ASSERT_FALSE( q.pop_front());

            size_t total_push = 0;
            uint8_t chfill = 0;
            while ( total_push < capacity * 4 ) {
                unsigned buf_size = cds_test::fixture::rand( static_cast<unsigned>( capacity / 4 )) + 1;
                total_push += buf_size;

                void* buf = q.back( buf_size );
                ASSERT_TRUE( buf != nullptr );

                memset( buf, chfill, buf_size );
                q.push_back();

                ASSERT_GE( q.size(), buf_size );

                auto pair = q.front();
                ASSERT_TRUE( pair.first != nullptr );
                ASSERT_EQ( pair.second, buf_size );
                for ( size_t i = 0; i < pair.second; ++i )
                    ASSERT_EQ( *reinterpret_cast<uint8_t*>( pair.first ), chfill );

                ASSERT_TRUE( q.pop_front());
                ASSERT_FALSE( q.pop_front());
            }

            ASSERT_TRUE( q.empty());
            ASSERT_EQ( q.size(), 0u );
            ASSERT_TRUE( q.front().first == nullptr );
            ASSERT_FALSE( q.pop_front());
        }
    };

    TEST_F( WeakRingBuffer, defaulted )
    {
        typedef cds::container::WeakRingBuffer< int > test_queue;

        test_queue q( 128 );
        test( q );
        test_array( q );
    }

    TEST_F( WeakRingBuffer, stat )
    {
        struct traits: public cds::container::weak_ringbuffer::traits
        {
            typedef cds::opt::v::uninitialized_static_buffer<int, 128> buffer;
        };
        typedef cds::container::WeakRingBuffer< int, traits > test_queue;

        test_queue q;
        test( q );
        test_array( q );
    }

    TEST_F( WeakRingBuffer, dynamic )
    {
        struct traits: public cds::container::weak_ringbuffer::traits
        {
            typedef cds::opt::v::uninitialized_dynamic_buffer<int> buffer;
        };
        typedef cds::container::WeakRingBuffer< int, traits > test_queue;

        test_queue q( 128 );
        test( q );
        test_array( q );
    }

    TEST_F( WeakRingBuffer, dynamic_mod )
    {
        struct traits: public cds::container::weak_ringbuffer::traits
        {
            typedef cds::opt::v::uninitialized_dynamic_buffer<int, CDS_DEFAULT_ALLOCATOR, false> buffer;
        };
        typedef cds::container::WeakRingBuffer< int, traits > test_queue;

        test_queue q( 100 );
        test( q );
        test_array( q );
    }

    TEST_F( WeakRingBuffer, dynamic_padding )
    {
        struct traits: public cds::container::weak_ringbuffer::traits
        {
            typedef cds::opt::v::uninitialized_dynamic_buffer<int> buffer;
            enum { padding = 32 };
        };
        typedef cds::container::WeakRingBuffer< int, traits > test_queue;

        test_queue q( 128 );
        test( q );
        test_array( q );
    }

    TEST_F( WeakRingBuffer, var_sized )
    {
        typedef cds::container::WeakRingBuffer< void > test_queue;

        test_queue q( 1024 * 64 );
        test_varsize_buffer( q );
    }

    TEST_F( WeakRingBuffer, var_sized_static )
    {
        struct traits: public cds::container::weak_ringbuffer::traits
        {
            typedef cds::opt::v::uninitialized_static_buffer<int, 1024 * 64> buffer;
        };
        typedef cds::container::WeakRingBuffer< void, traits > test_queue;

        test_queue q;
        test_varsize_buffer( q );
    }

} // namespace
