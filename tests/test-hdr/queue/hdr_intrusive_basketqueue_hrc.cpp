//$$CDS-header$$

#include "hdr_intrusive_msqueue.h"

#include "hdr_intrusive_basketqueue_node.h"
#include <cds/gc/hrc.h>

namespace queue {

#define TEST(X) void IntrusiveQueueHeaderTest::test_##X() { test_basket<X>(); }

    namespace {
        // HRC base hook
        typedef ci::BasketQueue< cds::gc::HRC,
            basket_queue::base_hook_item<cds::gc::HRC>
            ,ci::opt::hook<
                ci::basket_queue::base_hook< ci::opt::gc<cds::gc::HRC> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > BasketQueue_HRC_base;

        /// HRC base hook + item counter
        typedef ci::BasketQueue< cds::gc::HRC,
            basket_queue::base_hook_item<cds::gc::HRC>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::basket_queue::base_hook< ci::opt::gc<cds::gc::HRC> >
            >
            ,co::item_counter< cds::atomicity::item_counter >
            ,co::memory_model< co::v::relaxed_ordering >
        > BasketQueue_HRC_base_ic;

        // HRC base hook + stat
        typedef ci::BasketQueue< cds::gc::HRC,
            basket_queue::base_hook_item<cds::gc::HRC>
            ,ci::opt::hook<
                ci::basket_queue::base_hook< ci::opt::gc<cds::gc::HRC> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::stat< ci::queue_stat<> >
        > BasketQueue_HRC_base_stat;

        // HRC base hook + alignment
        typedef ci::BasketQueue< cds::gc::HRC,
            basket_queue::base_hook_item<cds::gc::HRC>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::basket_queue::base_hook< ci::opt::gc<cds::gc::HRC> >
            >
            ,co::alignment< 32 >
        > BasketQueue_HRC_base_align;

    }

    TEST(BasketQueue_HRC_base)
    TEST(BasketQueue_HRC_base_ic)
    TEST(BasketQueue_HRC_base_stat)
    TEST(BasketQueue_HRC_base_align)
}
