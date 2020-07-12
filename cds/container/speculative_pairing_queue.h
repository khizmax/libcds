/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CDSLIB_CONTAINER_SPECULATIVE_PAIRING_QUEUE_H
#define CDSLIB_CONTAINER_SPECULATIVE_PAIRING_QUEUE_H

#include <memory>
#include <cds/intrusive/speculative_pairing_queue.h>
#include <cds/container/details/base.h>

namespace cds { namespace container {

    /// MSQueue related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace speculative_pairing_queue {
        /// Internal statistics
        template <typename Counter = cds::intrusive::speculative_pairing_queue::stat<>::counter_type >
        using stat = cds::intrusive::speculative_pairing_queue::stat< Counter >;

        /// Dummy internal statistics
        typedef cds::intrusive::speculative_pairing_queue::empty_stat empty_stat;

        /// MSQueue default type traits
        struct traits
        {
            /// Node allocator
            typedef CDS_DEFAULT_ALLOCATOR       allocator;

            /// Back-off strategy
            typedef cds::backoff::empty         back_off;

            /// Item counting feature; by default, disabled. Use \p cds::atomicity::item_counter to enable item counting
            typedef atomicity::empty_item_counter   item_counter;

            /// Internal statistics (by default, disabled)
            /**
                Possible option value are: \p msqueue::stat, \p msqueue::empty_stat (the default),
                user-provided class that supports \p %msqueue::stat interface.
            */
            typedef speculative_pairing_queue::empty_stat         stat;

            /// C++ memory ordering model
            /**
                Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
            */
            typedef opt::v::relaxed_ordering    memory_model;

            /// Padding for internal critical atomic data. Default is \p opt::cache_line_padding
            enum { padding = opt::cache_line_padding };
        };

        /// Metafunction converting option list to \p msqueue::traits
        /**
            Supported \p Options are:
            - \p opt::allocator - allocator (like \p std::allocator) used for allocating queue nodes. Default is \ref CDS_DEFAULT_ALLOCATOR
            - \p opt::back_off - back-off strategy used, default is \p cds::backoff::empty.
            - \p opt::item_counter - the type of item counting feature. Default is \p cds::atomicity::empty_item_counter (item counting disabled)
                To enable item counting use \p cds::atomicity::item_counter
            - \p opt::stat - the type to gather internal statistics.
                Possible statistics types are: \p msqueue::stat, \p msqueue::empty_stat, user-provided class that supports \p %msqueue::stat interface.
                Default is \p %msqueue::empty_stat.
            - \p opt::padding - padding for internal critical atomic data. Default is \p opt::cache_line_padding
            - \p opt::memory_model - C++ memory ordering model. Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).

            Example: declare \p %MSQueue with item counting and internal statistics
            \code
            typedef cds::container::MSQueue< cds::gc::HP, Foo,
                typename cds::container::msqueue::make_traits<
                    cds::opt::item_counter< cds::atomicity::item_counter >,
                    cds::opt::stat< cds::container::msqueue::stat<> >
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
    } // namespace msqueue

    //@cond
    namespace details {
        template <typename GC, typename T, typename Traits>
        struct make_spqueue
        {
            typedef GC gc;
            typedef T value_type;
            typedef Traits traits;

            struct node_type : public intrusive::sp_queue::node< gc >
            {
                value_type  m_value;

                node_type( value_type const& val )
                    : m_value( val )
                {}

                template <typename... Args>
                node_type( Args&&... args )
                    : m_value( std::forward<Args>( args )... )
                {}
            };

            typedef typename traits::allocator::template rebind<node_type>::other allocator_type;
            typedef cds::details::Allocator< node_type, allocator_type >           cxx_allocator;

            struct node_deallocator
            {
                void operator ()( node_type * pNode )
                {
                    cxx_allocator().Delete( pNode );
                }
            };

            struct intrusive_traits : public traits
            {
                typedef cds::intrusive::speculative_pairing_queue::base_hook< cds::opt::gc<gc> > hook;
                typedef node_deallocator disposer;
                static constexpr const cds::intrusive::opt::link_check_type link_checker = cds::intrusive::speculative_pairing_queue::traits::link_checker;
            };

            typedef intrusive::SPQueue< gc, node_type, intrusive_traits > type;
        };
    }
    //@endcond

    /// Michael & Scott lock-free queue
    /** @ingroup cds_nonintrusive_queue
        It is non-intrusive version of Michael & Scott's queue algorithm based on intrusive implementation
        \p cds::intrusive::MSQueue.

        Template arguments:
        - \p GC - garbage collector type: \p gc::HP, \p gc::DHP
        - \p T is a type stored in the queue.
        - \p Traits - queue traits, default is \p msqueue::traits. You can use \p msqueue::make_traits
            metafunction to make your traits or just derive your traits from \p %msqueue::traits:
            \code
            struct myTraits: public cds::container::msqueue::traits {
                typedef cds::intrusive::msqueue::stat<> stat;
                typedef cds::atomicity::item_counter    item_counter;
            };
            typedef cds::container::MSQueue< cds::gc::HP, Foo, myTraits > myQueue;

            // Equivalent make_traits example:
            typedef cds::container::MSQueue< cds::gc::HP, Foo,
                typename cds::container::msqueue::make_traits<
                    cds::opt::stat< cds::container::msqueue::stat<> >,
                    cds::opt::item_counter< cds::atomicity::item_counter >
                >::type
            > myQueue;
            \endcode
    */
    template <typename GC, typename T, typename Traits = cds::container::speculative_pairing_queue::traits>
    class SPQueue:
#ifdef CDS_DOXYGEN_INVOKED
        private intrusive::SPQueue< GC, cds::intrusive::speculative_pairing_queue::node< T >, Traits >
#else
        private details::make_spqueue< GC, T, Traits >::type
#endif
    {
        //@cond
        typedef details::make_spqueue< GC, T, Traits > maker;
        typedef typename maker::type base_class;
        //@endcond

    public:
        /// Rebind template arguments
        template <typename GC2, typename T2, typename Traits2>
        struct rebind {
            typedef SPQueue< GC2, T2, Traits2> other   ;   ///< Rebinding result
        };

    public:
        typedef T value_type;   ///< Value type stored in the queue
        typedef Traits traits;  ///< Queue traits

        typedef typename base_class::gc             gc;             ///< Garbage collector used
        typedef typename base_class::back_off       back_off;       ///< Back-off strategy used
        typedef typename maker::allocator_type      allocator_type; ///< Allocator type used for allocate/deallocate the nodes
        typedef typename base_class::item_counter   item_counter;   ///< Item counting policy used
        typedef typename base_class::stat           stat;           ///< Internal statistics policy used
        typedef typename base_class::memory_model   memory_model;   ///< Memory ordering. See cds::opt::memory_model option

        static constexpr const size_t c_nHazardPtrCount = base_class::c_nHazardPtrCount; ///< Count of hazard pointer required for the algorithm

    protected:
        //@cond
        typedef typename maker::node_type  node_type;   ///< queue node type (derived from \p intrusive::msqueue::node)

        typedef typename maker::cxx_allocator     cxx_allocator;
        typedef typename maker::node_deallocator  node_deallocator;   // deallocate node
        typedef typename base_class::node_traits  node_traits;
        //@endcond

    protected:
        ///@cond
        static node_type * alloc_node()
        {
            return cxx_allocator().New();
        }
        static node_type * alloc_node( value_type const& val )
        {
            return cxx_allocator().New( val );
        }
        template <typename... Args>
        static node_type * alloc_node_move( Args&&... args )
        {
            return cxx_allocator().MoveNew( std::forward<Args>( args )... );
        }
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
        SPQueue()
        {}

        /// Destructor clears the queue
        ~SPQueue()
        {}

        /// Enqueues \p val value into the queue.
        /**
            The function makes queue node in dynamic memory calling copy constructor for \p val
            and then it calls \p intrusive::MSQueue::enqueue.
            Returns \p true if success, \p false otherwise.
        */
        bool enqueue( value_type const& val )
        {
            scoped_node_ptr p( alloc_node(val));
            if ( base_class::enqueue( *p )) {
                p.release();
                return true;
            }
            return false;
        }

        /// Enqueues \p val in the queue, move semantics
        bool enqueue( value_type&& val )
        {
            scoped_node_ptr p( alloc_node_move( std::move( val )));
            if ( base_class::enqueue( *p )) {
                p.release();
                return true;
            }
            return false;
        }

        /// Enqueues data to the queue using a functor
        /**
            \p Func is a functor called to create node.
            The functor \p f takes one argument - a reference to a new node of type \ref value_type :
            \code
            cds::container::MSQueue< cds::gc::HP, Foo > myQueue;
            Bar bar;
            myQueue.enqueue_with( [&bar]( Foo& dest ) { dest = bar; } );
            \endcode
        */
        template <typename Func>
        bool enqueue_with( Func f )
        {
            scoped_node_ptr p( alloc_node());
            f( p->m_value );
            if ( base_class::enqueue( *p )) {
                p.release();
                return true;
            }
            return false;
        }

        /// Enqueues data of type \ref value_type constructed from <tt>std::forward<Args>(args)...</tt>
        template <typename... Args>
        bool emplace( Args&&... args )
        {
            scoped_node_ptr p( alloc_node_move( std::forward<Args>( args )... ));
            if ( base_class::enqueue( *p )) {
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

        /// Synonym for \p enqueue() function
        bool push( value_type&& val )
        {
            return enqueue( std::move( val ));
        }

        /// Synonym for \p enqueue_with() function
        template <typename Func>
        bool push_with( Func f )
        {
            return enqueue_with( f );
        }

        /// Dequeues a value from the queue
        /**
            If queue is not empty, the function returns \p true, \p dest contains copy of
            dequeued value. The assignment operator for type \ref value_type is invoked.
            If queue is empty, the function returns \p false, \p dest is unchanged.
        */
        bool dequeue( value_type& dest )
        {
            return dequeue_with( [&dest]( value_type& src ) { dest = std::move( src );});
        }

        /// Dequeues a value using a functor
        /**
            \p Func is a functor called to copy dequeued value.
            The functor takes one argument - a reference to removed node:
            \code
            cds:container::MSQueue< cds::gc::HP, Foo > myQueue;
            Bar bar;
            myQueue.dequeue_with( [&bar]( Foo& src ) { bar = std::move( src );});
            \endcode
            The functor is called only if the queue is not empty.
        */
        template <typename Func>
        bool dequeue_with( Func f )
        {
            node_type* p = base_class::dequeue();
            if ( p) {
                f( p->m_value );
                return true;
            }
            return false;
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

        /// Clear the queue
        /**
            The function repeatedly calls \ref dequeue until it returns \p nullptr.
        */
        void clear()
        {
            base_class::clear();
        }

        /// Checks if the queue is empty
        bool empty() const
        {
            return base_class::empty();
        }

        /// Returns queue's item count (see \ref intrusive::MSQueue::size for explanation)
        /** \copydetails cds::intrusive::MSQueue::size()
        */
        size_t size() const
        {
            return base_class::size();
        }

        /// Returns reference to internal statistics
        const stat& statistics() const
        {
            return base_class::statistics();
        }
    };

}}  // namespace cds::container

#endif  // #ifndef CDSLIB_CONTAINER_MSQUEUE_H
