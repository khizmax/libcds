// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_TREIBER_STACK_H
#define CDSLIB_CONTAINER_TREIBER_STACK_H

#include <memory>   // unique_ptr
#include <cds/intrusive/treiber_stack.h>
#include <cds/container/details/base.h>

namespace cds { namespace container {

    /// TreiberStack related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace treiber_stack {
        /// Internal statistics
        template <typename Counter = cds::intrusive::treiber_stack::stat<>::counter_type >
        using stat = cds::intrusive::treiber_stack::stat< Counter >;

        /// Dummy internal statistics
        typedef cds::intrusive::treiber_stack::empty_stat empty_stat;

        /// TreiberStack default type traits
        struct traits
        {
            /// Back-off strategy
            typedef cds::backoff::Default       back_off;

            /// Node allocator
            typedef CDS_DEFAULT_ALLOCATOR       allocator;

            /// C++ memory ordering model
            /**
                Can be opt::v::relaxed_ordering (relaxed memory model, the default)
                or opt::v::sequential_consistent (sequentially consisnent memory model).
            */
            typedef opt::v::relaxed_ordering    memory_model;

            /// Item counting feature; by default, disabled. Use \p cds::atomicity::item_counter to enable item counting
            typedef cds::atomicity::empty_item_counter  item_counter;

            /// Internal statistics (by default, no internal statistics)
            /**
                Possible types are: \ref treiber_stack::stat, \ref treiber_stack::empty_stat (the default),
                user-provided class that supports treiber_stack::stat interface.
            */
            typedef empty_stat stat;

            /** @name Elimination back-off traits
                The following traits is used only if elimination enabled
            */
            ///@{

            /// Enable elimination back-off; by default, it is disabled
            static constexpr const bool enable_elimination = false;

            /// Elimination record storage type
            typedef cds::algo::elimination::storage elimination_storage;

            /// Back-off strategy to wait for elimination, default is cds::backoff::delay<>
            typedef cds::backoff::delay<>          elimination_backoff;

            /// Buffer type for elimination array
            /**
                Possible types are \p opt::v::initialized_static_buffer, \p opt::v::initialized_dynamic_buffer.
                The buffer can be any size: \p Exp2 template parameter of those classes can be \p false.
                The size should be selected empirically for your application and hardware, there are no common rules for that.
                Default is <tt> %opt::v::initialized_static_buffer< any_type, 4 > </tt>.
            */
            typedef opt::v::initialized_static_buffer< int, 4 > buffer;

            /// Random engine to generate a random position in elimination array
            typedef opt::v::c_rand  random_engine;

            /// Lock type used in elimination, default is cds::sync::spin
            typedef cds::sync::spin lock_type;

            ///@}
        };

        /// Metafunction converting option list to \p TreiberStack traits
        /**
            Supported \p Options are:
            - \p opt::allocator - allocator (like \p std::allocator) used for allocating stack nodes. Default is \ref CDS_DEFAULT_ALLOCATOR
            - \p opt::back_off - back-off strategy used. If the option is not specified, the \p cds::backoff::Default is used.
            - \p opt::memory_model - C++ memory ordering model. Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
            - \p opt::item_counter - the type of item counting feature. Default is \p cds::atomicity::empty_item_counter, i.e.
                no item counting. Use \p cds::atomicity::item_counter to enable item counting.
            - \p opt::stat - the type to gather internal statistics.
                Possible option value are: \p treiber_stack::stat, \p treiber_stack::empty_stat (the default),
                user-provided class that supports \p %treiber_stack::stat interface.
            - \p opt::enable_elimination - enable elimination back-off for the stack. Default value is \p false.

            If elimination back-off is enabled, additional options can be specified:
            - \p opt::buffer - an initialized buffer type for elimination array, see \p opt::v::initialized_static_buffer, \p opt::v::initialized_dynamic_buffer.
                The buffer can be any size: \p Exp2 template parameter of those classes can be \p false.
                The size should be selected empirically for your application and hardware, there are no common rules for that.
                Default is <tt> %opt::v::initialized_static_buffer< any_type, 4 > </tt>.
            - \p opt::random_engine - a random engine to generate a random position in elimination array.
                Default is \p opt::v::c_rand.
            - \p opt::elimination_storage - a storage class for elmination records. Default is \p cds::algo::elimination::storage.
            - \p opt::elimination_backoff - back-off strategy to wait for elimination, default is \p cds::backoff::delay<>
            - \p opt::lock_type - a lock type used in elimination back-off, default is \p cds::sync::spin.

            Example: declare %TreiberStack with item counting and internal statistics using \p %make_traits
            \code
            typedef cds::container::TreiberStack< cds::gc::HP, Foo,
                typename cds::container::treiber_stack::make_traits<
                    cds::opt::item_counter< cds::atomicity::item_counter >,
                    cds::opt::stat< cds::intrusive::treiber_stack::stat<> >
                >::type
            > myStack;
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
            >::type   type;
#   endif
        };
    } // namespace treiber_stack

    //@cond
    namespace details {
        template <typename GC, typename T, typename Traits>
        struct make_treiber_stack
        {
            typedef GC gc;
            typedef T       value_type;
            typedef Traits  traits;

            struct node_type: public cds::intrusive::treiber_stack::node< gc >
            {
                value_type  m_value;

                node_type( const value_type& val )
                    : m_value( val )
                {}

                template <typename... Args>
                node_type( Args&&... args )
                    : m_value( std::forward<Args>( args )... )
                {}
            };

            typedef typename std::allocator_traits<typename traits::allocator>::template rebind_alloc< node_type > allocator_type;
            typedef cds::details::Allocator< node_type, allocator_type >           cxx_allocator;

            struct node_deallocator
            {
                void operator ()( node_type * pNode )
                {
                    cxx_allocator().Delete( pNode );
                }
            };

            struct intrusive_traits: public traits
            {
                typedef cds::intrusive::treiber_stack::base_hook< cds::opt::gc<gc> > hook;
                typedef node_deallocator disposer;
                static constexpr const opt::link_check_type link_checker = cds::intrusive::treiber_stack::traits::link_checker;
            };

            // Result of metafunction
            typedef intrusive::TreiberStack< gc, node_type, intrusive_traits > type;
        };
    } // namespace details
    //@endcond

    /// Treiber's stack algorithm
    /** @ingroup cds_nonintrusive_stack
        It is non-intrusive version of Treiber's stack algorithm based on intrusive implementation
        intrusive::TreiberStack.

        Template arguments:
        - \p GC - garbage collector type: \p gc::HP, gc::DHP
        - \p T - type stored in the stack.
        - \p Traits - stack traits, default is \p treiber_stack::traits. You can use \p treiber_stack::make_traits
            metafunction to make your traits or just derive your traits from \p %treiber_stack::traits:
            \code
            struct myTraits: public cds::container::treiber_stack::traits {
                typedef cds::intrusive::treiber_stack::stat<> stat;
                typedef cds::atomicity::item_counter  item_counter;
            };
            typedef cds::container::TreiberStack< cds::gc::HP, Foo, myTraits > myStack;

            // Equivalent make_traits example:
            typedef cds::intrusive::TreiberStack< cds::gc::HP, Foo,
                typename cds::intrusive::treiber_stack::make_traits<
                    cds::opt::item_counter< cds::atomicity::item_counter >,
                    cds::opt::stat< cds::intrusive::treiber_stack::stat<> >
                >::type
            > myStack;
            \endcode
    */
    template <
        typename GC,
        typename T,
        typename Traits = treiber_stack::traits
    >
    class TreiberStack
        : public
#ifdef CDS_DOXYGEN_INVOKED
        intrusive::TreiberStack< GC, cds::intrusive::treiber_stack::node< T >, Traits >
#else
        details::make_treiber_stack< GC, T, Traits >::type
#endif
    {
        //@cond
        typedef details::make_treiber_stack< GC, T, Traits > maker;
        typedef typename maker::type base_class;
        //@endcond

    public:
        /// Rebind template arguments
        template <typename GC2, typename T2, typename Traits2>
        struct rebind {
            typedef TreiberStack< GC2, T2, Traits2 > other;   ///< Rebinding result
        };

    public:
        typedef T value_type ; ///< Value type stored in the stack
        typedef typename base_class::gc gc                     ;   ///< Garbage collector used
        typedef typename base_class::back_off  back_off        ;   ///< Back-off strategy used
        typedef typename maker::allocator_type allocator_type  ;   ///< Allocator type used for allocating/deallocating the nodes
        typedef typename base_class::memory_model  memory_model;   ///< Memory ordering. See cds::opt::memory_order option
        typedef typename base_class::stat       stat           ;   ///< Internal statistics policy used

    protected:
        typedef typename maker::node_type  node_type   ;   ///< stack node type (derived from \p intrusive::treiber_stack::node)

        //@cond
        typedef typename maker::cxx_allocator     cxx_allocator;
        typedef typename maker::node_deallocator  node_deallocator;
        //@endcond

    protected:
        ///@cond
        static node_type * alloc_node( const value_type& val )
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
        static void retire_node( node_type * p )
        {
            gc::template retire<typename base_class::disposer>( p );
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
        /// Constructs empty stack
        TreiberStack()
        {}

        /// Constructs empty stack and initializes elimination back-off data
        /**
            This form should be used if you use elimination back-off with dynamically allocated collision array, i.e
            \p Options... contains cds::opt::buffer< cds::opt::v::initialized_dynamic_buffer >.
            \p nCollisionCapacity parameter specifies the capacity of collision array.
        */
        TreiberStack( size_t nCollisionCapacity )
            : base_class( nCollisionCapacity )
        {}

        /// \p %TreiberStack is not copy-constructible
        TreiberStack( TreiberStack const& ) = delete;

        /// Clears the stack on destruction
        ~TreiberStack()
        {}

        /// Pushes copy of \p val on the stack
        bool push( value_type const& val )
        {
            scoped_node_ptr p( alloc_node(val));
            if ( base_class::push( *p )) {
                p.release();
                return true;
            }
            return false;
        }

        /// Pushes data of type \ref value_type created from <tt>std::forward<Args>(args)...</tt>
        template <typename... Args>
        bool emplace( Args&&... args )
        {
            scoped_node_ptr p( alloc_node_move( std::forward<Args>(args)...));
            if ( base_class::push( *p )) {
                p.release();
                return true;
            }
            return false;
        }

        /// Pops an item from the stack
        /**
            The value of popped item is stored in \p val using assignment operator.
            On success functions returns \p true, \p val contains value popped from the stack.
            If stack is empty the function returns \p false, \p val is unchanged.
        */
        bool pop( value_type& val )
        {
            return pop_with( [&val]( value_type& src ) { val = std::move(src); } );
        }

        /// Pops an item from the stack with functor
        /**
            \p Func can be used to copy/move popped item from the stack.
            \p Func interface is:
            \code
            void func( value_type& src );
            \endcode
            where \p src - item popped.
        */
        template <typename Func>
        bool pop_with( Func f )
        {
            node_type * p = base_class::pop();
            if ( !p )
                return false;

            f( p->m_value );
            retire_node( p );

            return true;
        }

        /// Check if stack is empty
        bool empty() const
        {
            return base_class::empty();
        }

        /// Clear the stack
        void clear()
        {
            base_class::clear();
        }

        /// Returns stack's item count
        /**
            The value returned depends on opt::item_counter option. For atomicity::empty_item_counter,
            this function always returns 0.

            <b>Warning</b>: even if you use real item counter and it returns 0, this fact is not mean that the stack
            is empty. To check emptyness use \ref empty() method.
        */
        size_t    size() const
        {
            return base_class::size();
        }

        /// Returns reference to internal statistics
        stat const& statistics() const
        {
            return base_class::statistics();
        }
    };

}}  // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_TREIBER_STACK_H
