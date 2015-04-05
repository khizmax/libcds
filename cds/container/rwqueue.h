//$$CDS-header$$

#ifndef CDSLIB_CONTAINER_RWQUEUE_H
#define CDSLIB_CONTAINER_RWQUEUE_H

#include <mutex>        // unique_lock
#include <cds/container/msqueue.h>
#include <cds/sync/spinlock.h>

namespace cds { namespace container {
    /// RWQueue related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace rwqueue {
        /// RWQueue default type traits
        struct traits
        {
            /// Lock policy
            typedef cds::sync::spin  lock_type;

            /// Node allocator
            typedef CDS_DEFAULT_ALLOCATOR   allocator;

            /// Item counting feature; by default, disabled. Use \p cds::atomicity::item_counter to enable item counting
            typedef cds::atomicity::empty_item_counter item_counter;

            /// Alignment of internal queue data. Default is \p opt::cache_line_alignment
            enum { alignment = opt::cache_line_alignment };
        };

        /// Metafunction converting option list to \p rwqueue::traits
        /**
            Supported \p Options are:
            - opt::lock_type - lock policy, default is \p cds::sync::spin. Any type satisfied \p Mutex C++ concept may be used.
            - opt::allocator - allocator (like \p std::allocator) used for allocating queue nodes. Default is \ref CDS_DEFAULT_ALLOCATOR
            - opt::item_counter - the type of item counting feature. Default is \p cds::atomicity::empty_item_counter (item counting disabled)
                To enable item counting use \p cds::atomicity::item_counter.
            - opt::alignment - the alignment for internal queue data. Default is \p opt::cache_line_alignment

            Example: declare mutex-based \p %RWQueue with item counting
            \code
            typedef cds::container::RWQueue< Foo,
                typename cds::container::rwqueue::make_traits<
                    cds::opt::item_counter< cds::atomicity::item_counter >,
                    cds::opt::lock_type< std::mutex >
                >::type
            > myQueue;
            \endcode
        */
        template <typename... Options>
        struct make_traits {
#   ifdef CDS_DOXYGEN_INVOKED
            typedef implementation_defined type;   ///< Metafunction result
#   else
            typedef typename cds::opt::make_options<
                typename cds::opt::find_type_traits< traits, Options... >::type
                , Options...
            >::type type;
#   endif
        };

    } // namespace rwqueue

    /// Michael & Scott blocking queue with fine-grained synchronization schema
    /** @ingroup cds_nonintrusive_queue
        The queue has two different locks: one for reading and one for writing.
        Therefore, one writer and one reader can simultaneously access to the queue.
        The queue does not require any garbage collector.

        <b>Source</b>
            - [1998] Maged Michael, Michael Scott "Simple, fast, and practical non-blocking
                and blocking concurrent queue algorithms"

        <b>Template arguments</b>
        - \p T - value type to be stored in the queue
        - \p Traits - queue traits, default is \p rwqueue::traits. You can use \p rwqueue::make_traits
            metafunction to make your traits or just derive your traits from \p %rwqueue::traits:
            \code
            struct myTraits: public cds::container::rwqueue::traits {
                typedef cds::atomicity::item_counter    item_counter;
            };
            typedef cds::container::RWQueue< Foo, myTraits > myQueue;

            // Equivalent make_traits example:
            typedef cds::container::RWQueue< Foo,
                typename cds::container::rwqueue::make_traits<
                    cds::opt::item_counter< cds::atomicity::item_counter >
                >::type
            > myQueue;
            \endcode
    */
    template <typename T, typename Traits = rwqueue::traits >
    class RWQueue
    {
    public:
        /// Rebind template arguments
        template <typename T2, typename Traits2>
        struct rebind {
            typedef RWQueue< T2, Traits2 > other   ;   ///< Rebinding result
        };

    public:
        typedef T       value_type; ///< Type of value to be stored in the queue
        typedef Traits  traits;     ///< Queue traits

        typedef typename traits::lock_type  lock_type;      ///< Locking primitive
        typedef typename traits::item_counter item_counter; ///< Item counting policy used

    protected:
        //@cond
        /// Node type
        struct node_type
        {
            node_type * volatile    m_pNext ;   ///< Pointer to the next node in the queue
            value_type              m_value ;   ///< Value stored in the node

            node_type( value_type const& v )
                : m_pNext( nullptr )
                , m_value(v)
            {}

            node_type()
                : m_pNext( nullptr )
            {}

            template <typename... Args>
            node_type( Args&&... args )
                : m_pNext( nullptr )
                , m_value( std::forward<Args>(args)...)
            {}
        };
        //@endcond

    public:
        typedef typename traits::allocator::template rebind<node_type>::other allocator_type; ///< Allocator type used for allocate/deallocate the queue nodes

    protected:
        //@cond
        typedef typename opt::details::alignment_setter< lock_type, traits::alignment >::type aligned_lock_type;
        typedef std::unique_lock<lock_type> scoped_lock;
        typedef cds::details::Allocator< node_type, allocator_type >  node_allocator;

        item_counter    m_ItemCounter;

        mutable aligned_lock_type   m_HeadLock;
        node_type * m_pHead;
        mutable aligned_lock_type   m_TailLock;
        node_type * m_pTail;
        //@endcond

    protected:
        //@cond
        static node_type * alloc_node()
        {
            return node_allocator().New();
        }

        static node_type * alloc_node( T const& data )
        {
            return node_allocator().New( data );
        }

        template <typename... Args>
        static node_type * alloc_node_move( Args&&... args )
        {
            return node_allocator().MoveNew( std::forward<Args>( args )... );
        }

        static void free_node( node_type * pNode )
        {
            node_allocator().Delete( pNode );
        }

        bool enqueue_node( node_type * p )
        {
            assert( p != nullptr );
            {
                scoped_lock lock( m_TailLock );
                m_pTail =
                    m_pTail->m_pNext = p;
            }
            ++m_ItemCounter;
            return true;
        }

        struct node_disposer {
            void operator()( node_type * pNode )
            {
                free_node( pNode );
            }
        };
        typedef std::unique_ptr< node_type, node_disposer >     scoped_node_ptr;
        //@endcond

    public:
        /// Makes empty queue
        RWQueue()
        {
            node_type * pNode = alloc_node();
            m_pHead =
                m_pTail = pNode;
        }

        /// Destructor clears queue
        ~RWQueue()
        {
            clear();
            assert( m_pHead == m_pTail );
            free_node( m_pHead );
        }

        /// Enqueues \p data. Always return \a true
        bool enqueue( value_type const& data )
        {
            scoped_node_ptr p( alloc_node( data ));
            if ( enqueue_node( p.get() )) {
                p.release();
                return true;
            }
            return false;
        }

        /// Enqueues \p data to the queue using a functor
        /**
            \p Func is a functor called to create node.
            The functor \p f takes one argument - a reference to a new node of type \ref value_type :
            \code
            cds::container::RWQueue< cds::gc::HP, Foo > myQueue;
            Bar bar;
            myQueue.enqueue_with( [&bar]( Foo& dest ) { dest = bar; } );
            \endcode
        */
        template <typename Func>
        bool enqueue_with( Func f )
        {
            scoped_node_ptr p( alloc_node() );
            f( p->m_value );
            if ( enqueue_node( p.get() )) {
                p.release();
                return true;
            }
            return false;
        }

        /// Enqueues data of type \ref value_type constructed with <tt>std::forward<Args>(args)...</tt>
        template <typename... Args>
        bool emplace( Args&&... args )
        {
            scoped_node_ptr p( alloc_node_move( std::forward<Args>(args)... ));
            if ( enqueue_node( p.get() )) {
                p.release();
                return true;
            }
            return false;
        }

        /// Synonym for \p enqueue() function
        bool push( value_type const& val )
        {
            return enqueue( val );
        }

        /// Synonym for \p enqueue_with() function
        template <typename Func>
        bool push_with( Func f )
        {
            return enqueue_with( f );
        }

        /// Dequeues a value to \p dest.
        /**
            If queue is empty returns \a false, \p dest can be corrupted.
            If queue is not empty returns \a true, \p dest contains the value dequeued
        */
        bool dequeue( value_type& dest )
        {
            return dequeue_with( [&dest]( value_type& src ) { dest = src; } );
        }

        /// Dequeues a value using a functor
        /**
            \p Func is a functor called to copy dequeued value.
            The functor takes one argument - a reference to removed node:
            \code
            cds:container::RWQueue< cds::gc::HP, Foo > myQueue;
            Bar bar;
            myQueue.dequeue_with( [&bar]( Foo& src ) { bar = std::move( src );});
            \endcode
            The functor is called only if the queue is not empty.
        */
        template <typename Func>
        bool dequeue_with( Func f )
        {
            node_type * pNode;
            {
                scoped_lock lock( m_HeadLock );
                pNode = m_pHead;
                node_type * pNewHead = pNode->m_pNext;
                if ( pNewHead == nullptr )
                    return false;
                f( pNewHead->m_value );
                m_pHead = pNewHead;
            }    // unlock here
            --m_ItemCounter;
            free_node( pNode );
            return true;
        }

        /// Synonym for \p dequeue() function
        bool pop( value_type& dest )
        {
            return dequeue( dest );
        }

        /// Synonym for \p dequeue_with() function
        template <typename Func>
        bool pop_with( Func f )
        {
            return dequeue_with( f );
        }

        /// Checks if queue is empty
        bool empty() const
        {
            scoped_lock lock( m_HeadLock );
            return m_pHead->m_pNext == nullptr;
        }

        /// Clears queue
        void clear()
        {
            scoped_lock lockR( m_HeadLock );
            scoped_lock lockW( m_TailLock );
            while ( m_pHead->m_pNext != nullptr ) {
                node_type * pHead = m_pHead;
                m_pHead = m_pHead->m_pNext;
                free_node( pHead );
            }
        }

        /// Returns queue's item count
        /**
            The value returned depends on \p rwqueue::traits::item_counter. For \p atomicity::empty_item_counter,
            this function always returns 0.

            @note Even if you use real item counter and it returns 0, this fact is not mean that the queue
            is empty. To check queue emptyness use \p empty() method.
        */
        size_t    size() const
        {
            return m_ItemCounter.value();
        }

        //@cond
        /// Returns reference to internal statistics
        cds::container::msqueue::empty_stat statistics() const
        {
            return cds::container::msqueue::empty_stat();
        }
        //@endcond
    };

}}  // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_RWQUEUE_H
