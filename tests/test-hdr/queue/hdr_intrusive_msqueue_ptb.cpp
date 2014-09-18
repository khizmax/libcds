//$$CDS-header$$

#include "hdr_intrusive_msqueue.h"
#include "hdr_intrusive_singlelink_node.h"

#include <cds/intrusive/msqueue.h>
#include <cds/gc/ptb.h>

namespace queue {

#define TEST(X)     void IntrusiveQueueHeaderTest::test_##X() { test<X>(); }

    namespace {

        // PTB base hook
        typedef ci::MSQueue< cds::gc::PTB,
            base_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::PTB> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > MSQueue_PTB_base;

        // PTB member hook
        typedef ci::MSQueue< cds::gc::PTB,
            member_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::single_link::member_hook<
                    offsetof(member_hook_item<cds::gc::PTB>, hMember),
                    ci::opt::gc<cds::gc::PTB>
                >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > MSQueue_PTB_member;

        /// PTB base hook + item counter
        typedef ci::MSQueue< cds::gc::PTB,
            base_hook_item<cds::gc::PTB>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::PTB> >
            >
            ,co::item_counter< cds::atomicity::item_counter >
        > MSQueue_PTB_base_ic;

        // PTB member hook + item counter
        typedef ci::MSQueue< cds::gc::PTB,
            member_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::single_link::member_hook<
                    offsetof(member_hook_item<cds::gc::PTB>, hMember),
                    ci::opt::gc<cds::gc::PTB>
                >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::item_counter< cds::atomicity::item_counter >
       > MSQueue_PTB_member_ic;

        // PTB base hook + stat
        typedef ci::MSQueue< cds::gc::PTB,
            base_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::PTB> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::stat< ci::queue_stat<> >
        > MSQueue_PTB_base_stat;

        // PTB member hook + stat
        typedef ci::MSQueue< cds::gc::PTB,
            member_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::single_link::member_hook<
                    offsetof(member_hook_item<cds::gc::PTB>, hMember),
                    ci::opt::gc<cds::gc::PTB>
                >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::stat< ci::queue_stat<> >
        > MSQueue_PTB_member_stat;

        // PTB base hook + alignment
        typedef ci::MSQueue< cds::gc::PTB,
            base_hook_item<cds::gc::PTB>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::PTB> >
            >
            ,co::alignment< 32 >
        > MSQueue_PTB_base_align;

        // PTB member hook + alignment
        typedef ci::MSQueue< cds::gc::PTB,
            member_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::single_link::member_hook<
                    offsetof(member_hook_item<cds::gc::PTB>, hMember),
                    ci::opt::gc<cds::gc::PTB>
                >
            >
            ,co::alignment< 32 >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > MSQueue_PTB_member_align;

        // PTB base hook + no alignment
        typedef ci::MSQueue< cds::gc::PTB,
            base_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::PTB> >
            >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::alignment< co::no_special_alignment >
        > MSQueue_PTB_base_noalign;

        // PTB member hook + no alignment
        typedef ci::MSQueue< cds::gc::PTB,
            member_hook_item<cds::gc::PTB>
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,ci::opt::hook<
                ci::single_link::member_hook<
                    offsetof(member_hook_item<cds::gc::PTB>, hMember),
                    ci::opt::gc<cds::gc::PTB>
                >
            >
            ,co::alignment< co::no_special_alignment >
        > MSQueue_PTB_member_noalign;


        // PTB base hook + cache alignment
        typedef ci::MSQueue< cds::gc::PTB,
            base_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::PTB> >
            >
            ,co::alignment< co::cache_line_alignment >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > MSQueue_PTB_base_cachealign;

        // PTB member hook + cache alignment
        typedef ci::MSQueue< cds::gc::PTB,
            member_hook_item<cds::gc::PTB>
            ,ci::opt::hook<
                ci::single_link::member_hook<
                    offsetof(member_hook_item<cds::gc::PTB>, hMember),
                    ci::opt::gc<cds::gc::PTB>
                >
            >
            ,co::alignment< co::cache_line_alignment >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
        > MSQueue_PTB_member_cachealign;

    }   // namespace

    TEST(MSQueue_PTB_base)
    TEST(MSQueue_PTB_member)
    TEST(MSQueue_PTB_base_ic)
    TEST(MSQueue_PTB_member_ic)
    TEST(MSQueue_PTB_base_stat)
    TEST(MSQueue_PTB_member_stat)
    TEST(MSQueue_PTB_base_align)
    TEST(MSQueue_PTB_member_align)
    TEST(MSQueue_PTB_base_noalign)
    TEST(MSQueue_PTB_member_noalign)
    TEST(MSQueue_PTB_base_cachealign)
    TEST(MSQueue_PTB_member_cachealign)

} // namespace queue
