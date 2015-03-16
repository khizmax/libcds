//$$CDS-header$$

#include "tree/hdr_intrusive_bintree.h"
#include "tree/hdr_intrusive_ellen_bintree_pool_rcu.h"
#include "tree/hdr_intrusive_ellen_bintree_pool_hp.h"
#include "tree/hdr_intrusive_ellen_bintree_pool_dhp.h"

#include "tree/hdr_ellenbintree_set.h"
#include "tree/hdr_ellenbintree_map.h"
#include "tree/hdr_bronson_avltree_map.h"

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
CPPUNIT_TEST_SUITE_REGISTRATION_( tree::BronsonAVLTreeHdrTest, s_BronsonAVLTreeHdrTest );
