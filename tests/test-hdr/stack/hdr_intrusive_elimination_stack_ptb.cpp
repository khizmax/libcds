//$$CDS-header$$

#include "hdr_intrusive_treiber_stack.h"
#include <cds/gc/ptb.h>
#include <cds/intrusive/treiber_stack.h>

namespace stack {

#define TEST(X)     void TestIntrusiveStack::X() { test<defs::X>(); }
#define TEST_DYN(X) void TestIntrusiveStack::X() { test_elimination<defs::X>(); }

    namespace defs {
        // PTB GC + base hook
        typedef cds::intrusive::TreiberStack< cds::gc::PTB,
            TestIntrusiveStack::base_hook_item<cds::gc::PTB>
            , typename ci::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                ,ci::opt::hook<
                    ci::single_link::base_hook< ci::opt::gc<cds::gc::PTB> >
                >
            >::type
        > Elimination_PTB_base;

        typedef cds::intrusive::TreiberStack< cds::gc::PTB,
            TestIntrusiveStack::base_hook_item<cds::gc::PTB>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::single_link::base_hook< ci::opt::gc<cds::gc::PTB> >
                >
                ,cds::opt::enable_elimination<true>
                ,ci::opt::buffer< ci::opt::v::dynamic_buffer<void *> >
            >::type
        > Elimination_PTB_base_dyn;

        typedef cds::intrusive::TreiberStack< cds::gc::PTB,
            TestIntrusiveStack::base_hook_item<cds::gc::PTB>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::single_link::base_hook< ci::opt::gc<cds::gc::PTB> >
                >
                ,cds::opt::enable_elimination<true>
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        > Elimination_PTB_base_relaxed;

        // PTB GC + base hook + disposer
        typedef cds::intrusive::TreiberStack< cds::gc::PTB,
            TestIntrusiveStack::base_hook_item<cds::gc::PTB>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::single_link::base_hook< ci::opt::gc<cds::gc::PTB> >
                >
                ,cds::opt::enable_elimination<true>
                ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
            >::type
        > Elimination_PTB_base_disposer;

        typedef cds::intrusive::TreiberStack< cds::gc::PTB,
            TestIntrusiveStack::base_hook_item<cds::gc::PTB>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::single_link::base_hook< ci::opt::gc<cds::gc::PTB> >
                >
                ,cds::opt::enable_elimination<true>
                ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        > Elimination_PTB_base_disposer_relaxed;

        // PTB GC + member hook
        typedef cds::intrusive::TreiberStack< cds::gc::PTB,
            TestIntrusiveStack::member_hook_item<cds::gc::PTB>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::single_link::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::PTB>, hMember),
                        ci::opt::gc<cds::gc::PTB>
                    >
                >
                ,cds::opt::enable_elimination<true>
            >::type
        > Elimination_PTB_member;

        typedef cds::intrusive::TreiberStack< cds::gc::PTB,
            TestIntrusiveStack::member_hook_item<cds::gc::PTB>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::single_link::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::PTB>, hMember),
                        ci::opt::gc<cds::gc::PTB>
                    >
                >
                ,cds::opt::enable_elimination<true>
                ,ci::opt::buffer< ci::opt::v::dynamic_buffer<void *> >
            >::type
        > Elimination_PTB_member_dyn;

        typedef cds::intrusive::TreiberStack< cds::gc::PTB,
            TestIntrusiveStack::member_hook_item<cds::gc::PTB>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::single_link::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::PTB>, hMember),
                        ci::opt::gc<cds::gc::PTB>
                    >
                >
                ,cds::opt::enable_elimination<true>
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        > Elimination_PTB_member_relaxed;

        // PTB GC + member hook + disposer
        typedef cds::intrusive::TreiberStack< cds::gc::PTB,
            TestIntrusiveStack::member_hook_item<cds::gc::PTB>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::single_link::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::PTB>, hMember),
                        ci::opt::gc<cds::gc::PTB>
                    >
                >
                ,cds::opt::enable_elimination<true>
                ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
            >::type
        > Elimination_PTB_member_disposer;

        typedef cds::intrusive::TreiberStack< cds::gc::PTB,
            TestIntrusiveStack::member_hook_item<cds::gc::PTB>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::single_link::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::PTB>, hMember),
                        ci::opt::gc<cds::gc::PTB>
                    >
                >
                ,cds::opt::enable_elimination<true>
                ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        > Elimination_PTB_member_disposer_relaxed;
    }

    TEST(Elimination_PTB_base)
    TEST_DYN(Elimination_PTB_base_dyn)
    TEST(Elimination_PTB_base_disposer)
    TEST(Elimination_PTB_member)
    TEST_DYN(Elimination_PTB_member_dyn)
    TEST(Elimination_PTB_member_disposer)

    TEST(Elimination_PTB_base_relaxed)
    TEST(Elimination_PTB_base_disposer_relaxed)
    TEST(Elimination_PTB_member_relaxed)
    TEST(Elimination_PTB_member_disposer_relaxed)

} // namespace stack

