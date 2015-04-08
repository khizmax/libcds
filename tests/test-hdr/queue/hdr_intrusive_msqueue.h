//$$CDS-header$$

#ifndef CDSTEST_HDR_INTRSUSIVE_MSQUEUE_H
#define CDSTEST_HDR_INTRSUSIVE_MSQUEUE_H

#include "cppunit/cppunit_proxy.h"
#include <cds/intrusive/details/base.h>

namespace queue {
    namespace ci = cds::intrusive;
    namespace co = cds::opt;

    class IntrusiveQueueHeaderTest: public CppUnitMini::TestCase
    {
    public:

        template <typename Base>
        struct base_hook_item : public Base
        {
            int nVal;
            int nDisposeCount;

            base_hook_item()
                : nDisposeCount( 0 )
            {}
        };

        template <typename Member>
        struct member_hook_item
        {
            int nVal;
            int nDisposeCount;
            Member hMember;

            member_hook_item()
                : nDisposeCount( 0 )
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


        // Test for MSQueue and derivatives
        template <class Queue>
        void test()
        {
            typedef typename Queue::value_type value_type;
            value_type v1, v2, v3;

            {
                Queue q;
                CPPUNIT_ASSERT( q.empty() );

                v1.nVal = 1;
                v2.nVal = 2;
                v3.nVal = 3;
                CPPUNIT_ASSERT( q.push(v1));
                CPPUNIT_ASSERT( !q.empty() );
                CPPUNIT_ASSERT( q.push(v2));
                CPPUNIT_ASSERT( !q.empty() );
                CPPUNIT_ASSERT( q.push(v3));
                CPPUNIT_ASSERT( !q.empty() );

                CPPUNIT_CHECK( v1.nDisposeCount == 0 );
                CPPUNIT_CHECK( v2.nDisposeCount == 0 );
                CPPUNIT_CHECK( v3.nDisposeCount == 0 );

                value_type * pv;
                pv = q.pop();
                Queue::gc::scan();
                CPPUNIT_ASSERT( pv != nullptr );
                CPPUNIT_ASSERT( pv->nVal == 1 );
                CPPUNIT_ASSERT( !q.empty() );
                CPPUNIT_CHECK( v1.nDisposeCount == 0 );
                CPPUNIT_CHECK( v2.nDisposeCount == 0 );
                CPPUNIT_CHECK( v3.nDisposeCount == 0 );

                pv = q.pop();
                Queue::gc::scan();
                CPPUNIT_ASSERT( pv != nullptr );
                CPPUNIT_ASSERT( pv->nVal == 2 );
                CPPUNIT_ASSERT( !q.empty() );
                CPPUNIT_CHECK( v1.nDisposeCount == 1 );
                CPPUNIT_CHECK( v2.nDisposeCount == 0 );
                CPPUNIT_CHECK( v3.nDisposeCount == 0 );

                pv = q.pop();
                Queue::gc::scan();
                CPPUNIT_ASSERT( pv != nullptr );
                CPPUNIT_ASSERT( pv->nVal == 3 );
                CPPUNIT_ASSERT( q.empty() );
                CPPUNIT_CHECK( v1.nDisposeCount == 1 );
                CPPUNIT_CHECK( v2.nDisposeCount == 1 );
                CPPUNIT_CHECK( v3.nDisposeCount == 0 );

                pv = q.pop();
                CPPUNIT_ASSERT( pv == nullptr );
                CPPUNIT_CHECK( v1.nDisposeCount == 1 );
                CPPUNIT_CHECK( v2.nDisposeCount == 1 );
                CPPUNIT_CHECK( v3.nDisposeCount == 0 );
            }

            Queue::gc::scan();
            CPPUNIT_CHECK( v1.nDisposeCount == 1 );
            CPPUNIT_CHECK( v2.nDisposeCount == 1 );
            CPPUNIT_CHECK( v3.nDisposeCount == 1 );
        }

        // Test for BasketQueue and derivatives
        template <class Queue>
        void test_basket()
        {
            typedef typename Queue::value_type value_type;
            value_type v1, v2, v3, v4;

            {
                Queue q;
                CPPUNIT_ASSERT( q.empty() );

                v1.nVal = 1;
                v2.nVal = 2;
                v3.nVal = 3;
                v4.nVal = 4;
                CPPUNIT_ASSERT( q.push(v1));
                CPPUNIT_ASSERT( !q.empty() );
                CPPUNIT_ASSERT( q.push(v2));
                CPPUNIT_ASSERT( !q.empty() );
                CPPUNIT_ASSERT( q.push(v3));
                CPPUNIT_ASSERT( !q.empty() );
                CPPUNIT_ASSERT( q.push(v4));
                CPPUNIT_ASSERT( !q.empty() );

                CPPUNIT_CHECK( v1.nDisposeCount == 0 );
                CPPUNIT_CHECK( v2.nDisposeCount == 0 );
                CPPUNIT_CHECK( v3.nDisposeCount == 0 );
                CPPUNIT_CHECK( v4.nDisposeCount == 0 );

                value_type * pv;
                pv = q.pop();
                Queue::gc::scan();
                CPPUNIT_ASSERT( pv != nullptr );
                CPPUNIT_ASSERT( pv->nVal == 1 );
                CPPUNIT_ASSERT( !q.empty() );
                CPPUNIT_CHECK( v1.nDisposeCount == 0 );
                CPPUNIT_CHECK( v2.nDisposeCount == 0 );
                CPPUNIT_CHECK( v3.nDisposeCount == 0 );
                CPPUNIT_CHECK( v4.nDisposeCount == 0 );

                pv = q.pop();
                Queue::gc::scan();
                CPPUNIT_ASSERT( pv != nullptr );
                CPPUNIT_ASSERT( pv->nVal == 2 );
                CPPUNIT_ASSERT( !q.empty() );
                CPPUNIT_CHECK( v1.nDisposeCount == 0 );
                CPPUNIT_CHECK( v2.nDisposeCount == 0 );
                CPPUNIT_CHECK( v3.nDisposeCount == 0 );
                CPPUNIT_CHECK( v4.nDisposeCount == 0 );

                pv = q.pop();
                Queue::gc::scan();
                CPPUNIT_ASSERT( pv != nullptr );
                CPPUNIT_ASSERT( pv->nVal == 3 );
                CPPUNIT_ASSERT( !q.empty() );
                CPPUNIT_CHECK( v1.nDisposeCount == 0 );
                CPPUNIT_CHECK( v2.nDisposeCount == 0 );
                CPPUNIT_CHECK( v3.nDisposeCount == 0 );
                CPPUNIT_CHECK( v4.nDisposeCount == 0 );

                pv = q.pop();
                Queue::gc::scan();
                CPPUNIT_ASSERT( pv != nullptr );
                CPPUNIT_ASSERT( pv->nVal == 4 );
                CPPUNIT_ASSERT( q.empty() );
                CPPUNIT_CHECK( v1.nDisposeCount == 1 );
                CPPUNIT_CHECK( v2.nDisposeCount == 1 );
                CPPUNIT_CHECK( v3.nDisposeCount == 1 );
                CPPUNIT_CHECK( v4.nDisposeCount == 0 );

                pv = q.pop();
                Queue::gc::scan();
                CPPUNIT_CHECK( pv == nullptr );
                CPPUNIT_CHECK( v1.nDisposeCount == 1 );
                CPPUNIT_CHECK( v2.nDisposeCount == 1 );
                CPPUNIT_CHECK( v3.nDisposeCount == 1 );
                CPPUNIT_CHECK( v4.nDisposeCount == 0 );
            }

            Queue::gc::scan();
            CPPUNIT_CHECK( v1.nDisposeCount == 1 );
            CPPUNIT_CHECK( v2.nDisposeCount == 1 );
            CPPUNIT_CHECK( v3.nDisposeCount == 1 );
            CPPUNIT_CHECK( v4.nDisposeCount == 1 );
        }

        // Test for a queue in what dequeued item should be manually disposed
        template <class Queue>
        void test2()
        {
            typedef typename Queue::value_type value_type;
            value_type v1, v2, v3;
            Queue q;
            CPPUNIT_ASSERT( q.empty() );

            v1.nVal = 1;
            v2.nVal = 2;
            v3.nVal = 3;
            CPPUNIT_ASSERT( q.push(v1));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( q.push(v2));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( q.push(v3));
            CPPUNIT_ASSERT( !q.empty() );

            CPPUNIT_CHECK( v1.nDisposeCount == 0 );
            CPPUNIT_CHECK( v2.nDisposeCount == 0 );
            CPPUNIT_CHECK( v3.nDisposeCount == 0 );

            value_type * pv;
            pv = q.pop();
            CPPUNIT_ASSERT( pv != nullptr );
            CPPUNIT_ASSERT( pv->nVal == 1 );
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_CHECK( v1.nDisposeCount == 0 );
            CPPUNIT_CHECK( v2.nDisposeCount == 0 );
            CPPUNIT_CHECK( v3.nDisposeCount == 0 );

            pv = q.pop();
            CPPUNIT_ASSERT( pv != nullptr );
            CPPUNIT_ASSERT( pv->nVal == 2 );
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_CHECK( v1.nDisposeCount == 0 );
            CPPUNIT_CHECK( v2.nDisposeCount == 0 );
            CPPUNIT_CHECK( v3.nDisposeCount == 0 );

            pv = q.pop();
            CPPUNIT_ASSERT( pv != nullptr );
            CPPUNIT_ASSERT( pv->nVal == 3 );
            CPPUNIT_ASSERT( q.empty() );
            CPPUNIT_CHECK( v1.nDisposeCount == 0 );
            CPPUNIT_CHECK( v2.nDisposeCount == 0 );
            CPPUNIT_CHECK( v3.nDisposeCount == 0 );

            pv = q.pop();
            CPPUNIT_ASSERT( pv == nullptr );
            CPPUNIT_CHECK( v1.nDisposeCount == 0 );
            CPPUNIT_CHECK( v2.nDisposeCount == 0 );
            CPPUNIT_CHECK( v3.nDisposeCount == 0 );

            CPPUNIT_ASSERT( q.push(v1));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( q.push(v2));
            CPPUNIT_ASSERT( !q.empty() );
            CPPUNIT_ASSERT( q.push(v3));
            CPPUNIT_ASSERT( !q.empty() );

            CPPUNIT_CHECK( v1.nDisposeCount == 0 );
            CPPUNIT_CHECK( v2.nDisposeCount == 0 );
            CPPUNIT_CHECK( v3.nDisposeCount == 0 );
            q.clear();
            CPPUNIT_CHECK( q.empty() );
            CPPUNIT_CHECK( v1.nDisposeCount == 1 );
            CPPUNIT_CHECK( v2.nDisposeCount == 1 );
            CPPUNIT_CHECK( v3.nDisposeCount == 1 );
        }


        void test_MSQueue_HP_default();
        void test_MSQueue_HP_default_ic();
        void test_MSQueue_HP_default_stat();
        void test_MSQueue_HP_base();
        void test_MSQueue_HP_member();
        void test_MSQueue_HP_base_ic();
        void test_MSQueue_HP_member_ic();
        void test_MSQueue_HP_base_stat();
        void test_MSQueue_HP_member_stat();
        void test_MSQueue_HP_base_align();
        void test_MSQueue_HP_member_align();
        void test_MSQueue_HP_base_noalign();
        void test_MSQueue_HP_member_noalign();
        void test_MSQueue_HP_base_cachealign();
        void test_MSQueue_HP_member_cachealign();
        void test_MSQueue_DHP_base();
        void test_MSQueue_DHP_member();
        void test_MSQueue_DHP_base_ic();
        void test_MSQueue_DHP_member_ic();
        void test_MSQueue_DHP_base_stat();
        void test_MSQueue_DHP_member_stat();
        void test_MSQueue_DHP_base_align();
        void test_MSQueue_DHP_member_align();
        void test_MSQueue_DHP_base_noalign();
        void test_MSQueue_DHP_member_noalign();
        void test_MSQueue_DHP_base_cachealign();
        void test_MSQueue_DHP_member_cachealign();

        void test_MoirQueue_HP_default();
        void test_MoirQueue_HP_default_ic();
        void test_MoirQueue_HP_default_stat();
        void test_MoirQueue_HP_base();
        void test_MoirQueue_HP_member();
        void test_MoirQueue_HP_base_ic();
        void test_MoirQueue_HP_member_ic();
        void test_MoirQueue_HP_base_stat();
        void test_MoirQueue_HP_member_stat();
        void test_MoirQueue_HP_base_align();
        void test_MoirQueue_HP_member_align();
        void test_MoirQueue_HP_base_noalign();
        void test_MoirQueue_HP_member_noalign();
        void test_MoirQueue_HP_base_cachealign();
        void test_MoirQueue_HP_member_cachealign();
        void test_MoirQueue_DHP_base();
        void test_MoirQueue_DHP_member();
        void test_MoirQueue_DHP_base_ic();
        void test_MoirQueue_DHP_member_ic();
        void test_MoirQueue_DHP_base_stat();
        void test_MoirQueue_DHP_member_stat();
        void test_MoirQueue_DHP_base_align();
        void test_MoirQueue_DHP_member_align();
        void test_MoirQueue_DHP_base_noalign();
        void test_MoirQueue_DHP_member_noalign();
        void test_MoirQueue_DHP_base_cachealign();
        void test_MoirQueue_DHP_member_cachealign();

        void test_OptimisticQueue_HP_default();
        void test_OptimisticQueue_HP_default_ic();
        void test_OptimisticQueue_HP_default_stat();
        void test_OptimisticQueue_HP_base();
        void test_OptimisticQueue_HP_member();
        void test_OptimisticQueue_HP_base_ic();
        void test_OptimisticQueue_HP_member_ic();
        void test_OptimisticQueue_HP_base_stat();
        void test_OptimisticQueue_HP_member_stat();
        void test_OptimisticQueue_HP_base_align();
        void test_OptimisticQueue_HP_member_align();
        void test_OptimisticQueue_HP_base_noalign();
        void test_OptimisticQueue_HP_member_noalign();
        void test_OptimisticQueue_HP_base_cachealign();
        void test_OptimisticQueue_HP_member_cachealign();
        void test_OptimisticQueue_DHP_base();
        void test_OptimisticQueue_DHP_member();
        void test_OptimisticQueue_DHP_base_ic();
        void test_OptimisticQueue_DHP_member_ic();
        void test_OptimisticQueue_DHP_base_stat();
        void test_OptimisticQueue_DHP_member_stat();
        void test_OptimisticQueue_DHP_base_align();
        void test_OptimisticQueue_DHP_member_align();
        void test_OptimisticQueue_DHP_base_noalign();
        void test_OptimisticQueue_DHP_member_noalign();
        void test_OptimisticQueue_DHP_base_cachealign();
        void test_OptimisticQueue_DHP_member_cachealign();

        void test_BasketQueue_HP_default();
        void test_BasketQueue_HP_default_ic();
        void test_BasketQueue_HP_default_stat();
        void test_BasketQueue_HP_base();
        void test_BasketQueue_HP_member();
        void test_BasketQueue_HP_base_ic();
        void test_BasketQueue_HP_member_ic();
        void test_BasketQueue_HP_base_stat();
        void test_BasketQueue_HP_member_stat();
        void test_BasketQueue_HP_base_align();
        void test_BasketQueue_HP_member_align();
        void test_BasketQueue_HP_base_noalign();
        void test_BasketQueue_HP_member_noalign();
        void test_BasketQueue_HP_base_cachealign();
        void test_BasketQueue_HP_member_cachealign();
        void test_BasketQueue_DHP_base();
        void test_BasketQueue_DHP_member();
        void test_BasketQueue_DHP_base_ic();
        void test_BasketQueue_DHP_member_ic();
        void test_BasketQueue_DHP_base_stat();
        void test_BasketQueue_DHP_member_stat();
        void test_BasketQueue_DHP_base_align();
        void test_BasketQueue_DHP_member_align();
        void test_BasketQueue_DHP_base_noalign();
        void test_BasketQueue_DHP_member_noalign();
        void test_BasketQueue_DHP_base_cachealign();
        void test_BasketQueue_DHP_member_cachealign();

        void test_TsigasCycleQueue_static();
        void test_TsigasCycleQueue_static_ic();
        void test_TsigasCycleQueue_dyn();
        void test_TsigasCycleQueue_dyn_ic();

        void test_VyukovMPMCCycleQueue_static();
        void test_VyukovMPMCCycleQueue_static_ic();
        void test_VyukovMPMCCycleQueue_dyn();
        void test_VyukovMPMCCycleQueue_dyn_ic();

        CPPUNIT_TEST_SUITE(IntrusiveQueueHeaderTest)
            CPPUNIT_TEST(test_MSQueue_HP_default)
            CPPUNIT_TEST(test_MSQueue_HP_default_ic)
            CPPUNIT_TEST(test_MSQueue_HP_default_stat)
            CPPUNIT_TEST(test_MSQueue_HP_base)
            CPPUNIT_TEST(test_MSQueue_HP_member)
            CPPUNIT_TEST(test_MSQueue_HP_base_ic)
            CPPUNIT_TEST(test_MSQueue_HP_member_ic)
            CPPUNIT_TEST(test_MSQueue_HP_base_stat)
            CPPUNIT_TEST(test_MSQueue_HP_member_stat)
            CPPUNIT_TEST(test_MSQueue_HP_base_align)
            CPPUNIT_TEST(test_MSQueue_HP_member_align)
            CPPUNIT_TEST(test_MSQueue_HP_base_noalign)
            CPPUNIT_TEST(test_MSQueue_HP_member_noalign)
            CPPUNIT_TEST(test_MSQueue_HP_base_cachealign)
            CPPUNIT_TEST(test_MSQueue_HP_member_cachealign)
            CPPUNIT_TEST(test_MSQueue_DHP_base)
            CPPUNIT_TEST(test_MSQueue_DHP_member)
            CPPUNIT_TEST(test_MSQueue_DHP_base_ic)
            CPPUNIT_TEST(test_MSQueue_DHP_member_ic)
            CPPUNIT_TEST(test_MSQueue_DHP_base_stat)
            CPPUNIT_TEST(test_MSQueue_DHP_member_stat)
            CPPUNIT_TEST(test_MSQueue_DHP_base_align)
            CPPUNIT_TEST(test_MSQueue_DHP_member_align)
            CPPUNIT_TEST(test_MSQueue_DHP_base_noalign)
            CPPUNIT_TEST(test_MSQueue_DHP_member_noalign)
            CPPUNIT_TEST(test_MSQueue_DHP_base_cachealign)
            CPPUNIT_TEST(test_MSQueue_DHP_member_cachealign)

            CPPUNIT_TEST(test_MoirQueue_HP_default)
            CPPUNIT_TEST(test_MoirQueue_HP_default_ic)
            CPPUNIT_TEST(test_MoirQueue_HP_default_stat)
            CPPUNIT_TEST(test_MoirQueue_HP_base)
            CPPUNIT_TEST(test_MoirQueue_HP_member)
            CPPUNIT_TEST(test_MoirQueue_HP_base_ic)
            CPPUNIT_TEST(test_MoirQueue_HP_member_ic)
            CPPUNIT_TEST(test_MoirQueue_HP_base_stat)
            CPPUNIT_TEST(test_MoirQueue_HP_member_stat)
            CPPUNIT_TEST(test_MoirQueue_HP_base_align)
            CPPUNIT_TEST(test_MoirQueue_HP_member_align)
            CPPUNIT_TEST(test_MoirQueue_HP_base_noalign)
            CPPUNIT_TEST(test_MoirQueue_HP_member_noalign)
            CPPUNIT_TEST(test_MoirQueue_HP_base_cachealign)
            CPPUNIT_TEST(test_MoirQueue_HP_member_cachealign)
            CPPUNIT_TEST(test_MoirQueue_DHP_base)
            CPPUNIT_TEST(test_MoirQueue_DHP_member)
            CPPUNIT_TEST(test_MoirQueue_DHP_base_ic)
            CPPUNIT_TEST(test_MoirQueue_DHP_member_ic)
            CPPUNIT_TEST(test_MoirQueue_DHP_base_stat)
            CPPUNIT_TEST(test_MoirQueue_DHP_member_stat)
            CPPUNIT_TEST(test_MoirQueue_DHP_base_align)
            CPPUNIT_TEST(test_MoirQueue_DHP_member_align)
            CPPUNIT_TEST(test_MoirQueue_DHP_base_noalign)
            CPPUNIT_TEST(test_MoirQueue_DHP_member_noalign)
            CPPUNIT_TEST(test_MoirQueue_DHP_base_cachealign)
            CPPUNIT_TEST(test_MoirQueue_DHP_member_cachealign)

            CPPUNIT_TEST(test_OptimisticQueue_HP_default)
            CPPUNIT_TEST(test_OptimisticQueue_HP_default_ic)
            CPPUNIT_TEST(test_OptimisticQueue_HP_default_stat)
            CPPUNIT_TEST(test_OptimisticQueue_HP_base)
            CPPUNIT_TEST(test_OptimisticQueue_HP_member)
            CPPUNIT_TEST(test_OptimisticQueue_HP_base_ic)
            CPPUNIT_TEST(test_OptimisticQueue_HP_member_ic)
            CPPUNIT_TEST(test_OptimisticQueue_HP_base_stat)
            CPPUNIT_TEST(test_OptimisticQueue_HP_member_stat)
            CPPUNIT_TEST(test_OptimisticQueue_HP_base_align)
            CPPUNIT_TEST(test_OptimisticQueue_HP_member_align)
            CPPUNIT_TEST(test_OptimisticQueue_HP_base_noalign)
            CPPUNIT_TEST(test_OptimisticQueue_HP_member_noalign)
            CPPUNIT_TEST(test_OptimisticQueue_HP_base_cachealign)
            CPPUNIT_TEST(test_OptimisticQueue_HP_member_cachealign)
            CPPUNIT_TEST(test_OptimisticQueue_DHP_base)
            CPPUNIT_TEST(test_OptimisticQueue_DHP_member)
            CPPUNIT_TEST(test_OptimisticQueue_DHP_base_ic)
            CPPUNIT_TEST(test_OptimisticQueue_DHP_member_ic)
            CPPUNIT_TEST(test_OptimisticQueue_DHP_base_stat)
            CPPUNIT_TEST(test_OptimisticQueue_DHP_member_stat)
            CPPUNIT_TEST(test_OptimisticQueue_DHP_base_align)
            CPPUNIT_TEST(test_OptimisticQueue_DHP_member_align)
            CPPUNIT_TEST(test_OptimisticQueue_DHP_base_noalign)
            CPPUNIT_TEST(test_OptimisticQueue_DHP_member_noalign)
            CPPUNIT_TEST(test_OptimisticQueue_DHP_base_cachealign)
            CPPUNIT_TEST(test_OptimisticQueue_DHP_member_cachealign)

            CPPUNIT_TEST(test_BasketQueue_HP_default)
            CPPUNIT_TEST(test_BasketQueue_HP_default_ic)
            CPPUNIT_TEST(test_BasketQueue_HP_default_stat)
            CPPUNIT_TEST(test_BasketQueue_HP_base)
            CPPUNIT_TEST(test_BasketQueue_HP_member)
            CPPUNIT_TEST(test_BasketQueue_HP_base_ic)
            CPPUNIT_TEST(test_BasketQueue_HP_member_ic)
            CPPUNIT_TEST(test_BasketQueue_HP_base_stat)
            CPPUNIT_TEST(test_BasketQueue_HP_member_stat)
            CPPUNIT_TEST(test_BasketQueue_HP_base_align)
            CPPUNIT_TEST(test_BasketQueue_HP_member_align)
            CPPUNIT_TEST(test_BasketQueue_HP_base_noalign)
            CPPUNIT_TEST(test_BasketQueue_HP_member_noalign)
            CPPUNIT_TEST(test_BasketQueue_HP_base_cachealign)
            CPPUNIT_TEST(test_BasketQueue_HP_member_cachealign)
            CPPUNIT_TEST(test_BasketQueue_DHP_base)
            CPPUNIT_TEST(test_BasketQueue_DHP_member)
            CPPUNIT_TEST(test_BasketQueue_DHP_base_ic)
            CPPUNIT_TEST(test_BasketQueue_DHP_member_ic)
            CPPUNIT_TEST(test_BasketQueue_DHP_base_stat)
            CPPUNIT_TEST(test_BasketQueue_DHP_member_stat)
            CPPUNIT_TEST(test_BasketQueue_DHP_base_align)
            CPPUNIT_TEST(test_BasketQueue_DHP_member_align)
            CPPUNIT_TEST(test_BasketQueue_DHP_base_noalign)
            CPPUNIT_TEST(test_BasketQueue_DHP_member_noalign)
            CPPUNIT_TEST(test_BasketQueue_DHP_base_cachealign)
            CPPUNIT_TEST(test_BasketQueue_DHP_member_cachealign)

            CPPUNIT_TEST(test_TsigasCycleQueue_static)
            CPPUNIT_TEST(test_TsigasCycleQueue_static_ic)
            CPPUNIT_TEST(test_TsigasCycleQueue_dyn)
            CPPUNIT_TEST(test_TsigasCycleQueue_dyn_ic)

            CPPUNIT_TEST(test_VyukovMPMCCycleQueue_static);
            CPPUNIT_TEST(test_VyukovMPMCCycleQueue_static_ic);
            CPPUNIT_TEST(test_VyukovMPMCCycleQueue_dyn);
            CPPUNIT_TEST(test_VyukovMPMCCycleQueue_dyn_ic);

        CPPUNIT_TEST_SUITE_END()
    };
}   // namespace queue

#endif // #ifndef CDSTEST_HDR_INTRSUSIVE_MSQUEUE_H
