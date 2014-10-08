//$$CDS-header$$

#include "hdr_intrusive_msqueue.h"
#include <cds/gc/hp.h>
#include <cds/intrusive/basket_queue.h>

namespace queue {

#define TEST(X) void IntrusiveQueueHeaderTest::test_##X() { test_basket<X>(); }

    namespace {
        typedef IntrusiveQueueHeaderTest::base_hook_item< ci::basket_queue::node<cds::gc::HP> > base_hook_item;
        typedef IntrusiveQueueHeaderTest::member_hook_item< ci::basket_queue::node<cds::gc::HP> > member_hook_item;

        struct traits_BasketQueue_HP_default : public ci::basket_queue::traits
        {
            typedef IntrusiveQueueHeaderTest::faked_disposer disposer;
        };
        typedef ci::BasketQueue< cds::gc::HP, base_hook_item, traits_BasketQueue_HP_default > BasketQueue_HP_default;

        /// HP + item counter
        struct traits_BasketQueue_HP_default_ic : public traits_BasketQueue_HP_default
        {
            typedef cds::atomicity::item_counter item_counter;
            typedef co::v::sequential_consistent memory_model;
        };
        typedef ci::BasketQueue< cds::gc::HP, base_hook_item, traits_BasketQueue_HP_default_ic > BasketQueue_HP_default_ic;

        /// HP + stat
        typedef ci::BasketQueue< cds::gc::HP, base_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::stat< ci::basket_queue::stat<> >
            >::type
        > BasketQueue_HP_default_stat;

        // HP base hook
        struct traits_BasketQueue_HP_base : public ci::basket_queue::traits
        {
            typedef IntrusiveQueueHeaderTest::faked_disposer disposer;
            typedef ci::basket_queue::base_hook< ci::opt::gc<cds::gc::HP> > hook;
        };
        typedef ci::BasketQueue< cds::gc::HP, base_hook_item, traits_BasketQueue_HP_base > BasketQueue_HP_base;

        // HP member hook
        struct traits_BasketQueue_HP_member : public traits_BasketQueue_HP_base
        {
            typedef ci::basket_queue::member_hook <
                offsetof( member_hook_item, hMember ),
                ci::opt::gc < cds::gc::HP >
            > hook;

        };
        typedef ci::BasketQueue< cds::gc::HP, member_hook_item, traits_BasketQueue_HP_member > BasketQueue_HP_member;

        /// HP base hook + item counter
        typedef ci::BasketQueue< cds::gc::HP, base_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,ci::opt::hook<
                    ci::basket_queue::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::memory_model< co::v::relaxed_ordering >
            >::type
        > BasketQueue_HP_base_ic;

        // HP member hook + item counter
        typedef ci::BasketQueue< cds::gc::HP, member_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::hook<
                    ci::basket_queue::member_hook<
                        offsetof( member_hook_item, hMember),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
       > BasketQueue_HP_member_ic;

        // HP base hook + stat
        typedef ci::BasketQueue< cds::gc::HP, base_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::hook<
                    ci::basket_queue::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::stat< ci::basket_queue::stat<> >
            >::type
        > BasketQueue_HP_base_stat;

        // HP member hook + stat
        typedef ci::BasketQueue< cds::gc::HP, member_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::hook<
                    ci::basket_queue::member_hook<
                        offsetof(member_hook_item, hMember),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::stat< ci::basket_queue::stat<> >
            >::type
        > BasketQueue_HP_member_stat;

        // HP base hook + alignment
        typedef ci::BasketQueue< cds::gc::HP, base_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,ci::opt::hook<
                    ci::basket_queue::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,co::alignment< 32 >
            >::type
        > BasketQueue_HP_base_align;

        // HP member hook + alignment
        typedef ci::BasketQueue< cds::gc::HP, member_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::hook<
                    ci::basket_queue::member_hook<
                        offsetof(member_hook_item, hMember),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,co::alignment< 32 >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > BasketQueue_HP_member_align;

        // HP base hook + no alignment
        typedef ci::BasketQueue< cds::gc::HP, base_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::hook<
                    ci::basket_queue::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::alignment< co::no_special_alignment >
            >::type
        > BasketQueue_HP_base_noalign;

        // HP member hook + no alignment
        typedef ci::BasketQueue< cds::gc::HP, member_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,ci::opt::hook<
                    ci::basket_queue::member_hook<
                        offsetof(member_hook_item, hMember),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,co::alignment< co::no_special_alignment >
            >::type
        > BasketQueue_HP_member_noalign;


        // HP base hook + cache alignment
        typedef ci::BasketQueue< cds::gc::HP, base_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::hook<
                    ci::basket_queue::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,co::alignment< co::cache_line_alignment >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > BasketQueue_HP_base_cachealign;

        // HP member hook + cache alignment
        typedef ci::BasketQueue< cds::gc::HP, member_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::hook<
                    ci::basket_queue::member_hook<
                        offsetof(member_hook_item, hMember),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,co::alignment< co::cache_line_alignment >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > BasketQueue_HP_member_cachealign;
    } // namespace

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
