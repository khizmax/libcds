//$$CDS-header$$

#ifndef CDSUNIT_PQUEUE_PQUEUE_DEFS_H
#define CDSUNIT_PQUEUE_PQUEUE_DEFS_H

// MSPriorityQueue
#define CDSUNIT_DECLARE_MSPriorityQueue \
    TEST_BOUNDED(MSPriorityQueue_static_less)   \
    TEST_BOUNDED(MSPriorityQueue_static_less_stat) \
    TEST_BOUNDED(MSPriorityQueue_static_cmp)    \
    TEST_BOUNDED(MSPriorityQueue_static_mutex)  \
    TEST_BOUNDED(MSPriorityQueue_dyn_less)      \
    TEST_BOUNDED(MSPriorityQueue_dyn_less_stat) \
    TEST_BOUNDED(MSPriorityQueue_dyn_cmp)       \
    TEST_BOUNDED(MSPriorityQueue_dyn_mutex)
#define CDSUNIT_TEST_MSPriorityQueue    \
    CPPUNIT_TEST(MSPriorityQueue_static_less)   \
    CPPUNIT_TEST(MSPriorityQueue_static_less_stat) \
    CPPUNIT_TEST(MSPriorityQueue_static_cmp)    \
    CPPUNIT_TEST(MSPriorityQueue_static_mutex)  \
    CPPUNIT_TEST(MSPriorityQueue_dyn_less)      \
    CPPUNIT_TEST(MSPriorityQueue_dyn_less_stat) \
    CPPUNIT_TEST(MSPriorityQueue_dyn_cmp)       \
    CPPUNIT_TEST(MSPriorityQueue_dyn_mutex)


// EllenBinTree
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#   define CDSUNIT_DECLARE_EllenBinTree_RCU_signal  \
        TEST_CASE(EllenBinTree_RCU_shb_max) \
        TEST_CASE(EllenBinTree_RCU_shb_max_stat) \
        TEST_CASE(EllenBinTree_RCU_shb_min) \
        TEST_CASE(EllenBinTree_RCU_shb_min_stat) \
        TEST_CASE(EllenBinTree_RCU_sht_max) \
        TEST_CASE(EllenBinTree_RCU_sht_max_stat) \
        TEST_CASE(EllenBinTree_RCU_sht_min) \
        TEST_CASE(EllenBinTree_RCU_sht_min_stat)
#   define CDSUNIT_TEST_EllenBinTree_RCU_signal \
        CPPUNIT_TEST(EllenBinTree_RCU_shb_max)  \
        CPPUNIT_TEST(EllenBinTree_RCU_shb_max_stat)  \
        CPPUNIT_TEST(EllenBinTree_RCU_shb_min)  \
        CPPUNIT_TEST(EllenBinTree_RCU_shb_min_stat)  \
        CPPUNIT_TEST(EllenBinTree_RCU_sht_max)  \
        CPPUNIT_TEST(EllenBinTree_RCU_sht_max_stat)  \
        CPPUNIT_TEST(EllenBinTree_RCU_sht_min) \
        CPPUNIT_TEST(EllenBinTree_RCU_sht_min_stat)
#else
#   define CDSUNIT_DECLARE_EllenBinTree_RCU_signal
#   define CDSUNIT_TEST_EllenBinTree_RCU_signal
#endif

#define CDSUNIT_DECLARE_EllenBinTree    \
    TEST_CASE(EllenBinTree_HP_max)          \
    TEST_CASE(EllenBinTree_HP_max_stat)     \
    TEST_CASE(EllenBinTree_HP_min)          \
    TEST_CASE(EllenBinTree_HP_min_stat)     \
    TEST_CASE(EllenBinTree_DHP_max)         \
    TEST_CASE(EllenBinTree_DHP_max_stat)    \
    TEST_CASE(EllenBinTree_DHP_min)         \
    TEST_CASE(EllenBinTree_DHP_min_stat)    \
    TEST_CASE(EllenBinTree_RCU_gpi_max)     \
    TEST_CASE(EllenBinTree_RCU_gpi_max_stat)     \
    TEST_CASE(EllenBinTree_RCU_gpi_min)     \
    TEST_CASE(EllenBinTree_RCU_gpi_min_stat)     \
    TEST_CASE(EllenBinTree_RCU_gpb_max)     \
    TEST_CASE(EllenBinTree_RCU_gpb_max_stat)     \
    TEST_CASE(EllenBinTree_RCU_gpb_min)     \
    TEST_CASE(EllenBinTree_RCU_gpb_min_stat)     \
    TEST_CASE(EllenBinTree_RCU_gpt_max)     \
    TEST_CASE(EllenBinTree_RCU_gpt_max_stat)     \
    TEST_CASE(EllenBinTree_RCU_gpt_min)     \
    TEST_CASE(EllenBinTree_RCU_gpt_min_stat)     \
    CDSUNIT_DECLARE_EllenBinTree_RCU_signal
#define CDSUNIT_TEST_EllenBinTree       \
    CPPUNIT_TEST(EllenBinTree_HP_max)       \
    CPPUNIT_TEST(EllenBinTree_HP_max_stat)  \
    CPPUNIT_TEST(EllenBinTree_HP_min)       \
    CPPUNIT_TEST(EllenBinTree_HP_min_stat)  \
    CPPUNIT_TEST(EllenBinTree_DHP_max)      \
    CPPUNIT_TEST(EllenBinTree_DHP_max_stat) \
    CPPUNIT_TEST(EllenBinTree_DHP_min)      \
    CPPUNIT_TEST(EllenBinTree_DHP_min_stat) \
    /*CPPUNIT_TEST(EllenBinTree_RCU_gpi_max)*/  \
    /*CPPUNIT_TEST(EllenBinTree_RCU_gpi_max_stat)*/  \
    /*CPPUNIT_TEST(EllenBinTree_RCU_gpi_min)*/  \
    /*CPPUNIT_TEST(EllenBinTree_RCU_gpi_min_stat)*/  \
    CPPUNIT_TEST(EllenBinTree_RCU_gpb_max)  \
    CPPUNIT_TEST(EllenBinTree_RCU_gpb_max_stat)  \
    CPPUNIT_TEST(EllenBinTree_RCU_gpb_min)  \
    CPPUNIT_TEST(EllenBinTree_RCU_gpb_min_stat)  \
    CPPUNIT_TEST(EllenBinTree_RCU_gpt_max)  \
    CPPUNIT_TEST(EllenBinTree_RCU_gpt_max_stat)  \
    CPPUNIT_TEST(EllenBinTree_RCU_gpt_min)  \
    CPPUNIT_TEST(EllenBinTree_RCU_gpt_min_stat)  \
    CDSUNIT_TEST_EllenBinTree_RCU_signal


// SkipList
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#   define CDSUNIT_DECLARE_SkipList_RCU_signal  \
    TEST_CASE(SkipList_RCU_shb_max) \
    TEST_CASE(SkipList_RCU_shb_min) \
    TEST_CASE(SkipList_RCU_sht_max) \
    TEST_CASE(SkipList_RCU_sht_min)
#   define CDSUNIT_TEST_SkipList_RCU_signal \
    CPPUNIT_TEST(SkipList_RCU_shb_max)  \
    CPPUNIT_TEST(SkipList_RCU_shb_min)  \
    CPPUNIT_TEST(SkipList_RCU_sht_max)  \
    CPPUNIT_TEST(SkipList_RCU_sht_min)
#else
#   define CDSUNIT_DECLARE_SkipList_RCU_signal
#   define CDSUNIT_TEST_SkipList_RCU_signal
#endif

#define CDSUNIT_DECLARE_SkipList    \
    TEST_CASE(SkipList_HP_max)          \
    TEST_CASE(SkipList_HP_max_stat)     \
    TEST_CASE(SkipList_HP_min)          \
    TEST_CASE(SkipList_HP_min_stat)     \
    TEST_CASE(SkipList_DHP_max)         \
    TEST_CASE(SkipList_DHP_max_stat)    \
    TEST_CASE(SkipList_DHP_min)         \
    TEST_CASE(SkipList_DHP_min_stat)    \
    TEST_CASE(SkipList_RCU_gpi_max)     \
    TEST_CASE(SkipList_RCU_gpi_min)     \
    TEST_CASE(SkipList_RCU_gpb_max)     \
    TEST_CASE(SkipList_RCU_gpb_min)     \
    TEST_CASE(SkipList_RCU_gpt_max)     \
    TEST_CASE(SkipList_RCU_gpt_min)     \
    CDSUNIT_DECLARE_SkipList_RCU_signal
#define CDSUNIT_TEST_SkipList       \
    CPPUNIT_TEST(SkipList_HP_max)       \
    CPPUNIT_TEST(SkipList_HP_max_stat)  \
    CPPUNIT_TEST(SkipList_HP_min)       \
    CPPUNIT_TEST(SkipList_HP_min_stat)  \
    CPPUNIT_TEST(SkipList_DHP_max)      \
    CPPUNIT_TEST(SkipList_DHP_max_stat) \
    CPPUNIT_TEST(SkipList_DHP_min)      \
    CPPUNIT_TEST(SkipList_DHP_min_stat) \
    CPPUNIT_TEST(SkipList_RCU_gpi_max)  \
    CPPUNIT_TEST(SkipList_RCU_gpi_min)  \
    CPPUNIT_TEST(SkipList_RCU_gpb_max)  \
    CPPUNIT_TEST(SkipList_RCU_gpb_min)  \
    CPPUNIT_TEST(SkipList_RCU_gpt_max)  \
    CPPUNIT_TEST(SkipList_RCU_gpt_min)  \
    CDSUNIT_TEST_SkipList_RCU_signal

// FCPriorityQueue
#define CDSUNIT_DECLARE_FCPriorityQueue \
    TEST_CASE(FCPQueue_vector)          \
    TEST_CASE(FCPQueue_vector_stat)     \
    TEST_CASE(FCPQueue_deque)           \
    TEST_CASE(FCPQueue_deque_stat)      \
    TEST_CASE(FCPQueue_boost_deque)          \
    TEST_CASE(FCPQueue_boost_deque_stat)     \
    TEST_CASE(FCPQueue_boost_stable_vector)  \
    TEST_CASE(FCPQueue_boost_stable_vector_stat)

#define CDSUNIT_TEST_FCPriorityQueue \
    CPPUNIT_TEST(FCPQueue_vector)          \
    CPPUNIT_TEST(FCPQueue_vector_stat)     \
    CPPUNIT_TEST(FCPQueue_deque)           \
    CPPUNIT_TEST(FCPQueue_deque_stat)      \
    CPPUNIT_TEST(FCPQueue_boost_deque)          \
    CPPUNIT_TEST(FCPQueue_boost_deque_stat)     \
    CPPUNIT_TEST(FCPQueue_boost_stable_vector)  \
    CPPUNIT_TEST(FCPQueue_boost_stable_vector_stat)

// Std::priority_queue
#define CDSUNIT_DECLARE_StdPQueue       \
    TEST_CASE(StdPQueue_vector_spin)    \
    TEST_CASE(StdPQueue_vector_mutex)   \
    TEST_CASE(StdPQueue_deque_spin)     \
    TEST_CASE(StdPQueue_deque_mutex)
#define CDUNIT_TEST_StdPQueue           \
    CPPUNIT_TEST(StdPQueue_vector_spin) \
    CPPUNIT_TEST(StdPQueue_vector_mutex)\
    CPPUNIT_TEST(StdPQueue_deque_spin)  \
    CPPUNIT_TEST(StdPQueue_deque_mutex)


#endif // #ifndef CDSUNIT_PQUEUE_PQUEUE_DEFS_H
