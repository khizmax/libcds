// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_IMPL_ITERABLE_KVLIST_H
#define CDSLIB_CONTAINER_IMPL_ITERABLE_KVLIST_H

#include <memory>
#include <cds/container/details/guarded_ptr_cast.h>

namespace cds { namespace container {

    /// Iterable ordered list for key-value pair
    /** @ingroup cds_nonintrusive_list
        \anchor cds_nonintrusive_IterableKVList_gc

        This is key-value variation of non-intrusive \p IterableList.
        Like standard container, this implementation split a value stored into two part -
        constant key and alterable value.

        Usually, ordered single-linked list is used as a building block for the hash table implementation.
        Iterable list is suitable for almost append-only hash table because the list doesn't delete
        its internal node when erasing a key but it is marked them as empty to be reused in the future.
        However, plenty of empty nodes degrades performance.

        The complexity of searching is <tt>O(N)</tt>.

        Template arguments:
        - \p GC - garbage collector used
        - \p Key - key type of an item stored in the list. It should be copy-constructible
        - \p Value - value type stored in a list
        - \p Traits - type traits, default is \p iterable_list::traits

        It is possible to declare option-based list with \p cds::container::iterable_list::make_traits metafunction instead of \p Traits template
        argument. For example, the following traits-based declaration of \p gc::HP iterable list
        \code
        #include <cds/container/iterable_kvlist_hp.h>
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
        typedef cds::container::IterableKVList< cds::gc::HP, int, int, my_traits > traits_based_list;
        \endcode
        is equivalent for the following option-based list
        \code
        #include <cds/container/iterable_kvlist_hp.h>

        // my_compare is the same

        // Declare option-based list
        typedef cds::container::IterableKVList< cds::gc::HP, int, int,
            typename cds::container::iterable_list::make_traits<
                cds::container::opt::compare< my_compare >     // item comparator option
            >::type
        >     option_based_list;
        \endcode

        \par Usage
        There are different specializations of this template for each garbage collecting schema used.
        You should include appropriate .h-file depending on GC you are using:
        - for gc::HP: \code #include <cds/container/iterable_kvlist_hp.h> \endcode
        - for gc::DHP: \code #include <cds/container/iterable_kvlist_dhp.h> \endcode
        - for \ref cds_urcu_desc "RCU": \code #include <cds/container/iterable_kvlist_rcu.h> \endcode
    */
    template <
        typename GC,
        typename Key,
        typename Value,
#ifdef CDS_DOXYGEN_INVOKED
        typename Traits = iterable_list::traits
#else
        typename Traits
#endif
    >
    class IterableKVList:
#ifdef CDS_DOXYGEN_INVOKED
        protected container::IterableList< GC, std::pair<Key, Value>, Traits >
#else
        protected details::make_iterable_kvlist< GC, Key, Value, Traits >::type
#endif
    {
        //@cond
        typedef details::make_iterable_kvlist< GC, Key, Value, Traits > maker;
        typedef typename maker::type base_class;
        //@endcond

    public:
#ifdef CDS_DOXYGEN_INVOKED
        typedef Key                                 key_type;      ///< Key type
        typedef Value                               mapped_type;   ///< Type of value stored in the list
        typedef std::pair<key_type const, mapped_type> value_type; ///< key/value pair stored in the list
#else
        typedef typename maker::key_type    key_type;
        typedef typename maker::mapped_type mapped_type;
        typedef typename maker::value_type  value_type;
#endif
        typedef Traits traits;  ///< List traits
        typedef typename base_class::gc           gc;             ///< Garbage collector used
        typedef typename base_class::back_off     back_off;       ///< Back-off strategy used
        typedef typename maker::data_allocator_type allocator_type; ///< Allocator type used for allocate/deallocate data
        typedef typename base_class::item_counter item_counter;   ///< Item counting policy used
        typedef typename maker::key_comparator    key_comparator; ///< key comparison functor
        typedef typename base_class::memory_model memory_model;   ///< Memory ordering. See cds::opt::memory_model option
        typedef typename base_class::stat         stat;           ///< Internal statistics

        static constexpr const size_t c_nHazardPtrCount = base_class::c_nHazardPtrCount; ///< Count of hazard pointer required for the algorithm

        /// Guarded pointer
        typedef typename base_class::guarded_ptr guarded_ptr;

        //@cond
        // Rebind traits (split-list support)
        template <typename... Options>
        struct rebind_traits {
            typedef IterableKVList<
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
        typedef typename base_class::head_type     head_type;
        typedef typename maker::cxx_data_allocator cxx_data_allocator;

        template <typename Less>
        using less_wrapper = typename maker::template less_wrapper< Less >;

        template <bool IsConst>
        using iterator_type = typename base_class::template iterator_type<IsConst>;
        //@endcond

    public:
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
        using typename base_class::iterator;
        using typename base_class::const_iterator;
        using base_class::begin;
        using base_class::end;
        using base_class::cbegin;
        using base_class::cend;

    public:
        /// Default constructor
        /**
            Initializes empty list
        */
        IterableKVList()
        {}

        //@cond
        template <typename Stat, typename = std::enable_if<std::is_same<stat, iterable_list::wrapped_stat<Stat>>::value >>
        explicit IterableKVList( Stat& st )
            : base_class( st )
        {}
        //@endcond

        /// List destructor
        /**
            Clears the list
        */
        ~IterableKVList()
        {}

        /// Inserts new node with key and default value
        /**
            The function creates a node with \p key and default value, and then inserts the node created into the list.

            Preconditions:
            - The \p key_type should be constructible from value of type \p K. In trivial case, \p K is equal to \p key_type.
            - The \p mapped_type should be default-constructible.

            Returns \p true if inserting successful, \p false otherwise.

            @note The function is supported only if \ref mapped_type is default constructible
        */
        template <typename K>
        bool insert( K&& key )
        {
            return base_class::emplace( key_type( std::forward<K>( key )), mapped_type());
        }

        /// Inserts new node with a key and a value
        /**
            The function creates a node with \p key and value \p val, and then inserts the node created into the list.

            Preconditions:
            - The \p key_type should be constructible from \p key of type \p K.
            - The \p mapped_type should be constructible from \p val of type \p V.

            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename K, typename V>
        bool insert( K&& key, V&& val )
        {
            return base_class::emplace( key_type( std::forward<K>( key )), mapped_type( std::forward<V>( val )));
        }

        /// Inserts new node and initialize it by a functor
        /**
            This function inserts new node with key \p key and if inserting is successful then it calls
            \p func functor with signature
            \code
                struct functor {
                    void operator()( value_type& item );
                };
            \endcode

            The argument \p item of user-defined functor \p func is the reference
            to the item inserted. <tt>item.second</tt> is a reference to item's value that may be changed.
            User-defined functor \p func should guarantee that during changing item's value no any other changes
            could be made on this list's item by concurrent threads.
            The user-defined functor is called only if inserting is successful.

            The \p key_type should be constructible from value of type \p K.

            The function allows to split creating of new item into two part:
            - create a new item from \p key;
            - insert the new item into the list;
            - if inserting is successful, initialize the value of item by calling \p func functor

            This can be useful if complete initialization of object of \p mapped_type is heavyweight and
            it is preferable that the initialization should be completed only if inserting is successful.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"

            @note The function is supported only if \ref mapped_type is default constructible
        */
        template <typename K, typename Func>
        bool insert_with( K&& key, Func func )
        {
            return base_class::insert( value_type( key_type( std::forward<K>( key )), mapped_type()), func );
        }

        /// Updates data by \p key
        /**
            The operation performs inserting or replacing the element with lock-free manner.

            If the \p key not found in the list, then the new item created from \p key
            will be inserted iff \p bAllowInsert is \p true.
            (note that in this case the \ref key_type should be constructible from type \p K).
            Otherwise, if \p key is found, the functor \p func is called with item found.

            The functor \p func is called after inserting or replacing, it signature is:
            \code
                void func( value_type& val, value_type* old );
            \endcode
            where
            - \p val - a new data constructed from \p key
            - \p old - old value that will be retired. If new item has been inserted then \p old is \p nullptr.

            The functor may change non-key fields of \p val; however, \p func must guarantee
            that during changing no any other modifications could be made on this item by concurrent threads.

            @return <tt> std::pair<bool, bool> </tt> where \p first is true if operation is successful,
            \p second is true if new item has been added or \p false if the item with such \p key
            already exists.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"

            @note The function is supported only if \ref mapped_type is default constructible
        */
        template <typename K, typename Func>
        std::pair<bool, bool> update( K&& key, Func f, bool bAllowInsert = true )
        {
            return base_class::update( value_type( key_type( std::forward<K>( key )), mapped_type()), f, bAllowInsert );
        }

        /// Insert or update
        /**
            The operation performs inserting or updating data with lock-free manner.

            If the item \p key is not found in the list, then \p key is inserted
            iff \p bInsert is \p true.
            Otherwise, the current element is changed to <tt> value_type( key, val )</tt>,
            the old element will be retired later.

            Returns std::pair<bool, bool> where \p first is \p true if operation is successful,
            \p second is \p true if \p key has been added or \p false if the item with that key
            already in the list.
        */
        template <typename Q, typename V >
        std::pair<bool, bool> upsert( Q&& key, V&& val, bool bInsert = true )
        {
            return base_class::upsert( value_type( key_type( std::forward<Q>( key )), mapped_type( std::forward<V>( val ))), bInsert );
        }

        /// Inserts a new node using move semantics
        /**
            \p key_type field of new item is constructed from \p key argument,
            \p mapped_type field is done from \p args.

            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename K, typename... Args>
        bool emplace( K&& key, Args&&... args )
        {
            return base_class::emplace( key_type( std::forward<K>( key )), mapped_type( std::forward<Args>( args )... ));
        }

        /// Deletes \p key from the list
        /**

            Returns \p true if \p key is found and has been deleted, \p false otherwise
        */
        template <typename K>
        bool erase( K const& key )
        {
            return base_class::erase( key );
        }

        /// Deletes the item from the list using \p pred predicate for searching
        /**
            The function is an analog of \p erase(K const&) but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename K, typename Less>
        bool erase_with( K const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return base_class::erase_with( key, less_wrapper<Less>());
        }

        /// Deletes \p key from the list
        /**
            The function searches an item with key \p key, calls \p f functor
            and deletes the item. If \p key is not found, the functor is not called.

            The functor \p Func interface:
            \code
            struct extractor {
                void operator()(value_type& val) { ... }
            };
            \endcode

            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename K, typename Func>
        bool erase( K const& key, Func f )
        {
            return base_class::erase( key, f );
        }

        /// Deletes the item from the list using \p pred predicate for searching
        /**
            The function is an analog of \p erase(K const&, Func) but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename K, typename Less, typename Func>
        bool erase_with( K const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return base_class::erase_with( key, less_wrapper<Less>(), f );
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

            Note the compare functor should accept a parameter of type \p K that can be not the same as \p key_type.

            The \p disposer specified in \p Traits class template parameter is called automatically
            by garbage collector \p GC specified in class' template parameters when returned \p guarded_ptr object
            will be destroyed or released.
            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.

            Usage:
            \code
            typedef cds::container::IterableKVList< cds::gc::HP, int, foo, my_traits >  ord_list;
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
        template <typename K>
        guarded_ptr extract( K const& key )
        {
            return base_class::extract( key );
        }

        /// Extracts the item from the list with comparing functor \p pred
        /**
            The function is an analog of \p extract(K const&) but \p pred predicate is used for key comparing.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref key_type and \p K
            in any order.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename K, typename Less>
        guarded_ptr extract_with( K const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return base_class::extract_with( key, less_wrapper<Less>());
        }

        /// Checks whether the list contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.
        */
        template <typename Q>
        bool contains( Q const& key ) const
        {
            return base_class::contains( key );
        }

        /// Checks whether the map contains \p key using \p pred predicate for searching
        /**
            The function is an analog of <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        bool contains( Q const& key, Less pred ) const
        {
            CDS_UNUSED( pred );
            return base_class::contains( key, less_wrapper<Less>());
        }

        /// Finds the key \p key and performs an action with it
        /**
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
        bool find( Q const& key, Func f ) const
        {
            return base_class::find( key, [&f]( value_type& v, Q const& ) { f( v ); } );
        }

        /// Finds \p key in the list and returns iterator pointed to the item found
        /**
            If \p key is not found the function returns \p end().
        */
        template <typename Q>
        iterator find( Q const& key ) const
        {
            return base_class::find( key );
        }

        /// Finds the key \p val using \p pred predicate for searching
        /**
            The function is an analog of \p find(Q&, Func) but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& key, Less pred, Func f ) const
        {
            CDS_UNUSED( pred );
            return base_class::find_with( key, less_wrapper<Less>(), [&f]( value_type& v, Q const& ) { f( v ); } );
        }

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
            return base_class::find_with( key, less_wrapper<Less>());
        }

        /// Finds the \p key and return the item found
        /**
            The function searches the item with key equal to \p key
            and returns it as \p guarded_ptr.
            If \p key is not found the function returns an empty guarded pointer.

            @note Each \p guarded_ptr object uses one GC's guard which can be limited resource.

            Usage:
            \code
            typedef cds::container::IterableKVList< cds::gc::HP, int, foo, my_traits >  ord_list;
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

            Note the compare functor specified for class \p Traits template parameter
            should accept a parameter of type \p K that can be not the same as \p key_type.
        */
        template <typename K>
        guarded_ptr get( K const& key ) const
        {
            return base_class::get( key );
        }

        /// Finds the \p key and return the item found
        /**
            The function is an analog of \p get( guarded_ptr& ptr, K const&)
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref key_type and \p K
            in any order.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename K, typename Less>
        guarded_ptr get_with( K const& key, Less pred ) const
        {
            CDS_UNUSED( pred );
            return base_class::get_with( key, less_wrapper<Less>());
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

        /// Clears the list
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
        // Split-list support

        template <typename K>
        bool insert_at( head_type& refHead, K&& key )
        {
            return base_class::insert_at( refHead, value_type( key_type( std::forward<K>( key )), mapped_type()));
        }

        template <typename K, typename V>
        bool insert_at( head_type& refHead, K&& key, V&& val )
        {
            return base_class::insert_at( refHead, value_type( key_type( std::forward<K>( key )), std::forward<V>( val )));
        }

        template <typename K, typename Func>
        bool insert_with_at( head_type& refHead, K&& key, Func f )
        {
            return base_class::insert_at( refHead, value_type( key_type( std::forward<K>( key )), mapped_type()), f );
        }

        template <typename K, typename... Args>
        bool emplace_at( head_type& refHead, K&& key, Args&&... args )
        {
            return base_class::emplace_at( refHead, std::forward<K>(key), std::forward<Args>(args)... );
        }

        template <typename K, typename Func>
        std::pair<bool, bool> update_at( head_type& refHead, K&& key, Func f, bool bAllowInsert )
        {
            return base_class::update_at( refHead, value_type( key_type( std::forward<K>( key )), mapped_type()), f, bAllowInsert );
        }

        template <typename K, typename Compare>
        bool erase_at( head_type& refHead, K const& key, Compare cmp )
        {
            return base_class::erase_at( refHead, key, cmp );
        }

        template <typename K, typename Compare, typename Func>
        bool erase_at( head_type& refHead, K const& key, Compare cmp, Func f )
        {
            return base_class::erase_at( refHead, key, cmp, f );
        }
        template <typename K, typename Compare>
        guarded_ptr extract_at( head_type& refHead, K const& key, Compare cmp )
        {
            return base_class::extract_at( refHead, key, cmp );
        }

        template <typename K, typename Compare>
        bool find_at( head_type& refHead, K const& key, Compare cmp )
        {
            return base_class::find_at( refHead, key, cmp );
        }

        template <typename K, typename Compare, typename Func>
        bool find_at( head_type& refHead, K& key, Compare cmp, Func f )
        {
            return base_class::find_at( refHead, key, cmp, f );
        }

        template <typename K, typename Compare>
        guarded_ptr get_at( head_type& refHead, K const& key, Compare cmp )
        {
            return base_class::get_at( refHead, key, cmp );
        }

        //@endcond
    };

}}  // namespace cds::container

#endif  // #ifndef CDSLIB_CONTAINER_IMPL_ITERABLE_KVLIST_H
