//$$CDS-header$$

#include "hdr_intrusive_treiber_stack.h"
#include <cds/gc/hp.h>
#include <cds/intrusive/treiber_stack.h>

namespace stack {

#define TEST(X)     void TestIntrusiveStack::X() { test<defs::X>(); }

    namespace defs { namespace {
        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::base_hook_item<cds::gc::HP>
        > Treiber_HP_default;

        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::base_hook_item<cds::gc::HP>
            , typename ci::treiber_stack::make_traits<
                ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        > Treiber_HP_default_relaxed;

        // HZP GC + base hook
        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::base_hook_item<cds::gc::HP>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::treiber_stack::base_hook<
                        ci::opt::gc<cds::gc::HP>
                    >
                >
            >::type
        > Treiber_HP_base;

        struct traits_Treiber_HP_base_relaxed
            : ci::treiber_stack::make_traits <
                ci::opt::hook<
                    ci::treiber_stack::base_hook<
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::base_hook_item<cds::gc::HP>,
            traits_Treiber_HP_base_relaxed
        > Treiber_HP_base_relaxed;

        // HZP GC + base hook + disposer
        struct traits_Treiber_HP_base_disposer
            : ci::treiber_stack::make_traits <
                ci::opt::hook<
                    ci::treiber_stack::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
            >::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::base_hook_item<cds::gc::HP>,
            traits_Treiber_HP_base_disposer
        > Treiber_HP_base_disposer;

        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::base_hook_item<cds::gc::HP>
            ,typename ci::treiber_stack::make_traits <
                ci::opt::hook<
                    ci::treiber_stack::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        > Treiber_HP_base_disposer_relaxed;

        // HZP GC + member hook
        struct traits_Treiber_HP_member
            : ci::treiber_stack::make_traits <
                ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::HP>, hMember),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
            > ::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::member_hook_item<cds::gc::HP>,
            traits_Treiber_HP_member
        > Treiber_HP_member;

        struct traits_Treiber_HP_member_relaxed
            : ci::treiber_stack::make_traits <
                ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::HP>, hMember),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::member_hook_item<cds::gc::HP>,
            traits_Treiber_HP_member_relaxed
        > Treiber_HP_member_relaxed;

        // HZP GC + member hook + disposer
        struct traits_Treiber_HP_member_disposer
            : ci::treiber_stack::make_traits <
                ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::HP>, hMember),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
            >::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::member_hook_item<cds::gc::HP>,
            traits_Treiber_HP_member_disposer
        > Treiber_HP_member_disposer;

        struct traits_Treiber_HP_member_disposer_relaxed
            : ci::treiber_stack::make_traits <
                ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::HP>, hMember),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        {};
        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::member_hook_item<cds::gc::HP>,
            traits_Treiber_HP_member_disposer_relaxed
        > Treiber_HP_member_disposer_relaxed;
    }}

    TEST(Treiber_HP_default)
    TEST(Treiber_HP_base)
    TEST(Treiber_HP_base_disposer)
    TEST(Treiber_HP_member)
    TEST(Treiber_HP_member_disposer)

    TEST(Treiber_HP_default_relaxed)
    TEST(Treiber_HP_base_relaxed)
    TEST(Treiber_HP_base_disposer_relaxed)
    TEST(Treiber_HP_member_relaxed)
    TEST(Treiber_HP_member_disposer_relaxed)

} // namespace stack

CPPUNIT_TEST_SUITE_REGISTRATION(stack::TestIntrusiveStack);
