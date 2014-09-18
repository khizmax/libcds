//$$CDS-header$$

#include "hdr_intrusive_msqueue.h"
#include "hdr_intrusive_singlelink_node.h"
#include <cds/intrusive/moir_queue.h>
#include <cds/gc/hrc.h>

namespace queue {

#define TEST(X)     void IntrusiveQueueHeaderTest::test_##X() { test<X>(); }

    namespace {

        // HRC base hook
        typedef ci::MoirQueue< cds::gc::HRC,
            base_hook_item<cds::gc::HRC>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::HRC> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > MoirQueue_HRC_base;

        // HRC base hook + item counter
        typedef ci::MoirQueue< cds::gc::HRC,
            base_hook_item<cds::gc::HRC>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::HRC> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::item_counter< cds::atomicity::item_counter >
        > MoirQueue_HRC_base_ic;

        // HRC base hook + stat
        typedef ci::MoirQueue< cds::gc::HRC,
            base_hook_item<cds::gc::HRC>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::HRC> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::stat< ci::queue_stat<> >
        > MoirQueue_HRC_base_stat;

        // HRC base hook + alignment
        typedef ci::MoirQueue< cds::gc::HRC,
            base_hook_item<cds::gc::HRC>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::HRC> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::alignment< 128 >
        > MoirQueue_HRC_base_align;
    }

    TEST(MoirQueue_HRC_base)
    TEST(MoirQueue_HRC_base_ic)
    TEST(MoirQueue_HRC_base_stat)
    TEST(MoirQueue_HRC_base_align)

} // namespace queue
