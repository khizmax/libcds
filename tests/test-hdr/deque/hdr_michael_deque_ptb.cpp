//$$CDS-header$$

#include "hdr_deque.h"
#include <cds/gc/ptb.h>
#include <cds/container/michael_deque.h>

namespace deque {
#define TEST(X) void DequeHeaderTest::test_##X() { test<X>(); }

    namespace cc = cds::container;
    namespace co = cds::opt;

    namespace {
        typedef cds::gc::PTB PTB;
        typedef deque::DequeHeaderTest::value_type value_type;

        typedef cc::MichaelDeque< PTB, value_type > MichaelDeque_PTB;

        /// PTB + item counter
        typedef cc::MichaelDeque< PTB, value_type
            ,co::item_counter< cds::atomicity::item_counter >
            ,co::memory_model< co::v::sequential_consistent >
        > MichaelDeque_PTB_ic;

        /// PTB + stat
        typedef cc::MichaelDeque< PTB, value_type
            ,co::stat< cds::intrusive::michael_deque::stat<> >
        > MichaelDeque_PTB_stat;

        /// PTB + noalign
        typedef cc::MichaelDeque< PTB, value_type
            ,co::alignment< co::no_special_alignment >
            ,co::item_counter< cds::atomicity::item_counter >
        > MichaelDeque_PTB_noalign;

    }   // namespace

    TEST( MichaelDeque_PTB)
    TEST( MichaelDeque_PTB_ic)
    TEST( MichaelDeque_PTB_stat)
    TEST( MichaelDeque_PTB_noalign)

} // namespace deque
