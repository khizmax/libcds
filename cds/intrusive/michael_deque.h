//$$CDS-header$$

#ifndef __CDS_INTRUSIVE_MICHAEL_DEQUE_H
#define __CDS_INTRUSIVE_MICHAEL_DEQUE_H

#include <type_traits>
#include <cds/intrusive/michael_list_impl.h>
#include <cds/intrusive/michael_set.h>
#include <cds/intrusive/deque_stat.h>
#include <cds/ref.h>
#include <cds/details/aligned_type.h>
#include <cds/gc/default_gc.h>

namespace cds { namespace intrusive {

    //@cond
    struct michael_deque_tag;
    //@endcond

    /// MichaelDeque related definitions
    /** @ingroup cds_intrusive_helper
    */
    namespace michael_deque
    {
        /// Anchor contains left/right sibling items
        /**
            The anchor object is maintained by one CAS instruction.
        */
        struct anchor
        {
            unsigned int  idxLeft ;     ///< Left sibling index; the most-significant bit contains left-stable flag
            unsigned int  idxRight  ;   ///< Right sibling index; the most-significant bit contains right-stable flag

#       ifdef CDS_CXX11_EXPLICITLY_DEFAULTED_FUNCTION_SUPPORT
            //@cond
            anchor() CDS_NOEXCEPT_DEFAULTED = default;
            anchor( anchor const& ) CDS_NOEXCEPT_DEFAULTED = default;
            ~anchor() CDS_NOEXCEPT_DEFAULTED = default;
            anchor& operator=(anchor const&) CDS_NOEXCEPT_DEFAULTED = default;
#       if defined(CDS_MOVE_SEMANTICS_SUPPORT) && !defined(CDS_DISABLE_DEFAULT_MOVE_CTOR)
            anchor( anchor&&) CDS_NOEXCEPT_DEFAULTED = default;
            anchor& operator=(anchor&&) CDS_NOEXCEPT_DEFAULTED = default;
#       endif
            //@endcond
#       else
            /// Default ctor does not initialize left/right indices
            anchor() CDS_NOEXCEPT
                : idxLeft( 0 )
                , idxRight( 0 )
            {
                static_check();
            }

            anchor( anchor const& a) CDS_NOEXCEPT
                : idxLeft( a.idxLeft )
                , idxRight( a.idxRight )
            {
                static_check();
            }
#       endif

            /// Constructor sets \p left / \p right indices
            anchor( unsigned int left, unsigned int right ) CDS_NOEXCEPT
                : idxLeft( left )
                , idxRight( right )
            {
                static_check();
            }

            /// Anchor equal operator
            bool operator ==( anchor const& a) const CDS_NOEXCEPT
            {
                return idxLeft == a.idxLeft && idxRight == a.idxRight;
            }

            /// Anchor non-equal operator
            bool operator !=( anchor const& a) const CDS_NOEXCEPT
            {
                return !( *this == a );
            }

        private:
            //@cond
            static void static_check()
            {
                static_assert( sizeof(unsigned int) * 2 <= 8, "The index type must be no more than 32bit long" );
                static_assert( sizeof(anchor) <= 8, "The anchor type must be no more than 64bit long" );
            }
            //@endcond
        };

        /// Michael's deque node
        /**
            Template parameters:
            - GC - garbage collector
            - Tag - a tag used to distinguish between different implementation
        */
        template <class GC, typename Tag = opt::none>
        struct node: public michael_list::node< GC, michael_deque_tag >
        {
            typedef GC              gc  ;   ///< Garbage collector
            typedef Tag             tag ;   ///< tag

            //@cond
            typedef michael_list::node< gc, michael_deque_tag > mapper_node_type;
            //@endcond

            typedef typename gc::template atomic_type< anchor >   atomic_anchor  ;    ///< atomic reference to left/right node

            CDS_DATA_ALIGNMENT(8) atomic_anchor   m_Links ;   ///< Left/right sibling links
            unsigned int    m_nIndex;   ///< Item index

            //@cond
            node()
            {
                m_Links.store( anchor(0,0), CDS_ATOMIC::memory_order_release );
            }

            explicit node( anchor const& a )
                : m_Links()
                , m_nIndex(0)
            {
                m_Links.store( a, CDS_ATOMIC::memory_order_release );
            }
            //@endcond
        };

        //@cond
        struct default_hook {
            typedef cds::gc::default_gc gc;
            typedef opt::none           tag;
            typedef unsigned int        index_type;
        };
        //@endcond

        //@cond
        template < typename HookType, CDS_DECL_OPTIONS3>
        struct hook
        {
            typedef typename opt::make_options< default_hook, CDS_OPTIONS3>::type  options;
            typedef typename options::gc    gc;
            typedef typename options::tag   tag;
            typedef typename options::index_type index_type;

            typedef node<gc, tag>   node_type;
            typedef HookType        hook_type;
        };
        //@endcond


        /// Base hook
        /**
            \p Options are:
            - opt::gc - garbage collector used.
            - opt::tag - tag
            - opt::index_type - integral index type
        */
        template < CDS_DECL_OPTIONS3 >
        struct base_hook: public hook< opt::base_hook_tag, CDS_OPTIONS3 >
        {};

        /// Member hook
        /**
            \p MemberOffset defines offset in bytes of \ref node member into your structure.
            Use \p offsetof macro to define \p MemberOffset

            \p Options are:
            - opt::gc - garbage collector used.
            - opt::tag - tag
            - opt::index_type - integral index type
        */
        template < size_t MemberOffset, CDS_DECL_OPTIONS3 >
        struct member_hook: public hook< opt::member_hook_tag, CDS_OPTIONS3 >
        {
            //@cond
            static const size_t c_nMemberOffset = MemberOffset;
            //@endcond
        };

        /// Traits hook
        /**
            \p NodeTraits defines type traits for node.
            See \ref node_traits for \p NodeTraits interface description

            \p Options are:
            - opt::gc - garbage collector used.
            - opt::tag - tag
            - opt::index_type - integral index type
        */
        template <typename NodeTraits, CDS_DECL_OPTIONS3 >
        struct traits_hook: public hook< opt::traits_hook_tag, CDS_OPTIONS3 >
        {
            //@cond
            typedef NodeTraits node_traits;
            //@endcond
        };

        /// Deque internal statistics. May be used for debugging or profiling
        /**
            Template argument \p Counter defines type of counter.
            Default is cds::atomics::event_counter.
            You may use other counter type like as cds::atomics::item_counter,
            or even integral type, for example, \p int.

            The class extends intrusive::deque_stat interface for MichaelDeque.
        */
        template <typename Counter = cds::atomicity::event_counter >
        struct stat: public cds::intrusive::deque_stat<Counter>
        {
            //@cond
            typedef cds::intrusive::deque_stat<Counter> base_class;
            typedef typename base_class::counter_type   counter_type;
            //@endcond

            counter_type m_StabilizeFrontCount  ;  ///< stabilize left event count
            counter_type m_StabilizeBackCount   ;  ///< stabilize right event count

            /// Register "stabilize left" event
            void onStabilizeFront()          { ++m_StabilizeFrontCount; }

            /// Register "stabilize right" event
            void onStabilizeBack()          { ++m_StabilizeBackCount; }
        };

        /// Dummy deque statistics - no counting is performed. Support interface like \ref michael_deque::stat
        struct dummy_stat: public cds::intrusive::deque_dummy_stat
        {
            //@cond
            void onStabilizeFront() {}
            void onStabilizeBack()  {}
            //@endcond
        };

        //@cond
        template < typename NodeType, opt::link_check_type LinkType>
        struct link_checker
        {
            typedef NodeType node_type;

            static void is_empty( const node_type * pNode )
            {
#           ifdef _DEBUG
                anchor a = pNode->m_Links.load(CDS_ATOMIC::memory_order_relaxed);
                assert( a.idxLeft == 0 && a.idxRight == 0 );
#           endif
            }
        };

        template < typename NodeType>
        struct link_checker<NodeType, opt::never_check_link>
        {
            typedef NodeType node_type;

            static void is_empty( const node_type * /*pNode*/ )
            {}
        };
        //@endcond
    }   // namespace michael_deque

    /// Michael's intrusive deque
    /** @ingroup cds_intrusive_deque
        Implementation of Michael's deque algorithm.

        \par Source:
            [2003] Maged Michael "CAS-based Lock-free Algorithm for Shared Deque"

        <b>Short description</b> (from Michael's paper)

        The deque is represented as a doubly-linked list. Each node in the list contains two link pointers,
        \p pRight and \p pLeft, and a data field. A shared variable, \p Anchor, holds the two anchor
        pointers to the leftmost and rightmost nodes in the list, if any, and a three-value
        status tag. Anchor must fit in a memory block that can be read and manipulated
        using CAS or LL/SC, atomically. Initially both anchor pointers have null values
        and the status tag holds the value stable, indicating an empty deque.

        The status tag serves to indicate if the deque is in an unstable state. When
        a process finds the deque in an unstable state, it must first attempt to take it
        to a stable state before attempting its own operation.

        The algorithm can use single-word CAS or LL/SC.
        In \p libcds implementation of the algorithm the node contains two
        31bit link indices instead of pointers + one bit for status tag;
        this trick allows use 64bit CAS to manipulate \p Anchor. Internal mapper
        (based on MichaelHashSet intrusive container)
        reflects link indices to item pointers. The maximum number of item in
        the deque is limited by 2**31 that is practically unbounded.

        Template arguments:
        - \p GC - garbage collector type: gc::HP or gc::PTB. Note that gc::HRC is not supported
        - \p T - type to be stored in the queue, should be convertible to michael_deque::node
        - \p Options - options

        Type of node: \ref michael_deque::node

        \p Options are:
        - opt::hook - hook used. Possible values are: michael_deque::base_hook, michael_deque::member_hook, michael_deque::traits_hook.
            If the option is not specified, <tt>michael_deque::base_hook<></tt> is used.
        - opt::back_off - back-off strategy used. If the option is not specified, the cds::backoff::empty is used.
        - opt::disposer - the functor used for dispose removed items. Default is opt::v::empty_disposer. This option is used
            in \ref pop_front and \ref pop_back functions.
        - opt::link_checker - the type of node's link fields checking. Default is \ref opt::debug_check_link
        - opt::item_counter - the type of item counting feature. Default is \ref atomicity::empty_item_counter (no item counting feature)
        - opt::stat - the type to gather internal statistics.
            Possible option value are: \ref michael_deque::stat, \ref michael_deque::dummy_stat, user-provided class that supports michael_deque::stat interface.
            Default is \ref michael_deque::dummy_stat.
        - opt::alignment - the alignment for internal deque data. Default is opt::cache_line_alignment
        - opt::memory_model - C++ memory ordering model. Can be opt::v::relaxed_ordering (relaxed memory model, the default)
            or opt::v::sequential_consistent (sequentially consisnent memory model).
        - opt::allocator - allocator using for internal memory mapper based on MichaelHashSet. Default is CDS_DEFAULT_ALLOCATOR.
    */
    template <typename GC, typename T, CDS_DECL_OPTIONS10>
    class MichaelDeque
    {
        //@cond
        struct default_options
        {
            typedef cds::backoff::empty             back_off;
            typedef michael_deque::base_hook<>      hook;
            typedef opt::v::empty_disposer          disposer;
            typedef atomicity::empty_item_counter   item_counter;
            typedef michael_deque::dummy_stat       stat;
            typedef opt::v::relaxed_ordering        memory_model;
            static const opt::link_check_type link_checker = opt::debug_check_link;
            enum { alignment = opt::cache_line_alignment };
            typedef CDS_DEFAULT_ALLOCATOR           allocator;
        };
        //@endcond

    public:
        //@cond
        typedef typename opt::make_options<
            typename cds::opt::find_type_traits< default_options, CDS_OPTIONS10 >::type
            ,CDS_OPTIONS10
        >::type   options;
        //@endcond

    private:
        //@cond
        typedef typename std::conditional<
            std::is_same<typename options::stat, cds::intrusive::deque_stat<> >::value
            ,michael_deque::stat<>
            ,typename std::conditional<
                std::is_same<typename options::stat, cds::intrusive::deque_dummy_stat>::value
                ,michael_deque::dummy_stat
                ,typename options::stat
            >::type
        >::type stat_type_;
        //@endcond


    public:
        typedef T  value_type   ;   ///< type of value stored in the deque
        typedef typename options::hook      hook        ;   ///< hook type
        typedef typename hook::node_type    node_type   ;   ///< node type
        typedef typename options::disposer  disposer    ;   ///< disposer used
        typedef typename get_node_traits< value_type, node_type, hook>::type node_traits ;    ///< node traits
        typedef michael_deque::link_checker< node_type, options::link_checker > link_checker   ;   ///< link checker

        typedef GC gc                                       ;   ///< Garbage collector
        typedef typename options::back_off  back_off        ;   ///< back-off strategy
        typedef typename options::item_counter item_counter ;   ///< Item counting policy used
        typedef stat_type_   stat                           ;   ///< Internal statistics policy used
        typedef typename options::memory_model  memory_model;   ///< Memory ordering. See cds::opt::memory_model option
        typedef typename options::allocator allocator_type  ;   ///< Allocator using for internal memory mapping

        typedef typename node_type::atomic_anchor   atomic_anchor   ;   ///< Atomic anchor

    protected:
        //@cond
        class index_mapper
        {
            struct node_less_comparator
            {
                bool operator ()( value_type const & n1, value_type const& n2) const
                {
                    return node_traits::to_node_ptr(n1)->m_nIndex < node_traits::to_node_ptr(n2)->m_nIndex;
                }
                bool operator ()( unsigned int i, value_type const& n2) const
                {
                    return i < node_traits::to_node_ptr(n2)->m_nIndex;
                }
                bool operator ()( value_type const & n1, unsigned int i) const
                {
                    return node_traits::to_node_ptr(n1)->m_nIndex < i;
                }
            };

            struct internal_disposer
            {
                void operator()( value_type * p )
                {
                    assert( p != nullptr );

                    MichaelDeque::clear_links( node_traits::to_node_ptr(p) );
                    disposer()( p );
                }
            };

            struct mapper_node_traits
            {
                typedef typename node_type::mapper_node_type mapper_node_type;

                static mapper_node_type * to_node_ptr( value_type& v )
                {
                    return static_cast<mapper_node_type *>( node_traits::to_node_ptr(v) );
                }

                static mapper_node_type * to_node_ptr( value_type * v )
                {
                    return static_cast<mapper_node_type *>( node_traits::to_node_ptr(v) );
                }

                static mapper_node_type const * to_node_ptr( value_type const& v )
                {
                    return static_cast<mapper_node_type const *>( node_traits::to_node_ptr(v) );
                }

                static mapper_node_type const * to_node_ptr( value_type const * v )
                {
                    return static_cast<mapper_node_type const *>( node_traits::to_node_ptr(v) );
                }

                static value_type * to_value_ptr( mapper_node_type&  n )
                {
                    return node_traits::to_value_ptr( static_cast<node_type&>(n));
                }

                static value_type * to_value_ptr( mapper_node_type *  n )
                {
                    return node_traits::to_value_ptr( static_cast<node_type *>(n));
                }

                static const value_type * to_value_ptr( mapper_node_type const& n )
                {
                    return node_traits::to_value_ptr( static_cast<node_type const&>(n));
                }

                static const value_type * to_value_ptr( mapper_node_type const * n )
                {
                    return node_traits::to_value_ptr( static_cast<node_type const *>(n));
                }
            };

            typedef MichaelList< gc, value_type,
                typename michael_list::make_traits<
                    opt::hook< michael_list::traits_hook<
                        mapper_node_traits
                        ,cds::opt::gc< gc >
                        ,cds::opt::tag<michael_deque_tag> >
                    >
                    ,opt::less< node_less_comparator >
                    ,opt::back_off< back_off >
                    ,opt::disposer< internal_disposer >
                    ,opt::memory_model< memory_model >
                >::type
            > mapper_ordered_list;

            struct mapper_hash {
                size_t operator()( value_type const& v ) const
                {
                    return cds::opt::v::hash<unsigned int>()( node_traits::to_node_ptr(v)->m_nIndex );
                }
                size_t operator()( unsigned int i ) const
                {
                    return cds::opt::v::hash<unsigned int>()(i);
                }
            };

            typedef MichaelHashSet< gc, mapper_ordered_list,
                typename michael_set::make_traits<
                    opt::hash< mapper_hash >
                    ,opt::allocator< allocator_type >
                >::type
            >   mapper_type;

#       if !(defined(CDS_CXX11_LAMBDA_SUPPORT) && !((CDS_COMPILER == CDS_COMPILER_MSVC || CDS_COMPILER == CDS_COMPILER_INTEL) && _MSC_VER < 1700))
            struct at_functor {
                node_type * pNode;

                at_functor()
                    : pNode( nullptr )
                    {}

                void operator()( value_type& v, unsigned int nIdx )
                {
                    pNode = node_traits::to_node_ptr(v);
                    assert( pNode->m_nIndex == nIdx );
                }
            };
#       endif

            mapper_type     m_set;
            CDS_ATOMIC::atomic<unsigned int>    m_nLastIndex;

        public:

            index_mapper( size_t nEstimatedItemCount, size_t nLoadFactor )
                : m_set( nEstimatedItemCount, nLoadFactor )
                , m_nLastIndex(1)
                {}

            unsigned int map( value_type& v )
            {
                while ( true ) {
                    node_type * pNode = node_traits::to_node_ptr( v );
                    pNode->m_nIndex = m_nLastIndex.fetch_add( 1, memory_model::memory_order_relaxed );
                    if ( pNode->m_nIndex && m_set.insert( v ))
                        return pNode->m_nIndex;
                }
            }

            bool unmap( unsigned int nIdx )
            {
                return m_set.erase( nIdx );
            }

            node_type * at( unsigned int nIdx )
            {
#   if defined(CDS_CXX11_LAMBDA_SUPPORT) && !((CDS_COMPILER == CDS_COMPILER_MSVC ||CDS_COMPILER == CDS_COMPILER_INTEL) && _MSC_VER < 1700)
                // MS VC++2010 bug: error C2955: 'cds::intrusive::node_traits' : use of class template requires template argument list
                // see declaration of 'cds::intrusive::node_traits'
                node_type * pNode = nullptr;
                if ( m_set.find( nIdx,
                    [&pNode](value_type& v, unsigned int nIdx) {
                        pNode = node_traits::to_node_ptr(v);
                        assert( pNode->m_nIndex == nIdx );
                    })
                )
                    return pNode;
#   else
                at_functor f;
                if ( m_set.find( nIdx, cds::ref(f) ))
                    return f.pNode;
#   endif
                return nullptr;
            }
        };
        //@endcond
    public:

        /// Rebind template arguments
        template <typename GC2, typename T2, CDS_DECL_OTHER_OPTIONS10>
        struct rebind {
            typedef MichaelDeque< GC2, T2, CDS_OTHER_OPTIONS10> other   ;   ///< Rebinding result
        };

    protected:
        typename cds::opt::details::alignment_setter< atomic_anchor, options::alignment >::type  m_Anchor ;   ///< Left/right heads
        typename cds::opt::details::alignment_setter< index_mapper, options::alignment >::type   m_Mapper ;   ///< Memory mapper

        item_counter            m_ItemCounter   ;   ///< item counter
        stat                    m_Stat  ;   ///< Internal statistics

        //@cond
        static const unsigned int c_nIndexMask    = ((unsigned int)(0 - 1)) >> 1;
        static const unsigned int c_nFlagMask     = ((unsigned int)(1)) << (sizeof(unsigned int) * 8 - 1);
        static const unsigned int c_nEmptyIndex   = 0;
        //@endcond

    private:
        //@cond
        typedef michael_deque::anchor CDS_TYPE_ALIGNMENT(8) anchor_type;
        typedef intrusive::node_to_value<MichaelDeque> node_to_value;

        static void clear_links( node_type * pNode )
        {
            pNode->m_Links.store( anchor_type(), memory_model::memory_order_release );
        }

        enum anchor_status {
            Stable,
            RPush,
            LPush
        };

        static anchor_status status( anchor_type const& a )
        {
            if ( a.idxLeft & c_nFlagMask )
                return LPush;
            if ( a.idxRight & c_nFlagMask )
                return RPush;
            return Stable;
        }

        static unsigned int index( unsigned int i )
        {
            return i & c_nIndexMask;
        }

        void stabilize( anchor_type& a )
        {
            switch ( status(a)) {
                case LPush:
                    stabilize_front(a);
                    break;
                case RPush:
                    stabilize_back(a);
                    break;
                default:
                    break;
            }
        }

        void stabilize_front( anchor_type& a )
        {
            m_Stat.onStabilizeFront();

            typename gc::template GuardArray<3>  guards;
            node_type * pLeft;
            node_type * pRight;
            unsigned int const idxLeft  = index( a.idxLeft );
            unsigned int const idxRight = index( a.idxRight );

            guards.assign( 0, node_traits::to_value_ptr( pLeft = m_Mapper.at( idxLeft )) );
            guards.assign( 1, node_traits::to_value_ptr( pRight = m_Mapper.at( idxRight )) );
            if ( m_Anchor.load( memory_model::memory_order_acquire ) != a )
                return;

            unsigned int idxPrev = index( pLeft->m_Links.load(memory_model::memory_order_relaxed ).idxRight );
            node_type * pPrev;
            guards.assign( 2, node_traits::to_value_ptr( pPrev = m_Mapper.at( idxPrev )) );
            if ( m_Anchor.load( memory_model::memory_order_acquire ) != a )
                return;

            anchor_type prevLinks( pPrev->m_Links.load( memory_model::memory_order_acquire ));
            if ( index( prevLinks.idxLeft ) != idxLeft ) {
                if ( m_Anchor.load( memory_model::memory_order_acquire ) != a )
                    return;

                if ( !pPrev->m_Links.compare_exchange_strong( prevLinks, anchor_type( idxLeft, prevLinks.idxRight ), memory_model::memory_order_release, memory_model::memory_order_relaxed ))
                    return;
            }

            // clear RPush/LPush flags
            m_Anchor.compare_exchange_weak( a, anchor_type(idxLeft, idxRight), memory_model::memory_order_release, memory_model::memory_order_relaxed );
        }

        void stabilize_back( anchor_type& a )
        {
            m_Stat.onStabilizeBack();

            typename gc::template GuardArray<3>  guards;
            node_type * pLeft;
            node_type * pRight;
            unsigned int const idxLeft  = index( a.idxLeft );
            unsigned int const idxRight = index( a.idxRight );

            guards.assign( 0, node_traits::to_value_ptr( pLeft = m_Mapper.at( idxLeft )) );
            guards.assign( 1, node_traits::to_value_ptr( pRight = m_Mapper.at( idxRight )) );
            if ( m_Anchor.load( memory_model::memory_order_acquire ) != a )
                return;

            unsigned int idxPrev = index( pRight->m_Links.load(memory_model::memory_order_relaxed ).idxLeft );
            node_type * pPrev;
            guards.assign( 2, node_traits::to_value_ptr( pPrev = m_Mapper.at( idxPrev )) );
            if ( m_Anchor.load( memory_model::memory_order_acquire ) != a )
                return;

            anchor_type prevLinks( pPrev->m_Links.load( memory_model::memory_order_acquire ));
            if ( index( prevLinks.idxRight ) != idxRight ) {
                if ( m_Anchor.load( memory_model::memory_order_acquire ) != a )
                    return;

                if ( !pPrev->m_Links.compare_exchange_strong( prevLinks, anchor_type( prevLinks.idxLeft, idxRight ), memory_model::memory_order_release, memory_model::memory_order_relaxed ))
                    return;
            }

            // clear RPush/LPush flags
            m_Anchor.compare_exchange_weak( a, anchor_type(idxLeft, idxRight), memory_model::memory_order_release, memory_model::memory_order_relaxed );
        }

        //@endcond

    protected:
        //@cond
        struct pop_result {
            value_type *                        pPopped;
            unsigned int                        nIdxPopped;
            typename gc::template GuardArray<2> guards;
        };

        void dispose_result( pop_result& res )
        {
            m_Mapper.unmap( res.nIdxPopped );
        }

        bool do_pop_back( pop_result& res )
        {
            back_off bkoff;
            anchor_type a;

            while ( true ) {
                a = m_Anchor.load( memory_model::memory_order_acquire );

                if ( a.idxRight == c_nEmptyIndex ) {
                    m_Stat.onPopEmpty();
                    return false;
                }

                if ( a.idxLeft == a.idxRight ) {
                    if ( m_Anchor.compare_exchange_weak( a, anchor_type( c_nEmptyIndex, c_nEmptyIndex ), memory_model::memory_order_release, memory_model::memory_order_relaxed ))
                        break;
                    bkoff();
                }
                else if ( status( a ) == Stable ) {
                    unsigned int idxLeft  = index( a.idxLeft );
                    unsigned int idxRight = index( a.idxRight );
                    node_type * pLeft;
                    res.guards.assign( 0, node_traits::to_value_ptr( pLeft = m_Mapper.at( idxLeft )) );
                    node_type * pRight;
                    res.guards.assign( 1, node_traits::to_value_ptr( pRight = m_Mapper.at( idxRight )) );

                    if ( m_Anchor.load( memory_model::memory_order_acquire ) != a ) {
                        m_Stat.onPopBackContention();
                        continue;
                    }

                    unsigned int nPrev = pRight->m_Links.load( memory_model::memory_order_acquire ).idxLeft;
                    if ( m_Anchor.compare_exchange_weak( a, anchor_type( a.idxLeft, nPrev ), memory_model::memory_order_release, memory_model::memory_order_relaxed ) )
                        break;
                    bkoff();
                    m_Stat.onPopBackContention();
                }
                else
                    stabilize( a );
            }

            res.nIdxPopped = a.idxRight;
            res.pPopped = node_traits::to_value_ptr( m_Mapper.at( a.idxRight ));

            --m_ItemCounter;
            m_Stat.onPopBack();

            return true;
        }

        bool do_pop_front( pop_result& res )
        {
            back_off bkoff;
            anchor_type a;

            while ( true ) {
                a = m_Anchor.load( memory_model::memory_order_acquire );

                if ( a.idxLeft == c_nEmptyIndex ) {
                    m_Stat.onPopEmpty();
                    return false;
                }

                if ( a.idxLeft == a.idxRight ) {
                    if ( m_Anchor.compare_exchange_weak( a, anchor_type( c_nEmptyIndex, c_nEmptyIndex ), memory_model::memory_order_release, memory_model::memory_order_relaxed ))
                        break;
                    bkoff();
                }
                else if ( status( a ) == Stable ) {
                    unsigned int idxLeft  = index( a.idxLeft );
                    unsigned int idxRight = index( a.idxRight );
                    node_type * pLeft;
                    res.guards.assign( 0, node_traits::to_value_ptr( pLeft = m_Mapper.at( idxLeft )) );
                    node_type * pRight;
                    res.guards.assign( 1, node_traits::to_value_ptr( pRight = m_Mapper.at( idxRight )) );

                    if ( m_Anchor.load( memory_model::memory_order_acquire ) != a ) {
                        m_Stat.onPopFrontContention();
                        continue;
                    }

                    unsigned int nPrev = pLeft->m_Links.load( memory_model::memory_order_acquire ).idxRight;
                    if ( m_Anchor.compare_exchange_weak( a, anchor_type( nPrev, a.idxRight ), memory_model::memory_order_release, memory_model::memory_order_relaxed ) )
                        break;
                    bkoff();
                    m_Stat.onPopFrontContention();
                }
                else
                    stabilize( a );
            }

            res.nIdxPopped = a.idxLeft;
            res.pPopped = node_traits::to_value_ptr( m_Mapper.at( a.idxLeft ));

            --m_ItemCounter;
            m_Stat.onPopFront();

            return true;
        }

        //@endcond

    public:
        /// Default constructor
        /**
            Initializes the deque object with up to <tt>2**16 - 2</tt> items
        */
        MichaelDeque()
            :m_Anchor()
            ,m_Mapper( 4096, 4 )
        {
            m_Anchor.store( anchor_type( c_nEmptyIndex, c_nEmptyIndex ), CDS_ATOMIC::memory_order_release );

            // GC and node_type::gc must be the same
            static_assert(( std::is_same<gc, typename node_type::gc>::value ), "GC and node_type::gc must be the same");

            // cds::gc::HRC is not allowed
            static_assert(( !std::is_same<gc, cds::gc::HRC>::value ), "cds::gc::HRC is not allowed here");
        }

        /// Constructor
        /**
            Initializes the deque object with estimated item count \p nMaxItemCount.
            \p nLoadFactor is a parameter of internal memory mapper based on MichaelHashSet;
            see MichaelHashSet ctor for details
        */
        MichaelDeque( unsigned int nMaxItemCount, unsigned int nLoadFactor = 4 )
            :m_Anchor()
            ,m_Mapper( nMaxItemCount, nLoadFactor )
        {
            m_Anchor.store( anchor_type( c_nEmptyIndex, c_nEmptyIndex ), CDS_ATOMIC::memory_order_release );

            // GC and node_type::gc must be the same
            static_assert(( std::is_same<gc, typename node_type::gc>::value ), "GC and node_type::gc must be the same");

            // cds::gc::HRC is not allowed
            static_assert(( !std::is_same<gc, cds::gc::HRC>::value ), "cds::gc::HRC is not allowed here");
        }

        /// Destructor clears the deque
        ~MichaelDeque()
        {
            clear();
        }

    public:
        /// Push back (right) side
        /**
            Push new item \p val to right side of the deque.
        */
        bool push_back( value_type& val )
        {
            back_off bkoff;

            node_type * pNode = node_traits::to_node_ptr( val );
            link_checker::is_empty( pNode );

            unsigned int nIdx = m_Mapper.map( val );
            if ( nIdx == c_nEmptyIndex )
                return false;

            while ( true ) {
                anchor_type a = m_Anchor.load( memory_model::memory_order_acquire );
                if ( a.idxRight == c_nEmptyIndex ) {
                    if ( m_Anchor.compare_exchange_weak( a, anchor_type( nIdx, nIdx ), memory_model::memory_order_release, memory_model::memory_order_relaxed ))
                        break;
                    bkoff();
                    m_Stat.onPushBackContention();
                }
                else if ( status(a) == Stable ) {
                    pNode->m_Links.store( anchor_type( a.idxRight, c_nEmptyIndex ), memory_model::memory_order_release );
                    anchor_type aNew( a.idxLeft, nIdx | c_nFlagMask );
                    if ( m_Anchor.compare_exchange_weak( a, aNew, memory_model::memory_order_release, memory_model::memory_order_relaxed) ) {
                        stabilize_back( aNew );
                        break;
                    }
                    bkoff();
                    m_Stat.onPushBackContention();
                }
                else
                    stabilize( a );
            }

            ++m_ItemCounter;
            m_Stat.onPushBack();
            return true;
        }

        /// Push front (left) side
        /**
            Push new item \p val to left side of the deque.
        */
        bool push_front( value_type& val )
        {
            back_off bkoff;
            node_type * pNode = node_traits::to_node_ptr( val );
            link_checker::is_empty( pNode );

            unsigned int nIdx = m_Mapper.map( val );
            if ( nIdx == c_nEmptyIndex )
                return false;

            while ( true ) {
                anchor_type a = m_Anchor.load( memory_model::memory_order_acquire );
                if ( a.idxLeft == c_nEmptyIndex ) {
                    if ( m_Anchor.compare_exchange_weak( a, anchor_type( nIdx, nIdx ), memory_model::memory_order_release, memory_model::memory_order_relaxed ))
                        break;
                    bkoff();
                    m_Stat.onPushFrontContention();
                }
                else if ( status(a) == Stable ) {
                    pNode->m_Links.store( anchor_type( c_nEmptyIndex, a.idxLeft ), memory_model::memory_order_release );
                    anchor_type aNew( nIdx | c_nFlagMask, a.idxRight );
                    if ( m_Anchor.compare_exchange_weak( a, aNew, memory_model::memory_order_release, memory_model::memory_order_relaxed )) {
                        stabilize_front( aNew );
                        break;
                    }
                    bkoff();
                    m_Stat.onPushFrontContention();
                }
                else
                    stabilize( a );
            }

            ++m_ItemCounter;
            m_Stat.onPushFront();
            return true;
        }

        /// Pop back
        /**
            Pops rightmost item from the deque. If the deque is empty then returns \p NULL.

            For popped object the disposer specified in \p Options template parameters is called.
        */
        value_type * pop_back()
        {
            pop_result res;
            if ( do_pop_back( res )) {
                dispose_result( res );
                return res.pPopped;
            }

            return nullptr;
        }

        /// Pop front
        /**
            Pops leftmost item from the deque. If the deque is empty then returns \p NULL.

            For popped object the disposer specified in \p Options template parameters is called.
        */
        value_type * pop_front()
        {
            pop_result res;
            if ( do_pop_front( res )) {
                dispose_result( res );
                return res.pPopped;
            }

            return nullptr;
        }

        /// Returns deque's item count
        /**
            The value returned depends on opt::item_counter option. For atomicity::empty_item_counter,
            this function always returns 0.

            <b>Warning</b>: even if you use real item counter and it returns 0, this fact does not mean that the deque
            is empty. To check deque emptyness use \ref empty() method.
        */
        size_t size() const
        {
            return m_ItemCounter.value();
        }

        /// Checks if the dequeue is empty
        bool empty() const
        {
            anchor_type a = m_Anchor.load( memory_model::memory_order_relaxed );
            return a.idxLeft == c_nEmptyIndex && a.idxRight == c_nEmptyIndex;
        }

        /// Clear the deque
        /**
            The function repeatedly calls \ref pop_back until it returns \p NULL.
            The disposer defined in template \p Options is called for each item
            that can be safely disposed.
        */
        void clear()
        {
            while ( pop_back() != nullptr );
        }

        /// Returns reference to internal statistics
        const stat& statistics() const
        {
            return m_Stat;
        }
    };


}}  // namespace cds::intrusive


#endif // #ifndef __CDS_INTRUSIVE_MICHAEL_DEQUE_H
