// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_MICHAEL_LIST_NOGC_H
#define CDSLIB_INTRUSIVE_MICHAEL_LIST_NOGC_H

#include <cds/intrusive/details/michael_list_base.h>
#include <cds/gc/nogc.h>
#include <cds/details/make_const_type.h>

namespace cds { namespace intrusive {

    namespace michael_list {
        /// Michael list node
        /**
            Template parameters:
            - Tag - a tag used to distinguish between different implementation
        */
        template <typename Tag>
        struct node<gc::nogc, Tag>
        {
            typedef gc::nogc        gc  ;   ///< Garbage collector
            typedef Tag             tag ;   ///< tag

            typedef atomics::atomic< node * >   atomic_ptr  ;    ///< atomic marked pointer

            atomic_ptr m_pNext ; ///< pointer to the next node in the container

            node()
                : m_pNext( nullptr )
            {}
        };
    }   // namespace michael_list

    /// Michael's lock-free ordered single-linked list (template specialization for gc::nogc)
    /** @ingroup cds_intrusive_list
        \anchor cds_intrusive_MichaelList_nogc

        This specialization is intended for so-called append-only usage when no item
        reclamation may be performed. The class does not support item removal.

        See \ref cds_intrusive_MichaelList_hp "MichaelList" for description of template parameters.
    */
    template < typename T,
#ifdef CDS_DOXYGEN_INVOKED
        class Traits = michael_list::traits
#else
        class Traits
#endif
    >
    class MichaelList<gc::nogc, T, Traits>
    {
    public:
        typedef gc::nogc gc;   ///< Garbage collector
        typedef T       value_type; ///< type of value to be stored in the queue
        typedef Traits  traits;     ///< List traits

        typedef typename traits::hook     hook;      ///< hook type
        typedef typename hook::node_type  node_type; ///< node type

#   ifdef CDS_DOXYGEN_INVOKED
        typedef implementation_defined key_comparator  ;    ///< key comparison functor based on opt::compare and opt::less option setter.
#   else
        typedef typename opt::details::make_comparator< value_type, traits >::type key_comparator;
#   endif

        typedef typename traits::disposer  disposer;   ///< disposer used
        typedef typename get_node_traits< value_type, node_type, hook>::type node_traits ;    ///< node traits
        typedef typename michael_list::get_link_checker< node_type, traits::link_checker >::type link_checker;   ///< link checker

        typedef typename traits::back_off     back_off;     ///< back-off strategy
        typedef typename traits::item_counter item_counter; ///< Item counting policy used
        typedef typename traits::memory_model memory_model; ///< Memory ordering. See cds::opt::memory_model option
        typedef typename traits::stat         stat;         ///< Internal statistics

        //@cond
        static_assert((std::is_same< gc, typename node_type::gc >::value), "GC and node_type::gc must be the same type");

        // Rebind traits (split-list support)
        template <typename... Options>
        struct rebind_traits {
            typedef MichaelList<
                gc
                , value_type
                , typename cds::opt::make_options< traits, Options...>::type
            >   type;
        };

        // Stat selector
        template <typename Stat>
        using select_stat_wrapper = michael_list::select_stat_wrapper< Stat >;
        //@endcond

    protected:
        typedef typename node_type::atomic_ptr   atomic_node_ptr ;   ///< Atomic node pointer
        typedef atomic_node_ptr     auxiliary_head      ;   ///< Auxiliary head type (for split-list support)

        atomic_node_ptr     m_pHead;        ///< Head pointer
        item_counter        m_ItemCounter;  ///< Item counter
        stat                m_Stat;         ///< Internal statistics

        //@cond
        /// Position pointer for item search
        struct position {
            atomic_node_ptr * pPrev ;   ///< Previous node
            node_type * pCur        ;   ///< Current node
            node_type * pNext       ;   ///< Next node
        };
        //@endcond

    protected:
        //@cond
        static void clear_links( node_type * pNode )
        {
            pNode->m_pNext.store( nullptr, memory_model::memory_order_release );
        }

        template <class Disposer>
        static void dispose_node( node_type * pNode, Disposer disp )
        {
            clear_links( pNode );
            disp( node_traits::to_value_ptr( *pNode ));
        }

        template <class Disposer>
        static void dispose_value( value_type& val, Disposer disp )
        {
            dispose_node( node_traits::to_node_ptr( val ), disp );
        }

        static bool link_node( node_type * pNode, position& pos )
        {
            assert( pNode != nullptr );
            link_checker::is_empty( pNode );

            pNode->m_pNext.store( pos.pCur, memory_model::memory_order_relaxed );
            if ( cds_likely( pos.pPrev->compare_exchange_strong( pos.pCur, pNode, memory_model::memory_order_release, atomics::memory_order_relaxed )))
                return true;

            pNode->m_pNext.store( nullptr, memory_model::memory_order_relaxed );
            return false;
        }
        //@endcond

    protected:
        //@cond
        template <bool IsConst>
        class iterator_type
        {
            friend class MichaelList;
            value_type * m_pNode;

            void next()
            {
                if ( m_pNode ) {
                    node_type * pNode = node_traits::to_node_ptr( *m_pNode )->m_pNext.load(memory_model::memory_order_acquire);
                    if ( pNode )
                        m_pNode = node_traits::to_value_ptr( *pNode );
                    else
                        m_pNode = nullptr;
                }
            }

        protected:
            explicit iterator_type( node_type * pNode)
            {
                if ( pNode )
                    m_pNode = node_traits::to_value_ptr( *pNode );
                else
                    m_pNode = nullptr;
            }
            explicit iterator_type( atomic_node_ptr const& refNode)
            {
                node_type * pNode = refNode.load(memory_model::memory_order_relaxed);
                if ( pNode )
                    m_pNode = node_traits::to_value_ptr( *pNode );
                else
                    m_pNode = nullptr;
            }

        public:
            typedef typename cds::details::make_const_type<value_type, IsConst>::pointer   value_ptr;
            typedef typename cds::details::make_const_type<value_type, IsConst>::reference value_ref;

            iterator_type()
                : m_pNode( nullptr )
            {}

            iterator_type( const iterator_type& src )
                : m_pNode( src.m_pNode )
            {}

            value_ptr operator ->() const
            {
                return m_pNode;
            }

            value_ref operator *() const
            {
                assert( m_pNode != nullptr );
                return *m_pNode;
            }

            /// Pre-increment
            iterator_type& operator ++()
            {
                next();
                return *this;
            }

            /// Post-increment
            iterator_type operator ++(int)
            {
                iterator_type i(*this);
                next();
                return i;
            }

            iterator_type& operator = (const iterator_type& src)
            {
                m_pNode = src.m_pNode;
                return *this;
            }

            template <bool C>
            bool operator ==(iterator_type<C> const& i ) const
            {
                return m_pNode == i.m_pNode;
            }
            template <bool C>
            bool operator !=(iterator_type<C> const& i ) const
            {
                return m_pNode != i.m_pNode;
            }
        };
        //@endcond

    public:
        /// Forward iterator
        typedef iterator_type<false>    iterator;
        /// Const forward iterator
        typedef iterator_type<true>     const_iterator;

        /// Returns a forward iterator addressing the first element in a list
        /**
            For empty list \code begin() == end() \endcode
        */
        iterator begin()
        {
            return iterator(m_pHead.load(memory_model::memory_order_relaxed));
        }

        /// Returns an iterator that addresses the location succeeding the last element in a list
        /**
            Do not use the value returned by <tt>end</tt> function to access any item.
            Internally, <tt>end</tt> returning value equals to \p nullptr.

            The returned value can be used only to control reaching the end of the list.
            For empty list \code begin() == end() \endcode
        */
        iterator end()
        {
            return iterator();
        }

        /// Returns a forward const iterator addressing the first element in a list
        const_iterator begin() const
        {
            return const_iterator(m_pHead.load(memory_model::memory_order_relaxed));
        }
        /// Returns a forward const iterator addressing the first element in a list
        const_iterator cbegin() const
        {
            return const_iterator(m_pHead.load(memory_model::memory_order_relaxed));
        }

        /// Returns an const iterator that addresses the location succeeding the last element in a list
        const_iterator end() const
        {
            return const_iterator();
        }
        /// Returns an const iterator that addresses the location succeeding the last element in a list
        const_iterator cend() const
        {
            return const_iterator();
        }

    public:
        /// Default constructor initializes empty list
        MichaelList()
            : m_pHead( nullptr )
        {}

        //@cond
        template <typename Stat, typename = std::enable_if<std::is_same<stat, michael_list::wrapped_stat<Stat>>::value >>
        explicit MichaelList( Stat& st )
            : m_pHead( nullptr )
            , m_Stat( st )
        {}
        //@endcond

        /// Destroys the list objects
        ~MichaelList()
        {
            clear();
        }

        /// Inserts new node
        /**
            The function inserts \p val in the list if the list does not contain
            an item with key equal to \p val.

            Returns \p true if \p val is linked into the list, \p false otherwise.
        */
        bool insert( value_type& val )
        {
            return insert_at( m_pHead, val );
        }

        /// Updates the item
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the item \p val not found in the list, then \p val is inserted into the list
            iff \p bAllowInsert is \p true.
            Otherwise, the functor \p func is called with item found.
            The functor signature is:
            \code
            struct functor {
                void operator()( bool bNew, value_type& item, value_type& val );
            };
            \endcode
            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the list
            - \p val - argument \p val passed into the \p update() function
            If new item has been inserted (i.e. \p bNew is \p true) then \p item and \p val arguments
            refer to the same thing.

            The functor may change non-key fields of the \p item; however, \p func must guarantee
            that during changing no any other modifications could be made on this item by concurrent threads.

            Returns <tt> std::pair<bool, bool>  </tt> where \p first is \p true if operation is successful,
            \p second is \p true if new item has been added or \p false if the item with \p key
            already is in the list.
        */
        template <typename Func>
        std::pair<bool, bool> update( value_type& val, Func func, bool bAllowInsert = true )
        {
            return update_at( m_pHead, val, func, bAllowInsert );
        }
        //@cond
        template <typename Func>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<bool, bool> ensure( value_type& val, Func func )
        {
            return update( val, func );
        }
        //@endcond

        /// Finds the key \p val
        /** \anchor cds_intrusive_MichaelList_nogc_find_func
            The function searches the item with key equal to \p key
            and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q& key );
            };
            \endcode
            where \p item is the item found, \p key is the <tt>find</tt> function argument.

            The functor can change non-key fields of \p item.
            The function \p find does not serialize simultaneous access to the list \p item. If such access is
            possible you must provide your own synchronization schema to exclude unsafe item modifications.

            The function returns \p true if \p val is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q& key, Func f )
        {
            return find_at( m_pHead, key, key_comparator(), f );
        }
        //@cond
        template <typename Q, typename Func>
        bool find( Q const& key, Func f )
        {
            return find_at( m_pHead, key, key_comparator(), f );
        }
        //@endcond

        /// Finds the key \p key using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_intrusive_MichaelList_nogc_find_func "find(Q&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return find_at( m_pHead, key, cds::opt::details::make_comparator_from_less<Less>(), f );
        }
        //@cond
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return find_at( m_pHead, key, cds::opt::details::make_comparator_from_less<Less>(), f );
        }
        //@endcond

        /// Checks whether the list contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.
        */
        template <typename Q>
        value_type * contains( Q const& key )
        {
            return find_at( m_pHead, key, key_comparator());
        }
        //@cond
        template <typename Q>
        CDS_DEPRECATED("deprecated, use contains()")
        value_type * find( Q const& key )
        {
            return contains( key );
        }
        //@endcond

        /// Checks whether the map contains \p key using \p pred predicate for searching
        /**
            The function is an analog of <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        value_type * contains( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return find_at( m_pHead, key, cds::opt::details::make_comparator_from_less<Less>());
        }
        //@cond
        template <typename Q, typename Less>
        CDS_DEPRECATED("deprecated, use contains()")
        value_type * find_with( Q const& key, Less pred )
        {
            return contains( key, pred );
        }
        //@endcond

        /// Clears the list
        /**
            The function unlink all items from the list.

            For each unlinked item the item disposer \p disp is called after unlinking.
        */
        template <typename Disposer>
        void clear( Disposer disp )
        {
            node_type * pHead = m_pHead.load(memory_model::memory_order_relaxed);
            do {} while ( cds_unlikely( !m_pHead.compare_exchange_weak( pHead, nullptr, memory_model::memory_order_relaxed )));

            while ( pHead ) {
                node_type * p = pHead->m_pNext.load(memory_model::memory_order_relaxed);
                dispose_node( pHead, disp );
                pHead = p;
                --m_ItemCounter;
            }
        }

        /// Clears the list using default disposer
        /**
            The function clears the list using default (provided in class template) disposer functor.
        */
        void clear()
        {
            clear( disposer());
        }

        /// Checks if the list is empty
        bool empty() const
        {
            return m_pHead.load( memory_model::memory_order_relaxed ) == nullptr;
        }

        /// Returns list's item count
        /**
            The value returned depends on item counter provided by \p Traits. For \p atomicity::empty_item_counter,
            this function always returns 0.

            @note Even if you use real item counter and it returns 0, this fact does not mean that the list
            is empty. To check list emptyness use \p empty() method.
        */
        size_t size() const
        {
            return m_ItemCounter.value();
        }

        /// Returns const reference to internal statistics
        stat const& statistics() const
        {
            return m_Stat;
        }

    protected:
        //@cond
        // split-list support
        bool insert_aux_node( node_type * pNode )
        {
            return insert_aux_node( m_pHead, pNode );
        }

        // split-list support
        bool insert_aux_node( atomic_node_ptr& refHead, node_type * pNode )
        {
            assert( pNode != nullptr );

            // Hack: convert node_type to value_type.
            // In principle, auxiliary node can be non-reducible to value_type
            // We assume that comparator can correctly distinguish aux and regular node.
            return insert_at( refHead, *node_traits::to_value_ptr( pNode ));
        }

        bool insert_at( atomic_node_ptr& refHead, value_type& val )
        {
            position pos;

            while ( true ) {
                if ( search( refHead, val, key_comparator(), pos )) {
                    m_Stat.onInsertFailed();
                    return false;
                }

                if ( link_node( node_traits::to_node_ptr( val ), pos )) {
                    ++m_ItemCounter;
                    m_Stat.onInsertSuccess();
                    return true;
                }

                m_Stat.onInsertRetry();
            }
        }

        iterator insert_at_( atomic_node_ptr& refHead, value_type& val )
        {
            if ( insert_at( refHead, val ))
                return iterator( node_traits::to_node_ptr( val ));
            return end();
        }

        template <typename Func>
        std::pair<iterator, bool> update_at_( atomic_node_ptr& refHead, value_type& val, Func func, bool bAllowInsert )
        {
            position pos;

            while ( true ) {
                if ( search( refHead, val, key_comparator(), pos )) {
                    assert( key_comparator()( val, *node_traits::to_value_ptr( *pos.pCur )) == 0 );

                    func( false, *node_traits::to_value_ptr( *pos.pCur ) , val );
                    m_Stat.onUpdateExisting();
                    return std::make_pair( iterator( pos.pCur ), false );
                }
                else {
                    if ( !bAllowInsert ) {
                        m_Stat.onUpdateFailed();
                        return std::make_pair( end(), false );
                    }

                    if ( link_node( node_traits::to_node_ptr( val ), pos )) {
                        ++m_ItemCounter;
                        func( true, val , val );
                        m_Stat.onUpdateNew();
                        return std::make_pair( iterator( node_traits::to_node_ptr( val )), true );
                    }
                }

                m_Stat.onUpdateRetry();
            }
        }

        template <typename Func>
        std::pair<bool, bool> update_at( atomic_node_ptr& refHead, value_type& val, Func func, bool bAllowInsert )
        {
            std::pair<iterator, bool> ret = update_at_( refHead, val, func, bAllowInsert );
            return std::make_pair( ret.first != end(), ret.second );
        }

        template <typename Q, typename Compare, typename Func>
        bool find_at( atomic_node_ptr& refHead, Q& val, Compare cmp, Func f )
        {
            position pos;

            if ( search( refHead, val, cmp, pos )) {
                assert( pos.pCur != nullptr );
                f( *node_traits::to_value_ptr( *pos.pCur ), val );
                m_Stat.onFindSuccess();
                return true;
            }

            m_Stat.onFindFailed();
            return false;
        }

        template <typename Q, typename Compare>
        value_type * find_at( atomic_node_ptr& refHead, Q const& val, Compare cmp )
        {
            iterator it = find_at_( refHead, val, cmp );
            if ( it != end()) {
                m_Stat.onFindSuccess();
                return &*it;
            }

            m_Stat.onFindFailed();
            return nullptr;
        }

        template <typename Q, typename Compare>
        iterator find_at_( atomic_node_ptr& refHead, Q const& val, Compare cmp )
        {
            position pos;

            if ( search( refHead, val, cmp, pos )) {
                assert( pos.pCur != nullptr );
                m_Stat.onFindSuccess();
                return iterator( pos.pCur );
            }

            m_Stat.onFindFailed();
            return end();
        }

        //@endcond

    protected:

        //@cond
        template <typename Q, typename Compare >
        bool search( atomic_node_ptr& refHead, const Q& val, Compare cmp, position& pos )
        {
            atomic_node_ptr * pPrev;
            node_type * pNext;
            node_type * pCur;

            back_off        bkoff;

        try_again:
            pPrev = &refHead;
            pCur = pPrev->load(memory_model::memory_order_acquire);
            pNext = nullptr;

            while ( true ) {
                if ( !pCur ) {
                    pos.pPrev = pPrev;
                    pos.pCur = pCur;
                    pos.pNext = pNext;
                    return false;
                }

                pNext = pCur->m_pNext.load(memory_model::memory_order_relaxed);
                if ( cds_unlikely( pCur->m_pNext.load(memory_model::memory_order_acquire) != pNext )) {
                    bkoff();
                    goto try_again;
                }

                if ( cds_unlikely( pPrev->load(memory_model::memory_order_acquire) != pCur )) {
                    bkoff();
                    goto try_again;
                }

                assert( pCur != nullptr );
                int nCmp = cmp( *node_traits::to_value_ptr( *pCur ), val );
                if ( nCmp >= 0 ) {
                    pos.pPrev = pPrev;
                    pos.pCur = pCur;
                    pos.pNext = pNext;
                    return nCmp == 0;
                }
                pPrev = &( pCur->m_pNext );
                pCur = pNext;
            }
        }

        // for split-list
        template <typename Predicate>
        void erase_for( Predicate pred )
        {
            node_type * pPred = nullptr;
            node_type * pHead = m_pHead.load( memory_model::memory_order_relaxed );
            while ( pHead ) {
                node_type * p = pHead->m_pNext.load( memory_model::memory_order_relaxed );
                if ( pred( *node_traits::to_value_ptr( pHead ))) {
                    assert( pPred != nullptr );
                    pPred->m_pNext.store( p, memory_model::memory_order_relaxed );
                    dispose_node( pHead, disposer());
                }
                else
                    pPred = pHead;
                pHead = p;
            }
        }
        //@endcond
    };

}}  // namespace cds::intrusive

#endif  // #ifndef CDSLIB_INTRUSIVE_MICHAEL_LIST_NOGC_H
