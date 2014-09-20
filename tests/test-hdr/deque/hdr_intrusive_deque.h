//$$CDS-header$$

#include "cppunit/cppunit_proxy.h"
#include "cppunit/test_beans.h"

namespace deque {
    namespace ci = cds::intrusive;
    namespace co = cds::opt;

    class IntrusiveDequeHeaderTest: public CppUnitMini::TestCase
    {
    public:
        template <typename NodeType>
        struct base_hook_item: public NodeType
        {
            int nVal;
            int nDisposeCount;

            base_hook_item()
                : nDisposeCount(0)
            {}
        };

        template <typename NodeType>
        struct member_hook_item
        {
            int nVal;
            int nDisposeCount;
            NodeType hMember;

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

        template <class Deque>
        void test()
        {
            test_beans::check_item_counter<typename Deque::item_counter> check_ic;

            typedef typename Deque::value_type value_type;
            value_type v1, v2, v3;

            Deque q;
            CPPUNIT_ASSERT( q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 0 ));

            v1.nVal = 1;
            v2.nVal = 2;
            v3.nVal = 3;

            // push_left / pop_right
            CPPUNIT_ASSERT( q.push_front(v1));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));
            CPPUNIT_ASSERT( q.push_front(v2));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 2 ));
            CPPUNIT_ASSERT( q.push_front(v3));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 3 ));

            CPPUNIT_ASSERT( v1.nDisposeCount == 0 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 0 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 0 );

            value_type * pv;
            pv = q.pop_back();
            Deque::gc::scan();
            CPPUNIT_ASSERT( pv != nullptr );
            CPPUNIT_ASSERT( pv->nVal == 1 );
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 2 ));
            CPPUNIT_ASSERT( v1.nDisposeCount == 1 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 0 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 0 );

            pv = q.pop_back();
            Deque::gc::scan();
            CPPUNIT_ASSERT( pv != nullptr );
            CPPUNIT_ASSERT( pv->nVal == 2 );
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));
            CPPUNIT_ASSERT( v1.nDisposeCount == 1 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 1 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 0 );

            pv = q.pop_back();
            Deque::gc::scan();
            CPPUNIT_ASSERT( pv != nullptr );
            CPPUNIT_ASSERT( pv->nVal == 3 );
            CPPUNIT_ASSERT( q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 0 ));
            CPPUNIT_ASSERT( v1.nDisposeCount == 1 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 1 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 1 );

            pv = q.pop_back();
            Deque::gc::scan();
            CPPUNIT_ASSERT( pv == nullptr );
            CPPUNIT_ASSERT( q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 0 ));
            CPPUNIT_ASSERT( v1.nDisposeCount == 1 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 1 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 1 );


            // push_right / pop_left
            CPPUNIT_ASSERT( q.push_back(v1));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));
            CPPUNIT_ASSERT( q.push_back(v2));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 2 ));
            CPPUNIT_ASSERT( q.push_back(v3));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 3 ));

            CPPUNIT_ASSERT( v1.nDisposeCount == 1 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 1 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 1 );

            pv = q.pop_front();
            Deque::gc::scan();
            CPPUNIT_ASSERT( pv != nullptr );
            CPPUNIT_ASSERT( pv->nVal == 1 );
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 2 ));
            CPPUNIT_ASSERT( v1.nDisposeCount == 2 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 1 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 1 );

            pv = q.pop_front();
            Deque::gc::scan();
            CPPUNIT_ASSERT( pv != nullptr );
            CPPUNIT_ASSERT( pv->nVal == 2 );
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));
            CPPUNIT_ASSERT( v1.nDisposeCount == 2 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 2 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 1 );

            pv = q.pop_front();
            Deque::gc::scan();
            CPPUNIT_ASSERT( pv != nullptr );
            CPPUNIT_ASSERT( pv->nVal == 3 );
            CPPUNIT_ASSERT( q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 0 ));
            CPPUNIT_ASSERT( v1.nDisposeCount == 2 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 2 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 2 );

            pv = q.pop_front();
            Deque::gc::scan();
            CPPUNIT_ASSERT( pv == nullptr );
            CPPUNIT_ASSERT( q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 0 ));
            CPPUNIT_ASSERT( v1.nDisposeCount == 2 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 2 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 2 );


            // push_right / pop_right
            CPPUNIT_ASSERT( q.push_back(v1));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));
            CPPUNIT_ASSERT( q.push_back(v2));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 2 ));
            CPPUNIT_ASSERT( q.push_back(v3));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 3 ));

            CPPUNIT_ASSERT( v1.nDisposeCount == 2 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 2 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 2 );

            pv = q.pop_back();
            Deque::gc::scan();
            CPPUNIT_ASSERT( pv != nullptr );
            CPPUNIT_ASSERT( pv->nVal == 3 );
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 2 ));
            CPPUNIT_ASSERT( v1.nDisposeCount == 2 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 2 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 3 );

            pv = q.pop_back();
            Deque::gc::scan();
            CPPUNIT_ASSERT( pv != nullptr );
            CPPUNIT_ASSERT( pv->nVal == 2 );
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));
            CPPUNIT_ASSERT( v1.nDisposeCount == 2 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 3 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 3 );

            pv = q.pop_back();
            Deque::gc::scan();
            CPPUNIT_ASSERT( pv != nullptr );
            CPPUNIT_ASSERT( pv->nVal == 1 );
            CPPUNIT_ASSERT( q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 0 ));
            CPPUNIT_ASSERT( v1.nDisposeCount == 3 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 3 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 3 );

            pv = q.pop_back();
            Deque::gc::scan();
            CPPUNIT_ASSERT( pv == nullptr );
            CPPUNIT_ASSERT( q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 0 ));
            CPPUNIT_ASSERT( v1.nDisposeCount == 3 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 3 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 3 );


            // push_left / pop_left
            CPPUNIT_ASSERT( q.push_front(v1));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));
            CPPUNIT_ASSERT( q.push_front(v2));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 2 ));
            CPPUNIT_ASSERT( q.push_front(v3));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 3 ));

            CPPUNIT_ASSERT( v1.nDisposeCount == 3 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 3 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 3 );

            pv = q.pop_front();
            Deque::gc::scan();
            CPPUNIT_ASSERT( pv != nullptr );
            CPPUNIT_ASSERT( pv->nVal == 3 );
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 2 ));
            CPPUNIT_ASSERT( v1.nDisposeCount == 3 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 3 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 4 );

            pv = q.pop_front();
            Deque::gc::scan();
            CPPUNIT_ASSERT( pv != nullptr );
            CPPUNIT_ASSERT( pv->nVal == 2 );
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 1 ));
            CPPUNIT_ASSERT( v1.nDisposeCount == 3 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 4 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 4 );

            pv = q.pop_front();
            Deque::gc::scan();
            CPPUNIT_ASSERT( pv != nullptr );
            CPPUNIT_ASSERT( pv->nVal == 1 );
            CPPUNIT_ASSERT( q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 0 ));
            CPPUNIT_ASSERT( v1.nDisposeCount == 4 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 4 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 4 );

            pv = q.pop_front();
            Deque::gc::scan();
            CPPUNIT_ASSERT( pv == nullptr );
            CPPUNIT_ASSERT( q.empty() );
            CPPUNIT_ASSERT( check_ic( q.size(), 0 ));
            CPPUNIT_ASSERT( v1.nDisposeCount == 4 );
            CPPUNIT_ASSERT( v2.nDisposeCount == 4 );
            CPPUNIT_ASSERT( v3.nDisposeCount == 4 );
        }

        void test_MichaelDeque_HP_default();
        void test_MichaelDeque_HP_default_ic();
        void test_MichaelDeque_HP_default_stat();
        void test_MichaelDeque_HP_base();
        void test_MichaelDeque_HP_member();
        void test_MichaelDeque_HP_base_ic();
        void test_MichaelDeque_HP_member_ic();
        void test_MichaelDeque_HP_base_stat();
        void test_MichaelDeque_HP_member_stat();
        void test_MichaelDeque_HP_base_align();
        void test_MichaelDeque_HP_member_align();
        void test_MichaelDeque_HP_base_noalign();
        void test_MichaelDeque_HP_member_noalign();
        void test_MichaelDeque_PTB_base();
        void test_MichaelDeque_PTB_member();
        void test_MichaelDeque_PTB_base_ic();
        void test_MichaelDeque_PTB_member_ic();
        void test_MichaelDeque_PTB_base_stat();
        void test_MichaelDeque_PTB_member_stat();
        void test_MichaelDeque_PTB_base_align();
        void test_MichaelDeque_PTB_member_align();
        void test_MichaelDeque_PTB_base_noalign();
        void test_MichaelDeque_PTB_member_noalign();

        CPPUNIT_TEST_SUITE(IntrusiveDequeHeaderTest)
            CPPUNIT_TEST( test_MichaelDeque_HP_default);
            CPPUNIT_TEST( test_MichaelDeque_HP_default_ic);
            CPPUNIT_TEST( test_MichaelDeque_HP_default_stat);
            CPPUNIT_TEST( test_MichaelDeque_HP_base);
            CPPUNIT_TEST( test_MichaelDeque_HP_member);
            CPPUNIT_TEST( test_MichaelDeque_HP_base_ic);
            CPPUNIT_TEST( test_MichaelDeque_HP_member_ic);
            CPPUNIT_TEST( test_MichaelDeque_HP_base_stat);
            CPPUNIT_TEST( test_MichaelDeque_HP_member_stat);
            CPPUNIT_TEST( test_MichaelDeque_HP_base_align);
            CPPUNIT_TEST( test_MichaelDeque_HP_member_align);
            CPPUNIT_TEST( test_MichaelDeque_HP_base_noalign);
            CPPUNIT_TEST( test_MichaelDeque_HP_member_noalign);
            CPPUNIT_TEST( test_MichaelDeque_PTB_base);
            CPPUNIT_TEST( test_MichaelDeque_PTB_member);
            CPPUNIT_TEST( test_MichaelDeque_PTB_base_ic);
            CPPUNIT_TEST( test_MichaelDeque_PTB_member_ic);
            CPPUNIT_TEST( test_MichaelDeque_PTB_base_stat);
            CPPUNIT_TEST( test_MichaelDeque_PTB_member_stat);
            CPPUNIT_TEST( test_MichaelDeque_PTB_base_align);
            CPPUNIT_TEST( test_MichaelDeque_PTB_member_align);
            CPPUNIT_TEST( test_MichaelDeque_PTB_base_noalign);
            CPPUNIT_TEST( test_MichaelDeque_PTB_member_noalign);

        CPPUNIT_TEST_SUITE_END()
    };

}   // namespace deque
