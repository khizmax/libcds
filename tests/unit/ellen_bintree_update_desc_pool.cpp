//$$CDS-header$$

#include "ellen_bintree_update_desc_pool.h"

namespace ellen_bintree_pool {
    update_desc_pool_type s_UpdateDescPool;

    bounded_update_desc_pool_type s_BoundedUpdateDescPool;

    cds::atomicity::event_counter   internal_node_counter::m_nAlloc;
    cds::atomicity::event_counter   internal_node_counter::m_nFree;
}
