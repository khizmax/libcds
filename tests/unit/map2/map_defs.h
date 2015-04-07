//$$CDS-header$$

#ifndef CDSUNIT_MAP_DEFS_H
#define CDSUNIT_MAP_DEFS_H

#define CDSUNIT_DECLARE_StdMap \
    CDSUNIT_DECLARE_TEST(StdMap_Spin) \
    CDSUNIT_DECLARE_TEST(StdHashMap_Spin)
#define CDSUNIT_DEFINE_StdMap(IMPL, C) \
    TEST_MAP(IMPL, C, StdMap_Spin) \
    TEST_MAP(IMPL, C, StdHashMap_Spin)
#define CDSUNIT_TEST_StdMap \
    CPPUNIT_TEST(StdMap_Spin) \
    CPPUNIT_TEST(StdHashMap_Spin) \

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#   define CDSUNIT_DECLARE_MichaelMap_RCU_signal \
    CDSUNIT_DECLARE_TEST(MichaelMap_RCU_SHB_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_RCU_SHB_less_michaelAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_RCU_SHT_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_RCU_SHT_less_michaelAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_Lazy_RCU_SHB_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_Lazy_RCU_SHB_less_michaelAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_Lazy_RCU_SHT_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_Lazy_RCU_SHT_less_michaelAlloc)

#   define CDSUNIT_DEFINE_MichaelMap_RCU_signal(IMPL, C) \
    TEST_MAP_EXTRACT(IMPL, C, MichaelMap_RCU_SHB_cmp_stdAlloc) \
    TEST_MAP_EXTRACT(IMPL, C, MichaelMap_RCU_SHB_less_michaelAlloc) \
    TEST_MAP_EXTRACT(IMPL, C, MichaelMap_RCU_SHT_cmp_stdAlloc) \
    TEST_MAP_EXTRACT(IMPL, C, MichaelMap_RCU_SHT_less_michaelAlloc) \
    TEST_MAP_EXTRACT(IMPL, C, MichaelMap_Lazy_RCU_SHB_cmp_stdAlloc) \
    TEST_MAP_EXTRACT(IMPL, C, MichaelMap_Lazy_RCU_SHB_less_michaelAlloc) \
    TEST_MAP_EXTRACT(IMPL, C, MichaelMap_Lazy_RCU_SHT_cmp_stdAlloc) \
    TEST_MAP_EXTRACT(IMPL, C, MichaelMap_Lazy_RCU_SHT_less_michaelAlloc)

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
#   define CDSUNIT_DEFINE_MichaelMap_RCU_signal(IMPL, C)
#   define CDSUNIT_TEST_MichaelMap_RCU_signal
#endif


#define CDSUNIT_DECLARE_MichaelMap  \
    CDSUNIT_DECLARE_TEST(MichaelMap_HP_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_HP_less_michaelAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_DHP_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_DHP_less_michaelAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_RCU_GPI_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_RCU_GPI_less_michaelAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_RCU_GPB_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_RCU_GPB_less_michaelAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_RCU_GPT_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_RCU_GPT_less_michaelAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_Lazy_HP_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_Lazy_HP_less_michaelAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_Lazy_DHP_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_Lazy_DHP_less_michaelAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_Lazy_RCU_GPI_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_Lazy_RCU_GPI_less_michaelAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_Lazy_RCU_GPB_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_Lazy_RCU_GPB_less_michaelAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_Lazy_RCU_GPT_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_Lazy_RCU_GPT_less_michaelAlloc)\
    CDSUNIT_DECLARE_MichaelMap_RCU_signal

#define CDSUNIT_DEFINE_MichaelMap( IMPL, C )  \
    TEST_MAP_EXTRACT(IMPL, C, MichaelMap_HP_cmp_stdAlloc) \
    TEST_MAP_EXTRACT(IMPL, C, MichaelMap_HP_less_michaelAlloc) \
    TEST_MAP_EXTRACT(IMPL, C, MichaelMap_DHP_cmp_stdAlloc) \
    TEST_MAP_EXTRACT(IMPL, C, MichaelMap_DHP_less_michaelAlloc) \
    TEST_MAP_EXTRACT(IMPL, C, MichaelMap_RCU_GPI_cmp_stdAlloc) \
    TEST_MAP_EXTRACT(IMPL, C, MichaelMap_RCU_GPI_less_michaelAlloc) \
    TEST_MAP_EXTRACT(IMPL, C, MichaelMap_RCU_GPB_cmp_stdAlloc) \
    TEST_MAP_EXTRACT(IMPL, C, MichaelMap_RCU_GPB_less_michaelAlloc) \
    TEST_MAP_EXTRACT(IMPL, C, MichaelMap_RCU_GPT_cmp_stdAlloc) \
    TEST_MAP_EXTRACT(IMPL, C, MichaelMap_RCU_GPT_less_michaelAlloc) \
    TEST_MAP_EXTRACT(IMPL, C, MichaelMap_Lazy_HP_cmp_stdAlloc) \
    TEST_MAP_EXTRACT(IMPL, C, MichaelMap_Lazy_HP_less_michaelAlloc) \
    TEST_MAP_EXTRACT(IMPL, C, MichaelMap_Lazy_DHP_cmp_stdAlloc) \
    TEST_MAP_EXTRACT(IMPL, C, MichaelMap_Lazy_DHP_less_michaelAlloc) \
    TEST_MAP_EXTRACT(IMPL, C, MichaelMap_Lazy_RCU_GPI_cmp_stdAlloc) \
    TEST_MAP_EXTRACT(IMPL, C, MichaelMap_Lazy_RCU_GPI_less_michaelAlloc) \
    TEST_MAP_EXTRACT(IMPL, C, MichaelMap_Lazy_RCU_GPB_cmp_stdAlloc) \
    TEST_MAP_EXTRACT(IMPL, C, MichaelMap_Lazy_RCU_GPB_less_michaelAlloc) \
    TEST_MAP_EXTRACT(IMPL, C, MichaelMap_Lazy_RCU_GPT_cmp_stdAlloc) \
    TEST_MAP_EXTRACT(IMPL, C, MichaelMap_Lazy_RCU_GPT_less_michaelAlloc)\
    CDSUNIT_DEFINE_MichaelMap_RCU_signal( IMPL, C )

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

#define CDSUNIT_DECLARE_MichaelMap_nogc  \
    CDSUNIT_DECLARE_TEST(MichaelMap_NOGC_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_NOGC_less_michaelAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_Lazy_NOGC_cmp_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_Lazy_NOGC_unord_stdAlloc) \
    CDSUNIT_DECLARE_TEST(MichaelMap_Lazy_NOGC_less_michaelAlloc)

#define CDSUNIT_DEFINE_MichaelMap_nogc( IMPL, C ) \
    TEST_MAP(IMPL, C, MichaelMap_NOGC_cmp_stdAlloc) \
    TEST_MAP(IMPL, C, MichaelMap_NOGC_less_michaelAlloc) \
    TEST_MAP(IMPL, C, MichaelMap_Lazy_NOGC_cmp_stdAlloc) \
    TEST_MAP(IMPL, C, MichaelMap_Lazy_NOGC_unord_stdAlloc) \
    TEST_MAP(IMPL, C, MichaelMap_Lazy_NOGC_less_michaelAlloc)

#define CDSUNIT_TEST_MichaelMap_nogc  \
    CPPUNIT_TEST(MichaelMap_NOGC_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelMap_NOGC_less_michaelAlloc) \
    CPPUNIT_TEST(MichaelMap_Lazy_NOGC_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelMap_Lazy_NOGC_unord_stdAlloc) \
    CPPUNIT_TEST(MichaelMap_Lazy_NOGC_less_michaelAlloc) \

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
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_SHB_dyn_cmp)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_SHB_dyn_cmp_stat)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_SHB_st_cmp)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_SHB_dyn_less)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_SHB_st_less)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_SHB_st_less_stat)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_SHT_dyn_cmp)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_SHT_dyn_cmp_stat)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_SHT_st_cmp)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_SHT_dyn_less)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_SHT_st_less)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_SHT_st_less_stat)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_SHB_dyn_cmp)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_SHB_dyn_cmp_stat)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_SHB_st_cmp)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_SHB_dyn_less)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_SHB_st_less)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_SHB_st_less_stat)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_SHT_dyn_cmp)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_SHT_dyn_cmp_stat)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_SHT_st_cmp)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_SHT_dyn_less)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_SHT_st_less)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_SHT_st_less_stat)

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
#   define CDSUNIT_DEFINE_SplitList_RCU_signal( IMPL, C )
#   define CDSUNIT_TEST_SplitList_RCU_signal
#endif

#define CDSUNIT_DECLARE_SplitList \
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
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_HP_dyn_cmp)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_HP_dyn_cmp_stat)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_HP_st_cmp)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_HP_dyn_less)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_HP_st_less)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_HP_st_less_stat)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_DHP_dyn_cmp)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_DHP_dyn_cmp_stat)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_DHP_st_cmp)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_DHP_dyn_less)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_DHP_st_less)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_DHP_st_less_stat)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_GPI_dyn_cmp)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_GPI_dyn_cmp_stat)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_GPI_st_cmp)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_GPI_dyn_less)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_GPI_st_less)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_GPI_st_less_stat)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_GPB_dyn_cmp)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_GPB_dyn_cmp_stat)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_GPB_st_cmp)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_GPB_dyn_less)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_GPB_st_less)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_GPB_st_less_stat)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_GPT_dyn_cmp)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_GPT_dyn_cmp_stat)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_GPT_st_cmp)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_GPT_dyn_less)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_GPT_st_less)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Michael_RCU_GPT_st_less_stat)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_HP_dyn_cmp)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_HP_dyn_cmp_stat)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_HP_st_cmp)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_HP_dyn_less)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_HP_st_less)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_HP_st_less_stat)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_DHP_dyn_cmp)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_DHP_dyn_cmp_stat)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_DHP_st_cmp)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_DHP_dyn_less)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_DHP_st_less)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_DHP_st_less_stat)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_GPI_dyn_cmp)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_GPI_dyn_cmp_stat)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_GPI_st_cmp)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_GPI_dyn_less)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_GPI_st_less)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_GPI_st_less_stat)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_GPB_dyn_cmp)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_GPB_dyn_cmp_stat)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_GPB_st_cmp)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_GPB_dyn_less)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_GPB_st_less)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_GPB_st_less_stat)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_GPT_dyn_cmp)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_GPT_dyn_cmp_stat)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_GPT_st_cmp)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_GPT_dyn_less)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_GPT_st_less)\
    TEST_MAP_EXTRACT(IMPL, C, SplitList_Lazy_RCU_GPT_st_less_stat)\
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
    CDSUNIT_DECLARE_TEST(SplitList_Michael_NOGC_dyn_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_NOGC_st_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_NOGC_dyn_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Michael_NOGC_st_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_NOGC_dyn_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_NOGC_st_cmp)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_NOGC_dyn_less)\
    CDSUNIT_DECLARE_TEST(SplitList_Lazy_NOGC_st_less)

#define CDSUNIT_DEFINE_SplitList_nogc( IMPL, C )  \
    TEST_MAP(IMPL, C, SplitList_Michael_NOGC_dyn_cmp)\
    TEST_MAP(IMPL, C, SplitList_Michael_NOGC_st_cmp)\
    TEST_MAP(IMPL, C, SplitList_Michael_NOGC_dyn_less)\
    TEST_MAP(IMPL, C, SplitList_Michael_NOGC_st_less)\
    TEST_MAP(IMPL, C, SplitList_Lazy_NOGC_dyn_cmp)\
    TEST_MAP(IMPL, C, SplitList_Lazy_NOGC_st_cmp)\
    TEST_MAP(IMPL, C, SplitList_Lazy_NOGC_dyn_less)\
    TEST_MAP(IMPL, C, SplitList_Lazy_NOGC_st_less)

#define CDSUNIT_TEST_SplitList_nogc  \
    CPPUNIT_TEST(SplitList_Michael_NOGC_dyn_cmp)\
    CPPUNIT_TEST(SplitList_Michael_NOGC_st_cmp)\
    CPPUNIT_TEST(SplitList_Michael_NOGC_dyn_less)\
    CPPUNIT_TEST(SplitList_Michael_NOGC_st_less)\
    CPPUNIT_TEST(SplitList_Lazy_NOGC_dyn_cmp)\
    CPPUNIT_TEST(SplitList_Lazy_NOGC_st_cmp)\
    CPPUNIT_TEST(SplitList_Lazy_NOGC_dyn_less)\
    CPPUNIT_TEST(SplitList_Lazy_NOGC_st_less)

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#   define CDSUNIT_DECLARE_SkipListMap_RCU_signal \
    CDSUNIT_DECLARE_TEST(SkipListMap_rcu_shb_less_pascal)\
    CDSUNIT_DECLARE_TEST(SkipListMap_rcu_shb_cmp_pascal_stat)\
    CDSUNIT_DECLARE_TEST(SkipListMap_rcu_shb_less_xorshift)\
    CDSUNIT_DECLARE_TEST(SkipListMap_rcu_shb_cmp_xorshift_stat)\
    CDSUNIT_DECLARE_TEST(SkipListMap_rcu_sht_less_pascal)\
    CDSUNIT_DECLARE_TEST(SkipListMap_rcu_sht_cmp_pascal_stat)\
    CDSUNIT_DECLARE_TEST(SkipListMap_rcu_sht_less_xorshift)\
    CDSUNIT_DECLARE_TEST(SkipListMap_rcu_sht_cmp_xorshift_stat)

#   define CDSUNIT_DEFINE_SkipListMap_RCU_signal( IMPL, C ) \
    TEST_MAP_NOLF_EXTRACT(IMPL, C, SkipListMap_rcu_shb_less_pascal)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, SkipListMap_rcu_shb_cmp_pascal_stat)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, SkipListMap_rcu_shb_less_xorshift)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, SkipListMap_rcu_shb_cmp_xorshift_stat)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, SkipListMap_rcu_sht_less_pascal)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, SkipListMap_rcu_sht_cmp_pascal_stat)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, SkipListMap_rcu_sht_less_xorshift)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, SkipListMap_rcu_sht_cmp_xorshift_stat)

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
#   define CDSUNIT_DEFINE_SkipListMap_RCU_signal( IMPL, C )
#   define CDSUNIT_TEST_SkipListMap_RCU_signal
#endif

#define CDSUNIT_DECLARE_SkipListMap \
    CDSUNIT_DECLARE_TEST(SkipListMap_hp_less_pascal)\
    CDSUNIT_DECLARE_TEST(SkipListMap_hp_cmp_pascal_stat)\
    CDSUNIT_DECLARE_TEST(SkipListMap_hp_less_xorshift)\
    CDSUNIT_DECLARE_TEST(SkipListMap_hp_cmp_xorshift_stat)\
    CDSUNIT_DECLARE_TEST(SkipListMap_dhp_less_pascal)\
    CDSUNIT_DECLARE_TEST(SkipListMap_dhp_cmp_pascal_stat)\
    CDSUNIT_DECLARE_TEST(SkipListMap_dhp_less_xorshift)\
    CDSUNIT_DECLARE_TEST(SkipListMap_dhp_cmp_xorshift_stat)\
    CDSUNIT_DECLARE_TEST(SkipListMap_rcu_gpi_less_pascal)\
    CDSUNIT_DECLARE_TEST(SkipListMap_rcu_gpi_cmp_pascal_stat)\
    CDSUNIT_DECLARE_TEST(SkipListMap_rcu_gpi_less_xorshift)\
    CDSUNIT_DECLARE_TEST(SkipListMap_rcu_gpi_cmp_xorshift_stat)\
    CDSUNIT_DECLARE_TEST(SkipListMap_rcu_gpb_less_pascal)\
    CDSUNIT_DECLARE_TEST(SkipListMap_rcu_gpb_cmp_pascal_stat)\
    CDSUNIT_DECLARE_TEST(SkipListMap_rcu_gpb_less_xorshift)\
    CDSUNIT_DECLARE_TEST(SkipListMap_rcu_gpb_cmp_xorshift_stat)\
    CDSUNIT_DECLARE_TEST(SkipListMap_rcu_gpt_less_pascal)\
    CDSUNIT_DECLARE_TEST(SkipListMap_rcu_gpt_cmp_pascal_stat)\
    CDSUNIT_DECLARE_TEST(SkipListMap_rcu_gpt_less_xorshift)\
    CDSUNIT_DECLARE_TEST(SkipListMap_rcu_gpt_cmp_xorshift_stat)\
    CDSUNIT_DECLARE_SkipListMap_RCU_signal

#define CDSUNIT_DEFINE_SkipListMap( IMPL, C ) \
    TEST_MAP_NOLF_EXTRACT(IMPL, C, SkipListMap_hp_less_pascal)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, SkipListMap_hp_cmp_pascal_stat)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, SkipListMap_hp_less_xorshift)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, SkipListMap_hp_cmp_xorshift_stat)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, SkipListMap_dhp_less_pascal)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, SkipListMap_dhp_cmp_pascal_stat)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, SkipListMap_dhp_less_xorshift)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, SkipListMap_dhp_cmp_xorshift_stat)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, SkipListMap_rcu_gpi_less_pascal)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, SkipListMap_rcu_gpi_cmp_pascal_stat)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, SkipListMap_rcu_gpi_less_xorshift)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, SkipListMap_rcu_gpi_cmp_xorshift_stat)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, SkipListMap_rcu_gpb_less_pascal)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, SkipListMap_rcu_gpb_cmp_pascal_stat)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, SkipListMap_rcu_gpb_less_xorshift)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, SkipListMap_rcu_gpb_cmp_xorshift_stat)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, SkipListMap_rcu_gpt_less_pascal)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, SkipListMap_rcu_gpt_cmp_pascal_stat)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, SkipListMap_rcu_gpt_less_xorshift)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, SkipListMap_rcu_gpt_cmp_xorshift_stat)\
    CDSUNIT_DEFINE_SkipListMap_RCU_signal( IMPL, C )

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
    CDSUNIT_DECLARE_TEST(SkipListMap_nogc_less_pascal)\
    CDSUNIT_DECLARE_TEST(SkipListMap_nogc_cmp_pascal_stat)\
    CDSUNIT_DECLARE_TEST(SkipListMap_nogc_less_xorshift)\
    CDSUNIT_DECLARE_TEST(SkipListMap_nogc_cmp_xorshift_stat)

#define CDSUNIT_DEFINE_SkipListMap_nogc( IMPL, C ) \
    TEST_MAP_NOLF(IMPL, C, SkipListMap_nogc_less_pascal)\
    TEST_MAP_NOLF(IMPL, C, SkipListMap_nogc_cmp_pascal_stat)\
    TEST_MAP_NOLF(IMPL, C, SkipListMap_nogc_less_xorshift)\
    TEST_MAP_NOLF(IMPL, C, SkipListMap_nogc_cmp_xorshift_stat)

#define CDSUNIT_TEST_SkipListMap_nogc \
    CPPUNIT_TEST(SkipListMap_nogc_less_pascal)\
    CPPUNIT_TEST(SkipListMap_nogc_cmp_pascal_stat)\
    CPPUNIT_TEST(SkipListMap_nogc_less_xorshift)\
    CPPUNIT_TEST(SkipListMap_nogc_cmp_xorshift_stat)

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#   define CDSUNIT_DECLARE_EllenBinTreeMap_RCU_signal \
    CDSUNIT_DECLARE_TEST(EllenBinTreeMap_rcu_shb)\
    CDSUNIT_DECLARE_TEST(EllenBinTreeMap_rcu_shb_stat)\
    CDSUNIT_DECLARE_TEST(EllenBinTreeMap_rcu_sht)\
    CDSUNIT_DECLARE_TEST(EllenBinTreeMap_rcu_sht_stat)

#   define CDSUNIT_DEFINE_EllenBinTreeMap_RCU_signal( IMPL, C ) \
    TEST_MAP_NOLF_EXTRACT(IMPL, C, EllenBinTreeMap_rcu_shb)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, EllenBinTreeMap_rcu_shb_stat)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, EllenBinTreeMap_rcu_sht)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, EllenBinTreeMap_rcu_sht_stat)

#   define CDSUNIT_TEST_EllenBinTreeMap_RCU_signal \
    CPPUNIT_TEST(EllenBinTreeMap_rcu_shb)\
    CPPUNIT_TEST(EllenBinTreeMap_rcu_shb_stat)\
    CPPUNIT_TEST(EllenBinTreeMap_rcu_sht)\
    CPPUNIT_TEST(EllenBinTreeMap_rcu_sht_stat)
#else
#   define CDSUNIT_DECLARE_EllenBinTreeMap_RCU_signal
#   define CDSUNIT_DEFINE_EllenBinTreeMap_RCU_signal( IMPL, C )
#   define CDSUNIT_TEST_EllenBinTreeMap_RCU_signal
#endif

#define CDSUNIT_DECLARE_EllenBinTreeMap \
    CDSUNIT_DECLARE_TEST(EllenBinTreeMap_hp)\
    CDSUNIT_DECLARE_TEST(EllenBinTreeMap_hp_yield)\
    CDSUNIT_DECLARE_TEST(EllenBinTreeMap_hp_stat)\
    CDSUNIT_DECLARE_TEST(EllenBinTreeMap_dhp)\
    CDSUNIT_DECLARE_TEST(EllenBinTreeMap_dhp_yield)\
    CDSUNIT_DECLARE_TEST(EllenBinTreeMap_dhp_stat)\
    CDSUNIT_DECLARE_TEST(EllenBinTreeMap_rcu_gpi)\
    CDSUNIT_DECLARE_TEST(EllenBinTreeMap_rcu_gpi_stat)\
    CDSUNIT_DECLARE_TEST(EllenBinTreeMap_rcu_gpb)\
    CDSUNIT_DECLARE_TEST(EllenBinTreeMap_rcu_gpb_yield)\
    CDSUNIT_DECLARE_TEST(EllenBinTreeMap_rcu_gpb_stat)\
    CDSUNIT_DECLARE_TEST(EllenBinTreeMap_rcu_gpt)\
    CDSUNIT_DECLARE_TEST(EllenBinTreeMap_rcu_gpt_stat)\
    CDSUNIT_DECLARE_EllenBinTreeMap_RCU_signal

#define CDSUNIT_DEFINE_EllenBinTreeMap( IMPL, C ) \
    TEST_MAP_NOLF_EXTRACT(IMPL, C, EllenBinTreeMap_hp)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, EllenBinTreeMap_hp_yield)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, EllenBinTreeMap_hp_stat)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, EllenBinTreeMap_dhp)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, EllenBinTreeMap_dhp_yield)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, EllenBinTreeMap_dhp_stat)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, EllenBinTreeMap_rcu_gpi)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, EllenBinTreeMap_rcu_gpi_stat)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, EllenBinTreeMap_rcu_gpb)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, EllenBinTreeMap_rcu_gpb_yield)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, EllenBinTreeMap_rcu_gpb_stat)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, EllenBinTreeMap_rcu_gpt)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, EllenBinTreeMap_rcu_gpt_stat)\
    CDSUNIT_DEFINE_EllenBinTreeMap_RCU_signal( IMPL, C )

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

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#   define CDSUNIT_DECLARE_BronsonAVLTreeMap_RCU_signal \
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_shb_less) \
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_sht_less) \
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_shb_cmp_stat) \
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_sht_cmp_stat) \
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_shb_less_pool_simple) \
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_sht_less_pool_simple) \
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_shb_less_pool_simple_stat) \
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_sht_less_pool_simple_stat) \
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_shb_less_pool_lazy) \
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_sht_less_pool_lazy) \
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_shb_less_pool_lazy_stat) \
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_sht_less_pool_lazy_stat) \
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_shb_less_pool_bounded) \
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_sht_less_pool_bounded) \
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_shb_less_pool_bounded_stat) \
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_sht_less_pool_bounded_stat) \

#   define CDSUNIT_DEFINE_BronsonAVLTreeMap_RCU_signal(IMPL, C) \
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_shb_less) \
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_sht_less) \
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_shb_cmp_stat) \
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_sht_cmp_stat) \
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_shb_less_pool_simple) \
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_sht_less_pool_simple) \
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_shb_less_pool_simple_stat) \
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_sht_less_pool_simple_stat) \
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_shb_less_pool_lazy) \
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_sht_less_pool_lazy) \
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_shb_less_pool_lazy_stat) \
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_sht_less_pool_lazy_stat) \
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_shb_less_pool_bounded) \
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_sht_less_pool_bounded) \
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_shb_less_pool_bounded_stat) \
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_sht_less_pool_bounded_stat) \

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
#   define CDSUNIT_DEFINE_BronsonAVLTreeMap_RCU_signal(IMPL, C)
#   define CDSUNIT_TEST_BronsonAVLTreeMap_RCU_signal
#endif

#define CDSUNIT_DECLARE_BronsonAVLTreeMap \
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_gpi_less)\
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_gpb_less)\
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_gpt_less)\
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_gpi_cmp_stat)\
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_gpb_cmp_stat)\
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_gpt_cmp_stat)\
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_gpi_less_pool_simple)\
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_gpb_less_pool_simple)\
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_gpt_less_pool_simple)\
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_gpi_less_pool_simple_stat)\
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_gpb_less_pool_simple_stat)\
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_gpt_less_pool_simple_stat)\
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_gpi_less_pool_lazy)\
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_gpb_less_pool_lazy)\
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_gpt_less_pool_lazy)\
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_gpi_less_pool_lazy_stat)\
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_gpb_less_pool_lazy_stat)\
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_gpt_less_pool_lazy_stat)\
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_gpi_less_pool_bounded)\
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_gpb_less_pool_bounded)\
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_gpt_less_pool_bounded)\
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_gpi_less_pool_bounded_stat)\
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_gpb_less_pool_bounded_stat)\
    CDSUNIT_DECLARE_TEST(BronsonAVLTreeMap_rcu_gpt_less_pool_bounded_stat)\
    CDSUNIT_DECLARE_BronsonAVLTreeMap_RCU_signal

#define CDSUNIT_DEFINE_BronsonAVLTreeMap( IMPL, C ) \
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_gpi_less)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_gpb_less)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_gpt_less)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_gpi_cmp_stat)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_gpb_cmp_stat)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_gpt_cmp_stat)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_gpi_less_pool_simple)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_gpb_less_pool_simple)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_gpt_less_pool_simple)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_gpi_less_pool_simple_stat)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_gpb_less_pool_simple_stat)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_gpt_less_pool_simple_stat)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_gpi_less_pool_lazy)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_gpb_less_pool_lazy)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_gpt_less_pool_lazy)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_gpi_less_pool_lazy_stat)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_gpb_less_pool_lazy_stat)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_gpt_less_pool_lazy_stat)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_gpi_less_pool_bounded)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_gpb_less_pool_bounded)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_gpt_less_pool_bounded)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_gpi_less_pool_bounded_stat)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_gpb_less_pool_bounded_stat)\
    TEST_MAP_NOLF_EXTRACT(IMPL, C, BronsonAVLTreeMap_rcu_gpt_less_pool_bounded_stat)\
    CDSUNIT_DEFINE_BronsonAVLTreeMap_RCU_signal( IMPL, C )

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

#define CDSUNIT_DECLARE_StripedMap_common \
    CDSUNIT_DECLARE_TEST(StripedMap_list) \
    CDSUNIT_DECLARE_TEST(StripedMap_map) \
    CDSUNIT_DECLARE_TEST(StripedMap_hashmap) \
    CDSUNIT_DECLARE_TEST(StripedMap_boost_unordered_map)

#define CDSUNIT_DEFINE_StripedMap_common( IMPL, C ) \
    TEST_MAP(IMPL, C, StripedMap_list) \
    TEST_MAP(IMPL, C, StripedMap_map) \
    TEST_MAP(IMPL, C, StripedMap_hashmap) \
    TEST_MAP(IMPL, C, StripedMap_boost_unordered_map)

#define CDSUNIT_TEST_StripedMap_common \
    CPPUNIT_TEST(StripedMap_list) \
    CPPUNIT_TEST(StripedMap_map) \
    CPPUNIT_TEST(StripedMap_hashmap) \
    CPPUNIT_TEST(StripedMap_boost_unordered_map)

#if BOOST_VERSION >= 104800
#   define CDSUNIT_DECLARE_StripedMap_boost_container \
        CDSUNIT_DECLARE_TEST(StripedMap_boost_list) \
        CDSUNIT_DECLARE_TEST(StripedMap_slist) \
        CDSUNIT_DECLARE_TEST(StripedMap_boost_map)
#   define CDSUNIT_DEFINE_StripedMap_boost_container( IMPL, C ) \
        TEST_MAP(IMPL, C, StripedMap_boost_list) \
        TEST_MAP(IMPL, C, StripedMap_slist) \
        TEST_MAP(IMPL, C, StripedMap_boost_map)
#   define CDSUNIT_TEST_StripedMap_boost_container \
        CPPUNIT_TEST(StripedMap_boost_list) \
        CPPUNIT_TEST(StripedMap_slist) \
        CPPUNIT_TEST(StripedMap_boost_map)
#else
#   define CDSUNIT_DECLARE_StripedMap_boost_container
#   define CDSUNIT_DEFINE_StripedMap_boost_container( IMPL, C )
#   define CDSUNIT_TEST_StripedMap_boost_container
#endif

#if BOOST_VERSION >= 104800 && defined(CDS_UNIT_MAP_TYPES_ENABLE_BOOST_FLAT_CONTAINERS)
#   define CDSUNIT_DECLARE_StripedMap_boost_flat_container \
        CDSUNIT_DECLARE_TEST(StripedMap_boost_flat_map)
#   define CDSUNIT_DEFINE_StripedMap_boost_flat_container( IMPL, C ) \
        TEST_MAP(IMPL, C, StripedMap_boost_flat_map)
#   define CDSUNIT_TEST_StripedMap_boost_flat_container \
        CPPUNIT_TEST(StripedMap_boost_flat_map)
#else
#   define CDSUNIT_DECLARE_StripedMap_boost_flat_container
#   define CDSUNIT_DEFINE_StripedMap_boost_flat_container( IMPL, C )
#   define CDSUNIT_TEST_StripedMap_boost_flat_container
#endif

#define CDSUNIT_DECLARE_StripedMap \
    CDSUNIT_DECLARE_StripedMap_common \
    CDSUNIT_DECLARE_StripedMap_boost_container \
    CDSUNIT_DECLARE_StripedMap_boost_flat_container
#define CDSUNIT_DEFINE_StripedMap( IMPL, C ) \
    CDSUNIT_DEFINE_StripedMap_common( IMPL, C ) \
    CDSUNIT_DEFINE_StripedMap_boost_container( IMPL, C ) \
    CDSUNIT_DEFINE_StripedMap_boost_flat_container( IMPL, C )
#define CDSUNIT_TEST_StripedMap \
    CDSUNIT_TEST_StripedMap_common \
    CDSUNIT_TEST_StripedMap_boost_container \
    CDSUNIT_TEST_StripedMap_boost_flat_container



#define CDSUNIT_DECLARE_RefinableMap_common \
    CDSUNIT_DECLARE_TEST(RefinableMap_list) \
    CDSUNIT_DECLARE_TEST(RefinableMap_map) \
    CDSUNIT_DECLARE_TEST(RefinableMap_hashmap) \
    CDSUNIT_DECLARE_TEST(RefinableMap_boost_unordered_map)
#define CDSUNIT_DEFINE_RefinableMap_common( IMPL, C ) \
    TEST_MAP(IMPL, C, RefinableMap_list) \
    TEST_MAP(IMPL, C, RefinableMap_map) \
    TEST_MAP(IMPL, C, RefinableMap_hashmap) \
    TEST_MAP(IMPL, C, RefinableMap_boost_unordered_map)
#define CDSUNIT_TEST_RefinableMap_common \
    CPPUNIT_TEST(RefinableMap_list) \
    CPPUNIT_TEST(RefinableMap_map) \
    CPPUNIT_TEST(RefinableMap_hashmap) \
    CPPUNIT_TEST(RefinableMap_boost_unordered_map)

#if BOOST_VERSION >= 104800
#   define CDSUNIT_DECLARE_RefinableMap_boost_container \
        CDSUNIT_DECLARE_TEST(RefinableMap_boost_list) \
        CDSUNIT_DECLARE_TEST(RefinableMap_slist) \
        CDSUNIT_DECLARE_TEST(RefinableMap_boost_map)
#   define CDSUNIT_DEFINE_RefinableMap_boost_container( IMPL, C ) \
        TEST_MAP(IMPL, C, RefinableMap_boost_list) \
        TEST_MAP(IMPL, C, RefinableMap_slist) \
        TEST_MAP(IMPL, C, RefinableMap_boost_map)
#   define CDSUNIT_TEST_RefinableMap_boost_container \
        CPPUNIT_TEST(RefinableMap_boost_list) \
        CPPUNIT_TEST(RefinableMap_slist) \
        CPPUNIT_TEST(RefinableMap_boost_map)
#else
#   define CDSUNIT_DECLARE_RefinableMap_boost_container
#   define CDSUNIT_DEFINE_RefinableMap_boost_container( IMPL, C )
#   define CDSUNIT_TEST_RefinableMap_boost_container
#endif

#if BOOST_VERSION >= 104800 && defined(CDS_UNIT_MAP_TYPES_ENABLE_BOOST_FLAT_CONTAINERS)
#   define CDSUNIT_DECLARE_RefinableMap_boost_flat_container \
        CDSUNIT_DECLARE_TEST(RefinableMap_boost_flat_map)
#   define CDSUNIT_DEFINE_RefinableMap_boost_flat_container( IMPL, C ) \
        TEST_MAP( IMPL, C, RefinableMap_boost_flat_map)
#   define CDSUNIT_TEST_RefinableMap_boost_flat_container \
        CPPUNIT_TEST(RefinableMap_boost_flat_map)
#else
#   define CDSUNIT_DECLARE_RefinableMap_boost_flat_container
#   define CDSUNIT_DEFINE_RefinableMap_boost_flat_container( IMPL, C )
#   define CDSUNIT_TEST_RefinableMap_boost_flat_container
#endif

#define CDSUNIT_DECLARE_RefinableMap \
    CDSUNIT_DECLARE_RefinableMap_common \
    CDSUNIT_DECLARE_RefinableMap_boost_container \
    CDSUNIT_DECLARE_RefinableMap_boost_flat_container
#define CDSUNIT_DEFINE_RefinableMap( IMPL, C ) \
    CDSUNIT_DEFINE_RefinableMap_common( IMPL, C ) \
    CDSUNIT_DEFINE_RefinableMap_boost_container( IMPL, C ) \
    CDSUNIT_DEFINE_RefinableMap_boost_flat_container( IMPL, C )
#define CDSUNIT_TEST_RefinableMap \
    CDSUNIT_TEST_RefinableMap_common \
    CDSUNIT_TEST_RefinableMap_boost_container \
    CDSUNIT_TEST_RefinableMap_boost_flat_container

#define CDSUNIT_DECLARE_CuckooMap \
    CDSUNIT_DECLARE_TEST(CuckooStripedMap_list_unord)\
    CDSUNIT_DECLARE_TEST(CuckooStripedMap_list_ord)\
    CDSUNIT_DECLARE_TEST(CuckooStripedMap_vector_unord)\
    CDSUNIT_DECLARE_TEST(CuckooStripedMap_vector_ord)\
    CDSUNIT_DECLARE_TEST(CuckooStripedMap_list_unord_stat)\
    CDSUNIT_DECLARE_TEST(CuckooStripedMap_list_ord_stat)\
    CDSUNIT_DECLARE_TEST(CuckooStripedMap_vector_unord_stat)\
    CDSUNIT_DECLARE_TEST(CuckooStripedMap_vector_ord_stat)\
    CDSUNIT_DECLARE_TEST(CuckooRefinableMap_list_unord)\
    CDSUNIT_DECLARE_TEST(CuckooRefinableMap_list_ord)\
    CDSUNIT_DECLARE_TEST(CuckooRefinableMap_vector_unord)\
    CDSUNIT_DECLARE_TEST(CuckooRefinableMap_vector_ord) \
    CDSUNIT_DECLARE_TEST(CuckooRefinableMap_list_unord_stat)\
    CDSUNIT_DECLARE_TEST(CuckooRefinableMap_list_ord_stat)\
    CDSUNIT_DECLARE_TEST(CuckooRefinableMap_vector_unord_stat)\
    CDSUNIT_DECLARE_TEST(CuckooRefinableMap_vector_ord_stat) \
    CDSUNIT_DECLARE_TEST(CuckooStripedMap_list_unord_storehash)\
    CDSUNIT_DECLARE_TEST(CuckooStripedMap_list_ord_storehash)\
    CDSUNIT_DECLARE_TEST(CuckooStripedMap_vector_unord_storehash)\
    CDSUNIT_DECLARE_TEST(CuckooStripedMap_vector_ord_storehash)\
    CDSUNIT_DECLARE_TEST(CuckooRefinableMap_list_unord_storehash)\
    CDSUNIT_DECLARE_TEST(CuckooRefinableMap_list_ord_storehash)\
    CDSUNIT_DECLARE_TEST(CuckooRefinableMap_vector_unord_storehash)\
    CDSUNIT_DECLARE_TEST(CuckooRefinableMap_vector_ord_storehash)

#define CDSUNIT_DEFINE_CuckooMap( IMPL, C ) \
    TEST_MAP(IMPL, C, CuckooStripedMap_list_unord)\
    TEST_MAP(IMPL, C, CuckooStripedMap_list_ord)\
    TEST_MAP(IMPL, C, CuckooStripedMap_vector_unord)\
    TEST_MAP(IMPL, C, CuckooStripedMap_vector_ord)\
    TEST_MAP(IMPL, C, CuckooStripedMap_list_unord_stat)\
    TEST_MAP(IMPL, C, CuckooStripedMap_list_ord_stat)\
    TEST_MAP(IMPL, C, CuckooStripedMap_vector_unord_stat)\
    TEST_MAP(IMPL, C, CuckooStripedMap_vector_ord_stat)\
    TEST_MAP(IMPL, C, CuckooRefinableMap_list_unord)\
    TEST_MAP(IMPL, C, CuckooRefinableMap_list_ord)\
    TEST_MAP(IMPL, C, CuckooRefinableMap_vector_unord)\
    TEST_MAP(IMPL, C, CuckooRefinableMap_vector_ord) \
    TEST_MAP(IMPL, C, CuckooRefinableMap_list_unord_stat)\
    TEST_MAP(IMPL, C, CuckooRefinableMap_list_ord_stat)\
    TEST_MAP(IMPL, C, CuckooRefinableMap_vector_unord_stat)\
    TEST_MAP(IMPL, C, CuckooRefinableMap_vector_ord_stat) \
    TEST_MAP(IMPL, C, CuckooStripedMap_list_unord_storehash)\
    TEST_MAP(IMPL, C, CuckooStripedMap_list_ord_storehash)\
    TEST_MAP(IMPL, C, CuckooStripedMap_vector_unord_storehash)\
    TEST_MAP(IMPL, C, CuckooStripedMap_vector_ord_storehash)\
    TEST_MAP(IMPL, C, CuckooRefinableMap_list_unord_storehash)\
    TEST_MAP(IMPL, C, CuckooRefinableMap_list_ord_storehash)\
    TEST_MAP(IMPL, C, CuckooRefinableMap_vector_unord_storehash)\
    TEST_MAP(IMPL, C, CuckooRefinableMap_vector_ord_storehash)

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

#endif // #ifndef CDSUNIT_MAP_DEFS_H
