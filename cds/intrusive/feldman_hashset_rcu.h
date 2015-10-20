//$$CDS-header$$

#ifndef CDSLIB_INTRUSIVE_FELDMAN_HASHSET_RCU_H
#define CDSLIB_INTRUSIVE_FELDMAN_HASHSET_RCU_H

#include <functional>   // std::ref
#include <iterator>     // std::iterator_traits

#include <cds/intrusive/details/feldman_hashset_base.h>
#include <cds/details/allocator.h>
#include <cds/urcu/details/check_deadlock.h>
#include <cds/urcu/exempt_ptr.h>
#include <cds/intrusive/details/raw_ptr_disposer.h>


namespace cds { namespace intrusive {
    /// Intrusive hash set based on multi-level array, \ref cds_urcu_desc "RCU" specialization
    /** @ingroup cds_intrusive_map
        @anchor cds_intrusive_FeldmanHashSet_rcu

        Source:
        - [2013] Steven Feldman, Pierre LaBorde, Damian Dechev "Concurrent Multi-level Arrays:
            Wait-free Extensible Hash Maps"

        See algorithm short description @ref cds_intrusive_FeldmanHashSet_hp "here"

        @note Two important things you should keep in mind when you're using \p %FeldmanHashSet:
        - all keys must be fixed-size. It means that you cannot use \p std::string as a key for \p %FeldmanHashSet.
          Instead, for the strings you should use well-known hashing algorithms like <a href="https://en.wikipedia.org/wiki/Secure_Hash_Algorithm">SHA1, SHA2</a>,
          <a href="https://en.wikipedia.org/wiki/MurmurHash">MurmurHash</a>, <a href="https://en.wikipedia.org/wiki/CityHash">CityHash</a>
          or its successor <a href="https://code.google.com/p/farmhash/">FarmHash</a> and so on, which
          converts variable-length strings to fixed-length bit-strings, and use that hash as a key in \p %FeldmanHashSet.
        - \p %FeldmanHashSet uses a perfect hashing. It means that if two different keys, for example, of type \p std::string,
          have identical hash then you cannot insert both that keys in the set. \p %FeldmanHashSet does not maintain the key,
          it maintains its fixed-size hash value.

        Template parameters:
        - \p RCU - one of \ref cds_urcu_gc "RCU type"
        - \p T - a value type to be stored in the set
        - \p Traits - type traits, the structure based on \p feldman_hashset::traits or result of \p feldman_hashset::make_traits metafunction.
        \p Traits is the mandatory argument because it has one mandatory type - an @ref feldman_hashset::traits::hash_accessor "accessor"
        to hash value of \p T. The set algorithm does not calculate that hash value.

        @note Before including <tt><cds/intrusive/feldman_hashset_rcu.h></tt> you should include appropriate RCU header file,
        see \ref cds_urcu_gc "RCU type" for list of existing RCU class and corresponding header files.

        The set supports @ref cds_intrusive_FeldmanHashSet_rcu_iterators "bidirectional thread-safe iterators" 
        with some restrictions.
    */
    template <
        class RCU,
        class T,
#ifdef CDS_DOXYGEN_INVOKED
        class Traits = feldman_hashset::traits
#else
        class Traits
#endif
    >
    class FeldmanHashSet< cds::urcu::gc< RCU >, T, Traits >
    {
    public:
        typedef cds::urcu::gc< RCU > gc; ///< RCU garbage collector
        typedef T       value_type;      ///< type of value stored in the set
        typedef Traits  traits;          ///< Traits template parameter

        typedef typename traits::hash_accessor hash_accessor; ///< Hash accessor functor
        static_assert(!std::is_same< hash_accessor, cds::opt::none >::value, "hash_accessor functor must be specified in Traits");

        /// Hash type deduced from \p hash_accessor return type
        typedef typename std::decay<
            typename std::remove_reference<
            decltype(hash_accessor()(std::declval<T>()))
            >::type
        >::type hash_type;
        //typedef typename std::result_of< hash_accessor( std::declval<T>()) >::type hash_type;
        static_assert(!std::is_pointer<hash_type>::value, "hash_accessor should return a reference to hash value");

        typedef typename traits::disposer disposer; ///< data node disposer

#   ifdef CDS_DOXYGEN_INVOKED
        typedef implementation_defined hash_comparator;    ///< hash compare functor based on opt::compare and opt::less option setter
#   else
        typedef typename cds::opt::details::make_comparator_from<
            hash_type,
            traits,
            feldman_hashset::bitwise_compare< hash_type >
        >::type hash_comparator;
#   endif

        typedef typename traits::item_counter   item_counter;   ///< Item counter type
        typedef typename traits::node_allocator node_allocator; ///< Array node allocator
        typedef typename traits::memory_model   memory_model;   ///< Memory model
        typedef typename traits::back_off       back_off;       ///< Backoff strategy
        typedef typename traits::stat           stat;           ///< Internal statistics type
        typedef typename traits::rcu_check_deadlock rcu_check_deadlock; ///< Deadlock checking policy
        typedef typename gc::scoped_lock       rcu_lock;        ///< RCU scoped lock
        static CDS_CONSTEXPR const bool c_bExtractLockExternal = false; ///< Group of \p extract_xxx functions does not require external locking

        using exempt_ptr = cds::urcu::exempt_ptr< gc, value_type, value_type, disposer, void >; ///< pointer to extracted node

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

            array_node(array_node * parent, size_t idx)
                : pParent(parent)
                , idxParent(idx)
            {}

            array_node() = delete;
            array_node(array_node const&) = delete;
            array_node(array_node&&) = delete;
        };

        typedef cds::details::Allocator< array_node, node_allocator > cxx_array_node_allocator;
        typedef feldman_hashset::details::hash_splitter< hash_type > hash_splitter;
        typedef cds::urcu::details::check_deadlock_policy< gc, rcu_check_deadlock> check_deadlock_policy;

        //@endcond

    private:
        //@cond
        feldman_hashset::details::metrics const m_Metrics;     ///< Metrics
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
        FeldmanHashSet(size_t head_bits = 8, size_t array_bits = 4)
            : m_Metrics(feldman_hashset::details::metrics::make(head_bits, array_bits, sizeof(hash_type)))
            , m_Head(alloc_head_node())
        {}

        /// Destructs the set and frees all data
        ~FeldmanHashSet()
        {
            destroy_tree();
            free_array_node(m_Head);
        }

        /// Inserts new node
        /**
            The function inserts \p val in the set if it does not contain
            an item with that hash.

            Returns \p true if \p val is placed into the set, \p false otherwise.

            The function locks RCU internally.
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

            The function locks RCU internally.
            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting".
        */
        template <typename Func>
        bool insert( value_type& val, Func f )
        {
            hash_type const& hash = hash_accessor()( val );
            hash_splitter splitter( hash );
            hash_comparator cmp;
            back_off bkoff;

            size_t nOffset = m_Metrics.head_node_size_log;
            array_node * pArr = m_Head;
            size_t nSlot = splitter.cut( m_Metrics.head_node_size_log );
            assert( nSlot < m_Metrics.head_node_size );
            size_t nHeight = 1;

            while ( true ) {
                node_ptr slot = pArr->nodes[nSlot].load( memory_model::memory_order_acquire );
                if ( slot.bits() == flag_array_node ) {
                    // array node, go down the tree
                    assert( slot.ptr() != nullptr );
                    nSlot = splitter.cut( m_Metrics.array_node_size_log );
                    assert( nSlot < m_Metrics.array_node_size );
                    pArr = to_array( slot.ptr() );
                    nOffset += m_Metrics.array_node_size_log;
                    ++nHeight;
                }
                else if ( slot.bits() == flag_array_converting ) {
                    // the slot is converting to array node right now
                    bkoff();
                    m_Stat.onSlotConverting();
                }
                else {
                    // data node
                    assert(slot.bits() == 0 );

                    rcu_lock rcuLock;
                    if ( pArr->nodes[nSlot].load(memory_model::memory_order_acquire) == slot ) {
                        if ( slot.ptr() ) {
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
                                m_Stat.height( nHeight );
                                return true;
                            }

                            // insert failed - slot has been changed by another thread
                            // retry inserting
                            m_Stat.onInsertRetry();
                        }
                    }
                    else
                        m_Stat.onSlotChanged();
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

            The function locks RCU internally.
        */
        std::pair<bool, bool> update( value_type& val, bool bInsert = true )
        {
            return do_update(val, [](value_type&, value_type *) {}, bInsert );
        }

        /// Unlinks the item \p val from the set
        /**
            The function searches the item \p val in the set and unlink it
            if it is found and its address is equal to <tt>&val</tt>.

            The function returns \p true if success and \p false otherwise.

            RCU should not be locked. The function locks RCU internally.
        */
        bool unlink( value_type const& val )
        {
            check_deadlock_policy::check();

            auto pred = [&val](value_type const& item) -> bool { return &item == &val; };
            value_type * p;
            {
                rcu_lock rcuLock;
                p = do_erase( hash_accessor()( val ), std::ref( pred ));
            }
            if ( p ) {
                gc::template retire_ptr<disposer>( p );
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

            RCU should not be locked. The function locks RCU internally.
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

            RCU should not be locked. The function locks RCU internally.
        */
        template <typename Func>
        bool erase( hash_type const& hash, Func f )
        {
            check_deadlock_policy::check();

            value_type * p;

            {
                rcu_lock rcuLock;
                p = do_erase( hash, []( value_type const&) -> bool { return true; } );
            }

            // p is guarded by HP
            if ( p ) {
                f( *p );
                gc::template retire_ptr<disposer>(p);
                return true;
            }
            return false;
        }

        /// Extracts the item with specified \p hash
        /**
            The function searches \p hash in the set,
            unlinks it from the set, and returns \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to the item found.
            If the item with key equal to \p key is not found the function returns an empty \p exempt_ptr.

            RCU \p synchronize method can be called. RCU should NOT be locked.
            The function does not call the disposer for the item found.
            The disposer will be implicitly invoked when the returned object is destroyed or when
            its \p release() member function is called.
            Example:
            \code
            typedef cds::intrusive::FeldmanHashSet< cds::urcu::gc< cds::urcu::general_buffered<> >, foo, my_traits > set_type;
            set_type theSet;
            // ...

            typename set_type::exempt_ptr ep( theSet.extract( 5 ));
            if ( ep ) {
                // Deal with ep
                //...

                // Dispose returned item.
                ep.release();
            }
            \endcode
        */
        exempt_ptr extract( hash_type const& hash )
        {
            check_deadlock_policy::check();

            value_type * p;
            {
                rcu_lock rcuLock;
                p = do_erase( hash, []( value_type const&) -> bool {return true;} );
            }
            return exempt_ptr( p );
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

            The function applies RCU lock internally.
        */
        template <typename Func>
        bool find( hash_type const& hash, Func f )
        {
            rcu_lock rcuLock;

            value_type * p = search( hash );
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

            The function applies RCU lock internally.
        */
        bool contains( hash_type const& hash )
        {
            return find( hash, [](value_type&) {} );
        }

        /// Finds an item by it's \p hash and returns the item found
        /**
            The function searches the item by its \p hash
            and returns the pointer to the item found.
            If \p hash is not found the function returns \p nullptr.

            RCU should be locked before the function invocation.
            Returned pointer is valid only while RCU is locked.

            Usage:
            \code
            typedef cds::intrusive::FeldmanHashSet< your_template_params >  my_set;
            my_set theSet;
            // ...
            {
                // lock RCU
                my_set::rcu_lock;

                foo * p = theSet.get( 5 );
                if ( p ) {
                    // Deal with p
                    //...
                }
            }
            \endcode
        */
        value_type * get( hash_type const& hash )
        {
            assert( gc::is_locked());
            return search( hash );
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

    protected:
        //@cond
        class iterator_base
        {
            friend class FeldmanHashSet;

        protected:
            array_node *        m_pNode;    ///< current array node
            size_t              m_idx;      ///< current position in m_pNode
            value_type *        m_pValue;   ///< current value
            FeldmanHashSet const*  m_set;    ///< Hash set

        public:
            iterator_base() CDS_NOEXCEPT
                : m_pNode(nullptr)
                , m_idx(0)
                , m_pValue(nullptr)
                , m_set(nullptr)
            {}

            iterator_base(iterator_base const& rhs) CDS_NOEXCEPT
                : m_pNode(rhs.m_pNode)
                , m_idx(rhs.m_idx)
                , m_pValue(rhs.m_pValue)
                , m_set(rhs.m_set)
            {}

            iterator_base& operator=(iterator_base const& rhs) CDS_NOEXCEPT
            {
                m_pNode = rhs.m_pNode;
                m_idx = rhs.m_idx;
                m_pValue = rhs.m_pValue;
                m_set = rhs.m_set;
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

            bool operator ==(iterator_base const& rhs) const CDS_NOEXCEPT
            {
                return m_pNode == rhs.m_pNode && m_idx == rhs.m_idx && m_set == rhs.m_set;
            }

            bool operator !=(iterator_base const& rhs) const CDS_NOEXCEPT
            {
                return !(*this == rhs);
            }

        protected:
            iterator_base(FeldmanHashSet const& set, array_node * pNode, size_t idx, bool)
                : m_pNode(pNode)
                , m_idx(idx)
                , m_pValue(nullptr)
                , m_set(&set)
            {}

            iterator_base(FeldmanHashSet const& set, array_node * pNode, size_t idx)
                : m_pNode(pNode)
                , m_idx(idx)
                , m_pValue(nullptr)
                , m_set(&set)
            {
                forward();
            }

            value_type * pointer() const CDS_NOEXCEPT
            {
                assert(gc::is_locked());
                return m_pValue;
            }

            void forward()
            {
                assert( gc::is_locked());
                assert(m_set != nullptr);
                assert(m_pNode != nullptr);

                size_t const arrayNodeSize = m_set->array_node_size();
                size_t const headSize = m_set->head_size();
                array_node * pNode = m_pNode;
                size_t idx = m_idx + 1;
                size_t nodeSize = m_pNode->pParent ? arrayNodeSize : headSize;

                for (;;) {
                    if (idx < nodeSize) {
                        node_ptr slot = pNode->nodes[idx].load(memory_model::memory_order_acquire);
                        if (slot.bits() == flag_array_node) {
                            // array node, go down the tree
                            assert(slot.ptr() != nullptr);
                            pNode = to_array(slot.ptr());
                            idx = 0;
                            nodeSize = arrayNodeSize;
                        }
                        else if (slot.bits() == flag_array_converting) {
                            // the slot is converting to array node right now - skip the node
                            ++idx;
                        }
                        else {
                            if (slot.ptr()) {
                                // data node
                                m_pNode = pNode;
                                m_idx = idx;
                                m_pValue = slot.ptr();
                                return;
                            }
                            ++idx;
                        }
                    }
                    else {
                        // up to parent node
                        if (pNode->pParent) {
                            idx = pNode->idxParent + 1;
                            pNode = pNode->pParent;
                            nodeSize = pNode->pParent ? arrayNodeSize : headSize;
                        }
                        else {
                            // end()
                            assert(pNode == m_set->m_Head);
                            assert(idx == headSize);
                            m_pNode = pNode;
                            m_idx = idx;
                            m_pValue = nullptr;
                            return;
                        }
                    }
                }
            }

            void backward()
            {
                assert(gc::is_locked());
                assert(m_set != nullptr);
                assert(m_pNode != nullptr);

                size_t const arrayNodeSize = m_set->array_node_size();
                size_t const headSize = m_set->head_size();
                size_t const endIdx = size_t(0) - 1;

                array_node * pNode = m_pNode;
                size_t idx = m_idx - 1;
                size_t nodeSize = m_pNode->pParent ? arrayNodeSize : headSize;

                for (;;) {
                    if (idx != endIdx) {
                        node_ptr slot = pNode->nodes[idx].load(memory_model::memory_order_acquire);
                        if (slot.bits() == flag_array_node) {
                            // array node, go down the tree
                            assert(slot.ptr() != nullptr);
                            pNode = to_array(slot.ptr());
                            nodeSize = arrayNodeSize;
                            idx = nodeSize - 1;
                        }
                        else if (slot.bits() == flag_array_converting) {
                            // the slot is converting to array node right now - skip the node
                            --idx;
                        }
                        else {
                            if (slot.ptr()) {
                                // data node
                                m_pNode = pNode;
                                m_idx = idx;
                                m_pValue = slot.ptr();
                                return;
                            }
                            --idx;
                        }
                    }
                    else {
                        // up to parent node
                        if (pNode->pParent) {
                            idx = pNode->idxParent - 1;
                            pNode = pNode->pParent;
                            nodeSize = pNode->pParent ? arrayNodeSize : headSize;
                        }
                        else {
                            // rend()
                            assert(pNode == m_set->m_Head);
                            assert(idx == endIdx);
                            m_pNode = pNode;
                            m_idx = idx;
                            m_pValue = nullptr;
                            return;
                        }
                    }
                }
            }
        };

        template <class Iterator>
        Iterator init_begin() const
        {
            return Iterator(*this, m_Head, size_t(0) - 1);
        }

        template <class Iterator>
        Iterator init_end() const
        {
            return Iterator(*this, m_Head, head_size(), false);
        }

        template <class Iterator>
        Iterator init_rbegin() const
        {
            return Iterator(*this, m_Head, head_size());
        }

        template <class Iterator>
        Iterator init_rend() const
        {
            return Iterator(*this, m_Head, size_t(0) - 1, false);
        }

        /// Bidirectional iterator class
        template <bool IsConst>
        class bidirectional_iterator : protected iterator_base
        {
            friend class FeldmanHashSet;

        protected:
            static CDS_CONSTEXPR bool const c_bConstantIterator = IsConst;

        public:
            typedef typename std::conditional< IsConst, value_type const*, value_type*>::type value_ptr; ///< Value pointer
            typedef typename std::conditional< IsConst, value_type const&, value_type&>::type value_ref; ///< Value reference

        public:
            bidirectional_iterator() CDS_NOEXCEPT
            {}

            bidirectional_iterator(bidirectional_iterator const& rhs) CDS_NOEXCEPT
                : iterator_base(rhs)
            {}

            bidirectional_iterator& operator=(bidirectional_iterator const& rhs) CDS_NOEXCEPT
            {
                iterator_base::operator=(rhs);
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

            value_ptr operator ->() const CDS_NOEXCEPT
            {
                return iterator_base::pointer();
            }

            value_ref operator *() const CDS_NOEXCEPT
            {
                value_ptr p = iterator_base::pointer();
                assert(p);
                return *p;
            }

            template <bool IsConst2>
            bool operator ==(bidirectional_iterator<IsConst2> const& rhs) const CDS_NOEXCEPT
            {
                return iterator_base::operator==(rhs);
            }

            template <bool IsConst2>
            bool operator !=(bidirectional_iterator<IsConst2> const& rhs) const CDS_NOEXCEPT
            {
                return !(*this == rhs);
            }

        protected:
            bidirectional_iterator(FeldmanHashSet& set, array_node * pNode, size_t idx, bool)
                : iterator_base(set, pNode, idx, false)
            {}

            bidirectional_iterator(FeldmanHashSet& set, array_node * pNode, size_t idx)
                : iterator_base(set, pNode, idx)
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
            reverse_bidirectional_iterator() CDS_NOEXCEPT
                : iterator_base()
            {}

            reverse_bidirectional_iterator(reverse_bidirectional_iterator const& rhs) CDS_NOEXCEPT
                : iterator_base(rhs)
            {}

            reverse_bidirectional_iterator& operator=(reverse_bidirectional_iterator const& rhs) CDS_NOEXCEPT
            {
                iterator_base::operator=(rhs);
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

            value_ptr operator ->() const CDS_NOEXCEPT
            {
                return iterator_base::pointer();
            }

            value_ref operator *() const CDS_NOEXCEPT
            {
                value_ptr p = iterator_base::pointer();
                assert(p);
                return *p;
            }

            template <bool IsConst2>
            bool operator ==(reverse_bidirectional_iterator<IsConst2> const& rhs) const
            {
                return iterator_base::operator==(rhs);
            }

            template <bool IsConst2>
            bool operator !=(reverse_bidirectional_iterator<IsConst2> const& rhs)
            {
                return !(*this == rhs);
            }

        private:
            reverse_bidirectional_iterator(FeldmanHashSet& set, array_node * pNode, size_t idx, bool)
                : iterator_base(set, pNode, idx, false)
            {}

            reverse_bidirectional_iterator(FeldmanHashSet& set, array_node * pNode, size_t idx)
                : iterator_base(set, pNode, idx, false)
            {
                iterator_base::backward();
            }
        };
        //@endcond

    public:
#ifdef CDS_DOXYGEN_INVOKED
        typedef implementation_defined iterator;            ///< @ref cds_intrusive_FeldmanHashSet_rcu_iterators "bidirectional iterator" type
        typedef implementation_defined const_iterator;      ///< @ref cds_intrusive_FeldmanHashSet_rcu_iterators "bidirectional const iterator" type
        typedef implementation_defined reverse_iterator;    ///< @ref cds_intrusive_FeldmanHashSet_rcu_iterators "bidirectional reverse iterator" type
        typedef implementation_defined const_reverse_iterator; ///< @ref cds_intrusive_FeldmanHashSet_rcu_iterators "bidirectional reverse const iterator" type
#else
        typedef bidirectional_iterator<false>   iterator;
        typedef bidirectional_iterator<true>    const_iterator;
        typedef reverse_bidirectional_iterator<false>   reverse_iterator;
        typedef reverse_bidirectional_iterator<true>    const_reverse_iterator;
#endif

        ///@name Thread-safe iterators
        /** @anchor cds_intrusive_FeldmanHashSet_rcu_iterators
            The set supports thread-safe iterators: you may iterate over the set in multi-threaded environment
            under explicit RCU lock.
            RCU lock requirement means that inserting or searching is allowed but you must not erase the items from the set
            since erasing under RCU lock can lead to a deadlock. However, another thread can call \p erase() safely
            while your thread is iterating.

            A typical example is:
            \code
            struct foo {
                uint32_t    hash;
                // ... other fields
                uint32_t    payload; // only for example
            };
            struct set_traits: cds::intrusive::feldman_hashset::traits
            {
                struct hash_accessor {
                    uint32_t operator()( foo const& src ) const
                    {
                        retur src.hash;
                    }
                };
            };

            typedef cds::urcu::gc< cds::urcu::general_buffered<>> rcu;
            typedef cds::intrusive::FeldmanHashSet< rcu, foo, set_traits > set_type;

            set_type s;

            // ...

            // iterate over the set
            {
                // lock the RCU.
                typename set_type::rcu_lock l; // scoped RCU lock

                // traverse the set
                for ( auto i = s.begin(); i != s.end(); ++i ) {
                    // deal with i. Remember, erasing is prohibited here!
                    i->payload++;
                }
            } // at this point RCU lock is released
            /endcode

            Each iterator object supports the common interface:
            - dereference operators:
                @code
                value_type [const] * operator ->() noexcept
                value_type [const] & operator *() noexcept
                @endcode
            - pre-increment and pre-decrement. Post-operators is not supported
            - equality operators <tt>==</tt> and <tt>!=</tt>.
                Iterators are equal iff they point to the same cell of the same array node.
                Note that for two iterators \p it1 and \p it2 the condition <tt> it1 == it2 </tt>
                does not entail <tt> &(*it1) == &(*it2) </tt>: welcome to concurrent containers

            @note It is possible the item can be iterated more that once, for example, if an iterator points to the item
            in an array node that is being splitted.
        */
        ///@{

        /// Returns an iterator to the beginning of the set
        iterator begin()
        {
            return iterator(*this, m_Head, size_t(0) - 1);
        }

        /// Returns an const iterator to the beginning of the set
        const_iterator begin() const
        {
            return const_iterator(*this, m_Head, size_t(0) - 1);
        }

        /// Returns an const iterator to the beginning of the set
        const_iterator cbegin()
        {
            return const_iterator(*this, m_Head, size_t(0) - 1);
        }

        /// Returns an iterator to the element following the last element of the set. This element acts as a placeholder; attempting to access it results in undefined behavior.
        iterator end()
        {
            return iterator(*this, m_Head, head_size(), false);
        }

        /// Returns a const iterator to the element following the last element of the set. This element acts as a placeholder; attempting to access it results in undefined behavior.
        const_iterator end() const
        {
            return const_iterator(*this, m_Head, head_size(), false);
        }

        /// Returns a const iterator to the element following the last element of the set. This element acts as a placeholder; attempting to access it results in undefined behavior.
        const_iterator cend()
        {
            return const_iterator(*this, m_Head, head_size(), false);
        }

        /// Returns a reverse iterator to the first element of the reversed set
        reverse_iterator rbegin()
        {
            return reverse_iterator(*this, m_Head, head_size());
        }

        /// Returns a const reverse iterator to the first element of the reversed set
        const_reverse_iterator rbegin() const
        {
            return const_reverse_iterator(*this, m_Head, head_size());
        }

        /// Returns a const reverse iterator to the first element of the reversed set
        const_reverse_iterator crbegin()
        {
            return const_reverse_iterator(*this, m_Head, head_size());
        }

        /// Returns a reverse iterator to the element following the last element of the reversed set
        /**
        It corresponds to the element preceding the first element of the non-reversed container.
        This element acts as a placeholder, attempting to access it results in undefined behavior.
        */
        reverse_iterator rend()
        {
            return reverse_iterator(*this, m_Head, size_t(0) - 1, false);
        }

        /// Returns a const reverse iterator to the element following the last element of the reversed set
        /**
        It corresponds to the element preceding the first element of the non-reversed container.
        This element acts as a placeholder, attempting to access it results in undefined behavior.
        */
        const_reverse_iterator rend() const
        {
            return const_reverse_iterator(*this, m_Head, size_t(0) - 1, false);
        }

        /// Returns a const reverse iterator to the element following the last element of the reversed set
        /**
        It corresponds to the element preceding the first element of the non-reversed container.
        This element acts as a placeholder, attempting to access it results in undefined behavior.
        */
        const_reverse_iterator crend()
        {
            return const_reverse_iterator(*this, m_Head, size_t(0) - 1, false);
        }
        ///@}

    protected:
        //@cond
        template <typename Func>
        std::pair<bool, bool> do_update(value_type& val, Func f, bool bInsert = true)
        {
            hash_type const& hash = hash_accessor()(val);
            hash_splitter splitter(hash);
            hash_comparator cmp;
            back_off bkoff;

            array_node * pArr = m_Head;
            size_t nSlot = splitter.cut(m_Metrics.head_node_size_log);
            assert(nSlot < m_Metrics.head_node_size);
            size_t nOffset = m_Metrics.head_node_size_log;
            size_t nHeight = 1;

            while (true) {
                node_ptr slot = pArr->nodes[nSlot].load(memory_model::memory_order_acquire);
                if (slot.bits() == flag_array_node) {
                    // array node, go down the tree
                    assert(slot.ptr() != nullptr);
                    nSlot = splitter.cut(m_Metrics.array_node_size_log);
                    assert(nSlot < m_Metrics.array_node_size);
                    pArr = to_array(slot.ptr());
                    nOffset += m_Metrics.array_node_size_log;
                    ++nHeight;
                }
                else if (slot.bits() == flag_array_converting) {
                    // the slot is converting to array node right now
                    bkoff();
                    m_Stat.onSlotConverting();
                }
                else {
                    // data node
                    assert(slot.bits() == 0);

                    value_type * pOld = nullptr;
                    {
                        rcu_lock rcuLock;

                        if ( pArr->nodes[nSlot].load(memory_model::memory_order_acquire) == slot ) {
                            if ( slot.ptr()) {
                                if (cmp(hash, hash_accessor()(*slot.ptr())) == 0) {
                                    // the item with that hash value already exists
                                    // Replace it with val
                                    if (slot.ptr() == &val) {
                                        m_Stat.onUpdateExisting();
                                        return std::make_pair(true, false);
                                    }

                                    if (pArr->nodes[nSlot].compare_exchange_strong(slot, node_ptr(&val), memory_model::memory_order_release, atomics::memory_order_relaxed)) {
                                        // slot can be disposed
                                        f(val, slot.ptr());
                                        pOld = slot.ptr();
                                        m_Stat.onUpdateExisting();
                                        goto update_existing_done;
                                    }

                                    m_Stat.onUpdateRetry();
                                    continue;
                                }

                                // the slot must be expanded
                                expand_slot(pArr, nSlot, slot, nOffset);
                            }
                            else {
                                // the slot is empty, try to insert data node
                                if (bInsert) {
                                    node_ptr pNull;
                                    if (pArr->nodes[nSlot].compare_exchange_strong(pNull, node_ptr(&val), memory_model::memory_order_release, atomics::memory_order_relaxed))
                                    {
                                        // the new data node has been inserted
                                        f(val, nullptr);
                                        ++m_ItemCounter;
                                        m_Stat.onUpdateNew();
                                        m_Stat.height(nHeight);
                                        return std::make_pair(true, true);
                                    }
                                }
                                else {
                                    m_Stat.onUpdateFailed();
                                    return std::make_pair(false, false);
                                }

                                // insert failed - slot has been changed by another thread
                                // retry updating
                                m_Stat.onUpdateRetry();
                            }
                        }
                        else
                            m_Stat.onSlotChanged();
                        continue;
                    } // rcu_lock

                    // update success
        update_existing_done:
                    if ( pOld )
                        gc::template retire_ptr<disposer>( pOld );
                    return std::make_pair(true, false);
                } 
            } // while
        }

        template <typename Predicate>
        value_type * do_erase( hash_type const& hash, Predicate pred)
        {
            assert(gc::is_locked());

            hash_splitter splitter(hash);
            hash_comparator cmp;
            back_off bkoff;

            array_node * pArr = m_Head;
            size_t nSlot = splitter.cut(m_Metrics.head_node_size_log);
            assert(nSlot < m_Metrics.head_node_size);

            while (true) {
                node_ptr slot = pArr->nodes[nSlot].load(memory_model::memory_order_acquire);
                if (slot.bits() == flag_array_node) {
                    // array node, go down the tree
                    assert(slot.ptr() != nullptr);
                    nSlot = splitter.cut(m_Metrics.array_node_size_log);
                    assert(nSlot < m_Metrics.array_node_size);
                    pArr = to_array(slot.ptr());
                }
                else if (slot.bits() == flag_array_converting) {
                    // the slot is converting to array node right now
                    bkoff();
                    m_Stat.onSlotConverting();
                }
                else {
                    // data node
                    assert(slot.bits() == 0);

                    if ( pArr->nodes[nSlot].load(memory_model::memory_order_acquire) == slot ) {
                        if (slot.ptr()) {
                            if (cmp(hash, hash_accessor()(*slot.ptr())) == 0 && pred(*slot.ptr())) {
                                // item found - replace it with nullptr
                                if (pArr->nodes[nSlot].compare_exchange_strong(slot, node_ptr(nullptr), memory_model::memory_order_acquire, atomics::memory_order_relaxed)) {
                                    --m_ItemCounter;
                                    m_Stat.onEraseSuccess();

                                    return slot.ptr();
                                }
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
                    else
                        m_Stat.onSlotChanged();
                }
            } // while
        }

        value_type * search(hash_type const& hash )
        {
            assert( gc::is_locked() );

            hash_splitter splitter(hash);
            hash_comparator cmp;
            back_off bkoff;

            array_node * pArr = m_Head;
            size_t nSlot = splitter.cut(m_Metrics.head_node_size_log);
            assert(nSlot < m_Metrics.head_node_size);

            while (true) {
                node_ptr slot = pArr->nodes[nSlot].load(memory_model::memory_order_acquire);
                if (slot.bits() == flag_array_node) {
                    // array node, go down the tree
                    assert(slot.ptr() != nullptr);
                    nSlot = splitter.cut(m_Metrics.array_node_size_log);
                    assert(nSlot < m_Metrics.array_node_size);
                    pArr = to_array(slot.ptr());
                }
                else if (slot.bits() == flag_array_converting) {
                    // the slot is converting to array node right now
                    bkoff();
                    m_Stat.onSlotConverting();
                }
                else {
                    // data node
                    assert(slot.bits() == 0);

                    // protect data node by hazard pointer
                    if ( pArr->nodes[nSlot].load(memory_model::memory_order_acquire) != slot) {
                        // slot value has been changed - retry
                        m_Stat.onSlotChanged();
                    }
                    else if (slot.ptr() && cmp(hash, hash_accessor()(*slot.ptr())) == 0) {
                        // item found
                        m_Stat.onFindSuccess();
                        return slot.ptr();
                    }
                    m_Stat.onFindFailed();
                    return nullptr;
                }
            } // while
        }

        //@endcond

    private:
        //@cond
        array_node * alloc_head_node() const
        {
            return alloc_array_node(head_size(), nullptr, 0);
        }

        array_node * alloc_array_node(array_node * pParent, size_t idxParent) const
        {
            return alloc_array_node(array_node_size(), pParent, idxParent);
        }

        static array_node * alloc_array_node(size_t nSize, array_node * pParent, size_t idxParent)
        {
            array_node * pNode = cxx_array_node_allocator().NewBlock(sizeof(array_node) + sizeof(atomic_node_ptr) * (nSize - 1), pParent, idxParent);
            new (pNode->nodes) atomic_node_ptr[nSize];
            return pNode;
        }

        static void free_array_node(array_node * parr)
        {
            cxx_array_node_allocator().Delete(parr);
        }

        void destroy_tree()
        {
            // The function is not thread-safe. For use in dtor only
            // Remove data node
            clear();

            // Destroy all array nodes
            destroy_array_nodes(m_Head, head_size());
        }

        void destroy_array_nodes(array_node * pArr, size_t nSize)
        {
            for (atomic_node_ptr * p = pArr->nodes, *pLast = pArr->nodes + nSize; p != pLast; ++p) {
                node_ptr slot = p->load(memory_model::memory_order_acquire);
                if (slot.bits() == flag_array_node) {
                    destroy_array_nodes(to_array(slot.ptr()), array_node_size());
                    free_array_node(to_array(slot.ptr()));
                    p->store(node_ptr(), memory_model::memory_order_relaxed);
                }
            }
        }

        void clear_array(array_node * pArrNode, size_t nSize)
        {
            back_off bkoff;


            for (atomic_node_ptr * pArr = pArrNode->nodes, *pLast = pArr + nSize; pArr != pLast; ++pArr) {
                while (true) {
                    node_ptr slot = pArr->load(memory_model::memory_order_acquire);
                    if (slot.bits() == flag_array_node) {
                        // array node, go down the tree
                        assert(slot.ptr() != nullptr);
                        clear_array(to_array(slot.ptr()), array_node_size());
                        break;
                    }
                    else if (slot.bits() == flag_array_converting) {
                        // the slot is converting to array node right now
                        while ((slot = pArr->load(memory_model::memory_order_acquire)).bits() == flag_array_converting) {
                            bkoff();
                            m_Stat.onSlotConverting();
                        }
                        bkoff.reset();

                        assert(slot.ptr() != nullptr);
                        assert(slot.bits() == flag_array_node);
                        clear_array(to_array(slot.ptr()), array_node_size());
                        break;
                    }
                    else {
                        // data node
                        if (pArr->compare_exchange_strong(slot, node_ptr(), memory_model::memory_order_acquire, atomics::memory_order_relaxed)) {
                            if (slot.ptr()) {
                                gc::template retire_ptr<disposer>(slot.ptr());
                                --m_ItemCounter;
                                m_Stat.onEraseSuccess();
                            }
                            break;
                        }
                    }
                }
            }
        }

        bool expand_slot(array_node * pParent, size_t idxParent, node_ptr current, size_t nOffset)
        {
            assert(current.bits() == 0);
            assert(current.ptr());

            size_t idx = hash_splitter(hash_accessor()(*current.ptr()), nOffset).cut(m_Metrics.array_node_size_log);
            array_node * pArr = alloc_array_node(pParent, idxParent);

            node_ptr cur(current.ptr());
            atomic_node_ptr& slot = pParent->nodes[idxParent];
            if (!slot.compare_exchange_strong(cur, cur | flag_array_converting, memory_model::memory_order_release, atomics::memory_order_relaxed))
            {
                m_Stat.onExpandNodeFailed();
                free_array_node(pArr);
                return false;
            }

            pArr->nodes[idx].store(current, memory_model::memory_order_release);

            cur = cur | flag_array_converting;
            CDS_VERIFY(
                slot.compare_exchange_strong(cur, node_ptr(to_node(pArr), flag_array_node), memory_model::memory_order_release, atomics::memory_order_relaxed)
                );

            m_Stat.onExpandNodeSuccess();
            m_Stat.onArrayNodeCreated();
            return true;
        }

        union converter {
            value_type * pData;
            array_node * pArr;

            converter(value_type * p)
                : pData(p)
            {}

            converter(array_node * p)
                : pArr(p)
            {}
        };

        static array_node * to_array(value_type * p)
        {
            return converter(p).pArr;
        }
        static value_type * to_node(array_node * p)
        {
            return converter(p).pData;
        }
        //@endcond
    };

}} // namespace cds::intrusive

#endif  // #ifndef CDSLIB_INTRUSIVE_FELDMAN_HASHSET_RCU_H
