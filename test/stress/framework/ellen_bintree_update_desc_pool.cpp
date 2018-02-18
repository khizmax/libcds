// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "ellen_bintree_update_desc_pool.h"

namespace ellen_bintree_pool {
    update_desc_pool_type s_UpdateDescPool;

    bounded_update_desc_pool_type s_BoundedUpdateDescPool;

    cds::atomicity::event_counter   internal_node_counter::m_nAlloc;
    cds::atomicity::event_counter   internal_node_counter::m_nFree;
}
