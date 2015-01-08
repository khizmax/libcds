//$$CDS-header$$

#ifndef __CDS_INTRUSIVE_WILLIAMS_QUEUE_H
#define __CDS_INTRUSIVE_WILLIAMS_QUEUE_H

#include <memory>
#include <cds/algo/atomic.h>

namespace cds { namespace intrusive {

    /// WilliamsQueue related definitions
    /** @ingroup cds_intrusive_helper
    */
    namespace williams_queue {

        /// WilliamsQueue default traits
        struct traits
        {

        };

    } // namespace williams_queue

    /// Williams' single-producer, single-consumer lock-free queue
    template<typename T, typename Traits = williams_queue::traits>
    class WilliamsQueue
    {
    public:
        typedef std::shared_ptr<T> node_data;

    private:
        struct node {
            node_data data;
            node* next;
    
            node() : next(nullptr) {
            }
        };

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
        WilliamsQueue() : head(new node), tail(head.load()) {
        }
    
        WilliamsQueue(const WilliamsQueue& other) = delete;
        WilliamsQueue& operator=(const WilliamsQueue& other) = delete;

        ~WilliamsQueue() {
            while (node * const old_head = head.load()) {
                head.store(old_head->next);
                delete old_head;
            }
        }

        node_data pop() {
            node* old_head = pop_head();
            if (!old_head) {
                return node_data();
            }
            node_data const res(old_head->data);
            delete old_head;
            return res;
        }

        void push(T new_value) {
            node_data new_data(std::make_shared<T>(new_value));
            node* p = new node;
            node * const old_tail = tail.load();
            old_tail->data.swap(new_data);
            old_tail->next = p;
            tail.store(p);
        }
    };

}} // namespace cds::intrusive

#endif // #ifndef __CDS_INTRUSIVE_WILLIAMS_QUEUE_H
