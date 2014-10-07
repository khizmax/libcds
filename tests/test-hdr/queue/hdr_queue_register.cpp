//$$CDS-header$$

#include "hdr_intrusive_msqueue.h"
#include "hdr_intrusive_segmented_queue.h"
#include "hdr_queue_new.h"
#include "hdr_queue.h"          //TODO must be removed after refactoring
#include "hdr_segmented_queue.h"

CPPUNIT_TEST_SUITE_REGISTRATION_( queue::HdrTestQueue,               s_HdrTestQueue );
CPPUNIT_TEST_SUITE_REGISTRATION_( queue::Queue_TestHeader,           s_Queue_TestHeader ); //TODO must be removed after refactoring
CPPUNIT_TEST_SUITE_REGISTRATION_( queue::HdrSegmentedQueue,          s_HdrSegmentedQueue );
CPPUNIT_TEST_SUITE_REGISTRATION_( queue::IntrusiveQueueHeaderTest,   s_IntrusiveQueueHeaderTest );
CPPUNIT_TEST_SUITE_REGISTRATION_( queue::HdrIntrusiveSegmentedQueue, s_HdrIntrusiveSegmentedQueue );
