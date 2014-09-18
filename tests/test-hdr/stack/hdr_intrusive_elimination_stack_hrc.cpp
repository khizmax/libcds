//$$CDS-header$$

#include "hdr_intrusive_treiber_stack.h"
#include <cds/gc/hrc.h>
#include <cds/intrusive/treiber_stack.h>

namespace stack {

#define TEST(X)      void TestIntrusiveStack::X() { test<defs::X>(); }
#define TEST_DYN(X)  void TestIntrusiveStack::X() { test_elimination<defs::X>(); }

    namespace defs {
        // HRC GC
        typedef cds::intrusive::TreiberStack< cds::gc::HRC,
            TestIntrusiveStack::base_hook_item<cds::gc::HRC>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::HRC> >
            >
            ,cds::opt::enable_elimination<true>
        > Elimination_HRC_base;

        typedef cds::intrusive::TreiberStack< cds::gc::HRC,
            TestIntrusiveStack::base_hook_item<cds::gc::HRC>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::HRC> >
            >
            ,cds::opt::enable_elimination<true>
            ,ci::opt::buffer< ci::opt::v::dynamic_buffer<void *> >
        > Elimination_HRC_base_dyn;

        typedef cds::intrusive::TreiberStack< cds::gc::HRC,
            TestIntrusiveStack::base_hook_item<cds::gc::HRC>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::HRC> >
            >
            ,cds::opt::enable_elimination<true>
            ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
        > Elimination_HRC_base_relaxed;

        // HRC GC + disposer
        typedef cds::intrusive::TreiberStack< cds::gc::HRC,
            TestIntrusiveStack::base_hook_item<cds::gc::HRC>
            ,cds::opt::enable_elimination<true>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::HRC> >
            >
            ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
        > Elimination_HRC_base_disposer;

        typedef cds::intrusive::TreiberStack< cds::gc::HRC,
            TestIntrusiveStack::base_hook_item<cds::gc::HRC>
            ,cds::opt::enable_elimination<true>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::HRC> >
            >
            ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
            ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
        > Elimination_HRC_base_disposer_relaxed;
    }

    TEST(Elimination_HRC_base)
    TEST_DYN(Elimination_HRC_base_dyn)
    TEST(Elimination_HRC_base_disposer)

    TEST(Elimination_HRC_base_relaxed)
    TEST(Elimination_HRC_base_disposer_relaxed)

} // namespace stack

