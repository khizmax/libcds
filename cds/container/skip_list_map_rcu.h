// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_SKIP_LIST_MAP_RCU_H
#define CDSLIB_CONTAINER_SKIP_LIST_MAP_RCU_H

#include <cds/container/details/skip_list_base.h>
#include <cds/intrusive/skip_list_rcu.h>
#include <cds/container/details/make_skip_list_map.h>

namespace cds { namespace container {

    /// Lock-free skip-list map (template specialization for \ref cds_urcu_desc "RCU")
    /** @ingroup cds_nonintrusive_map
        \anchor cds_nonintrusive_SkipListMap_rcu

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
                chapter 14.4 "A Lock-Free Concurrent Skiplist"

        Template arguments:
        - \p RCU - one of \ref cds_urcu_gc "RCU type".
        - \p K - type of a key to be stored in the list.
        - \p T - type of a value to be stored in the list.
        - \p Traits - map traits, default is \p skip_list::traits.
            It is possible to declare option-based list with \p cds::container::skip_list::make_traits metafunction
            instead of \p Traits template argument.

        Like STL map class, \p %SkipListMap stores its key-value pair as <tt>std:pair< K const, T></tt>.

        @note Before including <tt><cds/container/skip_list_map_rcu.h></tt> you should include appropriate RCU header file,
        see \ref cds_urcu_gc "RCU type" for list of existing RCU class and corresponding header files.

        <b>Iterators</b>

        The class supports a forward iterator (\ref iterator and \ref const_iterator).
        The iteration is ordered.
        You may iterate over skip-list set items only under RCU lock.
        Only in this case the iterator is thread-safe since
        while RCU is locked any set's item cannot be reclaimed.

        The requirement of RCU lock during iterating means that deletion of the elements (i.e. \ref erase)
        is not possible.

        @warning The iterator object cannot be passed between threads

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

    */
    template <
        typename RCU,
        typename Key,
        typename T,
#ifdef CDS_DOXYGEN_INVOKED
        typename Traits = skip_list::traits
#else
        typename Traits
#endif
    >
    class SkipListMap< cds::urcu::gc< RCU >, Key, T, Traits >:
#ifdef CDS_DOXYGEN_INVOKED
        protected intrusive::SkipListSet< cds::urcu::gc< RCU >, std::pair<Key const, T>, Traits >
#else
        protected details::make_skip_list_map< cds::urcu::gc< RCU >, Key, T, Traits >::type
#endif
    {
        //@cond
        typedef details::make_skip_list_map< cds::urcu::gc< RCU >, Key, T, Traits >    maker;
        typedef typename maker::type base_class;
        //@endcond
    public:
        typedef cds::urcu::gc< RCU > gc; ///< Garbage collector used
        typedef Key     key_type;       ///< Key type
        typedef T       mapped_type;    ///< Mapped type
#   ifdef CDS_DOXYGEN_INVOKED
        typedef std::pair< K const, T> value_type;   ///< Value type stored in the map
#   else
        typedef typename maker::value_type  value_type;
#   endif
        typedef Traits  traits;   ///< Map traits

        typedef typename base_class::back_off       back_off;       ///< Back-off strategy used
        typedef typename traits::allocator          allocator_type; ///< Allocator type used for allocate/deallocate the skip-list nodes
        typedef typename base_class::item_counter   item_counter;   ///< Item counting policy used
        typedef typename maker::key_comparator      key_comparator; ///< key comparison functor
        typedef typename base_class::memory_model   memory_model;   ///< Memory ordering. See cds::opt::memory_model option
        typedef typename traits::random_level_generator random_level_generator; ///< random level generator
        typedef typename traits::stat               stat;   ///< internal statistics type

    protected:
        //@cond
        typedef typename maker::node_type           node_type;
        typedef typename maker::node_allocator      node_allocator;

        typedef std::unique_ptr< node_type, typename maker::node_deallocator >    scoped_node_ptr;
        //@endcond

    public:
        typedef typename base_class::rcu_lock  rcu_lock;   ///< RCU scoped lock
        /// Group of \p extract_xxx functions do not require external locking
        static constexpr const bool c_bExtractLockExternal = base_class::c_bExtractLockExternal;

        /// pointer to extracted node
        using exempt_ptr = cds::urcu::exempt_ptr< gc, node_type, value_type, typename maker::intrusive_type_traits::disposer >;

    private:
        //@cond
        struct raw_ptr_converter
        {
            value_type * operator()( node_type * p ) const
            {
               return p ? &p->m_Value : nullptr;
            }

            value_type& operator()( node_type& n ) const
            {
                return n.m_Value;
            }

            value_type const& operator()( node_type const& n ) const
            {
                return n.m_Value;
            }
        };
        //@endcond

    public:
        /// Result of \p get(), \p get_with() functions - pointer to the node found
        typedef cds::urcu::raw_ptr_adaptor< value_type, typename base_class::raw_ptr, raw_ptr_converter > raw_ptr;

    protected:
        //@cond
        unsigned int random_level()
        {
            return base_class::random_level();
        }
        //@endcond

    public:
        /// Default ctor
        SkipListMap()
            : base_class()
        {}

        /// Destructor destroys the set object
        ~SkipListMap()
        {}

    public:
    ///@name Forward ordered iterators (thread-safe under RCU lock)
    //@{
        /// Forward iterator
        /**
            The forward iterator has some features:
            - it has no post-increment operator
            - it depends on iterator of underlying \p OrderedList

            You may safely use iterators in multi-threaded environment only under RCU lock.
            Otherwise, a crash is possible if another thread deletes the element the iterator points to.
        */
        typedef skip_list::details::iterator< typename base_class::iterator >  iterator;

        /// Const iterator type
        typedef skip_list::details::iterator< typename base_class::const_iterator >   const_iterator;

        /// Returns a forward iterator addressing the first element in a map
        iterator begin()
        {
            return iterator( base_class::begin());
        }

        /// Returns a forward const iterator addressing the first element in a map
        const_iterator begin() const
        {
            return cbegin();
        }
        /// Returns a forward const iterator addressing the first element in a map
        const_iterator cbegin() const
        {
            return const_iterator( base_class::cbegin());
        }

        /// Returns a forward iterator that addresses the location succeeding the last element in a map.
        iterator end()
        {
            return iterator( base_class::end());
        }

        /// Returns a forward const iterator that addresses the location succeeding the last element in a map.
        const_iterator end() const
        {
            return cend();
        }
        /// Returns a forward const iterator that addresses the location succeeding the last element in a map.
        const_iterator cend() const
        {
            return const_iterator( base_class::cend());
        }
    //@}

    public:
        /// Inserts new node with key and default value
        /**
            The function creates a node with \p key and default value, and then inserts the node created into the map.

            Preconditions:
            - The \p key_type should be constructible from a value of type \p K.
                In trivial case, \p K is equal to \p key_type.
            - The \p mapped_type should be default-constructible.

            RCU \p synchronize method can be called. RCU should not be locked.

            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename K>
        bool insert( K const& key )
        {
            return insert_with( key, [](value_type&){} );
        }

        /// Inserts new node
        /**
            The function creates a node with copy of \p val value
            and then inserts the node created into the map.

            Preconditions:
            - The \p key_type should be constructible from \p key of type \p K.
            - The \p value_type should be constructible from \p val of type \p V.

            RCU \p synchronize method can be called. RCU should not be locked.

            Returns \p true if \p val is inserted into the set, \p false otherwise.
        */
        template <typename K, typename V>
        bool insert( K const& key, V const& val )
        {
            scoped_node_ptr pNode( node_allocator().New( random_level(), key, val ));
            if ( base_class::insert( *pNode ))
            {
                pNode.release();
                return true;
            }
            return false;
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

            The function allows to split creating of new item into three part:
            - create item from \p key;
            - insert new item into the map;
            - if inserting is successful, initialize the value of item by calling \p func functor

            This can be useful if complete initialization of object of \p value_type is heavyweight and
            it is preferable that the initialization should be completed only if inserting is successful.

            RCU \p synchronize method can be called. RCU should not be locked.
        */
        template <typename K, typename Func>
        bool insert_with( const K& key, Func func )
        {
            scoped_node_ptr pNode( node_allocator().New( random_level(), key ));
            if ( base_class::insert( *pNode, [&func]( node_type& item ) { func( item.m_Value ); } )) {
                pNode.release();
                return true;
            }
            return false;
        }

        /// For key \p key inserts data of type \p value_type created in-place from \p args
        /**
            Returns \p true if inserting successful, \p false otherwise.

            RCU \p synchronize() method can be called. RCU should not be locked.
        */
        template <typename K, typename... Args>
        bool emplace( K&& key, Args&&... args )
        {
            scoped_node_ptr pNode( node_allocator().New( random_level(), std::forward<K>(key), std::forward<Args>(args)... ));
            if ( base_class::insert( *pNode )) {
                pNode.release();
                return true;
            }
            return false;
        }

        /// Updates data by \p key
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the \p key not found in the map, then the new item created from \p key
            is inserted into the map iff \p bInsert is \p true.
            Otherwise, if \p key found, the functor \p func is called with item found.
            The functor \p Func interface is:
            \code
                struct my_functor {
                    void operator()( bool bNew, value_type& item );
                };
            \endcode
            where:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the map

            The functor may change any fields of \p item.second.

            RCU \p synchronize() method can be called. RCU should not be locked.

            Returns <tt> std::pair<bool, bool> </tt> where \p first is \p true if operation is successful,
            \p second is \p true if new item has been added or \p false if the item with \p key
            already exists.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"
        */
        template <typename K, typename Func>
        std::pair<bool, bool> update( K const& key, Func func, bool bInsert = true )
        {
            scoped_node_ptr pNode( node_allocator().New( random_level(), key ));
            std::pair<bool, bool> res = base_class::update( *pNode,
                [&func](bool bNew, node_type& item, node_type const& ){ func( bNew, item.m_Value );},
                bInsert
            );
            if ( res.first && res.second )
                pNode.release();
            return res;
        }
        //@cond
        template <typename K, typename Func>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<bool, bool> ensure( K const& key, Func func )
        {
            return update( key, func, true );
        }
        //@endcond

        /// Delete \p key from the map
        /**\anchor cds_nonintrusive_SkipListMap_rcu_erase_val

            RCU \p synchronize method can be called. RCU should not be locked.

            Return \p true if \p key is found and deleted, \p false otherwise
        */
        template <typename K>
        bool erase( K const& key )
        {
            return base_class::erase(key);
        }

        /// Deletes the item from the map using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_SkipListMap_rcu_erase_val "erase(K const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less>
        bool erase_with( K const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return base_class::erase_with( key, cds::details::predicate_wrapper< node_type, Less, typename maker::key_accessor >());
        }

        /// Delete \p key from the map
        /** \anchor cds_nonintrusive_SkipListMap_rcu_erase_func

            The function searches an item with key \p key, calls \p f functor
            and deletes the item. If \p key is not found, the functor is not called.

            The functor \p Func interface:
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
            return base_class::erase( key, [&f]( node_type& node) { f( node.m_Value ); } );
        }

        /// Deletes the item from the map using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_SkipListMap_rcu_erase_func "erase(K const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less, typename Func>
        bool erase_with( K const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return base_class::erase_with( key, cds::details::predicate_wrapper< node_type, Less, typename maker::key_accessor >(),
                [&f]( node_type& node) { f( node.m_Value ); } );
        }

        /// Extracts the item from the map with specified \p key
        /** \anchor cds_nonintrusive_SkipListMap_rcu_extract
            The function searches an item with key equal to \p key in the map,
            unlinks it from the set, and returns \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to the item found.
            If the item is not found the function returns an empty \p exempt_ptr

            Note the compare functor from \p Traits class' template argument
            should accept a parameter of type \p K that can be not the same as \p key_type.

            RCU \p synchronize() method can be called. RCU should NOT be locked.

            The function does not free the item found.
            The item will be implicitly freed when the returned object is destroyed or when
            its \p release() member function is called.
        */
        template <typename K>
        exempt_ptr extract( K const& key )
        {
            return exempt_ptr( base_class::do_extract( key ));
        }

        /// Extracts the item from the map with comparing functor \p pred
        /**
            The function is an analog of \p extract(K const&) but \p pred predicate is used for key comparing.
            \p Less has the semantics like \p std::less.
            \p pred must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less>
        exempt_ptr extract_with( K const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return exempt_ptr( base_class::do_extract_with( key, cds::details::predicate_wrapper< node_type, Less, typename maker::key_accessor >()));
        }

        /// Extracts an item with minimal key from the map
        /**
            The function searches an item with minimal key, unlinks it,
            and returns \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to the item.
            If the skip-list is empty the function returns an empty \p exempt_ptr.

            RCU \p synchronize method can be called. RCU should NOT be locked.

            The function does not free the item found.
            The item will be implicitly freed when the returned object is destroyed or when
            its \p release() member function is called.
        */
        exempt_ptr extract_min()
        {
            return exempt_ptr( base_class::do_extract_min());
        }

        /// Extracts an item with maximal key from the map
        /**
            The function searches an item with maximal key, unlinks it from the set,
            and returns \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to the item.
            If the skip-list is empty the function returns an empty \p exempt_ptr.

            RCU \p synchronize method can be called. RCU should NOT be locked.

            The function does not free the item found.
            The item will be implicitly freed when the returned object is destroyed or when
            its \p release() member function is called.
            */
        exempt_ptr extract_max()
        {
            return exempt_ptr( base_class::do_extract_max());
        }

        /// Find the key \p key
        /** \anchor cds_nonintrusive_SkipListMap_rcu_find_cfunc

            The function searches the item with key equal to \p key and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item );
            };
            \endcode
            where \p item is the item found.

            The functor may change \p item.second.

            The function applies RCU lock internally.

            The function returns \p true if \p key is found, \p false otherwise.
        */
        template <typename K, typename Func>
        bool find( K const& key, Func f )
        {
            return base_class::find( key, [&f](node_type& item, K const& ) { f( item.m_Value );});
        }

        /// Finds the key \p val using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_SkipListMap_rcu_find_cfunc "find(K const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less, typename Func>
        bool find_with( K const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return base_class::find_with( key, cds::details::predicate_wrapper< node_type, Less, typename maker::key_accessor >(),
                [&f](node_type& item, K const& ) { f( item.m_Value );});
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
            return contains( key );
        }
        //@endcond

        /// Checks whether the map contains \p key using \p pred predicate for searching
        /**
            The function is similar to <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename K, typename Less>
        bool contains( K const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return base_class::contains( key, cds::details::predicate_wrapper< node_type, Less, typename maker::key_accessor >());
        }
        //@cond
        template <typename K, typename Less>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find_with( K const& key, Less pred )
        {
            return contains( key, pred );
        }
        //@endcond

        /// Finds the key \p key and return the item found
        /** \anchor cds_nonintrusive_SkipListMap_rcu_get
            The function searches the item with key equal to \p key and returns a \p raw_ptr object pointing to an item found.
            If \p key is not found it returns empty \p raw_ptr.

            Note the compare functor in \p Traits class' template argument
            should accept a parameter of type \p K that can be not the same as \p key_type.

            RCU should be locked before call of this function.
            Returned item is valid only while RCU is locked:
            \code
            typedef cds::container::SkipListMap< cds::urcu::gc< cds::urcu::general_buffered<> >, int, foo, my_traits > skip_list;
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
        template <typename K>
        raw_ptr get( K const& key )
        {
            return raw_ptr( base_class::get( key ));
        }

        /// Finds the key \p key and return the item found
        /**
            The function is an analog of \ref cds_nonintrusive_SkipListMap_rcu_get "get(K const&)"
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref key_type and \p K
            in any order.
            \p pred must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less>
        raw_ptr get_with( K const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return raw_ptr( base_class::get_with( key, cds::details::predicate_wrapper< node_type, Less, typename maker::key_accessor >()));
        }

        /// Clears the map (not atomic)
        void clear()
        {
            base_class::clear();
        }

        /// Checks if the map is empty
        /**
            Emptiness is checked by item counting: if item count is zero then the map is empty.
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

        /// Returns const reference to internal statistics
        stat const& statistics() const
        {
            return base_class::statistics();
        }
    };
}} // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_SKIP_LIST_MAP_RCU_H
