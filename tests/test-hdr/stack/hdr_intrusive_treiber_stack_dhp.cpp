//$$CDS-header$$

#include "hdr_intrusive_treiber_stack.h"
#include <cds/gc/dhp.h>
#include <cds/intrusive/treiber_stack.h>

namespace stack {

#define TEST(X)     void TestIntrusiveStack::X() { test<defs::X>(); }

    namespace defs {
        // DHP GC + base hook
        typedef cds::intrusive::TreiberStack< cds::gc::DHP,
            TestIntrusiveStack::base_hook_item<cds::gc::DHP>
            ,typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::treiber_stack::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
            >::type
        > Treiber_DHP_base;

        typedef cds::intrusive::TreiberStack< cds::gc::DHP,
            TestIntrusiveStack::base_hook_item<cds::gc::DHP>
            ,typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::treiber_stack::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        > Treiber_DHP_base_relaxed;

        // DHP GC + base hook + disposer
        typedef cds::intrusive::TreiberStack< cds::gc::DHP,
            TestIntrusiveStack::base_hook_item<cds::gc::DHP>
            ,typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::treiber_stack::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
            >::type
        > Treiber_DHP_base_disposer;

        typedef cds::intrusive::TreiberStack< cds::gc::DHP,
            TestIntrusiveStack::base_hook_item<cds::gc::DHP>
            ,typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::treiber_stack::base_hook< ci::opt::gc<cds::gc::DHP> >
                >
                ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        > Treiber_DHP_base_disposer_relaxed;

        // DHP GC + member hook
        typedef cds::intrusive::TreiberStack< cds::gc::DHP,
            TestIntrusiveStack::member_hook_item<cds::gc::DHP>
            ,typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::DHP>, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
            >::type
        > Treiber_DHP_member;

        typedef cds::intrusive::TreiberStack< cds::gc::DHP,
            TestIntrusiveStack::member_hook_item<cds::gc::DHP>
            ,typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::DHP>, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        > Treiber_DHP_member_relaxed;

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
                ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
            >::type
        > Treiber_DHP_member_disposer;

        typedef cds::intrusive::TreiberStack< cds::gc::DHP,
            TestIntrusiveStack::member_hook_item<cds::gc::DHP>
            , typename ci::treiber_stack::make_traits<
                ci::opt::hook<
                    ci::treiber_stack::member_hook<
                        offsetof(TestIntrusiveStack::member_hook_item<cds::gc::DHP>, hMember),
                        ci::opt::gc<cds::gc::DHP>
                    >
                >
                ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
                ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
            >::type
        > Treiber_DHP_member_disposer_relaxed;
    }

    TEST(Treiber_DHP_base)
    TEST(Treiber_DHP_base_disposer)
    TEST(Treiber_DHP_member)
    TEST(Treiber_DHP_member_disposer)

    TEST(Treiber_DHP_base_relaxed)
    TEST(Treiber_DHP_base_disposer_relaxed)
    TEST(Treiber_DHP_member_relaxed)
    TEST(Treiber_DHP_member_disposer_relaxed)

} // namespace stack

