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

#ifndef CDSLIB_CONTAINER_SEGMENTED_STACK_H
#define CDSLIB_CONTAINER_SEGMENTED_STACK_H

#include <memory>
#include <functional>   // ref
#include <cds/intrusive/segmented_stack.h>
#include <cds/details/trivial_assign.h>

namespace cds { namespace container {

    /// SegmentedStack -related declarations
    namespace segmented_stack {

#   ifdef CDS_DOXYGEN_INVOKED
        /// SegmentedStack internal statistics
        typedef cds::intrusive::segmented_stack::stat stat;
#   else
        using cds::intrusive::segmented_stack::stat;
#   endif

        /// SegmentedStack empty internal statistics (no overhead)
        typedef cds::intrusive::segmented_stack::empty_stat empty_stat;

        /// SegmentedStack default type traits
        struct traits {

            /// Item allocator. Default is \ref CDS_DEFAULT_ALLOCATOR
            typedef CDS_DEFAULT_ALLOCATOR   node_allocator;

            typedef atomicity::item_counter item_counter;

            /// Internal statistics, possible predefined types are \ref stat, \ref empty_stat (the default)
            typedef segmented_stack::empty_stat        stat;

            /// Memory model, default is opt::v::relaxed_ordering. See cds::opt::memory_model for the full list of possible types
            typedef opt::v::relaxed_ordering  memory_model;

            /// Alignment of critical data, default is cache line alignment. See cds::opt::alignment option specification
            enum { alignment = opt::cache_line_alignment };

            enum { padding = cds::intrusive::segmented_stack::traits::padding };

            /// Segment allocator. Default is \ref CDS_DEFAULT_ALLOCATOR
            typedef CDS_DEFAULT_ALLOCATOR allocator;

            /// Lock type used to maintain an internal list of allocated segments
            typedef cds::sync::spin lock_type;
        };

        template <typename... Options>
        struct make_traits {
#   ifdef CDS_DOXYGEN_INVOKED
            typedef implementation_defined type ;   ///< Metafunction result
#   else
            typedef typename cds::opt::make_options<
                typename cds::opt::find_type_traits< traits, Options... >::type
                ,Options...
            >::type   type;
#   endif
        };

    } // namespace segmented_stack

    //@cond
    namespace details {

        template <typename GC, typename T, typename Traits>
        struct make_segmented_stack
        {
            typedef GC      gc;
            typedef T       value_type;
            typedef Traits  original_type_traits;

            typedef cds::details::Allocator< T, typename original_type_traits::node_allocator > cxx_node_allocator;
            struct node_disposer {
                void operator()( T * p )
                {
                    cxx_node_allocator().Delete( p );
                }
            };

            struct intrusive_type_traits: public original_type_traits
            {
                typedef node_disposer   disposer;
            };

            typedef cds::intrusive::SegmentedStack< gc, value_type, intrusive_type_traits > type;
        };

    } // namespace details
    //@endcond

    /// Segmented stack
    /** @ingroup cds_nonintrusive_stack

    The stack is based on work
    - [2014] Henzinger, Kirsch, Payer, Sezgin, Sokolova Quantitative Relaxation of Concurrent Data Structures

        Template parameters:
        - \p GC - a garbage collector, possible types are cds::gc::HP, cds::gc::DHP
        - \p T - the type of values stored in the stack
        - \p Traits - stack type traits, default is \p segmented_stack::traits.
            \p segmented_stack::make_traits metafunction can be used to construct your
            type traits.
    */
    template <class GC, typename T, typename Traits = segmented_stack::traits >
    class SegmentedStack:
#ifdef CDS_DOXYGEN_INVOKED
        public cds::intrusive::SegmentedStack< GC, T, Traits >
#else
        public details::make_segmented_stack< GC, T, Traits >::type
#endif
    {
        //@cond
        typedef details::make_segmented_stack< GC, T, Traits > maker;
        typedef typename maker::type base_class;
        //@endcond
    public:
        typedef GC  gc;         ///< Garbage collector
        typedef T   value_type; ///< type of the value stored in the stack
        typedef Traits traits;  ///< Stack traits

        typedef typename traits::node_allocator node_allocator;   ///< Node allocator
        typedef typename base_class::memory_model  memory_model;   ///< Memory ordering. See cds::opt::memory_model option
        typedef typename base_class::item_counter  item_counter;   ///< Item counting policy, see cds::opt::item_counter option setter
        typedef typename base_class::stat          stat        ;   ///< Internal statistics policy
        typedef typename base_class::lock_type     lock_type   ;   ///< Type of mutex for maintaining an internal list of allocated segments.

        static const size_t c_nHazardPtrCount = base_class::c_nHazardPtrCount ; ///< Count of hazard pointer required for the algorithm

    protected:
        //@cond
        typedef typename maker::cxx_node_allocator  cxx_node_allocator;
        typedef std::unique_ptr< value_type, typename maker::node_disposer >  scoped_node_ptr;

        static value_type * alloc_node( value_type const& v )
        {
            return cxx_node_allocator().New( v );
        }

        static value_type * alloc_node()
        {
            return cxx_node_allocator().New();
        }

        template <typename... Args>
        static value_type * alloc_node_move( Args&&... args )
        {
            return cxx_node_allocator().MoveNew( std::forward<Args>( args )... );
        }
        //@endcond

    public:
        /// Initializes the empty stack
        SegmentedStack(
            size_t nQuasiFactor     ///< Quasi factor. If it is not a power of 2 it is rounded up to nearest power of 2. Minimum is 2.
            )
            : base_class( nQuasiFactor )
        {}

        /// Clears the stack and deletes all internal data
        ~SegmentedStack()
        {}

        bool push( value_type const& val )
        {
            scoped_node_ptr p( alloc_node(val));
            if ( base_class::push( *p )) {
                p.release();
                return true;
            }
            return false;
        }

        /// Inserts a new element at last segment of the stack, move semantics
        bool push( value_type&& val )
        {
            scoped_node_ptr p( alloc_node_move( std::move( val )));
            if ( base_class::push( *p )) {
                p.release();
                return true;
            }
            return false;
        }

        template <typename Func>
        bool push_with( Func f )
        {
            scoped_node_ptr p( alloc_node());
            f( *p );
            if ( base_class::push( *p )) {
                p.release();
                return true;
            }
            return false;
        }


        template <typename... Args>
        bool emplace( Args&&... args )
        {
            scoped_node_ptr p( alloc_node_move( std::forward<Args>(args)... ));
            if ( base_class::push( *p )) {
                p.release();
                return true;
            }
            return false;
        }

        /// Pop a value from the stack
        bool pop( value_type& dest )
        {
            return pop_with( [&dest]( value_type& src ) { dest = std::move( src );});
        }

        /// Pop a value using a functor
        template <typename Func>
        bool pop_with( Func f )
        {
            value_type * p = base_class::pop();
            if ( p ) {
                f( *p );
                gc::template retire< typename maker::node_disposer >( p );
                return true;
            }
            return false;
        }


        /// Checks if the stack is empty
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
        size_t size() const
        {
            return base_class::size();
        }

        /// Returns reference to internal statistics
        const stat& statistics() const
        {
            return base_class::statistics();
        }

        /// Returns quasi factor, a power-of-two number
        size_t quasi_factor() const
        {
            return base_class::quasi_factor();
        }
    };

}} // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_SEGMENTED_STACK_H
