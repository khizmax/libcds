#ifndef CDSLIB_CONTAINER_DETAILS_LAZY_SKIP_LIST_BASE_H
#define CDSLIB_CONTAINER_DETAILS_LAZY_SKIP_LIST_BASE_H

#include <cds/intrusive/details/lazy_skip_list_base.h>
#include <cds/container/details/base.h>
#include <memory>

namespace cds { namespace container {

    namespace lazy_skip_list {
        template <typename Type>
        using random_level_generator = cds::intrusive::lazy_skip_list::random_level_generator<Type>;

        template <unsigned MaxHeight>
        using xor_shift = cds::intrusive::lazy_skip_list::xor_shift<MaxHeight >;

        typedef cds::intrusive::lazy_skip_list::xorshift32 xorshift32;

        typedef cds::intrusive::lazy_skip_list::xorshift24 xorshift24;

        typedef cds::intrusive::lazy_skip_list::xorshift16 xorshift16;

        using cds::intrusive::lazy_skip_list::xorshift;

        template <unsigned MaxHeight>
        using turbo = cds::intrusive::lazy_skip_list::turbo<MaxHeight >;

        typedef cds::intrusive::lazy_skip_list::turbo32 turbo32;

        typedef cds::intrusive::lazy_skip_list::turbo24 turbo24;

        typedef cds::intrusive::lazy_skip_list::turbo16 turbo16;

        using cds::intrusive::lazy_skip_list::turbo_pascal;

        template <typename EventCounter = cds::atomicity::event_counter>
        using stat = cds::intrusive::lazy_skip_list::stat < EventCounter >;

        typedef cds::intrusive::lazy_skip_list::empty_stat empty_stat;

        struct traits
        {
            typedef opt::none                       compare;
            typedef opt::none                       less;
            typedef atomicity::empty_item_counter     item_counter;
            typedef opt::v::relaxed_ordering        memory_model;
            typedef turbo32 random_level_generator;
            typedef CDS_DEFAULT_ALLOCATOR           allocator;
            typedef cds::backoff::Default           back_off;
            typedef empty_stat                      stat;
            typedef opt::none                       key_accessor;
        };

        template <typename... Options>
        struct make_traits {
#   ifdef CDS_DOXYGEN_INVOKED
            typedef implementation_defined type ;
#   else
            typedef typename cds::opt::make_options<
                typename cds::opt::find_type_traits< traits, Options... >::type
                ,Options...
            >::type   type;
#   endif
        };

        namespace details {

            template <typename Node, typename Traits>
            class node_allocator
            {
            protected:
                typedef Node node_type;
                typedef Traits traits;

                typedef typename node_type::tower_item_type node_tower_item;

                typedef typename std::allocator_traits<typename traits::allocator>::template rebind_alloc<unsigned char> tower_allocator_type;
                typedef typename std::allocator_traits<typename traits::allocator>::template rebind_alloc<node_type>     node_allocator_type;

                static size_t const c_nTowerItemSize = sizeof(node_tower_item);
                static size_t const c_nNodePadding = sizeof(node_type) % c_nTowerItemSize;
                static size_t const c_nNodeSize = sizeof(node_type) + (c_nNodePadding ? (c_nTowerItemSize - c_nNodePadding) : 0);

                static constexpr size_t node_size( unsigned int nHeight ) noexcept
                {
                    return c_nNodeSize + (nHeight - 1) * c_nTowerItemSize;
                }

                static unsigned char * alloc_space( unsigned int nHeight )
                {
                    unsigned char * pMem;
                    size_t const sz = node_size( nHeight );

                    if ( nHeight > 1 ) {
                        pMem = tower_allocator_type().allocate( sz );

                        assert( (((uintptr_t) pMem) & (alignof(node_type) - 1)) == 0 );
                        assert( (((uintptr_t) (pMem + c_nNodeSize)) & (alignof(node_tower_item) - 1)) == 0 );
                        return pMem;
                    }
                    else
                        pMem = reinterpret_cast<unsigned char *>( node_allocator_type().allocate( 1 ));

                    return pMem;
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
                    node_allocator_type a;
                    std::allocator_traits<node_allocator_type>::destroy( a, p );
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

    } // namespace lazy_skip_list

    template <class GC, typename T, typename Traits = lazy_skip_list::traits >
    class LazySkipListSet;

}} // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_DETAILS_LAZY_SKIP_LIST_BASE_H
