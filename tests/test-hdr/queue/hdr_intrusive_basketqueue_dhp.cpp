//$$CDS-header$$

#include "hdr_intrusive_msqueue.h"
#include <cds/gc/dhp.h>
#include <cds/intrusive/basket_queue.h>

namespace queue {

#define TEST(X) void IntrusiveQueueHeaderTest::test_##X() { test_basket<X>(); }

    namespace {
        typedef IntrusiveQueueHeaderTest::base_hook_item < ci::basket_queue::node<cds::gc::DHP> > base_hook_item;
        typedef IntrusiveQueueHeaderTest::member_hook_item < ci::basket_queue::node<cds::gc::DHP> > member_hook_item;

        // DHP base hook
        struct traits_BasketQueue_disposer : public
            ci::basket_queue::make_traits < ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer > >::type
        {
            typedef ci::basket_queue::base_hook< ci::opt::gc < cds::gc::DHP > > hook;
        };
        typedef ci::BasketQueue< cds::gc::DHP, base_hook_item, traits_BasketQueue_disposer > BasketQueue_DHP_base;

        // DHP member hook
        struct traits_BasketQueue_member : public traits_BasketQueue_disposer
        {
            typedef ci::basket_queue::member_hook< offsetof( member_hook_item, hMember ), ci::opt::gc < cds::gc::DHP > > hook;
        };
        typedef ci::BasketQueue< cds::gc::DHP, member_hook_item, traits_BasketQueue_member >BasketQueue_DHP_member;

        /// DHP base hook + item counter
        typedef ci::BasketQueue< cds::gc::DHP, base_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,ci::opt::hook<
                    ci::basket_queue::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::memory_model< co::v::relaxed_ordering >
            >::type
        > BasketQueue_DHP_base_ic;

        // DHP member hook + item counter
        typedef ci::BasketQueue< cds::gc::DHP, member_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::hook<
                    ci::basket_queue::member_hook<
                        offsetof( member_hook_item, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
       > BasketQueue_DHP_member_ic;

        // DHP base hook + stat
        typedef ci::BasketQueue< cds::gc::DHP, base_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::hook<
                    ci::basket_queue::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::stat< ci::basket_queue::stat<> >
            >::type
        > BasketQueue_DHP_base_stat;

        // DHP member hook + stat
        typedef ci::BasketQueue< cds::gc::DHP, member_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::hook<
                    ci::basket_queue::member_hook<
                        offsetof(member_hook_item, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::stat< ci::basket_queue::stat<> >
            >::type
        > BasketQueue_DHP_member_stat;

        // DHP base hook + alignment
        typedef ci::BasketQueue< cds::gc::DHP, base_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,ci::opt::hook<
                    ci::basket_queue::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,co::alignment< 32 >
            >::type
        > BasketQueue_DHP_base_align;

        // DHP member hook + alignment
        typedef ci::BasketQueue< cds::gc::DHP, member_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::hook<
                    ci::basket_queue::member_hook<
                        offsetof(member_hook_item, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
                ,co::alignment< 32 >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > BasketQueue_DHP_member_align;

        // DHP base hook + no alignment
        typedef ci::BasketQueue< cds::gc::DHP, base_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::hook<
                    ci::basket_queue::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::alignment< co::no_special_alignment >
            >::type
        > BasketQueue_DHP_base_noalign;

        // DHP member hook + no alignment
        typedef ci::BasketQueue< cds::gc::DHP, member_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,ci::opt::hook<
                    ci::basket_queue::member_hook<
                        offsetof(member_hook_item, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
                ,co::alignment< co::no_special_alignment >
            >::type
        > BasketQueue_DHP_member_noalign;


        // DHP base hook + cache alignment
        typedef ci::BasketQueue< cds::gc::DHP, base_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::hook<
                    ci::basket_queue::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,co::alignment< co::cache_line_alignment >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > BasketQueue_DHP_base_cachealign;

        // DHP member hook + cache alignment
        typedef ci::BasketQueue< cds::gc::DHP, member_hook_item,
            typename ci::basket_queue::make_traits<
                ci::opt::hook<
                    ci::basket_queue::member_hook<
                        offsetof(member_hook_item, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
                ,co::alignment< co::cache_line_alignment >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > BasketQueue_DHP_member_cachealign;
    } // namespace

    TEST(BasketQueue_DHP_base)
    TEST(BasketQueue_DHP_member)
    TEST(BasketQueue_DHP_base_ic)
    TEST(BasketQueue_DHP_member_ic)
    TEST(BasketQueue_DHP_base_stat)
    TEST(BasketQueue_DHP_member_stat)
    TEST(BasketQueue_DHP_base_align)
    TEST(BasketQueue_DHP_member_align)
    TEST(BasketQueue_DHP_base_noalign)
    TEST(BasketQueue_DHP_member_noalign)
    TEST(BasketQueue_DHP_base_cachealign)
    TEST(BasketQueue_DHP_member_cachealign)
}
