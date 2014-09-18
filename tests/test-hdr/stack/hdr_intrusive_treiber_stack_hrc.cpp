//$$CDS-header$$

#include "hdr_intrusive_treiber_stack.h"
#include <cds/gc/hrc.h>
#include <cds/intrusive/treiber_stack.h>

namespace stack {

#define TEST(X)     void TestIntrusiveStack::X() { test<defs::X>(); }

    namespace defs {
        // HRC GC
        typedef cds::intrusive::TreiberStack< cds::gc::HRC,
            TestIntrusiveStack::base_hook_item<cds::gc::HRC>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::HRC> >
            >
        > Treiber_HRC_base;

        typedef cds::intrusive::TreiberStack< cds::gc::HRC,
            TestIntrusiveStack::base_hook_item<cds::gc::HRC>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::HRC> >
            >
            ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
        > Treiber_HRC_base_relaxed;

        // HRC GC + disposer
        typedef cds::intrusive::TreiberStack< cds::gc::HRC,
            TestIntrusiveStack::base_hook_item<cds::gc::HRC>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::HRC> >
            >
            ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
        > Treiber_HRC_base_disposer;

        typedef cds::intrusive::TreiberStack< cds::gc::HRC,
            TestIntrusiveStack::base_hook_item<cds::gc::HRC>
            ,ci::opt::hook<
                ci::single_link::base_hook< ci::opt::gc<cds::gc::HRC> >
            >
            ,ci::opt::disposer< TestIntrusiveStack::faked_disposer >
            ,ci::opt::memory_model< ci::opt::v::relaxed_ordering >
        > Treiber_HRC_base_disposer_relaxed;
    }

    TEST(Treiber_HRC_base)
    TEST(Treiber_HRC_base_disposer)

    TEST(Treiber_HRC_base_relaxed)
    TEST(Treiber_HRC_base_disposer_relaxed)

} // namespace stack

