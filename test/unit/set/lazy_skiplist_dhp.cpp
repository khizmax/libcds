#include "test_ordered_set_hp.h"

#include <cds/container/lazy_skip_list_set_dhp.h>

namespace {
    namespace cc = cds::container;
    typedef cds::gc::DHP gc_type;

    class LazySkipListSet_DHP : public cds_test::container_ordered_set_hp
    {
    protected:
        typedef cds_test::container_ordered_set_hp base_class;

        void SetUp()
        {
            typedef cc::LazySkipListSet< gc_type, int_item > set_type;

            cds::gc::dhp::smr::construct( set_type::c_nHazardPtrCount );
            cds::threading::Manager::attachThread();
        }

        void TearDown()
        {
            cds::threading::Manager::detachThread();
            cds::gc::dhp::smr::destruct();
        }
    };

#   define CDSTEST_FIXTURE_NAME LazySkipListSet_DHP
#   include "lazy_skiplist_hp_inl.h"

} // namespace
