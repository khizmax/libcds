#include "test_generic_queue.h"

#include <cds/container/williams_queue_spsc.h>

namespace {
	namespace cc = cds::container;

	class WilliamsQueueSPSC : public cds_test::generic_queue
	{};

	TEST_F(WilliamsQueueSPSC, defaulted)
	{
		typedef cc::WilliamsQueueSPSC< int > test_queue;

		test_queue q;
		test(q);
	}

	TEST_F(WilliamsQueueSPSC, item_counting)
	{
		typedef cds::container::WilliamsQueueSPSC< int,
			typename cds::container::williams_queue_spsc::make_traits <
			cds::opt::item_counter< cds::atomicity::item_counter >
			> ::type
		> test_queue;

		test_queue q;
		test(q);
	}

} // namespace

