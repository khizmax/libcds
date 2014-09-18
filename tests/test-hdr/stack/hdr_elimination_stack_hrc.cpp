//$$CDS-header$$

#include "hdr_treiber_stack.h"
#include <cds/gc/hrc.h>
#include <cds/container/treiber_stack.h>

namespace stack {
#define TEST(X)     void TestStack::X() { test<defs::X>(); }
#define TEST_DYN(X)     void TestStack::X() { test_elimination<defs::X>(); }

    namespace cs = cds::container;

    namespace defs { namespace {

        typedef cs::TreiberStack< cds::gc::HRC, int
            ,cds::opt::enable_elimination<true>
        > Elimination_HRC;

        typedef cs::TreiberStack< cds::gc::HRC, int
            , cds::opt::buffer< cds::opt::v::dynamic_buffer<void *> >
            ,cds::opt::enable_elimination<true>
        > Elimination_HRC_dyn;

        typedef cs::TreiberStack< cds::gc::HRC, int
            , cds::opt::buffer< cds::opt::v::dynamic_buffer<void *> >
            , cds::opt::stat< cs::treiber_stack::stat<> >
            ,cds::opt::enable_elimination<true>
        > Elimination_HRC_stat;

        typedef cs::TreiberStack< cds::gc::HRC, int
            , cds::opt::memory_model<cds::opt::v::relaxed_ordering>
            ,cds::opt::enable_elimination<true>
        > Elimination_HRC_relaxed;

        typedef cs::TreiberStack< cds::gc::HRC, int
            , cds::opt::back_off< cds::backoff::yield>
            ,cds::opt::enable_elimination<true>
        > Elimination_HRC_yield;

        typedef cs::TreiberStack< cds::gc::HRC, int
            ,cds::opt::back_off< cds::backoff::yield>
            ,cds::opt::memory_model<cds::opt::v::relaxed_ordering>
            ,cds::opt::enable_elimination<true>
        > Elimination_HRC_yield_relaxed;

        typedef cs::TreiberStack< cds::gc::HRC, int
            ,cds::opt::back_off< cds::backoff::pause>
            ,cds::opt::allocator< std::allocator< bool * > >
            ,cds::opt::enable_elimination<true>
        > Elimination_HRC_pause_alloc;

        typedef cs::TreiberStack< cds::gc::HRC, int
            ,cds::opt::back_off< cds::backoff::pause>
            ,cds::opt::memory_model<cds::opt::v::relaxed_ordering>
            ,cds::opt::enable_elimination<true>
            ,cds::opt::allocator< std::allocator< bool * > >
        > Elimination_HRC_pause_alloc_relaxed;
    }}

    TEST(Elimination_HRC)
    TEST_DYN(Elimination_HRC_dyn)
    TEST_DYN(Elimination_HRC_stat)
    TEST(Elimination_HRC_yield)
    TEST(Elimination_HRC_pause_alloc)

    TEST(Elimination_HRC_relaxed)
    TEST(Elimination_HRC_yield_relaxed)
    TEST(Elimination_HRC_pause_alloc_relaxed)
}
