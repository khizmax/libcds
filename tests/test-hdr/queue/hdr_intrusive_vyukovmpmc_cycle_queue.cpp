//$$CDS-header$$

#include "hdr_intrusive_msqueue.h"
#include <cds/intrusive/vyukov_mpmc_cycle_queue.h>

namespace queue {

#define TEST(X)     void IntrusiveQueueHeaderTest::test_##X() { test2<X>(); }

    namespace {
        struct item {
            int nVal;
            int nDisposeCount;

            item()
                : nDisposeCount(0)
            {}
        };

        struct traits_VyukovMPMCCycleQueue_static : public ci::vyukov_queue::traits
        {
            typedef co::v::static_buffer< int, 1024 > buffer;
            typedef IntrusiveQueueHeaderTest::faked_disposer disposer;
            typedef co::v::sequential_consistent memory_model;
        };
        typedef ci::VyukovMPMCCycleQueue< item, traits_VyukovMPMCCycleQueue_static > VyukovMPMCCycleQueue_static;

        struct traits_VyukovMPMCCycleQueue_static_ic : public traits_VyukovMPMCCycleQueue_static
        {
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef ci::VyukovMPMCCycleQueue< item, traits_VyukovMPMCCycleQueue_static_ic > VyukovMPMCCycleQueue_static_ic;

        struct traits_VyukovMPMCCycleQueue_dyn :
            public ci::vyukov_queue::make_traits <
                co::buffer< co::v::dynamic_buffer< int > >,
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        {};
        class VyukovMPMCCycleQueue_dyn
            : public ci::VyukovMPMCCycleQueue< item, traits_VyukovMPMCCycleQueue_dyn >
        {
            typedef ci::VyukovMPMCCycleQueue< item, traits_VyukovMPMCCycleQueue_dyn > base_class;

        public:
            VyukovMPMCCycleQueue_dyn()
                : base_class( 1024 )
            {}
        };

        struct traits_VyukovMPMCCycleQueue_dyn_ic :
            public ci::vyukov_queue::make_traits <
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                , co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        class VyukovMPMCCycleQueue_dyn_ic
            : public ci::VyukovMPMCCycleQueue< item, traits_VyukovMPMCCycleQueue_dyn_ic >
        {
            typedef ci::VyukovMPMCCycleQueue< item, traits_VyukovMPMCCycleQueue_dyn_ic > base_class;
        public:
            VyukovMPMCCycleQueue_dyn_ic()
                : base_class( 1024 )
            {}
        };
    }

    TEST(VyukovMPMCCycleQueue_static)
    TEST(VyukovMPMCCycleQueue_static_ic)
    TEST(VyukovMPMCCycleQueue_dyn)
    TEST(VyukovMPMCCycleQueue_dyn_ic)

} // namespace queue
