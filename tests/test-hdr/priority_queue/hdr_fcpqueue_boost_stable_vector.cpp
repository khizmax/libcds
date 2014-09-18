//$$CDS-header$$

#include "priority_queue/hdr_pqueue.h"
#include <cds/container/fcpriority_queue.h>
#include <boost/container/stable_vector.hpp>
#include <boost/container/deque.hpp>

namespace priority_queue {

    void PQueueHdrTest::FCPQueue_stablevector()
    {
        typedef cds::container::FCPriorityQueue<
            PQueueHdrTest::value_type
            ,std::priority_queue<
                PQueueHdrTest::value_type
                ,boost::container::stable_vector<PQueueHdrTest::value_type>
                ,PQueueHdrTest::less
            >
        > pqueue_type;
        test_fcpqueue<pqueue_type>();
    }

    void PQueueHdrTest::FCPQueue_stablevector_stat()
    {
        typedef cds::container::FCPriorityQueue<
            PQueueHdrTest::value_type
            ,std::priority_queue<
                PQueueHdrTest::value_type
                ,boost::container::stable_vector<PQueueHdrTest::value_type>
                ,PQueueHdrTest::less
            >
            ,cds::container::fcpqueue::make_traits<
                cds::opt::stat< cds::container::fcpqueue::stat<> >
            >::type
        > pqueue_type;
        test_fcpqueue<pqueue_type>();
    }

        void PQueueHdrTest::FCPQueue_boost_deque()
    {
        typedef cds::container::FCPriorityQueue<
            PQueueHdrTest::value_type
            ,std::priority_queue<
                PQueueHdrTest::value_type
                ,boost::container::deque<PQueueHdrTest::value_type>
                ,PQueueHdrTest::less
            >
        > pqueue_type;
        test_fcpqueue<pqueue_type>();
    }

    void PQueueHdrTest::FCPQueue_boost_deque_stat()
    {
        typedef cds::container::FCPriorityQueue<
            PQueueHdrTest::value_type
            ,std::priority_queue<
                PQueueHdrTest::value_type
                ,boost::container::deque<PQueueHdrTest::value_type>
                ,PQueueHdrTest::less
            >
            ,cds::container::fcpqueue::make_traits<
                cds::opt::stat< cds::container::fcpqueue::stat<> >
            >::type
        > pqueue_type;
        test_fcpqueue<pqueue_type>();
    }

} // namespace priorty_queue
