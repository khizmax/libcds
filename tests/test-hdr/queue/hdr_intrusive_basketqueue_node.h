//$$CDS-header$$

#ifndef CDSTEST_HDR_TEST_INTRUSIVE_BASKET_QUEUE_NODE_H
#define CDSTEST_HDR_TEST_INTRUSIVE_BASKET_QUEUE_NODE_H

#include <cds/intrusive/basket_queue.h>

namespace queue {
    namespace ci = cds::intrusive;
    namespace co = cds::opt;

    namespace basket_queue {
        template <typename GC>
        struct base_hook_item: public ci::basket_queue::node< GC >
        {
            int nVal;
            int nDisposeCount;

            base_hook_item()
                : nDisposeCount(0)
            {}
        };

        template <typename GC>
        struct member_hook_item
        {
            int nVal;
            int nDisposeCount;
            ci::basket_queue::node< GC > hMember;

            member_hook_item()
                : nDisposeCount(0)
            {}
        };

    } // namespace basket_queue
} // queue

#endif // #ifndef CDSTEST_HDR_TEST_INTRUSIVE_BASKET_QUEUE_NODE_H
