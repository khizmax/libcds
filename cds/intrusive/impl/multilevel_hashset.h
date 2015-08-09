//$$CDS-header$$

#ifndef CDSLIB_INTRUSIVE_IMPL_MULTILEVEL_HASHSET_H
#define CDSLIB_INTRUSIVE_IMPL_MULTILEVEL_HASHSET_H

#include <tuple> // std::tie
#include <functional> // std::ref

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

        Template parameters:
        - \p GC - safe memory reclamation schema. Can be \p gc::HP, \p gc::DHP or one of \ref cds_urcu_type "RCU type"
        - \p T - a value type to be stored in the set
        - \p Traits - type traits, the structure based on \p multilevel_hashset::traits or result of \p multilevel_hashset::make_traits metafunction

        There are several specializations of \p %MultiLevelHashSet for each \p GC. You should include:
        - <tt><cds/intrusive/multilevel_hashset_hp.h></tt> for \p gc::HP garbage collector
        - <tt><cds/intrusive/multilevel_hashset_dhp.h></tt> for \p gc::DHP garbage collector
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
        static_assert( !std::is_pointer<hash_type>::value, "hash_accessor should return a reference to hash value" );

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

        /// Count of hazard pointers required
        static CDS_CONSTEXPR size_t const c_nHazardPtrCount = 1;

        /// Node marked poiter
        typedef cds::details::marked_ptr< value_type, 3 > node_ptr;
        /// Array node element
        typedef atomics::atomic< node_ptr > atomic_node_ptr;

    protected:
        //@cond
        enum node_flags {
            array_converting = 1,   ///< the cell is converting from data node to an array node
            array_node = 2          ///< the cell is a pointer to an array node
        };

        typedef cds::details::Allocator< atomic_node_ptr, head_node_allocator > cxx_head_node_allocator;
        typedef cds::details::Allocator< atomic_node_ptr, array_node_allocator > cxx_array_node_allocator;

        typedef multilevel_hashset::details::hash_splitter< hash_type > hash_splitter;

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
            @param head_bits: 2<sup>head_bits</sup> specifies the size of head array, minimum is 4.
            @param array_bits: 2<sup>array_bits</sup> specifies the size of array node, minimum is 2.

            Equation for \p head_bits and \p array_bits:
            \code
            sizeof(hash_type) * 8 == head_bits + N * array_bits
            \endcode
            where \p N is multi-level array depth.
        */
        MultiLevelHashSet( size_t head_bits = 8, size_t array_bits = 4 )
            : m_Metrics(make_metrics( head_bits, array_bits ))
            , m_Head( cxx_head_node_allocator().NewArray( head_size(), nullptr ))
        {}

        /// Destructs the set and frees all data
        ~MultiLevelHashSet()
        {
            destroy_tree();
            cxx_array_node_allocator().Delete( m_Head, head_size() );
        }

        /// Inserts new node
        /**
            The function inserts \p val in the set if it does not contain 
            an item with that hash.

            Returns \p true if \p val is placed into the set, \p false otherwise.
        */
        bool insert( value_type& val )
        {
            return insert( val, [](value_type&) {} );
        }

        /// Inserts new node
        /**
            This function is intended for derived non-intrusive containers.

            The function allows to split creating of new item into two part:
            - create item with key only
            - insert new item into the set
            - if inserting is success, calls \p f functor to initialize \p val.

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
            hash_type const& hash = hash_accessor()( val );
            hash_splitter splitter( hash );
            hash_comparator cmp;
            typename gc::Guard guard;
            back_off bkoff;

            size_t nOffset = m_Metrics.head_node_size_log;
            atomic_node_ptr * pArr = m_Head;
            size_t nSlot = splitter.cut( m_Metrics.head_node_size_log );
            assert( nSlot < m_Metrics.head_node_size );

            while ( true ) {
                node_ptr slot = pArr[nSlot].load( memory_model::memory_order_acquire );
                if ( slot.bits() == array_node ) {
                    // array node, go down the tree
                    assert( slot.ptr() != nullptr );
                    nSlot = splitter.cut( m_Metrics.array_node_size_log );
                    assert( nSlot < m_Metrics.array_node_size );
                    pArr = to_array( slot.ptr() );
                    nOffset += m_Metrics.array_node_size_log;
                }
                else if ( slot.bits() == array_converting ) {
                    // the slot is converting to array node right now
                    bkoff();
                    m_Stat.onSlotConverting();
                }
                else {
                    // data node
                    assert(slot.bits() == 0 );

                    // protect data node by hazard pointer
                    if ( guard.protect( pArr[nSlot], [](node_ptr p) -> value_type * { return p.ptr(); }) != slot ) {
                        // slot value has been changed - retry
                        m_Stat.onSlotChanged();
                    }
                    else if ( slot.ptr() ) {
                        if ( cmp( hash, hash_accessor()( *slot.ptr() )) == 0 ) {
                            // the item with that hash value already exists
                            m_Stat.onInsertFailed();
                            return false;
                        }

                        // the slot must be expanded
                        expand_slot( pArr[ nSlot ], slot, nOffset );
                    }
                    else {
                        // the slot is empty, try to insert data node
                        node_ptr pNull;
                        if ( pArr[nSlot].compare_exchange_strong( pNull, node_ptr( &val ), memory_model::memory_order_release, atomics::memory_order_relaxed ))
                        {
                            // the new data node has been inserted
                            f( val );
                            ++m_ItemCounter;
                            m_Stat.onInsertSuccess();
                            return true;
                        }

                        // insert failed - slot has been changed by another thread
                        // retry inserting
                        m_Stat.onInsertRetry();
                    }
                }
            } // while
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
        std::pair<bool, bool> update( value_type& val, bool bInsert = true )
        {
            hash_type const& hash = hash_accessor()( val );
            hash_splitter splitter( hash );
            hash_comparator cmp;
            typename gc::Guard guard;
            back_off bkoff;

            atomic_node_ptr * pArr = m_Head;
            size_t nSlot = splitter.cut( m_Metrics.head_node_size_log );
            assert( nSlot < m_Metrics.head_node_size );
            size_t nOffset = m_Metrics.head_node_size_log;

            while ( true ) {
                node_ptr slot = pArr[nSlot].load( memory_model::memory_order_acquire );
                if ( slot.bits() == array_node ) {
                    // array node, go down the tree
                    assert( slot.ptr() != nullptr );
                    nSlot = splitter.cut( m_Metrics.array_node_size_log );
                    assert( nSlot < m_Metrics.array_node_size );
                    pArr = to_array( slot.ptr() );
                    nOffset += m_Metrics.array_node_size_log;
                }
                else if ( slot.bits() == array_converting ) {
                    // the slot is converting to array node right now
                    bkoff();
                    m_Stat.onSlotConverting();
                }
                else {
                    // data node
                    assert(slot.bits() == 0 );

                    // protect data node by hazard pointer
                    if ( guard.protect( pArr[nSlot], [](node_ptr p) -> value_type * { return p.ptr(); }) != slot ) {
                        // slot value has been changed - retry
                        m_Stat.onSlotChanged();
                    }
                    else if ( slot.ptr() ) {
                        if ( cmp( hash, hash_accessor()( *slot.ptr() )) == 0 ) {
                            // the item with that hash value already exists
                            // Replace it with val
                            if ( slot.ptr() == &val ) {
                                m_Stat.onUpdateExisting();
                                return std::make_pair( true, false );
                            }

                            if ( pArr[nSlot].compare_exchange_strong( slot, node_ptr( &val ), memory_model::memory_order_release, atomics::memory_order_relaxed )) {
                                // slot can be disposed
                                gc::template retire<disposer>( slot.ptr() );
                                m_Stat.onUpdateExisting();
                                return std::make_pair( true, false );
                            }

                            m_Stat.onUpdateRetry();
                            continue;
                        }

                        // the slot must be expanded
                        expand_slot( pArr[ nSlot ], slot, nOffset );
                    }
                    else {
                        // the slot is empty, try to insert data node
                        if ( bInsert ) {
                            node_ptr pNull;
                            if ( pArr[nSlot].compare_exchange_strong( pNull, node_ptr( &val ), memory_model::memory_order_release, atomics::memory_order_relaxed ))
                            {
                                // the new data node has been inserted
                                ++m_ItemCounter;
                                m_Stat.onUpdateNew();
                                return std::make_pair( true, true );
                            }
                        }
                        else {
                            m_Stat.onUpdateFailed();
                            return std::make_pair( false, false );
                        }

                        // insert failed - slot has been changed by another thread
                        // retry updating
                        m_Stat.onUpdateRetry();
                    }
                }
            } // while
        }

        /// Unlinks the item \p val from the set
        /**
            The function searches the item \p val in the set and unlink it
            if it is found and its address is equal to <tt>&val</tt>.

            The function returns \p true if success and \p false otherwise.
        */
        bool unlink( value_type const& val )
        {
            typename gc::Guard guard;
            auto pred = [&val](value_type const& item) -> bool { return &item == &val; };
            value_type * p = do_erase( hash_accessor()( val ), guard, std::ref( pred ));

            // p is guarded by HP
            if ( p ) {
                gc::template retire<disposer>( p );
                --m_ItemCounter;
                m_Stat.onEraseSuccess();
                return true;
            }
            return false;
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
            return erase(hash, [](value_type const&) {} );
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
                void operator()( value_type& item );
            };
            \endcode

            If \p hash is not found the function returns \p false.
        */
        template <typename Func>
        bool erase( hash_type const& hash, Func f )
        {
            typename gc::Guard guard;
            value_type * p = do_erase( hash, guard, []( value_type const&) -> bool {return true; } );

            // p is guarded by HP
            if ( p ) {
                gc::template retire<disposer>( p );
                --m_ItemCounter;
                f( *p );
                m_Stat.onEraseSuccess();
                return true;
            }
            return false;
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
            guarded_ptr gp;
            {
                typename gc::Guard guard;
                value_type * p = do_erase( hash, guard, []( value_type const&) -> bool {return true; } );

                // p is guarded by HP
                if ( p ) {
                    gc::template retire<disposer>( p );
                    --m_ItemCounter;
                    m_Stat.onEraseSuccess();
                    gp.reset( p );
                }
            }
            return gp;
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
            typename gc::Guard guard;
            value_type * p = search( hash, guard );

            // p is guarded by HP
            if ( p ) {
                f( *p );
                return true;
            }
            return false;
        }

        /// Checks whether the set contains \p hash
        /**
            The function searches the item by its \p hash
            and returns \p true if it is found, or \p false otherwise.
        */
        bool contains( hash_type const& hash )
        {
            return find( hash, [](value_type&) {} );
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
            guarded_ptr gp;
            {
                typename gc::Guard guard;
                gp.reset( search( hash, guard ));
            }
            return gp;
        }

        /// Clears the set (non-atomic)
        /**
            The function unlink all data node from the set.
            The function is not atomic but is thread-safe.
            After \p %clear() the set may not be empty because another threads may insert items.

            For each item the \p disposer is called after unlinking.
        */
        void clear()
        {
            clear_array( m_Head, head_size() );
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
            if ( head_bits < 4 )
                head_bits = 4;
            if ( head_bits > hash_bits )
                head_bits = hash_bits;
            if ( (hash_bits - head_bits) % array_bits != 0 )
                head_bits += (hash_bits - head_bits) % array_bits;

            assert( (hash_bits - head_bits) % array_bits == 0 );

            metrics m;
            m.head_node_size_log = head_bits;
            m.head_node_size = size_t(1) << head_bits;
            m.array_node_size_log = array_bits;
            m.array_node_size = size_t(1) << array_bits;
            return m;
        }

        template <typename Q>
        static bool check_node_alignment( Q const* p )
        {
            return (reinterpret_cast<uintptr_t>(p) & node_ptr::bitmask) == 0;
        }

        atomic_node_ptr * alloc_array_node() const
        {
            return cxx_array_node_allocator().NewArray( array_node_size(), nullptr );
        }

        void free_array_node( atomic_node_ptr * parr ) const
        {
            cxx_array_node_allocator().Delete( parr, array_node_size() );
        }

        void destroy_tree()
        {
            // The function is not thread-safe. For use in dtor only
            // Remove data node
            clear();

            // Destroy all array nodes
            destroy_array_nodes( m_Head, head_size());
        }

        void destroy_array_nodes( atomic_node_ptr * pArr, size_t nSize )
        {
            for ( atomic_node_ptr * pLast = pArr + nSize; pArr != pLast; ++pArr ) {
                node_ptr slot = pArr->load( memory_model::memory_order_acquire );
                if ( slot.bits() == array_node ) {
                    destroy_array_nodes(to_array(slot.ptr()), array_node_size());
                    free_array_node( to_array(slot.ptr()));
                    pArr->store(node_ptr(), memory_model::memory_order_relaxed );
                }
            }
        }

        void clear_array( atomic_node_ptr * pArr, size_t nSize )
        {
            back_off bkoff;

            for ( atomic_node_ptr * pLast = pArr + nSize; pArr != pLast; ++pArr ) {
                while ( true ) {
                    node_ptr slot = pArr->load( memory_model::memory_order_acquire );
                    if ( slot.bits() == array_node ) {
                        // array node, go down the tree
                        assert( slot.ptr() != nullptr );
                        clear_array( to_array( slot.ptr()), array_node_size() );
                        break;
                    }
                    else if ( slot.bits() == array_converting ) {
                        // the slot is converting to array node right now
                        while ( (slot = pArr->load(memory_model::memory_order_acquire)).bits() == array_converting ) {
                            bkoff();
                            m_Stat.onSlotConverting();
                        }
                        bkoff.reset();

                        assert( slot.ptr() != nullptr );
                        assert(slot.bits() == array_node );
                        clear_array( to_array( slot.ptr()), array_node_size() );
                        break;
                    }
                    else {
                        // data node
                        if ( pArr->compare_exchange_strong( slot, node_ptr(), memory_model::memory_order_acquire, atomics::memory_order_relaxed )) {
                            if ( slot.ptr() ) {
                                gc::template retire<disposer>( slot.ptr() );
                                --m_ItemCounter;
                                m_Stat.onEraseSuccess();
                            }
                            break;
                        }
                    }
                }
            }
        }

        union converter {
            value_type * pData;
            atomic_node_ptr * pArr;

            converter( value_type * p )
                : pData( p )
            {}

            converter( atomic_node_ptr * p )
                : pArr( p )
            {}
        };

        static atomic_node_ptr * to_array( value_type * p )
        {
            return converter( p ).pArr;
        }
        static value_type * to_node( atomic_node_ptr * p )
        {
            return converter( p ).pData;
        }

        bool expand_slot( atomic_node_ptr& slot, node_ptr current, size_t nOffset )
        {
            assert( current.bits() == 0 );
            assert( current.ptr() );

            size_t idx = hash_splitter(hash_accessor()(*current.ptr()), nOffset).cut( m_Metrics.array_node_size_log );
            atomic_node_ptr * pArr = alloc_array_node();

            node_ptr cur(current.ptr());
            if ( !slot.compare_exchange_strong( cur, cur | array_converting, memory_model::memory_order_release, atomics::memory_order_relaxed )) {
                m_Stat.onExpandNodeFailed();
                free_array_node( pArr );
                return false;
            }

            pArr[idx].store( current, memory_model::memory_order_release );

            cur = cur | array_converting;
            CDS_VERIFY( 
                slot.compare_exchange_strong( cur, node_ptr( to_node( pArr ), array_node ), memory_model::memory_order_release, atomics::memory_order_relaxed )
            );

            m_Stat.onArrayNodeCreated();
            return true;
        }

        value_type * search( hash_type const& hash, typename gc::Guard& guard )
        {
            hash_splitter splitter( hash );
            hash_comparator cmp;
            back_off bkoff;

            atomic_node_ptr * pArr = m_Head;
            size_t nSlot = splitter.cut( m_Metrics.head_node_size_log );
            assert( nSlot < m_Metrics.head_node_size );

            while ( true ) {
                node_ptr slot = pArr[nSlot].load( memory_model::memory_order_acquire );
                if ( slot.bits() == array_node ) {
                    // array node, go down the tree
                    assert( slot.ptr() != nullptr );
                    nSlot = splitter.cut( m_Metrics.array_node_size_log );
                    assert( nSlot < m_Metrics.array_node_size );
                    pArr = to_array( slot.ptr() );
                }
                else if ( slot.bits() == array_converting ) {
                    // the slot is converting to array node right now
                    bkoff();
                    m_Stat.onSlotConverting();
                }
                else {
                    // data node
                    assert(slot.bits() == 0 );

                    // protect data node by hazard pointer
                    if ( guard.protect( pArr[nSlot], [](node_ptr p) -> value_type * { return p.ptr(); }) != slot ) {
                        // slot value has been changed - retry
                        m_Stat.onSlotChanged();
                    }
                    else if ( slot.ptr() && cmp( hash, hash_accessor()( *slot.ptr() )) == 0 ) {
                        // item found
                        m_Stat.onFindSuccess();
                        return slot.ptr();
                    }
                    m_Stat.onFindFailed();
                    return nullptr;
                }
            } // while
        }

        template <typename Predicate>
        value_type * do_erase( hash_type const& hash, typename gc::Guard& guard, Predicate pred )
        {
            hash_splitter splitter( hash );
            hash_comparator cmp;
            back_off bkoff;

            atomic_node_ptr * pArr = m_Head;
            size_t nSlot = splitter.cut( m_Metrics.head_node_size_log );
            assert( nSlot < m_Metrics.head_node_size );

            while ( true ) {
                node_ptr slot = pArr[nSlot].load( memory_model::memory_order_acquire );
                if ( slot.bits() == array_node ) {
                    // array node, go down the tree
                    assert( slot.ptr() != nullptr );
                    nSlot = splitter.cut( m_Metrics.array_node_size_log );
                    assert( nSlot < m_Metrics.array_node_size );
                    pArr = to_array( slot.ptr() );
                }
                else if ( slot.bits() == array_converting ) {
                    // the slot is converting to array node right now
                    bkoff();
                    m_Stat.onSlotConverting();
                }
                else {
                    // data node
                    assert(slot.bits() == 0 );

                    // protect data node by hazard pointer
                    if ( guard.protect( pArr[nSlot], [](node_ptr p) -> value_type * { return p.ptr(); }) != slot ) {
                        // slot value has been changed - retry
                        m_Stat.onSlotChanged();
                    }
                    else if ( slot.ptr() ) {
                        if ( cmp( hash, hash_accessor()( *slot.ptr() )) == 0 && pred( *slot.ptr() )) {
                            // item found - replace it with nullptr
                            if ( pArr[nSlot].compare_exchange_strong(slot, node_ptr(nullptr), memory_model::memory_order_acquire, atomics::memory_order_relaxed))
                                return slot.ptr();
                            m_Stat.onEraseRetry();
                            continue;
                        }
                        m_Stat.onEraseFailed();
                        return nullptr;
                    }
                    else {
                        // the slot is empty
                        m_Stat.onEraseFailed();
                        return nullptr;
                    }
                }
            } // while
        }

        //@endcond
    };
}} // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_IMPL_MULTILEVEL_HASHSET_H
