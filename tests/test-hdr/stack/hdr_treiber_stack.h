//$$CDS-header$$

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
            CPPUNIT_ASSERT( stack.pop(v) );
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

#ifdef CDS_EMPLACE_SUPPORT
            CPPUNIT_ASSERT( stack.emplace(100));
            CPPUNIT_ASSERT( stack.pop(v));
            CPPUNIT_ASSERT( v == 100 );
#endif

            stack.clear();
            CPPUNIT_ASSERT( stack.empty() );

            Stack::gc::scan();
        }

        void Treiber_HP();
        void Treiber_HRC();
        void Treiber_PTB();
        void Treiber_HP_yield();
        void Treiber_HRC_yield();
        void Treiber_PTB_yield();
        void Treiber_HP_pause_alloc();
        void Treiber_HRC_pause_alloc();
        void Treiber_PTB_pause_alloc();

        void Treiber_HP_relaxed();
        void Treiber_HRC_relaxed();
        void Treiber_PTB_relaxed();
        void Treiber_HP_yield_relaxed();
        void Treiber_HRC_yield_relaxed();
        void Treiber_PTB_yield_relaxed();
        void Treiber_HP_pause_alloc_relaxed();
        void Treiber_HRC_pause_alloc_relaxed();
        void Treiber_PTB_pause_alloc_relaxed();

        void Elimination_HP();
        void Elimination_HP_dyn();
        void Elimination_HP_stat();
        void Elimination_HRC();
        void Elimination_HRC_dyn();
        void Elimination_HRC_stat();
        void Elimination_PTB();
        void Elimination_PTB_dyn();
        void Elimination_PTB_stat();
        void Elimination_HP_yield();
        void Elimination_HRC_yield();
        void Elimination_PTB_yield();
        void Elimination_HP_pause_alloc();
        void Elimination_HRC_pause_alloc();
        void Elimination_PTB_pause_alloc();

        void Elimination_HP_relaxed();
        void Elimination_HRC_relaxed();
        void Elimination_PTB_relaxed();
        void Elimination_HP_yield_relaxed();
        void Elimination_HRC_yield_relaxed();
        void Elimination_PTB_yield_relaxed();
        void Elimination_HP_pause_alloc_relaxed();
        void Elimination_HRC_pause_alloc_relaxed();
        void Elimination_PTB_pause_alloc_relaxed();

        CPPUNIT_TEST_SUITE(TestStack);
            CPPUNIT_TEST(Treiber_HP)
            CPPUNIT_TEST(Treiber_HP_relaxed)
            CPPUNIT_TEST(Treiber_HRC)
            CPPUNIT_TEST(Treiber_HRC_relaxed)
            CPPUNIT_TEST(Treiber_PTB)
            CPPUNIT_TEST(Treiber_PTB_relaxed)
            CPPUNIT_TEST(Treiber_HP_yield)
            CPPUNIT_TEST(Treiber_HP_yield_relaxed)
            CPPUNIT_TEST(Treiber_HRC_yield)
            CPPUNIT_TEST(Treiber_HRC_yield_relaxed)
            CPPUNIT_TEST(Treiber_PTB_yield)
            CPPUNIT_TEST(Treiber_PTB_yield_relaxed)
            CPPUNIT_TEST(Treiber_HP_pause_alloc)
            CPPUNIT_TEST(Treiber_HP_pause_alloc_relaxed)
            CPPUNIT_TEST(Treiber_HRC_pause_alloc)
            CPPUNIT_TEST(Treiber_HRC_pause_alloc_relaxed)
            CPPUNIT_TEST(Treiber_PTB_pause_alloc)
            CPPUNIT_TEST(Treiber_PTB_pause_alloc_relaxed)

            CPPUNIT_TEST(Elimination_HP)
            CPPUNIT_TEST(Elimination_HP_dyn)
            CPPUNIT_TEST(Elimination_HP_stat)
            CPPUNIT_TEST(Elimination_HP_relaxed)
            CPPUNIT_TEST(Elimination_HRC)
            CPPUNIT_TEST(Elimination_HRC_dyn)
            CPPUNIT_TEST(Elimination_HRC_stat)
            CPPUNIT_TEST(Elimination_HRC_relaxed)
            CPPUNIT_TEST(Elimination_PTB)
            CPPUNIT_TEST(Elimination_PTB_dyn)
            CPPUNIT_TEST(Elimination_PTB_stat)
            CPPUNIT_TEST(Elimination_PTB_relaxed)
            CPPUNIT_TEST(Elimination_HP_yield)
            CPPUNIT_TEST(Elimination_HP_yield_relaxed)
            CPPUNIT_TEST(Elimination_HRC_yield)
            CPPUNIT_TEST(Elimination_HRC_yield_relaxed)
            CPPUNIT_TEST(Elimination_PTB_yield)
            CPPUNIT_TEST(Elimination_PTB_yield_relaxed)
            CPPUNIT_TEST(Elimination_HP_pause_alloc)
            CPPUNIT_TEST(Elimination_HP_pause_alloc_relaxed)
            CPPUNIT_TEST(Elimination_HRC_pause_alloc)
            CPPUNIT_TEST(Elimination_HRC_pause_alloc_relaxed)
            CPPUNIT_TEST(Elimination_PTB_pause_alloc)
            CPPUNIT_TEST(Elimination_PTB_pause_alloc_relaxed)
        CPPUNIT_TEST_SUITE_END();
    };
}   // namespace stack

