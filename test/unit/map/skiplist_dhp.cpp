// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_skiplist_hp.h"

#include <cds/container/skip_list_map_dhp.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::DHP gc_type;

    class SkipListMap_DHP : public cds_test::skiplist_map_hp
    {
    protected:
        typedef cds_test::skiplist_map_hp base_class;

        void SetUp()
        {
            typedef cc::SkipListMap< gc_type, key_type, value_type > map_type;

            cds::gc::dhp::smr::construct( map_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::dhp::smr::destruct();
        }
    };
#   define CDSTEST_FIXTURE_NAME SkipListMap_DHP
#   include "skiplist_hp_inl.h"

} // namespace
