//$$CDS-header$$

#include "hdr_intrusive_msqueue.h"
#include "hdr_intrusive_segmented_queue.h"
#include "hdr_queue.h"
#include "hdr_fcqueue.h"
#include "hdr_segmented_queue.h"

CPPUNIT_TEST_SUITE_REGISTRATION_( queue::HdrTestQueue,               s_HdrTestQueue );
CPPUNIT_TEST_SUITE_REGISTRATION_( queue::HdrFCQueue,           s_Queue_TestHeader ); //TODO must be removed after refactoring
CPPUNIT_TEST_SUITE_REGISTRATION_( queue::HdrSegmentedQueue,          s_HdrSegmentedQueue );
CPPUNIT_TEST_SUITE_REGISTRATION_( queue::IntrusiveQueueHeaderTest,   s_IntrusiveQueueHeaderTest );
CPPUNIT_TEST_SUITE_REGISTRATION_( queue::HdrIntrusiveSegmentedQueue, s_HdrIntrusiveSegmentedQueue );
