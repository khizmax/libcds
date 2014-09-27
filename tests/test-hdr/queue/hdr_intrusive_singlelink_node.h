//$$CDS-header$$

#ifndef CDSTEST_HDR_TEST_INTRUSIVE_SINGLELINK_NODE_H
#define CDSTEST_HDR_TEST_INTRUSIVE_SINGLELINK_NODE_H

#include <cds/intrusive/details/single_link_struct.h>

namespace queue {
    namespace ci = cds::intrusive;
    namespace co = cds::opt;

    template <typename GC>
    struct base_hook_item: public ci::single_link::node< GC >
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
        ci::single_link::node< GC > hMember;

        member_hook_item()
            : nDisposeCount(0)
        {}
    };

} // queue

#endif // #ifndef CDSTEST_HDR_TEST_INTRUSIVE_SINGLELINK_NODE_H
