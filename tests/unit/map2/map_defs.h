//$$CDS-header$$

#define CDSUNIT_DECLARE_StdMap \
    TEST_CASE(tag_StdMap, StdMap_Spin) \
    TEST_CASE(tag_StdMap, StdHashMap_Spin) \
    TEST_CASE(tag_StdMap, StdMap_Mutex) \
    TEST_CASE(tag_StdMap, StdHashMap_Mutex)

#define CDSUNIT_TEST_StdMap \
    CPPUNIT_TEST(StdMap_Spin) \
    CPPUNIT_TEST(StdHashMap_Spin) \
    CPPUNIT_TEST(StdMap_Mutex) \
    CPPUNIT_TEST(StdHashMap_Mutex) \

#define CDSUNIT_DECLARE_StdMap_NoLock \
    TEST_CASE(tag_StdMap, StdMap_NoLock) \
    TEST_CASE(tag_StdMap, StdHashMap_NoLock) \

#define CDSUNIT_TEST_StdMap_NoLock \
    CPPUNIT_TEST(StdMap_NoLock) \
    CPPUNIT_TEST(StdHashMap_NoLock) \


// **************************************************************************************
// MichaelMap

#undef CDSUNIT_DECLARE_MichaelMap_RCU_signal
#undef CDSUNIT_TEST_MichaelMap_RCU_signal
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#   define CDSUNIT_DECLARE_MichaelMap_RCU_signal \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_RCU_SHB_cmp_stdAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_RCU_SHB_less_michaelAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_RCU_SHT_cmp_stdAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_RCU_SHT_less_michaelAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_Lazy_RCU_SHB_cmp_stdAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_Lazy_RCU_SHB_less_michaelAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_Lazy_RCU_SHT_cmp_stdAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_Lazy_RCU_SHT_less_michaelAlloc)

#   define CDSUNIT_TEST_MichaelMap_RCU_signal \
    CPPUNIT_TEST(MichaelMap_RCU_SHB_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelMap_RCU_SHB_less_michaelAlloc) \
    CPPUNIT_TEST(MichaelMap_RCU_SHT_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelMap_RCU_SHT_less_michaelAlloc) \
    CPPUNIT_TEST(MichaelMap_Lazy_RCU_SHB_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelMap_Lazy_RCU_SHB_less_michaelAlloc) \
    CPPUNIT_TEST(MichaelMap_Lazy_RCU_SHT_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelMap_Lazy_RCU_SHT_less_michaelAlloc)

#else
#   define CDSUNIT_DECLARE_MichaelMap_RCU_signal
#   define CDSUNIT_TEST_MichaelMap_RCU_signal
#endif

#undef CDSUNIT_DECLARE_MichaelMap
#define CDSUNIT_DECLARE_MichaelMap  \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_HP_cmp_stdAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_HP_less_michaelAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_DHP_cmp_stdAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_DHP_less_michaelAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_RCU_GPI_cmp_stdAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_RCU_GPI_less_michaelAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_RCU_GPB_cmp_stdAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_RCU_GPB_less_michaelAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_RCU_GPT_cmp_stdAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_RCU_GPT_less_michaelAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_Lazy_HP_cmp_stdAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_Lazy_HP_less_michaelAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_Lazy_DHP_cmp_stdAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_Lazy_DHP_less_michaelAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_Lazy_RCU_GPI_cmp_stdAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_Lazy_RCU_GPI_less_michaelAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_Lazy_RCU_GPB_cmp_stdAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_Lazy_RCU_GPB_less_michaelAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_Lazy_RCU_GPT_cmp_stdAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_Lazy_RCU_GPT_less_michaelAlloc)\
    CDSUNIT_DECLARE_MichaelMap_RCU_signal

#undef  CDSUNIT_TEST_MichaelMap
#define CDSUNIT_TEST_MichaelMap  \
    CPPUNIT_TEST(MichaelMap_HP_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelMap_HP_less_michaelAlloc) \
    CPPUNIT_TEST(MichaelMap_DHP_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelMap_DHP_less_michaelAlloc) \
    CPPUNIT_TEST(MichaelMap_RCU_GPI_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelMap_RCU_GPI_less_michaelAlloc) \
    CPPUNIT_TEST(MichaelMap_RCU_GPB_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelMap_RCU_GPB_less_michaelAlloc) \
    CPPUNIT_TEST(MichaelMap_RCU_GPT_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelMap_RCU_GPT_less_michaelAlloc) \
    CPPUNIT_TEST(MichaelMap_Lazy_HP_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelMap_Lazy_HP_less_michaelAlloc) \
    CPPUNIT_TEST(MichaelMap_Lazy_DHP_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelMap_Lazy_DHP_less_michaelAlloc) \
    CPPUNIT_TEST(MichaelMap_Lazy_RCU_GPI_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelMap_Lazy_RCU_GPI_less_michaelAlloc) \
    CPPUNIT_TEST(MichaelMap_Lazy_RCU_GPB_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelMap_Lazy_RCU_GPB_less_michaelAlloc) \
    CPPUNIT_TEST(MichaelMap_Lazy_RCU_GPT_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelMap_Lazy_RCU_GPT_less_michaelAlloc)\
    CDSUNIT_TEST_MichaelMap_RCU_signal

#undef  CDSUNIT_DECLARE_MichaelMap_nogc
#define CDSUNIT_DECLARE_MichaelMap_nogc  \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_NOGC_cmp_stdAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_NOGC_less_michaelAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_Lazy_NOGC_cmp_stdAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_Lazy_NOGC_unord_stdAlloc) \
    TEST_CASE(tag_MichaelHashMap, MichaelMap_Lazy_NOGC_less_michaelAlloc)

#undef  CDSUNIT_TEST_MichaelMap_nogc
#define CDSUNIT_TEST_MichaelMap_nogc  \
    CPPUNIT_TEST(MichaelMap_NOGC_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelMap_NOGC_less_michaelAlloc) \
    CPPUNIT_TEST(MichaelMap_Lazy_NOGC_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelMap_Lazy_NOGC_unord_stdAlloc) \
    CPPUNIT_TEST(MichaelMap_Lazy_NOGC_less_michaelAlloc) \


// **************************************************************************************
// SplitListMap

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#   define CDSUNIT_DECLARE_SplitList_RCU_signal  \
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_SHB_dyn_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_SHB_dyn_cmp_stat)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_SHB_st_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_SHB_dyn_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_SHB_st_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_SHB_st_less_stat)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_SHT_dyn_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_SHT_dyn_cmp_stat)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_SHT_st_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_SHT_dyn_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_SHT_st_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_SHT_st_less_stat)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_SHB_dyn_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_SHB_dyn_cmp_stat)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_SHB_st_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_SHB_dyn_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_SHB_st_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_SHB_st_less_stat)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_SHT_dyn_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_SHT_dyn_cmp_stat)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_SHT_st_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_SHT_dyn_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_SHT_st_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_SHT_st_less_stat)

#   define CDSUNIT_TEST_SplitList_RCU_signal \
    CPPUNIT_TEST(SplitList_Michael_RCU_SHB_dyn_cmp)\
    CPPUNIT_TEST(SplitList_Michael_RCU_SHB_dyn_cmp_stat)\
    CPPUNIT_TEST(SplitList_Michael_RCU_SHB_st_cmp)\
    CPPUNIT_TEST(SplitList_Michael_RCU_SHB_dyn_less)\
    CPPUNIT_TEST(SplitList_Michael_RCU_SHB_st_less)\
    CPPUNIT_TEST(SplitList_Michael_RCU_SHB_st_less_stat)\
    CPPUNIT_TEST(SplitList_Michael_RCU_SHT_dyn_cmp)\
    CPPUNIT_TEST(SplitList_Michael_RCU_SHT_dyn_cmp_stat)\
    CPPUNIT_TEST(SplitList_Michael_RCU_SHT_st_cmp)\
    CPPUNIT_TEST(SplitList_Michael_RCU_SHT_dyn_less)\
    CPPUNIT_TEST(SplitList_Michael_RCU_SHT_st_less)\
    CPPUNIT_TEST(SplitList_Michael_RCU_SHT_st_less_stat)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_SHB_dyn_cmp)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_SHB_dyn_cmp_stat)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_SHB_st_cmp)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_SHB_dyn_less)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_SHB_st_less)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_SHB_st_less_stat)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_SHT_dyn_cmp)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_SHT_dyn_cmp_stat)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_SHT_st_cmp)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_SHT_dyn_less)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_SHT_st_less)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_SHT_st_less_stat)

#else
#   define CDSUNIT_DECLARE_SplitList_RCU_signal
#   define CDSUNIT_TEST_SplitList_RCU_signal
#endif

#define CDSUNIT_DECLARE_SplitList \
    TEST_CASE(tag_SplitListMap, SplitList_Michael_HP_dyn_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_HP_dyn_cmp_stat)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_HP_st_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_HP_dyn_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_HP_st_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_HP_st_less_stat)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_DHP_dyn_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_DHP_dyn_cmp_stat)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_DHP_st_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_DHP_dyn_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_DHP_st_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_DHP_st_less_stat)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_GPI_dyn_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_GPI_dyn_cmp_stat)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_GPI_st_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_GPI_dyn_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_GPI_st_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_GPI_st_less_stat)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_GPB_dyn_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_GPB_dyn_cmp_stat)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_GPB_st_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_GPB_dyn_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_GPB_st_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_GPB_st_less_stat)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_GPT_dyn_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_GPT_dyn_cmp_stat)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_GPT_st_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_GPT_dyn_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_GPT_st_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_RCU_GPT_st_less_stat)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_HP_dyn_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_HP_dyn_cmp_stat)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_HP_st_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_HP_dyn_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_HP_st_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_HP_st_less_stat)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_DHP_dyn_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_DHP_dyn_cmp_stat)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_DHP_st_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_DHP_dyn_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_DHP_st_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_DHP_st_less_stat)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_GPI_dyn_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_GPI_dyn_cmp_stat)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_GPI_st_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_GPI_dyn_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_GPI_st_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_GPI_st_less_stat)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_GPB_dyn_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_GPB_dyn_cmp_stat)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_GPB_st_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_GPB_dyn_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_GPB_st_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_GPB_st_less_stat)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_GPT_dyn_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_GPT_dyn_cmp_stat)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_GPT_st_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_GPT_dyn_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_GPT_st_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_RCU_GPT_st_less_stat)\
    CDSUNIT_DECLARE_SplitList_RCU_signal

#define CDSUNIT_TEST_SplitList  \
    CPPUNIT_TEST(SplitList_Michael_HP_dyn_cmp)\
    CPPUNIT_TEST(SplitList_Michael_HP_dyn_cmp_stat)\
    CPPUNIT_TEST(SplitList_Michael_HP_st_cmp)\
    CPPUNIT_TEST(SplitList_Michael_HP_dyn_less)\
    CPPUNIT_TEST(SplitList_Michael_HP_st_less)\
    CPPUNIT_TEST(SplitList_Michael_HP_st_less_stat)\
    CPPUNIT_TEST(SplitList_Michael_DHP_dyn_cmp)\
    CPPUNIT_TEST(SplitList_Michael_DHP_dyn_cmp_stat)\
    CPPUNIT_TEST(SplitList_Michael_DHP_st_cmp)\
    CPPUNIT_TEST(SplitList_Michael_DHP_dyn_less)\
    CPPUNIT_TEST(SplitList_Michael_DHP_st_less)\
    CPPUNIT_TEST(SplitList_Michael_DHP_st_less_stat)\
    CPPUNIT_TEST(SplitList_Michael_RCU_GPI_dyn_cmp)\
    CPPUNIT_TEST(SplitList_Michael_RCU_GPI_dyn_cmp_stat)\
    CPPUNIT_TEST(SplitList_Michael_RCU_GPI_st_cmp)\
    CPPUNIT_TEST(SplitList_Michael_RCU_GPI_dyn_less)\
    CPPUNIT_TEST(SplitList_Michael_RCU_GPI_st_less)\
    CPPUNIT_TEST(SplitList_Michael_RCU_GPI_st_less_stat)\
    CPPUNIT_TEST(SplitList_Michael_RCU_GPB_dyn_cmp)\
    CPPUNIT_TEST(SplitList_Michael_RCU_GPB_dyn_cmp_stat)\
    CPPUNIT_TEST(SplitList_Michael_RCU_GPB_st_cmp)\
    CPPUNIT_TEST(SplitList_Michael_RCU_GPB_dyn_less)\
    CPPUNIT_TEST(SplitList_Michael_RCU_GPB_st_less)\
    CPPUNIT_TEST(SplitList_Michael_RCU_GPB_st_less_stat)\
    CPPUNIT_TEST(SplitList_Michael_RCU_GPT_dyn_cmp)\
    CPPUNIT_TEST(SplitList_Michael_RCU_GPT_dyn_cmp_stat)\
    CPPUNIT_TEST(SplitList_Michael_RCU_GPT_st_cmp)\
    CPPUNIT_TEST(SplitList_Michael_RCU_GPT_dyn_less)\
    CPPUNIT_TEST(SplitList_Michael_RCU_GPT_st_less)\
    CPPUNIT_TEST(SplitList_Michael_RCU_GPT_st_less_stat)\
    CPPUNIT_TEST(SplitList_Lazy_HP_dyn_cmp)\
    CPPUNIT_TEST(SplitList_Lazy_HP_dyn_cmp_stat)\
    CPPUNIT_TEST(SplitList_Lazy_HP_st_cmp)\
    CPPUNIT_TEST(SplitList_Lazy_HP_dyn_less)\
    CPPUNIT_TEST(SplitList_Lazy_HP_st_less)\
    CPPUNIT_TEST(SplitList_Lazy_HP_st_less_stat)\
    CPPUNIT_TEST(SplitList_Lazy_DHP_dyn_cmp)\
    CPPUNIT_TEST(SplitList_Lazy_DHP_dyn_cmp_stat)\
    CPPUNIT_TEST(SplitList_Lazy_DHP_st_cmp)\
    CPPUNIT_TEST(SplitList_Lazy_DHP_dyn_less)\
    CPPUNIT_TEST(SplitList_Lazy_DHP_st_less)\
    CPPUNIT_TEST(SplitList_Lazy_DHP_st_less_stat)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_GPI_dyn_cmp)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_GPI_dyn_cmp_stat)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_GPI_st_cmp)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_GPI_dyn_less)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_GPI_st_less)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_GPI_st_less_stat)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_GPB_dyn_cmp)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_GPB_dyn_cmp_stat)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_GPB_st_cmp)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_GPB_dyn_less)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_GPB_st_less)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_GPB_st_less_stat)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_GPT_dyn_cmp)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_GPT_dyn_cmp_stat)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_GPT_st_cmp)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_GPT_dyn_less)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_GPT_st_less)\
    CPPUNIT_TEST(SplitList_Lazy_RCU_GPT_st_less_stat)\
    CDSUNIT_TEST_SplitList_RCU_signal

#define CDSUNIT_DECLARE_SplitList_nogc  \
    TEST_CASE(tag_SplitListMap, SplitList_Michael_NOGC_dyn_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_NOGC_st_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_NOGC_dyn_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Michael_NOGC_st_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_NOGC_dyn_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_NOGC_st_cmp)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_NOGC_dyn_less)\
    TEST_CASE(tag_SplitListMap, SplitList_Lazy_NOGC_st_less)

#define CDSUNIT_TEST_SplitList_nogc  \
    CPPUNIT_TEST(SplitList_Michael_NOGC_dyn_cmp)\
    CPPUNIT_TEST(SplitList_Michael_NOGC_st_cmp)\
    CPPUNIT_TEST(SplitList_Michael_NOGC_dyn_less)\
    CPPUNIT_TEST(SplitList_Michael_NOGC_st_less)\
    CPPUNIT_TEST(SplitList_Lazy_NOGC_dyn_cmp)\
    CPPUNIT_TEST(SplitList_Lazy_NOGC_st_cmp)\
    CPPUNIT_TEST(SplitList_Lazy_NOGC_dyn_less)\
    CPPUNIT_TEST(SplitList_Lazy_NOGC_st_less)


// **************************************************************************************
// SkipListMap

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#   define CDSUNIT_DECLARE_SkipListMap_RCU_signal \
    TEST_CASE(tag_SkipListMap, SkipListMap_rcu_shb_less_pascal)\
    TEST_CASE(tag_SkipListMap, SkipListMap_rcu_shb_cmp_pascal_stat)\
    TEST_CASE(tag_SkipListMap, SkipListMap_rcu_shb_less_xorshift)\
    TEST_CASE(tag_SkipListMap, SkipListMap_rcu_shb_cmp_xorshift_stat)\
    TEST_CASE(tag_SkipListMap, SkipListMap_rcu_sht_less_pascal)\
    TEST_CASE(tag_SkipListMap, SkipListMap_rcu_sht_cmp_pascal_stat)\
    TEST_CASE(tag_SkipListMap, SkipListMap_rcu_sht_less_xorshift)\
    TEST_CASE(tag_SkipListMap, SkipListMap_rcu_sht_cmp_xorshift_stat)

#   define CDSUNIT_TEST_SkipListMap_RCU_signal \
    CPPUNIT_TEST(SkipListMap_rcu_shb_less_pascal)\
    CPPUNIT_TEST(SkipListMap_rcu_shb_cmp_pascal_stat)\
    CPPUNIT_TEST(SkipListMap_rcu_shb_less_xorshift)\
    CPPUNIT_TEST(SkipListMap_rcu_shb_cmp_xorshift_stat)\
    CPPUNIT_TEST(SkipListMap_rcu_sht_less_pascal)\
    CPPUNIT_TEST(SkipListMap_rcu_sht_cmp_pascal_stat)\
    CPPUNIT_TEST(SkipListMap_rcu_sht_less_xorshift)\
    CPPUNIT_TEST(SkipListMap_rcu_sht_cmp_xorshift_stat)

#else
#   define CDSUNIT_DECLARE_SkipListMap_RCU_signal
#   define CDSUNIT_TEST_SkipListMap_RCU_signal
#endif

#define CDSUNIT_DECLARE_SkipListMap \
    TEST_CASE(tag_SkipListMap, SkipListMap_hp_less_pascal)\
    TEST_CASE(tag_SkipListMap, SkipListMap_hp_cmp_pascal_stat)\
    TEST_CASE(tag_SkipListMap, SkipListMap_hp_less_xorshift)\
    TEST_CASE(tag_SkipListMap, SkipListMap_hp_cmp_xorshift_stat)\
    TEST_CASE(tag_SkipListMap, SkipListMap_dhp_less_pascal)\
    TEST_CASE(tag_SkipListMap, SkipListMap_dhp_cmp_pascal_stat)\
    TEST_CASE(tag_SkipListMap, SkipListMap_dhp_less_xorshift)\
    TEST_CASE(tag_SkipListMap, SkipListMap_dhp_cmp_xorshift_stat)\
    TEST_CASE(tag_SkipListMap, SkipListMap_rcu_gpi_less_pascal)\
    TEST_CASE(tag_SkipListMap, SkipListMap_rcu_gpi_cmp_pascal_stat)\
    TEST_CASE(tag_SkipListMap, SkipListMap_rcu_gpi_less_xorshift)\
    TEST_CASE(tag_SkipListMap, SkipListMap_rcu_gpi_cmp_xorshift_stat)\
    TEST_CASE(tag_SkipListMap, SkipListMap_rcu_gpb_less_pascal)\
    TEST_CASE(tag_SkipListMap, SkipListMap_rcu_gpb_cmp_pascal_stat)\
    TEST_CASE(tag_SkipListMap, SkipListMap_rcu_gpb_less_xorshift)\
    TEST_CASE(tag_SkipListMap, SkipListMap_rcu_gpb_cmp_xorshift_stat)\
    TEST_CASE(tag_SkipListMap, SkipListMap_rcu_gpt_less_pascal)\
    TEST_CASE(tag_SkipListMap, SkipListMap_rcu_gpt_cmp_pascal_stat)\
    TEST_CASE(tag_SkipListMap, SkipListMap_rcu_gpt_less_xorshift)\
    TEST_CASE(tag_SkipListMap, SkipListMap_rcu_gpt_cmp_xorshift_stat)\
    CDSUNIT_DECLARE_SkipListMap_RCU_signal

#define CDSUNIT_TEST_SkipListMap \
    CPPUNIT_TEST(SkipListMap_hp_less_pascal)\
    CPPUNIT_TEST(SkipListMap_hp_cmp_pascal_stat)\
    CPPUNIT_TEST(SkipListMap_hp_less_xorshift)\
    CPPUNIT_TEST(SkipListMap_hp_cmp_xorshift_stat)\
    CPPUNIT_TEST(SkipListMap_dhp_less_pascal)\
    CPPUNIT_TEST(SkipListMap_dhp_cmp_pascal_stat)\
    CPPUNIT_TEST(SkipListMap_dhp_less_xorshift)\
    CPPUNIT_TEST(SkipListMap_dhp_cmp_xorshift_stat)\
    CPPUNIT_TEST(SkipListMap_rcu_gpi_less_pascal)\
    CPPUNIT_TEST(SkipListMap_rcu_gpi_cmp_pascal_stat)\
    CPPUNIT_TEST(SkipListMap_rcu_gpi_less_xorshift)\
    CPPUNIT_TEST(SkipListMap_rcu_gpi_cmp_xorshift_stat)\
    CPPUNIT_TEST(SkipListMap_rcu_gpb_less_pascal)\
    CPPUNIT_TEST(SkipListMap_rcu_gpb_cmp_pascal_stat)\
    CPPUNIT_TEST(SkipListMap_rcu_gpb_less_xorshift)\
    CPPUNIT_TEST(SkipListMap_rcu_gpb_cmp_xorshift_stat)\
    CPPUNIT_TEST(SkipListMap_rcu_gpt_less_pascal)\
    CPPUNIT_TEST(SkipListMap_rcu_gpt_cmp_pascal_stat)\
    CPPUNIT_TEST(SkipListMap_rcu_gpt_less_xorshift)\
    CPPUNIT_TEST(SkipListMap_rcu_gpt_cmp_xorshift_stat)\
    CDSUNIT_TEST_SkipListMap_RCU_signal

#define CDSUNIT_DECLARE_SkipListMap_nogc \
    TEST_CASE(tag_SkipListMap, SkipListMap_nogc_less_pascal)\
    TEST_CASE(tag_SkipListMap, SkipListMap_nogc_cmp_pascal_stat)\
    TEST_CASE(tag_SkipListMap, SkipListMap_nogc_less_xorshift)\
    TEST_CASE(tag_SkipListMap, SkipListMap_nogc_cmp_xorshift_stat)

#define CDSUNIT_TEST_SkipListMap_nogc \
    CPPUNIT_TEST(SkipListMap_nogc_less_pascal)\
    CPPUNIT_TEST(SkipListMap_nogc_cmp_pascal_stat)\
    CPPUNIT_TEST(SkipListMap_nogc_less_xorshift)\
    CPPUNIT_TEST(SkipListMap_nogc_cmp_xorshift_stat)


// **************************************************************************************
// EllenBinTreeMap

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#   define CDSUNIT_DECLARE_EllenBinTreeMap_RCU_signal \
    TEST_CASE(tag_EllenBinTreeMap, EllenBinTreeMap_rcu_shb)\
    TEST_CASE(tag_EllenBinTreeMap, EllenBinTreeMap_rcu_shb_stat)\
    TEST_CASE(tag_EllenBinTreeMap, EllenBinTreeMap_rcu_sht)\
    TEST_CASE(tag_EllenBinTreeMap, EllenBinTreeMap_rcu_sht_stat)

#   define CDSUNIT_TEST_EllenBinTreeMap_RCU_signal \
    CPPUNIT_TEST(EllenBinTreeMap_rcu_shb)\
    CPPUNIT_TEST(EllenBinTreeMap_rcu_shb_stat)\
    CPPUNIT_TEST(EllenBinTreeMap_rcu_sht)\
    CPPUNIT_TEST(EllenBinTreeMap_rcu_sht_stat)
#else
#   define CDSUNIT_DECLARE_EllenBinTreeMap_RCU_signal
#   define CDSUNIT_TEST_EllenBinTreeMap_RCU_signal
#endif

#define CDSUNIT_DECLARE_EllenBinTreeMap \
    TEST_CASE(tag_EllenBinTreeMap, EllenBinTreeMap_hp)\
    TEST_CASE(tag_EllenBinTreeMap, EllenBinTreeMap_hp_yield)\
    TEST_CASE(tag_EllenBinTreeMap, EllenBinTreeMap_hp_stat)\
    TEST_CASE(tag_EllenBinTreeMap, EllenBinTreeMap_dhp)\
    TEST_CASE(tag_EllenBinTreeMap, EllenBinTreeMap_dhp_yield)\
    TEST_CASE(tag_EllenBinTreeMap, EllenBinTreeMap_dhp_stat)\
    TEST_CASE(tag_EllenBinTreeMap, EllenBinTreeMap_rcu_gpi)\
    TEST_CASE(tag_EllenBinTreeMap, EllenBinTreeMap_rcu_gpi_stat)\
    TEST_CASE(tag_EllenBinTreeMap, EllenBinTreeMap_rcu_gpb)\
    TEST_CASE(tag_EllenBinTreeMap, EllenBinTreeMap_rcu_gpb_yield)\
    TEST_CASE(tag_EllenBinTreeMap, EllenBinTreeMap_rcu_gpb_stat)\
    TEST_CASE(tag_EllenBinTreeMap, EllenBinTreeMap_rcu_gpt)\
    TEST_CASE(tag_EllenBinTreeMap, EllenBinTreeMap_rcu_gpt_stat)\
    CDSUNIT_DECLARE_EllenBinTreeMap_RCU_signal

#define CDSUNIT_TEST_EllenBinTreeMap \
    CPPUNIT_TEST(EllenBinTreeMap_hp)\
    CPPUNIT_TEST(EllenBinTreeMap_hp_yield)\
    CPPUNIT_TEST(EllenBinTreeMap_hp_stat)\
    CPPUNIT_TEST(EllenBinTreeMap_dhp)\
    CPPUNIT_TEST(EllenBinTreeMap_dhp_yield)\
    CPPUNIT_TEST(EllenBinTreeMap_dhp_stat)\
    CPPUNIT_TEST(EllenBinTreeMap_rcu_gpi)\
    CPPUNIT_TEST(EllenBinTreeMap_rcu_gpi_stat)\
    CPPUNIT_TEST(EllenBinTreeMap_rcu_gpb)\
    CPPUNIT_TEST(EllenBinTreeMap_rcu_gpb_yield)\
    CPPUNIT_TEST(EllenBinTreeMap_rcu_gpb_stat)\
    CPPUNIT_TEST(EllenBinTreeMap_rcu_gpt)\
    CPPUNIT_TEST(EllenBinTreeMap_rcu_gpt_stat)\
    CDSUNIT_TEST_EllenBinTreeMap_RCU_signal


// **************************************************************************************
// BronsonAVLTreeMap

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#   define CDSUNIT_DECLARE_BronsonAVLTreeMap_RCU_signal \
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_shb_less) \
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_sht_less) \
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_shb_cmp_stat) \
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_sht_cmp_stat) \
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_shb_less_pool_simple) \
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_sht_less_pool_simple) \
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_shb_less_pool_simple_stat) \
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_sht_less_pool_simple_stat) \
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_shb_less_pool_lazy) \
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_sht_less_pool_lazy) \
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_shb_less_pool_lazy_stat) \
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_sht_less_pool_lazy_stat) \
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_shb_less_pool_bounded) \
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_sht_less_pool_bounded) \
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_shb_less_pool_bounded_stat) \
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_sht_less_pool_bounded_stat) \

#   define CDSUNIT_TEST_BronsonAVLTreeMap_RCU_signal \
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_shb_less) \
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_sht_less) \
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_shb_cmp_stat) \
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_sht_cmp_stat) \
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_shb_less_pool_simple) \
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_shb_less_pool_simple_stat) \
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_sht_less_pool_simple) \
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_sht_less_pool_simple_stat) \
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_shb_less_pool_lazy) \
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_shb_less_pool_lazy_stat) \
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_sht_less_pool_lazy) \
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_sht_less_pool_lazy_stat) \
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_shb_less_pool_bounded) \
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_shb_less_pool_bounded_stat) \
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_sht_less_pool_bounded) \
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_sht_less_pool_bounded_stat) \

#else
#   define CDSUNIT_DECLARE_BronsonAVLTreeMap_RCU_signal
#   define CDSUNIT_TEST_BronsonAVLTreeMap_RCU_signal
#endif

#define CDSUNIT_DECLARE_BronsonAVLTreeMap \
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_gpi_less)\
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_gpb_less)\
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_gpt_less)\
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_gpi_cmp_stat)\
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_gpb_cmp_stat)\
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_gpt_cmp_stat)\
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_gpi_less_pool_simple)\
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_gpb_less_pool_simple)\
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_gpt_less_pool_simple)\
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_gpi_less_pool_simple_stat)\
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_gpb_less_pool_simple_stat)\
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_gpt_less_pool_simple_stat)\
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_gpi_less_pool_lazy)\
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_gpb_less_pool_lazy)\
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_gpt_less_pool_lazy)\
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_gpi_less_pool_lazy_stat)\
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_gpb_less_pool_lazy_stat)\
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_gpt_less_pool_lazy_stat)\
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_gpi_less_pool_bounded)\
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_gpb_less_pool_bounded)\
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_gpt_less_pool_bounded)\
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_gpi_less_pool_bounded_stat)\
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_gpb_less_pool_bounded_stat)\
    TEST_CASE(tag_BronsonAVLTreeMap, BronsonAVLTreeMap_rcu_gpt_less_pool_bounded_stat)\
    CDSUNIT_DECLARE_BronsonAVLTreeMap_RCU_signal

#define CDSUNIT_TEST_BronsonAVLTreeMap \
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_gpi_less)\
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_gpi_cmp_stat)\
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_gpb_less)\
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_gpb_cmp_stat)\
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_gpt_less)\
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_gpt_cmp_stat)\
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_gpi_less_pool_simple)\
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_gpi_less_pool_simple_stat)\
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_gpb_less_pool_simple)\
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_gpb_less_pool_simple_stat)\
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_gpt_less_pool_simple)\
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_gpt_less_pool_simple_stat)\
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_gpi_less_pool_lazy)\
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_gpi_less_pool_lazy_stat)\
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_gpb_less_pool_lazy)\
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_gpb_less_pool_lazy_stat)\
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_gpt_less_pool_lazy)\
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_gpt_less_pool_lazy_stat)\
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_gpi_less_pool_bounded)\
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_gpi_less_pool_bounded_stat)\
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_gpb_less_pool_bounded)\
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_gpb_less_pool_bounded_stat)\
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_gpt_less_pool_bounded)\
    CPPUNIT_TEST(BronsonAVLTreeMap_rcu_gpt_less_pool_bounded_stat)\
    CDSUNIT_TEST_BronsonAVLTreeMap_RCU_signal


// **************************************************************************************
// StripedMap

#define CDSUNIT_DECLARE_StripedMap_common \
    TEST_CASE(tag_StripedMap, StripedMap_list) \
    TEST_CASE(tag_StripedMap, StripedMap_map) \
    TEST_CASE(tag_StripedMap, StripedMap_hashmap) \
    TEST_CASE(tag_StripedMap, StripedMap_boost_unordered_map)

#define CDSUNIT_TEST_StripedMap_common \
    CPPUNIT_TEST(StripedMap_list) \
    CPPUNIT_TEST(StripedMap_map) \
    CPPUNIT_TEST(StripedMap_hashmap) \
    CPPUNIT_TEST(StripedMap_boost_unordered_map)

#if BOOST_VERSION >= 104800
#   define CDSUNIT_DECLARE_StripedMap_boost_container \
        TEST_CASE(tag_StripedMap, StripedMap_boost_list) \
        TEST_CASE(tag_StripedMap, StripedMap_slist) \
        TEST_CASE(tag_StripedMap, StripedMap_boost_map)

#   define CDSUNIT_TEST_StripedMap_boost_container \
        CPPUNIT_TEST(StripedMap_boost_list) \
        CPPUNIT_TEST(StripedMap_slist) \
        CPPUNIT_TEST(StripedMap_boost_map)
#else
#   define CDSUNIT_DECLARE_StripedMap_boost_container
#   define CDSUNIT_TEST_StripedMap_boost_container
#endif

#if BOOST_VERSION >= 104800 && defined(CDS_UNIT_MAP_TYPES_ENABLE_BOOST_FLAT_CONTAINERS)
#   define CDSUNIT_DECLARE_StripedMap_boost_flat_container \
        TEST_CASE(tag_StripedMap, StripedMap_boost_flat_map)

#   define CDSUNIT_TEST_StripedMap_boost_flat_container \
        CPPUNIT_TEST(StripedMap_boost_flat_map)
#else
#   define CDSUNIT_DECLARE_StripedMap_boost_flat_container
#   define CDSUNIT_TEST_StripedMap_boost_flat_container
#endif

#define CDSUNIT_DECLARE_StripedMap \
    CDSUNIT_DECLARE_StripedMap_common \
    CDSUNIT_DECLARE_StripedMap_boost_container \
    CDSUNIT_DECLARE_StripedMap_boost_flat_container

#define CDSUNIT_TEST_StripedMap \
    CDSUNIT_TEST_StripedMap_common \
    CDSUNIT_TEST_StripedMap_boost_container \
    CDSUNIT_TEST_StripedMap_boost_flat_container


// **************************************************************************************
// RefinableMap

#define CDSUNIT_DECLARE_RefinableMap_common \
    TEST_CASE(tag_StripedMap, RefinableMap_list) \
    TEST_CASE(tag_StripedMap, RefinableMap_map) \
    TEST_CASE(tag_StripedMap, RefinableMap_hashmap) \
    TEST_CASE(tag_StripedMap, RefinableMap_boost_unordered_map)

#define CDSUNIT_TEST_RefinableMap_common \
    CPPUNIT_TEST(RefinableMap_list) \
    CPPUNIT_TEST(RefinableMap_map) \
    CPPUNIT_TEST(RefinableMap_hashmap) \
    CPPUNIT_TEST(RefinableMap_boost_unordered_map)

#if BOOST_VERSION >= 104800
#   define CDSUNIT_DECLARE_RefinableMap_boost_container \
        TEST_CASE(tag_StripedMap, RefinableMap_boost_list) \
        TEST_CASE(tag_StripedMap, RefinableMap_slist) \
        TEST_CASE(tag_StripedMap, RefinableMap_boost_map)

#   define CDSUNIT_TEST_RefinableMap_boost_container \
        CPPUNIT_TEST(RefinableMap_boost_list) \
        CPPUNIT_TEST(RefinableMap_slist) \
        CPPUNIT_TEST(RefinableMap_boost_map)
#else
#   define CDSUNIT_DECLARE_RefinableMap_boost_container
#   define CDSUNIT_TEST_RefinableMap_boost_container
#endif

#if BOOST_VERSION >= 104800 && defined(CDS_UNIT_MAP_TYPES_ENABLE_BOOST_FLAT_CONTAINERS)
#   define CDSUNIT_DECLARE_RefinableMap_boost_flat_container \
        TEST_CASE(tag_StripedMap, RefinableMap_boost_flat_map)

#   define CDSUNIT_TEST_RefinableMap_boost_flat_container \
        CPPUNIT_TEST(RefinableMap_boost_flat_map)
#else
#   define CDSUNIT_DECLARE_RefinableMap_boost_flat_container
#   define CDSUNIT_TEST_RefinableMap_boost_flat_container
#endif

#define CDSUNIT_DECLARE_RefinableMap \
    CDSUNIT_DECLARE_RefinableMap_common \
    CDSUNIT_DECLARE_RefinableMap_boost_container \
    CDSUNIT_DECLARE_RefinableMap_boost_flat_container

#define CDSUNIT_TEST_RefinableMap \
    CDSUNIT_TEST_RefinableMap_common \
    CDSUNIT_TEST_RefinableMap_boost_container \
    CDSUNIT_TEST_RefinableMap_boost_flat_container


// **************************************************************************************
// CuckooMap

#define CDSUNIT_DECLARE_CuckooMap \
    TEST_CASE(tag_CuckooMap, CuckooStripedMap_list_unord)\
    TEST_CASE(tag_CuckooMap, CuckooStripedMap_list_ord)\
    TEST_CASE(tag_CuckooMap, CuckooStripedMap_vector_unord)\
    TEST_CASE(tag_CuckooMap, CuckooStripedMap_vector_ord)\
    TEST_CASE(tag_CuckooMap, CuckooStripedMap_list_unord_stat)\
    TEST_CASE(tag_CuckooMap, CuckooStripedMap_list_ord_stat)\
    TEST_CASE(tag_CuckooMap, CuckooStripedMap_vector_unord_stat)\
    TEST_CASE(tag_CuckooMap, CuckooStripedMap_vector_ord_stat)\
    TEST_CASE(tag_CuckooMap, CuckooRefinableMap_list_unord)\
    TEST_CASE(tag_CuckooMap, CuckooRefinableMap_list_ord)\
    TEST_CASE(tag_CuckooMap, CuckooRefinableMap_vector_unord)\
    TEST_CASE(tag_CuckooMap, CuckooRefinableMap_vector_ord) \
    TEST_CASE(tag_CuckooMap, CuckooRefinableMap_list_unord_stat)\
    TEST_CASE(tag_CuckooMap, CuckooRefinableMap_list_ord_stat)\
    TEST_CASE(tag_CuckooMap, CuckooRefinableMap_vector_unord_stat)\
    TEST_CASE(tag_CuckooMap, CuckooRefinableMap_vector_ord_stat) \
    TEST_CASE(tag_CuckooMap, CuckooStripedMap_list_unord_storehash)\
    TEST_CASE(tag_CuckooMap, CuckooStripedMap_list_ord_storehash)\
    TEST_CASE(tag_CuckooMap, CuckooStripedMap_vector_unord_storehash)\
    TEST_CASE(tag_CuckooMap, CuckooStripedMap_vector_ord_storehash)\
    TEST_CASE(tag_CuckooMap, CuckooRefinableMap_list_unord_storehash)\
    TEST_CASE(tag_CuckooMap, CuckooRefinableMap_list_ord_storehash)\
    TEST_CASE(tag_CuckooMap, CuckooRefinableMap_vector_unord_storehash)\
    TEST_CASE(tag_CuckooMap, CuckooRefinableMap_vector_ord_storehash)

#define CDSUNIT_TEST_CuckooMap \
    CPPUNIT_TEST(CuckooStripedMap_list_unord)\
    CPPUNIT_TEST(CuckooStripedMap_list_unord_stat)\
    CPPUNIT_TEST(CuckooStripedMap_list_unord_storehash)\
    CPPUNIT_TEST(CuckooStripedMap_list_ord)\
    CPPUNIT_TEST(CuckooStripedMap_list_ord_stat)\
    CPPUNIT_TEST(CuckooStripedMap_list_ord_storehash)\
    CPPUNIT_TEST(CuckooStripedMap_vector_unord)\
    CPPUNIT_TEST(CuckooStripedMap_vector_unord_stat)\
    CPPUNIT_TEST(CuckooStripedMap_vector_unord_storehash)\
    CPPUNIT_TEST(CuckooStripedMap_vector_ord)\
    CPPUNIT_TEST(CuckooStripedMap_vector_ord_stat)\
    CPPUNIT_TEST(CuckooStripedMap_vector_ord_storehash)\
    CPPUNIT_TEST(CuckooRefinableMap_list_unord)\
    CPPUNIT_TEST(CuckooRefinableMap_list_unord_stat)\
    CPPUNIT_TEST(CuckooRefinableMap_list_unord_storehash)\
    CPPUNIT_TEST(CuckooRefinableMap_list_ord)\
    CPPUNIT_TEST(CuckooRefinableMap_list_ord_stat)\
    CPPUNIT_TEST(CuckooRefinableMap_list_ord_storehash)\
    CPPUNIT_TEST(CuckooRefinableMap_vector_unord)\
    CPPUNIT_TEST(CuckooRefinableMap_vector_unord_stat)\
    CPPUNIT_TEST(CuckooRefinableMap_vector_unord_storehash)\
    CPPUNIT_TEST(CuckooRefinableMap_vector_ord)\
    CPPUNIT_TEST(CuckooRefinableMap_vector_ord_stat)\
    CPPUNIT_TEST(CuckooRefinableMap_vector_ord_storehash)


// **************************************************************************************
// FeldmanHashMap


// fixed-sized key - no hash function is necessary

#undef CDSUNIT_DECLARE_FeldmanHashMap_fixed
#undef CDSUNIT_DECLARE_FeldmanHashMap_fixed_RCU_Signal
#undef CDSUNIT_TEST_FeldmanHashMap_fixed
#undef CDSUNIT_TEST_FeldmanHashMap_fixed_RCU_Signal

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#   define CDSUNIT_DECLARE_FeldmanHashMap_fixed_RCU_Signal \
        TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_shb_fixed) \
        TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_shb_fixed_stat) \
        TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_sht_fixed) \
        TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_sht_fixed_stat) \

#   define CDSUNIT_TEST_FeldmanHashMap_fixed_RCU_Signal \
        CPPUNIT_TEST(FeldmanHashMap_rcu_shb_fixed) \
        CPPUNIT_TEST(FeldmanHashMap_rcu_shb_fixed_stat) \
        CPPUNIT_TEST(FeldmanHashMap_rcu_sht_fixed) \
        CPPUNIT_TEST(FeldmanHashMap_rcu_sht_fixed_stat) \

#else
#   define CDSUNIT_DECLARE_FeldmanHashMap_fixed_RCU_Signal
#   define CDSUNIT_TEST_FeldmanHashMap_fixed_RCU_Signal
#endif


#define CDSUNIT_DECLARE_FeldmanHashMap_fixed \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_hp_fixed) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_hp_fixed_stat) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_dhp_fixed) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_dhp_fixed_stat) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_gpi_fixed) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_gpi_fixed_stat) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_gpb_fixed) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_gpb_fixed_stat) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_gpt_fixed) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_gpt_fixed_stat) \
    CDSUNIT_DECLARE_FeldmanHashMap_fixed_RCU_Signal

#define CDSUNIT_TEST_FeldmanHashMap_fixed \
    CPPUNIT_TEST(FeldmanHashMap_hp_fixed) \
    CPPUNIT_TEST(FeldmanHashMap_hp_fixed_stat) \
    CPPUNIT_TEST(FeldmanHashMap_dhp_fixed) \
    CPPUNIT_TEST(FeldmanHashMap_dhp_fixed_stat) \
    CPPUNIT_TEST(FeldmanHashMap_rcu_gpi_fixed) \
    CPPUNIT_TEST(FeldmanHashMap_rcu_gpi_fixed_stat) \
    CPPUNIT_TEST(FeldmanHashMap_rcu_gpb_fixed) \
    CPPUNIT_TEST(FeldmanHashMap_rcu_gpb_fixed_stat) \
    CPPUNIT_TEST(FeldmanHashMap_rcu_gpt_fixed) \
    CPPUNIT_TEST(FeldmanHashMap_rcu_gpt_fixed_stat) \
    CDSUNIT_TEST_FeldmanHashMap_fixed_RCU_Signal

// std::hash

#undef CDSUNIT_DECLARE_FeldmanHashMap_stdhash
#undef CDSUNIT_DECLARE_FeldmanHashMap_stdhash_RCU_Signal
#undef CDSUNIT_TEST_FeldmanHashMap_stdhash
#undef CDSUNIT_TEST_FeldmanHashMap_stdhash_RCU_Signal

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#   define CDSUNIT_DECLARE_FeldmanHashMap_stdhash_RCU_Signal \
        TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_shb_stdhash) \
        TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_shb_stdhash_stat) \
        TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_sht_stdhash) \
        TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_sht_stdhash_stat) \

#   define CDSUNIT_TEST_FeldmanHashMap_stdhash_RCU_Signal \
        CPPUNIT_TEST(FeldmanHashMap_rcu_shb_stdhash) \
        CPPUNIT_TEST(FeldmanHashMap_rcu_shb_stdhash_stat) \
        CPPUNIT_TEST(FeldmanHashMap_rcu_sht_stdhash) \
        CPPUNIT_TEST(FeldmanHashMap_rcu_sht_stdhash_stat) \

#else
#   define CDSUNIT_DECLARE_FeldmanHashMap_stdhash_RCU_Signal
#   define CDSUNIT_TEST_FeldmanHashMap_stdhash_RCU_Signal
#endif


#define CDSUNIT_DECLARE_FeldmanHashMap_stdhash \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_hp_stdhash) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_hp_stdhash_stat) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_dhp_stdhash) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_dhp_stdhash_stat) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_gpi_stdhash) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_gpi_stdhash_stat) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_gpb_stdhash) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_gpb_stdhash_stat) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_gpt_stdhash) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_gpt_stdhash_stat) \
    CDSUNIT_DECLARE_FeldmanHashMap_stdhash_RCU_Signal

#define CDSUNIT_TEST_FeldmanHashMap_stdhash \
    CPPUNIT_TEST(FeldmanHashMap_hp_stdhash) \
    CPPUNIT_TEST(FeldmanHashMap_hp_stdhash_stat) \
    CPPUNIT_TEST(FeldmanHashMap_dhp_stdhash) \
    CPPUNIT_TEST(FeldmanHashMap_dhp_stdhash_stat) \
    CPPUNIT_TEST(FeldmanHashMap_rcu_gpi_stdhash) \
    CPPUNIT_TEST(FeldmanHashMap_rcu_gpi_stdhash_stat) \
    CPPUNIT_TEST(FeldmanHashMap_rcu_gpb_stdhash) \
    CPPUNIT_TEST(FeldmanHashMap_rcu_gpb_stdhash_stat) \
    CPPUNIT_TEST(FeldmanHashMap_rcu_gpt_stdhash) \
    CPPUNIT_TEST(FeldmanHashMap_rcu_gpt_stdhash_stat) \
    CDSUNIT_TEST_FeldmanHashMap_stdhash_RCU_Signal

// CityHash - only for 64bit

#undef CDSUNIT_DECLARE_FeldmanHashMap_city128_RCU_Signal
#undef CDSUNIT_TEST_FeldmanHashMap_city128_RCU_Signal

#if CDS_BUILD_BITS == 64

#undef CDSUNIT_DECLARE_FeldmanHashMap_city64
#undef CDSUNIT_DECLARE_FeldmanHashMap_city64_RCU_Signal
#undef CDSUNIT_TEST_FeldmanHashMap_city64
#undef CDSUNIT_TEST_FeldmanHashMap_city64_RCU_Signal

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#   define CDSUNIT_DECLARE_FeldmanHashMap_city64_RCU_Signal \
        TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_shb_city64) \
        TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_shb_city64_stat) \
        TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_sht_city64) \
        TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_sht_city64_stat) \

#   define CDSUNIT_TEST_FeldmanHashMap_city64_RCU_Signal \
        CPPUNIT_TEST(FeldmanHashMap_rcu_shb_city64) \
        CPPUNIT_TEST(FeldmanHashMap_rcu_shb_city64_stat) \
        CPPUNIT_TEST(FeldmanHashMap_rcu_sht_city64) \
        CPPUNIT_TEST(FeldmanHashMap_rcu_sht_city64_stat) \

#else
#   define CDSUNIT_DECLARE_FeldmanHashMap_city64_RCU_Signal
#   define CDSUNIT_TEST_FeldmanHashMap_city64_RCU_Signal
#endif


#define CDSUNIT_DECLARE_FeldmanHashMap_city64 \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_hp_city64) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_hp_city64_stat) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_dhp_city64) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_dhp_city64_stat) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_gpi_city64) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_gpi_city64_stat) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_gpb_city64) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_gpb_city64_stat) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_gpt_city64) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_gpt_city64_stat) \
    CDSUNIT_DECLARE_FeldmanHashMap_city64_RCU_Signal

#define CDSUNIT_TEST_FeldmanHashMap_city64 \
    CPPUNIT_TEST(FeldmanHashMap_hp_city64) \
    CPPUNIT_TEST(FeldmanHashMap_hp_city64_stat) \
    CPPUNIT_TEST(FeldmanHashMap_dhp_city64) \
    CPPUNIT_TEST(FeldmanHashMap_dhp_city64_stat) \
    CPPUNIT_TEST(FeldmanHashMap_rcu_gpi_city64) \
    CPPUNIT_TEST(FeldmanHashMap_rcu_gpi_city64_stat) \
    CPPUNIT_TEST(FeldmanHashMap_rcu_gpb_city64) \
    CPPUNIT_TEST(FeldmanHashMap_rcu_gpb_city64_stat) \
    CPPUNIT_TEST(FeldmanHashMap_rcu_gpt_city64) \
    CPPUNIT_TEST(FeldmanHashMap_rcu_gpt_city64_stat) \
    CDSUNIT_TEST_FeldmanHashMap_city64_RCU_Signal

#undef CDSUNIT_DECLARE_FeldmanHashMap_city128
#undef CDSUNIT_DECLARE_FeldmanHashMap_city128_RCU_Signal
#undef CDSUNIT_TEST_FeldmanHashMap_city128
#undef CDSUNIT_TEST_FeldmanHashMap_city128_RCU_Signal

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#   define CDSUNIT_DECLARE_FeldmanHashMap_city128_RCU_Signal \
        TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_shb_city128) \
        TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_shb_city128_stat) \
        TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_sht_city128) \
        TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_sht_city128_stat) \

#   define CDSUNIT_TEST_FeldmanHashMap_city128_RCU_Signal \
        CPPUNIT_TEST(FeldmanHashMap_rcu_shb_city128) \
        CPPUNIT_TEST(FeldmanHashMap_rcu_shb_city128_stat) \
        CPPUNIT_TEST(FeldmanHashMap_rcu_sht_city128) \
        CPPUNIT_TEST(FeldmanHashMap_rcu_sht_city128_stat) \

#else
#   define CDSUNIT_DECLARE_FeldmanHashMap_city128_RCU_Signal
#   define CDSUNIT_TEST_FeldmanHashMap_city128_RCU_Signal
#endif


#define CDSUNIT_DECLARE_FeldmanHashMap_city128 \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_hp_city128) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_hp_city128_stat) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_dhp_city128) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_dhp_city128_stat) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_gpi_city128) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_gpi_city128_stat) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_gpb_city128) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_gpb_city128_stat) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_gpt_city128) \
    TEST_CASE(tag_FeldmanHashMap, FeldmanHashMap_rcu_gpt_city128_stat) \
    CDSUNIT_DECLARE_FeldmanHashMap_city128_RCU_Signal

#define CDSUNIT_TEST_FeldmanHashMap_city128 \
    CPPUNIT_TEST(FeldmanHashMap_hp_city128) \
    CPPUNIT_TEST(FeldmanHashMap_hp_city128_stat) \
    CPPUNIT_TEST(FeldmanHashMap_dhp_city128) \
    CPPUNIT_TEST(FeldmanHashMap_dhp_city128_stat) \
    CPPUNIT_TEST(FeldmanHashMap_rcu_gpi_city128) \
    CPPUNIT_TEST(FeldmanHashMap_rcu_gpi_city128_stat) \
    CPPUNIT_TEST(FeldmanHashMap_rcu_gpb_city128) \
    CPPUNIT_TEST(FeldmanHashMap_rcu_gpb_city128_stat) \
    CPPUNIT_TEST(FeldmanHashMap_rcu_gpt_city128) \
    CPPUNIT_TEST(FeldmanHashMap_rcu_gpt_city128_stat) \
    CDSUNIT_TEST_FeldmanHashMap_city128_RCU_Signal

#define CDSUNIT_DECLARE_FeldmanHashMap_city \
    CDSUNIT_DECLARE_FeldmanHashMap_city64 \
    CDSUNIT_DECLARE_FeldmanHashMap_city128

#define CDSUNIT_TEST_FeldmanHashMap_city \
    CDSUNIT_TEST_FeldmanHashMap_city64 \
    CDSUNIT_TEST_FeldmanHashMap_city128

#else // 32bit
#   define CDSUNIT_DECLARE_FeldmanHashMap_city
#   define CDSUNIT_TEST_FeldmanHashMap_city
#endif // #if CDS_BUILD_BITS == 64

#undef CDSUNIT_DECLARE_FeldmanHashMap
#undef CDSUNIT_TEST_FeldmanHashMap

#define CDSUNIT_DECLARE_FeldmanHashMap \
    CDSUNIT_DECLARE_FeldmanHashMap_fixed \
    CDSUNIT_DECLARE_FeldmanHashMap_stdhash \
    CDSUNIT_DECLARE_FeldmanHashMap_city \

#define CDSUNIT_TEST_FeldmanHashMap \
    CDSUNIT_TEST_FeldmanHashMap_fixed \
    CDSUNIT_TEST_FeldmanHashMap_stdhash \
    CDSUNIT_TEST_FeldmanHashMap_city

