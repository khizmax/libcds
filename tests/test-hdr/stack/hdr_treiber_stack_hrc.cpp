//$$CDS-header$$

#include "hdr_treiber_stack.h"
#include <cds/gc/hrc.h>
#include <cds/container/treiber_stack.h>

namespace stack {
#define TEST(X)     void TestStack::X() { test<defs::X>(); }
    namespace cs = cds::container;

    namespace defs { namespace {

        typedef cs::TreiberStack< cds::gc::HRC, int > Treiber_HRC;
        typedef cs::TreiberStack< cds::gc::HRC, int, cds::opt::memory_model<cds::opt::v::relaxed_ordering> > Treiber_HRC_relaxed;

        typedef cs::TreiberStack< cds::gc::HRC, int, cds::opt::back_off< cds::backoff::yield> > Treiber_HRC_yield;
        typedef cs::TreiberStack< cds::gc::HRC, int
            ,cds::opt::back_off< cds::backoff::yield>
            ,cds::opt::memory_model<cds::opt::v::relaxed_ordering>
        > Treiber_HRC_yield_relaxed;

        typedef cs::TreiberStack< cds::gc::HRC, int
            ,cds::opt::back_off< cds::backoff::pause>
            ,cds::opt::allocator< std::allocator< bool * > >
        > Treiber_HRC_pause_alloc;

        typedef cs::TreiberStack< cds::gc::HRC, int
            ,cds::opt::back_off< cds::backoff::pause>
            ,cds::opt::memory_model<cds::opt::v::relaxed_ordering>
            ,cds::opt::allocator< std::allocator< bool * > >
        > Treiber_HRC_pause_alloc_relaxed;
    }}

    TEST(Treiber_HRC)
    TEST(Treiber_HRC_yield)
    TEST(Treiber_HRC_pause_alloc)

    TEST(Treiber_HRC_relaxed)
    TEST(Treiber_HRC_yield_relaxed)
    TEST(Treiber_HRC_pause_alloc_relaxed)
}
