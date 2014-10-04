//$$CDS-header$$

#include "hdr_intrusive_treiber_stack.h"
#include <cds/gc/hp.h>
#include <cds/intrusive/treiber_stack.h>

namespace stack {

#define TEST(X)     void TestIntrusiveStack::X() { test<defs::X>(); }
#define TEST_DYN(X)     void TestIntrusiveStack::X() { test_elimination<defs::X>(); }

    namespace defs {
        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::base_hook_item<cds::gc::HP>
            , typename ci::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
            >::type
        > Elimination_HP_default;

        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::base_hook_item<cds::gc::HP>
            , typename ci::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        > Elimination_HP_default_relaxed;

        // HZP GC + base hook
        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::base_hook_item<cds::gc::HP>
            , typename ci::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                ,ci::opt::hook<
                    ci::treiber_stack::base_hook<
                        ci::opt::gc<cds::gc::HP>
                    >
                >
            >::type
        > Elimination_HP_base;

        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::base_hook_item<cds::gc::HP>
            , typename ci::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                ,ci::opt::hook<
                    ci::treiber_stack::base_hook<
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,ci::opt::buffer< ci::opt::v::dynamic_buffer<void *> >
            >::type
        > Elimination_HP_base_dyn;

        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::base_hook_item<cds::gc::HP>
            , typename ci::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                ,ci::opt::hook<
                    ci::treiber_stack::base_hook<
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        > Elimination_HP_base_relaxed;

        // HZP GC + base hook + disposer
        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::base_hook_item<cds::gc::HP>
            , typename ci::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                ,ci::opt::hook<
                    ci::treiber_stack::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
            >::type
        > Elimination_HP_base_disposer;

        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::base_hook_item<cds::gc::HP>
            , typename ci::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                ,ci::opt::hook<
                    ci::treiber_stack::base_hook< ci::opt::gc<cds::gc::HP> >
                >
                ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        > Elimination_HP_base_disposer_relaxed;

        // HZP GC + member hook
        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::member_hook_item<cds::gc::HP>
            , typename ci::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                ,ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::HP>, hMember),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
            >::type
        > Elimination_HP_member;

        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::member_hook_item<cds::gc::HP>
            , typename ci::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                ,ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::HP>, hMember),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,ci::opt::buffer< ci::opt::v::dynamic_buffer<void *> >
            >::type
        > Elimination_HP_member_dyn;

        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::member_hook_item<cds::gc::HP>
            , typename ci::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                ,ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::HP>, hMember),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        > Elimination_HP_member_relaxed;

        // HZP GC + member hook + disposer
        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::member_hook_item<cds::gc::HP>
            , typename ci::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                ,ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::HP>, hMember),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
            >::type
        > Elimination_HP_member_disposer;

        typedef cds::intrusive::TreiberStack< cds::gc::HP,
            TestIntrusiveStack::member_hook_item<cds::gc::HP>
            , typename ci::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                ,ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::HP>, hMember),
                        ci::opt::gc<cds::gc::HP>
                    >
                >
                ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        > Elimination_HP_member_disposer_relaxed;
    }

    TEST(Elimination_HP_default)
    TEST(Elimination_HP_base)
    TEST_DYN(Elimination_HP_base_dyn)
    TEST(Elimination_HP_base_disposer)
    TEST(Elimination_HP_member)
    TEST_DYN(Elimination_HP_member_dyn)
    TEST(Elimination_HP_member_disposer)

    TEST(Elimination_HP_default_relaxed)
    TEST(Elimination_HP_base_relaxed)
    TEST(Elimination_HP_base_disposer_relaxed)
    TEST(Elimination_HP_member_relaxed)
    TEST(Elimination_HP_member_disposer_relaxed)

} // namespace stack
