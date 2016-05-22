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

#ifndef CDSTEST_HDR_QUEUE_H
#define CDSTEST_HDR_QUEUE_H

#include "cppunit/cppunit_proxy.h"
#include <cds/details/defs.h>

namespace queue {

    //
    // Test queue operation in single thread mode
    //
    class HdrTestQueue: public CppUnitMini::TestCase
    {
    protected:
        template <class Queue>
        void test_no_ic()
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

            // push/pop
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

            // push_with/pop_with
            for ( size_t i = 0; i < 3; ++i ) {
                CPPUNIT_ASSERT( q.empty() );
#ifndef _DEBUG
                CPPUNIT_ASSERT( q.size() == 0 );
#endif
                CPPUNIT_ASSERT( q.enqueue_with( []( int& dest ) { dest = 1; } ));
                CPPUNIT_ASSERT( !q.empty() );
                CPPUNIT_ASSERT( q.push_with( []( int& dest ) { dest = 10; } ));
                CPPUNIT_ASSERT( !q.empty() );
#ifndef _DEBUG
                CPPUNIT_ASSERT( q.size() == 0 );   // no queue's item counter!
#endif

                it = -1;
                CPPUNIT_ASSERT( q.pop_with( [&it]( int src ) { it = src; } ));
                CPPUNIT_ASSERT( it == 1 );
                CPPUNIT_ASSERT( !q.empty() );
                CPPUNIT_ASSERT( q.dequeue_with( [&it]( int src ) { it = src; } ));
                CPPUNIT_ASSERT( it == 10 );
#ifndef _DEBUG
                CPPUNIT_ASSERT( q.size() == 0 );
#endif
                CPPUNIT_ASSERT( q.empty() );
                it += 2009;
                nPrev = it;
                CPPUNIT_ASSERT( !q.dequeue_with( [&it]( int src ) { it = src; } ));
                CPPUNIT_ASSERT( it == nPrev );   // it must not be changed!
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
        void test_ic()
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

            // push/pop
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

            // push_with/pop_with
            for ( size_t i = 0; i < 3; ++i ) {
                CPPUNIT_ASSERT( q.empty() );
                CPPUNIT_ASSERT( q.size() == 0 );
                CPPUNIT_ASSERT( q.enqueue_with( []( int& dest ) { dest = 1; } ) );
                CPPUNIT_ASSERT( !q.empty() );
                CPPUNIT_ASSERT( q.size() == 1 );
                CPPUNIT_ASSERT( q.push_with( []( int& dest ) { dest = 10; } ) );
                CPPUNIT_ASSERT( !q.empty() );
                CPPUNIT_ASSERT( q.size() == 2 );

                it = -1;
                CPPUNIT_ASSERT( q.pop_with( [&it]( int src ) { it = src; } ) );
                CPPUNIT_ASSERT( it == 1 );
                CPPUNIT_ASSERT( !q.empty() );
                CPPUNIT_ASSERT( q.size() == 1 );
                CPPUNIT_ASSERT( q.dequeue_with( [&it]( int src ) { it = src; } ) );
                CPPUNIT_ASSERT( it == 10 );
                CPPUNIT_ASSERT( q.size() == 0 );
                CPPUNIT_ASSERT( q.empty() );
                it += 2009;
                nPrev = it;
                CPPUNIT_ASSERT( !q.dequeue_with( [&it]( int src ) { it = src; } ) );
                CPPUNIT_ASSERT( it == nPrev );   // it must not be changed!

                CPPUNIT_ASSERT( q.size() == 0 );
                CPPUNIT_ASSERT( q.empty() );
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

        template <class Queue>
        void test_bounded_no_ic()
        {
            Queue   q;
            test_with( q );
            test_emplace( q );

            CPPUNIT_ASSERT( q.empty() );
            size_t nCapacity = q.capacity();
            for ( size_t i = 0; i < nCapacity; ++i ) {
                CPPUNIT_CHECK_EX( q.push( static_cast<int>(i) ), "capacity=" << nCapacity << ", i=" << i );
            }
            // The queue is full
            CPPUNIT_CHECK( !q.empty() );
            CPPUNIT_ASSERT( !q.push_with( [nCapacity]( int& itm ) { itm = static_cast<int>(nCapacity); } ) );
            int n = -1;
            CPPUNIT_CHECK( q.pop(n) );
            CPPUNIT_CHECK( n == 0 );
            CPPUNIT_ASSERT( q.push( static_cast<int>(nCapacity) ) );
            CPPUNIT_ASSERT( !q.push( static_cast<int>(nCapacity) ) );
        }

        template <class Queue>
        void test_bounded_ic()
        {
            Queue   q;
            test_ic_with( q );
            test_emplace_ic( q );

            CPPUNIT_ASSERT( q.empty() );
            size_t nCapacity = q.capacity();
            for ( size_t i = 0; i < nCapacity; ++i ) {
                CPPUNIT_CHECK_EX( q.push( static_cast<int>(i) ), "capacity=" << nCapacity << ", i=" << i );
            }
            // The queue is full
            CPPUNIT_CHECK( !q.empty() );
            CPPUNIT_CHECK( q.size() == nCapacity );
            CPPUNIT_ASSERT( !q.push_with( [nCapacity]( int& itm ) { itm = static_cast<int>(nCapacity); } ));
            int n = -1;
            CPPUNIT_CHECK( q.pop( n ) );
            CPPUNIT_CHECK( n == 0 );
            CPPUNIT_ASSERT( q.push( static_cast<int>(nCapacity) ) );
            CPPUNIT_CHECK( q.size() == nCapacity );
            CPPUNIT_ASSERT( !q.push( static_cast<int>(nCapacity) ) );
            CPPUNIT_CHECK( q.size() == nCapacity );
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

        void OptimisticQueue_DHP();
        void OptimisticQueue_DHP_relax();
        void OptimisticQueue_DHP_seqcst();
        void OptimisticQueue_DHP_relax_align();
        void OptimisticQueue_DHP_seqcst_align();
        void OptimisticQueue_DHP_Counted();
        void OptimisticQueue_DHP_Counted_relax();
        void OptimisticQueue_DHP_Counted_seqcst();
        void OptimisticQueue_DHP_Counted_relax_align();
        void OptimisticQueue_DHP_Counted_seqcst_align();

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

        void BasketQueue_DHP();
        void BasketQueue_DHP_relax();
        void BasketQueue_DHP_seqcst();
        void BasketQueue_DHP_relax_align();
        void BasketQueue_DHP_seqcst_align();
        void BasketQueue_DHP_Counted();
        void BasketQueue_DHP_Counted_relax();
        void BasketQueue_DHP_Counted_seqcst();
        void BasketQueue_DHP_Counted_relax_align();
        void BasketQueue_DHP_Counted_seqcst_align();

        void RWQueue_default();
        void RWQueue_mutex();
        void RWQueue_ic();
        void RWQueue_ic_mutex();
        void RWQueue_padding();

        void TsigasCycleQueue_static();
        void TsigasCycleQueue_static_ic();
        void TsigasCycleQueue_dyn();
        void TsigasCycleQueue_dyn_ic();

        void VyukovMPMCCycleQueue_static();
        void VyukovMPMCCycleQueue_static_ic();
        void VyukovMPMCCycleQueue_dyn();
        void VyukovMPMCCycleQueue_dyn_ic();

		void WilliamsQueue_default();
		void WilliamsQueue_ic();

        CPPUNIT_TEST_SUITE( HdrTestQueue )
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

            CPPUNIT_TEST(OptimisticQueue_DHP);
            CPPUNIT_TEST(OptimisticQueue_DHP_relax);
            CPPUNIT_TEST(OptimisticQueue_DHP_seqcst);
            CPPUNIT_TEST(OptimisticQueue_DHP_relax_align);
            CPPUNIT_TEST(OptimisticQueue_DHP_seqcst_align);
            CPPUNIT_TEST(OptimisticQueue_DHP_Counted);
            CPPUNIT_TEST(OptimisticQueue_DHP_Counted_relax);
            CPPUNIT_TEST(OptimisticQueue_DHP_Counted_seqcst);
            CPPUNIT_TEST(OptimisticQueue_DHP_Counted_relax_align);
            CPPUNIT_TEST(OptimisticQueue_DHP_Counted_seqcst_align);

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

            CPPUNIT_TEST(BasketQueue_DHP);
            CPPUNIT_TEST(BasketQueue_DHP_relax);
            CPPUNIT_TEST(BasketQueue_DHP_seqcst);
            CPPUNIT_TEST(BasketQueue_DHP_relax_align);
            CPPUNIT_TEST(BasketQueue_DHP_seqcst_align);
            CPPUNIT_TEST(BasketQueue_DHP_Counted);
            CPPUNIT_TEST(BasketQueue_DHP_Counted_relax);
            CPPUNIT_TEST(BasketQueue_DHP_Counted_seqcst);
            CPPUNIT_TEST(BasketQueue_DHP_Counted_relax_align);
            CPPUNIT_TEST(BasketQueue_DHP_Counted_seqcst_align);

            CPPUNIT_TEST( TsigasCycleQueue_static )
            CPPUNIT_TEST( TsigasCycleQueue_static_ic )
            CPPUNIT_TEST( TsigasCycleQueue_dyn )
            CPPUNIT_TEST( TsigasCycleQueue_dyn_ic )

            CPPUNIT_TEST( VyukovMPMCCycleQueue_static )
            CPPUNIT_TEST( VyukovMPMCCycleQueue_static_ic )
            CPPUNIT_TEST( VyukovMPMCCycleQueue_dyn )
            CPPUNIT_TEST( VyukovMPMCCycleQueue_dyn_ic )

            CPPUNIT_TEST( RWQueue_default)
            CPPUNIT_TEST( RWQueue_mutex )
            CPPUNIT_TEST( RWQueue_ic )
            CPPUNIT_TEST( RWQueue_ic_mutex )
            CPPUNIT_TEST( RWQueue_padding )

			CPPUNIT_TEST( WilliamsQueue_default )
			CPPUNIT_TEST( WilliamsQueue_ic )

        CPPUNIT_TEST_SUITE_END();

    };
} // namespace queue

#endif // #ifndef CDSTEST_HDR_QUEUE_H
