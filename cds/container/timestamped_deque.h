/*
 * timestamped_deque.h
 *
 *  Created on: 17 февр. 2016 г.
 *      Author: lightwave
 */

#ifndef CDSLIB_CONTAINER_TIMESTAMPED_DEQUE_H_
#define CDSLIB_CONTAINER_TIMESTAMPED_DEQUE_H_

#include <cds/gc/hp.h>
#include <atomic>
#include <boost/thread.hpp>

namespace cds { namespace container {

///
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
 		class ThreadBuffer;
 		struct buffer_node;
 		struct node {
 			unsigned long timestamp;
 			T* item;
 			node(): timestamp(0) {}
 		};

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



	public:
	 	typedef T value_type;
	 	typedef Traits traits;
	 	typedef typename cds::details::Allocator<T, typename traits::allocator> allocator;
	 	typedef typename cds::details::Allocator<node, typename traits::node_allocator> node_allocator;
	 	typedef typename traits::item_counter item_counter;
	 	typedef std::atomic<buffer_node*> bnode_ptr;
	 	typedef typename ThreadBuffer::buffer_node   bnode;
	 	typedef cds::gc::HP::Guard guard;
	private:

 		ThreadBuffer t;
 		ThreadBuffer* localBuffers;
 		std::atomic<int> lastFree;
 		int maxThread;

 		bnode*** lastLefts;
 		bnode*** lastRights;
 		bool* wasEmpty;


 		boost::thread_specific_ptr<int> threadIndex;

 		int acquireIndex() {
 			int* temp = threadIndex.get();
 			if(temp == nullptr) {
 				int index = lastFree.load();
 				if(index >= maxThread)
 					return -1;
 				while(!lastFree.compare_exchange_strong(index, index + 1)) {
 					index = lastFree.load();
 				}
 				threadIndex.reset(new int(index));
 				return index;
 			}

 			return *temp;
 		}

 		bool doEmptyCheck() {
 			int threadIND = acquireIndex();
			guard* finded = nullptr;
			bool empty = true;
			bool exist = false;
			for(int i=0; i < maxThread; i++) {
				finded = localBuffers[i].getRight();
				empty = empty && checkEmptyCondition(finded, i);
				exist = finded != nullptr;
				delete finded;
			}
			empty = wasEmpty[threadIND] && empty;
			wasEmpty[threadIND] = !exist;
			return empty;
 		}
 		/*
 		 *  Helping fucntion for checking emptiness
 		 */
 		bool checkEmptyCondition(guard* finded , int i) {
 			int threadIND = acquireIndex();
			bnode *leftBorder = localBuffers[i].getLeftMost(),
				   *rightBorder = localBuffers[i].getRightMost();
			bool empty = leftBorder == rightBorder || (finded == nullptr
					&& lastLefts[threadIND][i] == leftBorder
					&& lastRights[threadIND][i] == rightBorder);
			lastLefts[threadIND][i] = leftBorder;
			lastRights[threadIND][i] = rightBorder;
			return empty;
 		}

 		guard* tryRemoveRight() throw(int) {
 			bool empty = true;
 			int threadIND = acquireIndex();

 			guard* rightest = nullptr;
 			unsigned long startTime = getTimestamp();
 			int bufferIndex = 0;
 			for(int i=0; i < maxThread; i++) {
 				guard* finded = localBuffers[i].getRight();

 				empty = empty && checkEmptyCondition(finded, i);

 				if(finded == nullptr)
					continue;
				if(rightest == nullptr) {
					rightest = finded;
					continue;
				}

 				if(isMoreR(finded->get<bnode>(), rightest->get<bnode>())) {
 					delete rightest;
 					rightest = finded;
 					bufferIndex = i;
 				} else {
 					delete finded;
 				}

 			}
 			empty = empty && wasEmpty[threadIND];
 			wasEmpty[threadIND] = rightest == nullptr;
 			if(empty && wasEmpty[threadIND])
 				return nullptr;

 			bnode* rightestNode = rightest->get<bnode>();
 			if(rightestNode->wasAddedRight()) {
 				if(localBuffers[bufferIndex].tryRemoveRight(rightest))
 					return rightest;
 			} else {
 				if(rightestNode->item->timestamp <= startTime)
 					if(localBuffers[bufferIndex].tryRemoveRight(rightest))
 						return rightest;
 			}
 			if(rightest != nullptr) {
				delete rightest;
 			}
 			throw -1;
 		}


 		guard* tryRemoveLeft() throw(int) {
 			bool empty = true;
 			int threadIND = acquireIndex();

 			guard* leftest = nullptr;
			unsigned long startTime = getTimestamp();
			int bufferIndex = 0;

			for(int i=0; i < maxThread; i++) {
				guard* finded = localBuffers[i].getLeft();

				empty = empty && checkEmptyCondition(finded, i);

				if(finded == nullptr)
					continue;
				if(leftest == nullptr) {
					leftest = finded;
					continue;
				}

				if(isMoreL(finded->get<bnode>(), leftest->get<bnode>())) {
					delete leftest;
					leftest = finded;
					bufferIndex = i;
				} else {
 					delete finded;
 				}
			}
 			empty = empty && wasEmpty[threadIND];
 			wasEmpty[threadIND] = leftest == nullptr;
 			if(empty && wasEmpty[threadIND])
 				return nullptr;


			bnode* leftestNode = leftest->get<bnode>();
			if(leftestNode->wasAddedLeft()) {
				if(localBuffers[bufferIndex].tryRemoveLeft(leftest))
					return leftest;
			} else {
				if(leftestNode->item->timestamp <= startTime)
					if(localBuffers[bufferIndex].tryRemoveLeft(leftest))
						return leftest;
			}
			if(leftest != nullptr) {
				delete leftest;
			}
			throw -1;
 		}

 		bool isMoreR(bnode* n1, bnode* n2) {
 			if(n1 == nullptr || n1->item->timestamp == 0)
 				return false;
 			else if (n2 == nullptr || n2->item->timestamp == 0)
 				return true;

 			node *t1 = n1->item, *t2 = n2->item;

 			if(n2->wasAddedLeft()) {
 				if(n1->wasAddedRight())
 					return true;
 				return (t1->timestamp < t2->timestamp);
 			} else {
 				if(n1->wasAddedLeft())
 					return false;
 				return (t1->timestamp > t2->timestamp);
 			}
 		}

 		bool isMoreL(bnode* n1, bnode* n2) {
 			if(n1 == nullptr || n1->item->timestamp == 0)
				return false;
			else if (n2 == nullptr || n2->item->timestamp == 0)
				return true;

 			node *t1 = n1->item, *t2 = n2->item;

 			if(n2->wasAddedLeft()) {
				if(n1->wasAddedRight())
					return false;
				return (t1->timestamp > t2->timestamp);
			} else {
				if(n1->wasAddedLeft())
					return true;
				return (t1->timestamp < t2->timestamp);
			}
 		}



		public:

		Timestamped_deque() {
			maxThread = cds::gc::HP::max_thread_count();
			localBuffers = new ThreadBuffer[maxThread];
			lastLefts = new bnode** [maxThread];
			// Initializing arrays for detecting empty state of container
			for(int i=0; i< maxThread; i++)
				lastLefts[i] = new bnode* [maxThread];
			lastRights = new bnode** [maxThread];
			for(int i=0; i< maxThread; i++)
				lastRights[i] = new bnode* [maxThread];
			wasEmpty = new bool [maxThread];
			for(int i=0; i< maxThread; i++)
				wasEmpty[i] = true;
			lastFree.store(0);
		}


		bool push_back(value_type const& value) {
			int index = acquireIndex();

			value_type* pvalue = allocator().New(value);
			node* timestamped = node_allocator().New();

			timestamped->item = pvalue;
			localBuffers[index].insertLeft(timestamped);
			unsigned long t = getTimestamp();
			timestamped->timestamp = t;
		}

		bool push_front(value_type const& value) {
			int index = acquireIndex();

			value_type* pvalue = allocator().New(value);
			node* timestamped = node_allocator().New();

			timestamped->item = pvalue;
			localBuffers[index].insertRight(timestamped);
			unsigned long t = getTimestamp();
			timestamped->timestamp = t;
		}

		bool push_back(
		            value_type&& value ///< Value to be moved to inserted element
		        ) {
			int index = acquireIndex();

			value_type* pvalue = allocator().New(value);
			node* timestamped = node_allocator().New();

			timestamped->item = pvalue;
			localBuffers[index].insertLeft(timestamped);
			unsigned long t = getTimestamp();
			timestamped->timestamp = t;
		}

		bool push_front(
					value_type&& value ///< Value to be moved to inserted element
				) {
			int index = acquireIndex();

			value_type* pvalue = allocator().New(value);
			node* timestamped = node_allocator().New();

			timestamped->item = pvalue;
			localBuffers[index].insertRight(timestamped);
			unsigned long t = getTimestamp();
			timestamped->timestamp = t;
		}

		value_type* pop_back() {
			guard* res;
			bool success = false;
			do {
				try {
					res = tryRemoveLeft();
					success = true;
				} catch(int err) {}
			} while(!success);
			bnode* temp = res->get<bnode>();
			res->clear();
			delete res;
			return temp->item->item;
		}

		value_type* pop_front() {
			guard* res;
			bool success = false;
			do {
				try {
					res = tryRemoveRight();
					success = true;
				} catch(int err) {}
			} while(!success);
			bnode* temp = res->get<bnode>();
			res->clear();
			delete res;
			return temp->item->item;
		}

		bool empty() {
			doEmptyCheck();
			return doEmptyCheck();
		}

		private:
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

		 				bool wasAddedRight() {
		 					return index > 0;
		 				}

		 				bool wasAddedLeft() {
		 					return index < 0;
		 				}

		 			};
		 		private:
		 			std::atomic<buffer_node*> leftMost;
		 			std::atomic<buffer_node*> rightMost;
		 			long lastIndex;
		 		public:

		 			typedef typename cds::details::Allocator<ThreadBuffer::buffer_node, typename traits::buffernode_allocator> buffernode_allocator;
		 			ThreadBuffer() : lastIndex(1) {
		 				buffer_node* newNode = buffernode_allocator().New();
		 				newNode->index = 0;
		 				newNode->item = nullptr;
		 				newNode->taken.store(true);
		 				leftMost.store(newNode);
		 				rightMost.store(newNode);
		 			}

		 			buffer_node* getLeftMost() {
		 				return leftMost.load();
		 			}

		 			buffer_node* getRightMost() {
						return rightMost.load();
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
		 							*oldLeft = leftMost.load();
		 				buffer_node* res = oldRight;


		 				while(true) {
		 					if(res->index < oldLeft->index) return nullptr;
		 					if(!res->taken.load()) {
		 						guard* guard = new cds::gc::HP::Guard();
		 						guard->protect(std::atomic<buffer_node*>(res));
		 						return guard;
		 					}
		 					if(res->left.load() == res) return nullptr;
		 					res = res->left.load();

		 				}
		 			}

		 			guard* getLeft() {
		 				buffer_node  *oldRight = rightMost.load(),
									 *oldLeft = leftMost.load();
						buffer_node* res = oldLeft;


						while(true) {
							if(res->index > oldRight->index) return nullptr;
							if(!res->taken.load()) {
								guard* guard = new cds::gc::HP::Guard();
								guard->protect(std::atomic<buffer_node*>(res));
								return guard;
							}
							if(res->right.load() == res) return nullptr;
							res = res->right.load();

						}
					}

		 			bool tryRemoveRight(guard* guard) {
		 				buffer_node* node = guard->get<buffer_node>();
		 				bool t = false;
		 				if(node->taken.compare_exchange_strong(t, true)) {
		 					return true;
		 				}
		 				std::cout << "Failed!\n";
		 				return false;
		 			}

		 			bool tryRemoveLeft(guard* guard) {
		 				buffer_node* node = guard->get<buffer_node>();
		 				bool t = false;
						if(node->taken.compare_exchange_strong(t, true)) {
							return true;
						}
						std::cout << "Failed!\n";
						return false;
		 			}

		 		};
	};
}} // namespace cds::container

#endif /* CDSLIB_CONTAINER_TIMESTAMPED_DEQUE_H_ */
