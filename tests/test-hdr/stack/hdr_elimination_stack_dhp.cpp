//$$CDS-header$$

#include "hdr_treiber_stack.h"
#include <cds/gc/dhp.h>
#include <cds/container/treiber_stack.h>

namespace stack {
#define TEST(X)     void TestStack::X() { test<defs::X>(); }
#define TEST_DYN(X)     void TestStack::X() { test_elimination<defs::X>(); }
    namespace cs = cds::container;

    namespace defs { namespace {

        typedef cs::TreiberStack< cds::gc::DHP, int
            ,typename cs::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
            >::type
        > Elimination_DHP;


        typedef cs::TreiberStack< cds::gc::DHP, int
            ,typename cs::treiber_stack::make_traits<
                cds::opt::enable_elimination<true>
                ,cds::opt::buffer< cds::opt::v::dynamic_buffer<void *> >
            >::type
        > Elimination_DHP_dyn;

        typedef cs::TreiberStack< cds::gc::DHP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::buffer< cds::opt::v::dynamic_buffer<int> >
                ,cds::opt::stat< cs::treiber_stack::stat<> >
                ,cds::opt::enable_elimination<true>
            >::type
        > Elimination_DHP_stat;

        typedef cs::TreiberStack< cds::gc::DHP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::memory_model<cds::opt::v::relaxed_ordering>
                ,cds::opt::enable_elimination<true>
            >::type
        > Elimination_DHP_relaxed;

        typedef cs::TreiberStack< cds::gc::DHP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::back_off< cds::backoff::yield>
                ,cds::opt::enable_elimination<true>
            >::type
        > Elimination_DHP_yield;

        typedef cs::TreiberStack< cds::gc::DHP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::back_off< cds::backoff::yield>
                ,cds::opt::memory_model<cds::opt::v::relaxed_ordering>
                ,cds::opt::enable_elimination<true>
            >::type
        > Elimination_DHP_yield_relaxed;

        typedef cs::TreiberStack< cds::gc::DHP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::back_off< cds::backoff::pause >
                ,cds::opt::allocator< std::allocator< bool * > >
                ,cds::opt::enable_elimination<true>
            >::type
        > Elimination_DHP_pause_alloc;

        typedef cs::TreiberStack< cds::gc::DHP, int
            , typename cs::treiber_stack::make_traits<
                cds::opt::memory_model<cds::opt::v::relaxed_ordering>
                ,cds::opt::back_off< cds::backoff::pause>
                ,cds::opt::allocator< std::allocator< bool * > >
                ,cds::opt::enable_elimination<true>
            >::type
        > Elimination_DHP_pause_alloc_relaxed;
    }}

    TEST(Elimination_DHP)
    TEST_DYN(Elimination_DHP_dyn)
    TEST_DYN(Elimination_DHP_stat)
    TEST(Elimination_DHP_yield)
    TEST(Elimination_DHP_pause_alloc)

    TEST(Elimination_DHP_relaxed)
    TEST(Elimination_DHP_yield_relaxed)
    TEST(Elimination_DHP_pause_alloc_relaxed)

}
