// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_IMPL_ITERABLE_LIST_H
#define CDSLIB_INTRUSIVE_IMPL_ITERABLE_LIST_H

#include <cds/intrusive/details/iterable_list_base.h>
#include <cds/details/make_const_type.h>

namespace cds { namespace intrusive {

    /// Iterable lock-free ordered single-linked list
    /** @ingroup cds_intrusive_list
        \anchor cds_intrusive_IterableList_hp

        This non-blocking list implementation supports thread-safe iterators;
        searching and removing are lock-free, inserting is non-blocking because it
        uses a light-weight synchronization based on marked pointers.

        Unlike \p cds::intrusive::MichaelList the iterable list does not require
        any hook in \p T to be stored in the list.

        Usually, ordered single-linked list is used as a building block for the hash table implementation.
        Iterable list is suitable for almost append-only hash table because the list doesn't delete
        its internal node when erasing a key but it is marked them as empty to be reused in the future.
        However, plenty of empty nodes degrades performance.
        Separation of internal nodes and user data implies the need for an allocator for internal node
        so the iterable list is not fully intrusive. Nevertheless, if you need thread-safe iterator,
        the iterable list is good choice.

        The complexity of searching is <tt>O(N)</tt>.

        Template arguments:
        - \p GC - Garbage collector used.
        - \p T - type to be stored in the list.
        - \p Traits - type traits, default is \p iterable_list::traits. It is possible to declare option-based
             list with \p cds::intrusive::iterable_list::make_traits metafunction:
            For example, the following traits-based declaration of \p gc::HP iterable list
            \code
            #include <cds/intrusive/iterable_list_hp.h>
            // Declare item stored in your list
            struct foo
            {
                int nKey;
                // .... other data
            };

            // Declare comparator for the item
            struct my_compare {
                int operator()( foo const& i1, foo const& i2 ) const
                {
                    return i1.nKey - i2.nKey;
                }
            };

            // Declare traits
            struct my_traits: public cds::intrusive::iterable_list::traits
            {
                typedef my_compare compare;
            };

            // Declare list
            typedef cds::intrusive::IterableList< cds::gc::HP, foo, my_traits > list_type;
            \endcode
            is equivalent for the following option-based list
            \code
            #include <cds/intrusive/iterable_list_hp.h>

            // foo struct and my_compare are the same

            // Declare option-based list
            typedef cds::intrusive::IterableList< cds::gc::HP, foo,
                typename cds::intrusive::iterable_list::make_traits<
                    cds::intrusive::opt::compare< my_compare >     // item comparator option
                >::type
            > option_list_type;
            \endcode

        \par Usage
        There are different specializations of this template for each garbage collecting schema.
        You should select GC you want and include appropriate .h-file:
        - for \p gc::HP: <tt> <cds/intrusive/iterable_list_hp.h> </tt>
        - for \p gc::DHP: <tt> <cds/intrusive/iterable_list_dhp.h> </tt>
    */
    template <
        class GC
        ,typename T
#ifdef CDS_DOXYGEN_INVOKED
        ,class Traits = iterable_list::traits
#else
        ,class Traits
#endif
    >
    class IterableList
#ifndef CDS_DOXYGEN_INVOKED
        : public iterable_list_tag
#endif
    {
    public:
        typedef T       value_type; ///< type of value stored in the list
        typedef Traits  traits;     ///< Traits template parameter

        typedef iterable_list::node< value_type > node_type; ///< node type

#   ifdef CDS_DOXYGEN_INVOKED
        typedef implementation_defined key_comparator  ;    ///< key comparison functor based on opt::compare and opt::less option setter.
#   else
        typedef typename opt::details::make_comparator< value_type, traits >::type key_comparator;
#   endif

        typedef typename traits::disposer  disposer; ///< disposer for \p value_type

        typedef GC  gc;   ///< Garbage collector
        typedef typename traits::back_off       back_off;       ///< back-off strategy
        typedef typename traits::item_counter   item_counter;   ///< Item counting policy used
        typedef typename traits::memory_model   memory_model;   ///< Memory ordering. See \p cds::opt::memory_model option
        typedef typename traits::node_allocator node_allocator; ///< Node allocator
        typedef typename traits::stat           stat;           ///< Internal statistics

        typedef typename gc::template guarded_ptr< value_type > guarded_ptr; ///< Guarded pointer

        static constexpr const size_t c_nHazardPtrCount = 4; ///< Count of hazard pointer required for the algorithm

        //@cond
        // Rebind traits (split-list support)
        template <typename... Options>
        struct rebind_traits {
            typedef IterableList<
                gc
                , value_type
                , typename cds::opt::make_options< traits, Options...>::type
            > type;
        };

        // Stat selector
        template <typename Stat>
        using select_stat_wrapper = iterable_list::select_stat_wrapper< Stat >;
        //@endcond

    protected:
        //@cond
        typedef atomics::atomic< node_type* > atomic_node_ptr;  ///< Atomic node pointer
        typedef atomic_node_ptr               auxiliary_head;   ///< Auxiliary head type (for split-list support)
        typedef typename node_type::marked_data_ptr marked_data_ptr;

        node_type       m_Head;
        node_type       m_Tail;

        item_counter    m_ItemCounter;  ///< Item counter
        mutable stat    m_Stat;         ///< Internal statistics

        typedef cds::details::Allocator< node_type, node_allocator > cxx_node_allocator;

        /// Position pointer for item search
        struct position {
            node_type const*  pHead;
            node_type *       pPrev;  ///< Previous node
            node_type *       pCur;   ///< Current node

            value_type *      pFound;       ///< Value of \p pCur->data, valid only if data found

            typename gc::Guard guard;       ///< guard for \p pFound
        };

        struct insert_position: public position
        {
            value_type *        pPrevVal;     ///< Value of \p pPrev->data, can be \p nullptr
            typename gc::Guard  prevGuard;   ///< guard for \p pPrevVal
        };
        //@endcond

    protected:
        //@cond
        template <bool IsConst>
        class iterator_type
        {
            friend class IterableList;

        protected:
            node_type*    m_pNode;
            typename gc::Guard  m_Guard; // data guard

            void next()
            {
                for ( node_type* p = m_pNode->next.load( memory_model::memory_order_relaxed ); p != m_pNode; p = p->next.load( memory_model::memory_order_relaxed ))
                {
                    m_pNode = p;
                    if ( m_Guard.protect( p->data, []( marked_data_ptr ptr ) { return ptr.ptr(); }).ptr())
                        return;
                }
                m_Guard.clear();
            }

            explicit iterator_type( node_type* pNode )
                : m_pNode( pNode )
            {
                if ( !m_Guard.protect( pNode->data, []( marked_data_ptr p ) { return p.ptr(); }).ptr())
                    next();
            }

            iterator_type( node_type* pNode, value_type* pVal )
                : m_pNode( pNode )
            {
                if ( m_pNode ) {
                    assert( pVal != nullptr );
                    m_Guard.assign( pVal );
                }
            }

            value_type* data() const
            {
                return m_Guard.template get<value_type>();
            }

        public:
            typedef typename cds::details::make_const_type<value_type, IsConst>::pointer   value_ptr;
            typedef typename cds::details::make_const_type<value_type, IsConst>::reference value_ref;

            iterator_type()
                : m_pNode( nullptr )
            {}

            iterator_type( iterator_type const& src )
                : m_pNode( src.m_pNode )
            {
                m_Guard.copy( src.m_Guard );
            }

            value_ptr operator ->() const
            {
                return data();
                //return m_Guard.template get<value_type>();
            }

            value_ref operator *() const
            {
                assert( m_Guard.get_native() != nullptr );
                return *data();
                //return *m_Guard.template get<value_type>();
            }

            /// Pre-increment
            iterator_type& operator ++()
            {
                next();
                return *this;
            }

            iterator_type& operator = (iterator_type const& src)
            {
                m_pNode = src.m_pNode;
                m_Guard.copy( src.m_Guard );
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
                return !( *this == i );
            }
        };
        //@endcond

    public:
    ///@name Thread-safe forward iterators
    //@{
        /// Forward iterator
        /**
            The forward iterator for iterable list has some features:
            - it has no post-increment operator
            - to protect the value, the iterator contains a GC-specific guard.
              For some GC (like as \p gc::HP), a guard is a limited resource per thread, so an exception (or assertion) "no free guard"
              may be thrown if the limit of guard count per thread is exceeded.
            - The iterator cannot be moved across thread boundary since it contains thread-private GC's guard.
            - Iterator is thread-safe: even if the element the iterator points to is removed, the iterator stays valid because
              it contains the guard keeping the value from to be recycled.

            The iterator interface:
            \code
            class iterator {
            public:
                // Default constructor
                iterator();

                // Copy construtor
                iterator( iterator const& src );

                // Dereference operator
                value_type * operator ->() const;

                // Dereference operator
                value_type& operator *() const;

                // Preincrement operator
                iterator& operator ++();

                // Assignment operator
                iterator& operator = (iterator const& src);

                // Equality operators
                bool operator ==(iterator const& i ) const;
                bool operator !=(iterator const& i ) const;
            };
            \endcode

            @note For two iterators pointed to the same element the value can be different;
            this code
            \code
                if ( it1 == it2 )
                    assert( &(*it1) == &(*it2));
            \endcode
            can throw assertion. The point is that the iterator stores the value of element which can be modified later by other thread.
            The guard inside the iterator prevents recycling that value so the iterator's value remains valid even after changing.
            Other iterator may observe modified value of the element.
        */
        typedef iterator_type<false>    iterator;
        /// Const forward iterator
        /**
            For iterator's features and requirements see \ref iterator
        */
        typedef iterator_type<true>     const_iterator;

        /// Returns a forward iterator addressing the first element in a list
        /**
            For empty list \code begin() == end() \endcode
        */
        iterator begin()
        {
            return iterator( &m_Head );
        }

        /// Returns an iterator that addresses the location succeeding the last element in a list
        /**
            Do not use the value returned by <tt>end</tt> function to access any item.
            Internally, <tt>end</tt> returning value equals to \p nullptr.

            The returned value can be used only to control reaching the end of the list.
            For empty list <tt>begin() == end()</tt>
        */
        iterator end()
        {
            return iterator( &m_Tail );
        }

        /// Returns a forward const iterator addressing the first element in a list
        const_iterator cbegin() const
        {
            return const_iterator( const_cast<node_type*>( &m_Head ));
        }

        /// Returns a forward const iterator addressing the first element in a list
        const_iterator begin() const
        {
            return const_iterator( const_cast<node_type*>( &m_Head ));
        }

        /// Returns an const iterator that addresses the location succeeding the last element in a list
        const_iterator end() const
        {
            return const_iterator( const_cast<node_type*>( &m_Tail ));
        }

        /// Returns an const iterator that addresses the location succeeding the last element in a list
        const_iterator cend() const
        {
            return const_iterator( const_cast<node_type*>( &m_Tail ));
        }
    //@}

    public:
        /// Default constructor initializes empty list
        IterableList()
        {
            init_list();
        }

        //@cond
        template <typename Stat, typename = std::enable_if<std::is_same<stat, iterable_list::wrapped_stat<Stat>>::value >>
        explicit IterableList( Stat& st )
            : m_Stat( st )
        {
            init_list();
        }
        //@endcond

        /// Destroys the list object
        ~IterableList()
        {
            destroy();
        }

        /// Inserts new node
        /**
            The function inserts \p val into the list if the list does not contain
            an item with key equal to \p val.

            Returns \p true if \p val has been linked to the list, \p false otherwise.
        */
        bool insert( value_type& val )
        {
            return insert_at( &m_Head, val );
        }

        /// Inserts new node
        /**
            This function is intended for derived non-intrusive containers.

            The function allows to split new item creating into two part:
            - create item with key only
            - insert new item into the list
            - if inserting is success, calls  \p f functor to initialize value-field of \p val.

            The functor signature is:
            \code
                void func( value_type& val );
            \endcode
            where \p val is the item inserted. User-defined functor \p f should guarantee that during changing
            \p val no any other changes could be made on this list's item by concurrent threads.
            The user-defined functor is called only if the inserting is success.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"
        */
        template <typename Func>
        bool insert( value_type& val, Func f )
        {
            return insert_at( &m_Head, val, f );
        }

        /// Updates the node
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the item \p val is not found in the list, then \p val is inserted
            iff \p bInsert is \p true.
            Otherwise, the current element is changed to \p val, the element will be retired later
            by call \p Traits::disposer.
            The functor \p func is called after inserting or replacing, it signature is:
            \code
                void func( value_type& val, value_type * old );
            \endcode
            where
            - \p val - argument \p val passed into the \p %update() function
            - \p old - old value that will be retired. If new item has been inserted then \p old is \p nullptr.

            Returns std::pair<bool, bool> where \p first is \p true if operation is successful,
            \p second is \p true if \p val has been added or \p false if the item with that key
            already in the list.
        */
        template <typename Func>
        std::pair<bool, bool> update( value_type& val, Func func, bool bInsert = true )
        {
            return update_at( &m_Head, val, func, bInsert );
        }

        /// Insert or update
        /**
            The operation performs inserting or updating data with lock-free manner.

            If the item \p val is not found in the list, then \p val is inserted
            iff \p bInsert is \p true.
            Otherwise, the current element is changed to \p val, the old element will be retired later
            by call \p Traits::disposer.

            Returns std::pair<bool, bool> where \p first is \p true if operation is successful,
            \p second is \p true if \p val has been added or \p false if the item with that key
            already in the list.
        */
        std::pair<bool, bool> upsert( value_type& val, bool bInsert = true )
        {
            return upsert_at( &m_Head, val, bInsert );
        }

        /// Unlinks the item \p val from the list
        /**
            The function searches the item \p val in the list and unlinks it from the list
            if it is found and it is equal to \p val.

            Difference between \p erase() and \p %unlink(): \p %erase() finds <i>a key</i>
            and deletes the item found. \p %unlink() finds an item by key and deletes it
            only if \p val is an item of the list, i.e. the pointer to item found
            is equal to <tt> &val </tt>.

            \p disposer specified in \p Traits is called for deleted item.

            The function returns \p true if success and \p false otherwise.
        */
        bool unlink( value_type& val )
        {
            return unlink_at( &m_Head, val );
        }

        /// Deletes the item from the list
        /** \anchor cds_intrusive_IterableList_hp_erase_val
            The function searches an item with key equal to \p key in the list,
            unlinks it from the list, and returns \p true.
            If \p key is not found the function return \p false.

            \p disposer specified in \p Traits is called for deleted item.
        */
        template <typename Q>
        bool erase( Q const& key )
        {
            return erase_at( &m_Head, key, key_comparator());
        }

        /// Deletes the item from the list using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_intrusive_IterableList_hp_erase_val "erase(Q const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.

            \p disposer specified in \p Traits is called for deleted item.
        */
        template <typename Q, typename Less>
        bool erase_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return erase_at( &m_Head, key, cds::opt::details::make_comparator_from_less<Less>());
        }

        /// Deletes the item from the list
        /** \anchor cds_intrusive_IterableList_hp_erase_func
            The function searches an item with key equal to \p key in the list,
            call \p func functor with item found, unlinks it from the list, and returns \p true.
            The \p Func interface is
            \code
            struct functor {
                void operator()( value_type const& item );
            };
            \endcode
            If \p key is not found the function return \p false, \p func is not called.

            \p disposer specified in \p Traits is called for deleted item.
        */
        template <typename Q, typename Func>
        bool erase( Q const& key, Func func )
        {
            return erase_at( &m_Head, key, key_comparator(), func );
        }

        /// Deletes the item from the list using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_intrusive_IterableList_hp_erase_func "erase(Q const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.

            \p disposer specified in \p Traits is called for deleted item.
        */
        template <typename Q, typename Less, typename Func>
        bool erase_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return erase_at( &m_Head, key, cds::opt::details::make_comparator_from_less<Less>(), f );
        }

        /// Deletes the item pointed by iterator \p iter
        /**
            Returns \p true if the operation is successful, \p false otherwise.
            The function can return \p false if the node the iterator points to has already been deleted
            by other thread.

            The function does not invalidate the iterator, it remains valid and can be used for further traversing.
        */
        bool erase_at( iterator const& iter )
        {
            assert( iter != end());

            marked_data_ptr val( iter.data());
            if ( iter.m_pNode->data.compare_exchange_strong( val, marked_data_ptr(), memory_model::memory_order_acquire, atomics::memory_order_relaxed )) {
                --m_ItemCounter;
                retire_data( val.ptr());
                m_Stat.onEraseSuccess();
                return true;
            }
            return false;
        }

        /// Extracts the item from the list with specified \p key
        /** \anchor cds_intrusive_IterableList_hp_extract
            The function searches an item with key equal to \p key,
            unlinks it from the list, and returns it as \p guarded_ptr.
            If \p key is not found returns an empty guarded pointer.

            Note the compare functor should accept a parameter of type \p Q that can be not the same as \p value_type.

            The \ref disposer specified in \p Traits class template parameter is called automatically
            by garbage collector \p GC when returned \ref guarded_ptr object will be destroyed or released.
            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.

            Usage:
            \code
            typedef cds::intrusive::IterableList< cds::gc::HP, foo, my_traits >  ord_list;
            ord_list theList;
            // ...
            {
                ord_list::guarded_ptr gp( theList.extract( 5 ));
                if ( gp ) {
                    // Deal with gp
                    // ...
                }
                // Destructor of gp releases internal HP guard
            }
            \endcode
        */
        template <typename Q>
        guarded_ptr extract( Q const& key )
        {
            return extract_at( &m_Head, key, key_comparator());
        }

        /// Extracts the item using compare functor \p pred
        /**
            The function is an analog of \ref cds_intrusive_IterableList_hp_extract "extract(Q const&)"
            but \p pred predicate is used for key comparing.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        guarded_ptr extract_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return extract_at( &m_Head, key, cds::opt::details::make_comparator_from_less<Less>());
        }

        /// Finds \p key in the list
        /** \anchor cds_intrusive_IterableList_hp_find_func
            The function searches the item with key equal to \p key and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q& key );
            };
            \endcode
            where \p item is the item found, \p key is the \p %find() function argument.

            The functor may change non-key fields of \p item. Note that the function is only guarantee
            that \p item cannot be disposed during functor is executing.
            The function does not serialize simultaneous access to the \p item. If such access is
            possible you must provide your own synchronization schema to keep out unsafe item modifications.

            The function returns \p true if \p val is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q& key, Func f ) const
        {
            return find_at( &m_Head, key, key_comparator(), f );
        }
        //@cond
        template <typename Q, typename Func>
        bool find( Q const& key, Func f ) const
        {
            return find_at( &m_Head, key, key_comparator(), f );
        }
        //@endcond

        /// Finds \p key in the list and returns iterator pointed to the item found
        /**
            If \p key is not found the function returns \p end().
        */
        template <typename Q>
        iterator find( Q const& key ) const
        {
            return find_iterator_at( &m_Head, key, key_comparator());
        }

        /// Finds the \p key using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_intrusive_IterableList_hp_find_func "find(Q&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q& key, Less pred, Func f ) const
        {
            CDS_UNUSED( pred );
            return find_at( &m_Head, key, cds::opt::details::make_comparator_from_less<Less>(), f );
        }
        //@cond
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& key, Less pred, Func f ) const
        {
            CDS_UNUSED( pred );
            return find_at( &m_Head, key, cds::opt::details::make_comparator_from_less<Less>(), f );
        }
        //@endcond

        /// Finds \p key in the list using \p pred predicate for searching and returns iterator pointed to the item found
        /**
            The function is an analog of \p find(Q&) but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.

            If \p key is not found the function returns \p end().
        */
        template <typename Q, typename Less>
        iterator find_with( Q const& key, Less pred ) const
        {
            CDS_UNUSED( pred );
            return find_iterator_at( &m_Head, key, cds::opt::details::make_comparator_from_less<Less>());
        }

        /// Checks whether the list contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.
        */
        template <typename Q>
        bool contains( Q const& key ) const
        {
            return find_at( &m_Head, key, key_comparator());
        }

        /// Checks whether the list contains \p key using \p pred predicate for searching
        /**
            The function is an analog of <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        bool contains( Q const& key, Less pred ) const
        {
            CDS_UNUSED( pred );
            return find_at( &m_Head, key, cds::opt::details::make_comparator_from_less<Less>());
        }

        /// Finds the \p key and return the item found
        /** \anchor cds_intrusive_IterableList_hp_get
            The function searches the item with key equal to \p key
            and returns it as \p guarded_ptr.
            If \p key is not found the function returns an empty guarded pointer.

            The \ref disposer specified in \p Traits class template parameter is called
            by garbage collector \p GC automatically when returned \ref guarded_ptr object
            will be destroyed or released.
            @note Each \p guarded_ptr object uses one GC's guard which can be limited resource.

            Usage:
            \code
            typedef cds::intrusive::IterableList< cds::gc::HP, foo, my_traits >  ord_list;
            ord_list theList;
            // ...
            {
                ord_list::guarded_ptr gp(theList.get( 5 ));
                if ( gp ) {
                    // Deal with gp
                    //...
                }
                // Destructor of guarded_ptr releases internal HP guard
            }
            \endcode

            Note the compare functor specified for \p Traits template parameter
            should accept a parameter of type \p Q that can be not the same as \p value_type.
        */
        template <typename Q>
        guarded_ptr get( Q const& key ) const
        {
            return get_at( &m_Head, key, key_comparator());
        }

        /// Finds the \p key and return the item found
        /**
            The function is an analog of \ref cds_intrusive_IterableList_hp_get "get( Q const&)"
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        guarded_ptr get_with( Q const& key, Less pred ) const
        {
            CDS_UNUSED( pred );
            return get_at( &m_Head, key, cds::opt::details::make_comparator_from_less<Less>());
        }

        /// Clears the list (thread safe, not atomic)
        void clear()
        {
            position pos;
            pos.pPrev = nullptr;
            for ( pos.pCur = m_Head.next.load( memory_model::memory_order_relaxed ); pos.pCur != pos.pPrev; pos.pCur = pos.pCur->next.load( memory_model::memory_order_relaxed )) {
                while ( true ) {
                    pos.pFound = pos.guard.protect( pos.pCur->data, []( marked_data_ptr p ) { return p.ptr(); }).ptr();
                    if ( !pos.pFound )
                        break;
                    if ( cds_likely( unlink_data( pos ))) {
                        --m_ItemCounter;
                        break;
                    }
                }
                pos.pPrev = pos.pCur;
            }
        }

        /// Checks if the list is empty
        /**
            Emptiness is checked by item counting: if item count is zero then the set is empty.
            Thus, if you need to use \p %empty() you should provide appropriate (non-empty) \p iterable_list::traits::item_counter
            feature.
        */
        bool empty() const
        {
            return size() == 0;
        }

        /// Returns list's item count
        /**
            The value returned depends on item counter provided by \p iterable_list::traits::item_counter. For \p atomicity::empty_item_counter,
            this function always returns 0.
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
        bool insert_aux_node( node_type* pHead, node_type * pNode )
        {
            assert( pNode != nullptr );
            assert( pNode->data.load( memory_model::memory_order_relaxed ) != nullptr );

            insert_position pos;

            while ( true ) {
                if ( inserting_search( pHead, *pNode->data.load(memory_model::memory_order_relaxed).ptr(), pos, key_comparator())) {
                    m_Stat.onInsertFailed();
                    return false;
                }

                if ( link_aux_node( pNode, pos, pHead )) {
                    ++m_ItemCounter;
                    m_Stat.onInsertSuccess();
                    return true;
                }

                m_Stat.onInsertRetry();
            }
        }

        bool insert_at( node_type* pHead, value_type& val )
        {
            insert_position pos;

            while ( true ) {
                if ( inserting_search( pHead, val, pos, key_comparator())) {
                    m_Stat.onInsertFailed();
                    return false;
                }

                if ( link_data( &val, pos, pHead )) {
                    ++m_ItemCounter;
                    m_Stat.onInsertSuccess();
                    return true;
                }

                m_Stat.onInsertRetry();
            }
        }

        template <typename Func>
        bool insert_at( node_type* pHead, value_type& val, Func f )
        {
            insert_position pos;

            typename gc::Guard guard;
            guard.assign( &val );

            while ( true ) {
                if ( inserting_search( pHead, val, pos, key_comparator())) {
                    m_Stat.onInsertFailed();
                    return false;
                }

                if ( link_data( &val, pos, pHead )) {
                    f( val );
                    ++m_ItemCounter;
                    m_Stat.onInsertSuccess();
                    return true;
                }

                m_Stat.onInsertRetry();
            }
        }

        template <typename Func>
        std::pair<bool, bool> update_at( node_type* pHead, value_type& val, Func func, bool bInsert )
        {
            insert_position pos;

            typename gc::Guard guard;
            guard.assign( &val );

            while ( true ) {
                if ( inserting_search( pHead, val, pos, key_comparator())) {
                    // try to replace pCur->data with val
                    assert( pos.pFound != nullptr );
                    assert( key_comparator()(*pos.pFound, val) == 0 );

                    marked_data_ptr pFound( pos.pFound );
                    if ( cds_likely( pos.pCur->data.compare_exchange_strong( pFound, marked_data_ptr( &val ),
                            memory_model::memory_order_release, atomics::memory_order_relaxed )))
                    {
                        if ( pos.pFound != &val ) {
                            retire_data( pos.pFound );
                            func( val, pos.pFound );
                        }
                        m_Stat.onUpdateExisting();
                        return std::make_pair( true, false );
                    }
                }
                else {
                    if ( !bInsert ) {
                        m_Stat.onUpdateFailed();
                        return std::make_pair( false, false );
                    }

                    if ( link_data( &val, pos, pHead )) {
                        func( val, static_cast<value_type*>( nullptr ));
                        ++m_ItemCounter;
                        m_Stat.onUpdateNew();
                        return std::make_pair( true, true );
                    }
                }

                m_Stat.onUpdateRetry();
            }
        }

        std::pair<bool, bool> upsert_at( node_type* pHead, value_type& val, bool bInsert )
        {
            return update_at( pHead, val, []( value_type&, value_type* ) {}, bInsert );
        }

        bool unlink_at( node_type* pHead, value_type& val )
        {
            position pos;

            back_off bkoff;
            while ( search( pHead, val, pos, key_comparator())) {
                if ( pos.pFound == &val ) {
                    if ( unlink_data( pos )) {
                        --m_ItemCounter;
                        m_Stat.onEraseSuccess();
                        return true;
                    }
                    else
                        bkoff();
                }
                else
                    break;

                m_Stat.onEraseRetry();
            }

            m_Stat.onEraseFailed();
            return false;
        }

        template <typename Q, typename Compare, typename Func>
        bool erase_at( node_type* pHead, Q const& val, Compare cmp, Func f, position& pos )
        {
            back_off bkoff;
            while ( search( pHead, val, pos, cmp )) {
                if ( unlink_data( pos )) {
                    f( *pos.pFound );
                    --m_ItemCounter;
                    m_Stat.onEraseSuccess();
                    return true;
                }
                else
                    bkoff();

                m_Stat.onEraseRetry();
            }

            m_Stat.onEraseFailed();
            return false;
        }

        template <typename Q, typename Compare, typename Func>
        bool erase_at( node_type* pHead, Q const& val, Compare cmp, Func f )
        {
            position pos;
            return erase_at( pHead, val, cmp, f, pos );
        }

        template <typename Q, typename Compare>
        bool erase_at( node_type* pHead, Q const& val, Compare cmp )
        {
            position pos;
            return erase_at( pHead, val, cmp, [](value_type const&){}, pos );
        }

        template <typename Q, typename Compare>
        guarded_ptr extract_at( node_type* pHead, Q const& val, Compare cmp )
        {
            position pos;
            back_off bkoff;
            while ( search( pHead, val, pos, cmp )) {
                if ( unlink_data( pos )) {
                    --m_ItemCounter;
                    m_Stat.onEraseSuccess();
                    assert( pos.pFound != nullptr );
                    return guarded_ptr( std::move( pos.guard ));
                }
                else
                    bkoff();

                m_Stat.onEraseRetry();
            }

            m_Stat.onEraseFailed();
            return guarded_ptr();
        }

        template <typename Q, typename Compare>
        bool find_at( node_type const* pHead, Q const& val, Compare cmp ) const
        {
            position pos;
            if ( search( pHead, val, pos, cmp )) {
                m_Stat.onFindSuccess();
                return true;
            }

            m_Stat.onFindFailed();
            return false;
        }

        template <typename Q, typename Compare, typename Func>
        bool find_at( node_type const* pHead, Q& val, Compare cmp, Func f ) const
        {
            position pos;
            if ( search( pHead, val, pos, cmp )) {
                assert( pos.pFound != nullptr );
                f( *pos.pFound, val );
                m_Stat.onFindSuccess();
                return true;
            }

            m_Stat.onFindFailed();
            return false;
        }

        template <typename Q, typename Compare>
        iterator find_iterator_at( node_type const* pHead, Q const& val, Compare cmp ) const
        {
            position pos;
            if ( search( pHead, val, pos, cmp )) {
                assert( pos.pCur != nullptr );
                assert( pos.pFound != nullptr );
                m_Stat.onFindSuccess();
                return iterator( pos.pCur, pos.pFound );
            }

            m_Stat.onFindFailed();
            return iterator( const_cast<node_type*>( &m_Tail ));
        }

        template <typename Q, typename Compare>
        guarded_ptr get_at( node_type const* pHead, Q const& val, Compare cmp ) const
        {
            position pos;
            if ( search( pHead, val, pos, cmp )) {
                m_Stat.onFindSuccess();
                return guarded_ptr( std::move( pos.guard ));
            }

            m_Stat.onFindFailed();
            return guarded_ptr();
        }

        node_type* head()
        {
            return &m_Head;
        }

        node_type const* head() const
        {
            return &m_Head;
        }
        //@endcond

    protected:
        //@cond
        template <typename Q, typename Compare >
        bool search( node_type const* pHead, Q const& val, position& pos, Compare cmp ) const
        {
            pos.pHead = pHead;
            node_type*  pPrev = const_cast<node_type*>( pHead );

            while ( true ) {
                node_type * pCur = pPrev->next.load( memory_model::memory_order_relaxed );

                if ( pCur == pCur->next.load( memory_model::memory_order_acquire )) {
                    // end-of-list
                    pos.pPrev = pPrev;
                    pos.pCur = pCur;
                    pos.pFound = nullptr;
                    return false;
                }

                value_type * pVal = pos.guard.protect( pCur->data,
                    []( marked_data_ptr p ) -> value_type*
                    {
                        return p.ptr();
                    }).ptr();

                if ( pVal ) {
                    int const nCmp = cmp( *pVal, val );
                    if ( nCmp >= 0 ) {
                        pos.pPrev = pPrev;
                        pos.pCur = pCur;
                        pos.pFound = pVal;
                        return nCmp == 0;
                    }
                }

                pPrev = pCur;
            }
        }

        template <typename Q, typename Compare >
        bool inserting_search( node_type const* pHead, Q const& val, insert_position& pos, Compare cmp ) const
        {
            pos.pHead = pHead;
            node_type*  pPrev = const_cast<node_type*>(pHead);
            value_type* pPrevVal = pPrev->data.load( memory_model::memory_order_relaxed ).ptr();

            while ( true ) {
                node_type * pCur = pPrev->next.load( memory_model::memory_order_relaxed );

                if ( pCur == pCur->next.load( memory_model::memory_order_acquire )) {
                    // end-of-list
                    pos.pPrev = pPrev;
                    pos.pCur = pCur;
                    pos.pFound = nullptr;
                    pos.pPrevVal = pPrevVal;
                    return false;
                }

                value_type * pVal = pos.guard.protect( pCur->data,
                    []( marked_data_ptr p ) -> value_type*
                {
                    return p.ptr();
                } ).ptr();

                if ( pVal ) {
                    int const nCmp = cmp( *pVal, val );
                    if ( nCmp >= 0 ) {
                        pos.pPrev = pPrev;
                        pos.pCur = pCur;
                        pos.pFound = pVal;
                        pos.pPrevVal = pPrevVal;
                        return nCmp == 0;
                    }
                }

                pPrev = pCur;
                pPrevVal = pVal;
                pos.prevGuard.copy( pos.guard );
            }
        }

        // split-list support
        template <typename Predicate>
        void destroy( Predicate pred )
        {
            node_type * pNode = m_Head.next.load( memory_model::memory_order_relaxed );
            while ( pNode != pNode->next.load( memory_model::memory_order_relaxed )) {
                value_type * pVal = pNode->data.load( memory_model::memory_order_relaxed ).ptr();
                node_type * pNext = pNode->next.load( memory_model::memory_order_relaxed );
                bool const is_regular_node = !pVal || pred( pVal );
                if ( is_regular_node ) {
                    if ( pVal )
                        retire_data( pVal );
                    delete_node( pNode );
                }
                pNode = pNext;
            }

            m_Head.next.store( &m_Tail, memory_model::memory_order_relaxed );
        }
        //@endcond

    private:
        //@cond
        void init_list()
        {
            m_Head.next.store( &m_Tail, memory_model::memory_order_relaxed );
            // end-of-list mark: node.next == node
            m_Tail.next.store( &m_Tail, memory_model::memory_order_release );
        }

        node_type * alloc_node( value_type * pVal )
        {
            m_Stat.onNodeCreated();
            return cxx_node_allocator().New( pVal );
        }

        void delete_node( node_type * pNode )
        {
            m_Stat.onNodeRemoved();
            cxx_node_allocator().Delete( pNode );
        }

        static void retire_data( value_type * pVal )
        {
            assert( pVal != nullptr );
            gc::template retire<disposer>( pVal );
        }

        void destroy()
        {
            node_type * pNode = m_Head.next.load( memory_model::memory_order_relaxed );
            while ( pNode != pNode->next.load( memory_model::memory_order_relaxed )) {
                value_type * pVal = pNode->data.load( memory_model::memory_order_relaxed ).ptr();
                if ( pVal )
                    retire_data( pVal );
                node_type * pNext = pNode->next.load( memory_model::memory_order_relaxed );
                delete_node( pNode );
                pNode = pNext;
            }
        }

        bool link_data( value_type* pVal, insert_position& pos, node_type* pHead )
        {
            assert( pos.pPrev != nullptr );
            assert( pos.pCur != nullptr );

            // We need pos.pCur data should be unchanged, otherwise ordering violation can be possible
            // if current thread will be preempted and another thread will delete pos.pCur data
            // and then set it to another.
            // To prevent this we mark pos.pCur data as undeletable by setting LSB
            marked_data_ptr valCur( pos.pFound );
            if ( !pos.pCur->data.compare_exchange_strong( valCur, valCur | 1, memory_model::memory_order_acquire, atomics::memory_order_relaxed )) {
                // oops, pos.pCur data has been changed or another thread is setting pos.pPrev data
                m_Stat.onNodeMarkFailed();
                return false;
            }

            marked_data_ptr valPrev( pos.pPrevVal );
            if ( !pos.pPrev->data.compare_exchange_strong( valPrev, valPrev | 1, memory_model::memory_order_acquire, atomics::memory_order_relaxed )) {
                pos.pCur->data.store( valCur, memory_model::memory_order_relaxed );

                m_Stat.onNodeMarkFailed();
                return false;
            }

            // checks if link pPrev -> pCur is broken
            if ( pos.pPrev->next.load( memory_model::memory_order_acquire ) != pos.pCur ) {
                // sequence pPrev - pCur is broken
                pos.pPrev->data.store( valPrev, memory_model::memory_order_relaxed );
                pos.pCur->data.store( valCur, memory_model::memory_order_relaxed );

                m_Stat.onNodeSeqBreak();
                return false;
            }

            if ( pos.pPrevVal == nullptr ) {
                // Check ABA-problem for prev
                // There is a possibility that the current thread was preempted
                // on entry of this function. Other threads can link data to prev
                // and then remove it. As a result, the order of items may be changed
                if ( find_prev( pHead, *pVal ) != pos.pPrev ) {
                    pos.pPrev->data.store( valPrev, memory_model::memory_order_relaxed );
                    pos.pCur->data.store( valCur, memory_model::memory_order_relaxed );

                    m_Stat.onNullPrevABA();
                    return false;
                }
            }

            if ( pos.pPrev != pos.pHead && pos.pPrevVal == nullptr ) {
                // reuse pPrev

                // Set pos.pPrev data if it is null
                valPrev |= 1;
                bool result = pos.pPrev->data.compare_exchange_strong( valPrev, marked_data_ptr( pVal ),
                    memory_model::memory_order_release, atomics::memory_order_relaxed );

                // Clears data marks
                pos.pCur->data.store( valCur, memory_model::memory_order_relaxed );

                if ( result ) {
                    m_Stat.onReuseNode();
                    return result;
                }
            }
            else {
                // insert new node between pos.pPrev and pos.pCur
                node_type * pNode = alloc_node( pVal );
                pNode->next.store( pos.pCur, memory_model::memory_order_relaxed );

                bool result = pos.pPrev->next.compare_exchange_strong( pos.pCur, pNode, memory_model::memory_order_release, atomics::memory_order_relaxed );

                // Clears data marks
                pos.pPrev->data.store( valPrev, memory_model::memory_order_relaxed );
                pos.pCur->data.store( valCur, memory_model::memory_order_relaxed );

                if ( result ) {
                    m_Stat.onNewNodeCreated();
                    return result;
                }

                delete_node( pNode );
            }

            return false;
        }

        // split-list support
        bool link_aux_node( node_type * pNode, insert_position& pos, node_type* pHead )
        {
            assert( pos.pPrev != nullptr );
            assert( pos.pCur != nullptr );

            // We need pos.pCur data should be unchanged, otherwise ordering violation can be possible
            // if current thread will be preempted and another thread will delete pos.pCur data
            // and then set it to another.
            // To prevent this we mark pos.pCur data as undeletable by setting LSB
            marked_data_ptr valCur( pos.pFound );
            if ( !pos.pCur->data.compare_exchange_strong( valCur, valCur | 1, memory_model::memory_order_acquire, atomics::memory_order_relaxed )) {
                // oops, pos.pCur data has been changed or another thread is setting pos.pPrev data
                m_Stat.onNodeMarkFailed();
                return false;
            }

            marked_data_ptr valPrev( pos.pPrevVal );
            if ( !pos.pPrev->data.compare_exchange_strong( valPrev, valPrev | 1, memory_model::memory_order_acquire, atomics::memory_order_relaxed )) {
                pos.pCur->data.store( valCur, memory_model::memory_order_relaxed );
                m_Stat.onNodeMarkFailed();
                return false;
            }

            // checks if link pPrev -> pCur is broken
            if ( pos.pPrev->next.load( memory_model::memory_order_acquire ) != pos.pCur ) {
                // sequence pPrev - pCur is broken
                pos.pPrev->data.store( valPrev, memory_model::memory_order_relaxed );
                pos.pCur->data.store( valCur, memory_model::memory_order_relaxed );
                m_Stat.onNodeSeqBreak();
                return false;
            }

            if ( pos.pPrevVal == nullptr ) {
                // Check ABA-problem for prev
                // There is a possibility that the current thread was preempted
                // on entry of this function. Other threads can insert (link) an item to prev
                // and then remove it. As a result, the order of items may be changed
                if ( find_prev( pHead, *pNode->data.load( memory_model::memory_order_relaxed ).ptr()) != pos.pPrev ) {
                    pos.pPrev->data.store( valPrev, memory_model::memory_order_relaxed );
                    pos.pCur->data.store( valCur, memory_model::memory_order_relaxed );

                    m_Stat.onNullPrevABA();
                    return false;
                }
            }

            // insert new node between pos.pPrev and pos.pCur
            pNode->next.store( pos.pCur, memory_model::memory_order_relaxed );

            bool result = pos.pPrev->next.compare_exchange_strong( pos.pCur, pNode, memory_model::memory_order_release, atomics::memory_order_relaxed );

            // Clears data marks
            pos.pPrev->data.store( valPrev, memory_model::memory_order_relaxed );
            pos.pCur->data.store( valCur, memory_model::memory_order_relaxed );

            return result;
        }

        static bool unlink_data( position& pos )
        {
            assert( pos.pCur != nullptr );
            assert( pos.pFound != nullptr );

            marked_data_ptr val( pos.pFound );
            if ( pos.pCur->data.compare_exchange_strong( val, marked_data_ptr(), memory_model::memory_order_acquire, atomics::memory_order_relaxed )) {
                retire_data( pos.pFound );
                return true;
            }
            return false;
        }

        template <typename Q>
        node_type* find_prev( node_type const* pHead, Q const& val ) const
        {
            node_type*  pPrev = const_cast<node_type*>(pHead);
            typename gc::Guard guard;
            key_comparator cmp;

            while ( true ) {
                node_type * pCur = pPrev->next.load( memory_model::memory_order_relaxed );

                if ( pCur == pCur->next.load( memory_model::memory_order_acquire )) {
                    // end-of-list
                    return pPrev;
                }

                value_type * pVal = guard.protect( pCur->data,
                    []( marked_data_ptr p ) -> value_type*
                {
                    return p.ptr();
                } ).ptr();

                if ( pVal && cmp( *pVal, val ) >= 0 )
                    return pPrev;

                pPrev = pCur;
            }
        }
        //@endcond
    };
}} // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_IMPL_ITERABLE_LIST_H
