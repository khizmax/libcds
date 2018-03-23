// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "test_skiplist_hp.h"

#include <cds/container/skip_list_map_hp.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::HP gc_type;

    class SkipListMap_HP : public cds_test::skiplist_map_hp
    {
    protected:
        typedef cds_test::skiplist_map_hp base_class;

        void SetUp()
        {
            typedef cc::SkipListMap< gc_type, key_type, value_type > map_type;

            // +1 - for guarded_ptr
            cds::gc::hp::GarbageCollector::Construct( map_type::c_nHazardPtrCount + 1, 1, 16 );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::hp::GarbageCollector::Destruct( true );
        }
    };

#   define CDSTEST_FIXTURE_NAME SkipListMap_HP
#   include "skiplist_hp_inl.h"

} // namespace
