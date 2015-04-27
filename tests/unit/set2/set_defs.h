//$$CDS-header$$

#ifndef CDSUNIT_SET_DEFS_H
#define CDSUNIT_SET_DEFS_H

#define CDSUNIT_DECLARE_StdSet \
    CDSUNIT_DECLARE_TEST(StdSet_Spin) \
    CDSUNIT_DECLARE_TEST(StdHashSet_Spin)

#define CDSUNIT_DEFINE_StdSet(IMPL, C) \
    TEST_SET(IMPL, C, StdSet_Spin) \
    TEST_SET(IMPL, C, StdHashSet_Spin)

#define CDSUNIT_TEST_StdSet \
    CPPUNIT_TEST(StdSet_Spin) \
    CPPUNIT_TEST(StdHashSet_Spin)

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#   define CDSUNIT_DECLARE_MichaelSet_RCU_signal  \
    CDSUNIT_DECLARE_TEST(MichaelSet_RCU_SHB_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelSet_RCU_SHB_less_michaelAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelSet_RCU_SHT_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelSet_RCU_SHT_less_michaelAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelSet_Lazy_RCU_SHB_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelSet_Lazy_RCU_SHB_less_michaelAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelSet_Lazy_RCU_SHT_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelSet_Lazy_RCU_SHT_less_michaelAlloc)

#   define CDSUNIT_DEFINE_MichaelSet_RCU_signal(IMPL, C )  \
    TEST_SET_EXTRACT(IMPL, C, MichaelSet_RCU_SHB_cmp_stdAlloc) \
    TEST_SET_EXTRACT(IMPL, C, MichaelSet_RCU_SHB_less_michaelAlloc) \
    TEST_SET_EXTRACT(IMPL, C, MichaelSet_RCU_SHT_cmp_stdAlloc) \
    TEST_SET_EXTRACT(IMPL, C, MichaelSet_RCU_SHT_less_michaelAlloc) \
    TEST_SET_EXTRACT(IMPL, C, MichaelSet_Lazy_RCU_SHB_cmp_stdAlloc) \
    TEST_SET_EXTRACT(IMPL, C, MichaelSet_Lazy_RCU_SHB_less_michaelAlloc) \
    TEST_SET_EXTRACT(IMPL, C, MichaelSet_Lazy_RCU_SHT_cmp_stdAlloc) \
    TEST_SET_EXTRACT(IMPL, C, MichaelSet_Lazy_RCU_SHT_less_michaelAlloc)

#   define CDSUNIT_TEST_MichaelSet_RCU_signal  \
    CPPUNIT_TEST(MichaelSet_RCU_SHB_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelSet_RCU_SHB_less_michaelAlloc) \
    CPPUNIT_TEST(MichaelSet_RCU_SHT_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelSet_RCU_SHT_less_michaelAlloc) \
    CPPUNIT_TEST(MichaelSet_Lazy_RCU_SHB_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelSet_Lazy_RCU_SHB_less_michaelAlloc) \
    CPPUNIT_TEST(MichaelSet_Lazy_RCU_SHT_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelSet_Lazy_RCU_SHT_less_michaelAlloc)
#else
#   define CDSUNIT_DECLARE_MichaelSet_RCU_signal
#   define CDSUNIT_DEFINE_MichaelSet_RCU_signal(IMPL, C )
#   define CDSUNIT_TEST_MichaelSet_RCU_signal
#endif


#define CDSUNIT_DECLARE_MichaelSet  \
    CDSUNIT_DECLARE_TEST(MichaelSet_HP_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelSet_HP_less_michaelAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelSet_DHP_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelSet_DHP_less_michaelAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelSet_RCU_GPI_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelSet_RCU_GPI_less_michaelAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelSet_RCU_GPB_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelSet_RCU_GPB_less_michaelAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelSet_RCU_GPT_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelSet_RCU_GPT_less_michaelAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelSet_Lazy_HP_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelSet_Lazy_HP_less_michaelAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelSet_Lazy_DHP_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelSet_Lazy_DHP_less_michaelAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelSet_Lazy_RCU_GPI_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelSet_Lazy_RCU_GPI_less_michaelAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelSet_Lazy_RCU_GPB_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelSet_Lazy_RCU_GPB_less_michaelAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelSet_Lazy_RCU_GPT_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelSet_Lazy_RCU_GPT_less_michaelAlloc) \
    CDSUNIT_DECLARE_MichaelSet_RCU_signal

#define CDSUNIT_DEFINE_MichaelSet( IMPL, C )  \
    TEST_SET_EXTRACT(IMPL, C, MichaelSet_HP_cmp_stdAlloc) \
    TEST_SET_EXTRACT(IMPL, C, MichaelSet_HP_less_michaelAlloc) \
    TEST_SET_EXTRACT(IMPL, C, MichaelSet_DHP_cmp_stdAlloc) \
    TEST_SET_EXTRACT(IMPL, C, MichaelSet_DHP_less_michaelAlloc) \
    TEST_SET_EXTRACT(IMPL, C, MichaelSet_RCU_GPI_cmp_stdAlloc) \
    TEST_SET_EXTRACT(IMPL, C, MichaelSet_RCU_GPI_less_michaelAlloc) \
    TEST_SET_EXTRACT(IMPL, C, MichaelSet_RCU_GPB_cmp_stdAlloc) \
    TEST_SET_EXTRACT(IMPL, C, MichaelSet_RCU_GPB_less_michaelAlloc) \
    TEST_SET_EXTRACT(IMPL, C, MichaelSet_RCU_GPT_cmp_stdAlloc) \
    TEST_SET_EXTRACT(IMPL, C, MichaelSet_RCU_GPT_less_michaelAlloc) \
    TEST_SET_EXTRACT(IMPL, C, MichaelSet_Lazy_HP_cmp_stdAlloc) \
    TEST_SET_EXTRACT(IMPL, C, MichaelSet_Lazy_HP_less_michaelAlloc) \
    TEST_SET_EXTRACT(IMPL, C, MichaelSet_Lazy_DHP_cmp_stdAlloc) \
    TEST_SET_EXTRACT(IMPL, C, MichaelSet_Lazy_DHP_less_michaelAlloc) \
    TEST_SET_EXTRACT(IMPL, C, MichaelSet_Lazy_RCU_GPI_cmp_stdAlloc) \
    TEST_SET_EXTRACT(IMPL, C, MichaelSet_Lazy_RCU_GPI_less_michaelAlloc) \
    TEST_SET_EXTRACT(IMPL, C, MichaelSet_Lazy_RCU_GPB_cmp_stdAlloc) \
    TEST_SET_EXTRACT(IMPL, C, MichaelSet_Lazy_RCU_GPB_less_michaelAlloc) \
    TEST_SET_EXTRACT(IMPL, C, MichaelSet_Lazy_RCU_GPT_cmp_stdAlloc) \
    TEST_SET_EXTRACT(IMPL, C, MichaelSet_Lazy_RCU_GPT_less_michaelAlloc) \
    CDSUNIT_DEFINE_MichaelSet_RCU_signal(IMPL, C)

#define CDSUNIT_TEST_MichaelSet  \
    CPPUNIT_TEST(MichaelSet_HP_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelSet_HP_less_michaelAlloc) \
    CPPUNIT_TEST(MichaelSet_DHP_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelSet_DHP_less_michaelAlloc) \
    CPPUNIT_TEST(MichaelSet_RCU_GPI_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelSet_RCU_GPI_less_michaelAlloc) \
    CPPUNIT_TEST(MichaelSet_RCU_GPB_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelSet_RCU_GPB_less_michaelAlloc) \
    CPPUNIT_TEST(MichaelSet_RCU_GPT_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelSet_RCU_GPT_less_michaelAlloc) \
    CPPUNIT_TEST(MichaelSet_Lazy_HP_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelSet_Lazy_HP_less_michaelAlloc) \
    CPPUNIT_TEST(MichaelSet_Lazy_DHP_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelSet_Lazy_DHP_less_michaelAlloc) \
    CPPUNIT_TEST(MichaelSet_Lazy_RCU_GPI_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelSet_Lazy_RCU_GPI_less_michaelAlloc) \
    CPPUNIT_TEST(MichaelSet_Lazy_RCU_GPB_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelSet_Lazy_RCU_GPB_less_michaelAlloc) \
    CPPUNIT_TEST(MichaelSet_Lazy_RCU_GPT_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelSet_Lazy_RCU_GPT_less_michaelAlloc) \
    CDSUNIT_TEST_MichaelSet_RCU_signal


#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#   define CDSUNIT_DECLARE_SplitList_RCU_signal  \
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_SHB_dyn_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_SHB_dyn_cmp_stat)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_SHB_st_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_SHB_dyn_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_SHB_st_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_SHB_st_less_stat)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_SHT_dyn_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_SHT_dyn_cmp_stat)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_SHT_st_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_SHT_dyn_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_SHT_st_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_SHT_st_less_stat)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_SHB_dyn_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_SHB_dyn_cmp_stat)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_SHB_st_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_SHB_dyn_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_SHB_st_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_SHB_st_less_stat)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_SHT_dyn_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_SHT_dyn_cmp_stat)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_SHT_st_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_SHT_dyn_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_SHT_st_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_SHT_st_less_stat)

#   define CDSUNIT_DEFINE_SplitList_RCU_signal( IMPL, C )  \
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_SHB_dyn_cmp)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_SHB_dyn_cmp_stat)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_SHB_st_cmp)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_SHB_dyn_less)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_SHB_st_less)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_SHB_st_less_stat)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_SHT_dyn_cmp)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_SHT_dyn_cmp_stat)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_SHT_st_cmp)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_SHT_dyn_less)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_SHT_st_less)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_SHT_st_less_stat)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_SHB_dyn_cmp)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_SHB_dyn_cmp_stat)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_SHB_st_cmp)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_SHB_dyn_less)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_SHB_st_less)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_SHB_st_less_stat)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_SHT_dyn_cmp)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_SHT_dyn_cmp_stat)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_SHT_st_cmp)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_SHT_dyn_less)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_SHT_st_less)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_SHT_st_less_stat)

#   define CDSUNIT_TEST_SplitList_RCU_signal  \
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
#   define CDSUNIT_DEFINE_SplitList_RCU_signal( IMPL, C )
#   define CDSUNIT_TEST_SplitList_RCU_signal
#endif

#define CDSUNIT_DECLARE_SplitList  \
    CDSUNIT_DECLARE_TEST(SplitList_Michael_HP_dyn_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_HP_dyn_cmp_stat)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_HP_st_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_HP_dyn_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_HP_st_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_HP_st_less_stat)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_DHP_dyn_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_DHP_dyn_cmp_stat)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_DHP_st_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_DHP_dyn_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_DHP_st_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_DHP_st_less_stat)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_GPI_dyn_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_GPI_dyn_cmp_stat)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_GPI_st_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_GPI_dyn_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_GPI_st_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_GPI_st_less_stat)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_GPB_dyn_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_GPB_dyn_cmp_stat)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_GPB_st_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_GPB_dyn_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_GPB_st_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_GPB_st_less_stat)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_GPT_dyn_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_GPT_dyn_cmp_stat)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_GPT_st_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_GPT_dyn_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_GPT_st_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_RCU_GPT_st_less_stat)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_HP_dyn_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_HP_dyn_cmp_stat)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_HP_st_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_HP_dyn_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_HP_st_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_HP_st_less_stat)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_DHP_dyn_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_DHP_dyn_cmp_stat)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_DHP_st_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_DHP_dyn_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_DHP_st_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_DHP_st_less_stat)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_GPI_dyn_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_GPI_dyn_cmp_stat)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_GPI_st_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_GPI_dyn_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_GPI_st_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_GPI_st_less_stat)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_GPB_dyn_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_GPB_dyn_cmp_stat)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_GPB_st_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_GPB_dyn_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_GPB_st_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_GPB_st_less_stat)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_GPT_dyn_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_GPT_dyn_cmp_stat)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_GPT_st_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_GPT_dyn_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_GPT_st_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_RCU_GPT_st_less_stat)\
    CDSUNIT_DECLARE_SplitList_RCU_signal

#define CDSUNIT_DEFINE_SplitList( IMPL, C ) \
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_HP_dyn_cmp)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_HP_dyn_cmp_stat)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_HP_st_cmp)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_HP_dyn_less)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_HP_st_less)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_HP_st_less_stat)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_DHP_dyn_cmp)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_DHP_dyn_cmp_stat)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_DHP_st_cmp)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_DHP_dyn_less)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_DHP_st_less)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_DHP_st_less_stat)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_GPI_dyn_cmp)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_GPI_dyn_cmp_stat)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_GPI_st_cmp)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_GPI_dyn_less)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_GPI_st_less)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_GPI_st_less_stat)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_GPB_dyn_cmp)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_GPB_dyn_cmp_stat)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_GPB_st_cmp)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_GPB_dyn_less)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_GPB_st_less)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_GPB_st_less_stat)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_GPT_dyn_cmp)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_GPT_dyn_cmp_stat)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_GPT_st_cmp)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_GPT_dyn_less)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_GPT_st_less)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Michael_RCU_GPT_st_less_stat)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_HP_dyn_cmp)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_HP_dyn_cmp_stat)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_HP_st_cmp)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_HP_dyn_less)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_HP_st_less)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_HP_st_less_stat)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_DHP_dyn_cmp)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_DHP_dyn_cmp_stat)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_DHP_st_cmp)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_DHP_dyn_less)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_DHP_st_less)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_DHP_st_less_stat)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_GPI_dyn_cmp)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_GPI_dyn_cmp_stat)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_GPI_st_cmp)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_GPI_dyn_less)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_GPI_st_less)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_GPI_st_less_stat)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_GPB_dyn_cmp)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_GPB_dyn_cmp_stat)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_GPB_st_cmp)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_GPB_dyn_less)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_GPB_st_less)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_GPB_st_less_stat)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_GPT_dyn_cmp)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_GPT_dyn_cmp_stat)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_GPT_st_cmp)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_GPT_dyn_less)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_GPT_st_less)\
    TEST_SET_EXTRACT( IMPL, C, SplitList_Lazy_RCU_GPT_st_less_stat)\
    CDSUNIT_DEFINE_SplitList_RCU_signal( IMPL, C )

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
    CPPUNIT_TEST(SplitList_Lazy_HP_st_less_stat)\
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


#define CDSUNIT_DECLARE_CuckooSet \
    CDSUNIT_DECLARE_TEST(CuckooStripedSet_list_unord)\
    CDSUNIT_DECLARE_TEST(CuckooStripedSet_list_unord_stat)\
    CDSUNIT_DECLARE_TEST(CuckooStripedSet_list_ord)\
    CDSUNIT_DECLARE_TEST(CuckooStripedSet_list_ord_stat)\
    CDSUNIT_DECLARE_TEST(CuckooStripedSet_vector_unord)\
    CDSUNIT_DECLARE_TEST(CuckooStripedSet_vector_ord)\
    CDSUNIT_DECLARE_TEST(CuckooStripedSet_vector_unord_stat)\
    CDSUNIT_DECLARE_TEST(CuckooStripedSet_vector_ord_stat)\
    CDSUNIT_DECLARE_TEST(CuckooRefinableSet_list_unord)\
    CDSUNIT_DECLARE_TEST(CuckooRefinableSet_list_ord)\
    CDSUNIT_DECLARE_TEST(CuckooRefinableSet_list_unord_stat)\
    CDSUNIT_DECLARE_TEST(CuckooRefinableSet_list_ord_stat)\
    CDSUNIT_DECLARE_TEST(CuckooRefinableSet_vector_unord)\
    CDSUNIT_DECLARE_TEST(CuckooRefinableSet_vector_unord_stat)\
    CDSUNIT_DECLARE_TEST(CuckooRefinableSet_vector_ord) \
    CDSUNIT_DECLARE_TEST(CuckooRefinableSet_vector_ord_stat) \
    CDSUNIT_DECLARE_TEST(CuckooStripedSet_list_unord_storehash)\
    CDSUNIT_DECLARE_TEST(CuckooStripedSet_list_ord_storehash)\
    CDSUNIT_DECLARE_TEST(CuckooStripedSet_vector_unord_storehash)\
    CDSUNIT_DECLARE_TEST(CuckooStripedSet_vector_ord_storehash)\
    CDSUNIT_DECLARE_TEST(CuckooRefinableSet_list_unord_storehash)\
    CDSUNIT_DECLARE_TEST(CuckooRefinableSet_list_ord_storehash)\
    CDSUNIT_DECLARE_TEST(CuckooRefinableSet_vector_unord_storehash)\
    CDSUNIT_DECLARE_TEST(CuckooRefinableSet_vector_ord_storehash)

#define CDSUNIT_DEFINE_CuckooSet(IMPL, C) \
    TEST_SET(IMPL, C, CuckooStripedSet_list_unord)\
    TEST_SET(IMPL, C, CuckooStripedSet_list_unord_stat)\
    TEST_SET(IMPL, C, CuckooStripedSet_list_ord)\
    TEST_SET(IMPL, C, CuckooStripedSet_list_ord_stat)\
    TEST_SET(IMPL, C, CuckooStripedSet_vector_unord)\
    TEST_SET(IMPL, C, CuckooStripedSet_vector_ord)\
    TEST_SET(IMPL, C, CuckooStripedSet_vector_unord_stat)\
    TEST_SET(IMPL, C, CuckooStripedSet_vector_ord_stat)\
    TEST_SET(IMPL, C, CuckooRefinableSet_list_unord)\
    TEST_SET(IMPL, C, CuckooRefinableSet_list_ord)\
    TEST_SET(IMPL, C, CuckooRefinableSet_list_unord_stat)\
    TEST_SET(IMPL, C, CuckooRefinableSet_list_ord_stat)\
    TEST_SET(IMPL, C, CuckooRefinableSet_vector_unord)\
    TEST_SET(IMPL, C, CuckooRefinableSet_vector_unord_stat)\
    TEST_SET(IMPL, C, CuckooRefinableSet_vector_ord) \
    TEST_SET(IMPL, C, CuckooRefinableSet_vector_ord_stat) \
    TEST_SET(IMPL, C, CuckooStripedSet_list_unord_storehash)\
    TEST_SET(IMPL, C, CuckooStripedSet_list_ord_storehash)\
    TEST_SET(IMPL, C, CuckooStripedSet_vector_unord_storehash)\
    TEST_SET(IMPL, C, CuckooStripedSet_vector_ord_storehash)\
    TEST_SET(IMPL, C, CuckooRefinableSet_list_unord_storehash)\
    TEST_SET(IMPL, C, CuckooRefinableSet_list_ord_storehash)\
    TEST_SET(IMPL, C, CuckooRefinableSet_vector_unord_storehash)\
    TEST_SET(IMPL, C, CuckooRefinableSet_vector_ord_storehash)


#define CDSUNIT_TEST_CuckooSet \
    CPPUNIT_TEST(CuckooStripedSet_list_unord)\
    CPPUNIT_TEST(CuckooStripedSet_list_unord_stat)\
    CPPUNIT_TEST(CuckooStripedSet_list_unord_storehash)\
    CPPUNIT_TEST(CuckooStripedSet_list_ord)\
    CPPUNIT_TEST(CuckooStripedSet_list_ord_stat)\
    CPPUNIT_TEST(CuckooStripedSet_list_ord_storehash)\
    CPPUNIT_TEST(CuckooStripedSet_vector_unord)\
    CPPUNIT_TEST(CuckooStripedSet_vector_unord_stat)\
    CPPUNIT_TEST(CuckooStripedSet_vector_unord_storehash)\
    CPPUNIT_TEST(CuckooStripedSet_vector_ord)\
    CPPUNIT_TEST(CuckooStripedSet_vector_ord_stat)\
    CPPUNIT_TEST(CuckooStripedSet_vector_ord_storehash)\
    CPPUNIT_TEST(CuckooRefinableSet_list_unord)\
    CPPUNIT_TEST(CuckooRefinableSet_list_unord_stat)\
    CPPUNIT_TEST(CuckooRefinableSet_list_unord_storehash)\
    CPPUNIT_TEST(CuckooRefinableSet_list_ord)\
    CPPUNIT_TEST(CuckooRefinableSet_list_ord_stat)\
    CPPUNIT_TEST(CuckooRefinableSet_list_ord_storehash)\
    CPPUNIT_TEST(CuckooRefinableSet_vector_unord)\
    CPPUNIT_TEST(CuckooRefinableSet_vector_unord_stat)\
    CPPUNIT_TEST(CuckooRefinableSet_vector_unord_storehash)\
    CPPUNIT_TEST(CuckooRefinableSet_vector_ord) \
    CPPUNIT_TEST(CuckooRefinableSet_vector_ord_stat) \
    CPPUNIT_TEST(CuckooRefinableSet_vector_ord_storehash)



#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#   define CDSUNIT_DECLARE_SkipListSet_RCU_signal \
    CDSUNIT_DECLARE_TEST(SkipListSet_rcu_shb_less_pascal)\
    CDSUNIT_DECLARE_TEST(SkipListSet_rcu_shb_cmp_pascal_stat)\
    CDSUNIT_DECLARE_TEST(SkipListSet_rcu_shb_less_xorshift)\
    CDSUNIT_DECLARE_TEST(SkipListSet_rcu_shb_cmp_xorshift_stat)\
    CDSUNIT_DECLARE_TEST(SkipListSet_rcu_sht_less_pascal)\
    CDSUNIT_DECLARE_TEST(SkipListSet_rcu_sht_cmp_pascal_stat)\
    CDSUNIT_DECLARE_TEST(SkipListSet_rcu_sht_less_xorshift)\
    CDSUNIT_DECLARE_TEST(SkipListSet_rcu_sht_cmp_xorshift_stat)

#   define CDSUNIT_DEFINE_SkipListSet_RCU_signal( IMPL, C ) \
    TEST_SET_NOLF_EXTRACT(IMPL, C, SkipListSet_rcu_shb_less_pascal)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, SkipListSet_rcu_shb_cmp_pascal_stat)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, SkipListSet_rcu_shb_less_xorshift)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, SkipListSet_rcu_shb_cmp_xorshift_stat)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, SkipListSet_rcu_sht_less_pascal)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, SkipListSet_rcu_sht_cmp_pascal_stat)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, SkipListSet_rcu_sht_less_xorshift)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, SkipListSet_rcu_sht_cmp_xorshift_stat)

#   define CDSUNIT_TEST_SkipListSet_RCU_signal \
    CPPUNIT_TEST(SkipListSet_rcu_shb_less_pascal)\
    CPPUNIT_TEST(SkipListSet_rcu_shb_cmp_pascal_stat)\
    CPPUNIT_TEST(SkipListSet_rcu_shb_less_xorshift)\
    CPPUNIT_TEST(SkipListSet_rcu_shb_cmp_xorshift_stat)\
    CPPUNIT_TEST(SkipListSet_rcu_sht_less_pascal)\
    CPPUNIT_TEST(SkipListSet_rcu_sht_cmp_pascal_stat)\
    CPPUNIT_TEST(SkipListSet_rcu_sht_less_xorshift)\
    CPPUNIT_TEST(SkipListSet_rcu_sht_cmp_xorshift_stat)

#else
#   define CDSUNIT_DECLARE_SkipListSet_RCU_signal
#   define CDSUNIT_DEFINE_SkipListSet_RCU_signal( IMPL, C )
#   define CDSUNIT_TEST_SkipListSet_RCU_signal
#endif

#define CDSUNIT_DECLARE_SkipListSet \
    CDSUNIT_DECLARE_TEST(SkipListSet_hp_less_pascal)\
    CDSUNIT_DECLARE_TEST(SkipListSet_hp_cmp_pascal_stat)\
    CDSUNIT_DECLARE_TEST(SkipListSet_hp_less_xorshift)\
    CDSUNIT_DECLARE_TEST(SkipListSet_hp_cmp_xorshift_stat)\
    CDSUNIT_DECLARE_TEST(SkipListSet_dhp_less_pascal)\
    CDSUNIT_DECLARE_TEST(SkipListSet_dhp_cmp_pascal_stat)\
    CDSUNIT_DECLARE_TEST(SkipListSet_dhp_less_xorshift)\
    CDSUNIT_DECLARE_TEST(SkipListSet_dhp_cmp_xorshift_stat)\
    CDSUNIT_DECLARE_TEST(SkipListSet_rcu_gpi_less_pascal)\
    CDSUNIT_DECLARE_TEST(SkipListSet_rcu_gpi_cmp_pascal_stat)\
    CDSUNIT_DECLARE_TEST(SkipListSet_rcu_gpi_less_xorshift)\
    CDSUNIT_DECLARE_TEST(SkipListSet_rcu_gpi_cmp_xorshift_stat)\
    CDSUNIT_DECLARE_TEST(SkipListSet_rcu_gpb_less_pascal)\
    CDSUNIT_DECLARE_TEST(SkipListSet_rcu_gpb_cmp_pascal_stat)\
    CDSUNIT_DECLARE_TEST(SkipListSet_rcu_gpb_less_xorshift)\
    CDSUNIT_DECLARE_TEST(SkipListSet_rcu_gpb_cmp_xorshift_stat)\
    CDSUNIT_DECLARE_TEST(SkipListSet_rcu_gpt_less_pascal)\
    CDSUNIT_DECLARE_TEST(SkipListSet_rcu_gpt_cmp_pascal_stat)\
    CDSUNIT_DECLARE_TEST(SkipListSet_rcu_gpt_less_xorshift)\
    CDSUNIT_DECLARE_TEST(SkipListSet_rcu_gpt_cmp_xorshift_stat)\
    CDSUNIT_DECLARE_SkipListSet_RCU_signal

#define CDSUNIT_DEFINE_SkipListSet(IMPL, C) \
    TEST_SET_NOLF_EXTRACT(IMPL, C, SkipListSet_hp_less_pascal)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, SkipListSet_hp_cmp_pascal_stat)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, SkipListSet_hp_less_xorshift)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, SkipListSet_hp_cmp_xorshift_stat)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, SkipListSet_dhp_less_pascal)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, SkipListSet_dhp_cmp_pascal_stat)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, SkipListSet_dhp_less_xorshift)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, SkipListSet_dhp_cmp_xorshift_stat)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, SkipListSet_rcu_gpi_less_pascal)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, SkipListSet_rcu_gpi_cmp_pascal_stat)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, SkipListSet_rcu_gpi_less_xorshift)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, SkipListSet_rcu_gpi_cmp_xorshift_stat)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, SkipListSet_rcu_gpb_less_pascal)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, SkipListSet_rcu_gpb_cmp_pascal_stat)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, SkipListSet_rcu_gpb_less_xorshift)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, SkipListSet_rcu_gpb_cmp_xorshift_stat)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, SkipListSet_rcu_gpt_less_pascal)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, SkipListSet_rcu_gpt_cmp_pascal_stat)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, SkipListSet_rcu_gpt_less_xorshift)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, SkipListSet_rcu_gpt_cmp_xorshift_stat)\
    CDSUNIT_DEFINE_SkipListSet_RCU_signal( IMPL, C )

#define CDSUNIT_TEST_SkipListSet \
    CPPUNIT_TEST(SkipListSet_hp_less_pascal)\
    CPPUNIT_TEST(SkipListSet_hp_cmp_pascal_stat)\
    CPPUNIT_TEST(SkipListSet_hp_less_xorshift)\
    CPPUNIT_TEST(SkipListSet_hp_cmp_xorshift_stat)\
    CPPUNIT_TEST(SkipListSet_dhp_less_pascal)\
    CPPUNIT_TEST(SkipListSet_dhp_cmp_pascal_stat)\
    CPPUNIT_TEST(SkipListSet_dhp_less_xorshift)\
    CPPUNIT_TEST(SkipListSet_dhp_cmp_xorshift_stat)\
    CPPUNIT_TEST(SkipListSet_rcu_gpi_less_pascal)\
    CPPUNIT_TEST(SkipListSet_rcu_gpi_cmp_pascal_stat)\
    CPPUNIT_TEST(SkipListSet_rcu_gpi_less_xorshift)\
    CPPUNIT_TEST(SkipListSet_rcu_gpi_cmp_xorshift_stat)\
    CPPUNIT_TEST(SkipListSet_rcu_gpb_less_pascal)\
    CPPUNIT_TEST(SkipListSet_rcu_gpb_cmp_pascal_stat)\
    CPPUNIT_TEST(SkipListSet_rcu_gpb_less_xorshift)\
    CPPUNIT_TEST(SkipListSet_rcu_gpb_cmp_xorshift_stat)\
    CPPUNIT_TEST(SkipListSet_rcu_gpt_less_pascal)\
    CPPUNIT_TEST(SkipListSet_rcu_gpt_cmp_pascal_stat)\
    CPPUNIT_TEST(SkipListSet_rcu_gpt_less_xorshift)\
    CPPUNIT_TEST(SkipListSet_rcu_gpt_cmp_xorshift_stat)\
    CDSUNIT_TEST_SkipListSet_RCU_signal


#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#   define CDSUNIT_DECLARE_EllenBinTreeSet_RCU_signal \
        CDSUNIT_DECLARE_TEST(EllenBinTreeSet_rcu_shb)\
        CDSUNIT_DECLARE_TEST(EllenBinTreeSet_rcu_shb_stat)\
        CDSUNIT_DECLARE_TEST(EllenBinTreeSet_rcu_sht)\
        CDSUNIT_DECLARE_TEST(EllenBinTreeSet_rcu_sht_stat)

#   define CDSUNIT_DEFINE_EllenBinTreeSet_RCU_signal( IMPL, C ) \
        TEST_SET_NOLF_EXTRACT(IMPL, C, EllenBinTreeSet_rcu_shb)\
        TEST_SET_NOLF_EXTRACT(IMPL, C, EllenBinTreeSet_rcu_shb_stat)\
        TEST_SET_NOLF_EXTRACT(IMPL, C, EllenBinTreeSet_rcu_sht)\
        TEST_SET_NOLF_EXTRACT(IMPL, C, EllenBinTreeSet_rcu_sht_stat)

#   define CDSUNIT_TEST_EllenBinTreeSet_RCU_signal \
        CPPUNIT_TEST(EllenBinTreeSet_rcu_shb)\
        CPPUNIT_TEST(EllenBinTreeSet_rcu_shb_stat)\
        CPPUNIT_TEST(EllenBinTreeSet_rcu_sht)\
        CPPUNIT_TEST(EllenBinTreeSet_rcu_sht_stat)
#else
#   define CDSUNIT_DECLARE_EllenBinTreeSet_RCU_signal
#   define CDSUNIT_DEFINE_EllenBinTreeSet_RCU_signal( IMPL, C )
#   define CDSUNIT_TEST_EllenBinTreeSet_RCU_signal
#endif

#define CDSUNIT_DECLARE_EllenBinTreeSet \
    CDSUNIT_DECLARE_TEST(EllenBinTreeSet_hp)\
    CDSUNIT_DECLARE_TEST(EllenBinTreeSet_yield_hp)\
    CDSUNIT_DECLARE_TEST(EllenBinTreeSet_hp_stat)\
    CDSUNIT_DECLARE_TEST(EllenBinTreeSet_dhp)\
    CDSUNIT_DECLARE_TEST(EllenBinTreeSet_yield_dhp)\
    CDSUNIT_DECLARE_TEST(EllenBinTreeSet_dhp_stat)\
    CDSUNIT_DECLARE_TEST(EllenBinTreeSet_rcu_gpi)\
    CDSUNIT_DECLARE_TEST(EllenBinTreeSet_rcu_gpi_stat)\
    CDSUNIT_DECLARE_TEST(EllenBinTreeSet_rcu_gpb)\
    CDSUNIT_DECLARE_TEST(EllenBinTreeSet_yield_rcu_gpb)\
    CDSUNIT_DECLARE_TEST(EllenBinTreeSet_rcu_gpb_stat)\
    CDSUNIT_DECLARE_TEST(EllenBinTreeSet_rcu_gpt)\
    CDSUNIT_DECLARE_TEST(EllenBinTreeSet_rcu_gpt_stat)\
    CDSUNIT_DECLARE_EllenBinTreeSet_RCU_signal

#define CDSUNIT_DEFINE_EllenBinTreeSet( IMPL, C ) \
    TEST_SET_NOLF_EXTRACT(IMPL, C, EllenBinTreeSet_hp)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, EllenBinTreeSet_yield_hp)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, EllenBinTreeSet_hp_stat)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, EllenBinTreeSet_dhp)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, EllenBinTreeSet_yield_dhp)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, EllenBinTreeSet_dhp_stat)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, EllenBinTreeSet_rcu_gpi)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, EllenBinTreeSet_rcu_gpi_stat)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, EllenBinTreeSet_rcu_gpb)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, EllenBinTreeSet_yield_rcu_gpb)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, EllenBinTreeSet_rcu_gpb_stat)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, EllenBinTreeSet_rcu_gpt)\
    TEST_SET_NOLF_EXTRACT(IMPL, C, EllenBinTreeSet_rcu_gpt_stat)\
    CDSUNIT_DEFINE_EllenBinTreeSet_RCU_signal(IMPL, C)

#define CDSUNIT_TEST_EllenBinTreeSet \
    CPPUNIT_TEST(EllenBinTreeSet_hp)\
    CPPUNIT_TEST(EllenBinTreeSet_yield_hp)\
    CPPUNIT_TEST(EllenBinTreeSet_hp_stat)\
    CPPUNIT_TEST(EllenBinTreeSet_dhp)\
    CPPUNIT_TEST(EllenBinTreeSet_yield_dhp)\
    CPPUNIT_TEST(EllenBinTreeSet_dhp_stat)\
    CPPUNIT_TEST(EllenBinTreeSet_rcu_gpi)\
    /*CPPUNIT_TEST(EllenBinTreeSet_rcu_gpi_stat)*/\
    CPPUNIT_TEST(EllenBinTreeSet_rcu_gpb)\
    CPPUNIT_TEST(EllenBinTreeSet_yield_rcu_gpb)\
    CPPUNIT_TEST(EllenBinTreeSet_rcu_gpb_stat)\
    CPPUNIT_TEST(EllenBinTreeSet_rcu_gpt)\
    CPPUNIT_TEST(EllenBinTreeSet_rcu_gpt_stat)\
    CDSUNIT_TEST_EllenBinTreeSet_RCU_signal



#define CDSUNIT_DECLARE_StripedSet_common \
    CDSUNIT_DECLARE_TEST(StripedSet_list) \
    CDSUNIT_DECLARE_TEST(StripedSet_vector) \
    CDSUNIT_DECLARE_TEST(StripedSet_set) \
    CDSUNIT_DECLARE_TEST(StripedSet_hashset) \
    CDSUNIT_DECLARE_TEST(StripedSet_boost_unordered_set) \
    CDSUNIT_DECLARE_TEST(StripedSet_rational_list) \
    CDSUNIT_DECLARE_TEST(StripedSet_rational_vector) \
    CDSUNIT_DECLARE_TEST(StripedSet_rational_set) \
    CDSUNIT_DECLARE_TEST(StripedSet_rational_hashset) \
    CDSUNIT_DECLARE_TEST(StripedSet_rational_boost_unordered_set)

#define CDSUNIT_DEFINE_StripedSet_common( IMPL, C ) \
    TEST_SET(IMPL, C, StripedSet_list) \
    TEST_SET(IMPL, C, StripedSet_vector) \
    TEST_SET(IMPL, C, StripedSet_set) \
    TEST_SET(IMPL, C, StripedSet_hashset) \
    TEST_SET(IMPL, C, StripedSet_boost_unordered_set) \
    TEST_SET(IMPL, C, StripedSet_rational_list) \
    TEST_SET(IMPL, C, StripedSet_rational_vector) \
    TEST_SET(IMPL, C, StripedSet_rational_set) \
    TEST_SET(IMPL, C, StripedSet_rational_hashset) \
    TEST_SET(IMPL, C, StripedSet_rational_boost_unordered_set)

#define CDSUNIT_TEST_StripedSet_common \
    CPPUNIT_TEST(StripedSet_list) \
    CPPUNIT_TEST(StripedSet_vector) \
    CPPUNIT_TEST(StripedSet_set) \
    CPPUNIT_TEST(StripedSet_hashset) \
    CPPUNIT_TEST(StripedSet_boost_unordered_set) \
    CPPUNIT_TEST(StripedSet_rational_list) \
    CPPUNIT_TEST(StripedSet_rational_vector) \
    CPPUNIT_TEST(StripedSet_rational_set) \
    CPPUNIT_TEST(StripedSet_rational_hashset) \
    CPPUNIT_TEST(StripedSet_rational_boost_unordered_set)

#if BOOST_VERSION >= 104800
#   define CDSUNIT_DECLARE_StripedSet_boost_container \
    CDSUNIT_DECLARE_TEST(StripedSet_boost_list) \
    CDSUNIT_DECLARE_TEST(StripedSet_boost_slist) \
    CDSUNIT_DECLARE_TEST(StripedSet_boost_vector) \
    CDSUNIT_DECLARE_TEST(StripedSet_boost_stable_vector) \
    CDSUNIT_DECLARE_TEST(StripedSet_boost_set) \
    CDSUNIT_DECLARE_TEST(StripedSet_rational_boost_list) \
    CDSUNIT_DECLARE_TEST(StripedSet_rational_boost_slist) \
    CDSUNIT_DECLARE_TEST(StripedSet_rational_boost_vector) \
    CDSUNIT_DECLARE_TEST(StripedSet_rational_boost_stable_vector) \
    CDSUNIT_DECLARE_TEST(StripedSet_rational_boost_set)

#   define CDSUNIT_DEFINE_StripedSet_boost_container( IMPL, C ) \
    TEST_SET(IMPL, C, StripedSet_boost_list) \
    TEST_SET(IMPL, C, StripedSet_boost_slist) \
    TEST_SET(IMPL, C, StripedSet_boost_vector) \
    TEST_SET(IMPL, C, StripedSet_boost_stable_vector) \
    TEST_SET(IMPL, C, StripedSet_boost_set) \
    TEST_SET(IMPL, C, StripedSet_rational_boost_list) \
    TEST_SET(IMPL, C, StripedSet_rational_boost_slist) \
    TEST_SET(IMPL, C, StripedSet_rational_boost_vector) \
    TEST_SET(IMPL, C, StripedSet_rational_boost_stable_vector) \
    TEST_SET(IMPL, C, StripedSet_rational_boost_set)

#   define CDSUNIT_TEST_StripedSet_boost_container \
    CPPUNIT_TEST(StripedSet_boost_list) \
    CPPUNIT_TEST(StripedSet_boost_slist) \
    CPPUNIT_TEST(StripedSet_boost_vector) \
    CPPUNIT_TEST(StripedSet_boost_stable_vector) \
    CPPUNIT_TEST(StripedSet_boost_set) \
    CPPUNIT_TEST(StripedSet_rational_boost_list) \
    CPPUNIT_TEST(StripedSet_rational_boost_slist) \
    CPPUNIT_TEST(StripedSet_rational_boost_vector) \
    CPPUNIT_TEST(StripedSet_rational_boost_stable_vector) \
    CPPUNIT_TEST(StripedSet_rational_boost_set)
#else
#   define CDSUNIT_DECLARE_StripedSet_boost_container
#   define CDSUNIT_DEFINE_StripedSet_boost_container( IMPL, C )
#   define CDSUNIT_TEST_StripedSet_boost_container
#endif

#if BOOST_VERSION >= 104800 && defined(CDS_UNIT_SET_TYPES_ENABLE_BOOST_FLAT_CONTAINERS)
#   define CDSUNIT_DECLARE_StripedSet_boost_flat_container \
    CDSUNIT_DECLARE_TEST(StripedSet_boost_flat_set) \
    CDSUNIT_DECLARE_TEST(StripedSet_rational_boost_flat_set)
#   define CDSUNIT_DEFINE_StripedSet_boost_flat_container( IMPL, C ) \
    TEST_SET(IMPL, C, StripedSet_boost_flat_set) \
    TEST_SET(IMPL, C, StripedSet_rational_boost_flat_set)
#   define CDSUNIT_TEST_StripedSet_boost_flat_container \
    CPPUNIT_TEST(StripedSet_boost_flat_set) \
    CPPUNIT_TEST(StripedSet_rational_boost_flat_set)
#else
#   define CDSUNIT_DECLARE_StripedSet_boost_flat_container
#   define CDSUNIT_DEFINE_StripedSet_boost_flat_container( IMPL, C )
#   define CDSUNIT_TEST_StripedSet_boost_flat_container
#endif

#define CDSUNIT_DECLARE_StripedSet \
    CDSUNIT_DECLARE_StripedSet_common \
    CDSUNIT_DECLARE_StripedSet_boost_container \
    CDSUNIT_DECLARE_StripedSet_boost_flat_container
#define CDSUNIT_DEFINE_StripedSet( IMPL, C ) \
    CDSUNIT_DEFINE_StripedSet_common( IMPL, C ) \
    CDSUNIT_DEFINE_StripedSet_boost_container( IMPL, C ) \
    CDSUNIT_DEFINE_StripedSet_boost_flat_container( IMPL, C )
#define CDSUNIT_TEST_StripedSet \
    CDSUNIT_TEST_StripedSet_common \
    CDSUNIT_TEST_StripedSet_boost_container \
    CDSUNIT_TEST_StripedSet_boost_flat_container


#define CDSUNIT_DECLARE_RefinableSet_common \
    CDSUNIT_DECLARE_TEST(RefinableSet_list) \
    CDSUNIT_DECLARE_TEST(RefinableSet_vector) \
    CDSUNIT_DECLARE_TEST(RefinableSet_set) \
    CDSUNIT_DECLARE_TEST(RefinableSet_hashset) \
    CDSUNIT_DECLARE_TEST(RefinableSet_boost_unordered_set) \
    CDSUNIT_DECLARE_TEST(RefinableSet_rational_list) \
    CDSUNIT_DECLARE_TEST(RefinableSet_rational_vector) \
    CDSUNIT_DECLARE_TEST(RefinableSet_rational_set) \
    CDSUNIT_DECLARE_TEST(RefinableSet_rational_hashset) \
    CDSUNIT_DECLARE_TEST(RefinableSet_rational_boost_unordered_set)
#define CDSUNIT_DEFINE_RefinableSet_common(IMPL, C) \
    TEST_SET(IMPL, C, RefinableSet_list) \
    TEST_SET(IMPL, C, RefinableSet_vector) \
    TEST_SET(IMPL, C, RefinableSet_set) \
    TEST_SET(IMPL, C, RefinableSet_hashset) \
    TEST_SET(IMPL, C, RefinableSet_boost_unordered_set) \
    TEST_SET(IMPL, C, RefinableSet_rational_list) \
    TEST_SET(IMPL, C, RefinableSet_rational_vector) \
    TEST_SET(IMPL, C, RefinableSet_rational_set) \
    TEST_SET(IMPL, C, RefinableSet_rational_hashset) \
    TEST_SET(IMPL, C, RefinableSet_rational_boost_unordered_set)
#define CDSUNIT_TEST_RefinableSet_common \
    CPPUNIT_TEST(RefinableSet_list) \
    CPPUNIT_TEST(RefinableSet_vector) \
    CPPUNIT_TEST(RefinableSet_set) \
    CPPUNIT_TEST(RefinableSet_hashset) \
    CPPUNIT_TEST(RefinableSet_boost_unordered_set) \
    CPPUNIT_TEST(RefinableSet_rational_list) \
    CPPUNIT_TEST(RefinableSet_rational_vector) \
    CPPUNIT_TEST(RefinableSet_rational_set) \
    CPPUNIT_TEST(RefinableSet_rational_hashset) \
    CPPUNIT_TEST(RefinableSet_rational_boost_unordered_set)

#if BOOST_VERSION >= 104800
#   define CDSUNIT_DECLARE_RefinableSet_boost_container \
    CDSUNIT_DECLARE_TEST(RefinableSet_boost_list) \
    CDSUNIT_DECLARE_TEST(RefinableSet_boost_slist) \
    CDSUNIT_DECLARE_TEST(RefinableSet_boost_vector) \
    CDSUNIT_DECLARE_TEST(RefinableSet_boost_stable_vector) \
    CDSUNIT_DECLARE_TEST(RefinableSet_boost_set) \
    CDSUNIT_DECLARE_TEST(RefinableSet_rational_boost_list) \
    CDSUNIT_DECLARE_TEST(RefinableSet_rational_boost_slist) \
    CDSUNIT_DECLARE_TEST(RefinableSet_rational_boost_vector) \
    CDSUNIT_DECLARE_TEST(RefinableSet_rational_boost_stable_vector) \
    CDSUNIT_DECLARE_TEST(RefinableSet_rational_boost_set)
#   define CDSUNIT_DEFINE_RefinableSet_boost_container( IMPL, C ) \
    TEST_SET(IMPL, C, RefinableSet_boost_list) \
    TEST_SET(IMPL, C, RefinableSet_boost_slist) \
    TEST_SET(IMPL, C, RefinableSet_boost_vector) \
    TEST_SET(IMPL, C, RefinableSet_boost_stable_vector) \
    TEST_SET(IMPL, C, RefinableSet_boost_set) \
    TEST_SET(IMPL, C, RefinableSet_rational_boost_list) \
    TEST_SET(IMPL, C, RefinableSet_rational_boost_slist) \
    TEST_SET(IMPL, C, RefinableSet_rational_boost_vector) \
    TEST_SET(IMPL, C, RefinableSet_rational_boost_stable_vector) \
    TEST_SET(IMPL, C, RefinableSet_rational_boost_set)
#   define CDSUNIT_TEST_RefinableSet_boost_container \
    CPPUNIT_TEST(RefinableSet_boost_list) \
    CPPUNIT_TEST(RefinableSet_boost_slist) \
    CPPUNIT_TEST(RefinableSet_boost_vector) \
    CPPUNIT_TEST(RefinableSet_boost_stable_vector) \
    CPPUNIT_TEST(RefinableSet_boost_set) \
    CPPUNIT_TEST(RefinableSet_rational_boost_list) \
    CPPUNIT_TEST(RefinableSet_rational_boost_slist) \
    CPPUNIT_TEST(RefinableSet_rational_boost_vector) \
    CPPUNIT_TEST(RefinableSet_rational_boost_stable_vector) \
    CPPUNIT_TEST(RefinableSet_rational_boost_set)
#else
#   define CDSUNIT_DECLARE_RefinableSet_boost_container
#   define CDSUNIT_DEFINE_RefinableSet_boost_container( IMPL, C ) \
#   define CDSUNIT_TEST_RefinableSet_boost_container
#endif

#if BOOST_VERSION >= 104800 && defined(CDS_UNIT_SET_TYPES_ENABLE_BOOST_FLAT_CONTAINERS)
#   define CDSUNIT_DECLARE_RefinableSet_boost_flat_container \
    CDSUNIT_DECLARE_TEST(RefinableSet_boost_flat_set) \
    CDSUNIT_DECLARE_TEST(RefinableSet_rational_boost_flat_set)
#   define CDSUNIT_DEFINE_RefinableSet_boost_flat_container( IMPL, C ) \
    TEST_SET(IMPL, C, RefinableSet_boost_flat_set) \
    TEST_SET(IMPL, C, RefinableSet_rational_boost_flat_set)
#   define CDSUNIT_TEST_RefinableSet_boost_flat_container \
    CPPUNIT_TEST(RefinableSet_boost_flat_set) \
    CPPUNIT_TEST(RefinableSet_rational_boost_flat_set)
#else
#   define CDSUNIT_DECLARE_RefinableSet_boost_flat_container
#   define CDSUNIT_DEFINE_RefinableSet_boost_flat_container( IMPL, C )
#   define CDSUNIT_TEST_RefinableSet_boost_flat_container
#endif

#define CDSUNIT_DECLARE_RefinableSet \
    CDSUNIT_DECLARE_RefinableSet_common \
    CDSUNIT_DECLARE_RefinableSet_boost_container \
    CDSUNIT_DECLARE_RefinableSet_boost_flat_container
#define CDSUNIT_DEFINE_RefinableSet( IMPL, C ) \
    CDSUNIT_DEFINE_RefinableSet_common( IMPL, C ) \
    CDSUNIT_DEFINE_RefinableSet_boost_container( IMPL, C ) \
    CDSUNIT_DEFINE_RefinableSet_boost_flat_container( IMPL, C )
#define CDSUNIT_TEST_RefinableSet \
    CDSUNIT_TEST_RefinableSet_common \
    CDSUNIT_TEST_RefinableSet_boost_container \
    CDSUNIT_TEST_RefinableSet_boost_flat_container

#endif // #ifndef CDSUNIT_SET_DEFS_H
