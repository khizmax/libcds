//$$CDS-header$$

#ifndef __CDS_CONTAINER_MOIR_QUEUE_H
#define __CDS_CONTAINER_MOIR_QUEUE_H

#include <memory>
#include <cds/intrusive/moir_queue.h>
#include <cds/intrusive/queue_stat.h>
#include <cds/container/base.h>
#include <cds/ref.h>
#include <cds/details/trivial_assign.h>

namespace cds { namespace container {

    //@cond
    namespace details {
        template <typename GC, typename T, CDS_DECL_OPTIONS7>
        struct make_moir_queue
        {
            typedef GC gc;
            typedef T value_type;

            struct default_options {
                typedef cds::backoff::empty     back_off;
                typedef CDS_DEFAULT_ALLOCATOR   allocator;
                typedef atomicity::empty_item_counter item_counter;
                typedef intrusive::queue_dummy_stat stat;
                typedef opt::v::relaxed_ordering    memory_model;
                enum { alignment = opt::cache_line_alignment };
            };

            typedef typename opt::make_options<
                typename cds::opt::find_type_traits< default_options, CDS_OPTIONS7 >::type
                ,CDS_OPTIONS7
            >::type   options;

            struct node_type: public intrusive::single_link::node< gc >
            {
                value_type  m_value;

                node_type( const value_type& val )
                    : m_value( val )
                {}
#           ifdef CDS_EMPLACE_SUPPORT
                template <typename... Args>
                node_type( Args&&... args )
                    : m_value( std::forward<Args>(args)...)
                {}
#           else
                node_type()
                {}
#           endif
            };

            typedef typename options::allocator::template rebind<node_type>::other allocator_type;
            typedef cds::details::Allocator< node_type, allocator_type >           cxx_allocator;

            struct node_deallocator
            {
                void operator ()( node_type * pNode )
                {
                    cxx_allocator().Delete( pNode );
                }
            };

            typedef intrusive::MoirQueue<
                gc
                ,node_type
                ,intrusive::opt::hook<
                    intrusive::single_link::base_hook< opt::gc<gc> >
                >
                ,opt::back_off< typename options::back_off >
                ,intrusive::opt::disposer< node_deallocator >
                ,opt::item_counter< typename options::item_counter >
                ,opt::stat< typename options::stat >
                ,opt::alignment< options::alignment >
                ,opt::memory_model< typename options::memory_model >
            >   type;
        };
    }
    //@endcond

    /// A variation of Michael & Scott's lock-free queue
    /** @ingroup cds_nonintrusive_queue
        It is non-intrusive version of intrusive::MoirQueue.

        \p T is a type stored in the queue. It should be default-constructible, copy-constructible, assignable type.

        \p Options description see MSQueue
    */
    template <typename GC, typename T, CDS_DECL_OPTIONS7>
    class MoirQueue:
#ifdef CDS_DOXYGEN_INVOKED
        intrusive::MoirQueue< GC, intrusive::single_link::node< T >, Options... >
#else
        details::make_moir_queue< GC, T, CDS_OPTIONS7 >::type
#endif
    {
        //@cond
        typedef details::make_moir_queue< GC, T, CDS_OPTIONS7 > options;
        typedef typename options::type base_class;
        //@endcond

    public:
        /// Rebind template arguments
        template <typename GC2, typename T2, CDS_DECL_OTHER_OPTIONS7>
        struct rebind {
            typedef MoirQueue< GC2, T2, CDS_OTHER_OPTIONS7> other   ;   ///< Rebinding result
        };

    public:
        typedef T value_type ; ///< Value type stored in the stack

        typedef typename base_class::gc                 gc              ; ///< Garbage collector used
        typedef typename base_class::back_off           back_off        ; ///< Back-off strategy used
        typedef typename options::allocator_type        allocator_type  ; ///< Allocator type used for allocate/deallocate the nodes
        typedef typename options::options::item_counter item_counter    ; ///< Item counting policy used
        typedef typename options::options::stat         stat            ; ///< Internal statistics policy used
        typedef typename base_class::memory_model       memory_model    ; ///< Memory ordering. See cds::opt::memory_model option

    protected:
        typedef typename options::node_type  node_type   ;   ///< queue node type (derived from intrusive::single_link::node)

        //@cond
        typedef typename options::cxx_allocator     cxx_allocator;
        typedef typename options::node_deallocator  node_deallocator;   // deallocate node
        typedef typename base_class::node_traits    node_traits;
        //@endcond

    protected:
        ///@cond
        static node_type * alloc_node()
        {
            return cxx_allocator().New();
        }
        static node_type * alloc_node( const value_type& val )
        {
            return cxx_allocator().New( val );
        }
#   ifdef CDS_EMPLACE_SUPPORT
        template <typename... Args>
        static node_type * alloc_node_move( Args&&... args )
        {
            return cxx_allocator().MoveNew( std::forward<Args>( args )... );
        }
#   endif
        static void free_node( node_type * p )
        {
            node_deallocator()( p );
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
        /// Initializes empty queue
        MoirQueue()
        {}

        /// Destructor clears the queue
        ~MoirQueue()
        {}

        /// Returns queue's item count (see \ref intrusive::MSQueue::size for explanation)
        size_t size() const
        {
            return base_class::size();
        }

        /// Returns refernce to internal statistics
        const stat& statistics() const
        {
            return base_class::statistics();
        }

        /// Enqueues \p val value into the queue.
        /**
            The function makes queue node in dynamic memory calling copy constructor for \p val
            and then it calls intrusive::MSQueue::enqueue.
            Returns \p true if success, \p false otherwise.
        */
        bool enqueue( value_type const& val )
        {
            scoped_node_ptr p( alloc_node(val) );
            if ( base_class::enqueue( *p )) {
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
                void operator()(T& dest, SOURCE const& data)
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
        bool enqueue( const Type& data, Func f  )
        {
            scoped_node_ptr p( alloc_node());
            unref(f)( node_traits::to_value_ptr( *p )->m_value, data );
            if ( base_class::enqueue( *p )) {
                p.release();
                return true;
            }
            return false;
        }

        /// Dequeues a value using copy functor
        /**
            \p Func is a functor called to copy dequeued value to \p dest of type \p Type
            which may be differ from type \p T stored in the queue.
            The functor's interface is:
            \code
            struct myFunctor {
                void operator()(Type& dest, T const& data)
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
        bool dequeue( Type& dest, Func f )
        {
            typename base_class::dequeue_result res;
            if ( base_class::do_dequeue( res )) {
                unref(f)( dest, node_traits::to_value_ptr( *res.pNext )->m_value );

                base_class::dispose_result( res );

                return true;
            }
            return false;
        }

        /// Dequeues a value from the queue
        /**
            If queue is not empty, the function returns \p true, \p dest contains copy of
            dequeued value. The assignment operator for type \ref value_type is invoked.
            If queue is empty, the function returns \p false, \p dest is unchanged.
        */
        bool dequeue( value_type& dest )
        {
            typedef cds::details::trivial_assign<value_type, value_type> functor;
            return dequeue( dest, functor() );
        }

        /// Synonym for \ref enqueue function
        bool push( const value_type& val )
        {
            return enqueue( val );
        }

        /// Synonym for template version of \ref enqueue function
        template <typename Type, typename Func>
        bool push( const Type& data, Func f  )
        {
            return enqueue( data, f );
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
            if ( base_class::enqueue( *p )) {
                p.release();
                return true;
            }
            return false;
        }
#   endif


        /// Synonym for \ref dequeue function
        bool pop( value_type& dest )
        {
            return dequeue( dest );
        }

        /// Synonym for template version of \ref dequeue function
        template <typename Type, typename Func>
        bool pop( Type& dest, Func f )
        {
            return dequeue( dest, f );
        }

        /// Checks if the queue is empty
        bool empty() const
        {
            return base_class::empty();
        }

        /// Clear the queue
        /**
            The function repeatedly calls \ref dequeue until it returns NULL.
            The disposer defined in template \p Options is called for each item
            that can be safely disposed.
        */
        void clear()
        {
            base_class::clear();
        }
    };

}}  // namespace cds::container

#endif  // #ifndef __CDS_CONTAINER_MOIR_QUEUE_H


