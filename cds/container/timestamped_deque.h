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
#include <atomic>

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
 			T* item;
 		};

	public:
	 	typedef T value_type;
	 	typedef Traits traits;
	 	typedef typename cds::details::Allocator<T, typename traits::allocator> allocator;
	 	typedef typename cds::details::Allocator<node, typename traits::node_allocator> node_allocator;
	 	typedef typename traits::item_counter item_counter;
	 	typedef std::atomic<buffer_node*> bnode_ptr;
	 	typedef cds::gc::HP::Guard guard;
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
 		public:
 			struct buffer_node {

 				buffer_node() {
 					taken.store(false);
 					left.store(this);
 					right.store(this);

 				}

 				std::atomic<buffer_node*> left;
 				std::atomic<buffer_node*> right;
 				node* item;
 				int index;
 				std::atomic<bool> taken;

 			};
 		private:
 			std::atomic<buffer_node*> leftMost;
 			std::atomic<buffer_node*> rightMost;
 			long lastIndex;
 		public:

 			typedef typename cds::details::Allocator<ThreadBuffer::buffer_node, typename traits::buffernode_allocator> buffernode_allocator;
 			ThreadBuffer() : lastIndex(0) {
 				buffer_node* newNode = buffernode_allocator().New();
 				newNode->index = 0;
 				newNode->item = nullptr;
 				newNode->taken.store(true);
 				leftMost.store(newNode);
 				rightMost.store(newNode);
 			}

 			void insertRight(node* timestamped) {
 				buffer_node* newNode = buffernode_allocator().New();
 				newNode->index = lastIndex;
				newNode->item = timestamped;
				lastIndex += 1;

				buffer_node* place = rightMost.load();
				while(place->left.load() != place && place->taken.load())
					place = place->left.load();
				if(place->left.load() == place)
					leftMost.store(place);
				newNode->left.store(place);
				place->right.store(newNode);
				rightMost.store(newNode);
 			}

 			void insertLeft(node* timestamped) {
 				buffer_node* newNode = buffernode_allocator().New();
 				newNode->index = -lastIndex;
				newNode->item = timestamped;
				lastIndex += 1;

				buffer_node* place = leftMost.load();
				while(place->right.load() != place && place->taken.load())
					place = place->right.load();
				if(place->right.load() == place)
					rightMost.store(place);
				newNode->right.store(place);
				place->left.store(newNode);
				leftMost.store(newNode);

 			}

 			guard* getRight() {
 				buffer_node* oldRight = rightMost.load(),
 							 oldLeft = leftMost.load();
 				buffer_node* res = oldRight;
 				guard* guard = new cds::gc::HP::Guard();

 				while(true) {
 					if(res->index < oldLeft->index) return nullptr;
 					if(!res->taken.load()) {
 						guard->protect(res);
 						return guard;
 					}
 					if(res->left.load() == res) return nullptr;
 					res = res->left.load();

 				}

 				return nullptr;
 			}

 			guard* getLeft() {
 				buffer_node  *oldRight = rightMost.load(),
							 *oldLeft = leftMost.load();
				buffer_node* res = oldLeft;
				guard* guard = new cds::gc::HP::Guard();

				while(true) {
					if(res->index > oldRight->index) return nullptr;
					if(!res->taken.load()) {
						guard->protect(std::atomic<buffer_node*>(res));
						return guard;
					}
					if(res->right.load() == res) return nullptr;
					res = res->right.load();

				}

				return nullptr;
			}

 			bool tryRemoveRight(guard* guard) {
 				buffer_node* node = guard->get<buffer_node*>();
 				if(node->taken.compare_exchange_strong(false, true)) {
 					return true;
 				}
 				return false;
 			}

 			bool tryRemoveLeft(guard* guard) {
 				buffer_node* node = guard->get<buffer_node*>();
				if(node->taken.compare_exchange_strong(false, true)) {
					return true;
				}
				return false;
 			}

 		};

 		ThreadBuffer t;

		public:

		Timestamped_deque() {
			std::cout << "Timestamped_deque";
		}


		bool push_back(value_type const& value) {
			value_type* pvalue = allocator().New(value);
			node* timestamped = node_allocator().New();
			timestamped->item = pvalue;
			t.insertLeft(timestamped);
			unsigned long t = getTimestamp();
			timestamped->timestamp = t;
		}
		typedef typename ThreadBuffer::buffer_node   bnode;
		value_type* pop_back() {
			guard* res = t.getLeft();
			bnode* temp = res->get<bnode>();
			return temp->item->item;
		}
	};
}} // namespace cds::container

#endif /* CDSLIB_CONTAINER_TIMESTAMPED_DEQUE_H_ */
