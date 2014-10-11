//$$CDS-header$$

#include "hdr_intrusive_msqueue.h"
#include <cds/intrusive/tsigas_cycle_queue.h>

namespace queue {

#define TEST(X)     void IntrusiveQueueHeaderTest::test_##X() { test2<X>(); }

    namespace {
        struct item {
            int nVal;
            int nDisposeCount;

            item()
                : nDisposeCount(0)
            {}
        };

        struct traits_TsigasCycleQueue_static : public cds::intrusive::tsigas_queue::traits
        {
            typedef co::v::static_buffer< int, 1024 > buffer;
            typedef IntrusiveQueueHeaderTest::faked_disposer disposer;
            typedef co::v::sequential_consistent memory_model;
        };
        typedef ci::TsigasCycleQueue< item, traits_TsigasCycleQueue_static > TsigasCycleQueue_static;

        struct traits_traits_TsigasCycleQueue_static_ic : public traits_TsigasCycleQueue_static
        {
            typedef cds::atomicity::item_counter item_counter;
        };
        typedef ci::TsigasCycleQueue< item, traits_traits_TsigasCycleQueue_static_ic > TsigasCycleQueue_static_ic;

        class TsigasCycleQueue_dyn
            : public ci::TsigasCycleQueue< item,
                typename ci::tsigas_queue::make_traits<
                    co::buffer< co::v::dynamic_buffer< int > >
                    ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                >::type
            >
        {
            typedef ci::TsigasCycleQueue< item,
                typename ci::tsigas_queue::make_traits<
                   co::buffer< co::v::dynamic_buffer< int > >
                    , ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                >::type
            > base_class;
        public:
            TsigasCycleQueue_dyn()
                : base_class( 1024 )
            {}
        };

        class TsigasCycleQueue_dyn_ic
            : public ci::TsigasCycleQueue< item,
                typename ci::tsigas_queue::make_traits<
                    co::buffer< co::v::dynamic_buffer< int > >
                    ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                    ,co::item_counter< cds::atomicity::item_counter >
                >::type
            >
        {
            typedef ci::TsigasCycleQueue< item,
                typename ci::tsigas_queue::make_traits<
                    co::buffer< co::v::dynamic_buffer< int > >
                    , ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                    , co::item_counter< cds::atomicity::item_counter >
                >::type
            > base_class;
        public:
            TsigasCycleQueue_dyn_ic()
                : base_class( 1024 )
            {}
        };
    }

    TEST(TsigasCycleQueue_static)
    TEST(TsigasCycleQueue_static_ic)
    TEST(TsigasCycleQueue_dyn)
    TEST(TsigasCycleQueue_dyn_ic)

} // namespace queue
