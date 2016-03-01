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

#ifndef CDSUNIT_QUEUE_DEFS_H
#define CDSUNIT_QUEUE_DEFS_H

// MoirQueue
#define CDSUNIT_DECLARE_MoirQueue( ITEM_TYPE ) \
    TEST_CASE( MoirQueue_HP, ITEM_TYPE ) \
    TEST_CASE( MoirQueue_HP_michaelAlloc, ITEM_TYPE ) \
    TEST_CASE( MoirQueue_HP_seqcst, ITEM_TYPE ) \
    TEST_CASE( MoirQueue_HP_ic, ITEM_TYPE ) \
    TEST_CASE( MoirQueue_HP_stat, ITEM_TYPE ) \
    TEST_CASE( MoirQueue_DHP, ITEM_TYPE ) \
    TEST_CASE( MoirQueue_DHP_michaelAlloc, ITEM_TYPE ) \
    TEST_CASE( MoirQueue_DHP_seqcst, ITEM_TYPE ) \
    TEST_CASE( MoirQueue_DHP_ic, ITEM_TYPE ) \
    TEST_CASE( MoirQueue_DHP_stat, ITEM_TYPE )

#define CDSUNIT_TEST_MoirQueue \
    CPPUNIT_TEST(MoirQueue_HP) \
    CPPUNIT_TEST(MoirQueue_HP_michaelAlloc) \
    CPPUNIT_TEST(MoirQueue_HP_seqcst) \
    CPPUNIT_TEST(MoirQueue_HP_ic) \
    CPPUNIT_TEST(MoirQueue_HP_stat) \
    CPPUNIT_TEST(MoirQueue_DHP) \
    CPPUNIT_TEST(MoirQueue_DHP_michaelAlloc) \
    CPPUNIT_TEST(MoirQueue_DHP_seqcst) \
    CPPUNIT_TEST(MoirQueue_DHP_ic) \
    CPPUNIT_TEST(MoirQueue_DHP_stat)

// MSQueue
#define CDSUNIT_DECLARE_MSQueue( ITEM_TYPE ) \
    TEST_CASE( MSQueue_HP, ITEM_TYPE  ) \
    TEST_CASE( MSQueue_HP_michaelAlloc, ITEM_TYPE  ) \
    TEST_CASE( MSQueue_HP_seqcst, ITEM_TYPE  ) \
    TEST_CASE( MSQueue_HP_ic, ITEM_TYPE  ) \
    TEST_CASE( MSQueue_HP_stat, ITEM_TYPE  ) \
    TEST_CASE( MSQueue_DHP, ITEM_TYPE ) \
    TEST_CASE( MSQueue_DHP_michaelAlloc, ITEM_TYPE ) \
    TEST_CASE( MSQueue_DHP_seqcst, ITEM_TYPE ) \
    TEST_CASE( MSQueue_DHP_ic, ITEM_TYPE ) \
    TEST_CASE( MSQueue_DHP_stat, ITEM_TYPE )

#define CDSUNIT_TEST_MSQueue \
    CPPUNIT_TEST(MSQueue_HP) \
    CPPUNIT_TEST(MSQueue_HP_michaelAlloc) \
    CPPUNIT_TEST(MSQueue_HP_seqcst) \
    CPPUNIT_TEST(MSQueue_HP_ic) \
    CPPUNIT_TEST(MSQueue_HP_stat) \
    CPPUNIT_TEST(MSQueue_DHP) \
    CPPUNIT_TEST(MSQueue_DHP_michaelAlloc) \
    CPPUNIT_TEST(MSQueue_DHP_seqcst) \
    CPPUNIT_TEST(MSQueue_DHP_ic) \
    CPPUNIT_TEST(MSQueue_DHP_stat)


// OptimisticQueue
#define CDSUNIT_DECLARE_OptimisticQueue( ITEM_TYPE ) \
    TEST_CASE(OptimisticQueue_HP, ITEM_TYPE ) \
    TEST_CASE(OptimisticQueue_HP_michaelAlloc, ITEM_TYPE ) \
    TEST_CASE(OptimisticQueue_HP_seqcst, ITEM_TYPE ) \
    TEST_CASE(OptimisticQueue_HP_ic, ITEM_TYPE ) \
    TEST_CASE(OptimisticQueue_HP_stat, ITEM_TYPE ) \
    TEST_CASE(OptimisticQueue_DHP, ITEM_TYPE ) \
    TEST_CASE(OptimisticQueue_DHP_michaelAlloc, ITEM_TYPE ) \
    TEST_CASE(OptimisticQueue_DHP_seqcst, ITEM_TYPE ) \
    TEST_CASE(OptimisticQueue_DHP_ic, ITEM_TYPE ) \
    TEST_CASE(OptimisticQueue_DHP_stat, ITEM_TYPE )

#define CDSUNIT_TEST_OptimisticQueue \
    CPPUNIT_TEST(OptimisticQueue_HP) \
    CPPUNIT_TEST(OptimisticQueue_HP_michaelAlloc) \
    CPPUNIT_TEST(OptimisticQueue_HP_seqcst) \
    CPPUNIT_TEST(OptimisticQueue_HP_ic) \
    CPPUNIT_TEST(OptimisticQueue_HP_stat) \
    CPPUNIT_TEST(OptimisticQueue_DHP) \
    CPPUNIT_TEST(OptimisticQueue_DHP_michaelAlloc) \
    CPPUNIT_TEST(OptimisticQueue_DHP_seqcst) \
    CPPUNIT_TEST(OptimisticQueue_DHP_ic) \
    CPPUNIT_TEST(OptimisticQueue_DHP_stat)


// BasketQueue
#define CDSUNIT_DECLARE_BasketQueue( ITEM_TYPE ) \
    TEST_CASE( BasketQueue_HP, ITEM_TYPE  ) \
    TEST_CASE( BasketQueue_HP_michaelAlloc, ITEM_TYPE ) \
    TEST_CASE( BasketQueue_HP_seqcst, ITEM_TYPE  ) \
    TEST_CASE( BasketQueue_HP_ic, ITEM_TYPE  ) \
    TEST_CASE( BasketQueue_HP_stat, ITEM_TYPE  ) \
    TEST_CASE( BasketQueue_DHP, ITEM_TYPE ) \
    TEST_CASE( BasketQueue_DHP_michaelAlloc, ITEM_TYPE ) \
    TEST_CASE( BasketQueue_DHP_seqcst, ITEM_TYPE ) \
    TEST_CASE( BasketQueue_DHP_ic, ITEM_TYPE ) \
    TEST_CASE( BasketQueue_DHP_stat, ITEM_TYPE )

#define CDSUNIT_TEST_BasketQueue \
    CPPUNIT_TEST(BasketQueue_HP) \
    CPPUNIT_TEST(BasketQueue_HP_michaelAlloc) \
    CPPUNIT_TEST(BasketQueue_HP_seqcst) \
    CPPUNIT_TEST(BasketQueue_HP_ic) \
    CPPUNIT_TEST(BasketQueue_HP_stat) \
    CPPUNIT_TEST(BasketQueue_DHP) \
    CPPUNIT_TEST(BasketQueue_DHP_michaelAlloc) \
    CPPUNIT_TEST(BasketQueue_DHP_seqcst) \
    CPPUNIT_TEST(BasketQueue_DHP_ic) \
    CPPUNIT_TEST(BasketQueue_DHP_stat)


// FCQueue
#define CDSUNIT_DECLARE_FCQueue( ITEM_TYPE ) \
    TEST_CASE( FCQueue_autoWaitStrategy, ITEM_TYPE )
//    TEST_CASE( FCQueue_timedWait_Global_Mutex_And_CondVar, ITEM_TYPE )\
//    TEST_CASE( FCQueue_timedWait_Local_Mutex_And_CondVar, ITEM_TYPE )\
//    TEST_CASE( FCQueue_deque, ITEM_TYPE ) \
//    TEST_CASE( FCQueue_backof, ITEM_TYPE ) \
//	TEST_CASE( FCQueue_oneMutex_oneCondVar, ITEM_TYPE ) \
//	TEST_CASE( FCQueue_singleMutex_MultCondVar, ITEM_TYPE ) \
//	TEST_CASE( FCQueue_multMutex_multCondVar, ITEM_TYPE ) \
    //TEST_CASE( FCQueue_deque_elimination, ITEM_TYPE ) \
    //TEST_CASE( FCQueue_deque_elimination_stat, ITEM_TYPE ) \
    //TEST_CASE( FCQueue_list, ITEM_TYPE ) \
    //TEST_CASE( FCQueue_list_elimination, ITEM_TYPE ) \
    //TEST_CASE( FCQueue_list_elimination_stat, ITEM_TYPE )

#define CDSUNIT_TEST_FCQueue \
    CPPUNIT_TEST( FCQueue_autoWaitStrategy)
//    CPPUNIT_TEST( FCQueue_timedWait_Global_Mutex_And_CondVar)\
//	CPPUNIT_TEST( FCQueue_timedWait_Local_Mutex_And_CondVar)\
//    CPPUNIT_TEST( FCQueue_deque) \
//    CPPUNIT_TEST( FCQueue_backof) \
//	CPPUNIT_TEST( FCQueue_oneMutex_oneCondVar) \
//	CPPUNIT_TEST( FCQueue_singleMutex_MultCondVar) \
//	CPPUNIT_TEST( FCQueue_multMutex_multCondVar)
    //CPPUNIT_TEST( FCQueue_deque_elimination) \
    //CPPUNIT_TEST( FCQueue_deque_elimination_stat) \
    //CPPUNIT_TEST( FCQueue_list) \
    //CPPUNIT_TEST( FCQueue_list_elimination) \
    //CPPUNIT_TEST( FCQueue_list_elimination_stat)


// FCDeque
#define CDSUNIT_DECLARE_FCDeque( ITEM_TYPE ) \
    TEST_CASE( FCDequeL_default, ITEM_TYPE ) \
    TEST_CASE( FCDequeL_mutex, ITEM_TYPE ) \
    TEST_CASE( FCDequeL_stat, ITEM_TYPE ) \
    TEST_CASE( FCDequeL_elimination, ITEM_TYPE ) \
    TEST_CASE( FCDequeL_elimination_stat, ITEM_TYPE ) \
    TEST_CASE( FCDequeL_boost, ITEM_TYPE ) \
    TEST_CASE( FCDequeL_boost_stat, ITEM_TYPE ) \
    TEST_CASE( FCDequeL_boost_elimination, ITEM_TYPE ) \
    TEST_CASE( FCDequeL_boost_elimination_stat, ITEM_TYPE ) \
    TEST_CASE( FCDequeR_default, ITEM_TYPE ) \
    TEST_CASE( FCDequeR_mutex, ITEM_TYPE ) \
    TEST_CASE( FCDequeR_stat, ITEM_TYPE ) \
    TEST_CASE( FCDequeR_elimination, ITEM_TYPE ) \
    TEST_CASE( FCDequeR_elimination_stat, ITEM_TYPE ) \
    TEST_CASE( FCDequeR_boost, ITEM_TYPE ) \
    TEST_CASE( FCDequeR_boost_stat, ITEM_TYPE ) \
    TEST_CASE( FCDequeR_boost_elimination, ITEM_TYPE ) \
    TEST_CASE( FCDequeR_boost_elimination_stat, ITEM_TYPE )

#define CDSUNIT_TEST_FCDeque \
    CPPUNIT_TEST( FCDequeL_default ) \
    CPPUNIT_TEST( FCDequeL_mutex ) \
    CPPUNIT_TEST( FCDequeL_stat ) \
    CPPUNIT_TEST( FCDequeL_elimination ) \
    CPPUNIT_TEST( FCDequeL_elimination_stat ) \
    CPPUNIT_TEST( FCDequeL_boost ) \
    CPPUNIT_TEST( FCDequeL_boost_stat ) \
    CPPUNIT_TEST( FCDequeL_boost_elimination ) \
    CPPUNIT_TEST( FCDequeL_boost_elimination_stat ) \
    CPPUNIT_TEST( FCDequeR_default ) \
    CPPUNIT_TEST( FCDequeR_mutex ) \
    CPPUNIT_TEST( FCDequeR_stat ) \
    CPPUNIT_TEST( FCDequeR_elimination ) \
    CPPUNIT_TEST( FCDequeR_elimination_stat ) \
    CPPUNIT_TEST( FCDequeR_boost ) \
    CPPUNIT_TEST( FCDequeR_boost_stat ) \
    CPPUNIT_TEST( FCDequeR_boost_elimination ) \
    CPPUNIT_TEST( FCDequeR_boost_elimination_stat )


// RWQueue
#define CDSUNIT_DECLARE_RWQueue( ITEM_TYPE ) \
    TEST_CASE( RWQueue_Spin, ITEM_TYPE ) \
    TEST_CASE( RWQueue_Spin_ic, ITEM_TYPE ) \
    TEST_CASE( RWQueue_mutex, ITEM_TYPE )

#define CDSUNIT_TEST_RWQueue \
    CPPUNIT_TEST(RWQueue_Spin) \
    CPPUNIT_TEST(RWQueue_Spin_ic) \
    CPPUNIT_TEST(RWQueue_mutex)

// TsigasCycleQueue
#define CDSUNIT_DECLARE_TsigasCycleQueue( ITEM_TYPE ) \
    TEST_BOUNDED(TsigasCycleQueue_dyn, ITEM_TYPE) \
    TEST_BOUNDED(TsigasCycleQueue_dyn_michaelAlloc, ITEM_TYPE) \
    TEST_BOUNDED(TsigasCycleQueue_dyn_ic, ITEM_TYPE)

#define CDSUNIT_TEST_TsigasCycleQueue \
    CPPUNIT_TEST(TsigasCycleQueue_dyn) \
    CPPUNIT_TEST(TsigasCycleQueue_dyn_michaelAlloc) \
    CPPUNIT_TEST(TsigasCycleQueue_dyn_ic)


// VyukovMPMCCycleQueue
#define CDSUNIT_DECLARE_VyukovMPMCCycleQueue( ITEM_TYPE ) \
    TEST_BOUNDED(VyukovMPMCCycleQueue_dyn, ITEM_TYPE) \
    TEST_BOUNDED(VyukovMPMCCycleQueue_dyn_michaelAlloc, ITEM_TYPE) \
    TEST_BOUNDED(VyukovMPMCCycleQueue_dyn_ic, ITEM_TYPE) \


#define CDSUNIT_TEST_VyukovMPMCCycleQueue \
    CPPUNIT_TEST(VyukovMPMCCycleQueue_dyn) \
    CPPUNIT_TEST(VyukovMPMCCycleQueue_dyn_michaelAlloc) \
    CPPUNIT_TEST(VyukovMPMCCycleQueue_dyn_ic) \

// SegmentedQueue
#define CDSUNIT_DECLARE_SegmentedQueue( ITEM_TYPE ) \
    TEST_SEGMENTED( SegmentedQueue_HP_spin, ITEM_TYPE ) \
    TEST_SEGMENTED( SegmentedQueue_HP_spin_padding, ITEM_TYPE ) \
    TEST_SEGMENTED( SegmentedQueue_HP_spin_stat, ITEM_TYPE ) \
    TEST_SEGMENTED( SegmentedQueue_HP_mutex, ITEM_TYPE ) \
    TEST_SEGMENTED( SegmentedQueue_HP_mutex_padding, ITEM_TYPE ) \
    TEST_SEGMENTED( SegmentedQueue_HP_mutex_stat, ITEM_TYPE ) \
    TEST_SEGMENTED( SegmentedQueue_DHP_spin, ITEM_TYPE ) \
    TEST_SEGMENTED( SegmentedQueue_DHP_spin_padding, ITEM_TYPE ) \
    TEST_SEGMENTED( SegmentedQueue_DHP_spin_stat, ITEM_TYPE ) \
    TEST_SEGMENTED( SegmentedQueue_DHP_mutex, ITEM_TYPE ) \
    TEST_SEGMENTED( SegmentedQueue_DHP_mutex_padding, ITEM_TYPE ) \
    TEST_SEGMENTED( SegmentedQueue_DHP_mutex_stat, ITEM_TYPE )

#define CDSUNIT_TEST_SegmentedQueue \
    CPPUNIT_TEST( SegmentedQueue_HP_spin ) \
    CPPUNIT_TEST( SegmentedQueue_HP_spin_padding ) \
    CPPUNIT_TEST( SegmentedQueue_HP_spin_stat ) \
    CPPUNIT_TEST( SegmentedQueue_HP_mutex ) \
    CPPUNIT_TEST( SegmentedQueue_HP_mutex_padding ) \
    CPPUNIT_TEST( SegmentedQueue_HP_mutex_stat ) \
    CPPUNIT_TEST( SegmentedQueue_DHP_spin ) \
    CPPUNIT_TEST( SegmentedQueue_DHP_spin_padding ) \
    CPPUNIT_TEST( SegmentedQueue_DHP_spin_stat ) \
    CPPUNIT_TEST( SegmentedQueue_DHP_mutex ) \
    CPPUNIT_TEST( SegmentedQueue_DHP_mutex_padding ) \
    CPPUNIT_TEST( SegmentedQueue_DHP_mutex_stat )

// std::queue
#define CDSUNIT_DECLARE_StdQueue( ITEM_TYPE ) \
    TEST_CASE( StdQueue_deque_Spinlock, ITEM_TYPE ) \
    TEST_CASE( StdQueue_list_Spinlock, ITEM_TYPE ) \
    TEST_CASE( StdQueue_deque_BoostMutex, ITEM_TYPE ) \
    TEST_CASE( StdQueue_list_BoostMutex, ITEM_TYPE )

#define CDSUNIT_TEST_StdQueue \
    CPPUNIT_TEST(StdQueue_deque_Spinlock) \
    CPPUNIT_TEST(StdQueue_list_Spinlock) \
    CPPUNIT_TEST(StdQueue_deque_BoostMutex) \
    CPPUNIT_TEST(StdQueue_list_BoostMutex)


#endif // #ifndef CDSUNIT_QUEUE_DEFS_H
