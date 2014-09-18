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

        typedef ci::TsigasCycleQueue<
            item
            ,co::buffer< co::v::static_buffer< int, 1024 > >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::memory_model< co::v::sequential_consistent >
        > TsigasCycleQueue_stat;

        typedef ci::TsigasCycleQueue<
            item
            ,co::buffer< co::v::static_buffer< int, 1024 > >
            ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            ,co::item_counter< cds::atomicity::item_counter >
            ,co::memory_model< co::v::relaxed_ordering >
        > TsigasCycleQueue_stat_ic;

        class TsigasCycleQueue_dyn
            : public ci::TsigasCycleQueue<
                item
                ,co::buffer< co::v::dynamic_buffer< int > >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            >
        {
            typedef ci::TsigasCycleQueue<
                item
                ,co::buffer< co::v::dynamic_buffer< int > >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
            > base_class;
        public:
            TsigasCycleQueue_dyn()
                : base_class( 1024 )
            {}
        };

        class TsigasCycleQueue_dyn_ic
            : public ci::TsigasCycleQueue<
                item
                ,co::buffer< co::v::dynamic_buffer< int > >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
            >
        {
            typedef ci::TsigasCycleQueue<
                item
                ,co::buffer< co::v::dynamic_buffer< int > >
                ,ci::opt::disposer< IntrusiveQueueHeaderTest::faked_disposer >
                ,co::item_counter< cds::atomicity::item_counter >
            > base_class;
        public:
            TsigasCycleQueue_dyn_ic()
                : base_class( 1024 )
            {}
        };
    }

    TEST(TsigasCycleQueue_stat)
    TEST(TsigasCycleQueue_stat_ic)
    TEST(TsigasCycleQueue_dyn)
    TEST(TsigasCycleQueue_dyn_ic)

} // namespace queue
