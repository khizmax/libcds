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

#ifndef CDSTEST_HDR_TREIBER_STACK_H
#define CDSTEST_HDR_TREIBER_STACK_H

#include "cppunit/cppunit_proxy.h"

namespace stack {

    class TestStack: public CppUnitMini::TestCase
    {
        template <class Stack>
        void test()
        {
            Stack s;
            test_with( s );
        }

        template <class Stack>
        void test_elimination()
        {
            Stack s(2);
            test_with( s );
        }

        template <class Stack>
        void test_with( Stack& stack)
        {
            typedef typename Stack::value_type  value_type;
            value_type v;

            CPPUNIT_ASSERT( stack.empty() );

            CPPUNIT_ASSERT( stack.push(1));
            CPPUNIT_ASSERT( !stack.empty() );
            CPPUNIT_ASSERT( stack.push(2));
            CPPUNIT_ASSERT( !stack.empty() );
            CPPUNIT_ASSERT( stack.push(3));
            CPPUNIT_ASSERT( !stack.empty() );

            CPPUNIT_ASSERT( stack.pop(v) );
            CPPUNIT_ASSERT( v == 3 );
            CPPUNIT_ASSERT( !stack.empty() );
            CPPUNIT_ASSERT( stack.pop_with( [&v]( value_type& src ) { v = src; } ));
            CPPUNIT_ASSERT( v == 2 );
            CPPUNIT_ASSERT( !stack.empty() );
            CPPUNIT_ASSERT( stack.pop(v) );
            CPPUNIT_ASSERT( v == 1 );
            CPPUNIT_ASSERT( stack.empty() );
            v = 1000;
            CPPUNIT_ASSERT( !stack.pop(v) );
            CPPUNIT_ASSERT( v == 1000 );
            CPPUNIT_ASSERT( stack.empty() );

            CPPUNIT_ASSERT( stack.push(10));
            CPPUNIT_ASSERT( stack.push(20));
            CPPUNIT_ASSERT( stack.push(30));
            CPPUNIT_ASSERT( !stack.empty());

            CPPUNIT_ASSERT( stack.emplace(100));
            CPPUNIT_ASSERT( stack.pop(v));
            CPPUNIT_ASSERT( v == 100 );

            stack.clear();
            CPPUNIT_ASSERT( stack.empty() );

            Stack::gc::scan();
        }

        void Treiber_HP();
        void Treiber_DHP();
        void Treiber_HP_yield();
        void Treiber_DHP_yield();
        void Treiber_HP_pause_alloc();
        void Treiber_DHP_pause_alloc();

        void Treiber_HP_relaxed();
        void Treiber_DHP_relaxed();
        void Treiber_HP_yield_relaxed();
        void Treiber_DHP_yield_relaxed();
        void Treiber_HP_pause_alloc_relaxed();
        void Treiber_DHP_pause_alloc_relaxed();

        void Elimination_HP();
        void Elimination_HP_dyn();
        void Elimination_HP_stat();
        void Elimination_DHP();
        void Elimination_DHP_dyn();
        void Elimination_DHP_stat();
        void Elimination_HP_yield();
        void Elimination_DHP_yield();
        void Elimination_HP_pause_alloc();
        void Elimination_DHP_pause_alloc();

        void Elimination_HP_relaxed();
        void Elimination_DHP_relaxed();
        void Elimination_HP_yield_relaxed();
        void Elimination_DHP_yield_relaxed();
        void Elimination_HP_pause_alloc_relaxed();
        void Elimination_DHP_pause_alloc_relaxed();

        CPPUNIT_TEST_SUITE(TestStack);
            CPPUNIT_TEST(Treiber_HP)
            CPPUNIT_TEST(Treiber_HP_relaxed)
            CPPUNIT_TEST(Treiber_DHP)
            CPPUNIT_TEST(Treiber_DHP_relaxed)
            CPPUNIT_TEST(Treiber_HP_yield)
            CPPUNIT_TEST(Treiber_HP_yield_relaxed)
            CPPUNIT_TEST(Treiber_DHP_yield)
            CPPUNIT_TEST(Treiber_DHP_yield_relaxed)
            CPPUNIT_TEST(Treiber_HP_pause_alloc)
            CPPUNIT_TEST(Treiber_HP_pause_alloc_relaxed)
            CPPUNIT_TEST(Treiber_DHP_pause_alloc)
            CPPUNIT_TEST(Treiber_DHP_pause_alloc_relaxed)

            CPPUNIT_TEST(Elimination_HP)
            CPPUNIT_TEST(Elimination_HP_dyn)
            CPPUNIT_TEST(Elimination_HP_stat)
            CPPUNIT_TEST(Elimination_HP_relaxed)
            CPPUNIT_TEST(Elimination_DHP)
            CPPUNIT_TEST(Elimination_DHP_dyn)
            CPPUNIT_TEST(Elimination_DHP_stat)
            CPPUNIT_TEST(Elimination_DHP_relaxed)
            CPPUNIT_TEST(Elimination_HP_yield)
            CPPUNIT_TEST(Elimination_HP_yield_relaxed)
            CPPUNIT_TEST(Elimination_DHP_yield)
            CPPUNIT_TEST(Elimination_DHP_yield_relaxed)
            CPPUNIT_TEST(Elimination_HP_pause_alloc)
            CPPUNIT_TEST(Elimination_HP_pause_alloc_relaxed)
            CPPUNIT_TEST(Elimination_DHP_pause_alloc)
            CPPUNIT_TEST(Elimination_DHP_pause_alloc_relaxed)
        CPPUNIT_TEST_SUITE_END();
    };
}   // namespace stack

#endif // #ifndef CDSTEST_HDR_TREIBER_STACK_H
