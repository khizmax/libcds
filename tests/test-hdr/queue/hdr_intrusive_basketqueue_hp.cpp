//$$CDS-header$$

#include "hdr_intrusive_msqueue.h"

#include "hdr_intrusive_basketqueue_node.h"
#include <cds/gc/hp.h>

namespace queue {

#define TEST(X) void IntrusiveQueueHeaderTest::test_##X() { test_basket<X>(); }

    namespace {
        typedef ci::BasketQueue< cds::gc::HP,
            basket_queue::base_hook_item<cds::gc::HP>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > BasketQueue_HP_default;

        /// HP + item counter
        typedef ci::BasketQueue< cds::gc::HP,
            basket_queue::base_hook_item<cds::gc::HP>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::item_counter< cds::atomicity::item_counter >
            ,co::memory_model< co::v::sequential_consistent >
        > BasketQueue_HP_default_ic;

        /// HP + stat
        typedef ci::BasketQueue< cds::gc::HP,
            basket_queue::base_hook_item<cds::gc::HP>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::stat< ci::queue_stat<> >
        > BasketQueue_HP_default_stat;

        // HP base hook
        typedef ci::BasketQueue< cds::gc::HP,
            basket_queue::base_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::basket_queue::base_hook< ci::opt::gc<cds::gc::HP> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > BasketQueue_HP_base;

        // HP member hook
        typedef ci::BasketQueue< cds::gc::HP,
            basket_queue::member_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::basket_queue::member_hook<
                    offsetof(basket_queue::member_hook_item<cds::gc::HP>, hMember),
                    ci::opt::gc<cds::gc::HP>
                >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > BasketQueue_HP_member;

        /// HP base hook + item counter
        typedef ci::BasketQueue< cds::gc::HP,
            basket_queue::base_hook_item<cds::gc::HP>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::basket_queue::base_hook< ci::opt::gc<cds::gc::HP> >
            >
            ,co::item_counter< cds::atomicity::item_counter >
            ,co::memory_model< co::v::relaxed_ordering >
        > BasketQueue_HP_base_ic;

        // HP member hook + item counter
        typedef ci::BasketQueue< cds::gc::HP,
            basket_queue::member_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::basket_queue::member_hook<
                    offsetof(basket_queue::member_hook_item<cds::gc::HP>, hMember),
                    ci::opt::gc<cds::gc::HP>
                >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::item_counter< cds::atomicity::item_counter >
       > BasketQueue_HP_member_ic;

        // HP base hook + stat
        typedef ci::BasketQueue< cds::gc::HP,
            basket_queue::base_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::basket_queue::base_hook< ci::opt::gc<cds::gc::HP> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::stat< ci::queue_stat<> >
        > BasketQueue_HP_base_stat;

        // HP member hook + stat
        typedef ci::BasketQueue< cds::gc::HP,
            basket_queue::member_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::basket_queue::member_hook<
                    offsetof(basket_queue::member_hook_item<cds::gc::HP>, hMember),
                    ci::opt::gc<cds::gc::HP>
                >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::stat< ci::queue_stat<> >
        > BasketQueue_HP_member_stat;

        // HP base hook + alignment
        typedef ci::BasketQueue< cds::gc::HP,
            basket_queue::base_hook_item<cds::gc::HP>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::basket_queue::base_hook< ci::opt::gc<cds::gc::HP> >
            >
            ,co::alignment< 32 >
        > BasketQueue_HP_base_align;

        // HP member hook + alignment
        typedef ci::BasketQueue< cds::gc::HP,
            basket_queue::member_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::basket_queue::member_hook<
                    offsetof(basket_queue::member_hook_item<cds::gc::HP>, hMember),
                    ci::opt::gc<cds::gc::HP>
                >
            >
            ,co::alignment< 32 >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > BasketQueue_HP_member_align;

        // HP base hook + no alignment
        typedef ci::BasketQueue< cds::gc::HP,
            basket_queue::base_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::basket_queue::base_hook< ci::opt::gc<cds::gc::HP> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::alignment< co::no_special_alignment >
        > BasketQueue_HP_base_noalign;

        // HP member hook + no alignment
        typedef ci::BasketQueue< cds::gc::HP,
            basket_queue::member_hook_item<cds::gc::HP>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::basket_queue::member_hook<
                    offsetof(basket_queue::member_hook_item<cds::gc::HP>, hMember),
                    ci::opt::gc<cds::gc::HP>
                >
            >
            ,co::alignment< co::no_special_alignment >
        > BasketQueue_HP_member_noalign;


        // HP base hook + cache alignment
        typedef ci::BasketQueue< cds::gc::HP,
            basket_queue::base_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::basket_queue::base_hook< ci::opt::gc<cds::gc::HP> >
            >
            ,co::alignment< co::cache_line_alignment >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > BasketQueue_HP_base_cachealign;

        // HP member hook + cache alignment
        typedef ci::BasketQueue< cds::gc::HP,
            basket_queue::member_hook_item<cds::gc::HP>
            ,ci::opt::hook<
                ci::basket_queue::member_hook<
                    offsetof(basket_queue::member_hook_item<cds::gc::HP>, hMember),
                    ci::opt::gc<cds::gc::HP>
                >
            >
            ,co::alignment< co::cache_line_alignment >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > BasketQueue_HP_member_cachealign;

    }

    TEST(BasketQueue_HP_default)
    TEST(BasketQueue_HP_default_ic)
    TEST(BasketQueue_HP_default_stat)
    TEST(BasketQueue_HP_base)
    TEST(BasketQueue_HP_member)
    TEST(BasketQueue_HP_base_ic)
    TEST(BasketQueue_HP_member_ic)
    TEST(BasketQueue_HP_base_stat)
    TEST(BasketQueue_HP_member_stat)
    TEST(BasketQueue_HP_base_align)
    TEST(BasketQueue_HP_member_align)
    TEST(BasketQueue_HP_base_noalign)
    TEST(BasketQueue_HP_member_noalign)
    TEST(BasketQueue_HP_base_cachealign)
    TEST(BasketQueue_HP_member_cachealign)
}
