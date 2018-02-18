// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_SPLIT_LIST_MAP_RCU_H
#define CDSLIB_CONTAINER_SPLIT_LIST_MAP_RCU_H

#include <cds/container/split_list_set_rcu.h>
#include <cds/details/binary_functor_wrapper.h>

namespace cds { namespace container {

    /// Split-ordered list map (template specialization for \ref cds_urcu_desc "RCU")
    /** @ingroup cds_nonintrusive_map
        \anchor cds_nonintrusive_SplitListMap_rcu

        Hash table implementation based on split-ordered list algorithm discovered by Ori Shalev and Nir Shavit, see
        - [2003] Ori Shalev, Nir Shavit "Split-Ordered Lists - Lock-free Resizable Hash Tables"
        - [2008] Nir Shavit "The Art of Multiprocessor Programming"

        See intrusive::SplitListSet for a brief description of the split-list algorithm.

        Template parameters:
        - \p RCU - one of \ref cds_urcu_gc "RCU type"
        - \p Key - key type to be stored in the map
        - \p Value - value type to be stored in the map
        - \p Traits - type traits, default is \p split_list::traits. Instead of declaring \p %split_list::traits -based
            struct you may apply option-based notation with \p split_list::make_traits metafunction.

        <b>Iterators</b>

        The class supports a forward unordered iterator (\ref iterator and \ref const_iterator).
        You may iterate over split-list map items only under RCU lock.
        Only in this case the iterator is thread-safe since
        while RCU is locked any map's item cannot be reclaimed.
        The requirement of RCU lock during iterating means that deletion of the elements
        is not possible.

        @warning The iterator object cannot be passed between threads.
        Due to concurrent nature of split-list map it is not guarantee that you can iterate
        all elements in the map: any concurrent deletion can exclude the element
        pointed by the iterator from the map, and your iteration can be terminated
        before end of the map. Therefore, such iteration is more suitable for debugging purposes

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

        \par Usage

        You should decide what garbage collector you want, and what ordered list you want to use. Split-ordered list
        is original data structure based on an ordered list. Suppose, you want construct split-list map based on \p cds::urcu::general_buffered<> GC
        and \p MichaelList as ordered list implementation. Your map should map \p int key to \p std::string value.
        So, you beginning your program with following include:
        \code
        #include <cds/urcu/general_buffered.h>
        #include <cds/container/michael_list_rcu.h>
        #include <cds/container/split_list_map_rcu.h>

        namespace cc = cds::container;
        \endcode
        The inclusion order is important:
        - first, include one of \ref cds_urcu_gc "RCU implementation" (<tt>cds/urcu/general_buffered.h</tt> in our case)
        - second, include the header of ordered-list implementation (for this example, <tt>cds/container/michael_list_rcu.h</tt>),
        - then, the header for RCU-based split-list map <tt>cds/container/split_list_map_rcu.h</tt>.

        Now, you should declare traits for split-list map. The main parts of traits are a hash functor for the map key and a comparing functor for ordered list.
        We use \p std::hash<int> and \p std::less<int>.

        The second attention: instead of using \p %MichaelList in \p %SplitListMap traits we use a tag \p ds::contaner::michael_list_tag
        for the Michael's list.
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
        typedef cc::SplitListMap< cds::urcu::gc<cds::urcu::general_buffered<> >, int, std::string, foo_set_traits > int_string_map;
        \endcode

        You may use the modern option-based declaration instead of classic traits-based one:
        \code
        typedef cc::SplitListMap<
            cds::urcu::gc<cds::urcu::general_buffered<> >  // RCU type
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
        In case of option-based declaration using \p split_list::make_traits metafunction the struct \p foo_set_traits is not required.

        Now, the map of type \p int_string_map is ready to use in your program.

        Note that in this example we show only mandatory \p traits parts, optional ones is the default and they are inherited
        from cds::container::split_list::traits.
        There are many other useful options for deep tuning the split-list and ordered-list containers.
    */
    template <
        class RCU,
        typename Key,
        typename Value,
#ifdef CDS_DOXYGEN_INVOKED
        class Traits = split_list::traits
#else
        class Traits
#endif
    >
    class SplitListMap< cds::urcu::gc< RCU >, Key, Value, Traits >:
        protected container::SplitListSet<
            cds::urcu::gc< RCU >,
            std::pair<Key const, Value>,
            split_list::details::wrap_map_traits<Key, Value, Traits>
        >
    {
        //@cond
        typedef container::SplitListSet<
            cds::urcu::gc< RCU >,
            std::pair<Key const, Value>,
            split_list::details::wrap_map_traits<Key, Value, Traits>
        >  base_class;
        //@endcond

    public:
        typedef cds::urcu::gc< RCU > gc; ///< Garbage collector
        typedef Key     key_type;    ///< key type
        typedef Value   mapped_type; ///< type of value to be stored in the map
        typedef Traits  traits;     ///< Map traits

        typedef std::pair<key_type const, mapped_type> value_type;     ///< key-value pair type
        typedef typename base_class::ordered_list      ordered_list;   ///< Underlying ordered list class
        typedef typename base_class::key_comparator    key_comparator; ///< key comparison functor

        typedef typename base_class::hash           hash;         ///< Hash functor for \ref key_type
        typedef typename base_class::item_counter   item_counter; ///< Item counter type
        typedef typename base_class::stat           stat;         ///< Internal statistics

        typedef typename base_class::rcu_lock       rcu_lock;   ///< RCU scoped lock
        typedef typename base_class::exempt_ptr     exempt_ptr; ///< pointer to extracted node
        /// Group of \p extract_xxx functions require external locking if underlying ordered list requires that
        static constexpr const bool c_bExtractLockExternal = base_class::c_bExtractLockExternal;
        typedef typename base_class::raw_ptr        raw_ptr;    ///< type of \p get() return value

    protected:
        //@cond
        typedef typename base_class::maker::traits::key_accessor key_accessor;
        //@endcond

    public:
        /// Forward iterator
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
        //@{
        const_iterator begin() const
        {
            return base_class::begin();
        }
        const_iterator cbegin() const
        {
            return base_class::cbegin();
        }
        //@}

        /// Returns an const iterator that addresses the location succeeding the last element in a map
        //@{
        const_iterator end() const
        {
            return base_class::end();
        }
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
            which selects by \p split_list::dynamic_bucket_table option.
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
            The function creates a node with \p key and the default value, and then inserts the node created into the map.

            Preconditions:
            - The \p key_type should be constructible from value of type \p K.
            - The \p mapped_type should be default-constructible.

            The function applies RCU lock internally.

            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename K>
        bool insert( K const& key )
        {
            return base_class::emplace( key_type( key ), mapped_type());
        }

        /// Inserts new node
        /**
            The function creates a node with copy of \p val value
            and then inserts the node into the map.

            Preconditions:
            - The \p key_type should be constructible from \p key of type \p K.
            - The \p mapped_type should be constructible from \p val of type \p V.

            The function applies RCU lock internally.

            Returns \p true if \p val is inserted into the map, \p false otherwise.
        */
        template <typename K, typename V>
        bool insert( K const& key, V const& val )
        {
            //TODO: pass arguments by reference (make_pair makes copy)
            return base_class::emplace( key_type( key ), mapped_type( val ));
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

            It should be keep in mind that concurrent modifications of \p <tt>item.second</tt> in \p func body
            should be careful. You shouldf guarantee that during changing item's value in \p func no any other changes
            could be made on this \p item by concurrent threads.

            \p func is called only if inserting is successful.

            The function allows to split creating of new item into two part:
            - create item from \p key;
            - insert new item into the map;
            - if inserting is successful, initialize the value of item by calling \p func functor

            This can be useful if complete initialization of object of \p mapped_type is heavyweight and
            it is preferable that the initialization should be completed only if inserting is successful.

            The function applies RCU lock internally.
        */
        template <typename K, typename Func>
        bool insert_with( K const& key, Func func )
        {
            //TODO: pass arguments by reference (make_pair makes copy)
            return base_class::insert( std::make_pair( key_type( key ), mapped_type()), func );
        }

        /// For key \p key inserts data of type \p mapped_type created in-place from \p args
        /**
            \p key_type should be constructible from type \p K

            The function applies RCU lock internally.

            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename K, typename... Args>
        bool emplace( K&& key, Args&&... args )
        {
            return base_class::emplace( key_type( std::forward<K>( key )), mapped_type( std::forward<Args>(args)... ));
        }

        /// Updates data by \p key
        /**
            The operation performs inserting or replacing the element with lock-free manner.

            If the \p key not found in the map, then the new item created from \p key
            will be inserted into the map iff \p bAllowInsert is \p true.
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

            The functor may change any fields of the \p item.second that is \p mapped_type.

            The function applies RCU lock internally.

            Returns <tt> std::pair<bool, bool> </tt> where \p first is true if operation is successful,
            \p second is true if new item has been added or \p false if the item with \p key
            already exists.

            @warning For \ref cds_nonintrusive_MichaelKVList_gc "MichaelKVList" as the ordered list see \ref cds_intrusive_item_creating "insert item troubleshooting".
            \ref cds_nonintrusive_LazyKVList_gc "LazyKVList" provides exclusive access to inserted item and does not require any node-level
            synchronization.
        */
        template <typename K, typename Func>
        std::pair<bool, bool> update( K const& key, Func func, bool bAllowInsert = true )
        {
            //TODO: pass arguments by reference (make_pair makes copy)
            typedef decltype( std::make_pair( key_type( key ), mapped_type())) arg_pair_type;

            return base_class::update( std::make_pair( key_type( key ), mapped_type()),
                [&func]( bool bNew, value_type& item, arg_pair_type const& /*val*/ ) {
                    func( bNew, item );
                },
                bAllowInsert );
        }
        //@cond
        template <typename K, typename Func>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<bool, bool> ensure( K const& key, Func func )
        {
            return update( key, func, true );
        }
        //@endcond

        /// Deletes \p key from the map
        /** \anchor cds_nonintrusive_SplitListMap_rcu_erase_val

            RCU \p synchronize method can be called. RCU should not be locked.

            Return \p true if \p key is found and deleted, \p false otherwise
        */
        template <typename K>
        bool erase( K const& key )
        {
            return base_class::erase( key );
        }

        /// Deletes the item from the map using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_SplitListMap_rcu_erase_val "erase(K const&)"
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
        /** \anchor cds_nonintrusive_SplitListMap_rcu_erase_func

            The function searches an item with key \p key, calls \p f functor
            and deletes the item. If \p key is not found, the functor is not called.

            The functor \p Func interface is:
            \code
            struct extractor {
                void operator()(value_type& item) { ... }
            };
            \endcode

            RCU \p synchronize method can be called. RCU should not be locked.

            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename K, typename Func>
        bool erase( K const& key, Func f )
        {
            return base_class::erase( key, f );
        }

        /// Deletes the item from the map using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_SplitListMap_rcu_erase_func "erase(K const&, Func)"
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

        /// Extracts an item from the map
        /** \anchor cds_nonintrusive_SplitListMap_rcu_extract
            The function searches an item with key equal to \p key in the map,
            unlinks it from the map, and returns \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to the item found.
            If the item with the key equal to \p key is not found the function returns an empty \p exempt_ptr.

            Depends on ordered list you should or should not lock RCU before calling of this function:
            - for the set based on \ref cds_intrusive_MichaelList_rcu "MichaelList" RCU should not be locked
            - for the set based on \ref cds_intrusive_LazyList_rcu "LazyList" RCU should be locked
            See ordered list implementation for details.

            \code
            typedef cds::urcu::gc< general_buffered<> > rcu;

            // Split-list set based on MichaelList by default
            typedef cds::container::SplitListMap< rcu, int, Foo > splitlist_map;

            splitlist_map theMap;
            // ...

            typename splitlist_map::exempt_ptr p;

            // For MichaelList we should not lock RCU

            // Now, you can apply extract function
            p = theMap.extract( 10 )
            if ( p ) {
                // do something with p
                ...
            }

            // We may safely release p here
            // release() passes the pointer to RCU reclamation cycle
            p.release();
            \endcode
        */
        template <typename K>
        exempt_ptr extract( K const& key )
        {
            return base_class::extract( key );
        }

        /// Extracts an item from the map using \p pred predicate for searching
        /**
            The function is an analog of \p extract(K const&) but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less>
        exempt_ptr extract_with( K const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return base_class::extract_with( key, cds::details::predicate_wrapper<value_type, Less, key_accessor>());
        }

        /// Finds the key \p key
        /** \anchor cds_nonintrusive_SplitListMap_rcu_find_cfunc

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

            The function applies RCU lock internally.

            The function returns \p true if \p key is found, \p false otherwise.
        */
        template <typename K, typename Func>
        bool find( K const& key, Func f )
        {
            return base_class::find( key, [&f](value_type& pair, K const&){ f( pair ); } );
        }

        /// Finds the key \p key using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_SplitListMap_rcu_find_cfunc "find(K const&, Func)"
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

        /// Checks whether the map contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.

            The function applies RCU lock internally.
        */
        template <typename K>
        bool contains( K const& key )
        {
            return base_class::contains( key );
        }
        //@cond
        template <typename K>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find( K const& key )
        {
            return base_class::find( key );
        }
        //@endcond

        /// Checks whether the map contains \p key using \p pred predicate for searching
        /**
            The function is an analog of <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less>
        bool contains( K const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return base_class::contains( key, cds::details::predicate_wrapper<value_type, Less, key_accessor>());
        }
        //@cond
        template <typename K, typename Less>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find_with( K const& key, Less pred )
        {
            return contains( key, pred );
        }
        //@endcond

        /// Finds \p key and return the item found
        /** \anchor cds_intrusive_SplitListMap_rcu_get
            The function searches the item with key equal to \p key and returns the pointer to item found.
            If \p key is not found it returns empty \p raw_ptr.

            Note the compare functor should accept a parameter of type \p K that can be not the same as \p value_type.

            RCU should be locked before call of this function.
            Returned item is valid only while RCU is locked:
            \code
            typedef cds::urcu::gc< general_buffered<> > rcu;
            typedef cds::container::SplitListMap< rcu, int, Foo > splitlist_map;
            splitlist_map theMap;
            // ...
            {
                // Lock RCU
                typename splitlist_map::rcu_lock lock;

                typename splitlist_map::raw_ptr pVal = theMap.get( 5 );
                if ( pVal ) {
                    // Deal with pVal
                    //...
                }
                // Unlock RCU by rcu_lock destructor
                // pVal can be retired by disposer at any time after RCU has been unlocked
            }
            \endcode
        */
        template <typename K>
        raw_ptr get( K const& key )
        {
            return base_class::get( key );
        }

        /// Finds \p key with predicate specified and return the item found
        /**
            The function is an analog of \ref cds_intrusive_SplitListMap_rcu_get "get(K const&)"
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p K
            in any order.
            \p pred must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less>
        raw_ptr get_with( K const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return base_class::get_with( key, cds::details::predicate_wrapper<value_type, Less, key_accessor>());
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

#endif // #ifndef CDSLIB_CONTAINER_SPLIT_LIST_MAP_RCU_H
