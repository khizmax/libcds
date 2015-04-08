/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#include "set2/set_insdel_func.h"

namespace set2 {

    CPPUNIT_TEST_SUITE_REGISTRATION( Set_InsDel_func );

    size_t  Set_InsDel_func::c_nMapSize = 1000000    ;  // map size
    size_t  Set_InsDel_func::c_nInsertThreadCount = 4;  // count of insertion thread
    size_t  Set_InsDel_func::c_nDeleteThreadCount = 4;  // count of deletion thread
    size_t  Set_InsDel_func::c_nEnsureThreadCount = 4;  // count of ensure thread
    size_t  Set_InsDel_func::c_nThreadPassCount = 4  ;  // pass count for each thread
    size_t  Set_InsDel_func::c_nMaxLoadFactor = 8    ;  // maximum load factor
    bool    Set_InsDel_func::c_bPrintGCState = true;


    CPPUNIT_TEST_SUITE_PART( Set_InsDel_func, run_MichaelSet )
        CDSUNIT_TEST_MichaelSet
    CPPUNIT_TEST_SUITE_END_PART()

} // namespace set2
