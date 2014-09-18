//$$CDS-header$$

#include "hdr_intrusive_msqueue.h"
#include "hdr_intrusive_singlelink_node.h"

#include <cds/intrusive/msqueue.h>
#include <cds/gc/hp.h>

namespace queue {

#define TEST(X) void IntrusiveQueueHeaderTest::test_##X() { test<X>(); }

    namespace {
        typedef ci::MSQueue< cds::gc::HP,
            base_hook_item<cds::gc::HP>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > MSQueue_HP_default;

        /// HP + item counter
        typedef ci::MSQueue< cds::gc::HP,
            base_hook_item<cds::gc::HP>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::item_counter< cds::atomicity::item_counter >
            ,co::memory_model< co::v::sequential_consistent >
        > MSQueue_HP_default_ic;

        /// HP + stat
        typedef ci::MSQueue< cds::gc::HP,
            base_hook_item<cds::gc::HP>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::stat< ci::queue_stat<> >
        > MSQueue_HP_default_stat;

        // HP base hook
        typedef ci::MSQueue< cds::gc::HP,
            base_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::HP> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > MSQueue_HP_base;

        // HP member hook
        typedef ci::MSQueue< cds::gc::HP,
            member_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::single_link::member_hook<
                    offsetof(member_hook_item<cds::gc::HP>, hMember),
                    ci::opt::gc<cds::gc::HP>
                >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > MSQueue_HP_member;

        /// HP base hook + item counter
        typedef ci::MSQueue< cds::gc::HP,
            base_hook_item<cds::gc::HP>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::HP> >
            >
            ,co::item_counter< cds::atomicity::item_counter >
            ,co::memory_model< co::v::relaxed_ordering >
        > MSQueue_HP_base_ic;

        // HP member hook + item counter
        typedef ci::MSQueue< cds::gc::HP,
            member_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::single_link::member_hook<
                    offsetof(member_hook_item<cds::gc::HP>, hMember),
                    ci::opt::gc<cds::gc::HP>
                >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::item_counter< cds::atomicity::item_counter >
       > MSQueue_HP_member_ic;

        // HP base hook + stat
        typedef ci::MSQueue< cds::gc::HP,
            base_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::HP> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::stat< ci::queue_stat<> >
        > MSQueue_HP_base_stat;

        // HP member hook + stat
        typedef ci::MSQueue< cds::gc::HP,
            member_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::single_link::member_hook<
                    offsetof(member_hook_item<cds::gc::HP>, hMember),
                    ci::opt::gc<cds::gc::HP>
                >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::stat< ci::queue_stat<> >
        > MSQueue_HP_member_stat;

        // HP base hook + alignment
        typedef ci::MSQueue< cds::gc::HP,
            base_hook_item<cds::gc::HP>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::HP> >
            >
            ,co::alignment< 32 >
        > MSQueue_HP_base_align;

        // HP member hook + alignment
        typedef ci::MSQueue< cds::gc::HP,
            member_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::single_link::member_hook<
                    offsetof(member_hook_item<cds::gc::HP>, hMember),
                    ci::opt::gc<cds::gc::HP>
                >
            >
            ,co::alignment< 32 >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > MSQueue_HP_member_align;

        // HP base hook + no alignment
        typedef ci::MSQueue< cds::gc::HP,
            base_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::HP> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::alignment< co::no_special_alignment >
        > MSQueue_HP_base_noalign;

        // HP member hook + no alignment
        typedef ci::MSQueue< cds::gc::HP,
            member_hook_item<cds::gc::HP>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::single_link::member_hook<
                    offsetof(member_hook_item<cds::gc::HP>, hMember),
                    ci::opt::gc<cds::gc::HP>
                >
            >
            ,co::alignment< co::no_special_alignment >
        > MSQueue_HP_member_noalign;


        // HP base hook + cache alignment
        typedef ci::MSQueue< cds::gc::HP,
            base_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::HP> >
            >
            ,co::alignment< co::cache_line_alignment >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > MSQueue_HP_base_cachealign;

        // HP member hook + cache alignment
        typedef ci::MSQueue< cds::gc::HP,
            member_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::single_link::member_hook<
                    offsetof(member_hook_item<cds::gc::HP>, hMember),
                    ci::opt::gc<cds::gc::HP>
                >
            >
            ,co::alignment< co::cache_line_alignment >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > MSQueue_HP_member_cachealign;

    }

    TEST(MSQueue_HP_default)
    TEST(MSQueue_HP_default_ic)
    TEST(MSQueue_HP_default_stat)
    TEST(MSQueue_HP_base)
    TEST(MSQueue_HP_member)
    TEST(MSQueue_HP_base_ic)
    TEST(MSQueue_HP_member_ic)
    TEST(MSQueue_HP_base_stat)
    TEST(MSQueue_HP_member_stat)
    TEST(MSQueue_HP_base_align)
    TEST(MSQueue_HP_member_align)
    TEST(MSQueue_HP_base_noalign)
    TEST(MSQueue_HP_member_noalign)
    TEST(MSQueue_HP_base_cachealign)
    TEST(MSQueue_HP_member_cachealign)
}

CPPUNIT_TEST_SUITE_REGISTRATION(queue::IntrusiveQueueHeaderTest);
