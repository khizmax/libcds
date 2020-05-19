#include "test_generic_queue.h"
 
  #include <cds/container/williams_queue.h>
 
  namespace {
 	namespace cc = cds::container;
 
  	class WilliamsQueue : public cds_test::generic_queue
 	{};
 
  	TEST_F(WilliamsQueue, defaulted)
 	{
 		typedef cds::container::WilliamsQueue< int > test_queue;
 
  		test_queue q;
 		test(q);
 	}
 
  	TEST_F(WilliamsQueue, item_counting)
 	{
 		typedef cds::container::WilliamsQueue< int,
 			typename cds::container::williams_queue::make_traits <
 			cds::opt::item_counter< cds::atomicity::item_counter >
 			> ::type
 		> test_queue;
 
  		test_queue q;
 		test(q);
 	}
 
  	TEST_F(WilliamsQueue, move)
 	{
 		typedef cds::container::WilliamsQueue< std::string > test_queue;
 
  		test_queue q;
 		test_string(q);
 	}
 
  	TEST_F(WilliamsQueue, move_item_counting)
 	{
 		struct traits : public cc::williams_queue::traits
 		{
 			typedef cds::atomicity::item_counter item_counter;
 		};
 		typedef cds::container::WilliamsQueue< std::string, traits > test_queue;
 
  		test_queue q;
 		test_string(q);
 	}
 
  } // namespace
