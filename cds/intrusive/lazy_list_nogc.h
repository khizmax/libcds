// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_LAZY_LIST_NOGC_H
#define CDSLIB_INTRUSIVE_LAZY_LIST_NOGC_H

#include <mutex>        // unique_lock
#include <cds/intrusive/details/lazy_list_base.h>
#include <cds/gc/nogc.h>

namespace cds { namespace intrusive {
    namespace lazy_list {
        /// Lazy list node for \p gc::nogc
        /**
            Template parameters:
             - Lock - lock type. Default is \p cds::sync::spin
             - Tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template <
#ifdef CDS_DOXYGEN_INVOKED
            typename Lock = cds::sync::spin,
            typename Tag = opt::none
#else
            typename Lock,
            typename Tag
#endif
        >
        struct node<gc::nogc, Lock, Tag>
        {
            typedef gc::nogc    gc;   ///< Garbage collector
            typedef Lock        lock_type;  ///< Lock type
            typedef Tag         tag;  ///< tag

            atomics::atomic<node *> m_pNext; ///< pointer to the next node in the list
            mutable lock_type   m_Lock;      ///< Node lock

            node()
                : m_pNext( nullptr )
            {}
        };
    }   // namespace lazy_list


    /// Lazy single-linked list (template specialization for \p gc::nogc)
    /** @ingroup cds_intrusive_list
        \anchor cds_intrusive_LazyList_nogc

        This specialization is append-only list when no item
        reclamation may be performed. The class does not support deleting of list item.

        The list can be ordered if \p Traits::sort is \p true that is default
        or unordered otherwise. Unordered list can be maintained by \p equal_to
        relationship (\p Traits::equal_to), but for the ordered list \p less
        or \p compare relations should be specified in \p Traits.

        See \ref cds_intrusive_LazyList_hp "LazyList" for description of template parameters.
    */
    template <
        typename T
#ifdef CDS_DOXYGEN_INVOKED
        ,class Traits = lazy_list::traits
#else
        ,class Traits
#endif
    >
    class LazyList<gc::nogc, T, Traits>
    {
    public:
        typedef gc::nogc gc;         ///< Garbage collector
        typedef T        value_type; ///< type of value stored in the list
        typedef Traits   traits;    ///< Traits template parameter

        typedef typename traits::hook    hook;      ///< hook type
        typedef typename hook::node_type node_type; ///< node type
        static constexpr bool const c_bSort = traits::sort; ///< List type: ordered (\p true) or unordered (\p false)

#   ifdef CDS_DOXYGEN_INVOKED
        /// Key comparing functor
        /**
            - for ordered list, the functor is based on \p traits::compare or \p traits::less
            - for unordered list, the functor is based on \p traits::equal_to, \p traits::compare or \p traits::less
        */
        typedef implementation_defined key_comparator;
#   else
        typedef typename std::conditional< c_bSort,
            typename opt::details::make_comparator< value_type, traits >::type,
            typename opt::details::make_equal_to< value_type, traits >::type
        >::type key_comparator;
#   endif
        typedef typename traits::back_off  back_off;   ///< Back-off strategy
        typedef typename traits::disposer  disposer;   ///< disposer
        typedef typename get_node_traits< value_type, node_type, hook>::type node_traits;    ///< node traits
        typedef typename lazy_list::get_link_checker< node_type, traits::link_checker >::type link_checker;   ///< link checker

        typedef typename traits::item_counter item_counter; ///< Item counting policy used
        typedef typename traits::memory_model memory_model; ///< C++ memory ordering (see \p lazy_list::traits::memory_model)
        typedef typename traits::stat         stat;         ///< Internal statistics

        //@cond
        static_assert((std::is_same< gc, typename node_type::gc >::value), "GC and node_type::gc must be the same type");

        // Rebind traits (split-list support)
        template <typename... Options>
        struct rebind_traits {
            typedef LazyList<
                gc
                , value_type
                , typename cds::opt::make_options< traits, Options...>::type
            >   type;
        };

        // Stat selector
        template <typename Stat>
        using select_stat_wrapper = lazy_list::select_stat_wrapper< Stat >;
        //@endcond

    protected:
        typedef node_type *     auxiliary_head   ;   ///< Auxiliary head type (for split-list support)

    protected:
        node_type       m_Head;        ///< List head (dummy node)
        node_type       m_Tail;        ///< List tail (dummy node)
        item_counter    m_ItemCounter; ///< Item counter
        mutable stat    m_Stat;        ///< Internal statistics

        //@cond

        /// Position pointer for item search
        struct position {
            node_type *     pPred   ;    ///< Previous node
            node_type *     pCur    ;    ///< Current node

            /// Locks nodes \p pPred and \p pCur
            void lock()
            {
                pPred->m_Lock.lock();
                pCur->m_Lock.lock();
            }

            /// Unlocks nodes \p pPred and \p pCur
            void unlock()
            {
                pCur->m_Lock.unlock();
                pPred->m_Lock.unlock();
            }
        };

        class auto_lock_position {
            position&   m_pos;
        public:
            auto_lock_position( position& pos )
                : m_pos(pos)
            {
                pos.lock();
            }
            ~auto_lock_position()
            {
                m_pos.unlock();
            }
        };
        //@endcond

    protected:
        //@cond
        void clear_links( node_type * pNode )
        {
            pNode->m_pNext.store( nullptr, memory_model::memory_order_relaxed );
        }

        template <class Disposer>
        void dispose_node( node_type * pNode, Disposer disp )
        {
            clear_links( pNode );
            disp( node_traits::to_value_ptr( *pNode ));
        }

        template <class Disposer>
        void dispose_value( value_type& val, Disposer disp )
        {
            dispose_node( node_traits::to_node_ptr( val ), disp );
        }

        void link_node( node_type * pNode, node_type * pPred, node_type * pCur )
        {
            link_checker::is_empty( pNode );
            assert( pPred->m_pNext.load(memory_model::memory_order_relaxed) == pCur );

            pNode->m_pNext.store( pCur, memory_model::memory_order_release );
            pPred->m_pNext.store( pNode, memory_model::memory_order_release );
        }
        //@endcond

    protected:
        //@cond
        template <bool IsConst>
        class iterator_type
        {
            friend class LazyList;

        protected:
            value_type * m_pNode;

            void next()
            {
                assert( m_pNode != nullptr );

                node_type * pNode = node_traits::to_node_ptr( m_pNode );
                node_type * pNext = pNode->m_pNext.load(memory_model::memory_order_relaxed);
                if ( pNext != nullptr )
                    m_pNode = node_traits::to_value_ptr( pNext );
            }

            iterator_type( node_type * pNode )
            {
                m_pNode = node_traits::to_value_ptr( pNode );
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
            iterator it( &m_Head );
            ++it        ;   // skip dummy head
            return it;
        }

        /// Returns an iterator that addresses the location succeeding the last element in a list
        /**
            Do not use the value returned by <tt>end</tt> function to access any item.

            The returned value can be used only to control reaching the end of the list.
            For empty list \code begin() == end() \endcode
        */
        iterator end()
        {
            return iterator( &m_Tail );
        }

        /// Returns a forward const iterator addressing the first element in a list
        const_iterator begin() const
        {
            return cbegin();
        }
        /// Returns a forward const iterator addressing the first element in a list
        const_iterator cbegin() const
        {
            const_iterator it( const_cast<node_type *>(&m_Head));
            ++it;   // skip dummy head
            return it;
        }

        /// Returns an const iterator that addresses the location succeeding the last element in a list
        const_iterator end() const
        {
            return cend();
        }
        /// Returns an const iterator that addresses the location succeeding the last element in a list
        const_iterator cend() const
        {
            return const_iterator( const_cast<node_type *>(&m_Tail));
        }

    public:
        /// Default constructor initializes empty list
        LazyList()
        {
            m_Head.m_pNext.store( &m_Tail, memory_model::memory_order_relaxed );
        }

        //@cond
        template <typename Stat, typename = std::enable_if<std::is_same<stat, lazy_list::wrapped_stat<Stat>>::value >>
        explicit LazyList( Stat& st )
            : m_Stat( st )
        {
            m_Head.m_pNext.store( &m_Tail, memory_model::memory_order_relaxed );
        }
        //@endcond

        /// Destroys the list object
        ~LazyList()
        {
            clear();
            assert( m_Head.m_pNext.load(memory_model::memory_order_relaxed) == &m_Tail );
            m_Head.m_pNext.store( nullptr, memory_model::memory_order_relaxed );
        }

        /// Inserts new node
        /**
            The function inserts \p val in the list if the list does not contain
            an item with key equal to \p val.

            Returns \p true if \p val is linked into the list, \p false otherwise.
        */
        bool insert( value_type& val )
        {
            return insert_at( &m_Head, val );
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

            The functor may change non-key fields of the \p item.
            While the functor \p f is calling the item \p item is locked.

            Returns <tt> std::pair<bool, bool>  </tt> where \p first is \p true if operation is successful,
            \p second is \p true if new item has been added or \p false if the item with \p key
            already is in the list.
        */
        template <typename Func>
        std::pair<bool, bool> update( value_type& val, Func func, bool bAllowInsert = true )
        {
            return update_at( &m_Head, val, func, bAllowInsert );
        }
        //@cond
        template <typename Func>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<bool, bool> ensure( value_type& val, Func func )
        {
            return update( val, func, true );
        }
        //@endcond

        /// Finds the key \p key
        /** \anchor cds_intrusive_LazyList_nogc_find_func
            The function searches the item with key equal to \p key
            and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q& key );
            };
            \endcode
            where \p item is the item found, \p key is the <tt>find</tt> function argument.

            The functor may change non-key fields of \p item.
            While the functor \p f is calling the item found \p item is locked.

            The function returns \p true if \p key is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q& key, Func f )
        {
            return find_at( &m_Head, key, key_comparator(), f );
        }
        //@cond
        template <typename Q, typename Func>
        bool find( Q const& key, Func f )
        {
            return find_at( &m_Head, key, key_comparator(), f );
        }
        //@endcond

        /// Finds the key \p key using \p less predicate for searching. Disabled for unordered lists.
        /**
            The function is an analog of \ref cds_intrusive_LazyList_nogc_find_func "find(Q&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less, typename Func, bool Sort = c_bSort>
        typename std::enable_if<Sort, bool>::type find_with( Q& key, Less less, Func f )
        {
            CDS_UNUSED( less );
            return find_at( &m_Head, key, cds::opt::details::make_comparator_from_less<Less>(), f );
        }

        /// Finds the key \p key using \p equal predicate for searching. Disabled for ordered lists.
        /**
            The function is an analog of \ref cds_intrusive_LazyList_nogc_find_func "find(Q&, Func)"
            but \p equal is used for key comparing.
            \p Equal functor has the interface like \p std::equal_to.
        */
        template <typename Q, typename Equal, typename Func, bool Sort = c_bSort>
        typename std::enable_if<!Sort, bool>::type find_with( Q& key, Equal eq, Func f )
        {
            //CDS_UNUSED( eq );
            return find_at( &m_Head, key, eq, f );
        }
        //@cond
        template <typename Q, typename Less, typename Func, bool Sort = c_bSort>
        typename std::enable_if<Sort, bool>::type find_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return find_at( &m_Head, key, cds::opt::details::make_comparator_from_less<Less>(), f );
        }

        template <typename Q, typename Equal, typename Func, bool Sort = c_bSort>
        typename std::enable_if<!Sort, bool>::type find_with( Q const& key, Equal eq, Func f )
        {
            //CDS_UNUSED( eq );
            return find_at( &m_Head, key, eq, f );
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
            return find_at( &m_Head, key, key_comparator());
        }
        //@cond
        template <typename Q>
        CDS_DEPRECATED("deprecated, use contains()")
        value_type * find( Q const& key )
        {
            return contains( key );
        }
        //@endcond

        /// Checks whether the map contains \p key using \p pred predicate for searching (ordered list version)
        /**
            The function is an analog of <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less, bool Sort = c_bSort>
        typename std::enable_if<Sort, value_type *>::type contains( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return find_at( &m_Head, key, cds::opt::details::make_comparator_from_less<Less>());
        }
        //@cond
        template <typename Q, typename Less, bool Sort = c_bSort>
        CDS_DEPRECATED("deprecated, use contains()")
        typename std::enable_if<Sort, value_type *>::type find_with( Q const& key, Less pred )
        {
            return contains( key, pred );
        }
        //@endcond

        /// Checks whether the map contains \p key using \p equal predicate for searching (unordered list version)
        /**
            The function is an analog of <tt>contains( key )</tt> but \p equal is used for key comparing.
            \p Equal functor has the interface like \p std::equal_to.
        */
        template <typename Q, typename Equal, bool Sort = c_bSort>
        typename std::enable_if<!Sort, value_type *>::type contains( Q const& key, Equal eq )
        {
            return find_at( &m_Head, key, eq );
        }
        //@cond
        template <typename Q, typename Equal, bool Sort = c_bSort>
        CDS_DEPRECATED("deprecated, use contains()")
        typename std::enable_if<!Sort, value_type *>::type find_with( Q const& key, Equal eq )
        {
            return contains( key, eq );
        }
        //@endcond

        /// Clears the list
        /**
            The function unlink all items from the list.
            For each unlinked item the item disposer \p disp is called after unlinking.

            This function is not thread-safe.
        */
        template <typename Disposer>
        void clear( Disposer disp )
        {
            node_type * pHead = m_Head.m_pNext.exchange( &m_Tail, memory_model::memory_order_release );

            while ( pHead != &m_Tail ) {
                node_type * p = pHead->m_pNext.load(memory_model::memory_order_relaxed);
                dispose_node( pHead, disp );
                --m_ItemCounter;
                pHead = p;
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
            return m_Head.m_pNext.load(memory_model::memory_order_relaxed) == &m_Tail;
        }

        /// Returns list's item count
        /**
            The value returned depends on opt::item_counter option. For atomicity::empty_item_counter,
            this function always returns 0.

            <b>Warning</b>: even if you use real item counter and it returns 0, this fact is not mean that the list
            is empty. To check list emptyness use \ref empty() method.
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
            return insert_aux_node( &m_Head, pNode );
        }

        // split-list support
        bool insert_aux_node( node_type * pHead, node_type * pNode )
        {
            assert( pHead != nullptr );
            assert( pNode != nullptr );

            // Hack: convert node_type to value_type.
            // In principle, auxiliary node can be non-reducible to value_type
            // We assume that comparator can correctly distinguish aux and regular node.
            return insert_at( pHead, *node_traits::to_value_ptr( pNode ));
        }

        bool insert_at( node_type * pHead, value_type& val )
        {
            position pos;
            key_comparator pred;

            while ( true ) {
                search( pHead, val, pos, pred );
                {
                    auto_lock_position alp( pos );
                    if ( validate( pos.pPred, pos.pCur )) {
                        if ( pos.pCur != &m_Tail && equal( *node_traits::to_value_ptr( *pos.pCur ), val, pred )) {
                            // failed: key already in list
                            m_Stat.onInsertFailed();
                            return false;
                        }
                        else {
                            link_node( node_traits::to_node_ptr( val ), pos.pPred, pos.pCur );
                            break;
                        }
                    }
                }

                m_Stat.onInsertRetry();
            }

            ++m_ItemCounter;
            m_Stat.onInsertSuccess();
            return true;
        }

        iterator insert_at_( node_type * pHead, value_type& val )
        {
            if ( insert_at( pHead, val ))
                return iterator( node_traits::to_node_ptr( val ));
            return end();
        }


        template <typename Func>
        std::pair<iterator, bool> update_at_( node_type * pHead, value_type& val, Func func, bool bAllowInsert )
        {
            position pos;
            key_comparator pred;

            while ( true ) {
                search( pHead, val, pos, pred );
                {
                    auto_lock_position alp( pos );
                    if ( validate( pos.pPred, pos.pCur )) {
                        if ( pos.pCur != &m_Tail && equal( *node_traits::to_value_ptr( *pos.pCur ), val, pred )) {
                            // key already in the list

                            func( false, *node_traits::to_value_ptr( *pos.pCur ) , val );
                            m_Stat.onUpdateExisting();
                            return std::make_pair( iterator( pos.pCur ), false );
                        }
                        else {
                            // new key
                            if ( !bAllowInsert ) {
                                m_Stat.onUpdateFailed();
                                return std::make_pair( end(), false );
                            }

                            link_node( node_traits::to_node_ptr( val ), pos.pPred, pos.pCur );
                            func( true, val, val );
                            break;
                        }
                    }

                    m_Stat.onUpdateRetry();
                }
            }

            ++m_ItemCounter;
            m_Stat.onUpdateNew();
            return std::make_pair( iterator( node_traits::to_node_ptr( val )), true );
        }

        template <typename Func>
        std::pair<bool, bool> update_at( node_type * pHead, value_type& val, Func func, bool bAllowInsert )
        {
            std::pair<iterator, bool> ret = update_at_( pHead, val, func, bAllowInsert );
            return std::make_pair( ret.first != end(), ret.second );
        }

        template <typename Q, typename Pred, typename Func>
        bool find_at( node_type * pHead, Q& val, Pred pred, Func f )
        {
            position pos;

            search( pHead, val, pos, pred );
            if ( pos.pCur != &m_Tail ) {
                std::unique_lock< typename node_type::lock_type> al( pos.pCur->m_Lock );
                if ( equal( *node_traits::to_value_ptr( *pos.pCur ), val, pred ))
                {
                    f( *node_traits::to_value_ptr( *pos.pCur ), val );
                    m_Stat.onFindSuccess();
                    return true;
                }
            }

            m_Stat.onFindFailed();
            return false;
        }

        template <typename Q, typename Pred>
        value_type * find_at( node_type * pHead, Q& val, Pred pred)
        {
            iterator it = find_at_( pHead, val, pred );
            if ( it != end())
                return &*it;
            return nullptr;
        }

        template <typename Q, typename Pred>
        iterator find_at_( node_type * pHead, Q& val, Pred pred)
        {
            position pos;

            search( pHead, val, pos, pred );
            if ( pos.pCur != &m_Tail ) {
                if ( equal( *node_traits::to_value_ptr( *pos.pCur ), val, pred )) {
                    m_Stat.onFindSuccess();
                    return iterator( pos.pCur );
                }
            }

            m_Stat.onFindFailed();
            return end();
        }

        //@endcond

    protected:
        //@cond
        template <typename Q, typename Equal, bool Sort = c_bSort>
        typename std::enable_if<!Sort, void>::type search( node_type * pHead, const Q& key, position& pos, Equal eq )
        {
            const node_type * pTail = &m_Tail;

            node_type * pCur = pHead;
            node_type * pPrev = pHead;

            while ( pCur != pTail && ( pCur == pHead || !equal( *node_traits::to_value_ptr( *pCur ), key, eq ))) {
                pPrev = pCur;
                pCur = pCur->m_pNext.load(memory_model::memory_order_acquire);
            }

            pos.pCur = pCur;
            pos.pPred = pPrev;
        }

        template <typename Q, typename Compare, bool Sort = c_bSort>
        typename std::enable_if<Sort, void>::type search( node_type * pHead, const Q& key, position& pos, Compare cmp )
        {
            const node_type * pTail = &m_Tail;

            node_type * pCur = pHead;
            node_type * pPrev = pHead;

            while ( pCur != pTail && ( pCur == pHead || cmp( *node_traits::to_value_ptr( *pCur ), key ) < 0 )) {
                pPrev = pCur;
                pCur = pCur->m_pNext.load(memory_model::memory_order_acquire);
            }

            pos.pCur = pCur;
            pos.pPred = pPrev;
        }

        template <typename L, typename R, typename Equal, bool Sort = c_bSort>
        static typename std::enable_if<!Sort, bool>::type equal( L const& l, R const& r, Equal eq )
        {
            return eq(l, r);
        }

        template <typename L, typename R, typename Compare, bool Sort = c_bSort>
        static typename std::enable_if<Sort, bool>::type equal( L const& l, R const& r, Compare cmp )
        {
            return cmp(l, r) == 0;
        }

        bool validate( node_type * pPred, node_type * pCur )
        {
            if ( pPred->m_pNext.load(memory_model::memory_order_acquire) == pCur ) {
                m_Stat.onValidationSuccess();
                return true;
            }

            m_Stat.onValidationFailed();
            return false;
        }

        // for split-list
        template <typename Predicate>
        void erase_for( Predicate pred )
        {
            node_type * pPred = nullptr;
            node_type * pHead = m_Head.m_pNext.load( memory_model::memory_order_relaxed );

            while ( pHead != &m_Tail ) {
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

#endif  // #ifndef CDSLIB_INTRUSIVE_LAZY_LIST_NOGC_H
