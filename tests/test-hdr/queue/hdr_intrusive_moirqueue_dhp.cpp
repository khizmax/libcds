//$$CDS-header$$

#include "hdr_intrusive_msqueue.h"
#include <cds/intrusive/moir_queue.h>
#include <cds/gc/dhp.h>

namespace queue {

#define TEST(X)     void IntrusiveQueueHeaderTest::test_##X() { test<X>(); }

    namespace {
        typedef IntrusiveQueueHeaderTest::base_hook_item< ci::msqueue::node<cds::gc::DHP > > base_item_type;
        typedef IntrusiveQueueHeaderTest::member_hook_item< ci::msqueue::node<cds::gc::DHP > > member_item_type;

        // DHP base hook
        typedef ci::MoirQueue< cds::gc::DHP, base_item_type,
            typename ci::msqueue::make_traits<
            ci::opt::hook<
            ci::msqueue::base_hook< ci::opt::gc<cds::gc::DHP> >
            >
            , ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > MoirQueue_DHP_base;

        // DHP member hook
        typedef ci::MoirQueue< cds::gc::DHP, member_item_type,
            typename ci::msqueue::make_traits<
            ci::opt::hook<
            ci::msqueue::member_hook<
            offsetof( member_item_type, hMember ),
            ci::opt::gc<cds::gc::DHP>
            >
            >
            , ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > MoirQueue_DHP_member;

        /// DHP base hook + item counter
        typedef ci::MoirQueue< cds::gc::DHP, base_item_type,
            typename ci::msqueue::make_traits<
            ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            , ci::opt::hook<
            ci::msqueue::base_hook< ci::opt::gc<cds::gc::DHP> >
            >
            , co::item_counter< cds::atomicity::item_counter >
            , co::memory_model< co::v::relaxed_ordering >
            >::type
        > MoirQueue_DHP_base_ic;

        // DHP member hook + item counter
        typedef ci::MoirQueue< cds::gc::DHP, member_item_type,
            typename ci::msqueue::make_traits<
            ci::opt::hook<
            ci::msqueue::member_hook<
            offsetof( member_item_type, hMember ),
            ci::opt::gc<cds::gc::DHP>
            >
            >
            , ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            , co::item_counter< cds::atomicity::item_counter >
            >::type
        > MoirQueue_DHP_member_ic;

        // DHP base hook + stat
        typedef ci::MoirQueue< cds::gc::DHP, base_item_type,
            typename ci::msqueue::make_traits<
            ci::opt::hook<
            ci::msqueue::base_hook< ci::opt::gc<cds::gc::DHP> >
            >
            , ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            , co::stat< ci::msqueue::stat<> >
            >::type
        > MoirQueue_DHP_base_stat;

        // DHP member hook + stat
        typedef ci::MoirQueue< cds::gc::DHP, member_item_type,
            typename ci::msqueue::make_traits<
            ci::opt::hook<
            ci::msqueue::member_hook<
            offsetof( member_item_type, hMember ),
            ci::opt::gc<cds::gc::DHP>
            >
            >
            , ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            , co::stat< ci::msqueue::stat<> >
            >::type
        > MoirQueue_DHP_member_stat;

        // DHP base hook + alignment
        typedef ci::MoirQueue< cds::gc::DHP, base_item_type,
            typename ci::msqueue::make_traits<
            ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            , ci::opt::hook<
            ci::msqueue::base_hook< ci::opt::gc<cds::gc::DHP> >
            >
            , co::alignment< 32 >
            >::type
        > MoirQueue_DHP_base_align;

        // DHP member hook + alignment
        typedef ci::MoirQueue< cds::gc::DHP, member_item_type,
            typename ci::msqueue::make_traits<
            ci::opt::hook<
            ci::msqueue::member_hook<
            offsetof( member_item_type, hMember ),
            ci::opt::gc<cds::gc::DHP>
            >
            >
            , co::alignment< 32 >
            , ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >::type
        > MoirQueue_DHP_member_align;

        // DHP base hook + no alignment
        struct traits_MoirQueue_DHP_base_noalign : public ci::msqueue::traits {
            typedef ci::msqueue::base_hook< ci::opt::gc<cds::gc::DHP> > hook;
            typedef IntrusiveQueueHeaderTest::faked_disposer disposer;
            enum { alignment = co::no_special_alignment };
        };
        typedef ci::MoirQueue< cds::gc::DHP, base_item_type, traits_MoirQueue_DHP_base_noalign > MoirQueue_DHP_base_noalign;

        // DHP member hook + no alignment
        struct traits_MoirQueue_DHP_member_noalign : public ci::msqueue::traits {
            typedef ci::msqueue::member_hook <
                offsetof( member_item_type, hMember ),
                ci::opt::gc < cds::gc::DHP >
            > hook;
            typedef IntrusiveQueueHeaderTest::faked_disposer disposer;
            enum { alignment = co::no_special_alignment };
        };
        typedef ci::MoirQueue< cds::gc::DHP, member_item_type, traits_MoirQueue_DHP_member_noalign > MoirQueue_DHP_member_noalign;


        // DHP base hook + cache alignment
        struct traits_MoirQueue_DHP_base_cachealign : public traits_MoirQueue_DHP_base_noalign
        {
            enum { alignment = co::cache_line_alignment };
        };
        typedef ci::MoirQueue< cds::gc::DHP, base_item_type, traits_MoirQueue_DHP_base_cachealign > MoirQueue_DHP_base_cachealign;

        // DHP member hook + cache alignment
        struct traits_MoirQueue_DHP_member_cachealign : public traits_MoirQueue_DHP_member_noalign
        {
            enum { alignment = co::cache_line_alignment };
        };
        typedef ci::MoirQueue< cds::gc::DHP, member_item_type, traits_MoirQueue_DHP_member_cachealign > MoirQueue_DHP_member_cachealign;
    }   // namespace

    TEST(MoirQueue_DHP_base)
    TEST(MoirQueue_DHP_member)
    TEST(MoirQueue_DHP_base_ic)
    TEST(MoirQueue_DHP_member_ic)
    TEST(MoirQueue_DHP_base_stat)
    TEST(MoirQueue_DHP_member_stat)
    TEST(MoirQueue_DHP_base_align)
    TEST(MoirQueue_DHP_member_align)
    TEST(MoirQueue_DHP_base_noalign)
    TEST(MoirQueue_DHP_member_noalign)
    TEST(MoirQueue_DHP_base_cachealign)
    TEST(MoirQueue_DHP_member_cachealign)

} // namespace queue
