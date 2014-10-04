//$$CDS-header$$

#include "hdr_intrusive_treiber_stack.h"
#include <cds/gc/dhp.h>
#include <cds/intrusive/treiber_stack.h>

namespace stack {

#define TEST(X)     void TestIntrusiveStack::X() { test<defs::X>(); }
#define TEST_DYN(X) void TestIntrusiveStack::X() { test_elimination<defs::X>(); }

    namespace defs {
        // DHP GC + base hook
        typedef cds::intrusive::TreiberStack< cds::gc::DHP,
            TestIntrusiveStack::base_hook_item<cds::gc::DHP>
            , typename ci::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                ,ci::opt::hook<
                    ci::treiber_stack::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
            >::type
        > Elimination_DHP_base;

        typedef cds::intrusive::TreiberStack< cds::gc::DHP,
            TestIntrusiveStack::base_hook_item<cds::gc::DHP>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::treiber_stack::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,cds::opt::enable_elimination<true>
                ,ci::opt::buffer< ci::opt::v::dynamic_buffer<void *> >
            >::type
        > Elimination_DHP_base_dyn;

        typedef cds::intrusive::TreiberStack< cds::gc::DHP,
            TestIntrusiveStack::base_hook_item<cds::gc::DHP>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::treiber_stack::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,cds::opt::enable_elimination<true>
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        > Elimination_DHP_base_relaxed;

        // DHP GC + base hook + disposer
        typedef cds::intrusive::TreiberStack< cds::gc::DHP,
            TestIntrusiveStack::base_hook_item<cds::gc::DHP>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::treiber_stack::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,cds::opt::enable_elimination<true>
                ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
            >::type
        > Elimination_DHP_base_disposer;

        typedef cds::intrusive::TreiberStack< cds::gc::DHP,
            TestIntrusiveStack::base_hook_item<cds::gc::DHP>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::treiber_stack::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,cds::opt::enable_elimination<true>
                ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        > Elimination_DHP_base_disposer_relaxed;

        // DHP GC + member hook
        typedef cds::intrusive::TreiberStack< cds::gc::DHP,
            TestIntrusiveStack::member_hook_item<cds::gc::DHP>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::DHP>, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
                ,cds::opt::enable_elimination<true>
            >::type
        > Elimination_DHP_member;

        typedef cds::intrusive::TreiberStack< cds::gc::DHP,
            TestIntrusiveStack::member_hook_item<cds::gc::DHP>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::DHP>, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
                ,cds::opt::enable_elimination<true>
                ,ci::opt::buffer< ci::opt::v::dynamic_buffer<void *> >
            >::type
        > Elimination_DHP_member_dyn;

        typedef cds::intrusive::TreiberStack< cds::gc::DHP,
            TestIntrusiveStack::member_hook_item<cds::gc::DHP>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::DHP>, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
                ,cds::opt::enable_elimination<true>
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        > Elimination_DHP_member_relaxed;

        // DHP GC + member hook + disposer
        typedef cds::intrusive::TreiberStack< cds::gc::DHP,
            TestIntrusiveStack::member_hook_item<cds::gc::DHP>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::DHP>, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
                ,cds::opt::enable_elimination<true>
                ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
            >::type
        > Elimination_DHP_member_disposer;

        typedef cds::intrusive::TreiberStack< cds::gc::DHP,
            TestIntrusiveStack::member_hook_item<cds::gc::DHP>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::DHP>, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
                ,cds::opt::enable_elimination<true>
                ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        > Elimination_DHP_member_disposer_relaxed;
    }

    TEST(Elimination_DHP_base)
    TEST_DYN(Elimination_DHP_base_dyn)
    TEST(Elimination_DHP_base_disposer)
    TEST(Elimination_DHP_member)
    TEST_DYN(Elimination_DHP_member_dyn)
    TEST(Elimination_DHP_member_disposer)

    TEST(Elimination_DHP_base_relaxed)
    TEST(Elimination_DHP_base_disposer_relaxed)
    TEST(Elimination_DHP_member_relaxed)
    TEST(Elimination_DHP_member_disposer_relaxed)

} // namespace stack

