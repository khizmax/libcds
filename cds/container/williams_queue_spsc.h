#ifndef CDSLIB_CONTAINER_WilliamsQueueSPSC_H
#define CDSLIB_CONTAINER_WilliamsQueueSPSC_H

#include <cds/algo/flat_combining.h>
#include <cds/algo/elimination_opt.h>

namespace cds
{
    namespace container
    {
        namespace williams_queue_spsc
        {
            struct traits
			{
				/// Node allocator
				typedef CDS_DEFAULT_ALLOCATOR allocator;

				/// Item counting feature; by default, disabled. Use \p cds::atomicity::item_counter to enable item counting
				typedef atomicity::empty_item_counter item_counter;
			};

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
        }
        
        template <typename T, typename traits = williams_queue_spsc::traits>
        class WilliamsQueueSPSC
        {
            public:
                typedef T value_type;
                typedef typename traits::item_counter item_counter;
            private:
                struct node
                {
                    std::shared_ptr<T> data;
                    node *next;

                    node() : next(nullptr) {}
                };

                item_counter itemCounter;

                std::atomic<node *> head;
                std::atomic<node *> tail;



                bool enqueue(std::shared_ptr<T> value)
                {
                    node *p = new node;
                    node *const old_tail = tail.load();
                    old_tail->data.swap(value);
                    old_tail->next = p;
                    tail.store(p);
                    itemCounter++;
                    return true;
                }

                node *dequeue_head()
                {
                    node *const old_head = head.load();
                    if (old_head == tail.load())
                    {
                        return nullptr;
                    }
                    head.store(old_head->next);
                    itemCounter--;
                    return old_head;
                }
            public:

                WilliamsQueueSPSC() : head(new node), tail(head.load()) {}

                WilliamsQueueSPSC(const WilliamsQueueSPSC &other) = delete;
                WilliamsQueueSPSC &operator=(const WilliamsQueueSPSC &other) = delete;

                ~WilliamsQueueSPSC()
                {
                    while (node *const old_head = head.load())
                    {
                        head.store(old_head->next);
                        delete old_head;
                    }
                }




                bool enqueue(value_type const& value)
                {
                    std::shared_ptr<T> new_data(std::make_shared<T>(value));
                    return enqueue(new_data);
                }


                bool enqueue(value_type&& value)
                {
                    std::shared_ptr<T> new_data(std::make_shared<T>(value));
                    return enqueue(new_data);
                }


                bool push(value_type const& value)
			    {
                    return enqueue(value);
                }

                bool push(value_type&& value)
                {
                    return enqueue(value);                  
                }
                
                template <typename Func>
			    bool enqueue_with(Func f)
			    {
                    value_type val;
				    f(val);
                    return enqueue(val);
                }

                template <typename Func>
			    bool push_with(Func f)
			    {
			    	return enqueue_with(f);
			    }



                template <typename Func>
			    bool dequeue_with(Func f)
			    {
                    node *old_head = dequeue_head();

                    if (!old_head)
                    {
                        return false;
                    }

                    f(*old_head->data);
                    delete old_head;
                    
                    return true;
                }

                template <typename Func>
                bool pop_with(Func f)
                {
                    return dequeue_with(f);
                }



                bool dequeue(value_type& destination)
                {
                    return dequeue_with([&destination](value_type& src) { destination = src; });
                }

                bool pop(value_type& destination)
                {
                     return dequeue(destination);                   
                }
                
                void clear()
                {
                    value_type v;
                    while (dequeue(v));
                }

                bool empty() const
                {
                    return head.load() == tail.load();
                }

                size_t size() const
                {
                    return itemCounter.value();
                }

                template <typename... Args>
                bool emplace(Args&&... args)
                {
                    value_type val(std::forward<Args>(args)...);
                    return enqueue(val);
                }

                std::nullptr_t statistics() const
                {
                    return nullptr;
                }
        };
    };
}

#endif // #ifndef CDSLIB_CONTAINER_WilliamsQueueSPSC_H
