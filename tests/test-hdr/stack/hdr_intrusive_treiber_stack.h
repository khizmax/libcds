//$$CDS-header$$

#include "cppunit/cppunit_proxy.h"
#include <cds/intrusive/details/single_link_struct.h>

namespace stack {

    namespace ci = cds::intrusive;

    class TestIntrusiveStack: public CppUnitMini::TestCase
    {
    public:
        template <typename GC>
        struct base_hook_item: public ci::single_link::node< GC >
        {
            int nVal;
            int nDisposeCount;

            base_hook_item()
                : nDisposeCount(0)
            {}
        };

        template <typename GC>
        struct member_hook_item
        {
            int nVal;
            int nDisposeCount;
            ci::single_link::node< GC > hMember;

            member_hook_item()
                : nDisposeCount(0)
            {}
        };

        struct faked_disposer
        {
            template <typename T>
            void operator ()( T * p )
            {
                ++p->nDisposeCount;
            }
        };

        template <class Stack>
        void test_elimination()
        {
            Stack stack( 2 );
            test_with( stack );
        }

        template <class Stack>
        void test()
        {
            Stack stack;
            test_with(stack);
        }


        template <class Stack>
        void test_with( Stack& stack )
        {
            typedef typename Stack::value_type  value_type;

            CPPUNIT_ASSERT( stack.empty() );

            value_type v1, v2, v3;
            v1.nVal = 1;
            v2.nVal = 2;
            v3.nVal = 3;
            CPPUNIT_ASSERT( stack.push(v1));
            CPPUNIT_ASSERT( !stack.empty() );
            CPPUNIT_ASSERT( stack.push(v2));
            CPPUNIT_ASSERT( !stack.empty() );
            CPPUNIT_ASSERT( stack.push(v3));
            CPPUNIT_ASSERT( !stack.empty() );

            value_type * pv;
            pv = stack.pop();
            CPPUNIT_ASSERT( pv != nullptr );
            CPPUNIT_ASSERT( pv->nVal == 3 );
            CPPUNIT_ASSERT( !stack.empty() );
            pv = stack.pop();
            CPPUNIT_ASSERT( pv != nullptr );
            CPPUNIT_ASSERT( pv->nVal == 2 );
            CPPUNIT_ASSERT( !stack.empty() );
            pv = stack.pop();
            CPPUNIT_ASSERT( pv != nullptr );
            CPPUNIT_ASSERT( pv->nVal == 1 );
            CPPUNIT_ASSERT( stack.empty() );
            pv = stack.pop();
            CPPUNIT_ASSERT( pv == nullptr );

            CPPUNIT_ASSERT( v1.nDisposeCount == 0 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 0 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 0 );

            stack.push(v1);
            stack.push(v2);
            stack.push(v3);

            stack.clear();
            CPPUNIT_ASSERT( stack.empty() );

            Stack::gc::scan();
            if ( !std::is_same<typename Stack::disposer, ci::opt::v::empty_disposer>::value ) {
                CPPUNIT_ASSERT( v1.nDisposeCount == 1 );
                CPPUNIT_ASSERT( v2.nDisposeCount == 1 );
                CPPUNIT_ASSERT( v3.nDisposeCount == 1 );
            }
        }

        void Treiber_HP_default();
        void Treiber_HP_base();
        void Treiber_HP_base_disposer();
        void Treiber_HP_member();
        void Treiber_HP_member_disposer();
        void Treiber_HRC_base();
        void Treiber_HRC_base_disposer();
        void Treiber_PTB_base();
        void Treiber_PTB_base_disposer();
        void Treiber_PTB_member();
        void Treiber_PTB_member_disposer();

        void Treiber_HP_default_relaxed();
        void Treiber_HP_base_relaxed();
        void Treiber_HP_base_disposer_relaxed();
        void Treiber_HP_member_relaxed();
        void Treiber_HP_member_disposer_relaxed();
        void Treiber_HRC_base_relaxed();
        void Treiber_HRC_base_disposer_relaxed();
        void Treiber_PTB_base_relaxed();
        void Treiber_PTB_base_disposer_relaxed();
        void Treiber_PTB_member_relaxed();
        void Treiber_PTB_member_disposer_relaxed();

        void Elimination_HP_default();
        void Elimination_HP_base();
        void Elimination_HP_base_dyn();
        void Elimination_HP_base_disposer();
        void Elimination_HP_member();
        void Elimination_HP_member_dyn();
        void Elimination_HP_member_disposer();
        void Elimination_HRC_base();
        void Elimination_HRC_base_dyn();
        void Elimination_HRC_base_disposer();
        void Elimination_PTB_base();
        void Elimination_PTB_base_dyn();
        void Elimination_PTB_base_disposer();
        void Elimination_PTB_member();
        void Elimination_PTB_member_dyn();
        void Elimination_PTB_member_disposer();

        void Elimination_HP_default_relaxed();
        void Elimination_HP_base_relaxed();
        void Elimination_HP_base_disposer_relaxed();
        void Elimination_HP_member_relaxed();
        void Elimination_HP_member_disposer_relaxed();
        void Elimination_HRC_base_relaxed();
        void Elimination_HRC_base_disposer_relaxed();
        void Elimination_PTB_base_relaxed();
        void Elimination_PTB_base_disposer_relaxed();
        void Elimination_PTB_member_relaxed();
        void Elimination_PTB_member_disposer_relaxed();

        CPPUNIT_TEST_SUITE(TestIntrusiveStack)
            CPPUNIT_TEST(Treiber_HP_default)
            CPPUNIT_TEST(Treiber_HP_default_relaxed)
            CPPUNIT_TEST(Treiber_HP_base)
            CPPUNIT_TEST(Treiber_HP_base_relaxed)
            CPPUNIT_TEST(Treiber_HP_base_disposer)
            CPPUNIT_TEST(Treiber_HP_base_disposer_relaxed)
            CPPUNIT_TEST(Treiber_HP_member)
            CPPUNIT_TEST(Treiber_HP_member_relaxed)
            CPPUNIT_TEST(Treiber_HP_member_disposer)
            CPPUNIT_TEST(Treiber_HP_member_disposer_relaxed)
            CPPUNIT_TEST(Treiber_HRC_base)
            CPPUNIT_TEST(Treiber_HRC_base_relaxed)
            CPPUNIT_TEST(Treiber_HRC_base_disposer)
            CPPUNIT_TEST(Treiber_HRC_base_disposer_relaxed)
            CPPUNIT_TEST(Treiber_PTB_base)
            CPPUNIT_TEST(Treiber_PTB_base_relaxed)
            CPPUNIT_TEST(Treiber_PTB_base_disposer)
            CPPUNIT_TEST(Treiber_PTB_base_disposer_relaxed)
            CPPUNIT_TEST(Treiber_PTB_member)
            CPPUNIT_TEST(Treiber_PTB_member_relaxed)
            CPPUNIT_TEST(Treiber_PTB_member_disposer)
            CPPUNIT_TEST(Treiber_PTB_member_disposer_relaxed)

            CPPUNIT_TEST(Elimination_HP_default)
            CPPUNIT_TEST(Elimination_HP_default_relaxed)
            CPPUNIT_TEST(Elimination_HP_base)
            CPPUNIT_TEST(Elimination_HP_base_dyn)
            CPPUNIT_TEST(Elimination_HP_base_relaxed)
            CPPUNIT_TEST(Elimination_HP_base_disposer)
            CPPUNIT_TEST(Elimination_HP_base_disposer_relaxed)
            CPPUNIT_TEST(Elimination_HP_member)
            CPPUNIT_TEST(Elimination_HP_member_dyn)
            CPPUNIT_TEST(Elimination_HP_member_relaxed)
            CPPUNIT_TEST(Elimination_HP_member_disposer)
            CPPUNIT_TEST(Elimination_HP_member_disposer_relaxed)
            CPPUNIT_TEST(Elimination_HRC_base)
            CPPUNIT_TEST(Elimination_HRC_base_dyn)
            CPPUNIT_TEST(Elimination_HRC_base_relaxed)
            CPPUNIT_TEST(Elimination_HRC_base_disposer)
            CPPUNIT_TEST(Elimination_HRC_base_disposer_relaxed)
            CPPUNIT_TEST(Elimination_PTB_base)
            CPPUNIT_TEST(Elimination_PTB_base_dyn)
            CPPUNIT_TEST(Elimination_PTB_base_relaxed)
            CPPUNIT_TEST(Elimination_PTB_base_disposer)
            CPPUNIT_TEST(Elimination_PTB_base_disposer_relaxed)
            CPPUNIT_TEST(Elimination_PTB_member)
            CPPUNIT_TEST(Elimination_PTB_member_dyn)
            CPPUNIT_TEST(Elimination_PTB_member_relaxed)
            CPPUNIT_TEST(Elimination_PTB_member_disposer)
            CPPUNIT_TEST(Elimination_PTB_member_disposer_relaxed)
        CPPUNIT_TEST_SUITE_END()
    };

} // namespace stack
