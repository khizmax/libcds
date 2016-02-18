/*
 * timestamped_deque.h
 *
 *  Created on: 17 февр. 2016 г.
 *      Author: lightwave
 */

#ifndef CDSLIB_CONTAINER_TIMESTAMPED_DEQUE_H_
#define CDSLIB_CONTAINER_TIMESTAMPED_DEQUE_H_

#include <cds/algo/flat_combining.h>
#include <cds/algo/elimination_opt.h>
#include <deque>
#include <cds/gc/hp.h>

namespace cds { namespace container {

/// Cache aware queue
 	/** @ingroup cds_nonintrusive_helper
 	*/
 	namespace timestamped_deque {
 		/// timestamped_deque default type traits
 		struct traits
 		{
 			/// Node allocator
 			using node_allocator = CDS_DEFAULT_ALLOCATOR;

 			using buffernode_allocator = CDS_DEFAULT_ALLOCATOR;
 			/// Item allocator
 			using allocator = CDS_DEFAULT_ALLOCATOR;
 			/// Item counting feature; by default, disabled. Use \p cds::atomicity::item_counter to enable item counting
 			using item_counter = atomicity::empty_item_counter;
 		};

         /// Metafunction converting option list to \p timestamped_deque::traits
         /**
             Supported \p Options are:
             - \p opt::allocator - allocator (like \p std::allocator) used for allocating queue items. Default is \ref CDS_DEFAULT_ALLOCATOR
             - \p opt::node_allocator - allocator (like \p std::allocator) used for allocating queue nodes. Default is \ref CDS_DEFAULT_ALLOCATOR
             - \p opt::item_counter - the type of item counting feature. Default is \p cds::atomicity::empty_item_counter (item counting disabled)
                 To enable item counting use \p cds::atomicity::item_counter
             Example: declare \p %CAQueue with item counting
             \code
             typedef cds::container::CAQueue< Foo,
                 typename cds::container::timestamped_deque::make_traits<
                     cds::opt::item_counter< cds::atomicity::item_counter > >
                 >::type
             > TimestampedDeque;
             \endcode
         */
         template <typename... Options> struct make_traits
 		{
             typedef typename cds::opt::make_options<
                 typename cds::opt::find_type_traits< traits, Options... >::type
                 ,Options...
             >::type type;
         };
 	} // namespace timestamped_deque
 	template <typename T, typename Traits = cds::container::timestamped_deque::traits>
	class Timestamped_deque {
 		struct buffer_node;
 		struct node {
 			unsigned long timestamp;
 			T item;
 		};

	public:
	 	typedef T value_type;
	 	typedef Traits traits;
	 	typedef typename cds::details::Allocator<T, typename traits::allocator> allocator;
	 	typedef typename cds::details::Allocator<node, typename traits::node_allocator> node_allocator;
	 	typedef typename cds::details::Allocator<node, typename traits::buffernode_allocator> buffernode_allocator;
	 	typedef typename traits::item_counter item_counter;
	 	typedef cds::gc::hp::guard* guard;
	private:
 		inline unsigned long getTimestamp ()
 		{
 			uint32_t time_edx1, time_eax1;
 			unsigned long time_last;
 			asm volatile (  "rdtscp\n\t"
 							"mov %%edx, %0\n\t"
 							"mov %%eax, %1\n\t"
 							"cpuid\n\t" : "=r"(time_edx1), "=r"(time_eax1) ::
 							"%rax", "%rbx", "%rcx", "%rdx");

 			time_last =
 						(static_cast<unsigned long long>(time_edx1) << 32 | static_cast<unsigned long long>(time_eax1));
 			return time_last;
 		}

 		class ThreadBuffer {


 			struct buffer_node {
 				buffer_node* left;
 				buffer_node* right;
 				node item;
 				int index;
 				bool taken;

 			};



 			guard leftend;
 			guard rightend;

 		public:
 			ThreadBuffer() {

 			}

 			void insertRight(node timestamped) {

 			}

 			void insertLeft(node timestamped) {

 			}

 			guard getRight() {
 				return nullptr;
 			}

 			guard getLeft() {
				return nullptr;
			}

 			bool tryRemoveRight(guard oldRight, buffer_node* node) {
 				return true;
 			}

 			bool tryRemoveLeft(guard oldRight, buffer_node* node) {
 				return true;
 			}

 		};

		public:



			Timestamped_deque() {
				std::cout << "Timestamped_deque";
			}
	};
}} // namespace cds::container

#endif /* CDSLIB_CONTAINER_TIMESTAMPED_DEQUE_H_ */
