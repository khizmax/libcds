//$$CDS-header$$

#include "hdr_treiber_stack.h"
#include <cds/gc/hp.h>
#include <cds/container/treiber_stack.h>

namespace stack {
#define TEST(X)         void TestStack::X() { test<defs::X>(); }
#define TEST_DYN(X)     void TestStack::X() { test_elimination<defs::X>(); }

    namespace cs = cds::container;

    namespace defs { namespace {

        typedef cs::TreiberStack< cds::gc::HP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
            >::type
        > Elimination_HP;

        typedef cs::TreiberStack< cds::gc::HP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                , cds::opt::buffer< cds::opt::v::dynamic_buffer<void *> >
            >::type
        > Elimination_HP_dyn;

        typedef cs::TreiberStack< cds::gc::HP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                , cds::opt::buffer< cds::opt::v::dynamic_buffer<void *> >
                , cds::opt::stat< cs::treiber_stack::stat<> >
            >::type
        > Elimination_HP_stat;

        typedef cs::TreiberStack< cds::gc::HP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                , cds::opt::memory_model<cds::opt::v::relaxed_ordering>
            >::type
        > Elimination_HP_relaxed;

        typedef cs::TreiberStack< cds::gc::HP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                ,cds::opt::back_off< cds::backoff::yield>
            >::type
        > Elimination_HP_yield;

        typedef cs::TreiberStack< cds::gc::HP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::back_off< cds::backoff::yield>
                ,cds::opt::enable_elimination<true>
                , cds::opt::memory_model<cds::opt::v::relaxed_ordering>
            >::type
        > Elimination_HP_yield_relaxed;

        typedef cs::TreiberStack< cds::gc::HP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::back_off< cds::backoff::pause>
                ,cds::opt::allocator< std::allocator< bool * > >
                ,cds::opt::enable_elimination<true>
            >::type
        > Elimination_HP_pause_alloc;

        typedef cs::TreiberStack< cds::gc::HP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::back_off< cds::backoff::pause>
                ,cds::opt::memory_model<cds::opt::v::relaxed_ordering>
                ,cds::opt::allocator< std::allocator< bool * > >
                ,cds::opt::enable_elimination<true>
            >::type
        > Elimination_HP_pause_alloc_relaxed;

    }}

    TEST(Elimination_HP)
    TEST_DYN(Elimination_HP_dyn)
    TEST_DYN(Elimination_HP_stat)
    TEST(Elimination_HP_yield)
    TEST(Elimination_HP_pause_alloc)

    TEST(Elimination_HP_relaxed)
    TEST(Elimination_HP_yield_relaxed)
    TEST(Elimination_HP_pause_alloc_relaxed)
}
