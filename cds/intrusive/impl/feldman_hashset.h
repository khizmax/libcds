// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_IMPL_FELDMAN_HASHSET_H
#define CDSLIB_INTRUSIVE_IMPL_FELDMAN_HASHSET_H

#include <functional>   // std::ref
#include <iterator>     // std::iterator_traits
#include <vector>

#include <cds/intrusive/details/feldman_hashset_base.h>
#include <cds/details/allocator.h>

namespace cds { namespace intrusive {
    /// Intrusive hash set based on multi-level array
    /** @ingroup cds_intrusive_map
        @anchor cds_intrusive_FeldmanHashSet_hp

        Source:
        - [2013] Steven Feldman, Pierre LaBorde, Damian Dechev "Concurrent Multi-level Arrays:
                 Wait-free Extensible Hash Maps"

        [From the paper] The hardest problem encountered while developing a parallel hash map is how to perform
        a global resize, the process of redistributing the elements in a hash map that occurs when adding new
        buckets. The negative impact of blocking synchronization is multiplied during a global resize, because all
        threads will be forced to wait on the thread that is performing the involved process of resizing the hash map
        and redistributing the elements. \p %FeldmanHashSet implementation avoids global resizes through new array
        allocation. By allowing concurrent expansion this structure is free from the overhead of an explicit resize,
        which facilitates concurrent operations.

        The presented design includes dynamic hashing, the use of sub-arrays within the hash map data structure;
        which, in combination with <b>perfect hashing</b>, means that each element has a unique final, as well as current, position.
        It is important to note that the perfect hash function required by our hash map is trivial to realize as
        any hash function that permutes the bits of the key is suitable. This is possible because of our approach
        to the hash function; we require that it produces hash values that are equal in size to that of the key.
        We know that if we expand the hash map a fixed number of times there can be no collision as duplicate keys
        are not provided for in the standard semantics of a hash map.

        \p %FeldmanHashSet is a multi-level array which has a structure similar to a tree:
        @image html feldman_hashset.png
        The multi-level array differs from a tree in that each position on the tree could hold an array of nodes or a single node.
        A position that holds a single node is a \p dataNode which holds the hash value of a key and the value that is associated
        with that key; it is a simple struct holding two variables. A \p dataNode in the multi-level array could be marked.
        A \p markedDataNode refers to a pointer to a \p dataNode that has been bitmarked at the least significant bit (LSB)
        of the pointer to the node. This signifies that this \p dataNode is contended. An expansion must occur at this node;
        any thread that sees this \p markedDataNode will try to replace it with an \p arrayNode; which is a position that holds
        an array of nodes. The pointer to an \p arrayNode is differentiated from that of a pointer to a \p dataNode by a bitmark
        on the second-least significant bit.

        \p %FeldmanHashSet multi-level array is similar to a tree in that we keep a pointer to the root, which is a memory array
        called \p head. The length of the \p head memory array is unique, whereas every other \p arrayNode has a uniform length;
        a normal \p arrayNode has a fixed power-of-two length equal to the binary logarithm of a variable called \p arrayLength.
        The maximum depth of the tree, \p maxDepth, is the maximum number of pointers that must be followed to reach any node.
        We define \p currentDepth as the number of memory arrays that we need to traverse to reach the \p arrayNode on which
        we need to operate; this is initially one, because of \p head.

        That approach to the structure of the hash set uses an extensible hashing scheme; <b> the hash value is treated as a bit
        string</b> and rehash incrementally.

        @note Two important things you should keep in mind when you're using \p %FeldmanHashSet:
        - all keys must be fixed-size. It means that you cannot use \p std::string as a key for \p %FeldmanHashSet.
          Instead, for the strings you should use well-known hashing algorithms like <a href="https://en.wikipedia.org/wiki/Secure_Hash_Algorithm">SHA1, SHA2</a>,
          <a href="https://en.wikipedia.org/wiki/MurmurHash">MurmurHash</a>, <a href="https://en.wikipedia.org/wiki/CityHash">CityHash</a>
          or its successor <a href="https://code.google.com/p/farmhash/">FarmHash</a> and so on, which
          converts variable-length strings to fixed-length bit-strings, and use that hash as a key in \p %FeldmanHashSet.
        - \p %FeldmanHashSet uses a perfect hashing. It means that if two different keys, for example, of type \p std::string,
          have identical hash then you cannot insert both that keys in the set. \p %FeldmanHashSet does not maintain the key,
          it maintains its fixed-size hash value.

        The set supports @ref cds_intrusive_FeldmanHashSet_iterators "bidirectional thread-safe iterators".

        Template parameters:
        - \p GC - safe memory reclamation schema. Can be \p gc::HP, \p gc::DHP or one of \ref cds_urcu_type "RCU type"
        - \p T - a value type to be stored in the set
        - \p Traits - type traits, the structure based on \p feldman_hashset::traits or result of \p feldman_hashset::make_traits metafunction.
            \p Traits is the mandatory argument because it has one mandatory type - an @ref feldman_hashset::traits::hash_accessor "accessor"
            to hash value of \p T. The set algorithm does not calculate that hash value.

        There are several specializations of \p %FeldmanHashSet for each \p GC. You should include:
        - <tt><cds/intrusive/feldman_hashset_hp.h></tt> for \p gc::HP garbage collector
        - <tt><cds/intrusive/feldman_hashset_dhp.h></tt> for \p gc::DHP garbage collector
        - <tt><cds/intrusive/feldman_hashset_rcu.h></tt> for \ref cds_intrusive_FeldmanHashSet_rcu "RCU type". RCU specialization
            has a slightly different interface.
    */
    template <
        class GC
        ,typename T
#ifdef CDS_DOXYGEN_INVOKED
       ,typename Traits = feldman_hashset::traits
#else
       ,typename Traits
#endif
    >
    class FeldmanHashSet: protected feldman_hashset::multilevel_array<T, Traits>
    {
        //@cond
        typedef feldman_hashset::multilevel_array<T, Traits> base_class;
        //@endcond

    public:
        typedef GC      gc;         ///< Garbage collector
        typedef T       value_type; ///< type of value stored in the set
        typedef Traits  traits;     ///< Traits template parameter, see \p feldman_hashset::traits

        typedef typename traits::hash_accessor hash_accessor;   ///< Hash accessor functor
        typedef typename base_class::hash_type hash_type;       ///< Hash type deduced from \p hash_accessor return type
        typedef typename traits::disposer disposer;             ///< data node disposer
        typedef typename base_class::hash_comparator hash_comparator; ///< hash compare functor based on \p traits::compare and \p traits::less options

        typedef typename traits::item_counter   item_counter;   ///< Item counter type
        typedef typename traits::node_allocator node_allocator; ///< Array node allocator
        typedef typename traits::memory_model   memory_model;   ///< Memory model
        typedef typename traits::back_off       back_off;       ///< Backoff strategy
        typedef typename traits::stat           stat;           ///< Internal statistics type

        typedef typename gc::template guarded_ptr< value_type > guarded_ptr; ///< Guarded pointer

        /// Count of hazard pointers required
        static constexpr size_t const c_nHazardPtrCount = 2;

        /// The size of hash_type in bytes, see \p feldman_hashset::traits::hash_size for explanation
        static constexpr size_t const c_hash_size = base_class::c_hash_size;

        /// Level statistics
        typedef feldman_hashset::level_statistics level_statistics;

    protected:
        //@cond
        typedef typename base_class::node_ptr node_ptr;
        typedef typename base_class::atomic_node_ptr atomic_node_ptr;
        typedef typename base_class::array_node array_node;
        typedef typename base_class::traverse_data traverse_data;

        using base_class::to_array;
        using base_class::to_node;
        using base_class::stats;
        using base_class::head;
        using base_class::metrics;
        //@endcond

    protected:
        //@cond
        class iterator_base
        {
            friend class FeldmanHashSet;

        protected:
            array_node *        m_pNode;    ///< current array node
            size_t              m_idx;      ///< current position in m_pNode
            typename gc::Guard  m_guard;    ///< HP guard
            FeldmanHashSet const*  m_set;    ///< Hash set

        public:
            iterator_base() noexcept
                : m_pNode( nullptr )
                , m_idx( 0 )
                , m_set( nullptr )
            {}

            iterator_base( iterator_base const& rhs ) noexcept
                : m_pNode( rhs.m_pNode )
                , m_idx( rhs.m_idx )
                , m_set( rhs.m_set )
            {
                m_guard.copy( rhs.m_guard );
            }

            iterator_base& operator=( iterator_base const& rhs ) noexcept
            {
                m_pNode = rhs.m_pNode;
                m_idx = rhs.m_idx;
                m_set = rhs.m_set;
                m_guard.copy( rhs.m_guard );
                return *this;
            }

            iterator_base& operator++()
            {
                forward();
                return *this;
            }

            iterator_base& operator--()
            {
                backward();
                return *this;
            }

            void release()
            {
                m_guard.clear();
            }

            bool operator ==( iterator_base const& rhs ) const noexcept
            {
                return m_pNode == rhs.m_pNode && m_idx == rhs.m_idx && m_set == rhs.m_set;
            }

            bool operator !=( iterator_base const& rhs ) const noexcept
            {
                return !( *this == rhs );
            }

        protected:
            iterator_base( FeldmanHashSet const& set, array_node * pNode, size_t idx, bool )
                : m_pNode( pNode )
                , m_idx( idx )
                , m_set( &set )
            {}

            iterator_base( FeldmanHashSet const& set, array_node * pNode, size_t idx )
                : m_pNode( pNode )
                , m_idx( idx )
                , m_set( &set )
            {
                forward();
            }

            value_type * pointer() const noexcept
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
                        if ( slot.bits() == base_class::flag_array_node ) {
                            // array node, go down the tree
                            assert( slot.ptr() != nullptr );
                            pNode = to_array( slot.ptr());
                            idx = 0;
                            nodeSize = arrayNodeSize;
                        }
                        else if ( slot.bits() == base_class::flag_array_converting ) {
                            // the slot is converting to array node right now - skip the node
                            ++idx;
                        }
                        else {
                            if ( slot.ptr()) {
                                // data node
                                if ( m_guard.protect( pNode->nodes[idx], []( node_ptr p ) -> value_type* { return p.ptr(); }) == slot ) {
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
                            assert( pNode == m_set->head());
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
                        if ( slot.bits() == base_class::flag_array_node ) {
                            // array node, go down the tree
                            assert( slot.ptr() != nullptr );
                            pNode = to_array( slot.ptr());
                            nodeSize = arrayNodeSize;
                            idx = nodeSize - 1;
                        }
                        else if ( slot.bits() == base_class::flag_array_converting ) {
                            // the slot is converting to array node right now - skip the node
                            --idx;
                        }
                        else {
                            if ( slot.ptr()) {
                                // data node
                                if ( m_guard.protect( pNode->nodes[idx], []( node_ptr p ) -> value_type* { return p.ptr(); }) == slot ) {
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
                            assert( pNode == m_set->head());
                            assert( idx == endIdx );
                            m_pNode = pNode;
                            m_idx = idx;
                            return;
                        }
                    }
                }
            }
        };

        template <class Iterator>
        Iterator init_begin() const
        {
            return Iterator( *this, head(), size_t(0) - 1 );
        }

        template <class Iterator>
        Iterator init_end() const
        {
            return Iterator( *this, head(), head_size(), false );
        }

        template <class Iterator>
        Iterator init_rbegin() const
        {
            return Iterator( *this, head(), head_size());
        }

        template <class Iterator>
        Iterator init_rend() const
        {
            return Iterator( *this, head(), size_t(0) - 1, false );
        }

        /// Bidirectional iterator class
        template <bool IsConst>
        class bidirectional_iterator: protected iterator_base
        {
            friend class FeldmanHashSet;

        protected:
            static constexpr bool const c_bConstantIterator = IsConst;

        public:
            typedef typename std::conditional< IsConst, value_type const*, value_type*>::type value_ptr; ///< Value pointer
            typedef typename std::conditional< IsConst, value_type const&, value_type&>::type value_ref; ///< Value reference

        public:
            bidirectional_iterator() noexcept
            {}

            bidirectional_iterator( bidirectional_iterator const& rhs ) noexcept
                : iterator_base( rhs )
            {}

            bidirectional_iterator& operator=( bidirectional_iterator const& rhs ) noexcept
            {
                iterator_base::operator=( rhs );
                return *this;
            }

            bidirectional_iterator& operator++()
            {
                iterator_base::operator++();
                return *this;
            }

            bidirectional_iterator& operator--()
            {
                iterator_base::operator--();
                return *this;
            }

            value_ptr operator ->() const noexcept
            {
                return iterator_base::pointer();
            }

            value_ref operator *() const noexcept
            {
                value_ptr p = iterator_base::pointer();
                assert( p );
                return *p;
            }

            void release()
            {
                iterator_base::release();
            }

            template <bool IsConst2>
            bool operator ==( bidirectional_iterator<IsConst2> const& rhs ) const noexcept
            {
                return iterator_base::operator==( rhs );
            }

            template <bool IsConst2>
            bool operator !=( bidirectional_iterator<IsConst2> const& rhs ) const noexcept
            {
                return !( *this == rhs );
            }

        protected:
            bidirectional_iterator( FeldmanHashSet& set, array_node * pNode, size_t idx, bool )
                : iterator_base( set, pNode, idx, false )
            {}

            bidirectional_iterator( FeldmanHashSet& set, array_node * pNode, size_t idx )
                : iterator_base( set, pNode, idx )
            {}
        };

        /// Reverse bidirectional iterator
        template <bool IsConst>
        class reverse_bidirectional_iterator : public iterator_base
        {
            friend class FeldmanHashSet;

        public:
            typedef typename std::conditional< IsConst, value_type const*, value_type*>::type value_ptr; ///< Value pointer
            typedef typename std::conditional< IsConst, value_type const&, value_type&>::type value_ref; ///< Value reference

        public:
            reverse_bidirectional_iterator() noexcept
                : iterator_base()
            {}

            reverse_bidirectional_iterator( reverse_bidirectional_iterator const& rhs ) noexcept
                : iterator_base( rhs )
            {}

            reverse_bidirectional_iterator& operator=( reverse_bidirectional_iterator const& rhs) noexcept
            {
                iterator_base::operator=( rhs );
                return *this;
            }

            reverse_bidirectional_iterator& operator++()
            {
                iterator_base::operator--();
                return *this;
            }

            reverse_bidirectional_iterator& operator--()
            {
                iterator_base::operator++();
                return *this;
            }

            value_ptr operator ->() const noexcept
            {
                return iterator_base::pointer();
            }

            value_ref operator *() const noexcept
            {
                value_ptr p = iterator_base::pointer();
                assert( p );
                return *p;
            }

            void release()
            {
                iterator_base::release();
            }

            template <bool IsConst2>
            bool operator ==( reverse_bidirectional_iterator<IsConst2> const& rhs ) const
            {
                return iterator_base::operator==( rhs );
            }

            template <bool IsConst2>
            bool operator !=( reverse_bidirectional_iterator<IsConst2> const& rhs )
            {
                return !( *this == rhs );
            }

        private:
            reverse_bidirectional_iterator( FeldmanHashSet& set, array_node * pNode, size_t idx, bool )
                : iterator_base( set, pNode, idx, false )
            {}

            reverse_bidirectional_iterator( FeldmanHashSet& set, array_node * pNode, size_t idx )
                : iterator_base( set, pNode, idx, false )
            {
                iterator_base::backward();
            }
        };
        //@endcond

    public:
#ifdef CDS_DOXYGEN_INVOKED
        typedef implementation_defined iterator;            ///< @ref cds_intrusive_FeldmanHashSet_iterators "bidirectional iterator" type
        typedef implementation_defined const_iterator;      ///< @ref cds_intrusive_FeldmanHashSet_iterators "bidirectional const iterator" type
        typedef implementation_defined reverse_iterator;    ///< @ref cds_intrusive_FeldmanHashSet_iterators "bidirectional reverse iterator" type
        typedef implementation_defined const_reverse_iterator; ///< @ref cds_intrusive_FeldmanHashSet_iterators "bidirectional reverse const iterator" type
#else
        typedef bidirectional_iterator<false>   iterator;
        typedef bidirectional_iterator<true>    const_iterator;
        typedef reverse_bidirectional_iterator<false>   reverse_iterator;
        typedef reverse_bidirectional_iterator<true>    const_reverse_iterator;
#endif

    private:
        //@cond
        item_counter      m_ItemCounter; ///< Item counter
        //@endcond

    public:
        /// Creates empty set
        /**
            @param head_bits - 2<sup>head_bits</sup> specifies the size of head array, minimum is 4.
            @param array_bits - 2<sup>array_bits</sup> specifies the size of array node, minimum is 2.

            Equation for \p head_bits and \p array_bits:
            \code
            sizeof( hash_type ) * 8 == head_bits + N * array_bits
            \endcode
            where \p N is multi-level array depth.
        */
        FeldmanHashSet( size_t head_bits = 8, size_t array_bits = 4 )
            : base_class( head_bits, array_bits )
        {}

        /// Destructs the set and frees all data
        ~FeldmanHashSet()
        {
            clear();
        }

        /// Inserts new node
        /**
            The function inserts \p val in the set if it does not contain
            an item with that hash.

            Returns \p true if \p val is placed into the set, \p false otherwise.
        */
        bool insert( value_type& val )
        {
            return insert( val, []( value_type& ) {} );
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
            traverse_data pos( hash, *this );
            hash_comparator cmp;
            typename gc::template GuardArray<2> guards;

            guards.assign( 1, &val );
            while ( true ) {
                node_ptr slot = base_class::traverse( pos );
                assert( slot.bits() == 0 );

                // protect data node by hazard pointer
                if ( guards.protect( 0, pos.pArr->nodes[pos.nSlot], []( node_ptr p ) -> value_type* { return p.ptr(); }) != slot ) {
                    // slot value has been changed - retry
                    stats().onSlotChanged();
                }
                else if ( slot.ptr()) {
                    if ( cmp( hash, hash_accessor()( *slot.ptr())) == 0 ) {
                        // the item with that hash value already exists
                        stats().onInsertFailed();
                        return false;
                    }

                    if ( !pos.splitter.eos()) {
                        // the slot must be expanded
                        base_class::expand_slot( pos, slot );
                    }
                    else
                        return false;
                }
                else {
                    // the slot is empty, try to insert data node
                    node_ptr pNull;
                    if ( pos.pArr->nodes[pos.nSlot].compare_exchange_strong( pNull, node_ptr( &val ), memory_model::memory_order_release, atomics::memory_order_relaxed ))
                    {
                        // the new data node has been inserted
                        f( val );
                        ++m_ItemCounter;
                        stats().onInsertSuccess();
                        stats().height( pos.nHeight );
                        return true;
                    }

                    // insert failed - slot has been changed by another thread
                    // retry inserting
                    stats().onInsertRetry();
                }
            }
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

            Returns <tt> std::pair<bool, bool> </tt> where \p first is \p true if operation is successful
            (i.e. the item has been inserted or updated),
            \p second is \p true if new item has been added or \p false if the set contains that hash.
        */
        std::pair<bool, bool> update( value_type& val, bool bInsert = true )
        {
            return do_update( val, []( value_type&, value_type* ) {}, bInsert );
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
            auto pred = [&val]( value_type const& item ) -> bool { return &item == &val; };
            value_type * p = do_erase( hash_accessor()( val ), guard, std::ref( pred ));
            return p != nullptr;
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
            return erase( hash, []( value_type const& ) {} );
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
                f( *p );
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
            return do_erase_at( iter );
        }
        //@cond
        bool erase_at( reverse_iterator const& iter )
        {
            return do_erase_at( iter );
        }
        //@endcond

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
            typedef cds::intrusive::FeldmanHashSet< your_template_args > my_set;
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
            typename gc::Guard guard;
            if ( do_erase( hash, guard, []( value_type const&) -> bool {return true;} ))
                return guarded_ptr( std::move( guard ));
            return guarded_ptr();
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
            return find( hash, []( value_type& ) {} );
        }

        /// Finds an item by it's \p hash and returns the item found
        /**
            The function searches the item by its \p hash
            and returns the guarded pointer to the item found.
            If \p hash is not found the function returns an empty \p guarded_ptr.

            @note Each \p guarded_ptr object uses one GC's guard which can be limited resource.

            Usage:
            \code
            typedef cds::intrusive::FeldmanHashSet< your_template_params >  my_set;
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
            typename gc::Guard guard;
            if ( search( hash, guard ))
                return guarded_ptr( std::move( guard ));
            return guarded_ptr();
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
            clear_array( head(), head_size());
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
            return stats();
        }

        /// Returns the size of head node
        using base_class::head_size;

        /// Returns the size of the array node
        using base_class::array_node_size;

        /// Collects tree level statistics into \p stat
        /**
            The function traverses the set and collects statistics for each level of the tree
            into \p feldman_hashset::level_statistics struct. The element of \p stat[i]
            represents statistics for level \p i, level 0 is head array.
            The function is thread-safe and may be called in multi-threaded environment.

            Result can be useful for estimating efficiency of hash functor you use.
        */
        void get_level_statistics( std::vector< feldman_hashset::level_statistics>& stat ) const
        {
            base_class::get_level_statistics( stat );
        }

    public:
    ///@name Thread-safe iterators
        /** @anchor cds_intrusive_FeldmanHashSet_iterators
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
                After \p release() the iterator points to \p nullptr but it still remain valid: further iterating is possible.

            During iteration you may safely erase any item from the set;
            @ref erase_at() function call doesn't invalidate any iterator.
            If some iterator points to the item to be erased, that item is not deleted immediately
            but only after that iterator will be advanced forward or backward.

            @note It is possible the item can be iterated more that once, for example, if an iterator points to the item
            in array node that is being splitted.
        */
    ///@{

        /// Returns an iterator to the beginning of the set
        iterator begin()
        {
            return iterator( *this, head(), size_t(0) - 1 );
        }

        /// Returns an const iterator to the beginning of the set
        const_iterator begin() const
        {
            return const_iterator( *this, head(), size_t(0) - 1 );
        }

        /// Returns an const iterator to the beginning of the set
        const_iterator cbegin()
        {
            return const_iterator( *this, head(), size_t(0) - 1 );
        }

        /// Returns an iterator to the element following the last element of the set. This element acts as a placeholder; attempting to access it results in undefined behavior.
        iterator end()
        {
            return iterator( *this, head(), head_size(), false );
        }

        /// Returns a const iterator to the element following the last element of the set. This element acts as a placeholder; attempting to access it results in undefined behavior.
        const_iterator end() const
        {
            return const_iterator( *this, head(), head_size(), false );
        }

        /// Returns a const iterator to the element following the last element of the set. This element acts as a placeholder; attempting to access it results in undefined behavior.
        const_iterator cend()
        {
            return const_iterator( *this, head(), head_size(), false );
        }

        /// Returns a reverse iterator to the first element of the reversed set
        reverse_iterator rbegin()
        {
            return reverse_iterator( *this, head(), head_size());
        }

        /// Returns a const reverse iterator to the first element of the reversed set
        const_reverse_iterator rbegin() const
        {
            return const_reverse_iterator( *this, head(), head_size());
        }

        /// Returns a const reverse iterator to the first element of the reversed set
        const_reverse_iterator crbegin()
        {
            return const_reverse_iterator( *this, head(), head_size());
        }

        /// Returns a reverse iterator to the element following the last element of the reversed set
        /**
            It corresponds to the element preceding the first element of the non-reversed container.
            This element acts as a placeholder, attempting to access it results in undefined behavior.
        */
        reverse_iterator rend()
        {
            return reverse_iterator( *this, head(), size_t(0) - 1, false );
        }

        /// Returns a const reverse iterator to the element following the last element of the reversed set
        /**
            It corresponds to the element preceding the first element of the non-reversed container.
            This element acts as a placeholder, attempting to access it results in undefined behavior.
        */
        const_reverse_iterator rend() const
        {
            return const_reverse_iterator( *this, head(), size_t(0) - 1, false );
        }

        /// Returns a const reverse iterator to the element following the last element of the reversed set
        /**
            It corresponds to the element preceding the first element of the non-reversed container.
            This element acts as a placeholder, attempting to access it results in undefined behavior.
        */
        const_reverse_iterator crend()
        {
            return const_reverse_iterator( *this, head(), size_t(0) - 1, false );
        }
    ///@}

    private:
        //@cond
        void clear_array( array_node * pArrNode, size_t nSize )
        {
            back_off bkoff;

            for ( atomic_node_ptr * pArr = pArrNode->nodes, *pLast = pArr + nSize; pArr != pLast; ++pArr ) {
                while ( true ) {
                    node_ptr slot = pArr->load( memory_model::memory_order_acquire );
                    if ( slot.bits() == base_class::flag_array_node ) {
                        // array node, go down the tree
                        assert( slot.ptr() != nullptr );
                        clear_array( to_array( slot.ptr()), array_node_size());
                        break;
                    }
                    else if ( slot.bits() == base_class::flag_array_converting ) {
                        // the slot is converting to array node right now
                        while (( slot = pArr->load( memory_model::memory_order_acquire )).bits() == base_class::flag_array_converting ) {
                            bkoff();
                            stats().onSlotConverting();
                        }
                        bkoff.reset();

                        assert( slot.ptr() != nullptr );
                        assert( slot.bits() == base_class::flag_array_node );
                        clear_array( to_array( slot.ptr()), array_node_size());
                        break;
                    }
                    else {
                        // data node
                        if ( pArr->compare_exchange_strong( slot, node_ptr(), memory_model::memory_order_acquire, atomics::memory_order_relaxed )) {
                            if ( slot.ptr()) {
                                gc::template retire<disposer>( slot.ptr());
                                --m_ItemCounter;
                                stats().onEraseSuccess();
                            }
                            break;
                        }
                    }
                }
            }
        }
        //@endcond

    protected:
        //@cond
        value_type * search( hash_type const& hash, typename gc::Guard& guard )
        {
            traverse_data pos( hash, *this );
            hash_comparator cmp;

            while ( true ) {
                node_ptr slot = base_class::traverse( pos );
                assert( slot.bits() == 0 );

                // protect data node by hazard pointer
                if ( guard.protect( pos.pArr->nodes[pos.nSlot], []( node_ptr p ) -> value_type* { return p.ptr(); }) != slot) {
                    // slot value has been changed - retry
                    stats().onSlotChanged();
                    continue;
                }
                else if ( slot.ptr() && cmp( hash, hash_accessor()( *slot.ptr())) == 0 ) {
                    // item found
                    stats().onFindSuccess();
                    return slot.ptr();
                }
                stats().onFindFailed();
                return nullptr;
            }
        }

        template <typename Predicate>
        value_type * do_erase( hash_type const& hash, typename gc::Guard& guard, Predicate pred )
        {
            traverse_data pos( hash, *this );
            hash_comparator cmp;
            while ( true ) {
                node_ptr slot = base_class::traverse( pos );
                assert( slot.bits() == 0 );

                // protect data node by hazard pointer
                if ( guard.protect( pos.pArr->nodes[pos.nSlot], []( node_ptr p ) -> value_type* { return p.ptr(); }) != slot ) {
                    // slot value has been changed - retry
                    stats().onSlotChanged();
                }
                else if ( slot.ptr()) {
                    if ( cmp( hash, hash_accessor()( *slot.ptr())) == 0 && pred( *slot.ptr())) {
                        // item found - replace it with nullptr
                        if ( pos.pArr->nodes[pos.nSlot].compare_exchange_strong( slot, node_ptr( nullptr ), memory_model::memory_order_acquire, atomics::memory_order_relaxed)) {
                            // slot is guarded by HP
                            gc::template retire<disposer>( slot.ptr());
                            --m_ItemCounter;
                            stats().onEraseSuccess();

                            return slot.ptr();
                        }
                        stats().onEraseRetry();
                        continue;
                    }
                    stats().onEraseFailed();
                    return nullptr;
                }
                else {
                    // the slot is empty
                    stats().onEraseFailed();
                    return nullptr;
                }
            }
        }

        bool do_erase_at( iterator_base const& iter )
        {
            if ( iter.m_set != this )
                return false;
            if ( iter.m_pNode == head()) {
                if ( iter.m_idx >= head_size())
                    return false;
            }
            else if ( iter.m_idx >= array_node_size())
                return false;

            for (;;) {
                node_ptr slot = iter.m_pNode->nodes[iter.m_idx].load( memory_model::memory_order_acquire );
                if ( slot.bits() == 0 && slot.ptr() == iter.pointer()) {
                    if ( iter.m_pNode->nodes[iter.m_idx].compare_exchange_strong( slot, node_ptr( nullptr ), memory_model::memory_order_acquire, atomics::memory_order_relaxed )) {
                        // the item is guarded by iterator, so we may retire it safely
                        gc::template retire<disposer>( slot.ptr());
                        --m_ItemCounter;
                        stats().onEraseSuccess();
                        return true;
                    }
                }
                else
                    return false;
            }
        }

        template <typename Func>
        std::pair<bool, bool> do_update( value_type& val, Func f, bool bInsert = true )
        {
            hash_type const& hash = hash_accessor()( val );
            traverse_data pos( hash, *this );
            hash_comparator cmp;
            typename gc::template GuardArray<2> guards;

            guards.assign( 1, &val );
            while ( true ) {
                node_ptr slot = base_class::traverse( pos );
                assert( slot.bits() == 0 );

                // protect data node by hazard pointer
                if ( guards.protect( 0, pos.pArr->nodes[pos.nSlot], []( node_ptr p ) -> value_type* { return p.ptr(); }) != slot ) {
                    // slot value has been changed - retry
                    stats().onSlotChanged();
                }
                else if ( slot.ptr()) {
                    if ( cmp( hash, hash_accessor()( *slot.ptr())) == 0 ) {
                        // the item with that hash value already exists
                        // Replace it with val
                        if ( slot.ptr() == &val ) {
                            stats().onUpdateExisting();
                            return std::make_pair( true, false );
                        }

                        if ( pos.pArr->nodes[pos.nSlot].compare_exchange_strong( slot, node_ptr( &val ), memory_model::memory_order_release, atomics::memory_order_relaxed )) {
                            // slot can be disposed
                            f( val, slot.ptr());
                            gc::template retire<disposer>( slot.ptr());
                            stats().onUpdateExisting();
                            return std::make_pair( true, false );
                        }

                        stats().onUpdateRetry();
                        continue;
                    }

                    if ( bInsert ) {
                        if ( !pos.splitter.eos()) {
                            // the slot must be expanded
                            base_class::expand_slot( pos, slot );
                        }
                        else
                            return std::make_pair( false, false );
                    }
                    else {
                        stats().onUpdateFailed();
                        return std::make_pair( false, false );
                    }
                }
                else {
                    // the slot is empty, try to insert data node
                    if ( bInsert ) {
                        node_ptr pNull;
                        if ( pos.pArr->nodes[pos.nSlot].compare_exchange_strong( pNull, node_ptr( &val ), memory_model::memory_order_release, atomics::memory_order_relaxed ))
                        {
                            // the new data node has been inserted
                            f( val, nullptr );
                            ++m_ItemCounter;
                            stats().onUpdateNew();
                            stats().height( pos.nHeight );
                            return std::make_pair( true, true );
                        }
                    }
                    else {
                        stats().onUpdateFailed();
                        return std::make_pair( false, false );
                    }

                    // insert failed - slot has been changed by another thread
                    // retry updating
                    stats().onUpdateRetry();
                }
            } // while
        }
        //@endcond
    };
}} // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_IMPL_FELDMAN_HASHSET_H
