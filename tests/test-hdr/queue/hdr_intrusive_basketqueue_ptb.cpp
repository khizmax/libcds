//$$CDS-header$$

#include "hdr_intrusive_msqueue.h"

#include "hdr_intrusive_basketqueue_node.h"
#include <cds/gc/ptb.h>

namespace queue {

#define TEST(X) void IntrusiveQueueHeaderTest::test_##X() { test_basket<X>(); }

    namespace {
        // PTB base hook
        typedef ci::BasketQueue< cds::gc::PTB,
            basket_queue::base_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::basket_queue::base_hook< ci::opt::gc<cds::gc::PTB> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > BasketQueue_PTB_base;

        // PTB member hook
        typedef ci::BasketQueue< cds::gc::PTB,
            basket_queue::member_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::basket_queue::member_hook<
                    offsetof(basket_queue::member_hook_item<cds::gc::PTB>, hMember),
                    ci::opt::gc<cds::gc::PTB>
                >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > BasketQueue_PTB_member;

        /// PTB base hook + item counter
        typedef ci::BasketQueue< cds::gc::PTB,
            basket_queue::base_hook_item<cds::gc::PTB>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::basket_queue::base_hook< ci::opt::gc<cds::gc::PTB> >
            >
            ,co::item_counter< cds::atomicity::item_counter >
            ,co::memory_model< co::v::relaxed_ordering >
        > BasketQueue_PTB_base_ic;

        // PTB member hook + item counter
        typedef ci::BasketQueue< cds::gc::PTB,
            basket_queue::member_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::basket_queue::member_hook<
                    offsetof(basket_queue::member_hook_item<cds::gc::PTB>, hMember),
                    ci::opt::gc<cds::gc::PTB>
                >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::item_counter< cds::atomicity::item_counter >
       > BasketQueue_PTB_member_ic;

        // PTB base hook + stat
        typedef ci::BasketQueue< cds::gc::PTB,
            basket_queue::base_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::basket_queue::base_hook< ci::opt::gc<cds::gc::PTB> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::stat< ci::queue_stat<> >
        > BasketQueue_PTB_base_stat;

        // PTB member hook + stat
        typedef ci::BasketQueue< cds::gc::PTB,
            basket_queue::member_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::basket_queue::member_hook<
                    offsetof(basket_queue::member_hook_item<cds::gc::PTB>, hMember),
                    ci::opt::gc<cds::gc::PTB>
                >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::stat< ci::queue_stat<> >
        > BasketQueue_PTB_member_stat;

        // PTB base hook + alignment
        typedef ci::BasketQueue< cds::gc::PTB,
            basket_queue::base_hook_item<cds::gc::PTB>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::basket_queue::base_hook< ci::opt::gc<cds::gc::PTB> >
            >
            ,co::alignment< 32 >
        > BasketQueue_PTB_base_align;

        // PTB member hook + alignment
        typedef ci::BasketQueue< cds::gc::PTB,
            basket_queue::member_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::basket_queue::member_hook<
                    offsetof(basket_queue::member_hook_item<cds::gc::PTB>, hMember),
                    ci::opt::gc<cds::gc::PTB>
                >
            >
            ,co::alignment< 32 >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > BasketQueue_PTB_member_align;

        // PTB base hook + no alignment
        typedef ci::BasketQueue< cds::gc::PTB,
            basket_queue::base_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::basket_queue::base_hook< ci::opt::gc<cds::gc::PTB> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::alignment< co::no_special_alignment >
        > BasketQueue_PTB_base_noalign;

        // PTB member hook + no alignment
        typedef ci::BasketQueue< cds::gc::PTB,
            basket_queue::member_hook_item<cds::gc::PTB>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::basket_queue::member_hook<
                    offsetof(basket_queue::member_hook_item<cds::gc::PTB>, hMember),
                    ci::opt::gc<cds::gc::PTB>
                >
            >
            ,co::alignment< co::no_special_alignment >
        > BasketQueue_PTB_member_noalign;


        // PTB base hook + cache alignment
        typedef ci::BasketQueue< cds::gc::PTB,
            basket_queue::base_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::basket_queue::base_hook< ci::opt::gc<cds::gc::PTB> >
            >
            ,co::alignment< co::cache_line_alignment >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > BasketQueue_PTB_base_cachealign;

        // PTB member hook + cache alignment
        typedef ci::BasketQueue< cds::gc::PTB,
            basket_queue::member_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::basket_queue::member_hook<
                    offsetof(basket_queue::member_hook_item<cds::gc::PTB>, hMember),
                    ci::opt::gc<cds::gc::PTB>
                >
            >
            ,co::alignment< co::cache_line_alignment >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > BasketQueue_PTB_member_cachealign;

    }

    TEST(BasketQueue_PTB_base)
    TEST(BasketQueue_PTB_member)
    TEST(BasketQueue_PTB_base_ic)
    TEST(BasketQueue_PTB_member_ic)
    TEST(BasketQueue_PTB_base_stat)
    TEST(BasketQueue_PTB_member_stat)
    TEST(BasketQueue_PTB_base_align)
    TEST(BasketQueue_PTB_member_align)
    TEST(BasketQueue_PTB_base_noalign)
    TEST(BasketQueue_PTB_member_noalign)
    TEST(BasketQueue_PTB_base_cachealign)
    TEST(BasketQueue_PTB_member_cachealign)
}
