//$$CDS-header$$

#ifndef CDSLIB_INTRUSIVE_IMPL_MULTILEVEL_HASHSET_H
#define CDSLIB_INTRUSIVE_IMPL_MULTILEVEL_HASHSET_H

#include <functional>   // std::ref
#include <iterator>     // std::iterator_traits

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

        \p %MultiLevelHashSet is a multi-level array which has a structure similar to a tree:
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

        That approach to the structure of the hash set uses an extensible hashing scheme; <b> the hash value is treated as a bit
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

        The set supports @ref cds_intrusive_MultilevelHashSet_iterators "bidirectional thread-safe iterators".

        Template parameters:
        - \p GC - safe memory reclamation schema. Can be \p gc::HP, \p gc::DHP or one of \ref cds_urcu_type "RCU type"
        - \p T - a value type to be stored in the set
        - \p Traits - type traits, the structure based on \p multilevel_hashset::traits or result of \p multilevel_hashset::make_traits metafunction.
            \p Traits is the mandatory argument because it has one mandatory type - an @ref multilevel_hashset::traits::hash_accessor "accessor" 
            to hash value of \p T. The set algorithm does not calculate that hash value.

        There are several specializations of \p %MultiLevelHashSet for each \p GC. You should include:
        - <tt><cds/intrusive/multilevel_hashset_hp.h></tt> for \p gc::HP garbage collector
        - <tt><cds/intrusive/multilevel_hashset_dhp.h></tt> for \p gc::DHP garbage collector
        - <tt><cds/intrusive/multilevel_hashset_rcu.h></tt> for \ref cds_intrusive_MultiLevelHashSet_rcu "RCU type". RCU specialization
            has a slightly different interface.
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

        /// Hash type deduced from \p hash_accessor return type
        typedef typename std::decay< 
            typename std::remove_reference<
                decltype( hash_accessor()( std::declval<T>()) )
            >::type
        >::type hash_type;
        //typedef typename std::result_of< hash_accessor( std::declval<T>()) >::type hash_type;
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

        typedef typename traits::item_counter   item_counter;   ///< Item counter type
        typedef typename traits::node_allocator node_allocator; ///< Array node allocator
        typedef typename traits::memory_model   memory_model;   ///< Memory model
        typedef typename traits::back_off       back_off;       ///< Backoff strategy
        typedef typename traits::stat           stat;           ///< Internal statistics type

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
            flag_array_converting = 1,   ///< the cell is converting from data node to an array node
            flag_array_node = 2          ///< the cell is a pointer to an array node
        };

        struct array_node {
            array_node * const  pParent;    ///< parent array node
            size_t const        idxParent;  ///< index in parent array node
            atomic_node_ptr     nodes[1];   ///< node array

            array_node( array_node * parent, size_t idx )
                : pParent( parent )
                , idxParent( idx )
            {}

            array_node() = delete;
            array_node( array_node const&) = delete;
            array_node( array_node&& ) = delete;
        };

        typedef cds::details::Allocator< array_node, node_allocator > cxx_array_node_allocator;

        typedef multilevel_hashset::details::hash_splitter< hash_type > hash_splitter;

        struct metrics {
            size_t  head_node_size;     // power-of-two
            size_t  head_node_size_log; // log2( head_node_size )
            size_t  array_node_size;    // power-of-two
            size_t  array_node_size_log;// log2( array_node_size )
        };
        //@endcond

    protected:
        //@cond
        /// Bidirectional iterator class
        template <bool IsConst>
        class bidirectional_iterator
        {
            friend class MultiLevelHashSet;

            array_node *        m_pNode;    ///< current array node
            size_t              m_idx;      ///< current position in m_pNode
            typename gc::Guard  m_guard;    ///< HP guard
            MultiLevelHashSet const*  m_set;    ///< Hash set

        public:
            typedef typename std::conditional< IsConst, value_type const*, value_type*>::type value_ptr; ///< Value pointer
            typedef typename std::conditional< IsConst, value_type const&, value_type&>::type value_ref; ///< Value reference

        public:
            bidirectional_iterator() CDS_NOEXCEPT
                : m_pNode( nullptr )
                , m_idx( 0 )
                , m_set( nullptr )
            {}

            bidirectional_iterator( bidirectional_iterator const& rhs ) CDS_NOEXCEPT
                : m_pNode( rhs.m_pNode )
                , m_idx( rhs.m_idx )
                , m_set( rhs.m_set )
            {
                m_guard.copy( rhs.m_guard );
            }

            bidirectional_iterator& operator=(bidirectional_iterator const& rhs) CDS_NOEXCEPT
            {
                m_pNode = rhs.m_pNode;
                m_idx = rhs.m_idx;
                m_set = rhs.m_set;
                m_guard.copy( rhs.m_guard );
                return *this;
            }

            bidirectional_iterator& operator++()
            {
                forward();
                return *this;
            }

            bidirectional_iterator& operator--()
            {
                backward();
                return *this;
            }

            value_ptr operator ->() const CDS_NOEXCEPT
            {
                return pointer();
            }

            value_ref operator *() const CDS_NOEXCEPT
            {
                value_ptr p = pointer();
                assert( p );
                return *p;
            }

            void release()
            {
                m_guard.clear();
            }

            template <bool IsConst2>
            bool operator ==(bidirectional_iterator<IsConst2> const& rhs) const
            {
                return m_pNode == rhs.m_pNode && m_idx == rhs.m_idx && m_set == rhs.m_set;
            }

            template <bool IsConst2>
            bool operator !=(bidirectional_iterator<IsConst2> const& rhs) const
            {
                return !( *this == rhs );
            }

        protected:
            bidirectional_iterator( MultiLevelHashSet& set, array_node * pNode, size_t idx, bool )
                : m_pNode( pNode )
                , m_idx( idx )
                , m_set( &set )
            {}

            bidirectional_iterator( MultiLevelHashSet& set, array_node * pNode, size_t idx )
                : m_pNode( pNode )
                , m_idx( idx )
                , m_set( &set )
            {
                forward();
            }

            value_ptr pointer() const CDS_NOEXCEPT
            {
                return m_guard.template get<value_type>();
            }

            void forward()
            {
                assert( m_set != nullptr );
                assert( m_pNode != nullptr );

                size_t const arrayNodeSize = m_set->array_node_size();
                size_t const headSize = m_set->head_size();
                array_node * pNode = m_pNode;
                size_t idx = m_idx + 1;
                size_t nodeSize = m_pNode->pParent? arrayNodeSize : headSize;

                for ( ;; ) {
                    if ( idx < nodeSize ) {
                        node_ptr slot = pNode->nodes[idx].load( memory_model::memory_order_acquire );
                        if ( slot.bits() == flag_array_node ) {
                            // array node, go down the tree
                            assert( slot.ptr() != nullptr );
                            pNode = to_array( slot.ptr() );
                            idx = 0;
                            nodeSize = arrayNodeSize;
                        }
                        else if ( slot.bits() == flag_array_converting ) {
                            // the slot is converting to array node right now - skip the node
                            ++idx;
                        }
                        else {
                            if ( slot.ptr()) {
                                // data node
                                if ( m_guard.protect( pNode->nodes[idx], [](node_ptr p) -> value_type * { return p.ptr(); }) == slot ) {
                                    m_pNode = pNode;
                                    m_idx = idx;
                                    return;
                                }
                            }
                            ++idx;
                        }
                    }
                    else {
                        // up to parent node
                        if ( pNode->pParent ) {
                            idx = pNode->idxParent + 1;
                            pNode = pNode->pParent;
                            nodeSize = pNode->pParent ? arrayNodeSize : headSize;
                        }
                        else {
                            // end()
                            assert( pNode == m_set->m_Head );
                            assert( idx == headSize );
                            m_pNode = pNode;
                            m_idx = idx;
                            return;
                        }
                    }
                }
            }

            void backward()
            {
                assert( m_set != nullptr );
                assert( m_pNode != nullptr );

                size_t const arrayNodeSize = m_set->array_node_size();
                size_t const headSize = m_set->head_size();
                size_t const endIdx = size_t(0) - 1;

                array_node * pNode = m_pNode;
                size_t idx = m_idx - 1;
                size_t nodeSize = m_pNode->pParent? arrayNodeSize : headSize;

                for ( ;; ) {
                    if ( idx != endIdx ) {
                        node_ptr slot = pNode->nodes[idx].load( memory_model::memory_order_acquire );
                        if ( slot.bits() == flag_array_node ) {
                            // array node, go down the tree
                            assert( slot.ptr() != nullptr );
                            pNode = to_array( slot.ptr() );
                            nodeSize = arrayNodeSize;
                            idx = nodeSize - 1;
                        }
                        else if ( slot.bits() == flag_array_converting ) {
                            // the slot is converting to array node right now - skip the node
                            --idx;
                        }
                        else {
                            if ( slot.ptr()) {
                                // data node
                                if ( m_guard.protect( pNode->nodes[idx], [](node_ptr p) -> value_type * { return p.ptr(); }) == slot ) {
                                    m_pNode = pNode;
                                    m_idx = idx;
                                    return;
                                }
                            }
                            --idx;
                        }
                    }
                    else {
                        // up to parent node
                        if ( pNode->pParent ) {
                            idx = pNode->idxParent - 1;
                            pNode = pNode->pParent;
                            nodeSize = pNode->pParent ? arrayNodeSize : headSize;
                        }
                        else {
                            // rend()
                            assert( pNode == m_set->m_Head );
                            assert( idx == endIdx );
                            m_pNode = pNode;
                            m_idx = idx;
                            return;
                        }
                    }
                }
            }
        };

        /// Reverse bidirectional iterator
        template <bool IsConst>
        class reverse_bidirectional_iterator : protected bidirectional_iterator<IsConst>
        {
            typedef bidirectional_iterator<IsConst> base_class;
            friend class MultiLevelHashSet;

        public:
            typedef typename base_class::value_ptr value_ptr;
            typedef typename base_class::value_ref value_ref;

        public:
            reverse_bidirectional_iterator() CDS_NOEXCEPT
                : base_class()
            {}

            reverse_bidirectional_iterator( reverse_bidirectional_iterator const& rhs ) CDS_NOEXCEPT
                : base_class( rhs )
            {}

            reverse_bidirectional_iterator& operator=( reverse_bidirectional_iterator const& rhs) CDS_NOEXCEPT
            {
                base_class::operator=( rhs );
                return *this;
            }

            reverse_bidirectional_iterator& operator++()
            {
                base_class::backward();
                return *this;
            }

            reverse_bidirectional_iterator& operator--()
            {
                base_class::forward();
                return *this;
            }

            value_ptr operator ->() const CDS_NOEXCEPT
            {
                return base_class::operator->();
            }

            value_ref operator *() const CDS_NOEXCEPT
            {
                return base_class::operator*();
            }

            void release()
            {
                base_class::release();
            }

            template <bool IsConst2>
            bool operator ==(reverse_bidirectional_iterator<IsConst2> const& rhs) const
            {
                return base_class::operator==( rhs );
            }

            template <bool IsConst2>
            bool operator !=(reverse_bidirectional_iterator<IsConst2> const& rhs)
            {
                return !( *this == rhs );
            }

        private:
            reverse_bidirectional_iterator( MultiLevelHashSet& set, array_node * pNode, size_t idx )
                : base_class( set, pNode, idx, false )
            {
                base_class::backward();
            }
        };
        //@endcond

    public:
        typedef bidirectional_iterator<false>   iterator;       ///< @ref cds_intrusive_MultilevelHashSet_iterators "bidirectional iterator" type
        typedef bidirectional_iterator<true>    const_iterator; ///< @ref cds_intrusive_MultilevelHashSet_iterators "bidirectional const iterator" type
        typedef reverse_bidirectional_iterator<false>   reverse_iterator;       ///< @ref cds_intrusive_MultilevelHashSet_iterators "bidirectional reverse iterator" type
        typedef reverse_bidirectional_iterator<true>    const_reverse_iterator; ///< @ref cds_intrusive_MultilevelHashSet_iterators "bidirectional reverse const iterator" type

    private:
        //@cond
        metrics const     m_Metrics;     ///< Metrics
        array_node *      m_Head;        ///< Head array
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
            , m_Head( alloc_head_node())
        {}

        /// Destructs the set and frees all data
        ~MultiLevelHashSet()
        {
            destroy_tree();
            free_array_node( m_Head );
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
            array_node * pArr = m_Head;
            size_t nSlot = splitter.cut( m_Metrics.head_node_size_log );
            assert( nSlot < m_Metrics.head_node_size );

            while ( true ) {
                node_ptr slot = pArr->nodes[nSlot].load( memory_model::memory_order_acquire );
                if ( slot.bits() == flag_array_node ) {
                    // array node, go down the tree
                    assert( slot.ptr() != nullptr );
                    nSlot = splitter.cut( m_Metrics.array_node_size_log );
                    assert( nSlot < m_Metrics.array_node_size );
                    pArr = to_array( slot.ptr() );
                    nOffset += m_Metrics.array_node_size_log;
                }
                else if ( slot.bits() == flag_array_converting ) {
                    // the slot is converting to array node right now
                    bkoff();
                    m_Stat.onSlotConverting();
                }
                else {
                    // data node
                    assert(slot.bits() == 0 );

                    // protect data node by hazard pointer
                    if ( guard.protect( pArr->nodes[nSlot], [](node_ptr p) -> value_type * { return p.ptr(); }) != slot ) {
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
                        expand_slot( pArr, nSlot, slot, nOffset );
                    }
                    else {
                        // the slot is empty, try to insert data node
                        node_ptr pNull;
                        if ( pArr->nodes[nSlot].compare_exchange_strong( pNull, node_ptr( &val ), memory_model::memory_order_release, atomics::memory_order_relaxed ))
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
            return do_update(val, [](bool, value_type&) {}, bInsert );
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

        /// Deletes the item pointed by iterator \p iter
        /**
            Returns \p true if the operation is successful, \p false otherwise.

            The function does not invalidate the iterator, it remains valid and can be used for further traversing.
        */
        bool erase_at( iterator const& iter )
        {
            if ( iter.m_set != this )
                return false;
            if ( iter.m_pNode == m_Head && iter.m_idx >= head_size())
                return false;
            if ( iter.m_idx >= array_node_size() )
                return false;

            for (;;) {
                node_ptr slot = iter.m_pNode->nodes[iter.m_idx].load( memory_model::memory_order_acquire );
                if ( slot.bits() == 0 && slot.ptr() == iter.pointer() ) {
                    if ( iter.m_pNode->nodes[iter.m_idx].compare_exchange_strong(slot, node_ptr(nullptr), memory_model::memory_order_acquire, atomics::memory_order_relaxed) ) {
                        // the item is guarded by iterator, so we may retire it safely
                        gc::template retire<disposer>( slot.ptr() );
                        --m_ItemCounter;
                        m_Stat.onEraseSuccess();
                        return true;
                    }
                }
                else
                    return false;
            }
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

    public:
    ///@name Thread-safe iterators
        /** @anchor cds_intrusive_MultilevelHashSet_iterators
            The set supports thread-safe iterators: you may iterate over the set in multi-threaded environment.
            It is guaranteed that the iterators will remain valid even if another thread deletes the node the iterator points to:
            Hazard Pointer embedded into the iterator object protects the node from physical reclamation.

            @note Since the iterator object contains hazard pointer that is a thread-local resource,
            the iterator should not be passed to another thread.

            Each iterator object supports the common interface:
            - dereference operators:
                @code
                value_type [const] * operator ->() noexcept
                value_type [const] & operator *() noexcept
                @endcode
            - pre-increment and pre-decrement. Post-operators is not supported
            - equality operators <tt>==</tt> and <tt>!=</tt>.
                Iterators are equal iff they point to the same cell of the same array node.
                Note that for two iterators \p it1 and \p it2, the conditon <tt> it1 == it2 </tt> 
                does not entail <tt> &(*it1) == &(*it2) </tt>: welcome to concurrent containers
            - helper member function \p release() that clears internal hazard pointer.
                After \p release() call the iterator points to \p nullptr but it still remain valid: further iterating is possible.
        */
    ///@{

        /// Returns an iterator to the beginning of the set
        iterator begin()
        {
            return iterator( *this, m_Head, size_t(0) - 1 );
        }

        /// Returns an const iterator to the beginning of the set
        const_iterator begin() const
        {
            return const_iterator( *this, m_Head, size_t(0) - 1 );
        }

        /// Returns an const iterator to the beginning of the set
        const_iterator cbegin()
        {
            return const_iterator( *this, m_Head, size_t(0) - 1 );
        }

        /// Returns an iterator to the element following the last element of the set. This element acts as a placeholder; attempting to access it results in undefined behavior. 
        iterator end()
        {
            return iterator( *this, m_Head, head_size() - 1 );
        }

        /// Returns a const iterator to the element following the last element of the set. This element acts as a placeholder; attempting to access it results in undefined behavior. 
        const_iterator end() const
        {
            return const_iterator( *this, m_Head, head_size() - 1 );
        }

        /// Returns a const iterator to the element following the last element of the set. This element acts as a placeholder; attempting to access it results in undefined behavior. 
        const_iterator cend()
        {
            return const_iterator( *this, m_Head, head_size() - 1 );
        }

        /// Returns a reverse iterator to the first element of the reversed set
        reverse_iterator rbegin()
        {
            return reverse_iterator( *this, m_Head, head_size() );
        }

        /// Returns a const reverse iterator to the first element of the reversed set
        const_reverse_iterator rbegin() const
        {
            return const_reverse_iterator( *this, m_Head, head_size() );
        }

        /// Returns a const reverse iterator to the first element of the reversed set
        const_reverse_iterator crbegin()
        {
            return const_reverse_iterator( *this, m_Head, head_size() );
        }

        /// Returns a reverse iterator to the element following the last element of the reversed set
        /**
            It corresponds to the element preceding the first element of the non-reversed container. 
            This element acts as a placeholder, attempting to access it results in undefined behavior. 
        */
        reverse_iterator rend()
        {
            return reverse_iterator( *this, m_Head, 0 );
        }

        /// Returns a const reverse iterator to the element following the last element of the reversed set
        /**
            It corresponds to the element preceding the first element of the non-reversed container. 
            This element acts as a placeholder, attempting to access it results in undefined behavior. 
        */
        const_reverse_iterator rend() const
        {
            return const_reverse_iterator( *this, m_Head, 0 );
        }

        /// Returns a const reverse iterator to the element following the last element of the reversed set
        /**
            It corresponds to the element preceding the first element of the non-reversed container. 
            This element acts as a placeholder, attempting to access it results in undefined behavior. 
        */
        const_reverse_iterator crend()
        {
            return const_reverse_iterator( *this, m_Head, 0 );
        }
    ///@}

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

        array_node * alloc_head_node() const
        {
            return alloc_array_node( head_size(), nullptr, 0 );
        }

        array_node * alloc_array_node( array_node * pParent, size_t idxParent ) const
        {
            return alloc_array_node( array_node_size(), pParent, idxParent );
        }

        static array_node * alloc_array_node( size_t nSize, array_node * pParent, size_t idxParent )
        {
            array_node * pNode = cxx_array_node_allocator().NewBlock( sizeof(array_node) + sizeof(atomic_node_ptr) * (nSize - 1), pParent, idxParent );
            for ( atomic_node_ptr * p = pNode->nodes, *pEnd = pNode->nodes + nSize; p < pEnd; ++p )
                p->store( node_ptr(), memory_model::memory_order_release );
            return pNode;
        }

        static void free_array_node( array_node * parr )
        {
            cxx_array_node_allocator().Delete( parr );
        }

        void destroy_tree()
        {
            // The function is not thread-safe. For use in dtor only
            // Remove data node
            clear();

            // Destroy all array nodes
            destroy_array_nodes( m_Head, head_size());
        }

        void destroy_array_nodes( array_node * pArr, size_t nSize )
        {
            for ( atomic_node_ptr * p = pArr->nodes, *pLast = pArr->nodes + nSize; p != pLast; ++p ) {
                node_ptr slot = p->load( memory_model::memory_order_acquire );
                if ( slot.bits() == flag_array_node ) {
                    destroy_array_nodes(to_array(slot.ptr()), array_node_size());
                    free_array_node( to_array(slot.ptr()));
                    p->store(node_ptr(), memory_model::memory_order_relaxed );
                }
            }
        }

        void clear_array( array_node * pArrNode, size_t nSize )
        {
            back_off bkoff;


            for ( atomic_node_ptr * pArr = pArrNode->nodes, *pLast = pArr + nSize; pArr != pLast; ++pArr ) {
                while ( true ) {
                    node_ptr slot = pArr->load( memory_model::memory_order_acquire );
                    if ( slot.bits() == flag_array_node ) {
                        // array node, go down the tree
                        assert( slot.ptr() != nullptr );
                        clear_array( to_array( slot.ptr()), array_node_size() );
                        break;
                    }
                    else if ( slot.bits() == flag_array_converting ) {
                        // the slot is converting to array node right now
                        while ( (slot = pArr->load(memory_model::memory_order_acquire)).bits() == flag_array_converting ) {
                            bkoff();
                            m_Stat.onSlotConverting();
                        }
                        bkoff.reset();

                        assert( slot.ptr() != nullptr );
                        assert(slot.bits() == flag_array_node );
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
            array_node * pArr;

            converter( value_type * p )
                : pData( p )
            {}

            converter( array_node * p )
                : pArr( p )
            {}
        };

        static array_node * to_array( value_type * p )
        {
            return converter( p ).pArr;
        }
        static value_type * to_node( array_node * p )
        {
            return converter( p ).pData;
        }

        bool expand_slot( array_node * pParent, size_t idxParent, node_ptr current, size_t nOffset )
        {
            assert( current.bits() == 0 );
            assert( current.ptr() );

            size_t idx = hash_splitter(hash_accessor()(*current.ptr()), nOffset).cut( m_Metrics.array_node_size_log );
            array_node * pArr = alloc_array_node( pParent, idxParent );

            node_ptr cur(current.ptr());
            atomic_node_ptr& slot = pParent->nodes[idxParent];
            if ( !slot.compare_exchange_strong( cur, cur | flag_array_converting, memory_model::memory_order_release, atomics::memory_order_relaxed )) 
            {
                m_Stat.onExpandNodeFailed();
                free_array_node( pArr );
                return false;
            }

            pArr->nodes[idx].store( current, memory_model::memory_order_release );

            cur = cur | flag_array_converting;
            CDS_VERIFY( 
                slot.compare_exchange_strong( cur, node_ptr( to_node( pArr ), flag_array_node ), memory_model::memory_order_release, atomics::memory_order_relaxed )
            );

            m_Stat.onArrayNodeCreated();
            return true;
        }
        //@endcond

    protected:
        //@cond
        value_type * search( hash_type const& hash, typename gc::Guard& guard )
        {
            hash_splitter splitter( hash );
            hash_comparator cmp;
            back_off bkoff;

            array_node * pArr = m_Head;
            size_t nSlot = splitter.cut( m_Metrics.head_node_size_log );
            assert( nSlot < m_Metrics.head_node_size );

            while ( true ) {
                node_ptr slot = pArr->nodes[nSlot].load( memory_model::memory_order_acquire );
                if ( slot.bits() == flag_array_node ) {
                    // array node, go down the tree
                    assert( slot.ptr() != nullptr );
                    nSlot = splitter.cut( m_Metrics.array_node_size_log );
                    assert( nSlot < m_Metrics.array_node_size );
                    pArr = to_array( slot.ptr() );
                }
                else if ( slot.bits() == flag_array_converting ) {
                    // the slot is converting to array node right now
                    bkoff();
                    m_Stat.onSlotConverting();
                }
                else {
                    // data node
                    assert(slot.bits() == 0 );

                    // protect data node by hazard pointer
                    if ( guard.protect( pArr->nodes[nSlot], [](node_ptr p) -> value_type * { return p.ptr(); }) != slot ) {
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

            array_node * pArr = m_Head;
            size_t nSlot = splitter.cut( m_Metrics.head_node_size_log );
            assert( nSlot < m_Metrics.head_node_size );

            while ( true ) {
                node_ptr slot = pArr->nodes[nSlot].load( memory_model::memory_order_acquire );
                if ( slot.bits() == flag_array_node ) {
                    // array node, go down the tree
                    assert( slot.ptr() != nullptr );
                    nSlot = splitter.cut( m_Metrics.array_node_size_log );
                    assert( nSlot < m_Metrics.array_node_size );
                    pArr = to_array( slot.ptr() );
                }
                else if ( slot.bits() == flag_array_converting ) {
                    // the slot is converting to array node right now
                    bkoff();
                    m_Stat.onSlotConverting();
                }
                else {
                    // data node
                    assert(slot.bits() == 0 );

                    // protect data node by hazard pointer
                    if ( guard.protect( pArr->nodes[nSlot], [](node_ptr p) -> value_type * { return p.ptr(); }) != slot ) {
                        // slot value has been changed - retry
                        m_Stat.onSlotChanged();
                    }
                    else if ( slot.ptr() ) {
                        if ( cmp( hash, hash_accessor()( *slot.ptr() )) == 0 && pred( *slot.ptr() )) {
                            // item found - replace it with nullptr
                            if ( pArr->nodes[nSlot].compare_exchange_strong(slot, node_ptr(nullptr), memory_model::memory_order_acquire, atomics::memory_order_relaxed))
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

        template <typename Func>
        std::pair<bool, bool> do_update( value_type& val, Func f, bool bInsert = true )
        {
            hash_type const& hash = hash_accessor()( val );
            hash_splitter splitter( hash );
            hash_comparator cmp;
            typename gc::Guard guard;
            back_off bkoff;

            array_node * pArr = m_Head;
            size_t nSlot = splitter.cut( m_Metrics.head_node_size_log );
            assert( nSlot < m_Metrics.head_node_size );
            size_t nOffset = m_Metrics.head_node_size_log;

            while ( true ) {
                node_ptr slot = pArr->nodes[nSlot].load( memory_model::memory_order_acquire );
                if ( slot.bits() == flag_array_node ) {
                    // array node, go down the tree
                    assert( slot.ptr() != nullptr );
                    nSlot = splitter.cut( m_Metrics.array_node_size_log );
                    assert( nSlot < m_Metrics.array_node_size );
                    pArr = to_array( slot.ptr() );
                    nOffset += m_Metrics.array_node_size_log;
                }
                else if ( slot.bits() == flag_array_converting ) {
                    // the slot is converting to array node right now
                    bkoff();
                    m_Stat.onSlotConverting();
                }
                else {
                    // data node
                    assert(slot.bits() == 0 );

                    // protect data node by hazard pointer
                    if ( guard.protect( pArr->nodes[nSlot], [](node_ptr p) -> value_type * { return p.ptr(); }) != slot ) {
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

                            if ( pArr->nodes[nSlot].compare_exchange_strong( slot, node_ptr( &val ), memory_model::memory_order_release, atomics::memory_order_relaxed )) {
                                // slot can be disposed
                                f( false, val );
                                gc::template retire<disposer>( slot.ptr() );
                                m_Stat.onUpdateExisting();
                                return std::make_pair( true, false );
                            }

                            m_Stat.onUpdateRetry();
                            continue;
                        }

                        // the slot must be expanded
                        expand_slot( pArr, nSlot, slot, nOffset );
                    }
                    else {
                        // the slot is empty, try to insert data node
                        if ( bInsert ) {
                            node_ptr pNull;
                            if ( pArr->nodes[nSlot].compare_exchange_strong( pNull, node_ptr( &val ), memory_model::memory_order_release, atomics::memory_order_relaxed ))
                            {
                                // the new data node has been inserted
                                f( true, val );
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
        //@endcond
    };
}} // namespace cds::intrusive

/*
namespace std {

    template <class GC, typename T, typename Traits>
    struct iterator_traits< typename cds::intrusive::MultiLevelHashSet< GC, T, Traits >::iterator >
    {
        typedef typename cds::intrusive::MultiLevelHashSet< GC, T, Traits >::iterator iterator_class;

        // difference_type is not applicable for that iterator
        // typedef ??? difference_type
        typedef T value_type;
        typedef typename iterator_class::value_ptr pointer;
        typedef typename iterator_class::value_ref reference;
        typedef bidirectional_iterator_tag iterator_category;
    };

    template <class GC, typename T, typename Traits>
    struct iterator_traits< typename cds::intrusive::MultiLevelHashSet< GC, T, Traits >::const_iterator >
    {
        typedef typename cds::intrusive::MultiLevelHashSet< GC, T, Traits >::const_iterator iterator_class;

        // difference_type is not applicable for that iterator
        // typedef ??? difference_type
        typedef T value_type;
        typedef typename iterator_class::value_ptr pointer;
        typedef typename iterator_class::value_ref reference;
        typedef bidirectional_iterator_tag iterator_category;
    };

} // namespace std
*/

#endif // #ifndef CDSLIB_INTRUSIVE_IMPL_MULTILEVEL_HASHSET_H
