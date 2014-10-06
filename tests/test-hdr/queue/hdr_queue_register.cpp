//$$CDS-header$$

#include "hdr_intrusive_msqueue.h"
#include "hdr_intrusive_segmented_queue.h"
#include "hdr_queue_new.h"
#include "hdr_queue.h"          //TODO must be removed after refactoring
#include "hdr_segmented_queue.h"

CPPUNIT_TEST_SUITE_REGISTRATION( queue::HdrTestQueue );
CPPUNIT_TEST_SUITE_REGISTRATION( queue::Queue_TestHeader); //TODO must be removed after refactoring
CPPUNIT_TEST_SUITE_REGISTRATION( queue::HdrSegmentedQueue );
CPPUNIT_TEST_SUITE_REGISTRATION( queue::IntrusiveQueueHeaderTest );
CPPUNIT_TEST_SUITE_REGISTRATION( queue::HdrIntrusiveSegmentedQueue );
