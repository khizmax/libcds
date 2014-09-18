//$$CDS-header$$

#include "hdr_deque.h"
#include <cds/gc/hp.h>
#include <cds/container/michael_deque.h>

namespace deque {
#define TEST(X) void DequeHeaderTest::test_##X() { test<X>(); }

    namespace cc = cds::container;
    namespace co = cds::opt;

    namespace {
        typedef cds::gc::HP HP;
        typedef deque::DequeHeaderTest::value_type value_type;

        typedef cc::MichaelDeque< HP, value_type > MichaelDeque_HP;

        /// HP + item counter
        typedef cc::MichaelDeque< HP, value_type
            ,co::item_counter< cds::atomicity::item_counter >
            ,co::memory_model< co::v::sequential_consistent >
        > MichaelDeque_HP_ic;

        /// HP + stat
        typedef cc::MichaelDeque< HP, value_type
            ,co::stat< cds::intrusive::michael_deque::stat<> >
        > MichaelDeque_HP_stat;

        /// HP + noalign
        typedef cc::MichaelDeque< HP, value_type
            ,co::alignment< co::no_special_alignment >
            ,co::item_counter< cds::atomicity::item_counter >
        > MichaelDeque_HP_noalign;

    }   // namespace

    TEST( MichaelDeque_HP)
    TEST( MichaelDeque_HP_ic)
    TEST( MichaelDeque_HP_stat)
    TEST( MichaelDeque_HP_noalign)

} // namespace deque

CPPUNIT_TEST_SUITE_REGISTRATION(deque::DequeHeaderTest);
