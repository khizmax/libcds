/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#include "set2/set_insdel_func.h"
#include "set2/set_type_ellen_bintree.h"

namespace set2 {
    CDSUNIT_DEFINE_EllenBinTreeSet( cc::ellen_bintree::implementation_tag, Set_InsDel_func )

    CPPUNIT_TEST_SUITE_PART( Set_InsDel_func, run_EllenBinTreeSet )
        CDSUNIT_TEST_EllenBinTreeSet
    CPPUNIT_TEST_SUITE_END_PART()

} // namespace set2
