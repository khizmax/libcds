#ifndef CDSLIB_CONTAINER_WILLIAMS_STACK_H
#define CDSLIB_CONTAINER_WILLIAMS_STACK_H

#include <memory>
#include <cds/container/details/base.h>

namespace cds
{
    namespace container
    {
        namespace williams_stack
        {
            /// Williams_Stack dummy statistics, no overhead
            struct empty_stat: public cds::algo::flat_combining::empty_stat
            {
                //@cond
                void    onPush()        {}
                void    onPushMove()    {}
                void    onPop(bool)     {}
                void    onCollide()     {}
                //@endcond
            };

            struct traits
            {
                typedef CDS_DEFAULT_ALLOCATOR allocator;
                typedef atomicity::empty_item_counter item_counter;
                typedef empty_stat stat;
            };

            template <typename... Options>
            struct make_traits
            {
#ifdef CDS_DOXYGEN_INVOKED
                typedef implementation_defined type;   ///< Metafunction result
#else
                typedef typename cds::opt::make_options<
                typename cds::opt::find_type_traits< traits, Options... >::type
                , Options...
                >::type type;
#endif
            };
        } // namespace williams_stack

        template <typename T, typename Traits = williams_stack::traits>
        class WilliamsStack
        {
        public:

            template <typename T2, typename Traits2>
            struct rebind
            {
                typedef WilliamsStack< T2, Traits2 > other;
            };

            typedef T value_type;
            typedef Traits traits;

            typedef typename traits::item_counter item_counter;
            typedef typename traits::stat stat;

        protected:
            struct node_type;

            struct counted_node_ptr
            {
                int external_count;
                node_type * ptr;
            };

            struct node_type
            {
                std::shared_ptr< value_type > data;
                atomics::atomic< int > internal_count;
                counted_node_ptr next;

                node_type( const value_type& data_ )
                    : data( std::make_shared<value_type>(data_) ),
                      internal_count(0)
                {}

                template <typename... Args>
                node_type( Args&&... args ) : node_type( value_type( std::forward<Args>( args )... ) )
                {}

                node_type() : node_type( (value_type*) nullptr )
                {}
            };

            atomics::atomic< counted_node_ptr > head;
            item_counter itemCounter;
            stat m_stat;

        public:
            typedef typename traits::allocator::template rebind< node_type >::other allocator_type;

        protected:
            typedef cds::details::Allocator< node_type, allocator_type > node_allocator;

            static node_type * alloc_node()
            {
                return node_allocator().New();
            }

            static node_type * alloc_node( value_type const& val )
            {
                return node_allocator().New( val );
            }

            template <typename... Args>
            static node_type * alloc_node_move( Args&&... args )
            {
                return node_allocator().MoveNew( std::forward<Args>( args )... );
            }

            static void free_node( node_type * p )
            {
                if (p) node_allocator().Delete( p );
            }

            void increase_head_count(counted_node_ptr& old_counter )
            {
                counted_node_ptr new_counter;

                do
                    {
                        new_counter = old_counter;
                        ++new_counter.external_count;
                    } while (!head.compare_exchange_strong( old_counter, new_counter,
                                                            std::memory_order_acquire,
                                                            std::memory_order_relaxed));

                old_counter.external_count = new_counter.external_count;
            }

        public:
            WilliamsStack()
            {
                counted_node_ptr _head;
                _head.external_count = 0;
                _head.ptr = nullptr;
                head.store(_head, atomics::memory_order_relaxed);
            }

            ~WilliamsStack()
            {
                clear();
                free_node( head.load().ptr );
            }

            WilliamsStack( const WilliamsStack& other ) = delete;
            WilliamsStack& operator=( const WilliamsStack& other ) = delete;

            // push
            bool push( value_type const& val )
            {
                counted_node_ptr new_node;
                new_node.ptr = alloc_node(val);
                new_node.external_count = 1;

                new_node.ptr->next = head.load(std::memory_order_relaxed);

                while (!head.compare_exchange_weak(	new_node.ptr->next, new_node,
                                                        std::memory_order_release,
                                                        std::memory_order_relaxed))
                    {}

                ++itemCounter;
                return true;
            }

            template <typename... Args>
            bool emplace(Args&&... args)
            {
                value_type val(std::forward<Args>(args)...);
                return push(val);
            }

            bool pop(value_type& val)
            {
                return pop_with([&val](value_type& src) { val = src; });
            }

            // pop
            template <typename Func>
            bool pop_with(Func f)
            {
                counted_node_ptr old_head = head.load(atomics::memory_order_relaxed);
                while (true)
                    {
                        increase_head_count(old_head);
                        node_type * const ptr = old_head.ptr;
                        if (!ptr)
                            {
                                return false;
                            }

                        counted_node_ptr next = ptr->next;
                        if (head.compare_exchange_strong(old_head, next,
                                                         std::memory_order_relaxed))
                            {
                                std::shared_ptr<value_type> res;
                                res.swap(ptr->data);

                                int const count_increase = old_head.external_count - 2;

                                if (ptr->internal_count.fetch_add(count_increase, std::memory_order_release) == -count_increase)
                                    {
                                        delete ptr;
                                    }

                                --itemCounter;
                                f(*res);
                                return true;
                            }
                        else
                            {
                                if (ptr->internal_count.fetch_add(-1, std::memory_order_relaxed) == 1)
                                    {
                                        ptr->internal_count.load(std::memory_order_acquire);
                                        delete ptr;
                                    }
                            }
                    }
            }

            void clear()
            {
                value_type v;
                while( pop( v ) );
            }

            bool empty() const
            {
                return head.load().ptr == nullptr;
            }

            size_t size() const
            {
                return itemCounter.value();
            }

            stat const& statistics() const
            {
                return m_stat;
            }
        };

    }
}  // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_WILLIAMS_STACK_H