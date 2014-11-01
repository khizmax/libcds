//$$CDS-header$$

#include "hdr_intrusive_msqueue.h"
#include <cds/gc/hp.h>
#include <cds/intrusive/optimistic_queue.h>

namespace queue {

#define TEST(X) void IntrusiveQueueHeaderTest::test_##X() { test<X>(); }

    namespace {
        struct base_hook_item: public ci::optimistic_queue::node< cds::gc::HP >
        {
            int nVal;
            int nDisposeCount;

            base_hook_item()
                : nDisposeCount(0)
            {}
        };

        struct member_hook_item
        {
            int nVal;
            int nDisposeCount;
            ci::optimistic_queue::node< cds::gc::HP > hMember;

            member_hook_item()
                : nDisposeCount(0)
            {}
        };

        struct traits_OptimisticQueue_HP_default : public ci::optimistic_queue::traits
        {
            typedef IntrusiveQueueHeaderTest::faked_disposer disposer;
        };
        typedef ci::OptimisticQueue< cds::gc::HP, base_hook_item, traits_OptimisticQueue_HP_default > OptimisticQueue_HP_default;

        /// HP + item counter
        struct traits_OptimisticQueue_HP_default_ic : public traits_OptimisticQueue_HP_default
        {
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef ci::OptimisticQueue< cds::gc::HP, base_hook_item, traits_OptimisticQueue_HP_default_ic > OptimisticQueue_HP_default_ic;

        /// HP + stat
        struct traits_OptimisticQueue_HP_default_stat : public
            ci::optimistic_queue::make_traits <
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                , co::stat< ci::optimistic_queue::stat<> >
                , co::memory_model< co::v::sequential_consistent >
            > ::type
        {};
        typedef ci::OptimisticQueue< cds::gc::HP, base_hook_item, traits_OptimisticQueue_HP_default_stat > OptimisticQueue_HP_default_stat;

        // HP base hook
        typedef ci::OptimisticQueue< cds::gc::HP, base_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::hook<
                    ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::memory_model< co::v::relaxed_ordering >
            >::type
        > OptimisticQueue_HP_base;

        // HP member hook
        struct traits_OptimisticQueue_HP_member : public OptimisticQueue_HP_base::traits
        {
            typedef ci::optimistic_queue::member_hook <
                offsetof( member_hook_item, hMember ),
                ci::opt::gc < cds::gc::HP >
            > hook;
        };
        typedef ci::OptimisticQueue< cds::gc::HP, member_hook_item, traits_OptimisticQueue_HP_member > OptimisticQueue_HP_member;

        /// HP base hook + item counter
        typedef ci::OptimisticQueue< cds::gc::HP, base_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,ci::opt::hook<
                    ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > OptimisticQueue_HP_base_ic;

        // HP member hook + item counter
        typedef ci::OptimisticQueue< cds::gc::HP, member_hook_item,
            typename ci::optimistic_queue::make_traits<
                cds::opt::type_traits< traits_OptimisticQueue_HP_member >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
       > OptimisticQueue_HP_member_ic;

        // HP base hook + stat
        typedef ci::OptimisticQueue< cds::gc::HP, base_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::hook<
                    ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::stat< ci::optimistic_queue::stat<> >
            >::type
        > OptimisticQueue_HP_base_stat;

        // HP member hook + stat
        typedef ci::OptimisticQueue< cds::gc::HP, member_hook_item,
            typename ci::optimistic_queue::make_traits<
                cds::opt::type_traits< OptimisticQueue_HP_base_stat::traits >
                , ci::opt::hook<
                    ci::optimistic_queue::member_hook<
                        offsetof(member_hook_item, hMember),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
            >::type
        > OptimisticQueue_HP_member_stat;

        // HP base hook + alignment
        typedef ci::OptimisticQueue< cds::gc::HP, base_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,ci::opt::hook<
                    ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,co::alignment< 32 >
            >::type
        > OptimisticQueue_HP_base_align;

        // HP member hook + alignment
        typedef ci::OptimisticQueue< cds::gc::HP, member_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::hook<
                    ci::optimistic_queue::member_hook<
                        offsetof(member_hook_item, hMember),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,co::alignment< 32 >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > OptimisticQueue_HP_member_align;

        // HP base hook + no alignment
        typedef ci::OptimisticQueue< cds::gc::HP, base_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::hook<
                    ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::alignment< co::no_special_alignment >
            >::type
        > OptimisticQueue_HP_base_noalign;

        // HP member hook + no alignment
        typedef ci::OptimisticQueue< cds::gc::HP, member_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,ci::opt::hook<
                    ci::optimistic_queue::member_hook<
                        offsetof(member_hook_item, hMember),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,co::alignment< co::no_special_alignment >
            >::type
        > OptimisticQueue_HP_member_noalign;


        // HP base hook + cache alignment
        typedef ci::OptimisticQueue< cds::gc::HP, base_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::hook<
                    ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,co::alignment< co::cache_line_alignment >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > OptimisticQueue_HP_base_cachealign;

        // HP member hook + cache alignment
        typedef ci::OptimisticQueue< cds::gc::HP, member_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::hook<
                    ci::optimistic_queue::member_hook<
                        offsetof(member_hook_item, hMember),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,co::alignment< co::cache_line_alignment >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
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
