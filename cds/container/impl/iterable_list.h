// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_IMPL_ITERABLE_LIST_H
#define CDSLIB_CONTAINER_IMPL_ITERABLE_LIST_H

#include <cds/container/details/make_iterable_list.h>
#include <memory>

namespace cds { namespace container {

    /// Iterable ordered list
    /** @ingroup cds_nonintrusive_list
        \anchor cds_nonintrusive_IterableList_gc

        This lock-free list implementation supports thread-safe iterators.

        Usually, ordered single-linked list is used as a building block for the hash table implementation.
        Iterable list is suitable for almost append-only hash table because the list doesn't delete
        its internal node when erasing a key but it is marked them as empty to be reused in the future.
        However, plenty of empty nodes degrades performance.

        The complexity of searching is <tt>O(N)</tt>.

        Template arguments:
        - \p GC - Garbage collector used.
        - \p T - type to be stored in the list.
        - \p Traits - type traits, default is \p iterable_list::traits.

        Unlike standard container, this implementation does not divide type \p T into key and value part and
        may be used as a main building block for hash set algorithms.
        The key is a function (or a part) of type \p T, and this function is specified by <tt>Traits::compare</tt> functor
        or <tt>Traits::less</tt> predicate.

        \p IterableKVList is a key-value version of iterable non-intrusive list that is closer to the C++ std library approach.

        It is possible to declare option-based list with cds::container::iterable_list::make_traits metafunction istead of \p Traits template
        argument. For example, the following traits-based declaration of gc::HP iterable list
        \code
        #include <cds/container/iterable_list_hp.h>
        // Declare comparator for the item
        struct my_compare {
            int operator ()( int i1, int i2 )
            {
                return i1 - i2;
            }
        };

        // Declare traits
        struct my_traits: public cds::container::iterable_list::traits
        {
            typedef my_compare compare;
        };

        // Declare traits-based list
        typedef cds::container::IterableList< cds::gc::HP, int, my_traits >     traits_based_list;
        \endcode

        is equivalent for the following option-based list
        \code
        #include <cds/container/iterable_list_hp.h>

        // my_compare is the same

        // Declare option-based list
        typedef cds::container::IterableList< cds::gc::HP, int,
            typename cds::container::iterable_list::make_traits<
                cds::container::opt::compare< my_compare >     // item comparator option
            >::type
        > option_based_list;
        \endcode

        \par Usage
        There are different specializations of this template for each garbage collecting schema used.
        You should include appropriate .h-file depending on GC you are using:
        - for gc::HP: \code #include <cds/container/iterable_list_hp.h> \endcode
        - for gc::DHP: \code #include <cds/container/iterable_list_dhp.h> \endcode
        - for \ref cds_urcu_desc "RCU": \code #include <cds/container/iterable_list_rcu.h> \endcode
    */
    template <
        typename GC,
        typename T,
#ifdef CDS_DOXYGEN_INVOKED
        typename Traits = iterable_list::traits
#else
        typename Traits
#endif
    >
    class IterableList:
#ifdef CDS_DOXYGEN_INVOKED
        protected intrusive::IterableList< GC, T, Traits >
#else
        protected details::make_iterable_list< GC, T, Traits >::type
#endif
    {
        //@cond
        typedef details::make_iterable_list< GC, T, Traits > maker;
        typedef typename maker::type base_class;
        //@endcond

    public:
        typedef T value_type;   ///< Type of value stored in the list
        typedef Traits traits;  ///< List traits

        typedef typename base_class::gc             gc;             ///< Garbage collector used
        typedef typename base_class::back_off       back_off;       ///< Back-off strategy used
        typedef typename maker::data_allocator_type allocator_type; ///< Allocator type used for allocate/deallocate data
        typedef typename base_class::item_counter   item_counter;   ///< Item counting policy used
        typedef typename maker::key_comparator      key_comparator; ///< key comparison functor
        typedef typename base_class::memory_model   memory_model;   ///< Memory ordering. See \p cds::opt::memory_model option
        typedef typename base_class::stat           stat;           ///< Internal statistics

        static constexpr const size_t c_nHazardPtrCount = base_class::c_nHazardPtrCount; ///< Count of hazard pointer required for the algorithm

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
        using select_stat_wrapper = typename base_class::template select_stat_wrapper< Stat >;
        //@endcond

    protected:
        //@cond
        typedef typename maker::cxx_data_allocator  cxx_data_allocator;
        typedef typename maker::data_disposer       data_disposer;
        typedef typename base_class::node_type      head_type;
        //@endcond

    public:
        /// Guarded pointer
        typedef typename base_class::guarded_ptr guarded_ptr;

    protected:
        //@cond
        template <bool IsConst>
        class iterator_type: protected base_class::template iterator_type<IsConst>
        {
            typedef typename base_class::template iterator_type<IsConst> iterator_base;
            friend class IterableList;

            iterator_type( iterator_base it )
                : iterator_base( it )
            {}

        public:
            typedef typename iterator_base::value_ptr value_ptr;
            typedef typename iterator_base::value_ref value_ref;

            iterator_type()
            {}

            iterator_type( iterator_type const& src )
                : iterator_base( src )
            {}

            value_ptr operator ->() const
            {
                return iterator_base::operator ->();
            }

            value_ref operator *() const
            {
                return iterator_base::operator *();
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
            - Iterator is thread-safe: even if an element the iterator points to is removed, the iterator stays valid because
              it contains the guard keeping the value from to be recycled.

            The iterator interface:
            \code
            class iterator {
            public:
                // Default constructor
                iterator();

                // Copy constructor
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
            return iterator( base_class::begin());
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
            return iterator( base_class::end());
        }

        /// Returns a forward const iterator addressing the first element in a list
        const_iterator begin() const
        {
            return const_iterator( base_class::cbegin());
        }

        /// Returns a forward const iterator addressing the first element in a list
        const_iterator cbegin() const
        {
            return const_iterator( base_class::cbegin());
        }

        /// Returns an const iterator that addresses the location succeeding the last element in a list
        const_iterator end() const
        {
            return const_iterator( base_class::cend());
        }

        /// Returns an const iterator that addresses the location succeeding the last element in a list
        const_iterator cend() const
        {
            return const_iterator( base_class::cend());
        }
    //@}

    public:
        /// Default constructor
        /**
            Initialize empty list
        */
        IterableList()
        {}

        //@cond
        template <typename Stat, typename = std::enable_if<std::is_same<stat, iterable_list::wrapped_stat<Stat>>::value >>
        explicit IterableList( Stat& st )
            : base_class( st )
        {}
        //@endcond

        /// List destructor
        /**
            Clears the list
        */
        ~IterableList()
        {}

        /// Inserts new node
        /**
            The function creates a node with copy of \p val value
            and then inserts the node created into the list.

            The type \p Q should contain least the complete key of the node.
            The object of \ref value_type should be constructible from \p val of type \p Q.
            In trivial case, \p Q is equal to \ref value_type.

            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename Q>
        bool insert( Q&& val )
        {
            return insert_at( head(), std::forward<Q>( val ));
        }

        /// Inserts new node
        /**
            This function inserts new node with default-constructed value and then it calls
            \p func functor with signature
            \code
            void func( value_type& data );
            \endcode

            The argument \p data of user-defined functor \p func is the reference
            to the list's item inserted. User-defined functor \p func should guarantee that during changing
            item's value no any other changes could be made on this list's item by concurrent threads.
            The user-defined functor is called only if inserting is success.

            The type \p Q should contain the complete key of the node.
            The object of \p value_type should be constructible from \p key of type \p Q.

            The function allows to split creating of new item into two part:
            - create item from \p key with initializing key-fields only;
            - insert new item into the list;
            - if inserting is successful, initialize non-key fields of item by calling \p func functor

            The method can be useful if complete initialization of object of \p value_type is heavyweight and
            it is preferable that the initialization should be completed only if inserting is successful.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"
        */
        template <typename Q, typename Func>
        bool insert( Q&& key, Func func )
        {
            return insert_at( head(), std::forward<Q>( key ), func );
        }

        /// Updates data by \p key
        /**
            The operation performs inserting or replacing the element with lock-free manner.

            If the \p key not found in the list, then the new item created from \p key
            will be inserted iff \p bAllowInsert is \p true.
            Otherwise, if \p key is found, the functor \p func is called with item found.

            The functor \p func is called after inserting or replacing, it signature is:
            \code
                void func( value_type& val, value_type * old );
            \endcode
            where
            - \p val - a new data constructed from \p key
            - \p old - old value that will be retired. If new item has been inserted then \p old is \p nullptr.

            The functor may change non-key fields of \p val; however, \p func must guarantee
            that during changing no any other modifications could be made on this item by concurrent threads.

            Returns <tt> std::pair<bool, bool> </tt> where \p first is true if operation is successful,
            \p second is true if new item has been added or \p false if the item with such \p key
            already exists.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"
        */
        template <typename Q, typename Func>
        std::pair<bool, bool> update( Q&& key, Func func, bool bAllowInsert = true )
        {
            return update_at( head(), std::forward<Q>( key ), func, bAllowInsert );
        }

        /// Insert or update
        /**
            The operation performs inserting or updating data with lock-free manner.

            If the item \p key is not found in the list, then \p key is inserted
            iff \p bInsert is \p true.
            Otherwise, the current element is changed to \p key, the old element will be retired later.

            \p value_type should be constructible from \p key.

            Returns std::pair<bool, bool> where \p first is \p true if operation is successful,
            \p second is \p true if \p key has been added or \p false if the item with that key
            already in the list.
        */
        template <typename Q>
        std::pair<bool, bool> upsert( Q&& key, bool bInsert = true )
        {
            return update_at( head(), std::forward<Q>( key ), []( value_type&, value_type* ) {}, bInsert );
        }

        /// Inserts data of type \p value_type constructed with <tt>std::forward<Args>(args)...</tt>
        /**
            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename... Args>
        bool emplace( Args&&... args )
        {
            return emplace_at( head(), std::forward<Args>(args)... );
        }

        /// Delete \p key from the list
        /**
            Since the key of IterableList's item type \p value_type is not explicitly specified,
            template parameter \p Q sould contain the complete key to search in the list.
            The list item comparator should be able to compare the type \p value_type
            and the type \p Q.

            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename Q>
        bool erase( Q const& key )
        {
            return erase_at( head(), key, key_comparator(), [](value_type const&){} );
        }

        /// Deletes the item from the list using \p pred predicate for searching
        /**
            The function is an analog of \p erase(Q const&) but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        bool erase_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return erase_at( head(), key, typename maker::template less_wrapper<Less>(), [](value_type const&){} );
        }

        /// Deletes \p key from the list
        /**
            The function searches an item with key \p key, calls \p f functor with item found
            and deletes it. If \p key is not found, the functor is not called.

            The functor \p Func interface:
            \code
            struct extractor {
                void operator()(const value_type& val) { ... }
            };
            \endcode

            Since the key of IterableList's item type \p value_type is not explicitly specified,
            template parameter \p Q should contain the complete key to search in the list.
            The list item comparator should be able to compare the type \p value_type of list item
            and the type \p Q.

            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename Q, typename Func>
        bool erase( Q const& key, Func f )
        {
            return erase_at( head(), key, key_comparator(), f );
        }

        /// Deletes the item from the list using \p pred predicate for searching
        /**
            The function is an analog of \p erase(Q const&, Func) but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less, typename Func>
        bool erase_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return erase_at( head(), key, typename maker::template less_wrapper<Less>(), f );
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
            return base_class::erase_at( iter );
        }

        /// Extracts the item from the list with specified \p key
        /**
            The function searches an item with key equal to \p key,
            unlinks it from the list, and returns it as \p guarded_ptr.
            If \p key is not found the function returns an empty guarded pointer.

            Note the compare functor should accept a parameter of type \p Q that can be not the same as \p value_type.

            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.

            Usage:
            \code
            typedef cds::container::IterableList< cds::gc::HP, foo, my_traits >  ord_list;
            ord_list theList;
            // ...
            {
                ord_list::guarded_ptr gp(theList.extract( 5 ));
                if ( gp ) {
                    // Deal with gp
                    // ...
                }
                // Destructor of gp releases internal HP guard and frees the item
            }
            \endcode
        */
        template <typename Q>
        guarded_ptr extract( Q const& key )
        {
            return extract_at( head(), key, key_comparator());
        }

        /// Extracts the item from the list with comparing functor \p pred
        /**
            The function is an analog of \p extract(Q const&) but \p pred predicate is used for key comparing.

            \p Less functor has the semantics like \p std::less but it should accept arguments
            of type \p value_type and \p Q in any order.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        guarded_ptr extract_with( Q const& key, Less pred )
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
        bool contains( Q const& key ) const
        {
            return find_at( head(), key, key_comparator());
        }

        /// Checks whether the list contains \p key using \p pred predicate for searching
        /**
            The function is an analog of <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        bool contains( Q const& key, Less pred ) const
        {
            CDS_UNUSED( pred );
            return find_at( head(), key, typename maker::template less_wrapper<Less>());
        }

        /// Finds \p key and perform an action with it
        /**
            The function searches an item with key equal to \p key and calls the functor \p f for the item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q& key );
            };
            \endcode
            where \p item is the item found, \p key is the <tt>find</tt> function argument.

            The functor may change non-key fields of \p item. Note that the function is only guarantee
            that \p item cannot be deleted during functor is executing.
            The function does not serialize simultaneous access to the list \p item. If such access is
            possible you must provide your own synchronization schema to exclude unsafe item modifications.

            The function returns \p true if \p key is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q& key, Func f ) const
        {
            return find_at( head(), key, key_comparator(), f );
        }
        //@cond
        template <typename Q, typename Func>
        bool find( Q const& key, Func f ) const
        {
            return find_at( head(), key, key_comparator(), f );
        }
        //@endcond

        /// Finds \p key in the list and returns iterator pointed to the item found
        /**
            If \p key is not found the function returns \p end().
        */
        template <typename Q>
        iterator find( Q const& key ) const
        {
            return find_iterator_at( head(), key, key_comparator());
        }

        /// Finds \p key using \p pred predicate for searching
        /**
            The function is an analog of \p find(Q&, Func) but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q& key, Less pred, Func f ) const
        {
            CDS_UNUSED( pred );
            return find_at( head(), key, typename maker::template less_wrapper<Less>(), f );
        }
        //@cond
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& key, Less pred, Func f ) const
        {
            CDS_UNUSED( pred );
            return find_at( head(), key, typename maker::template less_wrapper<Less>(), f );
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
            return find_iterator_at( head(), key, cds::opt::details::make_comparator_from_less<Less>());
        }

        /// Finds \p key and return the item found
        /** \anchor cds_nonintrusive_MichaelList_hp_get
            The function searches the item with key equal to \p key
            and returns it as \p guarded_ptr.
            If \p key is not found the function returns an empty guarded pointer.

            @note Each \p guarded_ptr object uses one GC's guard which can be limited resource.

            Usage:
            \code
            typedef cds::container::MichaelList< cds::gc::HP, foo, my_traits >  ord_list;
            ord_list theList;
            // ...
            {
                ord_list::guarded_ptr gp(theList.get( 5 ));
                if ( gp ) {
                    // Deal with gp
                    //...
                }
                // Destructor of guarded_ptr releases internal HP guard and frees the item
            }
            \endcode

            Note the compare functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that can be not the same as \p value_type.
        */
        template <typename Q>
        guarded_ptr get( Q const& key ) const
        {
            return get_at( head(), key, key_comparator());
        }

        /// Finds \p key and return the item found
        /**
            The function is an analog of \ref cds_nonintrusive_MichaelList_hp_get "get( Q const&)"
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should accept arguments of type \p value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        guarded_ptr get_with( Q const& key, Less pred ) const
        {
            CDS_UNUSED( pred );
            return get_at( head(), key, typename maker::template less_wrapper<Less>());
        }

        /// Checks if the list is empty
        /**
            Emptiness is checked by item counting: if item count is zero then the set is empty.
            Thus, if you need to use \p %empty() you should provide appropriate (non-empty) \p iterable_list::traits::item_counter
            feature.
        */
        bool empty() const
        {
            return base_class::empty();
        }

        /// Returns list's item count
        /**
            The value returned depends on item counter provided by \p Traits. For \p atomicity::empty_item_counter,
            this function always returns 0.
        */
        size_t size() const
        {
            return base_class::size();
        }

        /// Clears the list (thread safe, not atomic)
        void clear()
        {
            base_class::clear();
        }

        /// Returns const reference to internal statistics
        stat const& statistics() const
        {
            return base_class::statistics();
        }

    protected:
        //@cond
        template <typename... Args>
        static value_type* alloc_data( Args&&... args )
        {
            return cxx_data_allocator().MoveNew( std::forward<Args>(args)... );
        }

        static void free_data( value_type* pData )
        {
            cxx_data_allocator().Delete( pData );
        }

        typedef std::unique_ptr< value_type, data_disposer > scoped_data_ptr;

        using base_class::head;
        //@endcond

    protected:
        //@cond
        bool insert_node( value_type* pData )
        {
            return insert_node_at( head(), pData );
        }

        bool insert_node_at( head_type* pHead, value_type* pData )
        {
            assert( pData );
            scoped_data_ptr p( pData );
            if ( base_class::insert_at( pHead, *pData )) {
                p.release();
                return true;
            }

            return false;
        }

        template <typename Q>
        bool insert_at( head_type* pHead, Q&& val )
        {
            return insert_node_at( pHead, alloc_data( std::forward<Q>( val )));
        }

        template <typename Q, typename Func>
        bool insert_at( head_type* pHead, Q&& key, Func f )
        {
            scoped_data_ptr pNode( alloc_data( std::forward<Q>( key )));

            if ( base_class::insert_at( pHead, *pNode, f )) {
                pNode.release();
                return true;
            }
            return false;
        }

        template <typename... Args>
        bool emplace_at( head_type* pHead, Args&&... args )
        {
            return insert_node_at( pHead, alloc_data( std::forward<Args>(args)... ));
        }

        template <typename Q, typename Func>
        std::pair<bool, bool> update_at( head_type* pHead, Q&& key, Func f, bool bAllowInsert )
        {
            scoped_data_ptr pData( alloc_data( std::forward<Q>( key )));

            std::pair<bool, bool> ret = base_class::update_at( pHead, *pData, f, bAllowInsert );
            if ( ret.first )
                pData.release();

            return ret;
        }

        template <typename Q, typename Compare, typename Func>
        bool erase_at( head_type* pHead, Q const& key, Compare cmp, Func f )
        {
            return base_class::erase_at( pHead, key, cmp, f );
        }

        template <typename Q, typename Compare>
        guarded_ptr extract_at( head_type* pHead, Q const& key, Compare cmp )
        {
            return base_class::extract_at( pHead, key, cmp );
        }

        template <typename Q, typename Compare>
        bool find_at( head_type const* pHead, Q const& key, Compare cmp ) const
        {
            return base_class::find_at( pHead, key, cmp );
        }

        template <typename Q, typename Compare, typename Func>
        bool find_at( head_type const* pHead, Q& val, Compare cmp, Func f ) const
        {
            return base_class::find_at( pHead, val, cmp, f );
        }

        template <typename Q, typename Compare>
        iterator find_iterator_at( head_type const* pHead, Q const& key, Compare cmp ) const
        {
            return iterator( base_class::find_iterator_at( pHead, key, cmp ));
        }

        template <typename Q, typename Compare>
        guarded_ptr get_at( head_type const* pHead, Q const& key, Compare cmp ) const
        {
            return base_class::get_at( pHead, key, cmp );
        }
        //@endcond
    };

}}  // namespace cds::container

#endif  // #ifndef CDSLIB_CONTAINER_IMPL_ITERABLE_LIST_H
