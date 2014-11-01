//$$CDS-header$$

#include "hdr_intrusive_msqueue.h"
#include <cds/gc/dhp.h>
#include <cds/intrusive/optimistic_queue.h>

namespace queue {

#define TEST(X) void IntrusiveQueueHeaderTest::test_##X() { test<X>(); }

    namespace {
        struct base_hook_item: public ci::optimistic_queue::node< cds::gc::DHP >
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
            ci::optimistic_queue::node< cds::gc::DHP > hMember;

            member_hook_item()
                : nDisposeCount(0)
            {}
        };

        // DHP base hook
        typedef ci::OptimisticQueue< cds::gc::DHP, base_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::hook<
                    ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::memory_model< co::v::relaxed_ordering >
            >::type
        > OptimisticQueue_DHP_base;

        // DHP member hook
        struct traits_OptimisticQueue_DHP_member : public OptimisticQueue_DHP_base::traits
        {
            typedef ci::optimistic_queue::member_hook <
                offsetof( member_hook_item, hMember ),
                ci::opt::gc < cds::gc::DHP >
            > hook;
        };
        typedef ci::OptimisticQueue< cds::gc::DHP, member_hook_item, traits_OptimisticQueue_DHP_member > OptimisticQueue_DHP_member;

        /// DHP base hook + item counter
        typedef ci::OptimisticQueue< cds::gc::DHP, base_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,ci::opt::hook<
                    ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        > OptimisticQueue_DHP_base_ic;

        // DHP member hook + item counter
        typedef ci::OptimisticQueue< cds::gc::DHP, member_hook_item,
            typename ci::optimistic_queue::make_traits<
                cds::opt::type_traits< traits_OptimisticQueue_DHP_member >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
       > OptimisticQueue_DHP_member_ic;

        // DHP base hook + stat
        typedef ci::OptimisticQueue< cds::gc::DHP, base_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::hook<
                    ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::stat< ci::optimistic_queue::stat<> >
            >::type
        > OptimisticQueue_DHP_base_stat;

        // DHP member hook + stat
        typedef ci::OptimisticQueue< cds::gc::DHP, member_hook_item,
            typename ci::optimistic_queue::make_traits<
                cds::opt::type_traits< OptimisticQueue_DHP_base_stat::traits >
                , ci::opt::hook<
                    ci::optimistic_queue::member_hook<
                        offsetof(member_hook_item, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
            >::type
        > OptimisticQueue_DHP_member_stat;

        // DHP base hook + alignment
        typedef ci::OptimisticQueue< cds::gc::DHP, base_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,ci::opt::hook<
                    ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,co::alignment< 32 >
            >::type
        > OptimisticQueue_DHP_base_align;

        // DHP member hook + alignment
        typedef ci::OptimisticQueue< cds::gc::DHP, member_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::hook<
                    ci::optimistic_queue::member_hook<
                        offsetof(member_hook_item, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
                ,co::alignment< 32 >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > OptimisticQueue_DHP_member_align;

        // DHP base hook + no alignment
        typedef ci::OptimisticQueue< cds::gc::DHP, base_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::hook<
                    ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::alignment< co::no_special_alignment >
            >::type
        > OptimisticQueue_DHP_base_noalign;

        // DHP member hook + no alignment
        typedef ci::OptimisticQueue< cds::gc::DHP, member_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,ci::opt::hook<
                    ci::optimistic_queue::member_hook<
                        offsetof(member_hook_item, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
                ,co::alignment< co::no_special_alignment >
            >::type
        > OptimisticQueue_DHP_member_noalign;


        // DHP base hook + cache alignment
        typedef ci::OptimisticQueue< cds::gc::DHP, base_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::hook<
                    ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,co::alignment< co::cache_line_alignment >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > OptimisticQueue_DHP_base_cachealign;

        // DHP member hook + cache alignment
        typedef ci::OptimisticQueue< cds::gc::DHP, member_hook_item,
            typename ci::optimistic_queue::make_traits<
                ci::opt::hook<
                    ci::optimistic_queue::member_hook<
                        offsetof(member_hook_item, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
                ,co::alignment< co::cache_line_alignment >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > OptimisticQueue_DHP_member_cachealign;
    }

    TEST(OptimisticQueue_DHP_base)
    TEST(OptimisticQueue_DHP_member)
    TEST(OptimisticQueue_DHP_base_ic)
    TEST(OptimisticQueue_DHP_member_ic)
    TEST(OptimisticQueue_DHP_base_stat)
    TEST(OptimisticQueue_DHP_member_stat)
    TEST(OptimisticQueue_DHP_base_align)
    TEST(OptimisticQueue_DHP_member_align)
    TEST(OptimisticQueue_DHP_base_noalign)
    TEST(OptimisticQueue_DHP_member_noalign)
    TEST(OptimisticQueue_DHP_base_cachealign)
    TEST(OptimisticQueue_DHP_member_cachealign)
}
