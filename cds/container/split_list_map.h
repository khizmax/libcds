// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_SPLIT_LIST_MAP_H
#define CDSLIB_CONTAINER_SPLIT_LIST_MAP_H

#include <cds/container/split_list_set.h>
#include <cds/details/binary_functor_wrapper.h>

namespace cds { namespace container {

    /// Split-ordered list map
    /** @ingroup cds_nonintrusive_map
        \anchor cds_nonintrusive_SplitListMap_hp

        Hash table implementation based on split-ordered list algorithm discovered by Ori Shalev and Nir Shavit, see
        - [2003] Ori Shalev, Nir Shavit "Split-Ordered Lists - Lock-free Resizable Hash Tables"
        - [2008] Nir Shavit "The Art of Multiprocessor Programming"

        See intrusive::SplitListSet for a brief description of the split-list algorithm.

        Template parameters:
        - \p GC - Garbage collector used like \p cds::gc::HP or \p cds::gc::DHP
        - \p Key - key type of an item stored in the map. It should be copy-constructible
        - \p Value - value type stored in the map
        - \p Traits - map traits, default is \p split_list::traits. Instead of declaring \p %split_list::traits -based
            struct you may apply option-based notation with \p split_list::make_traits metafunction.

        There are the specializations:
        - for \ref cds_urcu_desc "RCU" - declared in <tt>cd/container/split_list_map_rcu.h</tt>,
            see \ref cds_nonintrusive_SplitListMap_rcu "SplitListMap<RCU>".
        - for \ref cds::gc::nogc declared in <tt>cds/container/split_list_map_nogc.h</tt>,
            see \ref cds_nonintrusive_SplitListMap_nogc "SplitListMap<gc::nogc>".

        \par Usage

        You should decide what garbage collector you want, and what ordered list you want to use. Split-ordered list
        is original data structure based on an ordered list. Suppose, you want construct split-list map based on \p gc::HP GC
        and \p MichaelList as ordered list implementation. Your map should map \p int key to \p std::string value.
        So, you beginning your code with the following:
        \code
        #include <cds/container/michael_list_hp.h>
        #include <cds/container/split_list_map.h>

        namespace cc = cds::container;
        \endcode
        The inclusion order is important: first, include file for ordered-list implementation (for this example, <tt>cds/container/michael_list_hp.h</tt>),
        then the header for split-list map <tt>cds/container/split_list_map.h</tt>.

        Now, you should declare traits for split-list map. The main parts of traits are a hash functor and a comparing functor for the ordered list.
        We use <tt>std::hash<int></tt> as hash functor and <tt>std::less<int></tt> predicate as comparing functor.

        The second attention: instead of using \p %MichaelList in \p %SplitListMap traits we use a tag \p cds::contaner::michael_list_tag for the Michael's list.
        The split-list requires significant support from underlying ordered list class and it is not good idea to dive you
        into deep implementation details of split-list and ordered list interrelations. The tag paradigm simplifies split-list interface.

        \code
        // SplitListMap traits
        struct foo_set_traits: public cc::split_list::traits
        {
            typedef cc::michael_list_tag   ordered_list    ;   // what type of ordered list we want to use
            typedef std::hash<int>         hash            ;   // hash functor for the key stored in split-list map

            // Type traits for our MichaelList class
            struct ordered_list_traits: public cc::michael_list::traits
            {
            typedef std::less<int> less   ;   // use our std::less predicate as comparator to order list nodes
            };
        };
        \endcode

        Now you are ready to declare our map class based on \p %SplitListMap:
        \code
        typedef cc::SplitListMap< cds::gc::DHP, int, std::string, foo_set_traits > int_string_map;
        \endcode

        You may use the modern option-based declaration instead of classic type-traits-based one:
        \code
        typedef cc::SplitListMap<
            cs::gc::DHP             // GC used
            ,int                    // key type
            ,std::string            // value type
            ,cc::split_list::make_traits<      // metafunction to build split-list traits
                cc::split_list::ordered_list<cc::michael_list_tag>     // tag for underlying ordered list implementation
                ,cc::opt::hash< std::hash<int> >        // hash functor
                ,cc::split_list::ordered_list_traits<    // ordered list traits desired
                    cc::michael_list::make_traits<    // metafunction to build lazy list traits
                        cc::opt::less< std::less<int> >         // less-based compare functor
                    >::type
                >
            >::type
        >  int_string_map;
        \endcode
        In case of option-based declaration with \p split_list::make_traits metafunction the struct \p foo_set_traits is not required.

        Now, the map of type \p int_string_map is ready to use in your program.

        Note that in this example we show only mandatory \p traits parts, optional ones is the default and they are inherited
        from \p container::split_list::traits. There are many other options for deep tuning of the split-list and
        ordered-list containers.
    */
    template <
        class GC,
        typename Key,
        typename Value,
#ifdef CDS_DOXYGEN_INVOKED
        class Traits = split_list::traits
#else
        class Traits
#endif
    >
    class SplitListMap:
        protected container::SplitListSet<
            GC,
            std::pair<Key const, Value>,
            split_list::details::wrap_map_traits<Key, Value, Traits>
        >
    {
        //@cond
        typedef container::SplitListSet<
            GC,
            std::pair<Key const, Value>,
            split_list::details::wrap_map_traits<Key, Value, Traits>
        >  base_class;
        //@endcond

    public:
        typedef GC     gc;          ///< Garbage collector
        typedef Key    key_type;    ///< key type
        typedef Value  mapped_type; ///< type of value to be stored in the map
        typedef Traits traits;      ///< Map traits

        typedef std::pair<key_type const, mapped_type>  value_type  ;   ///< key-value pair type
        typedef typename base_class::ordered_list       ordered_list;   ///< Underlying ordered list class
        typedef typename base_class::key_comparator     key_comparator; ///< key compare functor

        typedef typename base_class::hash           hash;         ///< Hash functor for \ref key_type
        typedef typename base_class::item_counter   item_counter; ///< Item counter type
        typedef typename base_class::stat           stat;         ///< Internal statistics

        /// Count of hazard pointer required
        static constexpr const size_t c_nHazardPtrCount = base_class::c_nHazardPtrCount;

    protected:
        //@cond
        typedef typename base_class::maker::traits::key_accessor key_accessor;
        typedef typename base_class::node_type node_type;
        //@endcond

    public:
        /// Guarded pointer
        typedef typename gc::template guarded_ptr< node_type, value_type, details::guarded_ptr_cast_set<node_type, value_type> > guarded_ptr;

    public:
    ///@name Forward iterators (only for debugging purpose)
    //@{
        /// Forward iterator
        /**
            The forward iterator for a split-list has the following features:
            - it has no post-increment operator
            - it depends on underlying ordered list iterator
            - The iterator object cannot be moved across thread boundary because it contains GC's guard that is thread-private GC data.
            - Iterator ensures thread-safety even if you delete the item that iterator points to. However, in case of concurrent
              deleting operations it is no guarantee that you iterate all item in the split-list.
              Moreover, a crash is possible when you try to iterate the next element that has been deleted by concurrent thread.

              @warning Use this iterator on the concurrent container for debugging purpose only.

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
        */
        typedef typename base_class::iterator iterator;

        /// Const forward iterator
        typedef typename base_class::const_iterator const_iterator;

        /// Returns a forward iterator addressing the first element in a map
        /**
            For empty map \code begin() == end() \endcode
        */
        iterator begin()
        {
            return base_class::begin();
        }

        /// Returns an iterator that addresses the location succeeding the last element in a map
        /**
            Do not use the value returned by <tt>end</tt> function to access any item.
            The returned value can be used only to control reaching the end of the map.
            For empty map \code begin() == end() \endcode
        */
        iterator end()
        {
            return base_class::end();
        }

        /// Returns a forward const iterator addressing the first element in a map
        const_iterator begin() const
        {
            return base_class::begin();
        }

        /// Returns a forward const iterator addressing the first element in a map
        const_iterator cbegin() const
        {
            return base_class::cbegin();
        }

        /// Returns an const iterator that addresses the location succeeding the last element in a map
        const_iterator end() const
        {
            return base_class::end();
        }

        /// Returns an const iterator that addresses the location succeeding the last element in a map
        const_iterator cend() const
        {
            return base_class::cend();
        }
    //@}

    public:
        /// Initializes split-ordered map of default capacity
        /**
            The default capacity is defined in bucket table constructor.
            See \p intrusive::split_list::expandable_bucket_table, \p intrusive::split_list::static_bucket_table
            which selects by \p intrusive::split_list::traits::dynamic_bucket_table.
        */
        SplitListMap()
            : base_class()
        {}

        /// Initializes split-ordered map
        SplitListMap(
            size_t nItemCount           ///< estimated average item count
            , size_t nLoadFactor = 1    ///< load factor - average item count per bucket. Small integer up to 10, default is 1.
            )
            : base_class( nItemCount, nLoadFactor )
        {}

    public:
        /// Inserts new node with key and default value
        /**
            The function creates a node with \p key and default value, and then inserts the node created into the map.

            Preconditions:
            - The \ref key_type should be constructible from value of type \p K.
                In trivial case, \p K is equal to \ref key_type.
            - The \ref mapped_type should be default-constructible.

            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename K>
        bool insert( K&& key )
        {
            return base_class::emplace( key_type( std::forward<K>( key )), mapped_type());
        }

        /// Inserts new node
        /**
            The function creates a node with copy of \p val value
            and then inserts the node created into the map.

            Preconditions:
            - The \ref key_type should be constructible from \p key of type \p K.
            - The \ref mapped_type should be constructible from \p val of type \p V.

            Returns \p true if \p val is inserted into the map, \p false otherwise.
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
            to the map's item inserted:
                - <tt>item.first</tt> is a const reference to item's key that cannot be changed.
                - <tt>item.second</tt> is a reference to item's value that may be changed.

            It should be keep in mind that concurrent modifications of \p <tt>item.second</tt> may be possible.

            The key_type should be constructible from value of type \p K.

            The function allows to split creating of new item into two part:
            - create item from \p key;
            - insert new item into the map;
            - if inserting is successful, initialize the value of item by calling \p func functor

            This can be useful if complete initialization of object of \p mapped_type is heavyweight and
            it is preferable that the initialization should be completed only if inserting is successful.

            @warning For \ref cds_nonintrusive_MichaelKVList_gc "MichaelKVList" as the bucket see \ref cds_intrusive_item_creating "insert item troubleshooting".
            \ref cds_nonintrusive_LazyKVList_gc "LazyKVList" provides exclusive access to inserted item and does not require any node-level
            synchronization.
        */
        template <typename K, typename Func>
        bool insert_with( K&& key, Func func )
        {
            //TODO: pass arguments by reference (make_pair makes copy)
            return base_class::insert( std::make_pair( key_type( std::forward<K>( key )), mapped_type()), func );
        }

        /// For key \p key inserts data of type \p mapped_type created from \p args
        /**
            \p key_type should be constructible from type \p K

            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename K, typename... Args>
        bool emplace( K&& key, Args&&... args )
        {
            return base_class::emplace( key_type( std::forward<K>(key)), mapped_type( std::forward<Args>(args)...));
        }

        /// Updates the node
        /**
            The operation performs inserting or changing data with lock-free manner.

            If \p key is not found in the map, then \p key is inserted iff \p bAllowInsert is \p true.
            Otherwise, the functor \p func is called with item found.

            The functor \p func signature depends on ordered list:

            <b>for \p MichaelKVList, \p LazyKVList</b>
            \code
                struct my_functor {
                    void operator()( bool bNew, value_type& item );
                };
            \endcode
            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - the item found or inserted

            The functor may change any fields of the \p item.second that is \p mapped_type.

            <b>for \p IterableKVList</b>
            \code
                void func( value_type& val, value_type * old );
            \endcode
            where
            - \p val - a new data constructed from \p key
            - \p old - old value that will be retired. If new item has been inserted then \p old is \p nullptr.

            Returns <tt> std::pair<bool, bool> </tt> where \p first is true if operation is successful,
            \p second is true if new item has been added or \p false if the item with \p key
            already is in the map.

            @warning For \ref cds_nonintrusive_MichaelKVList_gc "MichaelKVList" and \ref cds_nonintrusive_IterableKVList_gc "IterableKVList"
            as the ordered list see \ref cds_intrusive_item_creating "insert item troubleshooting".
            \ref cds_nonintrusive_LazyKVList_gc "LazyKVList" provides exclusive access to inserted item and does not require any node-level
            synchronization.
        */
        template <typename K, typename Func>
#ifdef CDS_DOXYGE_INVOKED
        std::pair<bool, bool>
#else
        typename std::enable_if<
            std::is_same<K,K>::value && !is_iterable_list< ordered_list >::value,
            std::pair<bool, bool>
        >::type
#endif
        update( K&& key, Func func, bool bAllowInsert = true )
        {
            typedef decltype( std::make_pair( key_type( std::forward<K>( key )), mapped_type())) arg_pair_type;

            return base_class::update( std::make_pair( key_type( key ), mapped_type()),
                [&func]( bool bNew, value_type& item, arg_pair_type const& /*val*/ ) {
                    func( bNew, item );
                },
                bAllowInsert );
        }
        //@cond
        template <typename K, typename Func>
#ifdef CDS_DOXYGE_INVOKED
        std::pair<bool, bool>
#else
        typename std::enable_if<
            std::is_same<K, K>::value && is_iterable_list< ordered_list >::value,
            std::pair<bool, bool>
        >::type
#endif
        update( K&& key, Func func, bool bAllowInsert = true )
        {
            return base_class::update( std::make_pair( key_type( std::forward<K>( key )), mapped_type()), func, bAllowInsert );
        }
        //@endcond
        //@cond
        template <typename K, typename Func>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<bool, bool> ensure( K const& key, Func func )
        {
            return update( key, func, true );
        }
        //@endcond

        /// Inserts or updates the node (only for \p IterableKVList)
        /**
            The operation performs inserting or changing data with lock-free manner.

            If \p key is not found in the map, then \p key is inserted iff \p bAllowInsert is \p true.
            Otherwise, the current element is changed to \p val, the old element will be retired later.

            Returns std::pair<bool, bool> where \p first is \p true if operation is successful,
            \p second is \p true if \p val has been added or \p false if the item with that key
            already in the map.
            */
        template <typename Q, typename V>
#ifdef CDS_DOXYGEN_INVOKED
        std::pair<bool, bool>
#else
        typename std::enable_if<
            std::is_same< Q, Q>::value && is_iterable_list< ordered_list >::value,
            std::pair<bool, bool>
        >::type
#endif
        upsert( Q&& key, V&& val, bool bAllowInsert = true )
        {
            return base_class::upsert( std::make_pair( key_type( std::forward<Q>( key )), mapped_type( std::forward<V>( val ))), bAllowInsert );
        }


        /// Deletes \p key from the map
        /** \anchor cds_nonintrusive_SplitListMap_erase_val

            Return \p true if \p key is found and deleted, \p false otherwise
        */
        template <typename K>
        bool erase( K const& key )
        {
            return base_class::erase( key );
        }

        /// Deletes the item from the map using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_SplitListMap_erase_val "erase(K const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less>
        bool erase_with( K const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return base_class::erase_with( key, cds::details::predicate_wrapper<value_type, Less, key_accessor>());
        }

        /// Deletes \p key from the map
        /** \anchor cds_nonintrusive_SplitListMap_erase_func

            The function searches an item with key \p key, calls \p f functor
            and deletes the item. If \p key is not found, the functor is not called.

            The functor \p Func interface is:
            \code
            struct extractor {
                void operator()(value_type& item) { ... }
            };
            \endcode

            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename K, typename Func>
        bool erase( K const& key, Func f )
        {
            return base_class::erase( key, f );
        }

        /// Deletes the item from the map using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_SplitListMap_erase_func "erase(K const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less, typename Func>
        bool erase_with( K const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return base_class::erase_with( key, cds::details::predicate_wrapper<value_type, Less, key_accessor>(), f );
        }

        /// Deletes the item pointed by iterator \p iter (only for \p IterableList based map)
        /**
            Returns \p true if the operation is successful, \p false otherwise.
            The function can return \p false if the node the iterator points to has already been deleted
            by other thread.

            The function does not invalidate the iterator, it remains valid and can be used for further traversing.

            @note \p %erase_at() is supported only for \p %SplitListMap based on \p IterableList.
        */
#ifdef CDS_DOXYGEN_INVOKED
        bool erase_at( iterator const& iter )
#else
        template <typename Iterator>
        typename std::enable_if< std::is_same<Iterator, iterator>::value && is_iterable_list< ordered_list >::value, bool >::type
        erase_at( Iterator const& iter )
#endif
        {
            return base_class::erase_at( iter );
        }

        /// Extracts the item with specified \p key
        /** \anchor cds_nonintrusive_SplitListMap_hp_extract
            The function searches an item with key equal to \p key,
            unlinks it from the map, and returns it as \p guarded_ptr.
            If \p key is not found the function returns an empty guarded pointer.

            Note the compare functor should accept a parameter of type \p K that may be not the same as \p value_type.

            The extracted item is freed automatically when returned \p guarded_ptr object will be destroyed or released.
            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.

            Usage:
            \code
            typedef cds::container::SplitListMap< your_template_args > splitlist_map;
            splitlist_map theMap;
            // ...
            {
                splitlist_map::guarded_ptr gp(theMap.extract( 5 ));
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
            return base_class::extract_( key );
        }

        /// Extracts the item using compare functor \p pred
        /**
            The function is an analog of \ref cds_nonintrusive_SplitListMap_hp_extract "extract(K const&)"
            but \p pred predicate is used for key comparing.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p K
            in any order.
            \p pred must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less>
        guarded_ptr extract_with( K const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return base_class::extract_with_( key, cds::details::predicate_wrapper<value_type, Less, key_accessor>());
        }

        /// Finds the key \p key
        /** \anchor cds_nonintrusive_SplitListMap_find_cfunc

            The function searches the item with key equal to \p key and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item );
            };
            \endcode
            where \p item is the item found.

            The functor may change \p item.second. Note that the functor is only guarantee
            that \p item cannot be disposed during functor is executing.
            The functor does not serialize simultaneous access to the map's \p item. If such access is
            possible you must provide your own synchronization schema on item level to exclude unsafe item modifications.

            The function returns \p true if \p key is found, \p false otherwise.
        */
        template <typename K, typename Func>
        bool find( K const& key, Func f )
        {
            return base_class::find( key, [&f](value_type& pair, K const&){ f( pair ); } );
        }

        /// Finds \p key and returns iterator pointed to the item found (only for \p IterableList)
        /**
            If \p key is not found the function returns \p end().

            @note This function is supported only for map based on \p IterableList
        */
        template <typename K>
#ifdef CDS_DOXYGEN_INVOKED
        iterator
#else
        typename std::enable_if< std::is_same<K,K>::value && is_iterable_list<ordered_list>::value, iterator >::type
#endif
        find( K const& key )
        {
            return base_class::find( key );
        }

        /// Finds the key \p val using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_SplitListMap_find_cfunc "find(K const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less, typename Func>
        bool find_with( K const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return base_class::find_with( key,
                cds::details::predicate_wrapper<value_type, Less, key_accessor>(),
                [&f](value_type& pair, K const&){ f( pair ); } );
        }

        /// Finds \p key using \p pred predicate and returns iterator pointed to the item found (only for \p IterableList)
        /**
            The function is an analog of \p find(K&) but \p pred is used for key comparing.
            \p Less functor has interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the map.

            If \p key is not found the function returns \p end().

            @note This function is supported only for map based on \p IterableList
        */
        template <typename K, typename Less>
#ifdef CDS_DOXYGEN_INVOKED
        iterator
#else
        typename std::enable_if< std::is_same<K, K>::value && is_iterable_list< ordered_list >::value, iterator >::type
#endif
        find_with( K const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return base_class::find_with( key, cds::details::predicate_wrapper<value_type, Less, key_accessor>());
        }

        /// Checks whether the map contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.

            Note the hash functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that can be not the same as \p value_type.
            Otherwise, you may use \p contains( Q const&, Less pred ) functions with explicit predicate for key comparing.
        */
        template <typename K>
        bool contains( K const& key )
        {
            return base_class::contains( key );
        }

        /// Checks whether the map contains \p key using \p pred predicate for searching
        /**
            The function is similar to <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less>
        bool contains( K const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return base_class::contains( key, cds::details::predicate_wrapper<value_type, Less, key_accessor>());
        }

        /// Finds \p key and return the item found
        /** \anchor cds_nonintrusive_SplitListMap_hp_get
            The function searches the item with key equal to \p key
            and returns the item found as a guarded pointer.
            If \p key is not found the function returns an empty guarded pointer.

            @note Each \p guarded_ptr object uses one GC's guard which can be limited resource.

            Usage:
            \code
            typedef cds::container::SplitListMap< your_template_params >  splitlist_map;
            splitlist_map theMap;
            // ...
            {
                splitlist_map::guarded_ptr gp(theMap.get( 5 ));
                if ( gp ) {
                    // Deal with gp
                    //...
                }
                // Destructor of guarded_ptr releases internal HP guard
            }
            \endcode

            Note the compare functor specified for split-list map
            should accept a parameter of type \p K that can be not the same as \p value_type.
        */
        template <typename K>
        guarded_ptr get( K const& key )
        {
            return base_class::get_( key );
        }

        /// Finds \p key and return the item found
        /**
            The function is an analog of \ref cds_nonintrusive_SplitListMap_hp_get "get( K const&)"
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p K
            in any order.
            \p pred must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less>
        guarded_ptr get_with( K const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return base_class::get_with_( key, cds::details::predicate_wrapper<value_type, Less, key_accessor>());
        }

        /// Clears the map (not atomic)
        void clear()
        {
            base_class::clear();
        }

        /// Checks if the map is empty
        /**
            Emptiness is checked by item counting: if item count is zero then the map is empty.
            Thus, the correct item counting is an important part of the map implementation.
        */
        bool empty() const
        {
            return base_class::empty();
        }

        /// Returns item count in the map
        size_t size() const
        {
            return base_class::size();
        }

        /// Returns internal statistics
        stat const& statistics() const
        {
            return base_class::statistics();
        }

        /// Returns internal statistics for \p ordered_list
        typename ordered_list::stat const& list_statistics() const
        {
            return base_class::list_statistics();
        }
    };

}} // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_SPLIT_LIST_MAP_H
