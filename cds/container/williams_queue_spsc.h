//$$CDS-header$$

#ifndef __CDS_CONTAINER_WILLIAMS_QUEUE_H
#define __CDS_CONTAINER_WILLIAMS_QUEUE_H

#include <memory>
#include <cds/algo/atomic.h>
#include <cds/details/allocator.h>

namespace cds { namespace container {

    /// WilliamsQueue related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace williams_queue {

        /// Queue internal statistics. May be used for debugging or profiling
        /**
            Template argument \p Counter defines type of counter.
            Default is \p cds::atomicity::event_counter, that is weak, i.e. it is not guaranteed
            strict event counting.
            You may use stronger type of counter like as \p cds::atomicity::item_counter,
            or even integral type, for example, \p int.
        */
        template <typename Counter = cds::atomicity::event_counter >
        struct stat
        {
            typedef Counter     counter_type;   ///< Counter type

            counter_type m_EnqueueCount      ;  ///< Enqueue call count
            counter_type m_DequeueCount      ;  ///< Dequeue call count
            counter_type m_EmptyDequeue      ;  ///< Count of dequeue from empty queue

            /// Register enqueue call
            void onEnqueue()                { ++m_EnqueueCount; }
            /// Register dequeue call
            void onDequeue()                { ++m_DequeueCount; }
            /// Register dequeuing from empty queue
            void onEmptyDequeue()           { ++m_EmptyDequeue; }

            //@cond
            void reset()
            {
                m_EnqueueCount.reset();
                m_DequeueCount.reset();
                m_EmptyDequeue.reset();
            }

            stat& operator +=( stat const& s )
            {
                m_EnqueueCount += s.m_EnqueueCount.get();
                m_DequeueCount += s.m_DequeueCount.get();
                m_EmptyDequeue += s.m_EmptyDequeue.get();

                return *this;
            }
            //@endcond
        };

        /// Dummy queue statistics - no counting is performed, no overhead. Support interface like \p msqueue::stat
        struct empty_stat
        {
            //@cond
            void onEnqueue()                {}
            void onDequeue()                {}
            void onEmptyDequeue()           {}

            void reset() {}
            empty_stat& operator +=( empty_stat const& )
            {
                return *this;
            }
            //@endcond
        };

        /// WilliamsQueue default traits
        struct traits
        {
            /// Item allocator. Default is \ref CDS_DEFAULT_ALLOCATOR
            typedef CDS_DEFAULT_ALLOCATOR allocator;

            /// Item counting feature; by default, disabled. Use \p cds::atomicity::item_counter to enable item counting
            typedef atomicity::empty_item_counter   item_counter;

            /// Internal statistics (by default, disabled)
            /**
                Possible option value are: \p msqueue::stat, \p msqueue::empty_stat (the default),
                user-provided class that supports \p %msqueue::stat interface.
            */
            typedef williams_queue::empty_stat         stat;
        };

    } // namespace williams_queue

    /// Williams' single-producer, single-consumer lock-free queue
    template<typename T, typename Traits = williams_queue::traits>
    class WilliamsQueue;

    template<typename T, typename Traits>
    class WilliamsQueue<std::shared_ptr<T>, Traits>
    {
    public:
        typedef std::shared_ptr<T> value_type;
        typedef Traits traits;                ///< Queue traits

        typedef typename traits::allocator    allocator_type;
        typedef typename traits::item_counter item_counter;   ///< Item counter class
        typedef typename traits::stat         stat;           ///< Internal statistics

    private:
        struct node {
            value_type data;
            node* next;
    
            node() : next(nullptr) {
            }
        };

        typedef cds::details::Allocator<node, allocator_type> allocator;

        item_counter  m_ItemCounter;  ///< Item counter
        stat          m_Stat;         ///< Internal statistics

        atomics::atomic<node*> head;
        atomics::atomic<node*> tail;
    
        node* pop_head() {
            node * const old_head = head.load();
            if (old_head == tail.load()) {
                m_Stat.onEmptyDequeue();
                return nullptr;
            }
            head.store(old_head->next);
            return old_head;
        }
    
    public:
        /// Initializes empty queue
        WilliamsQueue() : head(allocator().New()), tail(head.load()) {
        }
    
        WilliamsQueue(const WilliamsQueue& other) = delete;
        WilliamsQueue& operator=(const WilliamsQueue& other) = delete;

        /// Destructor clears the queue
        ~WilliamsQueue() {
            while (node * const old_head = head.load()) {
                head.store(old_head->next);
                allocator().Delete(old_head);
            }
        }

        /// Enqueues \p val value into the queue.
        void enqueue(value_type val) {
            node* p = allocator().New();
            node * const old_tail = tail.load();
            old_tail->data.swap(val);
            old_tail->next = p;
            tail.store(p);
            ++m_ItemCounter;
            m_Stat.onEnqueue();
        }

        /// Dequeues a value from the queue
        value_type dequeue() {
            node* old_head = pop_head();
            if (!old_head) {
                return value_type();
            }
            value_type const res(old_head->data);
            allocator().Delete(old_head);
            --m_ItemCounter;
            m_Stat.onDequeue();
            return res;
        }

        /// Checks if the queue is empty
        bool empty() const {
            if (head.load() == tail.load()) {
                return true;
            }
            return false;
        }

        /// Returns queue's item count
        /**
            The value returned depends on \p msqueue::traits::item_counter. For \p atomicity::empty_item_counter,
            this function always returns 0.

            @note Even if you use real item counter and it returns 0, this fact is not mean that the queue
            is empty. To check queue emptyness use \p empty() method.
        */
        size_t size() const {
            return m_ItemCounter.value();
        }

        /// Returns reference to internal statistics
        stat const& statistics() const {
            return m_Stat;
        }
    };

    template<typename T, typename Traits>
    class WilliamsQueue: private WilliamsQueue<std::shared_ptr<T>, Traits>
    {
    public:
        typedef T value_type;
        typedef Traits traits;                ///< Queue traits
        typedef typename traits::stat  stat;  ///< Internal statistics

    private:
        typedef WilliamsQueue<std::shared_ptr<T>, Traits> parent;
        typedef std::shared_ptr<T> parent_value_type;

    public:
        /// Enqueues \p val value into the queue.
        void enqueue(value_type val) {
            parent::enqueue(std::make_shared<value_type>(val));
        }

        /// Dequeues a value from the queue
        value_type * dequeue() {
            return parent::dequeue().get();
        }

        /// Checks if the queue is empty
        bool empty() const {
            parent::empty();
        }

        /// Returns queue's item count
        /**
            The value returned depends on \p msqueue::traits::item_counter. For \p atomicity::empty_item_counter,
            this function always returns 0.

            @note Even if you use real item counter and it returns 0, this fact is not mean that the queue
            is empty. To check queue emptyness use \p empty() method.
        */
        size_t size() const {
            return parent::size();
        }


        /// Returns reference to internal statistics
        stat const& statistics() const {
            return parent::statistics();
        }
    };

}} // namespace cds::container

#endif // #ifndef __CDS_CONTAINER_WILLIAMS_QUEUE_H
