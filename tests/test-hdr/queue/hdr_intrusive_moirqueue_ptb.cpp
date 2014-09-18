//$$CDS-header$$

#include "hdr_intrusive_msqueue.h"
#include "hdr_intrusive_singlelink_node.h"
#include <cds/intrusive/moir_queue.h>
#include <cds/gc/ptb.h>

namespace queue {

#define TEST(X)     void IntrusiveQueueHeaderTest::test_##X() { test<X>(); }

    namespace {

        // PTB base hook
        typedef ci::MoirQueue< cds::gc::PTB,
            base_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::PTB> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > MoirQueue_PTB_base;

        // PTB member hook
        typedef ci::MoirQueue< cds::gc::PTB,
            member_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::single_link::member_hook<
                    offsetof(member_hook_item<cds::gc::PTB>, hMember),
                    ci::opt::gc<cds::gc::PTB>
                >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > MoirQueue_PTB_member;

        /// PTB base hook + item counter
        typedef ci::MoirQueue< cds::gc::PTB,
            base_hook_item<cds::gc::PTB>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::PTB> >
            >
            ,co::item_counter< cds::atomicity::item_counter >
            ,co::memory_model< co::v::sequential_consistent >
        > MoirQueue_PTB_base_ic;

        // PTB member hook + item counter
        typedef ci::MoirQueue< cds::gc::PTB,
            member_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::single_link::member_hook<
                    offsetof(member_hook_item<cds::gc::PTB>, hMember),
                    ci::opt::gc<cds::gc::PTB>
                >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::item_counter< cds::atomicity::item_counter >
       > MoirQueue_PTB_member_ic;

        // PTB base hook + stat
        typedef ci::MoirQueue< cds::gc::PTB,
            base_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::PTB> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::stat< ci::queue_stat<> >
        > MoirQueue_PTB_base_stat;

        // PTB member hook + stat
        typedef ci::MoirQueue< cds::gc::PTB,
            member_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::single_link::member_hook<
                    offsetof(member_hook_item<cds::gc::PTB>, hMember),
                    ci::opt::gc<cds::gc::PTB>
                >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::stat< ci::queue_stat<> >
            ,co::memory_model< co::v::relaxed_ordering >
        > MoirQueue_PTB_member_stat;

        // PTB base hook + alignment
        typedef ci::MoirQueue< cds::gc::PTB,
            base_hook_item<cds::gc::PTB>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::PTB> >
            >
            ,co::alignment< 32 >
        > MoirQueue_PTB_base_align;

        // PTB member hook + alignment
        typedef ci::MoirQueue< cds::gc::PTB,
            member_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::single_link::member_hook<
                    offsetof(member_hook_item<cds::gc::PTB>, hMember),
                    ci::opt::gc<cds::gc::PTB>
                >
            >
            ,co::alignment< 32 >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > MoirQueue_PTB_member_align;

        // PTB base hook + no alignment
        typedef ci::MoirQueue< cds::gc::PTB,
            base_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::PTB> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::alignment< co::no_special_alignment >
        > MoirQueue_PTB_base_noalign;

        // PTB member hook + no alignment
        typedef ci::MoirQueue< cds::gc::PTB,
            member_hook_item<cds::gc::PTB>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::single_link::member_hook<
                    offsetof(member_hook_item<cds::gc::PTB>, hMember),
                    ci::opt::gc<cds::gc::PTB>
                >
            >
            ,co::alignment< co::no_special_alignment >
        > MoirQueue_PTB_member_noalign;


        // PTB base hook + cache alignment
        typedef ci::MoirQueue< cds::gc::PTB,
            base_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::PTB> >
            >
            ,co::alignment< co::cache_line_alignment >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > MoirQueue_PTB_base_cachealign;

        // PTB member hook + cache alignment
        typedef ci::MoirQueue< cds::gc::PTB,
            member_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::single_link::member_hook<
                    offsetof(member_hook_item<cds::gc::PTB>, hMember),
                    ci::opt::gc<cds::gc::PTB>
                >
            >
            ,co::alignment< co::cache_line_alignment >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > MoirQueue_PTB_member_cachealign;

    }   // namespace

    TEST(MoirQueue_PTB_base)
    TEST(MoirQueue_PTB_member)
    TEST(MoirQueue_PTB_base_ic)
    TEST(MoirQueue_PTB_member_ic)
    TEST(MoirQueue_PTB_base_stat)
    TEST(MoirQueue_PTB_member_stat)
    TEST(MoirQueue_PTB_base_align)
    TEST(MoirQueue_PTB_member_align)
    TEST(MoirQueue_PTB_base_noalign)
    TEST(MoirQueue_PTB_member_noalign)
    TEST(MoirQueue_PTB_base_cachealign)
    TEST(MoirQueue_PTB_member_cachealign)

} // namespace queue
