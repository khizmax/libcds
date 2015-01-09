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

        /// WilliamsQueue default traits
        struct traits
        {
            typedef CDS_DEFAULT_ALLOCATOR allocator;
        };

    } // namespace williams_queue

    /// Williams' single-producer, single-consumer lock-free queue
    template<typename T, typename Traits = williams_queue::traits>
    class WilliamsQueue
    {
    public:
        typedef std::shared_ptr<T> value_type;

    private:
        struct node {
            value_type data;
            node* next;
    
            node() : next(nullptr) {
            }
        };

        typedef Traits traits;
        typedef typename traits::allocator allocator_type;
        typedef cds::details::Allocator<node, allocator_type> allocator;

        atomics::atomic<node*> head;
        atomics::atomic<node*> tail;
    
        node* pop_head() {
            node * const old_head = head.load();
            if (old_head == tail.load()) {
                return nullptr;
            }
            head.store(old_head->next);
            return old_head;
        }
    
    public:
        WilliamsQueue() : head(allocator().New()), tail(head.load()) {
        }
    
        WilliamsQueue(const WilliamsQueue& other) = delete;
        WilliamsQueue& operator=(const WilliamsQueue& other) = delete;

        ~WilliamsQueue() {
            while (node * const old_head = head.load()) {
                head.store(old_head->next);
                allocator().Delete(old_head);
            }
        }

        /// Checks if the queue is empty
        bool empty() {
            if (head.load() == tail.load()) {
                return true;
            }
            return false;
        }

        value_type pop() {
            node* old_head = pop_head();
            if (!old_head) {
                return value_type();
            }
            value_type const res(old_head->data);
            allocator().Delete(old_head);
            return res;
        }

        void push(T new_value) {
            value_type new_data(std::make_shared<T>(new_value));
            node* p = allocator().New();
            node * const old_tail = tail.load();
            old_tail->data.swap(new_data);
            old_tail->next = p;
            tail.store(p);
        }
    };

}} // namespace cds::container

#endif // #ifndef __CDS_CONTAINER_WILLIAMS_QUEUE_H
