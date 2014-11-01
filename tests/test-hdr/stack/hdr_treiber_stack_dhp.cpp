//$$CDS-header$$

#include "hdr_treiber_stack.h"
#include <cds/gc/dhp.h>
#include <cds/container/treiber_stack.h>

namespace stack {
#define TEST(X)     void TestStack::X() { test<defs::X>(); }
    namespace cs = cds::container;

    namespace defs { namespace {

        typedef cs::TreiberStack< cds::gc::DHP, int > Treiber_DHP;
        typedef cs::TreiberStack< cds::gc::DHP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::memory_model<cds::opt::v::relaxed_ordering>
            >::type
        > Treiber_DHP_relaxed;

        typedef cs::TreiberStack< cds::gc::DHP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::back_off< cds::backoff::yield>
            >::type
        > Treiber_DHP_yield;

        typedef cs::TreiberStack< cds::gc::DHP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::back_off< cds::backoff::yield>
                ,cds::opt::memory_model<cds::opt::v::relaxed_ordering>
            >::type
        > Treiber_DHP_yield_relaxed;

        typedef cs::TreiberStack< cds::gc::DHP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::back_off< cds::backoff::pause>
                ,cds::opt::allocator< std::allocator< bool * > >
            >::type
        > Treiber_DHP_pause_alloc;

        typedef cs::TreiberStack< cds::gc::DHP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::memory_model<cds::opt::v::relaxed_ordering>
                ,cds::opt::back_off< cds::backoff::pause>
                ,cds::opt::allocator< std::allocator< bool * > >
            >::type
        > Treiber_DHP_pause_alloc_relaxed;
    }}

    TEST(Treiber_DHP)
    TEST(Treiber_DHP_yield)
    TEST(Treiber_DHP_pause_alloc)

    TEST(Treiber_DHP_relaxed)
    TEST(Treiber_DHP_yield_relaxed)
    TEST(Treiber_DHP_pause_alloc_relaxed)

}
