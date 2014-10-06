//$$CDS-header$$

#ifndef __UNIT_QUEUE_SIMPLE_H
#define __UNIT_QUEUE_SIMPLE_H

#include "cppunit/cppunit_proxy.h"
#include <cds/details/defs.h>

namespace queue {

    //
    // Test queue operation in single thread mode
    //
    class Queue_TestHeader: public CppUnitMini::TestCase
    {
    protected:
        template <class Queue>
        void testNoItemCounter()
        {
            Queue   q;
            test_with( q );
            test_emplace( q );
        }

        template <class Queue>
        void test_with( Queue& q )
        {
            int     it;
            int     nPrev;

            for ( size_t i = 0; i < 3; ++i ) {
                CPPUNIT_ASSERT( q.empty() );
#ifndef _DEBUG
                CPPUNIT_ASSERT( q.size() == 0 );
#endif
                CPPUNIT_ASSERT( q.enqueue( 1 ) );
                CPPUNIT_ASSERT( !q.empty() );
                CPPUNIT_ASSERT( q.push( 10 ) );
                CPPUNIT_ASSERT( !q.empty() );
#ifndef _DEBUG
                CPPUNIT_ASSERT( q.size() == 0 )     ;   // no queue's item counter!
#endif

                it = -1;
                CPPUNIT_ASSERT( q.pop( it ) );
                CPPUNIT_ASSERT( it == 1 );
                CPPUNIT_ASSERT( !q.empty() );
                CPPUNIT_ASSERT( q.dequeue( it ) );
                CPPUNIT_ASSERT( it == 10 );
#ifndef _DEBUG
                CPPUNIT_ASSERT( q.size() == 0 );
#endif
                CPPUNIT_ASSERT( q.empty() );
                it += 2009;
                nPrev = it;
                CPPUNIT_ASSERT( !q.dequeue( it ) );
                CPPUNIT_ASSERT( it == nPrev )       ;   // it must not be changed!
            }
        }

        template <class Queue>
        void test_emplace( Queue& q )
        {
            int     it;
            for ( size_t i = 0; i < 3; ++i ) {
                CPPUNIT_ASSERT( q.emplace( static_cast<int>( i * 42 )) );
                CPPUNIT_ASSERT( !q.empty() );
                it = -1;
                CPPUNIT_ASSERT( q.pop( it ));
                CPPUNIT_ASSERT( it == static_cast<int>( i * 42 ));
                CPPUNIT_ASSERT( q.empty() );
            }
        }

        template <class Queue>
        void testWithItemCounter()
        {
            Queue   q;
            test_ic_with( q );
            test_emplace_ic( q );
        }

        template <class Queue>
        void testFCQueue()
        {
            Queue   q;
            test_ic_with( q );
        }

        template <class Queue>
        void test_ic_with( Queue& q )
        {
            int     it;
            int     nPrev;

            for ( size_t i = 0; i < 3; ++i ) {
                CPPUNIT_ASSERT( q.empty() );
                CPPUNIT_ASSERT( q.size() == 0 );
                CPPUNIT_ASSERT( q.enqueue( 1 ) );
                CPPUNIT_ASSERT( q.size() == 1 );
                CPPUNIT_ASSERT( !q.empty() );
                CPPUNIT_ASSERT( q.push( 10 ) );
                CPPUNIT_ASSERT( !q.empty() );
                CPPUNIT_ASSERT( q.size() == 2 );

                it = -1;
                CPPUNIT_ASSERT( q.pop( it ) );
                CPPUNIT_ASSERT( it == 1 );
                CPPUNIT_ASSERT( !q.empty() );
                CPPUNIT_ASSERT( q.size() == 1 );
                CPPUNIT_ASSERT( q.dequeue( it ) );
                CPPUNIT_ASSERT( it == 10 );
                CPPUNIT_ASSERT( q.size() == 0 );
                CPPUNIT_ASSERT( q.empty() );
                CPPUNIT_ASSERT( q.size() == 0 );
                it += 2009;
                nPrev = it;
                CPPUNIT_ASSERT( !q.dequeue( it ) );
                CPPUNIT_ASSERT( it == nPrev )       ;   // it must not be changed!

                CPPUNIT_ASSERT( q.empty() );
                CPPUNIT_ASSERT( q.size() == 0 );
            }
        }

        template <class Queue>
        void test_emplace_ic( Queue& q )
        {
            int     it = 0;
            for ( size_t i = 0; i < 3; ++i ) {
                CPPUNIT_ASSERT( q.emplace( (int) i * 10 ) );
                CPPUNIT_ASSERT( !q.empty() );
                CPPUNIT_ASSERT( q.size() == 1 );
                CPPUNIT_ASSERT( q.pop( it ));
                CPPUNIT_ASSERT( it == (int) i * 10 );
                CPPUNIT_ASSERT( q.empty() );
                CPPUNIT_ASSERT( q.size() == 0 );
            }
        }

    public:
        void MSQueue_HP();
        void MSQueue_HP_relax();
        void MSQueue_HP_seqcst();
        void MSQueue_HP_relax_align();
        void MSQueue_HP_seqcst_align();
        void MSQueue_HP_Counted();
        void MSQueue_HP_Counted_relax();
        void MSQueue_HP_Counted_seqcst();
        void MSQueue_HP_Counted_relax_align();
        void MSQueue_HP_Counted_seqcst_align();

        void MSQueue_DHP();
        void MSQueue_DHP_relax();
        void MSQueue_DHP_seqcst();
        void MSQueue_DHP_relax_align();
        void MSQueue_DHP_seqcst_align();
        void MSQueue_DHP_Counted();
        void MSQueue_DHP_Counted_relax();
        void MSQueue_DHP_Counted_seqcst();
        void MSQueue_DHP_Counted_relax_align();
        void MSQueue_DHP_Counted_seqcst_align();

        void MoirQueue_HP();
        void MoirQueue_HP_relax();
        void MoirQueue_HP_seqcst();
        void MoirQueue_HP_relax_align();
        void MoirQueue_HP_seqcst_align();
        void MoirQueue_HP_Counted();
        void MoirQueue_HP_Counted_relax();
        void MoirQueue_HP_Counted_seqcst();
        void MoirQueue_HP_Counted_relax_align();
        void MoirQueue_HP_Counted_seqcst_align();

        void MoirQueue_DHP();
        void MoirQueue_DHP_relax();
        void MoirQueue_DHP_seqcst();
        void MoirQueue_DHP_relax_align();
        void MoirQueue_DHP_seqcst_align();
        void MoirQueue_DHP_Counted();
        void MoirQueue_DHP_Counted_relax();
        void MoirQueue_DHP_Counted_seqcst();
        void MoirQueue_DHP_Counted_relax_align();
        void MoirQueue_DHP_Counted_seqcst_align();

        void OptimisticQueue_HP();
        void OptimisticQueue_HP_relax();
        void OptimisticQueue_HP_seqcst();
        void OptimisticQueue_HP_relax_align();
        void OptimisticQueue_HP_seqcst_align();
        void OptimisticQueue_HP_Counted();
        void OptimisticQueue_HP_Counted_relax();
        void OptimisticQueue_HP_Counted_seqcst();
        void OptimisticQueue_HP_Counted_relax_align();
        void OptimisticQueue_HP_Counted_seqcst_align();

        void OptimisticQueue_PTB();
        void OptimisticQueue_PTB_relax();
        void OptimisticQueue_PTB_seqcst();
        void OptimisticQueue_PTB_relax_align();
        void OptimisticQueue_PTB_seqcst_align();
        void OptimisticQueue_PTB_Counted();
        void OptimisticQueue_PTB_Counted_relax();
        void OptimisticQueue_PTB_Counted_seqcst();
        void OptimisticQueue_PTB_Counted_relax_align();
        void OptimisticQueue_PTB_Counted_seqcst_align();

        void BasketQueue_HP();
        void BasketQueue_HP_relax();
        void BasketQueue_HP_seqcst();
        void BasketQueue_HP_relax_align();
        void BasketQueue_HP_seqcst_align();
        void BasketQueue_HP_Counted();
        void BasketQueue_HP_Counted_relax();
        void BasketQueue_HP_Counted_seqcst();
        void BasketQueue_HP_Counted_relax_align();
        void BasketQueue_HP_Counted_seqcst_align();

        void BasketQueue_HRC();
        void BasketQueue_HRC_relax();
        void BasketQueue_HRC_seqcst();
        void BasketQueue_HRC_relax_align();
        void BasketQueue_HRC_seqcst_align();
        void BasketQueue_HRC_Counted();
        void BasketQueue_HRC_Counted_relax();
        void BasketQueue_HRC_Counted_seqcst();
        void BasketQueue_HRC_Counted_relax_align();
        void BasketQueue_HRC_Counted_seqcst_align();

        void BasketQueue_PTB();
        void BasketQueue_PTB_relax();
        void BasketQueue_PTB_seqcst();
        void BasketQueue_PTB_relax_align();
        void BasketQueue_PTB_seqcst_align();
        void BasketQueue_PTB_Counted();
        void BasketQueue_PTB_Counted_relax();
        void BasketQueue_PTB_Counted_seqcst();
        void BasketQueue_PTB_Counted_relax_align();
        void BasketQueue_PTB_Counted_seqcst_align();

        void FCQueue_deque();
        void FCQueue_deque_elimination();
        void FCQueue_deque_mutex();
        void FCQueue_deque_stat();
        void FCQueue_list();
        void FCQueue_list_elimination();
        void FCQueue_list_mutex();
        void FCQueue_list_stat();

        void Vyukov_MPMCCyclicQueue();
        void Vyukov_MPMCCyclicQueue_Counted();

        void RWQueue_();
        void RWQueue_Counted();

        CPPUNIT_TEST_SUITE(Queue_TestHeader)
            CPPUNIT_TEST(MSQueue_HP);
            CPPUNIT_TEST(MSQueue_HP_relax);
            CPPUNIT_TEST(MSQueue_HP_seqcst);
            CPPUNIT_TEST(MSQueue_HP_relax_align);
            CPPUNIT_TEST(MSQueue_HP_seqcst_align);
            CPPUNIT_TEST(MSQueue_HP_Counted);
            CPPUNIT_TEST(MSQueue_HP_Counted_relax);
            CPPUNIT_TEST(MSQueue_HP_Counted_seqcst);
            CPPUNIT_TEST(MSQueue_HP_Counted_relax_align);
            CPPUNIT_TEST(MSQueue_HP_Counted_seqcst_align);

            CPPUNIT_TEST(MSQueue_DHP);
            CPPUNIT_TEST(MSQueue_DHP_relax);
            CPPUNIT_TEST(MSQueue_DHP_seqcst);
            CPPUNIT_TEST(MSQueue_DHP_relax_align);
            CPPUNIT_TEST(MSQueue_DHP_seqcst_align);
            CPPUNIT_TEST(MSQueue_DHP_Counted);
            CPPUNIT_TEST(MSQueue_DHP_Counted_relax);
            CPPUNIT_TEST(MSQueue_DHP_Counted_seqcst);
            CPPUNIT_TEST(MSQueue_DHP_Counted_relax_align);
            CPPUNIT_TEST(MSQueue_DHP_Counted_seqcst_align);

            CPPUNIT_TEST(MoirQueue_HP);
            CPPUNIT_TEST(MoirQueue_HP_relax);
            CPPUNIT_TEST(MoirQueue_HP_seqcst);
            CPPUNIT_TEST(MoirQueue_HP_relax_align);
            CPPUNIT_TEST(MoirQueue_HP_seqcst_align);
            CPPUNIT_TEST(MoirQueue_HP_Counted);
            CPPUNIT_TEST(MoirQueue_HP_Counted_relax);
            CPPUNIT_TEST(MoirQueue_HP_Counted_seqcst);
            CPPUNIT_TEST(MoirQueue_HP_Counted_relax_align);
            CPPUNIT_TEST(MoirQueue_HP_Counted_seqcst_align);

            CPPUNIT_TEST(MoirQueue_DHP);
            CPPUNIT_TEST(MoirQueue_DHP_relax);
            CPPUNIT_TEST(MoirQueue_DHP_seqcst);
            CPPUNIT_TEST(MoirQueue_DHP_relax_align);
            CPPUNIT_TEST(MoirQueue_DHP_seqcst_align);
            CPPUNIT_TEST(MoirQueue_DHP_Counted);
            CPPUNIT_TEST(MoirQueue_DHP_Counted_relax);
            CPPUNIT_TEST(MoirQueue_DHP_Counted_seqcst);
            CPPUNIT_TEST(MoirQueue_DHP_Counted_relax_align);
            CPPUNIT_TEST(MoirQueue_DHP_Counted_seqcst_align);

            CPPUNIT_TEST(OptimisticQueue_HP);
            CPPUNIT_TEST(OptimisticQueue_HP_relax);
            CPPUNIT_TEST(OptimisticQueue_HP_seqcst);
            CPPUNIT_TEST(OptimisticQueue_HP_relax_align);
            CPPUNIT_TEST(OptimisticQueue_HP_seqcst_align);
            CPPUNIT_TEST(OptimisticQueue_HP_Counted);
            CPPUNIT_TEST(OptimisticQueue_HP_Counted_relax);
            CPPUNIT_TEST(OptimisticQueue_HP_Counted_seqcst);
            CPPUNIT_TEST(OptimisticQueue_HP_Counted_relax_align);
            CPPUNIT_TEST(OptimisticQueue_HP_Counted_seqcst_align);

            CPPUNIT_TEST(OptimisticQueue_PTB);
            CPPUNIT_TEST(OptimisticQueue_PTB_relax);
            CPPUNIT_TEST(OptimisticQueue_PTB_seqcst);
            CPPUNIT_TEST(OptimisticQueue_PTB_relax_align);
            CPPUNIT_TEST(OptimisticQueue_PTB_seqcst_align);
            CPPUNIT_TEST(OptimisticQueue_PTB_Counted);
            CPPUNIT_TEST(OptimisticQueue_PTB_Counted_relax);
            CPPUNIT_TEST(OptimisticQueue_PTB_Counted_seqcst);
            CPPUNIT_TEST(OptimisticQueue_PTB_Counted_relax_align);
            CPPUNIT_TEST(OptimisticQueue_PTB_Counted_seqcst_align);

            CPPUNIT_TEST(BasketQueue_HP);
            CPPUNIT_TEST(BasketQueue_HP_relax);
            CPPUNIT_TEST(BasketQueue_HP_seqcst);
            CPPUNIT_TEST(BasketQueue_HP_relax_align);
            CPPUNIT_TEST(BasketQueue_HP_seqcst_align);
            CPPUNIT_TEST(BasketQueue_HP_Counted);
            CPPUNIT_TEST(BasketQueue_HP_Counted_relax);
            CPPUNIT_TEST(BasketQueue_HP_Counted_seqcst);
            CPPUNIT_TEST(BasketQueue_HP_Counted_relax_align);
            CPPUNIT_TEST(BasketQueue_HP_Counted_seqcst_align);

            CPPUNIT_TEST(BasketQueue_HRC);
            CPPUNIT_TEST(BasketQueue_HRC_relax);
            CPPUNIT_TEST(BasketQueue_HRC_seqcst);
            CPPUNIT_TEST(BasketQueue_HRC_relax_align);
            CPPUNIT_TEST(BasketQueue_HRC_seqcst_align);
            CPPUNIT_TEST(BasketQueue_HRC_Counted);
            CPPUNIT_TEST(BasketQueue_HRC_Counted_relax);
            CPPUNIT_TEST(BasketQueue_HRC_Counted_seqcst);
            CPPUNIT_TEST(BasketQueue_HRC_Counted_relax_align);
            CPPUNIT_TEST(BasketQueue_HRC_Counted_seqcst_align);

            CPPUNIT_TEST(BasketQueue_PTB);
            CPPUNIT_TEST(BasketQueue_PTB_relax);
            CPPUNIT_TEST(BasketQueue_PTB_seqcst);
            CPPUNIT_TEST(BasketQueue_PTB_relax_align);
            CPPUNIT_TEST(BasketQueue_PTB_seqcst_align);
            CPPUNIT_TEST(BasketQueue_PTB_Counted);
            CPPUNIT_TEST(BasketQueue_PTB_Counted_relax);
            CPPUNIT_TEST(BasketQueue_PTB_Counted_seqcst);
            CPPUNIT_TEST(BasketQueue_PTB_Counted_relax_align);
            CPPUNIT_TEST(BasketQueue_PTB_Counted_seqcst_align);

            CPPUNIT_TEST(FCQueue_deque)
            CPPUNIT_TEST(FCQueue_deque_elimination)
            CPPUNIT_TEST(FCQueue_deque_mutex)
            CPPUNIT_TEST(FCQueue_deque_stat)
            CPPUNIT_TEST(FCQueue_list)
            CPPUNIT_TEST(FCQueue_list_elimination)
            CPPUNIT_TEST(FCQueue_list_mutex)
            CPPUNIT_TEST(FCQueue_list_stat)

            CPPUNIT_TEST(RWQueue_);
            CPPUNIT_TEST(RWQueue_Counted);

            CPPUNIT_TEST(Vyukov_MPMCCyclicQueue);
            CPPUNIT_TEST(Vyukov_MPMCCyclicQueue_Counted);
        CPPUNIT_TEST_SUITE_END();

    };
} // namespace queue

#endif // #ifndef __UNIT_QUEUE_SIMPLE_H
