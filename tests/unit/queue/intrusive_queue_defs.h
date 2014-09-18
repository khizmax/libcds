//$$CDS-header$$

#ifndef __CDSUNIT_INTRUSIVE_QUEUE_DEFS_H
#define __CDSUNIT_INTRUSIVE_QUEUE_DEFS_H

// MSQueue
#define CDSUNIT_DECLARE_MSQueue \
    TEST_CASE(MSQueue_HP,               cds::intrusive::single_link::node< cds::gc::HP > ) \
    TEST_CASE(MSQueue_HP_ic,            cds::intrusive::single_link::node< cds::gc::HP > ) \
    TEST_CASE(MSQueue_HP_stat,          cds::intrusive::single_link::node< cds::gc::HP > ) \
    TEST_CASE(MSQueue_HP_seqcst,        cds::intrusive::single_link::node< cds::gc::HP > ) \
    TEST_CASE(MSQueue_HRC,              cds::intrusive::single_link::node< cds::gc::HRC > ) \
    TEST_CASE(MSQueue_HRC_ic,           cds::intrusive::single_link::node< cds::gc::HRC > ) \
    TEST_CASE(MSQueue_HRC_stat,         cds::intrusive::single_link::node< cds::gc::HRC > ) \
    TEST_CASE(MSQueue_HRC_seqcst,       cds::intrusive::single_link::node< cds::gc::HRC > ) \
    TEST_CASE(MSQueue_PTB,              cds::intrusive::single_link::node< cds::gc::PTB > ) \
    TEST_CASE(MSQueue_PTB_ic,           cds::intrusive::single_link::node< cds::gc::PTB > ) \
    TEST_CASE(MSQueue_PTB_stat,         cds::intrusive::single_link::node< cds::gc::PTB > ) \
    TEST_CASE(MSQueue_PTB_seqcst,       cds::intrusive::single_link::node< cds::gc::PTB > )

#define CDSUNIT_TEST_MSQueue \
    CPPUNIT_TEST(MSQueue_HP) \
    CPPUNIT_TEST(MSQueue_HP_ic) \
    CPPUNIT_TEST(MSQueue_HP_stat) \
    CPPUNIT_TEST(MSQueue_HP_seqcst) \
    /*CPPUNIT_TEST(MSQueue_HRC)*/ \
    /*CPPUNIT_TEST(MSQueue_HRC_ic)*/ \
    /*CPPUNIT_TEST(MSQueue_HRC_stat)*/ \
    /*CPPUNIT_TEST(MSQueue_HRC_seqcst)*/ \
    CPPUNIT_TEST(MSQueue_PTB) \
    CPPUNIT_TEST(MSQueue_PTB_ic) \
    CPPUNIT_TEST(MSQueue_PTB_stat) \
    CPPUNIT_TEST(MSQueue_PTB_seqcst)

// MoirQueue
#define CDSUNIT_DECLARE_MoirQueue \
    TEST_CASE(MoirQueue_HP,               cds::intrusive::single_link::node< cds::gc::HP > ) \
    TEST_CASE(MoirQueue_HP_ic,            cds::intrusive::single_link::node< cds::gc::HP > ) \
    TEST_CASE(MoirQueue_HP_stat,          cds::intrusive::single_link::node< cds::gc::HP > ) \
    TEST_CASE(MoirQueue_HP_seqcst,        cds::intrusive::single_link::node< cds::gc::HP > ) \
    TEST_CASE(MoirQueue_HRC,              cds::intrusive::single_link::node< cds::gc::HRC > ) \
    TEST_CASE(MoirQueue_HRC_ic,           cds::intrusive::single_link::node< cds::gc::HRC > ) \
    TEST_CASE(MoirQueue_HRC_stat,         cds::intrusive::single_link::node< cds::gc::HRC > ) \
    TEST_CASE(MoirQueue_HRC_seqcst,       cds::intrusive::single_link::node< cds::gc::HRC > ) \
    TEST_CASE(MoirQueue_PTB,              cds::intrusive::single_link::node< cds::gc::PTB > ) \
    TEST_CASE(MoirQueue_PTB_ic,           cds::intrusive::single_link::node< cds::gc::PTB > ) \
    TEST_CASE(MoirQueue_PTB_stat,         cds::intrusive::single_link::node< cds::gc::PTB > ) \
    TEST_CASE(MoirQueue_PTB_seqcst,       cds::intrusive::single_link::node< cds::gc::PTB > )

#define CDSUNIT_TEST_MoirQueue \
    CPPUNIT_TEST(MoirQueue_HP) \
    CPPUNIT_TEST(MoirQueue_HP_ic) \
    CPPUNIT_TEST(MoirQueue_HP_stat) \
    CPPUNIT_TEST(MoirQueue_HP_seqcst) \
    /*CPPUNIT_TEST(MoirQueue_HRC)*/ \
    /*CPPUNIT_TEST(MoirQueue_HRC_ic)*/ \
    /*CPPUNIT_TEST(MoirQueue_HRC_stat)*/ \
    /*CPPUNIT_TEST(MoirQueue_HRC_seqcst)*/ \
    CPPUNIT_TEST(MoirQueue_PTB) \
    CPPUNIT_TEST(MoirQueue_PTB_ic) \
    CPPUNIT_TEST(MoirQueue_PTB_stat) \
    CPPUNIT_TEST(MoirQueue_PTB_seqcst)


// OptimisticQueue
#define CDSUNIT_DECLARE_OptimisticQueue \
    TEST_CASE(OptimisticQueue_HP,               cds::intrusive::optimistic_queue::node< cds::gc::HP > ) \
    TEST_CASE(OptimisticQueue_HP_ic,            cds::intrusive::optimistic_queue::node< cds::gc::HP > ) \
    TEST_CASE(OptimisticQueue_HP_stat,          cds::intrusive::optimistic_queue::node< cds::gc::HP > ) \
    TEST_CASE(OptimisticQueue_HP_seqcst,        cds::intrusive::optimistic_queue::node< cds::gc::HP > ) \
    TEST_CASE(OptimisticQueue_PTB,              cds::intrusive::optimistic_queue::node< cds::gc::PTB > ) \
    TEST_CASE(OptimisticQueue_PTB_ic,           cds::intrusive::optimistic_queue::node< cds::gc::PTB > ) \
    TEST_CASE(OptimisticQueue_PTB_stat,         cds::intrusive::optimistic_queue::node< cds::gc::PTB > ) \
    TEST_CASE(OptimisticQueue_PTB_seqcst,       cds::intrusive::optimistic_queue::node< cds::gc::PTB > )

#define CDSUNIT_TEST_OptimisticQueue \
    CPPUNIT_TEST(OptimisticQueue_HP) \
    CPPUNIT_TEST(OptimisticQueue_HP_ic) \
    CPPUNIT_TEST(OptimisticQueue_HP_stat) \
    CPPUNIT_TEST(OptimisticQueue_HP_seqcst) \
    CPPUNIT_TEST(OptimisticQueue_PTB) \
    CPPUNIT_TEST(OptimisticQueue_PTB_ic) \
    CPPUNIT_TEST(OptimisticQueue_PTB_stat) \
    CPPUNIT_TEST(OptimisticQueue_PTB_seqcst)


// BasketQueue
#define CDSUNIT_DECLARE_BasketQueue \
    TEST_CASE(BasketQueue_HP,               cds::intrusive::basket_queue::node< cds::gc::HP > ) \
    TEST_CASE(BasketQueue_HP_ic,            cds::intrusive::basket_queue::node< cds::gc::HP > ) \
    TEST_CASE(BasketQueue_HP_stat,          cds::intrusive::basket_queue::node< cds::gc::HP > ) \
    TEST_CASE(BasketQueue_HP_seqcst,        cds::intrusive::basket_queue::node< cds::gc::HP > ) \
    TEST_CASE(BasketQueue_HRC,              cds::intrusive::basket_queue::node< cds::gc::HRC > ) \
    TEST_CASE(BasketQueue_HRC_ic,           cds::intrusive::basket_queue::node< cds::gc::HRC > ) \
    TEST_CASE(BasketQueue_HRC_stat,         cds::intrusive::basket_queue::node< cds::gc::HRC > ) \
    TEST_CASE(BasketQueue_HRC_seqcst,       cds::intrusive::basket_queue::node< cds::gc::HRC > ) \
    TEST_CASE(BasketQueue_PTB,              cds::intrusive::basket_queue::node< cds::gc::PTB > ) \
    TEST_CASE(BasketQueue_PTB_ic,           cds::intrusive::basket_queue::node< cds::gc::PTB > ) \
    TEST_CASE(BasketQueue_PTB_stat,         cds::intrusive::basket_queue::node< cds::gc::PTB > ) \
    TEST_CASE(BasketQueue_PTB_seqcst,       cds::intrusive::basket_queue::node< cds::gc::PTB > )

#define CDSUNIT_TEST_BasketQueue \
    CPPUNIT_TEST(BasketQueue_HP) \
    CPPUNIT_TEST(BasketQueue_HP_ic) \
    CPPUNIT_TEST(BasketQueue_HP_stat) \
    CPPUNIT_TEST(BasketQueue_HP_seqcst) \
    /*CPPUNIT_TEST(BasketQueue_HRC)*/ \
    /*CPPUNIT_TEST(BasketQueue_HRC_ic)*/ \
    /*CPPUNIT_TEST(BasketQueue_HRC_stat)*/ \
    /*CPPUNIT_TEST(BasketQueue_HRC_seqcst)*/ \
    CPPUNIT_TEST(BasketQueue_PTB) \
    CPPUNIT_TEST(BasketQueue_PTB_ic) \
    CPPUNIT_TEST(BasketQueue_PTB_stat) \
    CPPUNIT_TEST(BasketQueue_PTB_seqcst)


// MichaelDeque
#define CDSUNIT_DECLARE_MichaelDeque \
    TEST_CASE(MichaelDequeL_HP,               cds::intrusive::michael_deque::node< cds::gc::HP > ) \
    TEST_CASE(MichaelDequeL_HP_ic,            cds::intrusive::michael_deque::node< cds::gc::HP > ) \
    TEST_CASE(MichaelDequeL_HP_stat,          cds::intrusive::michael_deque::node< cds::gc::HP > ) \
    TEST_CASE(MichaelDequeL_PTB,              cds::intrusive::michael_deque::node< cds::gc::PTB > ) \
    TEST_CASE(MichaelDequeL_PTB_ic,           cds::intrusive::michael_deque::node< cds::gc::PTB > ) \
    TEST_CASE(MichaelDequeL_PTB_stat,         cds::intrusive::michael_deque::node< cds::gc::PTB > ) \
    TEST_CASE(MichaelDequeR_HP,               cds::intrusive::michael_deque::node< cds::gc::HP > ) \
    TEST_CASE(MichaelDequeR_HP_ic,            cds::intrusive::michael_deque::node< cds::gc::HP > ) \
    TEST_CASE(MichaelDequeR_HP_stat,          cds::intrusive::michael_deque::node< cds::gc::HP > ) \
    TEST_CASE(MichaelDequeR_PTB,              cds::intrusive::michael_deque::node< cds::gc::PTB > ) \
    TEST_CASE(MichaelDequeR_PTB_ic,           cds::intrusive::michael_deque::node< cds::gc::PTB > ) \
    TEST_CASE(MichaelDequeR_PTB_stat,         cds::intrusive::michael_deque::node< cds::gc::PTB > )

#define CDSUNIT_TEST_MichaelDeque \
    CPPUNIT_TEST(MichaelDequeL_HP) \
    CPPUNIT_TEST(MichaelDequeL_HP_ic) \
    CPPUNIT_TEST(MichaelDequeL_HP_stat) \
    CPPUNIT_TEST(MichaelDequeL_PTB) \
    CPPUNIT_TEST(MichaelDequeL_PTB_ic) \
    CPPUNIT_TEST(MichaelDequeL_PTB_stat) \
    CPPUNIT_TEST(MichaelDequeR_HP) \
    CPPUNIT_TEST(MichaelDequeR_HP_ic) \
    CPPUNIT_TEST(MichaelDequeR_HP_stat) \
    CPPUNIT_TEST(MichaelDequeR_PTB) \
    CPPUNIT_TEST(MichaelDequeR_PTB_ic) \
    CPPUNIT_TEST(MichaelDequeR_PTB_stat)


// TsigasCycleQueue
#define CDSUNIT_DECLARE_TsigasCycleQueue \
    TEST_BOUNDED(TsigasCycleQueue_dyn) \
    TEST_BOUNDED(TsigasCycleQueue_dyn_ic)

#define CDSUNIT_TEST_TsigasCycleQueue \
    CPPUNIT_TEST(TsigasCycleQueue_dyn) \
    CPPUNIT_TEST(TsigasCycleQueue_dyn_ic)


// VyukovMPMCCycleQueue
#define CDSUNIT_DECLARE_VyukovMPMCCycleQueue \
    TEST_BOUNDED(VyukovMPMCCycleQueue_dyn) \
    TEST_BOUNDED(VyukovMPMCCycleQueue_dyn_ic)

#define CDSUNIT_TEST_VyukovMPMCCycleQueue \
    CPPUNIT_TEST(VyukovMPMCCycleQueue_dyn) \
    CPPUNIT_TEST(VyukovMPMCCycleQueue_dyn_ic)


// FCQueue
#define CDSUNIT_DECLARE_FCQueue \
    TEST_FCQUEUE(FCQueue_list_delay2, boost::intrusive::list_base_hook<> ) \
    TEST_FCQUEUE(FCQueue_list_delay2_elimination, boost::intrusive::list_base_hook<> ) \
    TEST_FCQUEUE(FCQueue_list_delay2_elimination_stat, boost::intrusive::list_base_hook<> ) \
    TEST_FCQUEUE(FCQueue_list_expbackoff_elimination, boost::intrusive::list_base_hook<> ) \
    TEST_FCQUEUE(FCQueue_list_expbackoff_elimination_stat, boost::intrusive::list_base_hook<> )

#define CDSUNIT_TEST_FCQueue \
    CPPUNIT_TEST(FCQueue_list_delay2) \
    CPPUNIT_TEST(FCQueue_list_delay2_elimination) \
    CPPUNIT_TEST(FCQueue_list_delay2_elimination_stat) \
    CPPUNIT_TEST(FCQueue_list_expbackoff_elimination) \
    CPPUNIT_TEST(FCQueue_list_expbackoff_elimination_stat)

// SegmentedQueue
#define CDSUNIT_DECLARE_SegmentedQueue \
    TEST_SEGMENTED( SegmentedQueue_HP_spin ) \
    TEST_SEGMENTED( SegmentedQueue_HP_spin_stat ) \
    TEST_SEGMENTED( SegmentedQueue_HP_mutex ) \
    TEST_SEGMENTED( SegmentedQueue_HP_mutex_stat ) \
    TEST_SEGMENTED( SegmentedQueue_PTB_spin ) \
    TEST_SEGMENTED( SegmentedQueue_PTB_spin_stat ) \
    TEST_SEGMENTED( SegmentedQueue_PTB_mutex ) \
    TEST_SEGMENTED( SegmentedQueue_PTB_mutex_stat )

#define CDSUNIT_TEST_SegmentedQueue \
    CPPUNIT_TEST( SegmentedQueue_HP_spin ) \
    CPPUNIT_TEST( SegmentedQueue_HP_spin_stat ) \
    CPPUNIT_TEST( SegmentedQueue_HP_mutex ) \
    CPPUNIT_TEST( SegmentedQueue_HP_mutex_stat ) \
    CPPUNIT_TEST( SegmentedQueue_PTB_spin ) \
    CPPUNIT_TEST( SegmentedQueue_PTB_spin_stat ) \
    CPPUNIT_TEST( SegmentedQueue_PTB_mutex ) \
    CPPUNIT_TEST( SegmentedQueue_PTB_mutex_stat )


// BoostSList
#define CDSUNIT_DECLARE_BoostSList \
    TEST_BOOST( BoostSList_mutex, boost::intrusive::slist_base_hook<> ) \
    TEST_BOOST( BoostSList_spin, boost::intrusive::slist_base_hook<> )

#define CDSUNIT_TEST_BoostSList \
    CPPUNIT_TEST( BoostSList_mutex ) \
    CPPUNIT_TEST( BoostSList_spin )

#endif // #ifndef __CDSUNIT_INTRUSIVE_QUEUE_DEFS_H
