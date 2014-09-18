//$$CDS-header$$

#include "hdr_intrusive_msqueue.h"
#include "hdr_intrusive_singlelink_node.h"
#include <cds/intrusive/moir_queue.h>
#include <cds/gc/hp.h>

namespace queue {

#define TEST(X) void IntrusiveQueueHeaderTest::test_##X() { test<X>(); }

    namespace {
        typedef ci::MoirQueue< cds::gc::HP,
            base_hook_item<cds::gc::HP>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > MoirQueue_HP_default;

        /// cds::gc::HP + item counter
        typedef ci::MoirQueue< cds::gc::HP,
            base_hook_item<cds::gc::HP>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::item_counter< cds::atomicity::item_counter >
        > MoirQueue_HP_default_ic;

        /// cds::gc::HP + stat
        typedef ci::MoirQueue< cds::gc::HP,
            base_hook_item<cds::gc::HP>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::stat< ci::queue_stat<> >
        > MoirQueue_HP_default_stat;

        // cds::gc::HP base hook
        typedef ci::MoirQueue< cds::gc::HP,
            base_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::HP> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > MoirQueue_HP_base;

        // cds::gc::HP member hook
        typedef ci::MoirQueue< cds::gc::HP,
            member_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::single_link::member_hook<
                    offsetof(member_hook_item<cds::gc::HP>, hMember),
                    ci::opt::gc<cds::gc::HP>
                >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > MoirQueue_HP_member;

        /// cds::gc::HP base hook + item counter
        typedef ci::MoirQueue< cds::gc::HP,
            base_hook_item<cds::gc::HP>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::HP> >
            >
            ,co::item_counter< cds::atomicity::item_counter >
        > MoirQueue_HP_base_ic;

        // cds::gc::HP member hook + item counter
        typedef ci::MoirQueue< cds::gc::HP,
            member_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::single_link::member_hook<
                    offsetof(member_hook_item<cds::gc::HP>, hMember),
                    ci::opt::gc<cds::gc::HP>
                >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::item_counter< cds::atomicity::item_counter >
       > MoirQueue_HP_member_ic;

        // cds::gc::HP base hook + stat
        typedef ci::MoirQueue< cds::gc::HP,
            base_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::HP> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::stat< ci::queue_stat<> >
        > MoirQueue_HP_base_stat;

        // cds::gc::HP member hook + stat
        typedef ci::MoirQueue< cds::gc::HP,
            member_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::single_link::member_hook<
                    offsetof(member_hook_item<cds::gc::HP>, hMember),
                    ci::opt::gc<cds::gc::HP>
                >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::stat< ci::queue_stat<> >
        > MoirQueue_HP_member_stat;

        // cds::gc::HP base hook + alignment
        typedef ci::MoirQueue< cds::gc::HP,
            base_hook_item<cds::gc::HP>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::HP> >
            >
            ,co::alignment< 32 >
        > MoirQueue_HP_base_align;

        // cds::gc::HP member hook + alignment
        typedef ci::MoirQueue< cds::gc::HP,
            member_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::single_link::member_hook<
                    offsetof(member_hook_item<cds::gc::HP>, hMember),
                    ci::opt::gc<cds::gc::HP>
                >
            >
            ,co::alignment< 32 >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > MoirQueue_HP_member_align;

        // cds::gc::HP base hook + no alignment
        typedef ci::MoirQueue< cds::gc::HP,
            base_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::HP> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::alignment< co::no_special_alignment >
        > MoirQueue_HP_base_noalign;

        // cds::gc::HP member hook + no alignment
        typedef ci::MoirQueue< cds::gc::HP,
            member_hook_item<cds::gc::HP>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::single_link::member_hook<
                    offsetof(member_hook_item<cds::gc::HP>, hMember),
                    ci::opt::gc<cds::gc::HP>
                >
            >
            ,co::alignment< co::no_special_alignment >
        > MoirQueue_HP_member_noalign;


        // cds::gc::HP base hook + cache alignment
        typedef ci::MoirQueue< cds::gc::HP,
            base_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::HP> >
            >
            ,co::alignment< co::cache_line_alignment >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > MoirQueue_HP_base_cachealign;

        // cds::gc::HP member hook + cache alignment
        typedef ci::MoirQueue< cds::gc::HP,
            member_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::single_link::member_hook<
                    offsetof(member_hook_item<cds::gc::HP>, hMember),
                    ci::opt::gc<cds::gc::HP>
                >
            >
            ,co::alignment< co::cache_line_alignment >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > MoirQueue_HP_member_cachealign;

    }

    TEST(MoirQueue_HP_default)
    TEST(MoirQueue_HP_default_ic)
    TEST(MoirQueue_HP_default_stat)
    TEST(MoirQueue_HP_base)
    TEST(MoirQueue_HP_member)
    TEST(MoirQueue_HP_base_ic)
    TEST(MoirQueue_HP_member_ic)
    TEST(MoirQueue_HP_base_stat)
    TEST(MoirQueue_HP_member_stat)
    TEST(MoirQueue_HP_base_align)
    TEST(MoirQueue_HP_member_align)
    TEST(MoirQueue_HP_base_noalign)
    TEST(MoirQueue_HP_member_noalign)
    TEST(MoirQueue_HP_base_cachealign)
    TEST(MoirQueue_HP_member_cachealign)
}
