//$$CDS-header$$

#ifndef CDSLIB_CONTAINER_WILLIAMS_QUEUE_H
#define CDSLIB_CONTAINER_WILLIAMS_QUEUE_H

#include <memory>
#include <cds/container/details/base.h>

#ifdef UNICODE
#define tstring std::wstring
#define tostringstream std::wostringstream
#else
#define tstring std::string                      
#define tostringstream std::ostringstream
#endif
#define FMTDBGSTR(stream)  ((tostringstream&)(tostringstream() << tstring() << stream)).str().c_str() 

namespace cds {
	namespace container {
		/// Anthony Williams' queue
		/** @ingroup cds_nonintrusive_helper
		*/
		namespace williams_queue {
			/// WilliamsQueue default type traits
			struct traits
			{
				/// Node allocator
				typedef CDS_DEFAULT_ALLOCATOR allocator;

				/// Item counting feature; by default, disabled. Use \p cds::atomicity::item_counter to enable item counting
				typedef atomicity::empty_item_counter item_counter;
			};

			/// Metafunction converting option list to \p williams_queue::traits
			/**
			Supported \p Options are:
			- \p opt::allocator - allocator (like \p std::allocator) used for allocating queue nodes. Default is \ref CDS_DEFAULT_ALLOCATOR
			- \p opt::item_counter - the type of item counting feature. Default is \p cds::atomicity::empty_item_counter (item counting disabled)
			To enable item counting use \p cds::atomicity::item_counter

			Example: declare \p %WilliamsQueue with item counting
			\code
			typedef cds::container::WilliamsQueue< Foo,
			typename cds::container::williams_queue::make_traits<
			cds::opt::item_counter< cds::atomicity::item_counter > >
			>::type
			> williamsQueue;
			\endcode
			*/
			template <typename... Options>
			struct make_traits {
#ifdef CDS_DOXYGEN_INVOKED
				typedef implementation_defined type;   ///< Metafunction result
#else
				typedef typename cds::opt::make_options<
					typename cds::opt::find_type_traits< traits, Options... >::type
					, Options...
				>::type type;
#endif
			};
		} // namespace williams_queue

		  /// Williams lock-free queue
		  /** @ingroup cds_nonintrusive_queue
		  The queue uses in-built internal and external reference counting for each node.
		  The queue does not require any garbage collector.

		  <b>Source</b>
		  - [2012] Anthony Williams "C++ Concurrency in Action"

		  Template arguments:
		  - \p T is a type stored in the queue.
		  - \p Traits - queue traits, default is \p williams_queue::traits. You can use \p williams_queue::make_traits
		  metafunction to make your traits or just derive your traits from \p %williams_queue::traits:
		  \code
		  struct myTraits: public cds::container::williams_queue::traits {
		  typedef cds::atomicity::item_counter    item_counter;
		  };
		  typedef cds::container::WilliamsQueue< Foo, myTraits > myQueue;

		  // Equivalent make_traits example:
		  typedef cds::container::WilliamsQueue< Foo,
		  typename cds::container::williams_queue::make_traits<
		  cds::opt::item_counter< cds::atomicity::item_counter >
		  >::type
		  > myQueue;
		  \endcode
		  */
		template <typename T, typename Traits = williams_queue::traits>
		class WilliamsQueue
		{
		public:
			/// Rebing template arguments
			template <typename T2, typename Traits2>
			struct rebind {
				typedef WilliamsQueue< T2, Traits2 > other;    ///< Rebinding result
			};

			typedef T value_type;   ///< The value type to be stored in the queue
			typedef Traits traits;  ///< Queue traits

			typedef typename traits::item_counter item_counter; ///< Item counting policy used

		protected:
			//@cond
			struct node_type;
			/// Wraps external count together with the node_type pointer
			struct counted_node_ptr
			{
				int  external_count;
				node_type * ptr;
			};

			/**
			Note that we need only 2 bits for the external_counters
			because there are at most two such counters.
			By using a bit field for this and specifying
			internal_count as a 30-bit value, we keep
			the total counter size to 32 bits. This gives
			us plenty of scope for large internal count values
			while ensuring that the whole structure fits
			inside a machine word on 32-bit and 64-bit machines.
			It’s important to update these counts together as a
			single entity in order to avoid race conditions
			*/
			struct node_counter
			{
				unsigned internal_count : 30;
				unsigned external_counters : 2;
			};

			/// Node type
			struct node_type
			{
				atomics::atomic< value_type* > m_value;        ///< Value stored in the node
				atomics::atomic< node_counter > m_count;       ///< Internal and external reference counter
				atomics::atomic< counted_node_ptr > m_next;    ///< Pointer to the next node in the queue

				node_type() : m_value(nullptr)
				{
					node_counter new_count;
					new_count.internal_count = 0;
					new_count.external_counters = 2;
					m_count.store(new_count);

					counted_node_ptr new_next = { 0 };
					m_next.store(new_next);
				}
			};

			atomics::atomic< counted_node_ptr > m_Head;
			atomics::atomic< counted_node_ptr > m_Tail;

			item_counter m_ItemCounter;
			//@endcond

		public:
			typedef typename traits::allocator::template rebind< node_type >::other allocator_type; ///< Allocator type used for allocate/deallocate the queue nodes

		protected:
			//@cond
			typedef cds::details::Allocator< node_type, allocator_type > node_allocator;
			typedef std::unique_ptr< value_type > scoped_value_ptr;

			static node_type * alloc_node()
			{
				return node_allocator().New();
			}
			static node_type * alloc_node(value_type * const val)
			{
				node_type * new_node = alloc_node();
				new_node->m_value.store(val, atomics::memory_order_relaxed);
				return new_node;
			}
			static node_type * alloc_node(value_type const& val)
			{
				scoped_value_ptr new_value(new value_type(val));
				node_type * new_node = alloc_node(new_value.get());
				new_value.release();
				return new_node;
			}
			template <typename... Args>
			static node_type * alloc_node_move(Args&&... args)
			{
				scoped_value_ptr new_value(new value_type(std::forward<Args>(args)...));
				node_type * new_node = alloc_node(new_value.get());
				new_value.release();
				return new_node;
			}
			static void free_node(node_type * p)
			{
				node_allocator().Delete(p);
			}
			static void release_ref(node_type * p)
			{
				node_counter old_counter = p->m_count.load(atomics::memory_order_relaxed);
				node_counter new_counter;

				do
				{
					new_counter = old_counter;
					--new_counter.internal_count;
				} while (!p->m_count.compare_exchange_strong(
					old_counter, new_counter,
					atomics::memory_order_acquire, atomics::memory_order_relaxed));

				if (!new_counter.internal_count && !new_counter.external_counters) {
					p->m_value.exchange(nullptr);
					free_node(p);
				}
			}

			static void increase_external_count(atomics::atomic<counted_node_ptr>& counter, counted_node_ptr& old_counter)
			{
				counted_node_ptr new_counter;

				do
				{
					new_counter = old_counter;
					++new_counter.external_count;
				} while (!counter.compare_exchange_strong(
					old_counter, new_counter,
					atomics::memory_order_acquire, atomics::memory_order_relaxed));
				old_counter.external_count = new_counter.external_count;
			}
			static void free_external_counter(counted_node_ptr& old_node_ptr)
			{
				node_type * const ptr = old_node_ptr.ptr;
				int const count_increase = old_node_ptr.external_count - 2;

				node_counter old_counter = ptr->m_count.load(atomics::memory_order_relaxed);
				node_counter new_counter;

				do
				{
					new_counter = old_counter;
					--new_counter.external_counters;
					new_counter.internal_count += count_increase;
				} while (!ptr->m_count.compare_exchange_strong(
					old_counter, new_counter,
					atomics::memory_order_acquire, atomics::memory_order_relaxed));

				if (!new_counter.internal_count && !new_counter.external_counters) {
					free_node(ptr);
				}
			}
			void set_new_tail(counted_node_ptr& old_tail, counted_node_ptr const& new_tail)
			{
				node_type * const current_tail_ptr = old_tail.ptr;
				while (!m_Tail.compare_exchange_weak(old_tail, new_tail)
					&& old_tail.ptr == current_tail_ptr);
				if (old_tail.ptr == current_tail_ptr)
					free_external_counter(old_tail);
				else
					release_ref(current_tail_ptr);
			}
			struct node_disposer
			{
				void operator()(node_type * p)
				{
					free_node(p);
				}
			};
			//@endcond

		public:
			/// Initializes empty queue
			WilliamsQueue()
			{
				counted_node_ptr dummy_node;
				dummy_node.ptr = alloc_node();
				dummy_node.external_count = 1;

				m_Head.store(dummy_node);
				m_Tail.store(dummy_node);
			}

			/// Destructor clears the queue
			~WilliamsQueue()
			{
				clear();
				assert(m_Head.load().ptr == m_Tail.load().ptr);
				free_node(m_Head.load().ptr);
			}

			//@cond
			WilliamsQueue(const WilliamsQueue& other) = delete;
			WilliamsQueue& operator=(const WilliamsQueue& other) = delete;
			//@endcond

			/// Enqueues \p val value into the queue. Always returns \a true
			bool enqueue(value_type const& val)
			{
				scoped_value_ptr new_value_ptr(new value_type(val));
				return enqueue_value_ptr(new_value_ptr);
			}

			/// Enqueues \p data, move semantics
			bool enqueue(value_type&& data)
			{
				scoped_value_ptr new_value_ptr(new value_type(std::forward<value_type>(data)));
				return enqueue_value_ptr(new_value_ptr);
			}

			bool enqueue_value_ptr(scoped_value_ptr& value_ptr)
			{
				counted_node_ptr new_next;
				new_next.ptr = alloc_node();
				new_next.external_count = 1;

				counted_node_ptr old_tail = m_Tail.load();

				while (true)
				{
					increase_external_count(m_Tail, old_tail);
					value_type * old_value = nullptr;
					if (old_tail.ptr->m_value.compare_exchange_strong(old_value, value_ptr.get()))
					{

						counted_node_ptr old_next = { 0 };
						if (!old_tail.ptr->m_next.compare_exchange_strong(old_next, new_next))
						{
							free_node(new_next.ptr);
							new_next = old_next;
						}
						set_new_tail(old_tail, new_next);
						value_ptr.release();
						break;
					}
					else
					{
						counted_node_ptr old_next = { 0 };
						if (old_tail.ptr->m_next.compare_exchange_strong(old_next, new_next))
						{
							old_next = new_next;
							new_next.ptr = alloc_node();
						}
						set_new_tail(old_tail, old_next);
					}
				}

				++m_ItemCounter;
				return true;
			}

			/// Enqueues data to the queue using a functor
			/**
			\p Func is a functor called to create node.
			The functor \p f takes one argument - a reference to a new node of type \ref value_type :
			\code
			cds::container::WilliamsQueue< Foo > myQueue;
			Bar bar;
			myQueue.enqueue_with( [&bar]( Foo& dest ) { dest = bar; } );
			\endcode
			*/
			template <typename Func>
			bool enqueue_with(Func f)
			{
				value_type val;
				f(val);
				return enqueue(val);
			}

			/// Enqueues data of type \ref value_type constructed from <tt>std::forward<Args>(args)...</tt>
			template <typename... Args>
			bool emplace(Args&&... args)
			{
				value_type val(std::forward<Args>(args)...);
				return enqueue(val);
			}

			/// Synonym for \p enqueue() function
			bool push(value_type const& val)
			{
				return enqueue(val);
			}

			/// Synonym for \p enqueue( value_type&& ) function
			bool push(value_type&& val)
			{
				return enqueue(std::move(val));
			}

			/// Synonym for \p enqueue_with() function
			template <typename Func>
			bool push_with(Func f)
			{
				return enqueue_with(f);
			}

			/// Dequeues a value from the queue
			/**
			If queue is not empty, the function returns \p true, \p dest contains copy of
			dequeued value. The assignment operator for type \ref value_type is invoked.
			If queue is empty, the function returns \p false, \p dest is unchanged.
			*/
			bool dequeue(value_type& dest)
			{
				return dequeue_with([&dest](value_type& src) { dest = src; });
			}

			/// Dequeues a value using a functor
			/**
			\p Func is a functor called to copy dequeued value.
			The functor takes one argument - a reference to removed node:
			\code
			cds:container::WilliamsQueue< Foo > myQueue;
			Bar bar;
			myQueue.dequeue_with( [&bar]( Foo& src ) { bar = std::move( src );});
			\endcode
			The functor is called only if the queue is not empty.
			*/
			template <typename Func>
			bool dequeue_with(Func f)
			{
				counted_node_ptr old_head = m_Head.load(atomics::memory_order_relaxed);

				while (true)
				{
					increase_external_count(m_Head, old_head);
					node_type * const p = old_head.ptr;
					if (p == m_Tail.load().ptr)
					{
						release_ref(p);
						return false;
					}
					counted_node_ptr next = p->m_next.load();
					if (m_Head.compare_exchange_strong(old_head, next))
					{
						scoped_value_ptr res(p->m_value.load());
						free_external_counter(old_head); 
						--m_ItemCounter;
						f(*res.get());

						return true;
					}
					release_ref(p);
				}
			}

			/// Synonym for \p dequeue() function
			bool pop(value_type& dest)
			{
				return dequeue(dest);
			}

			/// Synonym for \p dequeue_with() function
			template <typename Func>
			bool pop_with(Func f)
			{
				return dequeue_with(f);
			}

			/// Clear the queue
			/**
			The function repeatedly calls \ref dequeue until it returns \a false.
			*/
			void clear()
			{
				value_type v;
				while (dequeue(v));
			}

			/// Checks whether the queue is empty
			bool empty() const
			{
				return m_Head.load().ptr == m_Tail.load().ptr;
			}

			/// Returns queue's item count
			/**
			The value returned depends on \p williams_queue::traits::item_counter.
			For \p atomicity::empty_item_counter, this function always returns 0.

			@note Even if you use real item counter and it returns 0, this fact is not
			mean that the queue is empty. To check queue emptyness use \p empty() instead.
			*/
			size_t size() const
			{
				return m_ItemCounter.value();
			}

			//@cond
			/// The class has no internal statistics. For test consistency only
			std::nullptr_t statistics() const
			{
				return nullptr;
			}
			//@endcond
		};

	}
}  // namespace cds::container

#endif  // #ifndef CDSLIB_CONTAINER_WILLIAMS_QUEUE_H
