//$$CDS-header$$

#include "hdr_intrusive_msqueue.h"
#include <cds/intrusive/optimistic_queue.h>
#include <cds/gc/hp.h>

namespace queue {

#define TEST(X) void IntrusiveQueueHeaderTest::test_##X() { test<X>(); }

    namespace {
        template <typename GC>
        struct base_hook_item: public ci::optimistic_queue::node< GC >
        {
            int nVal;
            int nDisposeCount;

            base_hook_item()
                : nDisposeCount(0)
            {}
        };

        template <typename GC>
        struct member_hook_item
        {
            ci::optimistic_queue::node< GC > hMember;
            int nVal;
            int nDisposeCount;

            member_hook_item()
                : nDisposeCount(0)
            {}
        };


        typedef ci::OptimisticQueue< cds::gc::HP,
            base_hook_item<cds::gc::HP>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > OptimisticQueue_HP_default;

        /// HP + item counter
        typedef ci::OptimisticQueue< cds::gc::HP,
            base_hook_item<cds::gc::HP>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::item_counter< cds::atomicity::item_counter >
        > OptimisticQueue_HP_default_ic;

        /// HP + stat
        typedef ci::OptimisticQueue< cds::gc::HP,
            base_hook_item<cds::gc::HP>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::stat< ci::queue_stat<> >
            ,co::memory_model< co::v::sequential_consistent >
        > OptimisticQueue_HP_default_stat;

        // HP base hook
        typedef ci::OptimisticQueue< cds::gc::HP,
            base_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::HP> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::memory_model< co::v::relaxed_ordering >
        > OptimisticQueue_HP_base;

        // HP member hook
        typedef ci::OptimisticQueue< cds::gc::HP,
            member_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::optimistic_queue::member_hook<
                    offsetof(member_hook_item<cds::gc::HP>, hMember),
                    ci::opt::gc<cds::gc::HP>
                >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > OptimisticQueue_HP_member;

        /// HP base hook + item counter
        typedef ci::OptimisticQueue< cds::gc::HP,
            base_hook_item<cds::gc::HP>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::HP> >
            >
            ,co::item_counter< cds::atomicity::item_counter >
        > OptimisticQueue_HP_base_ic;

        // HP member hook + item counter
        typedef ci::OptimisticQueue< cds::gc::HP,
            member_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::optimistic_queue::member_hook<
                    offsetof(member_hook_item<cds::gc::HP>, hMember),
                    ci::opt::gc<cds::gc::HP>
                >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::item_counter< cds::atomicity::item_counter >
       > OptimisticQueue_HP_member_ic;

        // HP base hook + stat
        typedef ci::OptimisticQueue< cds::gc::HP,
            base_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::HP> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::stat< ci::queue_stat<> >
        > OptimisticQueue_HP_base_stat;

        // HP member hook + stat
        typedef ci::OptimisticQueue< cds::gc::HP,
            member_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::optimistic_queue::member_hook<
                    offsetof(member_hook_item<cds::gc::HP>, hMember),
                    ci::opt::gc<cds::gc::HP>
                >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::stat< ci::queue_stat<> >
        > OptimisticQueue_HP_member_stat;

        // HP base hook + alignment
        typedef ci::OptimisticQueue< cds::gc::HP,
            base_hook_item<cds::gc::HP>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::HP> >
            >
            ,co::alignment< 32 >
        > OptimisticQueue_HP_base_align;

        // HP member hook + alignment
        typedef ci::OptimisticQueue< cds::gc::HP,
            member_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::optimistic_queue::member_hook<
                    offsetof(member_hook_item<cds::gc::HP>, hMember),
                    ci::opt::gc<cds::gc::HP>
                >
            >
            ,co::alignment< 32 >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > OptimisticQueue_HP_member_align;

        // HP base hook + no alignment
        typedef ci::OptimisticQueue< cds::gc::HP,
            base_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::HP> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::alignment< co::no_special_alignment >
        > OptimisticQueue_HP_base_noalign;

        // HP member hook + no alignment
        typedef ci::OptimisticQueue< cds::gc::HP,
            member_hook_item<cds::gc::HP>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::optimistic_queue::member_hook<
                    offsetof(member_hook_item<cds::gc::HP>, hMember),
                    ci::opt::gc<cds::gc::HP>
                >
            >
            ,co::alignment< co::no_special_alignment >
        > OptimisticQueue_HP_member_noalign;


        // HP base hook + cache alignment
        typedef ci::OptimisticQueue< cds::gc::HP,
            base_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::HP> >
            >
            ,co::alignment< co::cache_line_alignment >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > OptimisticQueue_HP_base_cachealign;

        // HP member hook + cache alignment
        typedef ci::OptimisticQueue< cds::gc::HP,
            member_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::optimistic_queue::member_hook<
                    offsetof(member_hook_item<cds::gc::HP>, hMember),
                    ci::opt::gc<cds::gc::HP>
                >
            >
            ,co::alignment< co::cache_line_alignment >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > OptimisticQueue_HP_member_cachealign;

    }

    TEST(OptimisticQueue_HP_default)
    TEST(OptimisticQueue_HP_default_ic)
    TEST(OptimisticQueue_HP_default_stat)
    TEST(OptimisticQueue_HP_base)
    TEST(OptimisticQueue_HP_member)
    TEST(OptimisticQueue_HP_base_ic)
    TEST(OptimisticQueue_HP_member_ic)
    TEST(OptimisticQueue_HP_base_stat)
    TEST(OptimisticQueue_HP_member_stat)
    TEST(OptimisticQueue_HP_base_align)
    TEST(OptimisticQueue_HP_member_align)
    TEST(OptimisticQueue_HP_base_noalign)
    TEST(OptimisticQueue_HP_member_noalign)
    TEST(OptimisticQueue_HP_base_cachealign)
    TEST(OptimisticQueue_HP_member_cachealign)
}
