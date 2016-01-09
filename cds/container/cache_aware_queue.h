#ifndef CDS_CONTAINER_CACHE_AWARE_QUEUE_H_
#define CDS_CONTAINER_CACHE_AWARE_QUEUE_H_

#include <atomic>
#include <stdexcept>
#include <cds/container/details/base.h>
#include <cds/user_setup/cache_line.h>

namespace cds { namespace container {

	namespace cache_aware_queue {
		struct traits
		{
			using node_allocator = CDS_DEFAULT_ALLOCATOR;
			using allocator = CDS_DEFAULT_ALLOCATOR;
		};

        template <typename... Options> struct make_traits
		{
            typedef typename cds::opt::make_options<
                typename cds::opt::find_type_traits< traits, Options... >::type
                ,Options...
            >::type type;
        };
	} // namespace cache_aware_queue

	template<class T, class Traits = cache_aware_queue::traits > class CAQueue
	{
	private:
		enum: size_t { null_item, free_item, removed_item };
		class node;
		class node_ptr
		{
			std::atomic<node*> ptr;
		public:
			node_ptr();
			node_ptr(node* unique);
			node_ptr(const node_ptr& other);
			node_ptr(node_ptr&& other);
			~node_ptr();
			node_ptr& operator=(const node_ptr& other);
			node_ptr& operator=(node_ptr&& other);
			bool operator==(const node_ptr& other) const;
			bool operator!=(const node_ptr& other) const { return !(*this == other); }
			node* operator->();
			const node* operator->() const;
			bool CAS(const node_ptr& compare, node_ptr& other);
			void reset(node* unique);
		};
		struct node
		{
			struct node_info {
				unsigned char head  = 0;
				unsigned char tail  = 0;
				std::atomic_uchar count  { 0       };
				std::atomic_bool deleted { false   };
				node_ptr next;
			} info;
			static const size_t block_size = (cds::c_nCacheLineSize - sizeof(node_info)) / sizeof(T*);
			std::atomic<T*> items[block_size];

			node() {
				for(std::atomic<T*>& ptr: items) {
					ptr.store((T*)free_item);
				}
			}
		};
	public:
		using value_type = T;
		using traits = Traits;
		using allocator = cds::details::Allocator<T, typename traits::allocator>;
		using node_allocator = cds::details::Allocator<node, typename traits::node_allocator>;

		// one accessor for thread
		class accessor
		{
			node_ptr head_node, tail_node;
			unsigned char a_head, a_tail;
			CAQueue& queue;
		public:
			accessor(CAQueue& parent): head_node(parent.head), tail_node(parent.tail), queue(parent) {
				a_head = head_node->info.head;
				a_tail = tail_node->info.tail;
			}
			void enqueue(const value_type& value) {
				node_ptr block = head_node;
				unsigned char head = a_head;

				while(true) {
					if(head == node::block_size) {
						node_ptr oldBlock = block;
						block->info.head = head;
						block = block->info.next;
						if(block == nullptr) {
							block = node_allocator().New();
							while((queue.head != oldBlock) && (oldBlock->info.next == nullptr)) {
								node_ptr headBlock = queue.head;
								if(headBlock->info.next != oldBlock) break;
								if(queue.head.CAS(headBlock, oldBlock)) break;
							}
							if(oldBlock->info.next.CAS(nullptr, block)) {
								queue.head.CAS(oldBlock, block);
							} else {
								block = oldBlock->info.next;
							}
						} else {
							if((block->info.head == node::block_size) && (block->info.next != nullptr)) {
								block = queue.head;
							}
						}
						head_node = block;
						head = block->info.head;
					} else {
						if(block->items[head] == (T*)free_item) {
							value_type* free = (T*)free_item;
							value_type* data = allocator().New(value);
							// TODO: first allocate, then construct if CAS successful
							if(block->items[head].compare_exchange_strong(free, data)) {
								a_head = head + 1;
								break;
							}
							allocator().Delete(data);
						} else {
							head += 1;
						}
					}
				} // while(true)
			}
			value_type* dequeue_ptr() {
				node_ptr block = tail_node;
				unsigned char tail = a_tail;
				while(true) {
					if(tail == node::block_size) {
						node_ptr oldBlock = block;
						block->info.tail = tail;
						block = block->info.next;
						if(block == nullptr) {
							return nullptr;
						} else {
							if(!oldBlock->info.deleted) {
								while((queue.tail != oldBlock) && (!oldBlock->info.deleted)) {
									node_ptr tailBlock = queue.tail;
									if(tailBlock->info.next != oldBlock) continue;
									if(queue.tail.CAS(tailBlock, oldBlock)) {
										//tailBlock.reset(nullptr);
									}
								}
								bool false_value = false;
								if(oldBlock->info.deleted.compare_exchange_strong(false_value, true)) {
									if(queue.tail.CAS(oldBlock, block)) {
										//oldBlock.reset(nullptr);
									}
								}
							}
							if(block->info.deleted) {
								block = queue.tail;
							}
						}
						tail_node = block;
						tail = block->info.tail;
					} else {
						T* value = block->items[tail].load();
						if(value == (T*)removed_item) {
							tail += 1;
						} else {
							T* free = (T*)free_item;
							if((value == (T*)free_item) && (block->items[tail].compare_exchange_strong(free, (T*)free_item))) {
								a_tail = tail;
								return nullptr;
							} else {
								if(block->items[tail].compare_exchange_strong(value, (T*)removed_item)) {
									a_tail = tail + 1;
									return value;
								}
							}
						}
					}
				} // while(true)
			}
			value_type dequeue() {
				T* value_ptr = dequeue_ptr();
				if(value_ptr != nullptr) {
					T value = std::move(*value_ptr);
					allocator().Delete(value_ptr);
					return std::move(value);
				}
				throw std::underflow_error("queue::dequeue(): empty queue");
			}
		};
	private:
		node_ptr head, tail;
	public:
		CAQueue(): head(node_allocator().New()), tail(head) {}
		~CAQueue() {
			node_ptr block = head;
			while(block != nullptr) {
				for(std::atomic<T*>& item: block->items) {
					T* ptr = item.load();
					if((ptr != (T*)free_item) && (ptr != (T*)removed_item)) {
						allocator().Delete(ptr);
					}
				}
				block = block->info.next;
			}
		}
	};

	template<class T, class Traits>
	CAQueue<T, Traits>::node_ptr::node_ptr(): ptr(nullptr) {}
	template<class T, class Traits>
	CAQueue<T, Traits>::node_ptr::node_ptr(node* unique): ptr(unique) { if(unique != nullptr) unique->info.count = 1; }
	template<class T, class Traits>
	CAQueue<T, Traits>::node_ptr::node_ptr(const node_ptr& other): ptr(other.ptr.load()) {
		node* pointer = ptr.load();
		if(pointer != nullptr) pointer->info.count++;
	}
	template<class T, class Traits>
	CAQueue<T, Traits>::node_ptr::node_ptr(node_ptr&& other): ptr(other.ptr.load()) {
		node* pointer = ptr.load();
		if(pointer != nullptr) pointer->info.count++;
	}
	template<class T, class Traits>
	CAQueue<T, Traits>::node_ptr::~node_ptr() {
		reset(nullptr);
	}
	template<class T, class Traits>
	typename CAQueue<T, Traits>::node_ptr& CAQueue<T, Traits>::node_ptr::operator=(const node_ptr& other) {
		node* pointer = other.ptr.load();
		reset(pointer);
		return *this;
	}
	template<class T, class Traits>
	typename CAQueue<T, Traits>::node_ptr& CAQueue<T, Traits>::node_ptr::operator=(node_ptr&& other) {
		node* pointer = other.ptr.load();
		reset(pointer);
		return *this;
	}
	template<class T, class Traits>
	bool CAQueue<T, Traits>::node_ptr::operator==(const node_ptr& other) const {
		return ptr.load() == other.ptr.load();
	}
	template<class T, class Traits>
	typename CAQueue<T, Traits>::node* CAQueue<T, Traits>::node_ptr::operator->() {
		return ptr.load();
	}
	template<class T, class Traits>
	const typename CAQueue<T, Traits>::node* CAQueue<T, Traits>::node_ptr::operator->() const {
		return ptr.load();
	}
	template<class T, class Traits>
	bool CAQueue<T, Traits>::node_ptr::CAS(const node_ptr& compare, node_ptr& other) {
		node* pointer_old = compare.ptr.load();
		node* pointer_new = other.ptr.load();
		if(ptr.compare_exchange_strong(pointer_old, pointer_new)) {
			if(pointer_new != nullptr) pointer_new->info.count++;
			if(pointer_old != nullptr) {
				pointer_old->info.count--;
				assert(pointer_old->info.count > 0);
			}
			return true;
		}
		return false;
	}
	template<class T, class Traits>
	void CAQueue<T, Traits>::node_ptr::reset(node* other) {
		node* pointer = ptr.load();
		if(pointer != nullptr) {
			unsigned char count = --pointer->info.count;
			if(count == 0) {
				node_allocator().Delete(pointer);
			}
		}
		ptr.store(other);
		if(other != nullptr) other->info.count++;
	}

}} // namespace cds::container

#endif /* CDS_CONTAINER_CACHE_AWARE_QUEUE_H_ */
