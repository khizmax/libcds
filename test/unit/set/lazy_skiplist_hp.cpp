#include "test_ordered_set_hp.h"

#include <cds/container/lazy_skip_list_set_hp.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::HP gc_type;

    class LazySkipListSet_HP: public cds_test::container_ordered_set_hp
    {
    protected:
        typedef cds_test::container_ordered_set_hp base_class;

        void SetUp()
        {
            typedef cc::LazySkipListSet< gc_type, int_item > set_type;

            // +1 - for guarded_ptr
            cds::gc::hp::GarbageCollector::Construct( set_type::c_nHazardPtrCount + 1, 1, 16 );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::hp::GarbageCollector::Destruct( true );
        }
    };

#   define CDSTEST_FIXTURE_NAME LazySkipListSet_HP
#   include "lazy_skiplist_hp_inl.h"

}
