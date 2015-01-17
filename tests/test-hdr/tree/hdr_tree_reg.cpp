/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#include "tree/hdr_intrusive_bintree.h"
#include "tree/hdr_intrusive_ellen_bintree_pool_rcu.h"
#include "tree/hdr_intrusive_ellen_bintree_pool_hp.h"
#include "tree/hdr_intrusive_ellen_bintree_pool_dhp.h"

#include "tree/hdr_ellenbintree_set.h"
#include "tree/hdr_ellenbintree_map.h"

namespace tree {
    namespace ellen_bintree_rcu {
        internal_node_pool_type s_InternalNodePool( 1024 * 8 );
        update_desc_pool_type   s_UpdateDescPool;
    }
    namespace ellen_bintree_hp {
        internal_node_pool_type s_InternalNodePool( 1024 * 8 );
        update_desc_pool_type   s_UpdateDescPool;
    }
    namespace ellen_bintree_dhp {
        internal_node_pool_type s_InternalNodePool( 1024 * 8 );
        update_desc_pool_type   s_UpdateDescPool;
    }
}


CPPUNIT_TEST_SUITE_REGISTRATION_(tree::IntrusiveBinTreeHdrTest, s_IntrusiveBinTreeHdrTest);
CPPUNIT_TEST_SUITE_REGISTRATION_(tree::EllenBinTreeSetHdrTest, s_EllenBinTreeSetHdrTest);
CPPUNIT_TEST_SUITE_REGISTRATION_(tree::EllenBinTreeMapHdrTest, s_EllenBinTreeMapHdrTest);
