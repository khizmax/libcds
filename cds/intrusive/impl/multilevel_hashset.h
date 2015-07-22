//$$CDS-header$$

#ifndef CDSLIB_INTRUSIVE_IMPL_MULTILEVEL_HASHSET_H
#define CDSLIB_INTRUSIVE_IMPL_MULTILEVEL_HASHSET_H

#include <cds/intrusive/details/multilevel_hashset_base.h>
#include <cds/details/allocator.h>

namespace cds { namespace intrusive {
    /// Intrusive hash set based on multi-level array
    /** @ingroup cds_intrusive_map
        @anchor cds_intrusive_MultilevelHashSet_hp

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

        \p %MultiLevelHashSet is a multi-level array whitch has a structure similar to a tree:
        @image html multilevel_hashset.png
        The multi-level array differs from a tree in that each position on the tree could hold an array of nodes or a single node.
        A position that holds a single node is a \p dataNode which holds the hash value of a key and the value that is associated
        with that key; it is a simple struct holding two variables. A \p dataNode in the multi-level array could be marked.
        A \p markedDataNode refers to a pointer to a \p dataNode that has been bitmarked at the least significant bit (LSB)
        of the pointer to the node. This signifies that this \p dataNode is contended. An expansion must occur at this node;
        any thread that sees this \p markedDataNode will try to replace it with an \p arrayNode; which is a position that holds
        an array of nodes. The pointer to an \p arrayNode is differentiated from that of a pointer to a \p dataNode by a bitmark
        on the second-least significant bit.

        \p %MultiLevelHashSet multi-level array is similar to a tree in that we keep a pointer to the root, which is a memory array
        called \p head. The length of the \p head memory array is unique, whereas every other \p arrayNode has a uniform length;
        a normal \p arrayNode has a fixed power-of-two length equal to the binary logarithm of a variable called \p arrayLength.
        The maximum depth of the tree, \p maxDepth, is the maximum number of pointers that must be followed to reach any node.
        We define \p currentDepth as the number of memory arrays that we need to traverse to reach the \p arrayNode on which
        we need to operate; this is initially one, because of \p head.

        That approach to the structure of the hash map uses an extensible hashing scheme; <b> the hash value is treated as a bit
        string</b> and rehash incrementally.

        @note Two important things you should keep in mind when you're using \p %MultiLevelHashSet:
        - all keys must be fixed-size. It means that you cannot use \p std::string as a key for \p %MultiLevelHashSet.
          Instead, for the strings you should use well-known hashing algorithms like <a href="https://en.wikipedia.org/wiki/Secure_Hash_Algorithm">SHA1, SHA2</a>,
          <a href="https://en.wikipedia.org/wiki/MurmurHash">MurmurHash</a>, <a href="https://en.wikipedia.org/wiki/CityHash">CityHash</a> 
          or its successor <a href="https://code.google.com/p/farmhash/">FarmHash</a> and so on, which
          converts variable-length strings to fixed-length bit-strings, and use that hash as a key in \p %MultiLevelHashSet.
        - \p %MultiLevelHashSet uses a perfect hashing. It means that if two different keys, for example, of type \p std::string,
          have identical hash then you cannot insert both that keys in the set. \p %MultiLevelHashSet does not maintain the key,
          it maintains its fixed-size hash value.


        There are several specializations of \p %MultiLevelHashSet for each \p GC. You should include:
        - <tt><cds/intrusive/multilevel_hashset_hp.h></tt> for \p gc::HP garbage collector
        - <tt><cds/intrusive/multilevel_hashset_dhp.h></tt> for \p gc::DHP garbage collector
        - <tt><cds/intrusive/multilevel_hashset_nogc.h></tt> for \ref cds_intrusive_MultiLevelHashSet_nogc for append-only set
        - <tt><cds/intrusive/multilevel_hashset_rcu.h></tt> for \ref cds_intrusive_MultiLevelHashSet_rcu "RCU type"
    */
    template <
        class GC
        ,typename T
#ifdef CDS_DOXYGEN_INVOKED
       ,typename Traits = multilevel_hashset::traits
#else
       ,typename Traits
#endif
    >
    class MultiLevelHashSet
    {
    public:
        typedef GC      gc;         ///< Garbage collector
        typedef T       value_type; ///< type of value stored in the set
        typedef Traits  traits;     ///< Traits template parameter, see \p multilevel_hashset::traits

        typedef typename traits::hash_accessor hash_accessor; ///< Hash accessor functor
        static_assert(!std::is_same< hash_accessor, cds::opt::none >::value, "hash_accessor functor must be specified" );

        /// Hash type defined as \p hash_accessor return type
        typedef typename std::decay< 
            typename std::remove_reference<
                decltype( hash_accessor()( std::declval<T>()) )
            >::type
        >::type hash_type;
        static_assert( !std::is_pointer<hash_type>, "hash_accessor should return a reference to hash value" );

        typedef typename traits::disposer disposer; ///< data node disposer

#   ifdef CDS_DOXYGEN_INVOKED
        typedef implementation_defined hash_comparator  ;    ///< hash compare functor based on opt::compare and opt::less option setter
#   else
        typedef typename cds::opt::details::make_comparator_from< 
            hash_type,
            traits,
            multilevel_hashset::bitwise_compare< hash_type >
        >::type hash_comparator;
#   endif

        typedef typename traits::item_counter         item_counter;         ///< Item counter type
        typedef typename traits::head_node_allocator  head_node_allocator;  ///< Head node allocator
        typedef typename traits::array_node_allocator array_node_allocator; ///< Array node allocator
        typedef typename traits::memory_model         memory_model;         ///< Memory model
        typedef typename traits::back_off             back_off;             ///< Backoff strategy
        typedef typename traits::stat                 stat;                 ///< Internal statistics type

        typedef typename gc::template guarded_ptr< value_type > guarded_ptr; ///< Guarded pointer

    protected:
        //@cond
        enum cell_flags {
            logically_deleted = 1,  ///< the cell (data node) is logically deleted
            array_converting = 2,   ///< the cell is converting from data node to an array node
            array_node = 4          ///< the cell is a pointer to an array node
        };
        typedef cds::details::marked_ptr< value_type, 7 > node_ptr;
        typedef atomics::atomic< node_ptr * >  atomic_node_ptr;

        typedef cds::details::Allocator< atomic_node_ptr, head_node_allocator > cxx_head_node_allocator;
        typedef cds::details::Allocator< atomic_node_ptr, array_node_allocator > cxx_array_node_allocator;

        struct metrics {
            size_t  head_node_size;     // power-of-two
            size_t  head_node_size_log; // log2( head_node_size )
            size_t  array_node_size;    // power-of-two
            size_t  array_node_size_log;// log2( array_node_size )
        };
        //@endcond

    private:
        //@cond
        metrics const     m_Metrics;     ///< Metrics
        atomic_node_ptr * m_Head;        ///< Head array
        item_counter      m_ItemCounter; ///< Item counter
        stat              m_Stat;        ///< Internal statistics
        //@endcond

    public:
        /// Creates empty set
        /**
            @param head_bits: 2<sup>head_bits</sup> specifies the size of head array, minimum is 8.
            @param array_bits: 2<sup>array_bits</sup> specifies the size of array node, minimum is 2.

            Equation for \p head_bits and \p array_bits:
            \code
            sizeof(hash_type) * 8 == head_bits + N * array_bits
            \endcode
            where \p N is multi-level array depth.
        */
        MultiLevelHashSet( size_t head_bits = 8, size_t array_bits = 4 )
            : m_Metrics(make_metrics( head_bits, array_bits ))
            , m_Head( cxx_head_node_allocator().NewArray( m_Metrics.head_node_size, nullptr ))
        {}

        /// Destructs the set and frees all data
        ~MultiLevelHashSet()
        {
            destroy_tree();
            cxx_head_node_allocator().Delete( m_Head, m_Metrics.head_node_size );
        }

        /// Inserts new node
        /**
            The function inserts \p val in the set if it does not contain 
            an item with that hash.

            Returns \p true if \p val is placed into the set, \p false otherwise.
        */
        bool insert( value_type& val )
        {
        }

        /// Inserts new node
        /**
            This function is intended for derived non-intrusive containers.

            The function allows to split creating of new item into two part:
            - create item with key only
            - insert new item into the set
            - if inserting is success, calls  \p f functor to initialize \p val.

            The functor signature is:
            \code
                void func( value_type& val );
            \endcode
            where \p val is the item inserted.

            The user-defined functor is called only if the inserting is success.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting".
        */
        template <typename Func>
        bool insert( value_type& val, Func f )
        {
        }

        /// Updates the node
        /**
            Performs inserting or updating the item with hash value equal to \p val.
            - If hash value is found then existing item is replaced with \p val, old item is disposed
              with \p Traits::disposer. Note that the disposer is called by \p GC asynchronously.
              The function returns <tt> std::pair<true, false> </tt>
            - If hash value is not found and \p bInsert is \p true then \p val is inserted,
              the function returns <tt> std::pair<true, true> </tt>
            - If hash value is not found and \p bInsert is \p false then the set is unchanged,
              the function returns <tt> std::pair<false, false> </tt>

            Returns <tt> std::pair<bool, bool> </tt> where \p first is \p true if operation is successfull
            (i.e. the item has been inserted or updated),
            \p second is \p true if new item has been added or \p false if the set contains that hash.
        */
        template <typename Func>
        std::pair<bool, bool> update( value_type& val, bool bInsert = true )
        {
        }

        /// Unlinks the item \p val from the set
        /**
            The function searches the item \p val in the set and unlink it
            if it is found and its address is equal to <tt>&val</tt>.

            The function returns \p true if success and \p false otherwise.
        */
        bool unlink( value_type& val )
        {
        }

        /// Deletes the item from the set
        /** 
            The function searches \p hash in the set,
            unlinks the item found, and returns \p true.
            If that item is not found the function returns \p false.

            The \ref disposer specified in \p Traits is called by garbage collector \p GC asynchronously.

        */
        bool erase( hash_type const& hash )
        {
        }

        /// Deletes the item from the set
        /**
            The function searches \p hash in the set,
            call \p f functor with item found, and unlinks it from the set.
            The \ref disposer specified in \p Traits is called
            by garbage collector \p GC asynchronously.

            The \p Func interface is
            \code
            struct functor {
                void operator()( value_type const& item );
            };
            \endcode

            If \p hash is not found the function returns \p false.
        */
        template <typename Func>
        bool erase( hash_type const& hash, Func f )
        {
        }

        /// Extracts the item with specified \p hash
        /** 
            The function searches \p hash in the set,
            unlinks it from the set, and returns an guarded pointer to the item extracted.
            If \p hash is not found the function returns an empty guarded pointer.

            The \p disposer specified in \p Traits class' template parameter is called automatically
            by garbage collector \p GC when returned \ref guarded_ptr object to be destroyed or released.
            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.

            Usage:
            \code
            typedef cds::intrusive::MultiLevelHashSet< your_template_args > my_set;
            my_set theSet;
            // ...
            {
                my_set::guarded_ptr gp( theSet.extract( 5 ));
                if ( gp ) {
                    // Deal with gp
                    // ...
                }
                // Destructor of gp releases internal HP guard
            }
            \endcode
        */
        guarded_ptr extract( hash_type const& hash )
        {
        }

        /// Finds an item by it's \p hash
        /**
            The function searches the item by \p hash and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item );
            };
            \endcode
            where \p item is the item found.

            The functor may change non-key fields of \p item. Note that the functor is only guarantee
            that \p item cannot be disposed during the functor is executing.
            The functor does not serialize simultaneous access to the set's \p item. If such access is
            possible you must provide your own synchronization schema on item level to prevent unsafe item modifications.

            The function returns \p true if \p hash is found, \p false otherwise.
        */
        template <typename Func>
        bool find( hash_type const& hash, Func f )
        {
        }

        /// Checks whether the set contains \p hash
        /**
            The function searches the item by its \p hash
            and returns \p true if it is found, or \p false otherwise.
        */
        bool contains( hash_type const& hash )
        {
        }

        /// Finds an item by it's \p hash and returns the item found
        /**
            The function searches the item by its \p hash
            and returns the guarded pointer to the item found.
            If \p hash is not found the function returns an empty \p guarded_ptr.

            @note Each \p guarded_ptr object uses one GC's guard which can be limited resource.

            Usage:
            \code
            typedef cds::intrusive::MultiLevelHashSet< your_template_params >  my_set;
            my_set theSet;
            // ...
            {
                my_set::guarded_ptr gp( theSet.get( 5 ));
                if ( theSet.get( 5 )) {
                    // Deal with gp
                    //...
                }
                // Destructor of guarded_ptr releases internal HP guard
            }
            \endcode
        */
        guarded_ptr get( hash_type const& hash )
        {
        }

        /// Clears the set (non-atomic)
        /**
            The function unlink all items from the set.
            The function is not atomic. It removes all data nodes and then resets the item counter to zero.
            If there are a thread that performs insertion while \p %clear() is working the result is undefined in general case:
            \p empty() may return \p true but the set may contain item(s).
            Therefore, \p %clear() may be used only for debugging purposes.

            For each item the \p disposer is called after unlinking.
        */
        void clear()
        {
        }

        /// Checks if the set is empty
        /**
            Emptiness is checked by item counting: if item count is zero then the set is empty.
            Thus, the correct item counting feature is an important part of the set implementation.
        */
        bool empty() const
        {
            return size() == 0;
        }

        /// Returns item count in the set
        size_t size() const
        {
            return m_ItemCounter;
        }

        /// Returns const reference to internal statistics
        stat const& statistics() const
        {
            return m_Stat;
        }

        /// Returns the size of head node
        size_t head_size() const
        {
            return m_Metrics.head_node_size;
        }

        /// Returns the size of the array node
        size_t array_node_size() const
        {
            return m_Metrics.array_node_size;
        }

    private:
        //@cond
        static metrics make_metrics( size_t head_bits, size_t array_bits )
        {
            size_t const hash_bits = sizeof( hash_type ) * 8;

            if ( array_bits < 2 )
                array_bits = 2;
            if ( head_bits < 8 )
                head_bits = 8;
            if ( head_bits > hash_bits )
                head_bits = hash_bits;
            if ( (hash_bits - head_bits) % array_bits != 0 )
                head_bits += (hash_bits - head_bits) % array_bits;

            assert( (hash_bits - head_bits) % array_bits == 0 );

            metrics m;
            m.head_node_size_log = head_bits;
            m.head_node_size = 1 << head_bits;
            m.array_node_size_log = array_bits;
            m.array_node_size = 1 << array_bits;
            return m;
        }

        template <typename T>
        static bool check_node_alignment( T * p )
        {
            return (reinterpret_cast<uintptr_t>(p) & node_ptr::bitmask) == 0;
        }

        void destroy_tree()
        {
            // The function is not thread-safe. For use in dtor only
            // Remove data node
            clear();

            //TODO: free all array nodes
        }
        //@endcond
    };
}} // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_IMPL_MULTILEVEL_HASHSET_H
