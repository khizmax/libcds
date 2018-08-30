// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_IMPL_LAZY_KVLIST_H
#define CDSLIB_CONTAINER_IMPL_LAZY_KVLIST_H

#include <memory>
#include <cds/container/details/guarded_ptr_cast.h>

namespace cds { namespace container {

    /// Lazy ordered list (key-value pair)
    /** @ingroup cds_nonintrusive_list
        \anchor cds_nonintrusive_LazyKVList_gc

        This is key-value variation of non-intrusive LazyList.
        Like standard container, this implementation split a value stored into two part -
        constant key and alterable value.

        Usually, ordered single-linked list is used as a building block for the hash table implementation.
        The complexity of searching is <tt>O(N)</tt>.

        Template arguments:
        - \p GC - garbage collector
        - \p Key - key type of an item to be stored in the list. It should be copy-constructible
        - \p Value - value type to be stored in the list
        - \p Traits - type traits, default is \p lazy_list::traits
            It is possible to declare option-based list with cds::container::lazy_list::make_traits metafunction istead of \p Traits template
            argument. For example, the following traits-based declaration of \p gc::HP lazy list
            \code
            #include <cds/container/lazy_kvlist_hp.h>
            // Declare comparator for the item
            struct my_compare {
                int operator ()( int i1, int i2 )
                {
                    return i1 - i2;
                }
            };

            // Declare traits
            struct my_traits: public cds::container::lazy_list::traits
            {
                typedef my_compare compare;
            };

            // Declare traits-based list
            typedef cds::container::LazyKVList< cds::gc::HP, int, int, my_traits >     traits_based_list;
            \endcode
            is equal to the following option-based list
            \code
            #include <cds/container/lazy_kvlist_hp.h>

            // my_compare is the same

            // Declare option-based list
            typedef cds::container::LazyKVList< cds::gc::HP, int, int,
                typename cds::container::lazy_list::make_traits<
                    cds::container::opt::compare< my_compare >     // item comparator option
                >::type
            >     option_based_list;
            \endcode

        \par Usage
        There are different specializations of this template for each garbage collecting schema used.
        You should include appropriate .h-file depending on GC you are using:
        - for \p gc::HP: <tt> <cds/container/lazy_kvlist_hp.h> </tt>
        - for \p gc::DHP: <tt> <cds/container/lazy_kvlist_dhp.h> </tt>
        - for \ref cds_urcu_desc "RCU": <tt> <cds/container/lazy_kvlist_rcu.h> </tt>
        - for \p gc::nogc: <tt> <cds/container/lazy_kvlist_nogc.h> </tt>
    */
    template <
        typename GC,
        typename Key,
        typename Value,
#ifdef CDS_DOXYGEN_INVOKED
        typename Traits = lazy_list::traits
#else
        typename Traits
#endif
    >
    class LazyKVList:
#ifdef CDS_DOXYGEN_INVOKED
        protected intrusive::LazyList< GC, implementation_defined, Traits >
#else
        protected details::make_lazy_kvlist< GC, Key, Value, Traits >::type
#endif
    {
        //@cond
        typedef details::make_lazy_kvlist< GC, Key, Value, Traits > maker;
        typedef typename maker::type base_class;
        //@endcond

    public:
        typedef GC     gc;     ///< Garbage collector
        typedef Traits traits; ///< Traits
#ifdef CDS_DOXYGEN_INVOKED
        typedef Key                                 key_type        ;   ///< Key type
        typedef Value                               mapped_type     ;   ///< Type of value stored in the list
        typedef std::pair<key_type const, mapped_type> value_type   ;   ///< key/value pair stored in the list
#else
        typedef typename maker::key_type    key_type;
        typedef typename maker::mapped_type mapped_type;
        typedef typename maker::value_type  value_type;
#endif
        typedef typename base_class::back_off     back_off;       ///< Back-off strategy
        typedef typename maker::allocator_type    allocator_type; ///< Allocator type used for allocate/deallocate the nodes
        typedef typename base_class::item_counter item_counter;   ///< Item counter type
        typedef typename maker::key_comparator    key_comparator; ///< key comparing functor
        typedef typename base_class::memory_model memory_model;   ///< Memory ordering. See \p cds::opt::memory_model
        typedef typename base_class::stat         stat;           ///< Internal statistics

        static constexpr const size_t c_nHazardPtrCount = base_class::c_nHazardPtrCount; ///< Count of hazard pointer required for the algorithm

        //@cond
        // Rebind traits (split-list support)
        template <typename... Options>
        struct rebind_traits {
            typedef LazyKVList<
                gc
                , key_type, mapped_type
                , typename cds::opt::make_options< traits, Options...>::type
            > type;
        };

        // Stat selector
        template <typename Stat>
        using select_stat_wrapper = typename base_class::template select_stat_wrapper< Stat >;
        //@endcond

    protected:
        //@cond
        typedef typename base_class::value_type   node_type;
        typedef typename maker::cxx_allocator     cxx_allocator;
        typedef typename maker::node_deallocator  node_deallocator;
        typedef typename maker::intrusive_traits::compare intrusive_key_comparator;

        typedef typename base_class::node_type head_type;
        //@endcond

    public:
        /// Guarded pointer
        typedef typename gc::template guarded_ptr< node_type, value_type, details::guarded_ptr_cast_map<node_type, value_type> > guarded_ptr;

    protected:
        //@cond
        template <typename K>
        static node_type * alloc_node(const K& key)
        {
            return cxx_allocator().New( key );
        }

        template <typename K, typename V>
        static node_type * alloc_node( const K& key, const V& val )
        {
            return cxx_allocator().New( key, val );
        }

        template <typename... Args>
        static node_type * alloc_node( Args&&... args )
        {
            return cxx_allocator().MoveNew( std::forward<Args>(args)... );
        }

        static void free_node( node_type * pNode )
        {
            cxx_allocator().Delete( pNode );
        }

        struct node_disposer {
            void operator()( node_type * pNode )
            {
                free_node( pNode );
            }
        };
        typedef std::unique_ptr< node_type, node_disposer >     scoped_node_ptr;

        head_type& head()
        {
            return base_class::m_Head;
        }

        head_type const& head() const
        {
            return base_class::m_Head;
        }

        head_type& tail()
        {
            return base_class::m_Tail;
        }

        head_type const& tail() const
        {
            return base_class::m_Tail;
        }

        //@endcond

    protected:
        //@cond
        template <bool IsConst>
        class iterator_type: protected base_class::template iterator_type<IsConst>
        {
            typedef typename base_class::template iterator_type<IsConst>    iterator_base;

            iterator_type( head_type const& pNode )
                : iterator_base( const_cast<head_type *>(&pNode))
            {}
            iterator_type( head_type const * pNode )
                : iterator_base( const_cast<head_type *>(pNode))
            {}

            friend class LazyKVList;

        public:
            typedef typename cds::details::make_const_type<mapped_type, IsConst>::reference  value_ref;
            typedef typename cds::details::make_const_type<mapped_type, IsConst>::pointer    value_ptr;

            typedef typename cds::details::make_const_type<value_type,  IsConst>::reference  pair_ref;
            typedef typename cds::details::make_const_type<value_type,  IsConst>::pointer    pair_ptr;

            iterator_type()
            {}

            iterator_type( iterator_type const& src )
                : iterator_base( src )
            {}

            key_type const& key() const
            {
                typename iterator_base::value_ptr p = iterator_base::operator ->();
                assert( p != nullptr );
                return p->m_Data.first;
            }

            value_ref val() const
            {
                typename iterator_base::value_ptr p = iterator_base::operator ->();
                assert( p != nullptr );
                return p->m_Data.second;
            }

            pair_ptr operator ->() const
            {
                typename iterator_base::value_ptr p = iterator_base::operator ->();
                return p ? &(p->m_Data) : nullptr;
            }

            pair_ref operator *() const
            {
                typename iterator_base::value_ref p = iterator_base::operator *();
                return p.m_Data;
            }

            /// Pre-increment
            iterator_type& operator ++()
            {
                iterator_base::operator ++();
                return *this;
            }

            template <bool C>
            bool operator ==(iterator_type<C> const& i ) const
            {
                return iterator_base::operator ==(i);
            }
            template <bool C>
            bool operator !=(iterator_type<C> const& i ) const
            {
                return iterator_base::operator !=(i);
            }
        };
        //@endcond

    public:
        /// Forward iterator
        /**
            The forward iterator for lazy list has some features:
            - it has no post-increment operator
            - to protect the value, the iterator contains a GC-specific guard + another guard is required locally for increment operator.
              For some GC (\p gc::HP), a guard is limited resource per thread, so an exception (or assertion) "no free guard"
              may be thrown if a limit of guard count per thread is exceeded.
            - The iterator cannot be moved across thread boundary since it contains GC's guard that is thread-private GC data.
            - Iterator ensures thread-safety even if you delete the item that iterator points to. However, in case of concurrent
              deleting operations it is no guarantee that you iterate all item in the list.

            @warning Use this iterator on the concurrent container for debugging purpose only.

            The iterator interface to access item data:
            - <tt> operator -> </tt> - returns a pointer to \ref value_type for iterator
            - <tt> operator *</tt> - returns a reference (a const reference for \p const_iterator) to \ref value_type for iterator
            - <tt> const key_type& key() </tt> - returns a key reference for iterator
            - <tt> mapped_type& val() </tt> - retuns a value reference for iterator (const reference for \p const_iterator)

            For both functions the iterator should not be equal to <tt> end() </tt>
        */
        typedef iterator_type<false>    iterator;

        /// Const forward iterator
        /**
            For iterator's features and requirements see \ref iterator
        */
        typedef iterator_type<true>     const_iterator;

    ///@name Forward iterators (only for debugging purpose)
    //@{
        /// Returns a forward iterator addressing the first element in a list
        /**
            For empty list \code begin() == end() \endcode
        */
        iterator begin()
        {
            iterator it( head());
            ++it ;  // skip dummy head
            return it;
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
            return iterator( tail());
        }

        /// Returns a forward const iterator addressing the first element in a list
        const_iterator begin() const
        {
            const_iterator it( head());
            ++it;   // skip dummy head
            return it;
        }

        /// Returns a forward const iterator addressing the first element in a list
        const_iterator cbegin() const
        {
            const_iterator it( head());
            ++it;   // skip dummy head
            return it;
        }

        /// Returns an const iterator that addresses the location succeeding the last element in a list
        const_iterator end() const
        {
            return const_iterator( tail());
        }

        /// Returns an const iterator that addresses the location succeeding the last element in a list
        const_iterator cend() const
        {
            return const_iterator( tail());
        }
    //@}

    public:
        /// Default constructor
        LazyKVList()
        {}

        //@cond
        template <typename Stat, typename = std::enable_if<std::is_same<stat, lazy_list::wrapped_stat<Stat>>::value >>
        explicit LazyKVList( Stat& st )
            : base_class( st )
        {}
        //@endcond

        /// Destructor clears the list
        ~LazyKVList()
        {
            clear();
        }

        /// Inserts new node with key and default value
        /**
            The function creates a node with \p key and default value, and then inserts the node created into the list.

            Preconditions:
            - The \ref key_type should be constructible from value of type \p K.
                In trivial case, \p K is equal to \ref key_type.
            - The \ref mapped_type should be default-constructible.

            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename K>
        bool insert( K&& key )
        {
            return insert_at( head(), std::forward<K>( key ));
        }

        /// Inserts new node with a key and a value
        /**
            The function creates a node with \p key and value \p val, and then inserts the node created into the list.

            Preconditions:
            - The \ref key_type should be constructible from \p key of type \p K.
            - The \ref mapped_type should be constructible from \p val of type \p V.

            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename K, typename V>
        bool insert( K&& key, V&& val )
        {
            // We cannot use insert with functor here
            // because we cannot lock inserted node for updating
            // Therefore, we use separate function
            return insert_at( head(), std::forward<K>( key ), std::forward<V>( val ));
        }

        /// Inserts new node and initializes it by a functor
        /**
            This function inserts new node with key \p key and if inserting is successful then it calls
            \p func functor with signature
            \code
                struct functor {
                    void operator()( value_type& item );
                };
            \endcode

            The argument \p item of user-defined functor \p func is the reference
            to the list's item inserted. <tt>item.second</tt> is a reference to item's value that may be changed.
            The user-defined functor is called only if inserting is successful.

            The \p key_type should be constructible from value of type \p K.

            The function allows to split creating of new item into two part:
            - create item from \p key;
            - insert new item into the list;
            - if inserting is successful, initialize the value of item by calling \p func functor

            This can be useful if complete initialization of object of \p mapped_type is heavyweight and
            it is preferable that the initialization should be completed only if inserting is successful.
        */
        template <typename K, typename Func>
        bool insert_with( K&& key, Func func )
        {
            return insert_with_at( head(), std::forward<K>( key ), func );
        }

        /// Inserts data of type \ref mapped_type constructed with <tt>std::forward<Args>(args)...</tt>
        /**
            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename... Args>
        bool emplace( Args&&... args )
        {
            return emplace_at( head(), std::forward<Args>(args)... );
        }

        /// Updates data by \p key
        /**
            The operation performs inserting or replacing the element with lock-free manner.

            If the \p key not found in the list, then the new item created from \p key
            will be inserted iff \p bAllowInsert is \p true.
            (note that in this case the \ref key_type should be constructible from type \p K).
            Otherwise, if \p key is found, the functor \p func is called with item found.

            The functor \p Func signature is:
            \code
                struct my_functor {
                    void operator()( bool bNew, value_type& item );
                };
            \endcode
            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - the item found or inserted

            The functor may change any fields of the \p item.second of \p mapped_type;
            during \p func call \p item is locked so it is safe to modify the item in
            multi-threaded environment.

            Returns <tt> std::pair<bool, bool> </tt> where \p first is true if operation is successful,
            \p second is true if new item has been added or \p false if the item with \p key
            already exists.
        */
        template <typename K, typename Func>
        std::pair<bool, bool> update( K&& key, Func f, bool bAllowInsert = true )
        {
            return update_at( head(), std::forward<K>( key ), f, bAllowInsert );
        }
        //@cond
        template <typename K, typename Func>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<bool, bool> ensure( const K& key, Func f )
        {
            return update( key, f, true );
        }
        //@endcond

        /// Deletes \p key from the list
        /** \anchor cds_nonintrusive_LazyKVList_hp_erase_val

            Returns \p true if \p key is found and has been deleted, \p false otherwise
        */
        template <typename K>
        bool erase( K const& key )
        {
            return erase_at( head(), key, intrusive_key_comparator());
        }

        /// Deletes the item from the list using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_LazyKVList_hp_erase_val "erase(K const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename K, typename Less>
        bool erase_with( K const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return erase_at( head(), key, typename maker::template less_wrapper<Less>());
        }

        /// Deletes \p key from the list
        /** \anchor cds_nonintrusive_LazyKVList_hp_erase_func
            The function searches an item with key \p key, calls \p f functor with item found
            and deletes it. If \p key is not found, the functor is not called.

            The functor \p Func interface:
            \code
            struct extractor {
                void operator()(value_type& val) { ... }
            };
            \endcode

            Returns \p true if key is found and deleted, \p false otherwise
        */
        template <typename K, typename Func>
        bool erase( K const& key, Func f )
        {
            return erase_at( head(), key, intrusive_key_comparator(), f );
        }

        /// Deletes the item from the list using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_LazyKVList_hp_erase_func "erase(K const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename K, typename Less, typename Func>
        bool erase_with( K const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return erase_at( head(), key, typename maker::template less_wrapper<Less>(), f );
        }

        /// Extracts the item from the list with specified \p key
        /** \anchor cds_nonintrusive_LazyKVList_hp_extract
            The function searches an item with key equal to \p key,
            unlinks it from the list, and returns it as \p guarded_ptr.
            If \p key is not found the function returns an empty guarded pointer.

            Note the compare functor should accept a parameter of type \p K that can be not the same as \p key_type.

            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.

            Usage:
            \code
            typedef cds::container::LazyKVList< cds::gc::HP, int, foo, my_traits >  ord_list;
            ord_list theList;
            // ...
            {
                ord_list::guarded_ptr gp( theList.extract( 5 ));
                if ( gp ) {
                    // Deal with gp
                    // ...
                }
                // Destructor of gp releases internal HP guard and frees the item
            }
            \endcode
        */
        template <typename K>
        guarded_ptr extract( K const& key )
        {
            return extract_at( head(), key, intrusive_key_comparator());
        }

        /// Extracts the item from the list with comparing functor \p pred
        /**
            The function is an analog of \ref cds_nonintrusive_LazyKVList_hp_extract "extract(K const&)"
            but \p pred predicate is used for key comparing.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref key_type and \p K
            in any order.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename K, typename Less>
        guarded_ptr extract_with( K const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return extract_at( head(), key, typename maker::template less_wrapper<Less>());
        }

        /// Checks whether the list contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.
        */
        template <typename Q>
        bool contains( Q const& key )
        {
            return find_at( head(), key, intrusive_key_comparator());
        }
        //@cond
        template <typename Q>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find( Q const& key )
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
        bool contains( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return find_at( head(), key, typename maker::template less_wrapper<Less>());
        }
        //@cond
        template <typename Q, typename Less>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find_with( Q const& key, Less pred )
        {
            return contains( key, pred );
        }
        //@endcond

        /// Finds the key \p key and performs an action with it
        /** \anchor cds_nonintrusive_LazyKVList_hp_find_func
            The function searches an item with key equal to \p key and calls the functor \p f for the item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item );
            };
            \endcode
            where \p item is the item found.

            The functor may change <tt>item.second</tt> that is reference to value of node.
            Note that the function is only guarantee that \p item cannot be deleted during functor is executing.
            The function does not serialize simultaneous access to the list \p item. If such access is
            possible you must provide your own synchronization schema to exclude unsafe item modifications.

            The function returns \p true if \p key is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q const& key, Func f )
        {
            return find_at( head(), key, intrusive_key_comparator(), f );
        }

        /// Finds the key \p val using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_LazyKVList_hp_find_func "find(Q&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return find_at( head(), key, typename maker::template less_wrapper<Less>(), f );
        }

        /// Finds \p key and return the item found
        /** \anchor cds_nonintrusive_LazyKVList_hp_get
            The function searches the item with key equal to \p key
            and returns the item found as a guarded pointer.
            If \p key is not found the functions returns an empty \p guarded_ptr.

            @note Each \p guarded_ptr object uses one GC's guard which can be limited resource.

            Usage:
            \code
            typedef cds::container::LazyKVList< cds::gc::HP, int, foo, my_traits >  ord_list;
            ord_list theList;
            // ...
            {
                ord_list::guarded_ptr gp( theList.get( 5 ));
                if ( gp ) {
                    // Deal with gp
                    //...
                }
                // Destructor of guarded_ptr releases internal HP guard and frees the item
            }
            \endcode

            Note the compare functor specified for class \p Traits template parameter
            should accept a parameter of type \p K that can be not the same as \p key_type.
        */
        template <typename K>
        guarded_ptr get( K const& key )
        {
            return get_at( head(), key, intrusive_key_comparator());
        }

        /// Finds the key \p val and return the item found
        /**
            The function is an analog of \ref cds_nonintrusive_LazyKVList_hp_get "get(K const&)"
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref key_type and \p K
            in any order.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename K, typename Less>
        guarded_ptr get_with( K const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return get_at( head(), key, typename maker::template less_wrapper<Less>());
        }

        /// Checks if the list is empty
        bool empty() const
        {
            return base_class::empty();
        }

        /// Returns list's item count
        /**
            The value returned depends on opt::item_counter option. For atomicity::empty_item_counter,
            this function always returns 0.

            @note Even if you use real item counter and it returns 0, this fact is not mean that the list
            is empty. To check list emptyness use \ref empty() method.
        */
        size_t size() const
        {
            return base_class::size();
        }

        /// Returns const reference to internal statistics
        stat const& statistics() const
        {
            return base_class::statistics();
        }

        /// Clears the list
        void clear()
        {
            base_class::clear();
        }

    protected:
        //@cond
        bool insert_node_at( head_type& refHead, node_type * pNode )
        {
            assert( pNode != nullptr );
            scoped_node_ptr p( pNode );

            if ( base_class::insert_at( &refHead, *p )) {
                p.release();
                return true;
            }

            return false;
        }

        template <typename K>
        bool insert_at( head_type& refHead, K&& key )
        {
            return insert_node_at( refHead, alloc_node( std::forward<K>( key )));
        }

        template <typename K, typename V>
        bool insert_at( head_type& refHead, K&& key, V&& val )
        {
            return insert_node_at( refHead, alloc_node( std::forward<K>( key ), std::forward<V>( val )));
        }

        template <typename K, typename Func>
        bool insert_with_at( head_type& refHead, K&& key, Func f )
        {
            scoped_node_ptr pNode( alloc_node( std::forward<K>( key )));

            if ( base_class::insert_at( &refHead, *pNode, [&f](node_type& node){ f( node.m_Data ); } )) {
                pNode.release();
                return true;
            }
            return false;
        }

        template <typename... Args>
        bool emplace_at( head_type& refHead, Args&&... args )
        {
            return insert_node_at( refHead, alloc_node( std::forward<Args>(args)... ));
        }

        template <typename K, typename Compare>
        bool erase_at( head_type& refHead, K const& key, Compare cmp )
        {
            return base_class::erase_at( &refHead, key, cmp );
        }

        template <typename K, typename Compare, typename Func>
        bool erase_at( head_type& refHead, K const& key, Compare cmp, Func f )
        {
            return base_class::erase_at( &refHead, key, cmp, [&f](node_type const & node){f( const_cast<value_type&>(node.m_Data)); });
        }

        template <typename K, typename Compare>
        guarded_ptr extract_at( head_type& refHead, K const& key, Compare cmp )
        {
            return base_class::extract_at( &refHead, key, cmp );
        }

        template <typename K, typename Func>
        std::pair<bool, bool> update_at( head_type& refHead, K&& key, Func f, bool bAllowInsert )
        {
            scoped_node_ptr pNode( alloc_node( std::forward<K>( key )));

            std::pair<bool, bool> ret = base_class::update_at( &refHead, *pNode,
                [&f]( bool bNew, node_type& node, node_type& ){ f( bNew, node.m_Data ); },
                bAllowInsert );
            if ( ret.first && ret.second )
                pNode.release();

            return ret;
        }

        template <typename K, typename Compare>
        bool find_at( head_type& refHead, K const& key, Compare cmp )
        {
            return base_class::find_at( &refHead, key, cmp );
        }

        template <typename K, typename Compare, typename Func>
        bool find_at( head_type& refHead, K& key, Compare cmp, Func f )
        {
            return base_class::find_at( &refHead, key, cmp, [&f]( node_type& node, K& ){ f( node.m_Data ); });
        }

        template <typename K, typename Compare>
        guarded_ptr get_at( head_type& refHead, K const& key, Compare cmp )
        {
            return base_class::get_at( &refHead, key, cmp );
        }

        //@endcond
    };

}}  // namespace cds::container

#endif  // #ifndef CDSLIB_CONTAINER_IMPL_LAZY_KVLIST_H
