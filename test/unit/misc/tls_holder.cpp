// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cds_test/ext_gtest.h>
#include <cds/details/tls_holder.h>
#include <thread>

namespace {

    class tls_test: public ::testing::Test
    {};

    TEST_F( tls_test, int_val )
    {
        struct tag_a;
        cds::details::tls_holder<int, tag_a> a;
        a.set( 1 );

        struct tag_b;
        cds::details::tls_holder<int, tag_b> b;
        b.set( 2 );

        cds::details::tls_holder<int, tag_a> c;
        c.set( 3 );

        EXPECT_EQ( a.get(), 3 );
        EXPECT_EQ( b.get(), 2 );
        EXPECT_EQ( c.get(), 3 );

        EXPECT_EQ( a.get(), c.get() );
        EXPECT_NE( a.get(), b.get() );
        EXPECT_NE( c.get(), b.get() );
    }

    struct tls_tuple {
        struct tag_a;
        cds::details::tls_holder<int, tag_a> a;

        struct tag_b;
        cds::details::tls_holder<int, tag_b> b;

        cds::details::tls_holder<int, tag_a> c;
    };

    void thread_func( tls_tuple* tls, int inc )
    {
        tls->a.set( 1 + inc );
        EXPECT_EQ( tls->a.get(), 1 + inc );

        tls->b.set( 2 + inc );
        EXPECT_EQ( tls->b.get(), 2 + inc );

        tls->c.set( 3 + inc );
        EXPECT_EQ( tls->c.get(), 3 + inc );

        EXPECT_EQ( tls->a.get(), 3 + inc );
        EXPECT_EQ( tls->b.get(), 2 + inc );
        EXPECT_EQ( tls->c.get(), 3 + inc );

        EXPECT_EQ( tls->a.get(), tls->c.get() );
        EXPECT_NE( tls->a.get(), tls->b.get() );
        EXPECT_NE( tls->c.get(), tls->b.get() );
    }

    TEST_F( tls_test, thread_test )
    {
        tls_tuple tls;

        tls.a.set( 1 );
        EXPECT_EQ( tls.a.get(), 1 );

        tls.b.set( 2 );
        EXPECT_EQ( tls.b.get(), 2 );

        tls.c.set( 3 );
        EXPECT_EQ( tls.c.get(), 3 );

        EXPECT_EQ( tls.a.get(), 3 );
        EXPECT_EQ( tls.b.get(), 2 );
        EXPECT_EQ( tls.c.get(), 3 );

        EXPECT_EQ( tls.a.get(), tls.c.get() );
        EXPECT_NE( tls.a.get(), tls.b.get() );
        EXPECT_NE( tls.c.get(), tls.b.get() );

        std::thread t1( thread_func, &tls, 1000 );

        EXPECT_EQ( tls.a.get(), 3 );
        EXPECT_EQ( tls.b.get(), 2 );
        EXPECT_EQ( tls.c.get(), 3 );

        std::thread t2( thread_func, &tls, 2000 );

        EXPECT_EQ( tls.a.get(), 3 );
        EXPECT_EQ( tls.b.get(), 2 );
        EXPECT_EQ( tls.c.get(), 3 );

        t1.join();
        t2.join();

        EXPECT_EQ( tls.a.get(), 3 );
        EXPECT_EQ( tls.b.get(), 2 );
        EXPECT_EQ( tls.c.get(), 3 );
    }

} // namespace
