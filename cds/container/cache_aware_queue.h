#ifndef CDS_CONTAINER_CACHE_AWARE_QUEUE_H_
#define CDS_CONTAINER_CACHE_AWARE_QUEUE_H_

#include <atomic>
#include <stdexcept>
#include <boost/thread/tss.hpp>
#include <cds/container/details/base.h>
#include <cds/user_setup/cache_line.h>

namespace cds { namespace container {
	/// Cache aware queue
	/** @ingroup cds_nonintrusive_helper
	*/
	namespace cache_aware_queue {
		/// CAQueue default type traits
		struct traits
		{
			/// Node allocator
			using node_allocator = CDS_DEFAULT_ALLOCATOR;
			/// Item allocator
			using allocator = CDS_DEFAULT_ALLOCATOR;
			/// Item counting feature; by default, disabled. Use \p cds::atomicity::item_counter to enable item counting
			using item_counter = atomicity::empty_item_counter;
		};

        /// Metafunction converting option list to \p cache_aware_queue::traits
        /**
            Supported \p Options are:
            - \p opt::allocator - allocator (like \p std::allocator) used for allocating queue items. Default is \ref CDS_DEFAULT_ALLOCATOR
            - \p opt::node_allocator - allocator (like \p std::allocator) used for allocating queue nodes. Default is \ref CDS_DEFAULT_ALLOCATOR
            - \p opt::item_counter - the type of item counting feature. Default is \p cds::atomicity::empty_item_counter (item counting disabled)
                To enable item counting use \p cds::atomicity::item_counter
            Example: declare \p %CAQueue with item counting
            \code
            typedef cds::container::CAQueue< Foo,
                typename cds::container::cache_aware_queue::make_traits<
                    cds::opt::item_counter< cds::atomicity::item_counter > >
                >::type
            > CacheAwareQueue;
            \endcode
        */
        template <typename... Options> struct make_traits
		{
            typedef typename cds::opt::make_options<
                typename cds::opt::find_type_traits< traits, Options... >::type
                ,Options...
            >::type type;
        };
	} // namespace cache_aware_queue

    /// Cache aware lock-free queue
    /** @ingroup cds_nonintrusive_queue
        A lock-free queue implemented using a linked list of arrays, where each thread is avoiding accesses to global pointers in order to reduce number of cache misses.
        The queue does not require any garbage collector.
        Template arguments:
        - \p T is a type stored in the queue.
        - \p Traits - queue traits, default is \p cache_aware_queue::traits. You can use \p cache_aware_queue::make_traits
            metafunction to make your traits or just derive your traits from \p %cache_aware_queue::traits:
            \code
            struct myTraits: public cds::container::cache_aware_queue::traits {
                typedef cds::atomicity::item_counter    item_counter;
            };
            typedef cds::container::CAQueue< Foo, myTraits > myQueue;
            // Equivalent make_traits example:
            typedef cds::container::CAQueue< Foo,
                typename cds::container::cache_aware_queue::make_traits<
                    cds::opt::item_counter< cds::atomicity::item_counter >
                >::type
            > myQueue;
            \endcode
	*/
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
				std::atomic_uchar head {0};
				std::atomic_uchar tail {0};
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
			~node() {
				for(std::atomic<T*>& item: items) {
					T* ptr = item.load();
					if((ptr != (T*)free_item) && (ptr != (T*)removed_item)) {
						allocator().Delete(ptr);
					}
				}
			}
		};
	public:
		using value_type = T;  ///< The value type to be stored in the queue
		using traits = Traits; ///< Queue traits
		using allocator = cds::details::Allocator<T, typename traits::allocator>;              ///< Allocator type used for allocate/deallocate the queue items
		using node_allocator = cds::details::Allocator<node, typename traits::node_allocator>; ///< Allocator type used for allocate/deallocate the queue nodes
		using item_counter = typename traits::item_counter; ///< Item counting policy used

		// one accessor for thread
		class accessor
		{
			node_ptr head_node, tail_node;
			unsigned char a_head, a_tail;
			CAQueue& queue;
		public:
			accessor(CAQueue& parent): head_node(parent.head), tail_node(parent.tail), queue(parent) {
				a_head = head_node->info.head.load();
				a_tail = tail_node->info.tail.load();
			}
			bool enqueue(value_type* value) {
				node_ptr block = head_node;
				unsigned char head = a_head;

				while(true) {
					if(head == node::block_size) {
						node_ptr oldBlock = block;
						block->info.head = head;
						block = block->info.next;
						if(block == nullptr) {
							block = node_allocator().New();
							if(block == nullptr) return false;
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
						head = block->info.head.load();
					} else {
						if(block->items[head] == (T*)free_item) {
							value_type* free = (T*)free_item;
							// TODO: first allocate, then construct if CAS successful
							if(block->items[head].compare_exchange_strong(free, value)) {
								a_head = head + 1;
								queue.m_ItemCounter++;
								break;
							}
						} else {
							head += 1;
						}
					}
				} // while(true)
				return true;
			}
			value_type* dequeue() {
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
						tail = block->info.tail.load();
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
									queue.m_ItemCounter--;
									return value;
								}
							}
						}
					}
				} // while(true)
			}
		};
	private:
		node_ptr head, tail;
		boost::thread_specific_ptr<accessor> tls_ptr;
		item_counter m_ItemCounter;
	public:
		/// Initializes empty queue
		CAQueue(): head(node_allocator().New()), tail(head) {}

        //@cond
		CAQueue(const CAQueue&) = delete;
		CAQueue& operator=(const CAQueue&) = delete;
        //@endcond

		/// Enqueues \p val value into the queue
		bool enqueue( value_type const& val ) {
			if(tls_ptr.get() == nullptr) {
				tls_ptr.reset(new accessor(*this));
			}
			value_type* data = allocator().New(val);
			if(data == nullptr) return false;
			if(!tls_ptr->enqueue(data)) {
				allocator().Delete(data);
				return false;
			}
			return true;
		}
        /// Enqueues data to the queue using a functor
        /**
            \p Func is a functor called to create node.
            The functor \p f takes one argument - a reference to a new node of type \ref value_type :
            \code
            cds::container::CAQueue< Foo > myQueue;
            Bar bar;
            myQueue.enqueue_with( [&bar]( Foo& dest ) { dest = bar; } );
            \endcode
        */
        template <typename Func> bool enqueue_with( Func f )
        {
			if(tls_ptr.get() == nullptr) {
				tls_ptr.reset(new accessor(*this));
			}
			value_type* data = allocator().New();
			if(data == nullptr) return false;
			f(*data);
			if(!tls_ptr->enqueue(data)) {
				allocator().Delete(data);
				return false;
			}
			return true;
        }
        /// Synonym for \p enqueue() function
        bool push( value_type const& val )
        {
            return enqueue(val);
        }
        /// Synonym for \p enqueue_with() function
        template <typename Func> bool push_with( Func f )
        {
            return enqueue_with(f);
        }
        /// Enqueues data of type \ref value_type constructed from <tt>std::forward<Args>(args)...</tt>
        template <typename... Args>
        bool emplace( Args&&... args )
        {
        	value_type* data = allocator().New( std::forward<Args>(args)... );
			if(data == nullptr) return false;
			if(!tls_ptr->enqueue(data)) {
				allocator().Delete(data);
				return false;
			}
			return true;
        }
        /// Dequeues a value from the queue
        /**
            If queue is not empty, the function returns \p true, \p dest contains copy of
            dequeued value. The assignment operator for type \ref value_type is invoked.
            If queue is empty, the function returns \p false, \p dest is unchanged.
        */
        bool dequeue( value_type& dest )
        {
            return dequeue_with( [&dest]( value_type& src ) { dest = src; });
        }
        /// Dequeues a value using a functor
        /**
            \p Func is a functor called to copy dequeued value.
            The functor takes one argument - a reference to removed node:
            \code
            cds:container::CAQueue< Foo > myQueue;
            Bar bar;
            myQueue.dequeue_with( [&bar]( Foo& src ) { bar = std::move( src );});
            \endcode
            The functor is called only if the queue is not empty.
            If queue is empty, functor will not call
        */
        template <typename Func> bool dequeue_with( Func f )
        {
			if(tls_ptr.get() == nullptr) {
				tls_ptr.reset(new accessor(*this));
			}
            value_type * p = tls_ptr->dequeue();
            if ( p ) {
                f( *p );
                allocator().Delete(p);
                return true;
            }
            return false;
        }
        /// Synonym for \p dequeue_with() function
        template <typename Func> bool pop_with( Func f )
        {
            return dequeue_with( f );
        }
        /// Synonym for \p dequeue() function
        bool pop( value_type& dest )
        {
            return dequeue( dest );
        }
        /// Checks if the queue is empty
        bool empty() const
        {
            return (head == tail) && (head->info.head == tail->info.tail);
        }
        /// Clear the queue
		/**
			The function repeatedly calls \ref dequeue until it returns false.
		*/
        void clear()
        {
        	value_type v;
        	while(dequeue(v));
        }
        /// Returns queue's item count
        size_t size() const
        {
            return m_ItemCounter.value();
        }
        std::nullptr_t statistics() const
        {
            return nullptr;
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
