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

#ifndef CDSLIB_INTRUSIVE_IMPL_ITERABLE_LIST_H
#define CDSLIB_INTRUSIVE_IMPL_ITERABLE_LIST_H

#include <cds/intrusive/details/iterable_list_base.h>
#include <cds/details/make_const_type.h>

namespace cds { namespace intrusive {

    /// Iterable lock-free ordered single-linked list
    /** @ingroup cds_intrusive_list
        \anchor cds_intrusive_IterableList_hp

        This lock-free list implementation supports thread-safe iterators.
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
        - for \ref cds_urcu_gc "RCU type" - see \ref cds_intrusive_IterableList_rcu "RCU-based IterableList"
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

        static CDS_CONSTEXPR const size_t c_nHazardPtrCount = 2; ///< Count of hazard pointer required for the algorithm

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
        typedef atomics::atomic< node_type* > atomic_node_ptr;  ///< Atomic node pointer
        typedef atomic_node_ptr               auxiliary_head;   ///< Auxiliary head type (for split-list support)

        atomic_node_ptr m_pHead;        ///< Head pointer
        item_counter    m_ItemCounter;  ///< Item counter
        mutable stat    m_Stat;         ///< Internal statistics

        //@cond
        typedef cds::details::Allocator< node_type, node_allocator > cxx_node_allocator;

        /// Position pointer for item search
        struct position {
            atomic_node_ptr * pHead; ///< Previous node (pointer to pPrev->next or to m_pHead)
            node_type *       pPrev;  ///< Previous node
            node_type *       pCur;   ///< Current node

            value_type *      pFound; ///< Value of \p pCur->data, valid only if data found
            typename gc::Guard guard; ///< guard for \p pFound
        };
        //@endcond

    protected:
        //@cond
        template <bool IsConst>
        class iterator_type
        {
            friend class IterableList;

        protected:
            node_type*  m_pNode;
            value_type* m_pVal;
            typename gc::Guard  m_Guard; // for m_pVal

            void next()
            {
                while ( m_pNode ) {
                    m_pNode = m_pNode->next.load( memory_model::memory_order_relaxed );
                    if ( !m_pNode )
                        break;
                    m_pVal = m_Guard.protect( m_pNode->data );
                    if ( m_pVal )
                        break;
                }
            }

            explicit iterator_type( atomic_node_ptr const& pNode )
                : m_pNode( pNode.load( memory_model::memory_order_relaxed ))
                , m_pVal( nullptr )
            {
                if ( m_pNode ) {
                    m_pVal = m_Guard.protect( m_pNode->data );
                    if ( !m_pVal )
                        next();
                }
            }

            iterator_type( node_type* pNode, value_type* pVal )
                : m_pNode( pNode )
                , m_pVal( pVal )
            {
                if ( m_pNode ) {
                    assert( pVal != nullptr );
                    m_Guard.assign( pVal );
                }
            }

        public:
            typedef typename cds::details::make_const_type<value_type, IsConst>::pointer   value_ptr;
            typedef typename cds::details::make_const_type<value_type, IsConst>::reference value_ref;

            iterator_type()
                : m_pNode( nullptr )
                , m_pVal( nullptr )
            {}

            iterator_type( iterator_type const& src )
                : m_pNode( src.m_pNode )
                , m_pVal( src.m_pVal )
            {
                m_Guard.assign( m_pVal );
            }

            value_ptr operator ->() const
            {
                return m_pVal;
            }

            value_ref operator *() const
            {
                assert( m_pVal != nullptr );
                return *m_pVal;
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
                m_pVal = src.m_pVal;
                m_Guard.assign( m_pVal );
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
                    assert( &(*it1) == &(*it2) );
            \endcode
            can throw assertion. The point is that the iterator stores the value of element which can be modified later by other thread.
            The guard inside the iterator prevents recycling that value so the iterator's value remains valid even after such changing.
            Other iterator can observe modified value of the element.
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
            return iterator( m_pHead );
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
            return iterator();
        }

        /// Returns a forward const iterator addressing the first element in a list
        const_iterator cbegin() const
        {
            return const_iterator( m_pHead );
        }

        /// Returns a forward const iterator addressing the first element in a list
        const_iterator begin() const
        {
            return const_iterator( m_pHead );
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
    //@}

    public:
        /// Default constructor initializes empty list
        IterableList()
            : m_pHead( nullptr )
        {}

        //@cond
        template <typename Stat, typename = std::enable_if<std::is_same<stat, iterable_list::wrapped_stat<Stat>>::value >>
        explicit IterableList( Stat& st )
            : m_pHead( nullptr )
            , m_Stat( st )
        {}
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
            return insert_at( m_pHead, val );
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
            return insert_at( m_pHead, val, f );
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
            return update_at( m_pHead, val, func, bInsert );
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
            return update_at( m_pHead, val, []( value_type&, value_type* ) {}, bInsert );
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
            return unlink_at( m_pHead, val );
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
            return erase_at( m_pHead, key, key_comparator());
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
            return erase_at( m_pHead, key, cds::opt::details::make_comparator_from_less<Less>());
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
            return erase_at( m_pHead, key, key_comparator(), func );
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
            return erase_at( m_pHead, key, cds::opt::details::make_comparator_from_less<Less>(), f );
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
                ord_list::guarded_ptr gp(theList.extract( 5 ));
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
            guarded_ptr gp;
            extract_at( m_pHead, gp.guard(), key, key_comparator());
            return gp;
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
            guarded_ptr gp;
            extract_at( m_pHead, gp.guard(), key, cds::opt::details::make_comparator_from_less<Less>());
            return gp;
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
            return find_at( m_pHead, key, key_comparator(), f );
        }
        //@cond
        template <typename Q, typename Func>
        bool find( Q const& key, Func f ) const
        {
            return find_at( m_pHead, key, key_comparator(), f );
        }
        //@endcond

        /// Finds \p key in the list and returns iterator pointed to the item found
        /**
            If \p key is not found the function returns \p end().
        */
        template <typename Q>
        iterator find( Q const& key ) const
        {
            return find_iterator_at( m_pHead, key, key_comparator());
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
            return find_at( m_pHead, key, cds::opt::details::make_comparator_from_less<Less>(), f );
        }
        //@cond
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& key, Less pred, Func f ) const
        {
            CDS_UNUSED( pred );
            return find_at( m_pHead, key, cds::opt::details::make_comparator_from_less<Less>(), f );
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
            return find_iterator_at( m_pHead, key, cds::opt::details::make_comparator_from_less<Less>());
        }

        /// Checks whether the list contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.
        */
        template <typename Q>
        bool contains( Q const& key ) const
        {
            return find_at( m_pHead, key, key_comparator());
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
            return find_at( m_pHead, key, cds::opt::details::make_comparator_from_less<Less>());
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
            guarded_ptr gp;
            get_at( m_pHead, gp.guard(), key, key_comparator());
            return gp;
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
            guarded_ptr gp;
            get_at( m_pHead, gp.guard(), key, cds::opt::details::make_comparator_from_less<Less>());
            return gp;
        }

        /// Clears the list (thread safe, not atomic)
        void clear()
        {
            position pos;
            for ( pos.pCur = m_pHead.load( memory_model::memory_order_relaxed ); pos.pCur; pos.pCur = pos.pCur->next.load( memory_model::memory_order_relaxed )) {
                while ( true ) {
                    pos.pFound = pos.guard.protect( pos.pCur->data );
                    if ( !pos.pFound )
                        break;
                    if ( cds_likely( unlink_node( pos ))) {
                        --m_ItemCounter;
                        break;
                    }
                }
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
#if 0
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
            return insert_at( refHead, *node_traits::to_value_ptr( pNode ) );
        }
#endif

        bool insert_at( atomic_node_ptr& refHead, value_type& val )
        {
            position pos;

            while ( true ) {
                if ( search( refHead, val, pos, key_comparator() )) {
                    m_Stat.onInsertFailed();
                    return false;
                }

                if ( link_node( &val, pos ) ) {
                    ++m_ItemCounter;
                    m_Stat.onInsertSuccess();
                    return true;
                }

                m_Stat.onInsertRetry();
            }
        }

        template <typename Func>
        bool insert_at( atomic_node_ptr& refHead, value_type& val, Func f )
        {
            position pos;

            typename gc::Guard guard;
            guard.assign( &val );

            while ( true ) {
                if ( search( refHead, val, pos, key_comparator() ) ) {
                    m_Stat.onInsertFailed();
                    return false;
                }

                if ( link_node( &val, pos ) ) {
                    f( val );
                    ++m_ItemCounter;
                    m_Stat.onInsertSuccess();
                    return true;
                }

                m_Stat.onInsertRetry();
            }
        }

        template <typename Func>
        std::pair<bool, bool> update_at( atomic_node_ptr& refHead, value_type& val, Func func, bool bInsert )
        {
            position pos;

            typename gc::Guard guard;
            guard.assign( &val );

            while ( true ) {
                if ( search( refHead, val, pos, key_comparator() ) ) {
                    // try to replace pCur->data with val
                    assert( pos.pFound != nullptr );
                    assert( key_comparator()(*pos.pFound, val) == 0 );

                    if ( cds_likely( pos.pCur->data.compare_exchange_strong( pos.pFound, &val, memory_model::memory_order_release, atomics::memory_order_relaxed ))) {
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

                    if ( link_node( &val, pos )) {
                        func( val, static_cast<value_type*>( nullptr ));
                        ++m_ItemCounter;
                        m_Stat.onUpdateNew();
                        return std::make_pair( true, true );
                    }
                }

                m_Stat.onUpdateRetry();
            }
        }

        bool unlink_at( atomic_node_ptr& refHead, value_type& val )
        {
            position pos;

            back_off bkoff;
            while ( search( refHead, val, pos, key_comparator())) {
                if ( pos.pFound == &val ) {
                    if ( unlink_node( pos )) {
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
        bool erase_at( atomic_node_ptr& refHead, const Q& val, Compare cmp, Func f, position& pos )
        {
            back_off bkoff;
            while ( search( refHead, val, pos, cmp )) {
                if ( unlink_node( pos )) {
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
        bool erase_at( atomic_node_ptr& refHead, const Q& val, Compare cmp, Func f )
        {
            position pos;
            return erase_at( refHead, val, cmp, f, pos );
        }

        template <typename Q, typename Compare>
        bool erase_at( atomic_node_ptr& refHead, Q const& val, Compare cmp )
        {
            position pos;
            return erase_at( refHead, val, cmp, [](value_type const&){}, pos );
        }

        template <typename Q, typename Compare>
        bool extract_at( atomic_node_ptr& refHead, typename guarded_ptr::native_guard& dest, Q const& val, Compare cmp )
        {
            position pos;
            back_off bkoff;
            while ( search( refHead, val, pos, cmp )) {
                if ( unlink_node( pos )) {
                    dest.set( pos.pFound );
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

        template <typename Q, typename Compare>
        bool find_at( atomic_node_ptr const& refHead, Q const& val, Compare cmp ) const
        {
            position pos;
            if ( search( refHead, val, pos, cmp ) ) {
                m_Stat.onFindSuccess();
                return true;
            }

            m_Stat.onFindFailed();
            return false;
        }

        template <typename Q, typename Compare, typename Func>
        bool find_at( atomic_node_ptr const& refHead, Q& val, Compare cmp, Func f ) const
        {
            position pos;
            if ( search( refHead, val, pos, cmp )) {
                assert( pos.pFound != nullptr );
                f( *pos.pFound, val );
                m_Stat.onFindSuccess();
                return true;
            }

            m_Stat.onFindFailed();
            return false;
        }

        template <typename Q, typename Compare>
        iterator find_iterator_at( atomic_node_ptr const& refHead, Q const& val, Compare cmp ) const
        {
            position pos;
            if ( search( refHead, val, pos, cmp )) {
                assert( pos.pCur != nullptr );
                assert( pos.pFound != nullptr );
                m_Stat.onFindSuccess();
                return iterator( pos.pCur, pos.pFound );
            }

            m_Stat.onFindFailed();
            return iterator{};
        }

        template <typename Q, typename Compare>
        bool get_at( atomic_node_ptr const& refHead, typename guarded_ptr::native_guard& guard, Q const& val, Compare cmp ) const
        {
            position pos;
            if ( search( refHead, val, pos, cmp )) {
                guard.set( pos.pFound );
                m_Stat.onFindSuccess();
                return true;
            }

            m_Stat.onFindFailed();
            return false;
        }
        //@endcond

    protected:

        //@cond
        template <typename Q, typename Compare >
        bool search( atomic_node_ptr const& refHead, const Q& val, position& pos, Compare cmp ) const
        {
            atomic_node_ptr* pHead = const_cast<atomic_node_ptr*>( &refHead );
            node_type * pPrev = nullptr;

            while ( true ) {
                node_type * pCur = pHead->load( memory_model::memory_order_relaxed );

                if ( pCur == nullptr ) {
                    // end-of-list
                    pos.pHead = pHead;
                    pos.pPrev = pPrev;
                    pos.pCur = nullptr;
                    pos.pFound = nullptr;
                    return false;
                }

                value_type * pVal = pos.guard.protect( pCur->data );

                if ( pVal ) {
                    int nCmp = cmp( *pVal, val );
                    if ( nCmp >= 0 ) {
                        pos.pHead = pHead;
                        pos.pPrev = pPrev;
                        pos.pCur = pCur;
                        pos.pFound = pVal;
                        return nCmp == 0;
                    }
                }

                pPrev = pCur;
                pHead = &( pCur->next );
            }
        }
        //@endcond

    private:
        //@cond
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
            node_type * pNode = m_pHead.load( memory_model::memory_order_relaxed );
            while ( pNode ) {
                value_type * pVal = pNode->data.load( memory_model::memory_order_relaxed );
                if ( pVal )
                    retire_data( pVal );
                node_type * pNext = pNode->next.load( memory_model::memory_order_relaxed );
                delete_node( pNode );
                pNode = pNext;
            }
        }

        bool link_node( value_type * pVal, position& pos )
        {
            if ( pos.pPrev ) {
                if ( pos.pPrev->data.load( memory_model::memory_order_relaxed ) == nullptr ) {
                    // reuse pPrev
                    value_type * p = nullptr;
                    return pos.pPrev->data.compare_exchange_strong( p, pVal, memory_model::memory_order_release, atomics::memory_order_relaxed );
                }
                else {
                    // insert new node between pos.pPrev and pos.pCur
                    node_type * pNode = alloc_node( pVal );
                    pNode->next.store( pos.pCur, memory_model::memory_order_relaxed );

                    if ( cds_likely( pos.pPrev->next.compare_exchange_strong( pos.pCur, pNode, memory_model::memory_order_release, atomics::memory_order_relaxed )))
                        return true;

                    delete_node( pNode );
                }
            }
            else {
                node_type * pNode = alloc_node( pVal );
                pNode->next.store( pos.pCur, memory_model::memory_order_relaxed );
                if ( cds_likely( pos.pHead->compare_exchange_strong( pos.pCur, pNode, memory_model::memory_order_release, atomics::memory_order_relaxed ) ) )
                    return true;

                delete_node( pNode );
            }
            return false;
        }

        static bool unlink_node( position& pos )
        {
            assert( pos.pCur != nullptr );
            assert( pos.pFound != nullptr );

            if ( pos.pCur->data.compare_exchange_strong( pos.pFound, nullptr, memory_model::memory_order_acquire, atomics::memory_order_relaxed ) ) {
                retire_data( pos.pFound );
                return true;
            }
            return false;
        }

        //@endcond
    };
}} // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_IMPL_ITERABLE_LIST_H
