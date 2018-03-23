// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_SKIP_LIST_MAP_NOGC_H
#define CDSLIB_CONTAINER_SKIP_LIST_MAP_NOGC_H

#include <cds/container/skip_list_set_nogc.h>

namespace cds { namespace container {
    //@cond
    namespace skip_list { namespace details {
        struct map_key_accessor
        {
            template <typename NodeType>
            typename NodeType::stored_value_type::first_type const& operator()( NodeType const& node ) const
            {
                return node.m_Value.first;
            }
        };
    }} // namespace skip_list::details
    //@endcond

    /// Lock-free skip-list map (template specialization for gc::nogc)
    /** @ingroup cds_nonintrusive_map
        \anchor cds_nonintrusive_SkipListMap_nogc

        This specialization is intended for so-called persistent usage when no item
        reclamation may be performed. The class does not support deleting of map item.
        See \ref cds_nonintrusive_SkipListMap_hp "SkipListMap" for detailed description.

        Template arguments:
        - \p K - type of a key to be stored in the map.
        - \p T - type of a value to be stored in the map.
        - \p Traits - map traits, default is \p skip_list::traits
            It is possible to declare option-based list with \p cds::container::skip_list::make_traits
            metafunction istead of \p Traits template argument.
    */
    template <
        typename Key,
        typename T,
#ifdef CDS_DOXYGEN_INVOKED
        typename Traits = skip_list::traits
#else
        typename Traits
#endif
    >
    class SkipListMap< cds::gc::nogc, Key, T, Traits >:
#ifdef CDS_DOXYGEN_INVOKED
        protected SkipListSet< cds::gc::nogc, std::pair< Key const, T >, Traits >
#else
        protected SkipListSet<
            cds::gc::nogc
            ,std::pair< Key const, T >
            ,typename cds::opt::replace_key_accessor< Traits, skip_list::details::map_key_accessor >::type
        >
#endif
    {
        //@cond
        typedef SkipListSet<
            cds::gc::nogc
            ,std::pair< Key const, T >
            ,typename cds::opt::replace_key_accessor< Traits, skip_list::details::map_key_accessor >::type
        > base_class;
        //@endcond

    public:
        typedef cds::gc::nogc gc;   ///< Garbage collector
        typedef Key key_type;       ///< Key type
        typedef T   mapped_type;    ///< Mapped type
        typedef std::pair< key_type const, mapped_type> value_type; ///< Key-value pair stored in the map
        typedef Traits  traits;     ///< Options specified

        typedef typename base_class::back_off       back_off;       ///< Back-off strategy
        typedef typename base_class::allocator_type allocator_type; ///< Allocator type used for allocate/deallocate the skip-list nodes
        typedef typename base_class::item_counter   item_counter;   ///< Item counting policy
        typedef typename base_class::key_comparator key_comparator; ///< key compare functor
        typedef typename base_class::memory_model   memory_model;   ///< Memory ordering, see \p cds::opt::memory_model option
        typedef typename base_class::stat           stat;           ///< internal statistics type
        typedef typename base_class::random_level_generator random_level_generator; ///< random level generator

    protected:
        //@cond
        typedef typename base_class::node_type      node_type;
        typedef typename base_class::node_allocator node_allocator;
        //@endcond

    public:
        /// Default constructor
        SkipListMap()
            : base_class()
        {}

        /// Destructor clears the map
        ~SkipListMap()
        {}

    public:
    ///@name Forward ordered iterators
    //@{
        /// Forward iterator
        /**
            The forward iterator for a split-list has some features:
            - it has no post-increment operator
            - it depends on iterator of underlying \p OrderedList
        */
        typedef typename base_class::iterator iterator;

        /// Const forward iterator
        typedef typename base_class::const_iterator const_iterator;

        /// Returns a forward iterator addressing the first element in a map
        /**
            For empty set \code begin() == end() \endcode
        */
        iterator begin()
        {
            return base_class::begin();
        }

        /// Returns an iterator that addresses the location succeeding the last element in a map
        /**
            Do not use the value returned by <tt>end</tt> function to access any item.
            The returned value can be used only to control reaching the end of the set.
            For empty set \code begin() == end() \endcode
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
        /// Inserts new node with key and default value
        /**
            The function creates a node with \p key and default value, and then inserts the node created into the map.

            Preconditions:
            - The \ref key_type should be constructible from value of type \p K.
                In trivial case, \p K is equal to \ref key_type.
            - The \ref mapped_type should be default-constructible.

            Returns an iterator pointed to inserted value, or \p end() if inserting is failed
        */
        template <typename K>
        iterator insert( K const& key )
        {
            //TODO: pass arguments by reference (make_pair makes copy)
            return base_class::insert( std::make_pair( key_type( key ), mapped_type()));
        }

        /// Inserts new node
        /**
            The function creates a node with copy of \p val value
            and then inserts the node created into the map.

            Preconditions:
            - The \ref key_type should be constructible from \p key of type \p K.
            - The \ref mapped_type should be constructible from \p val of type \p V.

            Returns an iterator pointed to inserted value, or \p end() if inserting is failed
        */
        template <typename K, typename V>
        iterator insert( K const& key, V const& val )
        {
            //TODO: pass arguments by reference (make_pair makes copy)
            return base_class::insert( std::make_pair( key_type( key ), mapped_type( val )));
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
            to the map's item inserted. <tt>item.second</tt> is a reference to item's value that may be changed.
            User-defined functor \p func should guarantee that during changing item's value no any other changes
            could be made on this map's item by concurrent threads.

            The key_type should be constructible from value of type \p K.

            The function allows to split creating of new item into three part:
            - create item from \p key;
            - insert new item into the map;
            - if inserting is successful, initialize the value of item by calling \p f functor

            This can be useful if complete initialization of object of \p mapped_type is heavyweight and
            it is preferable that the initialization should be completed only if inserting is successful.

            Returns an iterator pointed to inserted value, or \p end() if inserting is failed
        */
        template <typename K, typename Func>
        iterator insert_with( K const& key, Func func )
        {
            iterator it = insert( key );
            if ( it != end())
                func( (*it));
            return it;
        }

        /// For key \p key inserts data of type \p mapped_type created in-place from \p args
        /**
            \p key_type should be constructible from type \p K

            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename K, typename... Args>
        iterator emplace( K&& key, Args&&... args )
        {
            return base_class::emplace( key_type( std::forward<K>( key )), mapped_type( std::forward<Args>(args)... ));
        }

        /// UPdates data by \p key
        /**
            The operation inserts new item if \p key is not found in the map and \p bInsert is \p true.
            Otherwise, if \p key is found, the function returns an iterator that points to item found.

            Returns <tt> std::pair<iterator, bool>  </tt> where \p first is an iterator pointing to
            item found or inserted or \p end() if \p key is not found and insertion is not allowed (\p bInsert is \p false),
            \p second is \p true if new item has been added or \p false if the item already exists.
        */
        template <typename K>
        std::pair<iterator, bool> update( K const& key, bool bInsert = true )
        {
            //TODO: pass arguments by reference (make_pair makes copy)
            return base_class::update( std::make_pair( key_type( key ), mapped_type()), bInsert );
        }
        //@cond
        template <typename K>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<iterator, bool> ensure( K const& key )
        {
            return update( key, true );
        }
        //@endcond

        /// Checks whether the map contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns an iterator pointed to item found if the key is found,
            and \ref end() otherwise
        */
        template <typename K>
        iterator contains( K const& key )
        {
            return base_class::contains( key );
        }
        //@cond

        template <typename K>
        CDS_DEPRECATED("deprecated, use contains()")
        iterator find( K const& key )
        {
            return contains( key );
        }
        //@endcond

        /// Checks whether the map contains \p key using \p pred predicate for searching
        /**
            The function is similar to <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less>
        iterator contains( K const& key, Less pred ) const
        {
            return base_class::contains( key, pred );
        }
        //@cond
        template <typename K, typename Less>
        CDS_DEPRECATED("deprecated, use contains()")
        iterator find_with( K const& key, Less pred ) const
        {
            return contains( key, pred );
        }
        //@endcond

        /// Gets minimum key from the map
        /**
            If the map is empty the function returns \p nullptr
        */
        value_type * get_min() const
        {
            return base_class::get_min();
        }

        /// Gets maximum key from the map
        /**
            The function returns \p nullptr if the map is empty
        */
        value_type * get_max() const
        {
            return base_class::get_max();
        }

        /// Clears the map (not atomic)
        /**
            Finding and/or inserting is prohibited while clearing.
            Otherwise an unpredictable result may be encountered.
            Thus, \p clear() may be used only for debugging purposes.
        */
        void clear()
        {
            base_class::clear();
        }

        /// Checks if the map is empty
        /**
            Emptiness is checked by item counting: if item count is zero then the map is empty.
            Thus, the correct item counting feature is an important part of Michael's map implementation.
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

        /// Returns maximum height of skip-list. The max height is a constant for each object and does not exceed 32.
        static constexpr unsigned int max_height() noexcept
        {
            return base_class::max_height();
        }

        /// Returns const reference to internal statistics
        stat const& statistics() const
        {
            return base_class::statistics();
        }
    };

}} // namespace cds::container


#endif // #ifndef CDSLIB_CONTAINER_SKIP_LIST_MAP_NOGC_H
