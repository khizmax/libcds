//$$CDS-header$$

#ifndef __CDS_CONTAINER_RWQUEUE_H
#define __CDS_CONTAINER_RWQUEUE_H

#include <memory>
#include <cds/opt/options.h>
#include <cds/lock/spinlock.h>
#include <cds/intrusive/queue_stat.h>
#include <cds/details/allocator.h>
#include <cds/details/trivial_assign.h>
#include <cds/ref.h>

namespace cds { namespace container {

    /// Michael & Scott blocking queue with fine-grained synchronization schema
    /** @ingroup cds_nonintrusive_queue
        The queue has two different locks: one for reading and one for writing.
        Therefore, one writer and one reader can simultaneously access to the queue.
        The queue does not require any garbage collector.

        <b>Source</b>
            - [1998] Maged Michael, Michael Scott "Simple, fast, and practical non-blocking
                and blocking concurrent queue algorithms"

        <b>Template arguments</b>
        - \p T - type to be stored in the queue
        - \p Options - options

        \p Options are:
        - opt::allocator - allocator (like \p std::allocator). Default is \ref CDS_DEFAULT_ALLOCATOR
        - opt::lock_type - type of lock primitive. Default is cds::lock::Spin.
        - opt::item_counter - the type of item counting feature. Default is \ref atomicity::empty_item_counter
        - opt::stat - the type to gather internal statistics.
            Possible option value are: queue_stat, queue_dummy_stat, user-provided class that supports queue_stat interface.
            Default is \ref intrusive::queue_dummy_stat.
            <tt>RWQueue</tt> uses only \p onEnqueue and \p onDequeue counter.
        - opt::alignment - the alignment for \p lock_type to prevent false sharing. Default is opt::cache_line_alignment

        This queue has no intrusive counterpart.
    */
    template <typename T, CDS_DECL_OPTIONS6>
    class RWQueue
    {
        //@cond
        struct default_options
        {
            typedef lock::Spin  lock_type;
            typedef CDS_DEFAULT_ALLOCATOR   allocator;
            typedef atomicity::empty_item_counter item_counter;
            typedef intrusive::queue_dummy_stat stat;
            enum { alignment = opt::cache_line_alignment };
        };
        //@endcond

    public:
        //@cond
        typedef typename opt::make_options<
            typename cds::opt::find_type_traits< default_options, CDS_OPTIONS6 >::type
            ,CDS_OPTIONS6
        >::type   options;
        //@endcond

    public:
        /// Rebind template arguments
        template <typename T2, CDS_DECL_OTHER_OPTIONS6>
        struct rebind {
            typedef RWQueue< T2, CDS_OTHER_OPTIONS6> other   ;   ///< Rebinding result
        };

    public:
        typedef T   value_type  ;   ///< type of value stored in the queue

        typedef typename options::lock_type lock_type   ;   ///< Locking primitive used

    protected:
        //@cond
        /// Node type
        struct node_type
        {
            node_type * volatile    m_pNext ;   ///< Pointer to the next node in queue
            value_type              m_value ;   ///< Value stored in the node

            node_type( value_type const& v )
                : m_pNext( nullptr )
                , m_value(v)
            {}

            node_type()
                : m_pNext( nullptr )
            {}

#       ifdef CDS_EMPLACE_SUPPORT
            template <typename... Args>
            node_type( Args&&... args )
                : m_pNext( nullptr )
                , m_value( std::forward<Args>(args)...)
            {}
#       endif
        };
        //@endcond

    public:
        typedef typename options::allocator::template rebind<node_type>::other allocator_type   ; ///< Allocator type used for allocate/deallocate the queue nodes
        typedef typename options::item_counter item_counter ;   ///< Item counting policy used
        typedef typename options::stat      stat        ;   ///< Internal statistics policy used

    protected:
        //@cond
        typedef typename opt::details::alignment_setter< lock_type, options::alignment >::type aligned_lock_type;
        typedef cds::lock::scoped_lock<lock_type>   auto_lock;
        typedef cds::details::Allocator< node_type, allocator_type >  node_allocator;

        item_counter    m_ItemCounter;
        stat            m_Stat;

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

#   ifdef CDS_EMPLACE_SUPPORT
        template <typename... Args>
        static node_type * alloc_node_move( Args&&... args )
        {
            return node_allocator().MoveNew( std::forward<Args>( args )... );
        }
#   endif

        static void free_node( node_type * pNode )
        {
            node_allocator().Delete( pNode );
        }

        bool enqueue_node( node_type * p )
        {
            assert( p != nullptr );
            {
                auto_lock lock( m_TailLock );
                m_pTail =
                    m_pTail->m_pNext = p;
            }
            ++m_ItemCounter;
            m_Stat.onEnqueue();
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

        /// Enqueues \p data to queue using copy functor
        /**
            \p Func is a functor called to copy value \p data of type \p Type
            which may be differ from type \p T stored in the queue.
            The functor's interface is:
            \code
            struct myFunctor {
                void operator()(T& dest, Type const& data)
                {
                    // // Code to copy \p data to \p dest
                    dest = data;
                }
            };
            \endcode
            You may use \p boost:ref construction to pass functor \p f by reference.

            <b>Requirements</b> The functor \p Func should not throw any exception.
        */
        template <typename Type, typename Func>
        bool enqueue( Type const& data, Func f  )
        {
            scoped_node_ptr p( alloc_node());
            unref(f)( p->m_value, data );
            if ( enqueue_node( p.get() )) {
                p.release();
                return true;
            }
            return false;
        }

#   ifdef CDS_EMPLACE_SUPPORT
        /// Enqueues data of type \ref value_type constructed with <tt>std::forward<Args>(args)...</tt>
        /**
            This function is available only for compiler that supports
            variadic template and move semantics
        */
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
#   endif

        /// Dequeues a value using copy functor
        /**
            \p Func is a functor called to copy dequeued value to \p dest of type \p Type
            which may be differ from type \p T stored in the queue.
            The functor's interface is:
            \code
            struct myFunctor {
                void operator()(Type& dest, T const& data)
                {
                    // // Copy \p data to \p dest
                    dest = data;
                }
            };
            \endcode
            You may use \p boost:ref construction to pass functor \p f by reference.

            <b>Requirements</b> The functor \p Func should not throw any exception.
        */
        template <typename Type, typename Func>
        bool dequeue( Type& dest, Func f )
        {
            node_type * pNode;
            {
                auto_lock lock( m_HeadLock );
                pNode = m_pHead;
                node_type * pNewHead = pNode->m_pNext;
                if ( pNewHead == nullptr )
                    return false;
                unref(f)( dest, pNewHead->m_value );
                m_pHead = pNewHead;
            }    // unlock here
            --m_ItemCounter;
            free_node( pNode );
            m_Stat.onDequeue();
            return true;
        }

        /** Dequeues a value to \p dest.

            If queue is empty returns \a false, \p dest may be corrupted.
            If queue is not empty returns \a true, \p dest contains the value dequeued
        */
        bool dequeue( value_type& dest )
        {
            typedef cds::details::trivial_assign<value_type, value_type> functor;
            return dequeue( dest, functor() );
        }

        /// Synonym for \ref enqueue
        bool push( value_type const& data )
        {
            return enqueue( data );
        }

        /// Synonym for template version of \ref enqueue function
        template <typename Type, typename Func>
        bool push( Type const& data, Func f  )
        {
            return enqueue( data, f );
        }

        /// Synonym for \ref dequeue
        bool pop( value_type& data )
        {
            return dequeue( data );
        }

        /// Synonym for template version of \ref dequeue function
        template <typename Type, typename Func>
        bool pop( Type& dest, Func f )
        {
            return dequeue( dest, f );
        }

        /// Checks if queue is empty
        bool empty() const
        {
            auto_lock lock( m_HeadLock );
            return m_pHead->m_pNext == nullptr;
        }

        /// Clears queue
        void clear()
        {
            auto_lock lockR( m_HeadLock );
            auto_lock lockW( m_TailLock );
            while ( m_pHead->m_pNext != nullptr ) {
                node_type * pHead = m_pHead;
                m_pHead = m_pHead->m_pNext;
                free_node( pHead );
            }
        }

        /// Returns queue's item count
        /**
            The value returned depends on opt::item_counter option. For atomicity::empty_item_counter,
            this function always returns 0.

            <b>Warning</b>: even if you use real item counter and it returns 0, this fact is not mean that the queue
            is empty. To check queue emptyness use \ref empty() method.
        */
        size_t    size() const
        {
            return m_ItemCounter.value();
        }

        /// Returns reference to internal statistics
        stat const& statistics() const
        {
            return m_Stat;
        }

    };

}}  // namespace cds::container

#endif // #ifndef __CDS_CONTAINER_RWQUEUE_H
