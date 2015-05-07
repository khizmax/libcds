/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#include "ellen_bintree_update_desc_pool.h"

namespace ellen_bintree_pool {
    update_desc_pool_type s_UpdateDescPool;

    bounded_update_desc_pool_type s_BoundedUpdateDescPool;

    cds::atomicity::event_counter   internal_node_counter::m_nAlloc;
    cds::atomicity::event_counter   internal_node_counter::m_nFree;
}
