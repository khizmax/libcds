/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#ifndef _CDSUNIT_MAP2_MAP_DEFS_H
#define _CDSUNIT_MAP2_MAP_DEFS_H

#define CDSUNIT_DECLARE_StdMap \
    TEST_MAP(StdMap_Spin) \
    TEST_MAP(StdHashMap_Spin)
#define CDSUNIT_TEST_StdMap \
    CPPUNIT_TEST(StdMap_Spin) \
    CPPUNIT_TEST(StdHashMap_Spin) \

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#   define CDSUNIT_DECLARE_MichaelMap_RCU_signal \
    TEST_MAP_EXTRACT(MichaelMap_RCU_SHB_cmp_stdAlloc) \
    TEST_MAP_EXTRACT(MichaelMap_RCU_SHB_less_michaelAlloc) \
    TEST_MAP_EXTRACT(MichaelMap_RCU_SHT_cmp_stdAlloc) \
    TEST_MAP_EXTRACT(MichaelMap_RCU_SHT_less_michaelAlloc) \
    TEST_MAP_EXTRACT(MichaelMap_Lazy_RCU_SHB_cmp_stdAlloc) \
    TEST_MAP_EXTRACT(MichaelMap_Lazy_RCU_SHB_less_michaelAlloc) \
    TEST_MAP_EXTRACT(MichaelMap_Lazy_RCU_SHT_cmp_stdAlloc) \
    TEST_MAP_EXTRACT(MichaelMap_Lazy_RCU_SHT_less_michaelAlloc)

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


#define CDSUNIT_DECLARE_MichaelMap  \
    TEST_MAP_EXTRACT(MichaelMap_HP_cmp_stdAlloc) \
    TEST_MAP_EXTRACT(MichaelMap_HP_less_michaelAlloc) \
    TEST_MAP_EXTRACT(MichaelMap_DHP_cmp_stdAlloc) \
    TEST_MAP_EXTRACT(MichaelMap_DHP_less_michaelAlloc) \
    TEST_MAP_EXTRACT(MichaelMap_RCU_GPI_cmp_stdAlloc) \
    TEST_MAP_EXTRACT(MichaelMap_RCU_GPI_less_michaelAlloc) \
    TEST_MAP_EXTRACT(MichaelMap_RCU_GPB_cmp_stdAlloc) \
    TEST_MAP_EXTRACT(MichaelMap_RCU_GPB_less_michaelAlloc) \
    TEST_MAP_EXTRACT(MichaelMap_RCU_GPT_cmp_stdAlloc) \
    TEST_MAP_EXTRACT(MichaelMap_RCU_GPT_less_michaelAlloc) \
    TEST_MAP_EXTRACT(MichaelMap_Lazy_HP_cmp_stdAlloc) \
    TEST_MAP_EXTRACT(MichaelMap_Lazy_HP_less_michaelAlloc) \
    TEST_MAP_EXTRACT(MichaelMap_Lazy_DHP_cmp_stdAlloc) \
    TEST_MAP_EXTRACT(MichaelMap_Lazy_DHP_less_michaelAlloc) \
    TEST_MAP_EXTRACT(MichaelMap_Lazy_RCU_GPI_cmp_stdAlloc) \
    TEST_MAP_EXTRACT(MichaelMap_Lazy_RCU_GPI_less_michaelAlloc) \
    TEST_MAP_EXTRACT(MichaelMap_Lazy_RCU_GPB_cmp_stdAlloc) \
    TEST_MAP_EXTRACT(MichaelMap_Lazy_RCU_GPB_less_michaelAlloc) \
    TEST_MAP_EXTRACT(MichaelMap_Lazy_RCU_GPT_cmp_stdAlloc) \
    TEST_MAP_EXTRACT(MichaelMap_Lazy_RCU_GPT_less_michaelAlloc)\
    CDSUNIT_DECLARE_MichaelMap_RCU_signal

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
    TEST_MAP(MichaelMap_NOGC_cmp_stdAlloc) \
    TEST_MAP(MichaelMap_NOGC_less_michaelAlloc) \
    TEST_MAP(MichaelMap_Lazy_NOGC_cmp_stdAlloc) \
    TEST_MAP(MichaelMap_Lazy_NOGC_less_michaelAlloc)

#define CDSUNIT_TEST_MichaelMap_nogc  \
    CPPUNIT_TEST(MichaelMap_NOGC_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelMap_NOGC_less_michaelAlloc) \
    CPPUNIT_TEST(MichaelMap_Lazy_NOGC_cmp_stdAlloc) \
    CPPUNIT_TEST(MichaelMap_Lazy_NOGC_less_michaelAlloc) \

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#   define CDSUNIT_DECLARE_SplitList_RCU_signal  \
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_SHB_dyn_cmp)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_SHB_dyn_cmp_stat)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_SHB_st_cmp)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_SHB_dyn_less)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_SHB_st_less)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_SHB_st_less_stat)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_SHT_dyn_cmp)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_SHT_dyn_cmp_stat)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_SHT_st_cmp)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_SHT_dyn_less)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_SHT_st_less)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_SHT_st_less_stat)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_SHB_dyn_cmp)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_SHB_dyn_cmp_stat)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_SHB_st_cmp)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_SHB_dyn_less)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_SHB_st_less)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_SHB_st_less_stat)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_SHT_dyn_cmp)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_SHT_dyn_cmp_stat)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_SHT_st_cmp)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_SHT_dyn_less)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_SHT_st_less)\
TEST_MAP_EXTRACT(SplitList_Lazy_RCU_SHT_st_less_stat)

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

#define CDSUNIT_DECLARE_SplitList  \
    TEST_MAP_EXTRACT(SplitList_Michael_HP_dyn_cmp)\
    TEST_MAP_EXTRACT(SplitList_Michael_HP_dyn_cmp_stat)\
    TEST_MAP_EXTRACT(SplitList_Michael_HP_st_cmp)\
    TEST_MAP_EXTRACT(SplitList_Michael_HP_dyn_less)\
    TEST_MAP_EXTRACT(SplitList_Michael_HP_st_less)\
    TEST_MAP_EXTRACT(SplitList_Michael_HP_st_less_stat)\
    TEST_MAP_EXTRACT(SplitList_Michael_DHP_dyn_cmp)\
    TEST_MAP_EXTRACT(SplitList_Michael_DHP_dyn_cmp_stat)\
    TEST_MAP_EXTRACT(SplitList_Michael_DHP_st_cmp)\
    TEST_MAP_EXTRACT(SplitList_Michael_DHP_dyn_less)\
    TEST_MAP_EXTRACT(SplitList_Michael_DHP_st_less)\
    TEST_MAP_EXTRACT(SplitList_Michael_DHP_st_less_stat)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_GPI_dyn_cmp)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_GPI_dyn_cmp_stat)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_GPI_st_cmp)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_GPI_dyn_less)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_GPI_st_less)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_GPI_st_less_stat)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_GPB_dyn_cmp)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_GPB_dyn_cmp_stat)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_GPB_st_cmp)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_GPB_dyn_less)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_GPB_st_less)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_GPB_st_less_stat)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_GPT_dyn_cmp)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_GPT_dyn_cmp_stat)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_GPT_st_cmp)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_GPT_dyn_less)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_GPT_st_less)\
    TEST_MAP_EXTRACT(SplitList_Michael_RCU_GPT_st_less_stat)\
    TEST_MAP_EXTRACT(SplitList_Lazy_HP_dyn_cmp)\
    TEST_MAP_EXTRACT(SplitList_Lazy_HP_dyn_cmp_stat)\
    TEST_MAP_EXTRACT(SplitList_Lazy_HP_st_cmp)\
    TEST_MAP_EXTRACT(SplitList_Lazy_HP_dyn_less)\
    TEST_MAP_EXTRACT(SplitList_Lazy_HP_st_less)\
    TEST_MAP_EXTRACT(SplitList_Lazy_HP_st_less_stat)\
    TEST_MAP_EXTRACT(SplitList_Lazy_DHP_dyn_cmp)\
    TEST_MAP_EXTRACT(SplitList_Lazy_DHP_dyn_cmp_stat)\
    TEST_MAP_EXTRACT(SplitList_Lazy_DHP_st_cmp)\
    TEST_MAP_EXTRACT(SplitList_Lazy_DHP_dyn_less)\
    TEST_MAP_EXTRACT(SplitList_Lazy_DHP_st_less)\
    TEST_MAP_EXTRACT(SplitList_Lazy_DHP_st_less_stat)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_GPI_dyn_cmp)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_GPI_dyn_cmp_stat)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_GPI_st_cmp)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_GPI_dyn_less)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_GPI_st_less)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_GPI_st_less_stat)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_GPB_dyn_cmp)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_GPB_dyn_cmp_stat)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_GPB_st_cmp)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_GPB_dyn_less)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_GPB_st_less)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_GPB_st_less_stat)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_GPT_dyn_cmp)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_GPT_dyn_cmp_stat)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_GPT_st_cmp)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_GPT_dyn_less)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_GPT_st_less)\
    TEST_MAP_EXTRACT(SplitList_Lazy_RCU_GPT_st_less_stat)\
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
    TEST_MAP(SplitList_Michael_NOGC_dyn_cmp)\
    TEST_MAP(SplitList_Michael_NOGC_st_cmp)\
    TEST_MAP(SplitList_Michael_NOGC_dyn_less)\
    TEST_MAP(SplitList_Michael_NOGC_st_less)\
    TEST_MAP(SplitList_Lazy_NOGC_dyn_cmp)\
    TEST_MAP(SplitList_Lazy_NOGC_st_cmp)\
    TEST_MAP(SplitList_Lazy_NOGC_dyn_less)\
    TEST_MAP(SplitList_Lazy_NOGC_st_less)

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
    TEST_MAP_NOLF_EXTRACT(SkipListMap_rcu_shb_less_pascal)\
    TEST_MAP_NOLF_EXTRACT(SkipListMap_rcu_shb_cmp_pascal_stat)\
    TEST_MAP_NOLF_EXTRACT(SkipListMap_rcu_shb_less_xorshift)\
    TEST_MAP_NOLF_EXTRACT(SkipListMap_rcu_shb_cmp_xorshift_stat)\
    TEST_MAP_NOLF_EXTRACT(SkipListMap_rcu_sht_less_pascal)\
    TEST_MAP_NOLF_EXTRACT(SkipListMap_rcu_sht_cmp_pascal_stat)\
    TEST_MAP_NOLF_EXTRACT(SkipListMap_rcu_sht_less_xorshift)\
    TEST_MAP_NOLF_EXTRACT(SkipListMap_rcu_sht_cmp_xorshift_stat)

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
    TEST_MAP_NOLF_EXTRACT(SkipListMap_hp_less_pascal)\
    TEST_MAP_NOLF_EXTRACT(SkipListMap_hp_cmp_pascal_stat)\
    TEST_MAP_NOLF_EXTRACT(SkipListMap_hp_less_xorshift)\
    TEST_MAP_NOLF_EXTRACT(SkipListMap_hp_cmp_xorshift_stat)\
    TEST_MAP_NOLF_EXTRACT(SkipListMap_dhp_less_pascal)\
    TEST_MAP_NOLF_EXTRACT(SkipListMap_dhp_cmp_pascal_stat)\
    TEST_MAP_NOLF_EXTRACT(SkipListMap_dhp_less_xorshift)\
    TEST_MAP_NOLF_EXTRACT(SkipListMap_dhp_cmp_xorshift_stat)\
    TEST_MAP_NOLF_EXTRACT(SkipListMap_rcu_gpi_less_pascal)\
    TEST_MAP_NOLF_EXTRACT(SkipListMap_rcu_gpi_cmp_pascal_stat)\
    TEST_MAP_NOLF_EXTRACT(SkipListMap_rcu_gpi_less_xorshift)\
    TEST_MAP_NOLF_EXTRACT(SkipListMap_rcu_gpi_cmp_xorshift_stat)\
    TEST_MAP_NOLF_EXTRACT(SkipListMap_rcu_gpb_less_pascal)\
    TEST_MAP_NOLF_EXTRACT(SkipListMap_rcu_gpb_cmp_pascal_stat)\
    TEST_MAP_NOLF_EXTRACT(SkipListMap_rcu_gpb_less_xorshift)\
    TEST_MAP_NOLF_EXTRACT(SkipListMap_rcu_gpb_cmp_xorshift_stat)\
    TEST_MAP_NOLF_EXTRACT(SkipListMap_rcu_gpt_less_pascal)\
    TEST_MAP_NOLF_EXTRACT(SkipListMap_rcu_gpt_cmp_pascal_stat)\
    TEST_MAP_NOLF_EXTRACT(SkipListMap_rcu_gpt_less_xorshift)\
    TEST_MAP_NOLF_EXTRACT(SkipListMap_rcu_gpt_cmp_xorshift_stat)\
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
    TEST_MAP_NOLF(SkipListMap_nogc_less_pascal)\
    TEST_MAP_NOLF(SkipListMap_nogc_cmp_pascal_stat)\
    TEST_MAP_NOLF(SkipListMap_nogc_less_xorshift)\
    TEST_MAP_NOLF(SkipListMap_nogc_cmp_xorshift_stat)

#define CDSUNIT_TEST_SkipListMap_nogc \
    CPPUNIT_TEST(SkipListMap_nogc_less_pascal)\
    CPPUNIT_TEST(SkipListMap_nogc_cmp_pascal_stat)\
    CPPUNIT_TEST(SkipListMap_nogc_less_xorshift)\
    CPPUNIT_TEST(SkipListMap_nogc_cmp_xorshift_stat)

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
#   define CDSUNIT_DECLARE_EllenBinTreeMap_RCU_signal \
    TEST_MAP_NOLF_EXTRACT(EllenBinTreeMap_rcu_shb)\
    TEST_MAP_NOLF_EXTRACT(EllenBinTreeMap_rcu_shb_stat)\
    TEST_MAP_NOLF_EXTRACT(EllenBinTreeMap_rcu_sht)\
    TEST_MAP_NOLF_EXTRACT(EllenBinTreeMap_rcu_sht_stat)

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
    TEST_MAP_NOLF_EXTRACT(EllenBinTreeMap_hp)\
    TEST_MAP_NOLF_EXTRACT(EllenBinTreeMap_hp_yield)\
    TEST_MAP_NOLF_EXTRACT(EllenBinTreeMap_hp_stat)\
    TEST_MAP_NOLF_EXTRACT(EllenBinTreeMap_dhp)\
    TEST_MAP_NOLF_EXTRACT(EllenBinTreeMap_dhp_yield)\
    TEST_MAP_NOLF_EXTRACT(EllenBinTreeMap_dhp_stat)\
    TEST_MAP_NOLF_EXTRACT(EllenBinTreeMap_rcu_gpi)\
    TEST_MAP_NOLF_EXTRACT(EllenBinTreeMap_rcu_gpi_stat)\
    TEST_MAP_NOLF_EXTRACT(EllenBinTreeMap_rcu_gpb)\
    TEST_MAP_NOLF_EXTRACT(EllenBinTreeMap_rcu_gpb_yield)\
    TEST_MAP_NOLF_EXTRACT(EllenBinTreeMap_rcu_gpb_stat)\
    TEST_MAP_NOLF_EXTRACT(EllenBinTreeMap_rcu_gpt)\
    TEST_MAP_NOLF_EXTRACT(EllenBinTreeMap_rcu_gpt_stat)\
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


#define CDSUNIT_DECLARE_StripedMap_common \
    TEST_MAP(StripedMap_list) \
    TEST_MAP(StripedMap_map) \
    TEST_MAP(StripedMap_hashmap) \
    TEST_MAP(StripedMap_boost_unordered_map)

#define CDSUNIT_TEST_StripedMap_common \
    CPPUNIT_TEST(StripedMap_list) \
    CPPUNIT_TEST(StripedMap_map) \
    CPPUNIT_TEST(StripedMap_hashmap) \
    CPPUNIT_TEST(StripedMap_boost_unordered_map)

#if BOOST_VERSION >= 104800
#   define CDSUNIT_DECLARE_StripedMap_boost_container \
        TEST_MAP(StripedMap_boost_list) \
        TEST_MAP(StripedMap_slist) \
        TEST_MAP(StripedMap_boost_map)
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
        TEST_MAP(StripedMap_boost_flat_map)
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



#define CDSUNIT_DECLARE_RefinableMap_common \
    TEST_MAP(RefinableMap_list) \
    TEST_MAP(RefinableMap_map) \
    TEST_MAP(RefinableMap_hashmap) \
    TEST_MAP(RefinableMap_boost_unordered_map)
#define CDSUNIT_TEST_RefinableMap_common \
    CPPUNIT_TEST(RefinableMap_list) \
    CPPUNIT_TEST(RefinableMap_map) \
    CPPUNIT_TEST(RefinableMap_hashmap) \
    CPPUNIT_TEST(RefinableMap_boost_unordered_map)

#if BOOST_VERSION >= 104800
#   define CDSUNIT_DECLARE_RefinableMap_boost_container \
        TEST_MAP(RefinableMap_boost_list) \
        TEST_MAP(RefinableMap_slist) \
        TEST_MAP(RefinableMap_boost_map)
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
        TEST_MAP(RefinableMap_boost_flat_map)
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

#define CDSUNIT_DECLARE_CuckooMap \
    TEST_MAP(CuckooStripedMap_list_unord)\
    TEST_MAP(CuckooStripedMap_list_ord)\
    TEST_MAP(CuckooStripedMap_vector_unord)\
    TEST_MAP(CuckooStripedMap_vector_ord)\
    TEST_MAP(CuckooStripedMap_list_unord_stat)\
    TEST_MAP(CuckooStripedMap_list_ord_stat)\
    TEST_MAP(CuckooStripedMap_vector_unord_stat)\
    TEST_MAP(CuckooStripedMap_vector_ord_stat)\
    TEST_MAP(CuckooRefinableMap_list_unord)\
    TEST_MAP(CuckooRefinableMap_list_ord)\
    TEST_MAP(CuckooRefinableMap_vector_unord)\
    TEST_MAP(CuckooRefinableMap_vector_ord) \
    TEST_MAP(CuckooRefinableMap_list_unord_stat)\
    TEST_MAP(CuckooRefinableMap_list_ord_stat)\
    TEST_MAP(CuckooRefinableMap_vector_unord_stat)\
    TEST_MAP(CuckooRefinableMap_vector_ord_stat) \
    TEST_MAP(CuckooStripedMap_list_unord_storehash)\
    TEST_MAP(CuckooStripedMap_list_ord_storehash)\
    TEST_MAP(CuckooStripedMap_vector_unord_storehash)\
    TEST_MAP(CuckooStripedMap_vector_ord_storehash)\
    TEST_MAP(CuckooRefinableMap_list_unord_storehash)\
    TEST_MAP(CuckooRefinableMap_list_ord_storehash)\
    TEST_MAP(CuckooRefinableMap_vector_unord_storehash)\
    TEST_MAP(CuckooRefinableMap_vector_ord_storehash)

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

#endif // #ifndef _CDSUNIT_MAP2_MAP_DEFS_H
