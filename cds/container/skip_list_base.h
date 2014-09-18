//$$CDS-header$$

#ifndef __CDS_CONTAINER_SKIP_LIST_BASE_H
#define __CDS_CONTAINER_SKIP_LIST_BASE_H

#include <cds/intrusive/skip_list_base.h>
#include <cds/container/base.h>

namespace cds { namespace container {

    /// SkipListSet related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace skip_list {

#ifdef CDS_DOXYGEN_INVOKED
        /// Typedef for intrusive::skip_list::random_level_generator template
        struct random_level_generator {};
#else
        using cds::intrusive::skip_list::random_level_generator;
#endif

#ifdef CDS_DOXYGEN_INVOKED
        /// Typedef for intrusive::skip_list::xorshift class
        class xorshift {};
#else
        using cds::intrusive::skip_list::xorshift;
#endif

#ifdef CDS_DOXYGEN_INVOKED
        /// Typedef for intrusive::skip_list::turbo_pascal class
        class turbo_pascal {};
#else
        using cds::intrusive::skip_list::turbo_pascal;
#endif

#ifdef CDS_DOXYGEN_INVOKED
        /// Typedef for intrusive::skip_list::stat class
        class stat {};
#else
        using cds::intrusive::skip_list::stat;
#endif

#ifdef CDS_DOXYGEN_INVOKED
        /// Typedef for intrusive::skip_list::empty_stat class
        class empty_stat {};
#else
        using cds::intrusive::skip_list::empty_stat;
#endif

        /// Type traits for SkipListSet class
        struct type_traits
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
                The type for item counting feature.
                Default is no item counter (\ref atomicity::empty_item_counter)
            */
            typedef atomicity::empty_item_counter     item_counter;

            /// C++ memory ordering model
            /**
                List of available memory ordering see opt::memory_model
            */
            typedef opt::v::relaxed_ordering        memory_model;

            /// Random level generator
            /**
                The random level generator is an important part of skip-list algorithm.
                The node height in the skip-list have a probabilistic distribution
                where half of the nodes that have level \p i also have level <tt>i+1</tt>
                (i = 0..30). The height of a node is in range [0..31].

                See skip_list::random_level_generator option setter.
            */
            typedef turbo_pascal                    random_level_generator;

            /// Allocator for skip-list nodes, \p std::allocator interface
            typedef CDS_DEFAULT_ALLOCATOR           allocator;

            /// back-off strategy used
            /**
                If the option is not specified, the cds::backoff::Default is used.
            */
            typedef cds::backoff::Default           back_off;

            /// Internal statistics
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
            This is a wrapper for <tt> cds::opt::make_options< type_traits, Options...> </tt>
            \p Options list see \ref SkipListSet.
        */
        template <CDS_DECL_OPTIONS10>
        struct make_traits {
#   ifdef CDS_DOXYGEN_INVOKED
            typedef implementation_defined type ;   ///< Metafunction result
#   else
            typedef typename cds::opt::make_options<
                typename cds::opt::find_type_traits< type_traits, CDS_OPTIONS10 >::type
                ,CDS_OPTIONS10
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
                typedef Traits type_traits;

                typedef typename node_type::tower_item_type node_tower_item;
                typedef typename type_traits::allocator::template rebind<unsigned char>::other  tower_allocator_type;
                typedef typename type_traits::allocator::template rebind<node_type>::other      node_allocator_type;

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
                    return new( pMem )
                        node_type( nHeight, nHeight > 1 ? reinterpret_cast<node_tower_item *>(pMem + c_nNodeSize) : nullptr, v );
                }

#       ifdef CDS_EMPLACE_SUPPORT
                template <typename... Args>
                node_type * New( unsigned int nHeight, Args&&... args )
                {
                    unsigned char * pMem = alloc_space( nHeight );
                    return new( pMem )
                        node_type( nHeight, nHeight > 1 ? reinterpret_cast<node_tower_item *>(pMem + c_nNodeSize) : nullptr,
                        std::forward<Args>(args)... );
                }
#       endif

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
                    void operator()( intrusive_node_type * pNode ) const {}
                };
            };

            template <typename ForwardIterator>
            class iterator
            {
                typedef ForwardIterator intrusive_iterator;
                typedef typename intrusive_iterator::value_type node_type;
                typedef typename node_type::stored_value_type   value_type;
                static bool const c_isConst = intrusive_iterator::c_isConst;

                typedef typename std::conditional< c_isConst, value_type const &, value_type &>::type   value_ref;

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
    template <class GC, typename T, typename Traits = skip_list::type_traits >
    class SkipListSet;

    // Forward declaration
    template <class GC, typename K, typename T, typename Traits = skip_list::type_traits >
    class SkipListMap;

}} // namespace cds::container

#endif // #ifndef __CDS_CONTAINER_SKIP_LIST_BASE_H
