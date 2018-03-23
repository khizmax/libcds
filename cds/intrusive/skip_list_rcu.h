// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_SKIP_LIST_RCU_H
#define CDSLIB_INTRUSIVE_SKIP_LIST_RCU_H

#include <type_traits>
#include <memory>
#include <cds/intrusive/details/skip_list_base.h>
#include <cds/opt/compare.h>
#include <cds/urcu/details/check_deadlock.h>
#include <cds/details/binary_functor_wrapper.h>
#include <cds/urcu/exempt_ptr.h>
#include <cds/urcu/raw_ptr.h>
#include <cds/intrusive/details/raw_ptr_disposer.h>

namespace cds { namespace intrusive {

    //@cond
    namespace skip_list {

        template <class RCU, typename Tag>
        class node< cds::urcu::gc< RCU >, Tag >
        {
        public:
            typedef cds::urcu::gc< RCU >    gc; ///< Garbage collector
            typedef Tag     tag               ; ///< tag

            // Mark bits:
            //  bit 0 - the item is logically deleted
            //  bit 1 - the item is extracted (only for level 0)
            typedef cds::details::marked_ptr<node, 3> marked_ptr;        ///< marked pointer
            typedef atomics::atomic< marked_ptr >     atomic_marked_ptr; ///< atomic marked pointer
            typedef atomic_marked_ptr                 tower_item_type;

        protected:
            atomic_marked_ptr       m_pNext;     ///< Next item in bottom-list (list at level 0)
        public:
            node *                  m_pDelChain; ///< Deleted node chain (local for a thread)
        protected:
            unsigned int            m_nHeight;   ///< Node height (size of m_arrNext array). For node at level 0 the height is 1.
            atomic_marked_ptr *     m_arrNext;   ///< Array of next items for levels 1 .. m_nHeight - 1. For node at level 0 \p m_arrNext is \p nullptr
            atomics::atomic<unsigned> m_nUnlink; ///< Unlink helper

        public:
            /// Constructs a node of height 1 (a bottom-list node)
            node()
                : m_pNext( nullptr )
                , m_pDelChain( nullptr )
                , m_nHeight(1)
                , m_arrNext( nullptr )
            {
                m_nUnlink.store( 1, atomics::memory_order_release );
            }

            /// Constructs a node of height \p nHeight
            void make_tower( unsigned int nHeight, atomic_marked_ptr * nextTower )
            {
                assert( nHeight > 0 );
                assert( (nHeight == 1 && nextTower == nullptr)  // bottom-list node
                        || (nHeight > 1 && nextTower != nullptr)   // node at level of more than 0
                    );

                m_arrNext = nextTower;
                m_nHeight = nHeight;
                m_nUnlink.store( nHeight, atomics::memory_order_release );
            }

            atomic_marked_ptr * release_tower()
            {
                atomic_marked_ptr * pTower = m_arrNext;
                m_arrNext = nullptr;
                m_nHeight = 1;
                return pTower;
            }

            atomic_marked_ptr * get_tower() const
            {
                return m_arrNext;
            }

            void clear_tower()
            {
                for ( unsigned int nLevel = 1; nLevel < m_nHeight; ++nLevel )
                    next(nLevel).store( marked_ptr(), atomics::memory_order_relaxed );
            }

            /// Access to element of next pointer array
            atomic_marked_ptr& next( unsigned int nLevel )
            {
                assert( nLevel < height());
                assert( nLevel == 0 || (nLevel > 0 && m_arrNext != nullptr));

                return nLevel ? m_arrNext[ nLevel - 1] : m_pNext;
            }

            /// Access to element of next pointer array (const version)
            atomic_marked_ptr const& next( unsigned int nLevel ) const
            {
                assert( nLevel < height());
                assert( nLevel == 0 || nLevel > 0 && m_arrNext != nullptr );

                return nLevel ? m_arrNext[ nLevel - 1] : m_pNext;
            }

            /// Access to element of next pointer array (same as \ref next function)
            atomic_marked_ptr& operator[]( unsigned int nLevel )
            {
                return next( nLevel );
            }

            /// Access to element of next pointer array (same as \ref next function)
            atomic_marked_ptr const& operator[]( unsigned int nLevel ) const
            {
                return next( nLevel );
            }

            /// Height of the node
            unsigned int height() const
            {
                return m_nHeight;
            }

            /// Clears internal links
            void clear()
            {
                assert( m_arrNext == nullptr );
                m_pNext.store( marked_ptr(), atomics::memory_order_release );
                m_pDelChain = nullptr;
            }

            bool is_cleared() const
            {
                return m_pNext == atomic_marked_ptr()
                    && m_arrNext == nullptr
                    && m_nHeight <= 1;
            }

            bool level_unlinked( unsigned nCount = 1 )
            {
                return m_nUnlink.fetch_sub( nCount, std::memory_order_relaxed ) == 1;
            }

            bool is_upper_level( unsigned nLevel ) const
            {
                return m_nUnlink.load( atomics::memory_order_relaxed ) == nLevel + 1;
            }
        };
    } // namespace skip_list
    //@endcond

    //@cond
    namespace skip_list { namespace details {

        template <class RCU, typename NodeTraits, typename BackOff, bool IsConst>
        class iterator< cds::urcu::gc< RCU >, NodeTraits, BackOff, IsConst >
        {
        public:
            typedef cds::urcu::gc< RCU >                gc;
            typedef NodeTraits                          node_traits;
            typedef BackOff                             back_off;
            typedef typename node_traits::node_type     node_type;
            typedef typename node_traits::value_type    value_type;
            static bool const c_isConst = IsConst;

            typedef typename std::conditional< c_isConst, value_type const &, value_type &>::type   value_ref;

        protected:
            typedef typename node_type::marked_ptr          marked_ptr;
            typedef typename node_type::atomic_marked_ptr   atomic_marked_ptr;

            node_type *             m_pNode;

        protected:
            void next()
            {
                back_off bkoff;

                for (;;) {
                    if ( m_pNode->next( m_pNode->height() - 1 ).load( atomics::memory_order_acquire ).bits()) {
                        // Current node is marked as deleted. So, its next pointer can point to anything
                        // In this case we interrupt our iteration and returns end() iterator.
                        *this = iterator();
                        return;
                    }

                    marked_ptr p = m_pNode->next(0).load( atomics::memory_order_relaxed );
                    node_type * pp = p.ptr();
                    if ( p.bits()) {
                        // p is marked as deleted. Spin waiting for physical removal
                        bkoff();
                        continue;
                    }
                    else if ( pp && pp->next( pp->height() - 1 ).load( atomics::memory_order_relaxed ).bits()) {
                        // p is marked as deleted. Spin waiting for physical removal
                        bkoff();
                        continue;
                    }

                    m_pNode = pp;
                    break;
                }
            }

        public: // for internal use only!!!
            iterator( node_type& refHead )
                : m_pNode( nullptr )
            {
                back_off bkoff;

                for (;;) {
                    marked_ptr p = refHead.next(0).load( atomics::memory_order_relaxed );
                    if ( !p.ptr()) {
                        // empty skip-list
                        break;
                    }

                    node_type * pp = p.ptr();
                    // Logically deleted node is marked from highest level
                    if ( !pp->next( pp->height() - 1 ).load( atomics::memory_order_acquire ).bits()) {
                        m_pNode = pp;
                        break;
                    }

                    bkoff();
                }
            }

        public:
            iterator()
                : m_pNode( nullptr )
            {}

            iterator( iterator const& s)
                : m_pNode( s.m_pNode )
            {}

            value_type * operator ->() const
            {
                assert( m_pNode != nullptr );
                assert( node_traits::to_value_ptr( m_pNode ) != nullptr );

                return node_traits::to_value_ptr( m_pNode );
            }

            value_ref operator *() const
            {
                assert( m_pNode != nullptr );
                assert( node_traits::to_value_ptr( m_pNode ) != nullptr );

                return *node_traits::to_value_ptr( m_pNode );
            }

            /// Pre-increment
            iterator& operator ++()
            {
                next();
                return *this;
            }

            iterator& operator = (const iterator& src)
            {
                m_pNode = src.m_pNode;
                return *this;
            }

            template <typename Bkoff, bool C>
            bool operator ==(iterator<gc, node_traits, Bkoff, C> const& i ) const
            {
                return m_pNode == i.m_pNode;
            }
            template <typename Bkoff, bool C>
            bool operator !=(iterator<gc, node_traits, Bkoff, C> const& i ) const
            {
                return !( *this == i );
            }
        };
    }}  // namespace skip_list::details
    //@endcond

    /// Lock-free skip-list set (template specialization for \ref cds_urcu_desc "RCU")
    /** @ingroup cds_intrusive_map
        @anchor cds_intrusive_SkipListSet_rcu

        The implementation of well-known probabilistic data structure called skip-list
        invented by W.Pugh in his papers:
            - [1989] W.Pugh Skip Lists: A Probabilistic Alternative to Balanced Trees
            - [1990] W.Pugh A Skip List Cookbook

        A skip-list is a probabilistic data structure that provides expected logarithmic
        time search without the need of rebalance. The skip-list is a collection of sorted
        linked list. Nodes are ordered by key. Each node is linked into a subset of the lists.
        Each list has a level, ranging from 0 to 32. The bottom-level list contains
        all the nodes, and each higher-level list is a sublist of the lower-level lists.
        Each node is created with a random top level (with a random height), and belongs
        to all lists up to that level. The probability that a node has the height 1 is 1/2.
        The probability that a node has the height N is 1/2 ** N (more precisely,
        the distribution depends on an random generator provided, but our generators
        have this property).

        The lock-free variant of skip-list is implemented according to book
            - [2008] M.Herlihy, N.Shavit "The Art of Multiprocessor Programming",
                chapter 14.4 "A Lock-Free Concurrent Skiplist".

        <b>Template arguments</b>:
            - \p RCU - one of \ref cds_urcu_gc "RCU type"
            - \p T - type to be stored in the list. The type must be based on \p skip_list::node (for \p skip_list::base_hook)
                or it must have a member of type \p skip_list::node (for \p skip_list::member_hook).
            - \p Traits - set traits, default is \p skip_list::traits
                It is possible to declare option-based list with \p cds::intrusive::skip_list::make_traits metafunction
                instead of \p Traits template argument.

        @note Before including <tt><cds/intrusive/skip_list_rcu.h></tt> you should include appropriate RCU header file,
        see \ref cds_urcu_gc "RCU type" for list of existing RCU class and corresponding header files.

        <b>Iterators</b>

        The class supports a forward iterator (\ref iterator and \ref const_iterator).
        The iteration is ordered.

        You may iterate over skip-list set items only under RCU lock.
        Only in this case the iterator is thread-safe since
        while RCU is locked any set's item cannot be reclaimed.

        @note The requirement of RCU lock during iterating means that any type of modification of the skip list
        (i.e. inserting, erasing and so on) is not possible.

        @warning The iterator object cannot be passed between threads.

        Example how to use skip-list set iterators:
        \code
        // First, you should include the header for RCU type you have chosen
        #include <cds/urcu/general_buffered.h>
        #include <cds/intrusive/skip_list_rcu.h>

        typedef cds::urcu::gc< cds::urcu::general_buffered<> > rcu_type;

        struct Foo {
            // ...
        };

        // Traits for your skip-list.
        // At least, you should define cds::opt::less or cds::opt::compare for Foo struct
        struct my_traits: public cds::intrusive::skip_list::traits
        {
            // ...
        };
        typedef cds::intrusive::SkipListSet< rcu_type, Foo, my_traits > my_skiplist_set;

        my_skiplist_set theSet;

        // ...

        // Begin iteration
        {
            // Apply RCU locking manually
            typename rcu_type::scoped_lock sl;

            for ( auto it = theList.begin(); it != theList.end(); ++it ) {
                // ...
            }

            // rcu_type::scoped_lock destructor releases RCU lock implicitly
        }
        \endcode

        The iterator class supports the following minimalistic interface:
        \code
        struct iterator {
            // Default ctor
            iterator();

            // Copy ctor
            iterator( iterator const& s);

            value_type * operator ->() const;
            value_type& operator *() const;

            // Pre-increment
            iterator& operator ++();

            // Copy assignment
            iterator& operator = (const iterator& src);

            bool operator ==(iterator const& i ) const;
            bool operator !=(iterator const& i ) const;
        };
        \endcode
        Note, the iterator object returned by \ref end, \p cend member functions points to \p nullptr and should not be dereferenced.

        <b>How to use</b>

        You should incorporate skip_list::node into your struct \p T and provide
        appropriate skip_list::traits::hook in your \p Traits template parameters. Usually, for \p Traits you
        define a struct based on \p skip_list::traits.

        Example for <tt>cds::urcu::general_buffered<></tt> RCU and base hook:
        \code
        // First, you should include the header for RCU type you have chosen
        #include <cds/urcu/general_buffered.h>

        // Include RCU skip-list specialization
        #include <cds/intrusive/skip_list_rcu.h>

        // RCU type typedef
        typedef cds::urcu::gc< cds::urcu::general_buffered<> > rcu_type;

        // Data stored in skip list
        struct my_data: public cds::intrusive::skip_list::node< rcu_type >
        {
            // key field
            std::string     strKey;

            // other data
            // ...
        };

        // my_data compare functor
        struct my_data_cmp {
            int operator()( const my_data& d1, const my_data& d2 )
            {
                return d1.strKey.compare( d2.strKey );
            }

            int operator()( const my_data& d, const std::string& s )
            {
                return d.strKey.compare(s);
            }

            int operator()( const std::string& s, const my_data& d )
            {
                return s.compare( d.strKey );
            }
        };

        // Declare traits
        struct my_traits: public cds::intrusive::skip_list::traits
        {
            typedef cds::intrusive::skip_list::base_hook< cds::opt::gc< rcu_type > >   hook;
            typedef my_data_cmp compare;
        };

        // Declare skip-list set type
        typedef cds::intrusive::SkipListSet< rcu_type, my_data, my_traits >     traits_based_set;
        \endcode

        Equivalent option-based code:
        \code
        #include <cds/urcu/general_buffered.h>
        #include <cds/intrusive/skip_list_rcu.h>

        typedef cds::urcu::gc< cds::urcu::general_buffered<> > rcu_type;

        struct my_data {
            // see above
        };
        struct compare {
            // see above
        };

        // Declare option-based skip-list set
        typedef cds::intrusive::SkipListSet< rcu_type
            ,my_data
            , typename cds::intrusive::skip_list::make_traits<
                cds::intrusive::opt::hook< cds::intrusive::skip_list::base_hook< cds::opt::gc< rcu_type > > >
                ,cds::intrusive::opt::compare< my_data_cmp >
            >::type
        > option_based_set;

        \endcode
    */
    template <
        class RCU
       ,typename T
#ifdef CDS_DOXYGEN_INVOKED
       ,typename Traits = skip_list::traits
#else
       ,typename Traits
#endif
    >
    class SkipListSet< cds::urcu::gc< RCU >, T, Traits >
    {
    public:
        typedef cds::urcu::gc< RCU > gc; ///< Garbage collector
        typedef T       value_type;      ///< type of value stored in the skip-list
        typedef Traits  traits;          ///< Traits template parameter

        typedef typename traits::hook    hook;      ///< hook type
        typedef typename hook::node_type node_type; ///< node type

#   ifdef CDS_DOXYGEN_INVOKED
        typedef implementation_defined key_comparator  ;    ///< key comparison functor based on \p Traits::compare and \p Traits::less
#   else
        typedef typename opt::details::make_comparator< value_type, traits >::type key_comparator;
#   endif

        typedef typename traits::disposer  disposer;   ///< disposer
        typedef typename get_node_traits< value_type, node_type, hook>::type node_traits;    ///< node traits

        typedef typename traits::item_counter  item_counter;   ///< Item counting policy used
        typedef typename traits::memory_model  memory_model;   ///< Memory ordering, see \p cds::opt::memory_model option
        typedef typename traits::random_level_generator    random_level_generator;   ///< random level generator
        typedef typename traits::allocator     allocator_type; ///< allocator for maintaining array of next pointers of the node
        typedef typename traits::back_off      back_off;       ///< Back-off strategy
        typedef typename traits::stat          stat;           ///< internal statistics type
        typedef typename traits::rcu_check_deadlock rcu_check_deadlock; ///< Deadlock checking policy
        typedef typename gc::scoped_lock       rcu_lock;      ///< RCU scoped lock
        static constexpr const bool c_bExtractLockExternal = false; ///< Group of \p extract_xxx functions does not require external locking


        /// Max node height. The actual node height should be in range <tt>[0 .. c_nMaxHeight)</tt>
        /**
            The max height is specified by \ref skip_list::random_level_generator "random level generator" constant \p m_nUpperBound
            but it should be no more than 32 (\ref skip_list::c_nHeightLimit).
        */
        static unsigned int const c_nMaxHeight = std::conditional<
            (random_level_generator::c_nUpperBound <= skip_list::c_nHeightLimit),
            std::integral_constant< unsigned int, random_level_generator::c_nUpperBound >,
            std::integral_constant< unsigned int, skip_list::c_nHeightLimit >
        >::type::value;

        //@cond
        static unsigned int const c_nMinHeight = 5;
        //@endcond

    protected:
        typedef typename node_type::atomic_marked_ptr   atomic_node_ptr ;   ///< Atomic marked node pointer
        typedef typename node_type::marked_ptr          marked_node_ptr ;   ///< Node marked pointer

    protected:
        //@cond
        typedef skip_list::details::intrusive_node_builder< node_type, atomic_node_ptr, allocator_type > intrusive_node_builder;

        typedef typename std::conditional<
            std::is_same< typename traits::internal_node_builder, cds::opt::none >::value
            ,intrusive_node_builder
            ,typename traits::internal_node_builder
        >::type node_builder;

        typedef std::unique_ptr< node_type, typename node_builder::node_disposer >    scoped_node_ptr;

        static void dispose_node( value_type * pVal )
        {
            assert( pVal );

            typename node_builder::node_disposer()( node_traits::to_node_ptr(pVal));
            disposer()( pVal );
        }

        struct node_disposer
        {
            void operator()( value_type * pVal )
            {
                dispose_node( pVal );
            }
        };

        static void dispose_chain( node_type * pChain )
        {
            if ( pChain ) {
                assert( !gc::is_locked());

                auto f = [&pChain]() -> cds::urcu::retired_ptr {
                    node_type * p = pChain;
                    if ( p ) {
                        pChain = p->m_pDelChain;
                        return cds::urcu::make_retired_ptr<node_disposer>( node_traits::to_value_ptr( p ));
                    }
                    return cds::urcu::make_retired_ptr<node_disposer>( static_cast<value_type *>(nullptr));
                };
                gc::batch_retire(std::ref(f));
            }
        }

        struct position {
            node_type *   pPrev[ c_nMaxHeight ];
            node_type *   pSucc[ c_nMaxHeight ];
            node_type *   pNext[ c_nMaxHeight ];

            node_type *   pCur;
            node_type *   pDelChain;

            position()
                : pDelChain( nullptr )
            {}

            ~position()
            {
                dispose_chain( pDelChain );
            }

            void dispose( node_type * p )
            {
                assert( p != nullptr );
                assert( p->m_pDelChain == nullptr );

                p->m_pDelChain = pDelChain;
                pDelChain = p;
            }
        };

        typedef cds::urcu::details::check_deadlock_policy< gc, rcu_check_deadlock>   check_deadlock_policy;
        //@endcond

    protected:
        skip_list::details::head_node< node_type > m_Head;   ///< head tower (max height)

        random_level_generator      m_RandomLevelGen;   ///< random level generator instance
        atomics::atomic<unsigned int>    m_nHeight;     ///< estimated high level
        atomics::atomic<node_type *>     m_pDeferredDelChain ;   ///< Deferred deleted node chain
        item_counter                m_ItemCounter;      ///< item counter
        mutable stat                m_Stat;             ///< internal statistics

    protected:
        //@cond
        unsigned int random_level()
        {
            // Random generator produces a number from range [0..31]
            // We need a number from range [1..32]
            return m_RandomLevelGen() + 1;
        }

        template <typename Q>
        node_type * build_node( Q v )
        {
            return node_builder::make_tower( v, m_RandomLevelGen );
        }
        //@endcond

    public:
        using exempt_ptr = cds::urcu::exempt_ptr< gc, value_type, value_type, node_disposer, void >; ///< pointer to extracted node

    private:
        //@cond
        struct chain_disposer {
            void operator()( node_type * pChain ) const
            {
                dispose_chain( pChain );
            }
        };
        typedef cds::intrusive::details::raw_ptr_disposer< gc, node_type, chain_disposer> raw_ptr_disposer;
        //@endcond

    public:
        /// Result of \p get(), \p get_with() functions - pointer to the node found
        typedef cds::urcu::raw_ptr< gc, value_type, raw_ptr_disposer > raw_ptr;

    public:
        /// Default constructor
        SkipListSet()
            : m_Head( c_nMaxHeight )
            , m_nHeight( c_nMinHeight )
            , m_pDeferredDelChain( nullptr )
        {
            static_assert( (std::is_same< gc, typename node_type::gc >::value), "GC and node_type::gc must be the same type" );

            // Barrier for head node
            atomics::atomic_thread_fence( memory_model::memory_order_release );
        }

        /// Clears and destructs the skip-list
        ~SkipListSet()
        {
            destroy();
        }

    public:
    ///@name Forward iterators (thread-safe under RCU lock)
    //@{
        /// Forward iterator
        /**
            The forward iterator has some features:
            - it has no post-increment operator
            - it depends on iterator of underlying \p OrderedList

            You may safely use iterators in multi-threaded environment only under RCU lock.
            Otherwise, a crash is possible if another thread deletes the element the iterator points to.
        */
        typedef skip_list::details::iterator< gc, node_traits, back_off, false >  iterator;

        /// Const iterator type
        typedef skip_list::details::iterator< gc, node_traits, back_off, true >   const_iterator;

        /// Returns a forward iterator addressing the first element in a set
        iterator begin()
        {
            return iterator( *m_Head.head());
        }

        /// Returns a forward const iterator addressing the first element in a set
        const_iterator begin() const
        {
            return const_iterator( *m_Head.head());
        }

        /// Returns a forward const iterator addressing the first element in a set
        const_iterator cbegin() const
        {
            return const_iterator( *m_Head.head());
        }

        /// Returns a forward iterator that addresses the location succeeding the last element in a set.
        iterator end()
        {
            return iterator();
        }

        /// Returns a forward const iterator that addresses the location succeeding the last element in a set.
        const_iterator end() const
        {
            return const_iterator();
        }

        /// Returns a forward const iterator that addresses the location succeeding the last element in a set.
        const_iterator cend() const
        {
            return const_iterator();
        }
    //@}

    public:
        /// Inserts new node
        /**
            The function inserts \p val in the set if it does not contain
            an item with key equal to \p val.

            The function applies RCU lock internally.

            Returns \p true if \p val is placed into the set, \p false otherwise.
        */
        bool insert( value_type& val )
        {
            return insert( val, []( value_type& ) {} );
        }

        /// Inserts new node
        /**
            This function is intended for derived non-intrusive containers.

            The function allows to split creating of new item into two part:
            - create item with key only
            - insert new item into the set
            - if inserting is success, calls  \p f functor to initialize value-field of \p val.

            The functor signature is:
            \code
                void func( value_type& val );
            \endcode
            where \p val is the item inserted. User-defined functor \p f should guarantee that during changing
            \p val no any other changes could be made on this set's item by concurrent threads.
            The user-defined functor is called only if the inserting is success.

            RCU \p synchronize method can be called. RCU should not be locked.
        */
        template <typename Func>
        bool insert( value_type& val, Func f )
        {
            check_deadlock_policy::check();

            position pos;
            bool bRet;

            {
                node_type * pNode = node_traits::to_node_ptr( val );
                scoped_node_ptr scp( pNode );
                unsigned int nHeight = pNode->height();
                bool bTowerOk = nHeight > 1 && pNode->get_tower() != nullptr;
                bool bTowerMade = false;

                rcu_lock rcuLock;

                while ( true )
                {
                    bool bFound = find_position( val, pos, key_comparator(), true );
                    if ( bFound ) {
                        // scoped_node_ptr deletes the node tower if we create it
                        if ( !bTowerMade )
                            scp.release();

                        m_Stat.onInsertFailed();
                        bRet = false;
                        break;
                    }

                    if ( !bTowerOk ) {
                        build_node( pNode );
                        nHeight = pNode->height();
                        bTowerMade =
                            bTowerOk = true;
                    }

                    if ( !insert_at_position( val, pNode, pos, f )) {
                        m_Stat.onInsertRetry();
                        continue;
                    }

                    increase_height( nHeight );
                    ++m_ItemCounter;
                    m_Stat.onAddNode( nHeight );
                    m_Stat.onInsertSuccess();
                    scp.release();
                    bRet =  true;
                    break;
                }
            }

            return bRet;
        }

        /// Updates the node
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the item \p val is not found in the set, then \p val is inserted into the set
            iff \p bInsert is \p true.
            Otherwise, the functor \p func is called with item found.
            The functor signature is:
            \code
                void func( bool bNew, value_type& item, value_type& val );
            \endcode
            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the set
            - \p val - argument \p val passed into the \p %update() function
            If new item has been inserted (i.e. \p bNew is \p true) then \p item and \p val arguments
            refer to the same thing.

            The functor can change non-key fields of the \p item; however, \p func must guarantee
            that during changing no any other modifications could be made on this item by concurrent threads.

            RCU \p synchronize method can be called. RCU should not be locked.

            Returns std::pair<bool, bool> where \p first is \p true if operation is successful,
            i.e. the node has been inserted or updated,
            \p second is \p true if new item has been added or \p false if the item with \p key
            already exists.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"
        */
        template <typename Func>
        std::pair<bool, bool> update( value_type& val, Func func, bool bInsert = true )
        {
            check_deadlock_policy::check();

            position pos;
            std::pair<bool, bool> bRet( true, false );

            {
                node_type * pNode = node_traits::to_node_ptr( val );
                scoped_node_ptr scp( pNode );
                unsigned int nHeight = pNode->height();
                bool bTowerOk = nHeight > 1 && pNode->get_tower() != nullptr;
                bool bTowerMade = false;

                rcu_lock rcuLock;
                while ( true )
                {
                    bool bFound = find_position( val, pos, key_comparator(), true );
                    if ( bFound ) {
                        // scoped_node_ptr deletes the node tower if we create it before
                        if ( !bTowerMade )
                            scp.release();

                        func( false, *node_traits::to_value_ptr(pos.pCur), val );
                        m_Stat.onUpdateExist();
                        break;
                    }

                    if ( !bInsert ) {
                        scp.release();
                        bRet.first = false;
                        break;
                    }

                    if ( !bTowerOk ) {
                        build_node( pNode );
                        nHeight = pNode->height();
                        bTowerMade =
                            bTowerOk = true;
                    }

                    if ( !insert_at_position( val, pNode, pos, [&func]( value_type& item ) { func( true, item, item ); })) {
                        m_Stat.onInsertRetry();
                        continue;
                    }

                    increase_height( nHeight );
                    ++m_ItemCounter;
                    scp.release();
                    m_Stat.onAddNode( nHeight );
                    m_Stat.onUpdateNew();
                    bRet.second = true;
                    break;
                }
            }

            return bRet;
        }
        //@cond
        template <typename Func>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<bool, bool> ensure( value_type& val, Func func )
        {
            return update( val, func, true );
        }
        //@endcond

        /// Unlinks the item \p val from the set
        /**
            The function searches the item \p val in the set and unlink it from the set
            if it is found and is equal to \p val.

            Difference between \p erase() and \p %unlink() functions: \p erase() finds <i>a key</i>
            and deletes the item found. \p %unlink() searches an item by key and deletes it
            only if \p val is an item of that set, i.e. the pointer to item found
            is equal to <tt> &val </tt>.

            RCU \p synchronize method can be called. RCU should not be locked.

            The \ref disposer specified in \p Traits class template parameter is called
            by garbage collector \p GC asynchronously.

            The function returns \p true if success and \p false otherwise.
        */
        bool unlink( value_type& val )
        {
            check_deadlock_policy::check();

            position pos;
            bool bRet;

            {
                rcu_lock l;

                if ( !find_position( val, pos, key_comparator(), false )) {
                    m_Stat.onUnlinkFailed();
                    bRet = false;
                }
                else {
                    node_type * pDel = pos.pCur;
                    assert( key_comparator()( *node_traits::to_value_ptr( pDel ), val ) == 0 );

                    unsigned int nHeight = pDel->height();

                    if ( node_traits::to_value_ptr( pDel ) == &val && try_remove_at( pDel, pos, [](value_type const&) {}, false )) {
                        --m_ItemCounter;
                        m_Stat.onRemoveNode( nHeight );
                        m_Stat.onUnlinkSuccess();
                        bRet = true;
                    }
                    else {
                        m_Stat.onUnlinkFailed();
                        bRet = false;
                    }
                }
            }

            return bRet;
        }

        /// Extracts the item from the set with specified \p key
        /** \anchor cds_intrusive_SkipListSet_rcu_extract
            The function searches an item with key equal to \p key in the set,
            unlinks it from the set, and returns \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to the item found.
            If the item with key equal to \p key is not found the function returns an empty \p exempt_ptr.

            Note the compare functor should accept a parameter of type \p Q that can be not the same as \p value_type.

            RCU \p synchronize method can be called. RCU should NOT be locked.
            The function does not call the disposer for the item found.
            The disposer will be implicitly invoked when the returned object is destroyed or when
            its \p release() member function is called.
            Example:
            \code
            typedef cds::intrusive::SkipListSet< cds::urcu::gc< cds::urcu::general_buffered<> >, foo, my_traits > skip_list;
            skip_list theList;
            // ...

            typename skip_list::exempt_ptr ep( theList.extract( 5 ));
            if ( ep ) {
                // Deal with ep
                //...

                // Dispose returned item.
                ep.release();
            }
            \endcode
        */
        template <typename Q>
        exempt_ptr extract( Q const& key )
        {
            return exempt_ptr( do_extract( key ));
        }

        /// Extracts the item from the set with comparing functor \p pred
        /**
            The function is an analog of \p extract(Q const&) but \p pred predicate is used for key comparing.
            \p Less has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        exempt_ptr extract_with( Q const& key, Less pred )
        {
            return exempt_ptr( do_extract_with( key, pred ));
        }

        /// Extracts an item with minimal key from the list
        /**
            The function searches an item with minimal key, unlinks it, and returns \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to the item.
            If the skip-list is empty the function returns an empty \p exempt_ptr.

            RCU \p synchronize method can be called. RCU should NOT be locked.
            The function does not call the disposer for the item found.
            The disposer will be implicitly invoked when the returned object is destroyed or when
            its \p release() member function is manually called.
            Example:
            \code
            typedef cds::intrusive::SkipListSet< cds::urcu::gc< cds::urcu::general_buffered<> >, foo, my_traits > skip_list;
            skip_list theList;
            // ...

            typename skip_list::exempt_ptr ep(theList.extract_min());
            if ( ep ) {
                // Deal with ep
                //...

                // Dispose returned item.
                ep.release();
            }
            \endcode

            @note Due the concurrent nature of the list, the function extracts <i>nearly</i> minimum key.
            It means that the function gets leftmost item and tries to unlink it.
            During unlinking, a concurrent thread may insert an item with key less than leftmost item's key.
            So, the function returns the item with minimum key at the moment of list traversing.
        */
        exempt_ptr extract_min()
        {
            return exempt_ptr( do_extract_min());
        }

        /// Extracts an item with maximal key from the list
        /**
            The function searches an item with maximal key, unlinks it, and returns \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to the item.
            If the skip-list is empty the function returns an empty \p exempt_ptr.

            RCU \p synchronize method can be called. RCU should NOT be locked.
            The function does not call the disposer for the item found.
            The disposer will be implicitly invoked when the returned object is destroyed or when
            its \p release() member function is manually called.
            Example:
            \code
            typedef cds::intrusive::SkipListSet< cds::urcu::gc< cds::urcu::general_buffered<> >, foo, my_traits > skip_list;
            skip_list theList;
            // ...

            typename skip_list::exempt_ptr ep( theList.extract_max());
            if ( ep ) {
                // Deal with ep
                //...
                // Dispose returned item.
                ep.release();
            }
            \endcode

            @note Due the concurrent nature of the list, the function extracts <i>nearly</i> maximal key.
            It means that the function gets rightmost item and tries to unlink it.
            During unlinking, a concurrent thread can insert an item with key greater than rightmost item's key.
            So, the function returns the item with maximum key at the moment of list traversing.
        */
        exempt_ptr extract_max()
        {
            return exempt_ptr( do_extract_max());
        }

        /// Deletes the item from the set
        /** \anchor cds_intrusive_SkipListSet_rcu_erase
            The function searches an item with key equal to \p key in the set,
            unlinks it from the set, and returns \p true.
            If the item with key equal to \p key is not found the function return \p false.

            Note the hash functor should accept a parameter of type \p Q that can be not the same as \p value_type.

            RCU \p synchronize method can be called. RCU should not be locked.
        */
        template <typename Q>
        bool erase( const Q& key )
        {
            return do_erase( key, key_comparator(), [](value_type const&) {} );
        }

        /// Delete the item from the set with comparing functor \p pred
        /**
            The function is an analog of \ref cds_intrusive_SkipListSet_rcu_erase "erase(Q const&)"
            but \p pred predicate is used for key comparing.
            \p Less has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        bool erase_with( const Q& key, Less pred )
        {
            CDS_UNUSED( pred );
            return do_erase( key, cds::opt::details::make_comparator_from_less<Less>(), [](value_type const&) {} );
        }

        /// Deletes the item from the set
        /** \anchor cds_intrusive_SkipListSet_rcu_erase_func
            The function searches an item with key equal to \p key in the set,
            call \p f functor with item found, unlinks it from the set, and returns \p true.
            The \ref disposer specified in \p Traits class template parameter is called
            by garbage collector \p GC asynchronously.

            The \p Func interface is
            \code
            struct functor {
                void operator()( value_type const& item );
            };
            \endcode
            If the item with key equal to \p key is not found the function return \p false.

            Note the hash functor should accept a parameter of type \p Q that can be not the same as \p value_type.

            RCU \p synchronize method can be called. RCU should not be locked.
        */
        template <typename Q, typename Func>
        bool erase( Q const& key, Func f )
        {
            return do_erase( key, key_comparator(), f );
        }

        /// Delete the item from the set with comparing functor \p pred
        /**
            The function is an analog of \ref cds_intrusive_SkipListSet_rcu_erase_func "erase(Q const&, Func)"
            but \p pred predicate is used for key comparing.
            \p Less has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less, typename Func>
        bool erase_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return do_erase( key, cds::opt::details::make_comparator_from_less<Less>(), f );
        }

        /// Finds \p key
        /** @anchor cds_intrusive_SkipListSet_rcu_find_func
            The function searches the item with key equal to \p key and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q& key );
            };
            \endcode
            where \p item is the item found, \p key is the <tt>find</tt> function argument.

            The functor can change non-key fields of \p item. Note that the functor is only guarantee
            that \p item cannot be disposed during functor is executing.
            The functor does not serialize simultaneous access to the set \p item. If such access is
            possible you must provide your own synchronization schema on item level to exclude unsafe item modifications.

            The \p key argument is non-const since it can be used as \p f functor destination i.e., the functor
            can modify both arguments.

            The function applies RCU lock internally.

            The function returns \p true if \p key is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q& key, Func f )
        {
            return do_find_with( key, key_comparator(), f );
        }
        //@cond
        template <typename Q, typename Func>
        bool find( Q const& key, Func f )
        {
            return do_find_with( key, key_comparator(), f );
        }
        //@endcond

        /// Finds the key \p key with comparing functor \p pred
        /**
            The function is an analog of \ref cds_intrusive_SkipListSet_rcu_find_func "find(Q&, Func)"
            but \p cmp is used for key comparison.
            \p Less functor has the interface like \p std::less.
            \p cmp must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return do_find_with( key, cds::opt::details::make_comparator_from_less<Less>(), f );
        }
        //@cond
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return do_find_with( key, cds::opt::details::make_comparator_from_less<Less>(), f );
        }
        //@endcond

        /// Checks whether the set contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.

            The function applies RCU lock internally.
        */
        template <typename Q>
        bool contains( Q const& key )
        {
            return do_find_with( key, key_comparator(), [](value_type& , Q const& ) {} );
        }
        //@cond
        template <typename Q>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find( Q const& key )
        {
            return contains( key );
        }
        //@endcond

        /// Checks whether the set contains \p key using \p pred predicate for searching
        /**
            The function is similar to <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        bool contains( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return do_find_with( key, cds::opt::details::make_comparator_from_less<Less>(), [](value_type& , Q const& ) {} );
        }
        //@cond
        template <typename Q, typename Less>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find_with( Q const& key, Less pred )
        {
            return contains( key, pred );
        }
        //@endcond

        /// Finds \p key and return the item found
        /** \anchor cds_intrusive_SkipListSet_rcu_get
            The function searches the item with key equal to \p key and returns a \p raw_ptr object pointed to item found.
            If \p key is not found it returns empty \p raw_ptr.

            Note the compare functor should accept a parameter of type \p Q that can be not the same as \p value_type.

            RCU should be locked before call of this function.
            Returned item is valid only while RCU is locked:
            \code
            typedef cds::intrusive::SkipListSet< cds::urcu::gc< cds::urcu::general_buffered<> >, foo, my_traits > skip_list;
            skip_list theList;
            // ...
            typename skip_list::raw_ptr pVal;
            {
                // Lock RCU
                skip_list::rcu_lock lock;

                pVal = theList.get( 5 );
                if ( pVal ) {
                    // Deal with pVal
                    //...
                }
            }
            // You can manually release pVal after RCU-locked section
            pVal.release();
            \endcode
        */
        template <typename Q>
        raw_ptr get( Q const& key )
        {
            assert( gc::is_locked());

            position pos;
            value_type * pFound;
            if ( do_find_with( key, key_comparator(), [&pFound](value_type& found, Q const& ) { pFound = &found; }, pos ))
                return raw_ptr( pFound, raw_ptr_disposer( pos ));
            return raw_ptr( raw_ptr_disposer( pos ));
        }

        /// Finds \p key and return the item found
        /**
            The function is an analog of \ref cds_intrusive_SkipListSet_rcu_get "get(Q const&)"
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        raw_ptr get_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            assert( gc::is_locked());

            value_type * pFound = nullptr;
            position pos;
            if ( do_find_with( key, cds::opt::details::make_comparator_from_less<Less>(),
                [&pFound](value_type& found, Q const& ) { pFound = &found; }, pos ))
            {
                return raw_ptr( pFound, raw_ptr_disposer( pos ));
            }
            return raw_ptr( raw_ptr_disposer( pos ));
        }

        /// Returns item count in the set
        /**
            The value returned depends on item counter type provided by \p Traits template parameter.
            For \p atomicity::empty_item_counter the function always returns 0.
            Therefore, the function is not suitable for checking the set emptiness, use \p empty()
            member function for this purpose.
        */
        size_t size() const
        {
            return m_ItemCounter;
        }

        /// Checks if the set is empty
        bool empty() const
        {
            return m_Head.head()->next( 0 ).load( memory_model::memory_order_relaxed ) == nullptr;
        }

        /// Clears the set (not atomic)
        /**
            The function unlink all items from the set.
            The function is not atomic, thus, in multi-threaded environment with parallel insertions
            this sequence
            \code
            set.clear();
            assert( set.empty());
            \endcode
            the assertion could be raised.

            For each item the \p disposer will be called automatically after unlinking.
        */
        void clear()
        {
            exempt_ptr ep;
            while ( (ep = extract_min()));
        }

        /// Returns maximum height of skip-list. The max height is a constant for each object and does not exceed 32.
        static constexpr unsigned int max_height() noexcept
        {
            return c_nMaxHeight;
        }

        /// Returns const reference to internal statistics
        stat const& statistics() const
        {
            return m_Stat;
        }

    protected:
        //@cond

        bool is_extracted( marked_node_ptr const p ) const
        {
            return ( p.bits() & 2 ) != 0;
        }

        void help_remove( int nLevel, node_type* pPred, marked_node_ptr pCur, marked_node_ptr pSucc, position& pos )
        {
            marked_node_ptr p( pCur.ptr());

            if ( pCur->is_upper_level( nLevel )
                && pPred->next( nLevel ).compare_exchange_strong( p, marked_node_ptr( pSucc.ptr()),
                    memory_model::memory_order_release, atomics::memory_order_relaxed ))
            {
                if ( pCur->level_unlinked()) {
                    if ( !is_extracted( pSucc )) {
                        // We cannot free the node at this moment because RCU is locked
                        // Link deleted nodes to a chain to free later
                        pos.dispose( pCur.ptr());
                        m_Stat.onEraseWhileFind();
                    }
                    else
                        m_Stat.onExtractWhileFind();
                }
            }
        }

        template <typename Q, typename Compare >
        bool find_position( Q const& val, position& pos, Compare cmp, bool bStopIfFound )
        {
            assert( gc::is_locked());

            node_type * pPred;
            marked_node_ptr pSucc;
            marked_node_ptr pCur;
            int nCmp = 1;

        retry:
            pPred = m_Head.head();

            for ( int nLevel = static_cast<int>( c_nMaxHeight - 1 ); nLevel >= 0; --nLevel ) {

                while ( true ) {
                    pCur = pPred->next( nLevel ).load( memory_model::memory_order_acquire );
                    if ( pCur.bits()) {
                        // pCur.bits() means that pPred is logically deleted
                        goto retry;
                    }

                    if ( pCur.ptr() == nullptr ) {
                        // end of the list at level nLevel - goto next level
                        break;
                    }

                    // pSucc contains deletion mark for pCur
                    pSucc = pCur->next( nLevel ).load( memory_model::memory_order_acquire );

                    if ( pPred->next( nLevel ).load( memory_model::memory_order_acquire ).all() != pCur.ptr())
                        goto retry;

                    if ( pSucc.bits()) {
                        // pCur is marked, i.e. logically deleted.
                        help_remove( nLevel, pPred, pCur, pSucc, pos );
                        goto retry;
                    }
                    else {
                        nCmp = cmp( *node_traits::to_value_ptr( pCur.ptr()), val );
                        if ( nCmp < 0 )
                            pPred = pCur.ptr();
                        else if ( nCmp == 0 && bStopIfFound )
                            goto found;
                        else
                            break;
                    }
                }

                // Next level
                pos.pPrev[nLevel] = pPred;
                pos.pSucc[nLevel] = pCur.ptr();
            }

            if ( nCmp != 0 )
                return false;

        found:
            pos.pCur = pCur.ptr();
            return pCur.ptr() && nCmp == 0;
        }

        bool find_min_position( position& pos )
        {
            assert( gc::is_locked());

            node_type * pPred;
            marked_node_ptr pSucc;
            marked_node_ptr pCur;

        retry:
            pPred = m_Head.head();

            for ( int nLevel = static_cast<int>( c_nMaxHeight - 1 ); nLevel >= 0; --nLevel ) {

                pCur = pPred->next( nLevel ).load( memory_model::memory_order_acquire );
                // pCur.bits() means that pPred is logically deleted
                // head cannot be deleted
                assert( pCur.bits() == 0 );

                if ( pCur.ptr()) {

                    // pSucc contains deletion mark for pCur
                    pSucc = pCur->next( nLevel ).load( memory_model::memory_order_acquire );

                    if ( pPred->next( nLevel ).load( memory_model::memory_order_acquire ).all() != pCur.ptr())
                        goto retry;

                    if ( pSucc.bits()) {
                        // pCur is marked, i.e. logically deleted.
                        help_remove( nLevel, pPred, pCur, pSucc, pos );
                        goto retry;
                    }
                }

                // Next level
                pos.pPrev[nLevel] = pPred;
                pos.pSucc[nLevel] = pCur.ptr();
            }
            return ( pos.pCur = pCur.ptr()) != nullptr;
        }

        bool find_max_position( position& pos )
        {
            assert( gc::is_locked());

            node_type * pPred;
            marked_node_ptr pSucc;
            marked_node_ptr pCur;

        retry:
            pPred = m_Head.head();

            for ( int nLevel = static_cast<int>( c_nMaxHeight - 1 ); nLevel >= 0; --nLevel ) {

                while ( true ) {
                    pCur = pPred->next( nLevel ).load( memory_model::memory_order_acquire );
                    if ( pCur.bits()) {
                        // pCur.bits() means that pPred is logically deleted
                        goto retry;
                    }

                    if ( pCur.ptr() == nullptr ) {
                        // end of the list at level nLevel - goto next level
                        break;
                    }

                    // pSucc contains deletion mark for pCur
                    pSucc = pCur->next( nLevel ).load( memory_model::memory_order_acquire );

                    if ( pPred->next( nLevel ).load( memory_model::memory_order_acquire ).all() != pCur.ptr())
                        goto retry;

                    if ( pSucc.bits()) {
                        // pCur is marked, i.e. logically deleted.
                        help_remove( nLevel, pPred, pCur, pSucc, pos );
                        goto retry;
                    }
                    else {
                        if ( !pSucc.ptr())
                            break;

                        pPred = pCur.ptr();
                    }
                }

                // Next level
                pos.pPrev[nLevel] = pPred;
                pos.pSucc[nLevel] = pCur.ptr();
            }

            return ( pos.pCur = pCur.ptr()) != nullptr;
        }

        bool renew_insert_position( value_type& val, node_type * pNode, position& pos )
        {
            assert( gc::is_locked());

            node_type * pPred;
            marked_node_ptr pSucc;
            marked_node_ptr pCur;
            key_comparator cmp;
            int nCmp = 1;

        retry:
            pPred = m_Head.head();

            for ( int nLevel = static_cast<int>( c_nMaxHeight - 1 ); nLevel >= 0; --nLevel ) {

                while ( true ) {
                    pCur = pPred->next( nLevel ).load( memory_model::memory_order_acquire );
                    if ( pCur.bits()) {
                        // pCur.bits() means that pPred is logically deleted
                        goto retry;
                    }

                    if ( pCur.ptr() == nullptr ) {
                        // end of the list at level nLevel - goto next level
                        break;
                    }

                    // pSucc contains deletion mark for pCur
                    pSucc = pCur->next( nLevel ).load( memory_model::memory_order_acquire );

                    if ( pPred->next( nLevel ).load( memory_model::memory_order_acquire ).all() != pCur.ptr())
                        goto retry;

                    if ( pSucc.bits()) {
                        // pCur is marked, i.e. logically deleted.
                        if ( pCur.ptr() == pNode ) {
                            // Node is removing while we are inserting it
                            return false;
                        }

                        // try to help deleting pCur
                        help_remove( nLevel, pPred, pCur, pSucc, pos );
                        goto retry;
                    }
                    else {
                        nCmp = cmp( *node_traits::to_value_ptr( pCur.ptr()), val );
                        if ( nCmp < 0 )
                            pPred = pCur.ptr();
                        else
                            break;
                    }
                }

                // Next level
                pos.pPrev[nLevel] = pPred;
                pos.pSucc[nLevel] = pCur.ptr();
            }

            return nCmp == 0;
        }

        template <typename Func>
        bool insert_at_position( value_type& val, node_type * pNode, position& pos, Func f )
        {
            assert( gc::is_locked());

            unsigned int const nHeight = pNode->height();
            pNode->clear_tower();

            // Insert at level 0
            {
                marked_node_ptr p( pos.pSucc[0] );
                pNode->next( 0 ).store( p, memory_model::memory_order_relaxed );
                if ( !pos.pPrev[0]->next( 0 ).compare_exchange_strong( p, marked_node_ptr( pNode ), memory_model::memory_order_release, atomics::memory_order_relaxed ))
                    return false;

                f( val );
            }

            // Insert at level 1..max
            for ( unsigned int nLevel = 1; nLevel < nHeight; ++nLevel ) {
                marked_node_ptr p;
                while ( true ) {
                    marked_node_ptr pSucc( pos.pSucc[nLevel] );

                    // Set pNode->next
                    // pNode->next must be null but can have a "logical deleted" flag if another thread is removing pNode right now
                    if ( !pNode->next( nLevel ).compare_exchange_strong( p, pSucc,
                        memory_model::memory_order_acq_rel, atomics::memory_order_acquire ))
                    {
                        // pNode has been marked as removed while we are inserting it
                        // Stop inserting
                        assert( p.bits() != 0 );

                        // Here pNode is linked at least level 0 so level_unlinked() cannot returns true
                        CDS_VERIFY_FALSE( pNode->level_unlinked( nHeight - nLevel ));

                        // pNode is linked up to nLevel - 1
                        // Remove it via find_position()
                        find_position( val, pos, key_comparator(), false );

                        m_Stat.onLogicDeleteWhileInsert();
                        return true;
                    }
                    p = pSucc;

                    // Link pNode into the list at nLevel
                    if ( pos.pPrev[nLevel]->next( nLevel ).compare_exchange_strong( pSucc, marked_node_ptr( pNode ),
                        memory_model::memory_order_release, atomics::memory_order_relaxed ))
                    {
                        // go to next level
                        break;
                    }

                    // Renew insert position
                    m_Stat.onRenewInsertPosition();

                    if ( !renew_insert_position( val, pNode, pos )) {
                        // The node has been deleted while we are inserting it
                        // Update current height for concurent removing
                        CDS_VERIFY_FALSE( pNode->level_unlinked( nHeight - nLevel ));

                        m_Stat.onRemoveWhileInsert();

                        // help to removing val
                        find_position( val, pos, key_comparator(), false );
                        return true;
                    }
                }
            }
            return true;
        }

        template <typename Func>
        bool try_remove_at( node_type * pDel, position& pos, Func f, bool bExtract )
        {
            assert( pDel != nullptr );
            assert( gc::is_locked());

            marked_node_ptr pSucc;
            back_off bkoff;

            unsigned const nMask = bExtract ? 3u : 1u;

            // logical deletion (marking)
            for ( unsigned int nLevel = pDel->height() - 1; nLevel > 0; --nLevel ) {
                pSucc = pDel->next( nLevel ).load( memory_model::memory_order_relaxed );
                if ( pSucc.bits() == 0 ) {
                    bkoff.reset();
                    while ( !pDel->next( nLevel ).compare_exchange_weak( pSucc, pSucc | nMask,
                        memory_model::memory_order_release, atomics::memory_order_acquire ))
                    {
                        if ( pSucc.bits() == 0 ) {
                            bkoff();
                            m_Stat.onMarkFailed();
                        }
                        else if ( pSucc.bits() != nMask )
                            return false;
                    }
                }
            }

            marked_node_ptr p( pDel->next( 0 ).load( memory_model::memory_order_relaxed ).ptr());
            while ( true ) {
                if ( pDel->next( 0 ).compare_exchange_strong( p, p | nMask, memory_model::memory_order_release, atomics::memory_order_acquire ))
                {
                    f( *node_traits::to_value_ptr( pDel ));

                    // physical deletion
                    // try fast erase
                    p = pDel;
                    for ( int nLevel = static_cast<int>( pDel->height() - 1 ); nLevel >= 0; --nLevel ) {

                        pSucc = pDel->next( nLevel ).load( memory_model::memory_order_acquire );
                        if ( pos.pPrev[nLevel]->next( nLevel ).compare_exchange_strong( p, marked_node_ptr( pSucc.ptr()),
                            memory_model::memory_order_acq_rel, atomics::memory_order_relaxed ))
                        {
                            pDel->level_unlinked();
                        }
                        else {
                            // Make slow erase
#       ifdef CDS_DEBUG
                            if ( find_position( *node_traits::to_value_ptr( pDel ), pos, key_comparator(), false ))
                                assert( pDel != pos.pCur );
#       else
                            find_position( *node_traits::to_value_ptr( pDel ), pos, key_comparator(), false );
#       endif
                            if ( bExtract )
                                m_Stat.onSlowExtract();
                            else
                                m_Stat.onSlowErase();

                            return true;
                        }
                    }

                    // Fast erasing success
                    if ( !bExtract ) {
                        // We cannot free the node at this moment since RCU is locked
                        // Link deleted nodes to a chain to free later
                        pos.dispose( pDel );
                        m_Stat.onFastErase();
                    }
                    else
                        m_Stat.onFastExtract();
                    return true;
                }
                else if ( p.bits()) {
                    // Another thread is deleting pDel right now
                    m_Stat.onEraseContention();
                    return false;
                }

                m_Stat.onEraseRetry();
                bkoff();
            }
        }

        enum finsd_fastpath_result {
            find_fastpath_found,
            find_fastpath_not_found,
            find_fastpath_abort
        };
        template <typename Q, typename Compare, typename Func>
        finsd_fastpath_result find_fastpath( Q& val, Compare cmp, Func f ) const
        {
            node_type * pPred;
            marked_node_ptr pCur;
            marked_node_ptr pSucc;
            marked_node_ptr pNull;

            back_off bkoff;
            unsigned attempt = 0;

        try_again:
            pPred = m_Head.head();
            for ( int nLevel = static_cast<int>( m_nHeight.load( memory_model::memory_order_relaxed ) - 1 ); nLevel >= 0; --nLevel ) {
                pCur = pPred->next( nLevel ).load( memory_model::memory_order_acquire );

                while ( pCur != pNull ) {
                    if ( pCur.bits()) {
                        // pPred is being removed
                        if ( ++attempt < 4 ) {
                            bkoff();
                            goto try_again;
                        }

                        return find_fastpath_abort;
                    }

                    if ( pCur.ptr()) {
                        int nCmp = cmp( *node_traits::to_value_ptr( pCur.ptr()), val );
                        if ( nCmp < 0 ) {
                            pPred = pCur.ptr();
                            pCur = pCur->next( nLevel ).load( memory_model::memory_order_acquire );
                        }
                        else if ( nCmp == 0 ) {
                            // found
                            f( *node_traits::to_value_ptr( pCur.ptr()), val );
                            return find_fastpath_found;
                        }
                        else // pCur > val - go down
                            break;
                    }
                }
            }

            return find_fastpath_not_found;
        }

        template <typename Q, typename Compare, typename Func>
        bool find_slowpath( Q& val, Compare cmp, Func f, position& pos )
        {
            if ( find_position( val, pos, cmp, true )) {
                assert( cmp( *node_traits::to_value_ptr( pos.pCur ), val ) == 0 );

                f( *node_traits::to_value_ptr( pos.pCur ), val );
                return true;
            }
            else
                return false;
        }

        template <typename Q, typename Compare, typename Func>
        bool do_find_with( Q& val, Compare cmp, Func f )
        {
            position pos;
            return do_find_with( val, cmp, f, pos );
        }

        template <typename Q, typename Compare, typename Func>
        bool do_find_with( Q& val, Compare cmp, Func f, position& pos )
        {
            bool bRet;

            {
                rcu_lock l;

                switch ( find_fastpath( val, cmp, f )) {
                case find_fastpath_found:
                    m_Stat.onFindFastSuccess();
                    return true;
                case find_fastpath_not_found:
                    m_Stat.onFindFastFailed();
                    return false;
                default:
                    break;
                }

                if ( find_slowpath( val, cmp, f, pos )) {
                    m_Stat.onFindSlowSuccess();
                    bRet = true;
                }
                else {
                    m_Stat.onFindSlowFailed();
                    bRet = false;
                }
            }
            return bRet;
        }

        template <typename Q, typename Compare, typename Func>
        bool do_erase( Q const& val, Compare cmp, Func f )
        {
            check_deadlock_policy::check();

            position pos;
            bool bRet;

            {
                rcu_lock rcuLock;

                if ( !find_position( val, pos, cmp, false )) {
                    m_Stat.onEraseFailed();
                    bRet = false;
                }
                else {
                    node_type * pDel = pos.pCur;
                    assert( cmp( *node_traits::to_value_ptr( pDel ), val ) == 0 );

                    unsigned int nHeight = pDel->height();
                    if ( try_remove_at( pDel, pos, f, false )) {
                        --m_ItemCounter;
                        m_Stat.onRemoveNode( nHeight );
                        m_Stat.onEraseSuccess();
                        bRet = true;
                    }
                    else {
                        m_Stat.onEraseFailed();
                        bRet = false;
                    }
                }
            }

            return bRet;
        }

        template <typename Q, typename Compare>
        value_type * do_extract_key( Q const& key, Compare cmp, position& pos )
        {
            // RCU should be locked!!!
            assert( gc::is_locked());

            node_type * pDel;

            if ( !find_position( key, pos, cmp, false )) {
                m_Stat.onExtractFailed();
                pDel = nullptr;
            }
            else {
                pDel = pos.pCur;
                assert( cmp( *node_traits::to_value_ptr( pDel ), key ) == 0 );

                unsigned int const nHeight = pDel->height();

                if ( try_remove_at( pDel, pos, []( value_type const& ) {}, true )) {
                    --m_ItemCounter;
                    m_Stat.onRemoveNode( nHeight );
                    m_Stat.onExtractSuccess();
                }
                else {
                    m_Stat.onExtractFailed();
                    pDel = nullptr;
                }
            }

            return pDel ? node_traits::to_value_ptr( pDel ) : nullptr;
        }

        template <typename Q>
        value_type * do_extract( Q const& key )
        {
            check_deadlock_policy::check();
            value_type * pDel = nullptr;
            position pos;
            {
                rcu_lock l;
                pDel = do_extract_key( key, key_comparator(), pos );
            }

            return pDel;
        }

        template <typename Q, typename Less>
        value_type * do_extract_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            check_deadlock_policy::check();
            value_type * pDel = nullptr;
            position pos;
            {
                rcu_lock l;
                pDel = do_extract_key( key, cds::opt::details::make_comparator_from_less<Less>(), pos );
            }

            return pDel;
        }

        value_type * do_extract_min()
        {
            assert( !gc::is_locked());

            position pos;
            node_type * pDel;

            {
                rcu_lock l;

                if ( !find_min_position( pos )) {
                    m_Stat.onExtractMinFailed();
                    pDel = nullptr;
                }
                else {
                    pDel = pos.pCur;
                    unsigned int const nHeight = pDel->height();

                    if ( try_remove_at( pDel, pos, []( value_type const& ) {}, true )) {
                        --m_ItemCounter;
                        m_Stat.onRemoveNode( nHeight );
                        m_Stat.onExtractMinSuccess();
                    }
                    else {
                        m_Stat.onExtractMinFailed();
                        pDel = nullptr;
                    }
                }
            }

            return pDel ? node_traits::to_value_ptr( pDel ) : nullptr;
        }

        value_type * do_extract_max()
        {
            assert( !gc::is_locked());

            position pos;
            node_type * pDel;

            {
                rcu_lock l;

                if ( !find_max_position( pos )) {
                    m_Stat.onExtractMaxFailed();
                    pDel = nullptr;
                }
                else {
                    pDel = pos.pCur;
                    unsigned int const nHeight = pDel->height();

                    if ( try_remove_at( pDel, pos, []( value_type const& ) {}, true )) {
                        --m_ItemCounter;
                        m_Stat.onRemoveNode( nHeight );
                        m_Stat.onExtractMaxSuccess();
                    }
                    else {
                        m_Stat.onExtractMaxFailed();
                        pDel = nullptr;
                    }
                }
            }

            return pDel ? node_traits::to_value_ptr( pDel ) : nullptr;
        }

        void increase_height( unsigned int nHeight )
        {
            unsigned int nCur = m_nHeight.load( memory_model::memory_order_relaxed );
            if ( nCur < nHeight )
                m_nHeight.compare_exchange_strong( nCur, nHeight, memory_model::memory_order_release, atomics::memory_order_relaxed );
        }

        void destroy()
        {
            node_type* p = m_Head.head()->next( 0 ).load( atomics::memory_order_relaxed ).ptr();
            while ( p ) {
                node_type* pNext = p->next( 0 ).load( atomics::memory_order_relaxed ).ptr();
                dispose_node( node_traits::to_value_ptr( p ));
                p = pNext;
            }
        }

        //@endcond
    };

}} // namespace cds::intrusive


#endif // #ifndef CDSLIB_INTRUSIVE_SKIP_LIST_RCU_H
