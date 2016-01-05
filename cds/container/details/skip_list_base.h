/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

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

#ifndef CDSLIB_CONTAINER_DETAILS_SKIP_LIST_BASE_H
#define CDSLIB_CONTAINER_DETAILS_SKIP_LIST_BASE_H

#include <cds/intrusive/details/skip_list_base.h>
#include <cds/container/details/base.h>

namespace cds { namespace container {

    /// SkipListSet related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace skip_list {
        /// Option specifying random level generator
        template <typename Type>
        using random_level_generator = cds::intrusive::skip_list::random_level_generator<Type>;

        /// Xor-shift random level generator
        typedef cds::intrusive::skip_list::xorshift xorshift;

        /// Turbo-pascal random level generator
        typedef cds::intrusive::skip_list::turbo_pascal turbo_pascal;

        /// Skip list internal statistics
        template <typename EventCounter = cds::atomicity::event_counter>
        using stat = cds::intrusive::skip_list::stat < EventCounter >;

        /// Skip list empty internal statistics
        typedef cds::intrusive::skip_list::empty_stat empty_stat;

        /// SkipListSet traits
        struct traits
        {
            /// Key comparison functor
            /**
                No default functor is provided. If the option is not specified, the \p less is used.
            */
            typedef opt::none                       compare;

            /// specifies binary predicate used for key compare.
            /**
                Default is \p std::less<T>.
            */
            typedef opt::none                       less;

            /// Item counter
            /**
                The type for item counting feature,
                 by defaulr disabled (\p atomicity::empty_item_counter)
            */
            typedef atomicity::empty_item_counter     item_counter;

            /// C++ memory ordering model
            /**
                List of available memory ordering see \p opt::memory_model
            */
            typedef opt::v::relaxed_ordering        memory_model;

            /// Random level generator
            /**
                The random level generator is an important part of skip-list algorithm.
                The node height in the skip-list have a probabilistic distribution
                where half of the nodes that have level \p i also have level <tt>i+1</tt>
                (i = 0..30). The height of a node is in range [0..31].

                See \p skip_list::random_level_generator option setter.
            */
            typedef turbo_pascal                    random_level_generator;

            /// Allocator for skip-list nodes, \p std::allocator interface
            typedef CDS_DEFAULT_ALLOCATOR           allocator;

            /// back-off strategy, default is \p cds::backoff::Default
            typedef cds::backoff::Default           back_off;

            /// Internal statistics, by default disabled. To enable, use \p split_list::stat
            typedef empty_stat                      stat;

            /// RCU deadlock checking policy (for \ref cds_nonintrusive_SkipListSet_rcu "RCU-based SkipListSet")
            /**
                List of available options see opt::rcu_check_deadlock
            */
            typedef opt::v::rcu_throw_deadlock      rcu_check_deadlock;

            //@cond
            // For internal use only
            typedef opt::none                       key_accessor;
            //@endcond
        };

        /// Metafunction converting option list to SkipListSet traits
        /**
            \p Options are:
            - \p opt::compare - key comparison functor. No default functor is provided.
                If the option is not specified, the \p opt::less is used.
            - \p opt::less - specifies binary predicate used for key comparison. Default is \p std::less<T>.
            - \p opt::item_counter - the type of item counting feature. Default is \p atomicity::empty_item_counter that is no item counting.
            - \p opt::memory_model - C++ memory ordering model. Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
            - \p skip_list::random_level_generator - random level generator. Can be \p skip_list::xorshift, \p skip_list::turbo_pascal or
                user-provided one.
                Default is \p %skip_list::turbo_pascal.
            - \p opt::allocator - allocator for skip-list node. Default is \ref CDS_DEFAULT_ALLOCATOR.
            - \p opt::back_off - back-off strategy used. If the option is not specified, the \p cds::backoff::Default is used.
            - \p opt::stat - internal statistics. Available types: \p skip_list::stat, \p skip_list::empty_stat (the default)
            - \p opt::rcu_check_deadlock - a deadlock checking policy for RCU-based skip-list.
                Default is \p opt::v::rcu_throw_deadlock

        */
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

        //@cond
        namespace details {

            template <typename Node, typename Traits>
            class node_allocator
            {
            protected:
                typedef Node node_type;
                typedef Traits traits;

                typedef typename node_type::tower_item_type node_tower_item;
                typedef typename traits::allocator::template rebind<unsigned char>::other  tower_allocator_type;
                typedef typename traits::allocator::template rebind<node_type>::other      node_allocator_type;

                static size_t const c_nTowerItemSize = sizeof(node_tower_item);
                static size_t const c_nNodePadding = sizeof(node_type) % c_nTowerItemSize;
                static size_t const c_nNodeSize = sizeof(node_type) + (c_nNodePadding ? (c_nTowerItemSize - c_nNodePadding) : 0);

                static CDS_CONSTEXPR size_t node_size( unsigned int nHeight ) CDS_NOEXCEPT
                {
                    return c_nNodeSize + (nHeight - 1) * c_nTowerItemSize;
                }
                static unsigned char * alloc_space( unsigned int nHeight )
                {
                    if ( nHeight > 1 ) {
                        unsigned char * pMem = tower_allocator_type().allocate( node_size(nHeight) );

                        // check proper alignments
                        assert( (((uintptr_t) pMem) & (alignof(node_type) - 1)) == 0 );
                        assert( (((uintptr_t) (pMem + c_nNodeSize)) & (alignof(node_tower_item) - 1)) == 0 );
                        return pMem;
                    }

                    return reinterpret_cast<unsigned char *>( node_allocator_type().allocate(1));
                }

                static void free_space( unsigned char * p, unsigned int nHeight )
                {
                    assert( p != nullptr );
                    if ( nHeight == 1 )
                        node_allocator_type().deallocate( reinterpret_cast<node_type *>(p), 1 );
                    else
                        tower_allocator_type().deallocate( p, node_size(nHeight));
                }

            public:
                template <typename Q>
                node_type * New( unsigned int nHeight, Q const& v )
                {
                    unsigned char * pMem = alloc_space( nHeight );
                    node_type * p = new( pMem )
                        node_type( nHeight, nHeight > 1 ? reinterpret_cast<node_tower_item *>(pMem + c_nNodeSize) : nullptr, v );
                    return p;
                }

                template <typename... Args>
                node_type * New( unsigned int nHeight, Args&&... args )
                {
                    unsigned char * pMem = alloc_space( nHeight );
                    node_type * p = new( pMem )
                        node_type( nHeight, nHeight > 1 ? reinterpret_cast<node_tower_item *>(pMem + c_nNodeSize) : nullptr,
                        std::forward<Args>(args)... );
                    return p;
                }

                void Delete( node_type * p )
                {
                    assert( p != nullptr );

                    unsigned int nHeight = p->height();
                    node_allocator_type().destroy( p );
                    free_space( reinterpret_cast<unsigned char *>(p), nHeight );
                }
            };

            template <typename IntrusiveNode>
            struct dummy_node_builder {
                typedef IntrusiveNode intrusive_node_type;

                template <typename RandomGen>
                static intrusive_node_type * make_tower( intrusive_node_type * pNode, RandomGen& /*gen*/ ) { return pNode ; }
                static intrusive_node_type * make_tower( intrusive_node_type * pNode, unsigned int /*nHeight*/ ) { return pNode ; }
                static void dispose_tower( intrusive_node_type * pNode )
                {
                    pNode->release_tower();
                }

                struct node_disposer {
                    void operator()( intrusive_node_type * /*pNode*/ ) const {}
                };
            };

            template <typename ForwardIterator>
            class iterator
            {
                typedef ForwardIterator intrusive_iterator;
                typedef typename intrusive_iterator::value_type node_type;
                typedef typename node_type::stored_value_type   value_type;
                static bool const c_isConst = intrusive_iterator::c_isConst;

                typedef typename std::conditional< c_isConst, value_type const&, value_type&>::type   value_ref;
                template <typename FwdIt> friend class iterator;

                intrusive_iterator      m_It;

            public: // for internal use only!!!
                iterator( intrusive_iterator const& it )
                    : m_It( it )
                {}

            public:
                iterator()
                    : m_It()
                {}

                iterator( iterator const& s)
                    : m_It( s.m_It )
                {}

                value_type * operator ->() const
                {
                    return &( m_It.operator->()->m_Value );
                }

                value_ref operator *() const
                {
                    return m_It.operator*().m_Value;
                }

                /// Pre-increment
                iterator& operator ++()
                {
                    ++m_It;
                    return *this;
                }

                iterator& operator = (iterator const& src)
                {
                    m_It = src.m_It;
                    return *this;
                }

                template <typename FwIt>
                bool operator ==(iterator<FwIt> const& i ) const
                {
                    return m_It == i.m_It;
                }
                template <typename FwIt>
                bool operator !=(iterator<FwIt> const& i ) const
                {
                    return !( *this == i );
                }
            };

        } // namespace details
        //@endcond

    } // namespace skip_list

    // Forward declaration
    template <class GC, typename T, typename Traits = skip_list::traits >
    class SkipListSet;

    // Forward declaration
    template <class GC, typename K, typename T, typename Traits = skip_list::traits >
    class SkipListMap;

}} // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_DETAILS_SKIP_LIST_BASE_H
