//$$CDS-header$$

#include "hdr_intrusive_msqueue.h"
#include <cds/intrusive/moir_queue.h>
#include <cds/gc/hp.h>

namespace queue {

#define TEST(X) void IntrusiveQueueHeaderTest::test_##X() { test<X>(); }

    namespace {
        typedef IntrusiveQueueHeaderTest::base_hook_item< ci::msqueue::node<cds::gc::HP > > base_item_type;
        typedef IntrusiveQueueHeaderTest::member_hook_item< ci::msqueue::node<cds::gc::HP > > member_item_type;

        typedef ci::MoirQueue< cds::gc::HP, base_item_type,
            typename ci::msqueue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > MoirQueue_HP_default;

        /// HP + item counter
        typedef ci::MoirQueue< cds::gc::HP, base_item_type,
            typename ci::msqueue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        > MoirQueue_HP_default_ic;

        /// HP + stat
        typedef ci::MoirQueue< cds::gc::HP, base_item_type,
            typename ci::msqueue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::stat< ci::msqueue::stat<> >
            >::type
        > MoirQueue_HP_default_stat;

        // HP base hook
        typedef ci::MoirQueue< cds::gc::HP, base_item_type,
            typename ci::msqueue::make_traits<
                ci::opt::hook<
                    ci::msqueue::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > MoirQueue_HP_base;

        // HP member hook
        typedef ci::MoirQueue< cds::gc::HP, member_item_type,
            typename ci::msqueue::make_traits<
                ci::opt::hook<
                    ci::msqueue::member_hook<
                        offsetof( member_item_type, hMember ),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > MoirQueue_HP_member;

        /// HP base hook + item counter
        typedef ci::MoirQueue< cds::gc::HP, base_item_type,
            typename ci::msqueue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,ci::opt::hook<
                    ci::msqueue::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,co::item_counter< cds::atomicity::item_counter >
                ,co::memory_model< co::v::relaxed_ordering >
            >::type
        > MoirQueue_HP_base_ic;

        // HP member hook + item counter
        typedef ci::MoirQueue< cds::gc::HP, member_item_type,
            typename ci::msqueue::make_traits<
                ci::opt::hook<
                    ci::msqueue::member_hook<
                        offsetof( member_item_type, hMember ),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
       > MoirQueue_HP_member_ic;

        // HP base hook + stat
        typedef ci::MoirQueue< cds::gc::HP, base_item_type,
            typename ci::msqueue::make_traits<
                ci::opt::hook<
                    ci::msqueue::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::stat< ci::msqueue::stat<> >
            >::type
        > MoirQueue_HP_base_stat;

        // HP member hook + stat
        typedef ci::MoirQueue< cds::gc::HP, member_item_type,
            typename ci::msqueue::make_traits<
                ci::opt::hook<
                    ci::msqueue::member_hook<
                        offsetof( member_item_type, hMember ),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::stat< ci::msqueue::stat<> >
            >::type
        > MoirQueue_HP_member_stat;

        // HP base hook + alignment
        typedef ci::MoirQueue< cds::gc::HP, base_item_type,
            typename ci::msqueue::make_traits<
                ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,ci::opt::hook<
                    ci::msqueue::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,co::alignment< 32 >
            >::type
        > MoirQueue_HP_base_align;

        // HP member hook + alignment
        typedef ci::MoirQueue< cds::gc::HP, member_item_type,
            typename ci::msqueue::make_traits<
                ci::opt::hook<
                    ci::msqueue::member_hook<
                        offsetof( member_item_type, hMember ),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,co::alignment< 32 >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > MoirQueue_HP_member_align;

        // HP base hook + no alignment
        struct traits_MoirQueue_HP_base_noalign : public ci::msqueue::traits {
            typedef ci::msqueue::base_hook< ci::opt::gc<cds::gc::HP> > hook;
            typedef IntrusiveQueueHeaderTest::faked_disposer disposer;
            enum { alignment = co::no_special_alignment };
        };
        typedef ci::MoirQueue< cds::gc::HP, base_item_type, traits_MoirQueue_HP_base_noalign > MoirQueue_HP_base_noalign;

        // HP member hook + no alignment
        struct traits_MoirQueue_HP_member_noalign : public ci::msqueue::traits {
            typedef ci::msqueue::member_hook <
                offsetof( member_item_type, hMember ),
                ci::opt::gc < cds::gc::HP >
            > hook;
            typedef IntrusiveQueueHeaderTest::faked_disposer disposer;
            enum { alignment = co::no_special_alignment };
        };
        typedef ci::MoirQueue< cds::gc::HP, member_item_type, traits_MoirQueue_HP_member_noalign > MoirQueue_HP_member_noalign;


        // HP base hook + cache alignment
        struct traits_MoirQueue_HP_base_cachealign : public traits_MoirQueue_HP_base_noalign
        {
            enum { alignment = co::cache_line_alignment };
        };
        typedef ci::MoirQueue< cds::gc::HP, base_item_type, traits_MoirQueue_HP_base_cachealign > MoirQueue_HP_base_cachealign;

        // HP member hook + cache alignment
        struct traits_MoirQueue_HP_member_cachealign : public traits_MoirQueue_HP_member_noalign
        {
            enum { alignment = co::cache_line_alignment };
        };
        typedef ci::MoirQueue< cds::gc::HP, member_item_type, traits_MoirQueue_HP_member_cachealign > MoirQueue_HP_member_cachealign;
    }

    TEST(MoirQueue_HP_default)
    TEST(MoirQueue_HP_default_ic)
    TEST(MoirQueue_HP_default_stat)
    TEST(MoirQueue_HP_base)
    TEST(MoirQueue_HP_member)
    TEST(MoirQueue_HP_base_ic)
    TEST(MoirQueue_HP_member_ic)
    TEST(MoirQueue_HP_base_stat)
    TEST(MoirQueue_HP_member_stat)
    TEST(MoirQueue_HP_base_align)
    TEST(MoirQueue_HP_member_align)
    TEST(MoirQueue_HP_base_noalign)
    TEST(MoirQueue_HP_member_noalign)
    TEST(MoirQueue_HP_base_cachealign)
    TEST(MoirQueue_HP_member_cachealign)
}
