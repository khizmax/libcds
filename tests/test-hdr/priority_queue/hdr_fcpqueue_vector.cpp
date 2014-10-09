//$$CDS-header$$

#include "priority_queue/hdr_pqueue.h"
#include <cds/container/fcpriority_queue.h>

namespace priority_queue {

    void PQueueHdrTest::FCPQueue_vector()
    {
        typedef cds::container::FCPriorityQueue< PQueueHdrTest::value_type > pqueue_type;
        test_fcpqueue<pqueue_type>();
    }

    void PQueueHdrTest::FCPQueue_vector_stat()
    {
        struct pqueue_traits : public cds::container::fcpqueue::traits
        {
            typedef cds::container::fcpqueue::stat<> stat;
        };
        typedef cds::container::FCPriorityQueue<
            PQueueHdrTest::value_type
            ,std::priority_queue<
                PQueueHdrTest::value_type
                ,std::vector<PQueueHdrTest::value_type>
                ,PQueueHdrTest::less
            >
            ,pqueue_traits
        > pqueue_type;
        test_fcpqueue<pqueue_type>();
    }

    void PQueueHdrTest::FCPQueue_vector_mutex()
    {
        typedef cds::container::FCPriorityQueue<
            PQueueHdrTest::value_type
            ,std::priority_queue< PQueueHdrTest::value_type >
            ,cds::container::fcpqueue::make_traits<
                cds::opt::lock_type< std::mutex >
            >::type
        > pqueue_type;
        test_fcpqueue<pqueue_type>();
    }

} // namespace priorty_queue
