//$$CDS-header$$

#ifndef CDSLIB_CONTAINER_SPLIT_LIST_MAP_NOGC_H
#define CDSLIB_CONTAINER_SPLIT_LIST_MAP_NOGC_H

#include <cds/container/split_list_set_nogc.h>
#include <cds/details/binary_functor_wrapper.h>

namespace cds { namespace container {

    /// Split-ordered list map (template specialization for gc::nogc)
    /** @ingroup cds_nonintrusive_map
        \anchor cds_nonintrusive_SplitListMap_nogc

        This specialization is so-called append-only.
        The map does not support the removal of list item.

        See \ref cds_nonintrusive_SplitListMap_hp "SplitListMap" for description of template parameters.

        @warning Many member functions return an iterator pointing to an item.
        The iterator can be used to set up field of the item,
        but you should provide an exclusive access to it,
        see \ref cds_intrusive_item_creating "insert item troubleshooting".
    */
    template <
        typename Key,
        typename Value,
#ifdef CDS_DOXYGEN_INVOKED
        class Traits = split_list::traits
#else
        class Traits
#endif
    >
    class SplitListMap<cds::gc::nogc, Key, Value, Traits>:
        protected container::SplitListSet<
            cds::gc::nogc,
            std::pair<Key const, Value>,
            split_list::details::wrap_map_traits<Key, Value, Traits>
        >
    {
        //@cond
        typedef container::SplitListSet<
            cds::gc::nogc,
            std::pair<Key const, Value>,
            split_list::details::wrap_map_traits<Key, Value, Traits>
        > base_class;
        //@endcond
    public:
        typedef cds::gc::nogc gc;          ///< Garbage collector
        typedef Key           key_type;    ///< key type
        typedef Value         mapped_type; ///< type of value stored in the map

        typedef std::pair<key_type const, mapped_type>  value_type  ;   ///< Pair type
        typedef typename base_class::ordered_list       ordered_list;   ///< Underlying ordered list class
        typedef typename base_class::key_comparator     key_comparator; ///< key comparison functor

        typedef typename base_class::hash           hash;         ///< Hash functor for \ref key_type
        typedef typename base_class::item_counter   item_counter; ///< Item counter type
        typedef typename base_class::stat           stat;         ///< Internal statistics

        //@cond
        typedef cds::container::split_list::implementation_tag implementation_tag;
        //@endcond

    protected:
        //@cond
        typedef typename base_class::traits::key_accessor key_accessor;
        //@endcond

    public:
        /// Forward iterator (see \p SplitListSet::iterator)
        /**
            Remember, the iterator <tt>operator -> </tt> and <tt>operator *</tt> returns \ref value_type pointer and reference.
            To access item key and value use <tt>it->first</tt> and <tt>it->second</tt> respectively.
        */
        typedef typename base_class::iterator iterator;

        /// Const forward iterator (see SplitListSet::const_iterator)
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
        /// Initialize split-ordered map of default capacity
        /**
            The default capacity is defined in bucket table constructor.
            See \p intrusive::split_list::expandable_bucket_table, \p intrusive::split_list::static_ducket_table
            which selects by \p intrusive::split_list::traits::dynamic_bucket_table.
        */
        SplitListMap()
            : base_class()
        {}

        /// Initialize split-ordered map
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
            - The \p key_type should be constructible from value of type \p K.
                In trivial case, \p K is equal to \ref key_type.
            - The \p mapped_type should be default-constructible.

            Returns an iterator pointed to inserted value, or \p end() if inserting is failed
        */
        template <typename K>
        iterator insert( K const& key )
        {
            //TODO: pass arguments by reference (make_pair makes copy)
            return base_class::insert( std::make_pair( key, mapped_type() ) );
        }

        /// Inserts new node
        /**
            The function creates a node with copy of \p val value
            and then inserts the node created into the map.

            Preconditions:
            - The \p key_type should be constructible from \p key of type \p K.
            - The \p mapped_type should be constructible from \p val of type \p V.

            Returns an iterator pointed to inserted value, or \p end() if inserting is failed
        */
        template <typename K, typename V>
        iterator insert( K const& key, V const& val )
        {
            //TODO: pass arguments by reference (make_pair makes copy)
            return base_class::insert( std::make_pair( key, val ) );
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
            to the map's item inserted. \p item.second is a reference to item's value that may be changed.
            User-defined functor \p func should guarantee that during changing item's value no any other changes
            could be made on this map's item by concurrent threads.
            The user-defined functor is called only if the inserting is successful.

            The \p key_type should be constructible from value of type \p K.

            The function allows to split creating of new item into two part:
            - create item from \p key;
            - insert new item into the map;
            - if inserting is successful, initialize the value of item by calling \p f functor

            This can be useful if complete initialization of object of \p mapped_type is heavyweight and
            it is preferable that the initialization should be completed only if inserting is successful.

            Returns an iterator pointed to inserted value, or \p end() if inserting is failed
        */
        template <typename K, typename Func>
        iterator insert_with( const K& key, Func func )
        {
            iterator it = insert( key );
            if ( it != end() )
                func( (*it) );
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
            return base_class::emplace( std::forward<K>(key), std::move(mapped_type(std::forward<Args>(args)...)));
        }

        /// Ensures that the key \p key exists in the map
        /**
            The operation inserts new item if the key \p key is not found in the map.
            Otherwise, the function returns an iterator that points to item found.

            Returns <tt> std::pair<iterator, bool>  </tt> where \p first is an iterator pointing to
            item found or inserted, \p second is true if new item has been added or \p false if the item
            already is in the list.
        */
        template <typename K>
        std::pair<iterator, bool> ensure( K const& key )
        {
            //TODO: pass arguments by reference (make_pair makes copy)
            return base_class::ensure( std::make_pair( key, mapped_type() ));
        }

        /// Find the key \p key
        /** \anchor cds_nonintrusive_SplitListMap_nogc_find

            The function searches the item with key equal to \p key
            and returns an iterator pointed to item found if the key is found,
            and \p end() otherwise
        */
        template <typename K>
        iterator find( K const& key )
        {
            return base_class::find( key );
        }

        /// Finds the key \p key using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_SplitListMap_nogc_find "find(K const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename K, typename Less>
        iterator find_with( K const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return base_class::find_with( key, cds::details::predicate_wrapper<value_type, Less, key_accessor>() );
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

        /// Returns internal statistics
        stat const& statistics() const
        {
            return base_class::statistics();
        }
    };
}}  // namespace cds::container


#endif // #ifndef CDSLIB_CONTAINER_SPLIT_LIST_MAP_NOGC_H
