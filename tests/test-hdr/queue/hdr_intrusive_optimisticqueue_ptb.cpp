//$$CDS-header$$

#include "hdr_intrusive_msqueue.h"
#include <cds/intrusive/optimistic_queue.h>
#include <cds/gc/ptb.h>

namespace queue {

#define TEST(X)     void IntrusiveQueueHeaderTest::test_##X() { test<X>(); }

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
            int nVal;
            int nDisposeCount;
            ci::optimistic_queue::node< GC > hMember;

            member_hook_item()
                : nDisposeCount(0)
            {}
        };

        // PTB base hook
        typedef ci::OptimisticQueue< cds::gc::PTB,
            base_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::PTB> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > OptimisticQueue_PTB_base;

        // PTB member hook
        typedef ci::OptimisticQueue< cds::gc::PTB,
            member_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::optimistic_queue::member_hook<
                    offsetof(member_hook_item<cds::gc::PTB>, hMember),
                    ci::opt::gc<cds::gc::PTB>
                >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > OptimisticQueue_PTB_member;

        /// PTB base hook + item counter
        typedef ci::OptimisticQueue< cds::gc::PTB,
            base_hook_item<cds::gc::PTB>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::PTB> >
            >
            ,co::item_counter< cds::atomicity::item_counter >
        > OptimisticQueue_PTB_base_ic;

        // PTB member hook + item counter
        typedef ci::OptimisticQueue< cds::gc::PTB,
            member_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::optimistic_queue::member_hook<
                    offsetof(member_hook_item<cds::gc::PTB>, hMember),
                    ci::opt::gc<cds::gc::PTB>
                >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::item_counter< cds::atomicity::item_counter >
       > OptimisticQueue_PTB_member_ic;

        // PTB base hook + stat
        typedef ci::OptimisticQueue< cds::gc::PTB,
            base_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::PTB> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::stat< ci::queue_stat<> >
        > OptimisticQueue_PTB_base_stat;

        // PTB member hook + stat
        typedef ci::OptimisticQueue< cds::gc::PTB,
            member_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::optimistic_queue::member_hook<
                    offsetof(member_hook_item<cds::gc::PTB>, hMember),
                    ci::opt::gc<cds::gc::PTB>
                >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::stat< ci::queue_stat<> >
        > OptimisticQueue_PTB_member_stat;

        // PTB base hook + alignment
        typedef ci::OptimisticQueue< cds::gc::PTB,
            base_hook_item<cds::gc::PTB>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::PTB> >
            >
            ,co::alignment< 32 >
        > OptimisticQueue_PTB_base_align;

        // PTB member hook + alignment
        typedef ci::OptimisticQueue< cds::gc::PTB,
            member_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::optimistic_queue::member_hook<
                    offsetof(member_hook_item<cds::gc::PTB>, hMember),
                    ci::opt::gc<cds::gc::PTB>
                >
            >
            ,co::alignment< 32 >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > OptimisticQueue_PTB_member_align;

        // PTB base hook + no alignment
        typedef ci::OptimisticQueue< cds::gc::PTB,
            base_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::PTB> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::alignment< co::no_special_alignment >
        > OptimisticQueue_PTB_base_noalign;

        // PTB member hook + no alignment
        typedef ci::OptimisticQueue< cds::gc::PTB,
            member_hook_item<cds::gc::PTB>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::optimistic_queue::member_hook<
                    offsetof(member_hook_item<cds::gc::PTB>, hMember),
                    ci::opt::gc<cds::gc::PTB>
                >
            >
            ,co::alignment< co::no_special_alignment >
        > OptimisticQueue_PTB_member_noalign;


        // PTB base hook + cache alignment
        typedef ci::OptimisticQueue< cds::gc::PTB,
            base_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::optimistic_queue::base_hook< ci::opt::gc<cds::gc::PTB> >
            >
            ,co::alignment< co::cache_line_alignment >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > OptimisticQueue_PTB_base_cachealign;

        // PTB member hook + cache alignment
        typedef ci::OptimisticQueue< cds::gc::PTB,
            member_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::optimistic_queue::member_hook<
                    offsetof(member_hook_item<cds::gc::PTB>, hMember),
                    ci::opt::gc<cds::gc::PTB>
                >
            >
            ,co::alignment< co::cache_line_alignment >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > OptimisticQueue_PTB_member_cachealign;

    }   // namespace

    TEST(OptimisticQueue_PTB_base)
    TEST(OptimisticQueue_PTB_member)
    TEST(OptimisticQueue_PTB_base_ic)
    TEST(OptimisticQueue_PTB_member_ic)
    TEST(OptimisticQueue_PTB_base_stat)
    TEST(OptimisticQueue_PTB_member_stat)
    TEST(OptimisticQueue_PTB_base_align)
    TEST(OptimisticQueue_PTB_member_align)
    TEST(OptimisticQueue_PTB_base_noalign)
    TEST(OptimisticQueue_PTB_member_noalign)
    TEST(OptimisticQueue_PTB_base_cachealign)
    TEST(OptimisticQueue_PTB_member_cachealign)

} // namespace queue
