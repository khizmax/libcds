//$$CDS-header$$

#include "hdr_treiber_stack.h"
#include <cds/gc/hp.h>
#include <cds/container/treiber_stack.h>


namespace stack {
#define TEST(X)     void TestStack::X() { test<defs::X>(); }
    namespace cs = cds::container;

    namespace defs { namespace {

        typedef cs::TreiberStack< cds::gc::HP, int > Treiber_HP;
        typedef cs::TreiberStack< cds::gc::HP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::memory_model<cds::opt::v::relaxed_ordering>
            >::type
        > Treiber_HP_relaxed;

        typedef cs::TreiberStack< cds::gc::HP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::back_off< cds::backoff::yield>
            >::type
        > Treiber_HP_yield;

        typedef cs::TreiberStack< cds::gc::HP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::back_off< cds::backoff::yield>
                , cds::opt::memory_model<cds::opt::v::relaxed_ordering>
            >::type
        > Treiber_HP_yield_relaxed;

        typedef cs::TreiberStack< cds::gc::HP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::back_off< cds::backoff::pause>
                ,cds::opt::allocator< std::allocator< bool * > >
            >::type
        > Treiber_HP_pause_alloc;

        typedef cs::TreiberStack< cds::gc::HP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::back_off< cds::backoff::pause>
                ,cds::opt::memory_model<cds::opt::v::relaxed_ordering>
                ,cds::opt::allocator< std::allocator< unsigned long > >
            >::type
        > Treiber_HP_pause_alloc_relaxed;
    }}

    TEST(Treiber_HP)
    TEST(Treiber_HP_yield)
    TEST(Treiber_HP_pause_alloc)

    TEST(Treiber_HP_relaxed)
    TEST(Treiber_HP_yield_relaxed)
    TEST(Treiber_HP_pause_alloc_relaxed)
}
CPPUNIT_TEST_SUITE_REGISTRATION(stack::TestStack);
