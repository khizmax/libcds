//$$CDS-header$$

#ifndef CDSLIB_CONTAINER_IMPL_MULTILEVEL_HASHMAP_H
#define CDSLIB_CONTAINER_IMPL_MULTILEVEL_HASHMAP_H

#include <cds/intrusive/impl/multilevel_hashset.h>
#include <cds/container/details/multilevel_hashmap_base.h>

namespace cds { namespace container {

    /// Hash map based on multi-level array
    /** @ingroup cds_nonintrusive_map
        @anchor cds_container_MultilevelHashMap_hp

        Source:
        - [2013] Steven Feldman, Pierre LaBorde, Damian Dechev "Concurrent Multi-level Arrays:
                 Wait-free Extensible Hash Maps"

        [From the paper] The hardest problem encountered while developing a parallel hash map is how to perform
        a global resize, the process of redistributing the elements in a hash map that occurs when adding new
        buckets. The negative impact of blocking synchronization is multiplied during a global resize, because all
        threads will be forced to wait on the thread that is performing the involved process of resizing the hash map
        and redistributing the elements. \p %MultilevelHashSet implementation avoids global resizes through new array
        allocation. By allowing concurrent expansion this structure is free from the overhead of an explicit resize,
        which facilitates concurrent operations.

        The presented design includes dynamic hashing, the use of sub-arrays within the hash map data structure;
        which, in combination with <b>perfect hashing</b>, means that each element has a unique final, as well as current, position.
        It is important to note that the perfect hash function required by our hash map is trivial to realize as
        any hash function that permutes the bits of the key is suitable. This is possible because of our approach
        to the hash function; we require that it produces hash values that are equal in size to that of the key.
        We know that if we expand the hash map a fixed number of times there can be no collision as duplicate keys
        are not provided for in the standard semantics of a hash map.

        \p %MultiLevelHashMap is a multi-level array which has an internal structure similar to a tree:
        @image html multilevel_hashset.png
        The multi-level array differs from a tree in that each position on the tree could hold an array of nodes or a single node.
        A position that holds a single node is a \p dataNode which holds the hash value of a key and the value that is associated
        with that key; it is a simple struct holding two variables. A \p dataNode in the multi-level array could be marked.
        A \p markedDataNode refers to a pointer to a \p dataNode that has been bitmarked at the least significant bit (LSB)
        of the pointer to the node. This signifies that this \p dataNode is contended. An expansion must occur at this node;
        any thread that sees this \p markedDataNode will try to replace it with an \p arrayNode; which is a position that holds
        an array of nodes. The pointer to an \p arrayNode is differentiated from that of a pointer to a \p dataNode by a bitmark
        on the second-least significant bit.

        \p %MultiLevelHashMap multi-level array is similar to a tree in that we keep a pointer to the root, which is a memory array
        called \p head. The length of the \p head memory array is unique, whereas every other \p arrayNode has a uniform length;
        a normal \p arrayNode has a fixed power-of-two length equal to the binary logarithm of a variable called \p arrayLength.
        The maximum depth of the tree, \p maxDepth, is the maximum number of pointers that must be followed to reach any node.
        We define \p currentDepth as the number of memory arrays that we need to traverse to reach the \p arrayNode on which
        we need to operate; this is initially one, because of \p head.

        That approach to the structure of the hash set uses an extensible hashing scheme; <b> the hash value is treated as a bit
        string</b> and rehash incrementally.

        @note Two important things you should keep in mind when you're using \p %MultiLevelHashMap:
        - all keys is converted to fixed-size bit-string by hash functor provided. 
          You can use variable-length keys, for example, \p std::string as a key for \p %MultiLevelHashMap,
          but real key in the map will be fixed-ize hash values of your keys.
          For the strings you may use well-known hashing algorithms like <a href="https://en.wikipedia.org/wiki/Secure_Hash_Algorithm">SHA1, SHA2</a>,
          <a href="https://en.wikipedia.org/wiki/MurmurHash">MurmurHash</a>, <a href="https://en.wikipedia.org/wiki/CityHash">CityHash</a> 
          or its successor <a href="https://code.google.com/p/farmhash/">FarmHash</a> and so on, which
          converts variable-length strings to fixed-length bit-strings, and such hash values will be the keys in \p %MultiLevelHashMap.
        - \p %MultiLevelHashMap uses a perfect hashing. It means that if two different keys, for example, of type \p std::string,
          have identical hash then you cannot insert both that keys in the set. \p %MultiLevelHashMap does not maintain the key,
          it maintains its fixed-size hash value.

        The set supports @ref cds_container_MultilevelHashMap_iterators "bidirectional thread-safe iterators".

        Template parameters:
        - \p GC - safe memory reclamation schema. Can be \p gc::HP, \p gc::DHP or one of \ref cds_urcu_type "RCU type"
        - \p Key - a key type to be stored in the map
        - \p T - a value type to be stored in the map
        - \p Traits - type traits, the structure based on \p multilevel_hashmap::traits or result of \p multilevel_hashmap::make_traits metafunction.

        There are several specializations of \p %MultiLevelHashMap for each \p GC. You should include:
        - <tt><cds/container/multilevel_hashmap_hp.h></tt> for \p gc::HP garbage collector
        - <tt><cds/container/multilevel_hashmap_dhp.h></tt> for \p gc::DHP garbage collector
        - <tt><cds/container/multilevel_hashmap_rcu.h></tt> for \ref cds_intrusive_MultiLevelHashSet_rcu "RCU type". RCU specialization
            has a slightly different interface.
    */
    template < 
        class GC
        ,typename Key
        ,typename T
#ifdef CDS_DOXYGEN_INVOKED
        ,class Traits = multilevel_hashmap::traits 
#else
        ,class Traits
#endif
    >
    class MultiLevelHashMap
#ifdef CDS_DOXYGEN_INVOKED
        : protected cds::intrusive::MultiLevelHashSet< GC, std::pair<Key const, T>, Traits >
#else
        : protected cds::container::details::make_multilevel_hashmap< GC, Key, T, Hasher, Traits >::type
#endif
    {
        //@cond
        typedef cds::container::details::make_multilevel_hashmap< GC, Key, T, Hasher, Traits > maker;
        typedef typename maker::type base_class;
        //@endcond
    public:
        typedef GC      gc;          ///< Garbage collector
        typedef Key     key_type;    ///< Key type
        typedef T       mapped_type; ///< Mapped type
        typedef std::pair< key_type const, mapped_type> value_type;   ///< Key-value pair to be stored in the map
        typedef Traits  traits;      ///< Map traits
#ifdef CDS_DOXYGEN_INVOKED
        typedef typename traits::hash hasher; ///< Hash functor, see \p multilevel_hashmap::traits::hash
#else
        typedef typename maker::hasher hasher;
#endif

        typedef typename maker::hash_type hash_type; ///< Hash type deduced from \p hasher return type
        typedef typename base_class::hash_comparator hash_comparator; ///< hash compare functor based on \p Traits::compare and \p Traits::less

        typedef typename traits::item_counter   item_counter;   ///< Item counter type
        typedef typename traits::allocator      allocator;      ///< Element allocator
        typedef typename traits::node_allocator node_allocator; ///< Array node allocator
        typedef typename traits::memory_model   memory_model;   ///< Memory model
        typedef typename traits::back_off       back_off;       ///< Backoff strategy
        typedef typename traits::stat           stat;           ///< Internal statistics type

        typedef typename gc::template guarded_ptr< value_type > guarded_ptr; ///< Guarded pointer

        /// Count of hazard pointers required
        static CDS_CONSTEXPR size_t const c_nHazardPtrCount = base_class::c_nHazardPtrCount;

    protected:
        //@cond
        typedef typename maker::node_type node_type;
        typedef typename maker::cxx_node_allocator cxx_node_allocator;
        typedef std::unique_ptr< node_type, typename maker::node_disposer > scoped_node_ptr;
        //@endcond

    protected:
        //@cond
        hasher  m_Hasher;
        //@endcond

    public:
        /// Creates empty map
        /**
            @param head_bits: 2<sup>head_bits</sup> specifies the size of head array, minimum is 4.
            @param array_bits: 2<sup>array_bits</sup> specifies the size of array node, minimum is 2.

            Equation for \p head_bits and \p array_bits:
            \code
            sizeof(hash_type) * 8 == head_bits + N * array_bits
            \endcode
            where \p N is multi-level array depth.
        */
        MultiLevelHashMap( size_t head_bits = 8, size_t array_bits = 4 )
            : base_class( head_bits, array_bits )
        {}

        /// Destructs the map and frees all data
        ~MultiLevelHashMap()
        {}

        /// Inserts new element with key and default value
        /**
            The function creates an element with \p key and default value, and then inserts the node created into the map.

            Preconditions:
            - The \p key_type should be constructible from a value of type \p K.
                In trivial case, \p K is equal to \p key_type.
            - The \p mapped_type should be default-constructible.

            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename K>
        bool insert( K const& key )
        {
            scoped_node_ptr sp( cxx_node_allocator().New( m_Hasher, key ));
            if ( base_class::insert( *sp )) {
                sp.release();
                return true;
            }
            return false;
        }

        /// Inserts new element
        /**
            The function creates a node with copy of \p val value
            and then inserts the node created into the map.

            Preconditions:
            - The \p key_type should be constructible from \p key of type \p K.
            - The \p value_type should be constructible from \p val of type \p V.

            Returns \p true if \p val is inserted into the set, \p false otherwise.
        */
        template <typename K, typename V>
        bool insert( K const& key, V const& val )
        {
            scoped_node_ptr sp( cxx_node_allocator().New( m_Hasher, key, val ));
            if ( base_class::insert( *sp )) {
                sp.release();
                return true;
            }
            return false;
        }

        /// Inserts new element and initialize it by a functor
        /**
            This function inserts new element with key \p key and if inserting is successful then it calls
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

            \p key_type should be constructible from value of type \p K.

            The function allows to split creating of new item into two part:
            - create item from \p key;
            - insert new item into the map;
            - if inserting is successful, initialize the value of item by calling \p func functor

            This can be useful if complete initialization of object of \p value_type is heavyweight and
            it is preferable that the initialization should be completed only if inserting is successful.
        */
        template <typename K, typename Func>
        bool insert_with( K const& key, Func func )
        {
            scoped_node_ptr sp( cxx_node_allocator().New( m_Hasher, key ));
            if ( base_class::insert( *sp, [&func]( node_type& item ) { func( item.m_Value ); } )) {
                sp.release();
                return true;
            }
            return false;
        }

        /// For key \p key inserts data of type \p value_type created in-place from <tt>std::forward<Args>(args)...</tt>
        /**
            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename K, typename... Args>
        bool emplace( K&& key, Args&&... args )
        {
            scoped_node_ptr sp( cxx_node_allocator().New( m_Hasher, std::forward<K>(key), std::forward<Args>(args)... ));
            if ( base_class::insert( *sp )) {
                sp.release();
                return true;
            }
            return false;
        }

        /// Updates data by \p key
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the \p key not found in the map, then the new item created from \p key
            will be inserted into the map iff \p bInsert is \p true
            (note that in this case the \ref key_type should be constructible from type \p K).
            Otherwise, if \p key is found, the functor \p func is called with item found.
            The functor \p Func signature:
            \code
                struct my_functor {
                    void operator()( bool bNew, value_type& item );
                };
            \endcode
            where:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the map

            The functor may change any fields of the \p item.second that is \ref value_type.

            Returns <tt> std::pair<bool, bool> </tt> where \p first is \p true if operation is successfull,
            \p second is \p true if new item has been added or \p false if \p key already exists.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"
        */
        template <typename K, typename Func>
        std::pair<bool, bool> update( K const& key, Func func, bool bInsert = true )
        {
            scoped_node_ptr sp( cxx_node_allocator().New( m_Hasher, key ));
            std::pair<bool, bool> result = base_class::do_update( *sp, [&func]( bool bNew, node_type& node ) { func( bNew, node.m_Value );}, bInsert );
            if ( !result.first )
                sp.release();
            return result;
        }

        /// Delete \p key from the map
        /**
            \p key_type must be constructible from value of type \p K.

            Return \p true if \p key is found and deleted, \p false otherwise.
        */
        template <typename K>
        bool erase( K const& key )
        {
            hash_type h = m_Hasher( key_type( key ));
            return base_class::erase( h );
        }

        /// Delete \p key from the map
        /**
            The function searches an item with key \p key, calls \p f functor
            and deletes the item. If \p key is not found, the functor is not called.

            The functor \p Func interface:
            \code
            struct extractor {
                void operator()(value_type& item) { ... }
            };
            \endcode
            \p key_type must be constructible from value of type \p K.

            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename K, typename Func>
        bool erase( K const& key, Func f )
        {
            hash_type h = m_Hasher( key_type( key ));
            return base_class::erase( h, [&f]( node_type& node) { f( node.m_Value ); } );
        }

        /// Deletes the element pointed by iterator \p iter
        /**
            Returns \p true if the operation is successful, \p false otherwise.

            The function does not invalidate the iterator, it remains valid and can be used for further traversing.
        */
        bool erase_at( iterator const& iter )
        {
            //TODO
        }


    };

}} // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_IMPL_MULTILEVEL_HASHMAP_H
