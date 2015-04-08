//$$CDS-header$$

#ifndef CDSLIB_CONTAINER_MICHAEL_KVLIST_RCU_H
#define CDSLIB_CONTAINER_MICHAEL_KVLIST_RCU_H

#include <memory>
#include <functional>   // ref
#include <cds/container/details/michael_list_base.h>
#include <cds/intrusive/michael_list_rcu.h>
#include <cds/container/details/make_michael_kvlist.h>

namespace cds { namespace container {

    /// Michael's ordered list (key-value pair), template specialization for \ref cds_urcu_desc "RCU"
    /** @ingroup cds_nonintrusive_list
        \anchor cds_nonintrusive_MichaelKVList_rcu

        This is key-value variation of non-intrusive \ref cds_nonintrusive_MichaelList_rcu "MichaelList".
        Like standard container, this implementation split a value stored into two part -
        constant key and alterable value.

        Usually, ordered single-linked list is used as a building block for the hash table implementation.
        The complexity of searching is <tt>O(N)</tt>.

        Template arguments:
        - \p RCU - one of \ref cds_urcu_gc "RCU type"
        - \p Key - key type of an item stored in the list. It should be copy-constructible
        - \p Value - value type stored in a list
        - \p Traits - type traits, default is \p michael_list::traits

        @note Before including <tt><cds/container/michael_kvlist_rcu.h></tt> you should include appropriate RCU header file,
        see \ref cds_urcu_gc "RCU type" for list of existing RCU class and corresponding header files.

        It is possible to declare option-based list using \p cds::container::michael_list::make_traits metafunction istead of \p Traits template
        argument. For example, the following traits-based declaration of Michael's list
        \code
        #include <cds/urcu/general_buffered.h>
        #include <cds/container/michael_kvlist_rcu.h>
        // Declare comparator for the item
        struct my_compare {
            int operator ()( int i1, int i2 )
            {
                return i1 - i2;
            }
        };

        // Declare traits
        struct my_traits: public cds::container::michael_list::traits
        {
            typedef my_compare compare;
        };

        // Declare traits-based list
        typedef cds::container::MichaelKVList< cds::urcu::gc< cds::urcu::general_buffered<> >, int, int, my_traits > traits_based_list;
        \endcode

        is equivalent for the following option-based list
        \code
        #include <cds/urcu/general_buffered.h>
        #include <cds/container/michael_kvlist_rcu.h>

        // my_compare is the same

        // Declare option-based list
        typedef cds::container::MichaelKVList< cds::urcu::gc< cds::urcu::general_buffered<> >, int, int,
            typename cds::container::michael_list::make_traits<
                cds::container::opt::compare< my_compare >     // item comparator option
            >::type
        >     option_based_list;
        \endcode
    */
    template <
        typename RCU,
        typename Key,
        typename Value,
#ifdef CDS_DOXYGEN_INVOKED
        typename Traits = michael_list::traits
#else
        typename Traits
#endif
    >
    class MichaelKVList< cds::urcu::gc<RCU>, Key, Value, Traits >:
#ifdef CDS_DOXYGEN_INVOKED
        protected intrusive::MichaelList< cds::urcu::gc<RCU>, implementation_defined, Traits >
#else
        protected details::make_michael_kvlist< cds::urcu::gc<RCU>, Key, Value, Traits >::type
#endif
    {
        //@cond
        typedef details::make_michael_kvlist< cds::urcu::gc<RCU>, Key, Value, Traits > maker;
        typedef typename maker::type  base_class;
        //@endcond

    public:
        typedef cds::urcu::gc<RCU>  gc;   ///< Garbage collector

#ifdef CDS_DOXYGEN_INVOKED
        typedef Key                                 key_type;      ///< Key type
        typedef Value                               mapped_type;   ///< Type of value stored in the list
        typedef std::pair<key_type const, mapped_type> value_type; ///< key/value pair stored in the list
#else
        typedef typename maker::key_type   key_type;
        typedef typename maker::value_type mapped_type;
        typedef typename maker::pair_type  value_type;
#endif
        typedef Traits traits; ///< List traits

        typedef typename base_class::back_off       back_off;       ///< Back-off strategy
        typedef typename maker::allocator_type    allocator_type;   ///< Allocator type used for allocate/deallocate the nodes
        typedef typename base_class::item_counter   item_counter;   ///< Item counting policy
        typedef typename maker::key_comparator    key_comparator;   ///< key comparison functor
        typedef typename base_class::memory_model   memory_model;   ///< Memory ordering. See \p michael_list::traits::memory_model
        typedef typename base_class::rcu_check_deadlock rcu_check_deadlock ; ///< RCU deadlock checking policy

        typedef typename gc::scoped_lock    rcu_lock ;  ///< RCU scoped lock
        static CDS_CONSTEXPR const bool c_bExtractLockExternal = base_class::c_bExtractLockExternal; ///< Group of \p extract_xxx functions require external locking

    protected:
        //@cond
        typedef typename base_class::value_type     node_type;
        typedef typename maker::cxx_allocator     cxx_allocator;
        typedef typename maker::node_deallocator  node_deallocator;
        typedef typename maker::intrusive_traits::compare  intrusive_key_comparator;

        typedef typename base_class::atomic_node_ptr head_type;
        //@endcond

    public:
        /// pointer to extracted node
        using exempt_ptr = cds::urcu::exempt_ptr< gc, node_type, value_type, typename maker::intrusive_traits::disposer,
            cds::urcu::details::conventional_exempt_pair_cast<node_type, value_type>
        >;

    protected:
        //@cond
        template <typename K>
        static node_type * alloc_node(const K& key)
        {
            return cxx_allocator().New( key );
        }

        template <typename K, typename V>
        static node_type * alloc_node( const K& key, const V& val )
        {
            return cxx_allocator().New( key, val );
        }

        template <typename K, typename... Args>
        static node_type * alloc_node( K&& key, Args&&... args )
        {
            return cxx_allocator().MoveNew( std::forward<K>(key), std::forward<Args>(args)...);
        }

        static void free_node( node_type * pNode )
        {
            cxx_allocator().Delete( pNode );
        }

        struct node_disposer {
            void operator()( node_type * pNode )
            {
                free_node( pNode );
            }
        };
        typedef std::unique_ptr< node_type, node_disposer >     scoped_node_ptr;

        head_type& head()
        {
            return base_class::m_pHead;
        }

        head_type& head() const
        {
            return const_cast<head_type&>( base_class::m_pHead );
        }
        //@endcond

    protected:
        //@cond
        template <bool IsConst>
        class iterator_type: protected base_class::template iterator_type<IsConst>
        {
            typedef typename base_class::template iterator_type<IsConst>    iterator_base;

            iterator_type( head_type const& pNode )
                : iterator_base( pNode )
            {}

            friend class MichaelKVList;

        public:
            typedef typename cds::details::make_const_type<mapped_type, IsConst>::reference  value_ref;
            typedef typename cds::details::make_const_type<mapped_type, IsConst>::pointer    value_ptr;

            typedef typename cds::details::make_const_type<value_type,  IsConst>::reference  pair_ref;
            typedef typename cds::details::make_const_type<value_type,  IsConst>::pointer    pair_ptr;

            iterator_type()
            {}

            iterator_type( iterator_type const& src )
                : iterator_base( src )
            {}

            key_type const& key() const
            {
                typename iterator_base::value_ptr p = iterator_base::operator ->();
                assert( p != nullptr );
                return p->m_Data.first;
            }

            pair_ptr operator ->() const
            {
                typename iterator_base::value_ptr p = iterator_base::operator ->();
                return p ? &(p->m_Data) : nullptr;
            }

            pair_ref operator *() const
            {
                typename iterator_base::value_ref p = iterator_base::operator *();
                return p.m_Data;
            }

            value_ref val() const
            {
                typename iterator_base::value_ptr p = iterator_base::operator ->();
                assert( p != nullptr );
                return p->m_Data.second;
            }

            /// Pre-increment
            iterator_type& operator ++()
            {
                iterator_base::operator ++();
                return *this;
            }

            template <bool C>
            bool operator ==(iterator_type<C> const& i ) const
            {
                return iterator_base::operator ==(i);
            }
            template <bool C>
            bool operator !=(iterator_type<C> const& i ) const
            {
                return iterator_base::operator !=(i);
            }
        };
        //@endcond

    public:
        /// Forward iterator
        typedef iterator_type<false>    iterator;

        /// Const forward iterator
        typedef iterator_type<true>     const_iterator;

        /// Returns a forward iterator addressing the first element in a list
        /**
            For empty list \code begin() == end() \endcode
        */
        iterator begin()
        {
            return iterator( head() );
        }

        /// Returns an iterator that addresses the location succeeding the last element in a list
        /**
            Do not use the value returned by <tt>end</tt> function to access any item.
            Internally, <tt>end</tt> returning value equals to \p nullptr.

            The returned value can be used only to control reaching the end of the list.
            For empty list \code begin() == end() \endcode
        */
        iterator end()
        {
            return iterator();
        }

        /// Returns a forward const iterator addressing the first element in a list
        //@{
        const_iterator begin() const
        {
            return const_iterator( head() );
        }
        const_iterator cbegin() const
        {
            return const_iterator( head() );
        }
        //@}

        /// Returns an const iterator that addresses the location succeeding the last element in a list
        //@{
        const_iterator end() const
        {
            return const_iterator();
        }
        const_iterator cend() const
        {
            return const_iterator();
        }
        //@}

    public:
        /// Default constructor
        /**
            Initializes empty list
        */
        MichaelKVList()
        {}

        /// List desctructor
        /**
            Clears the list
        */
        ~MichaelKVList()
        {
            clear();
        }

        /// Inserts new node with key and default value
        /**
            The function creates a node with \p key and default value, and then inserts the node created into the list.

            Preconditions:
            - The \ref key_type should be constructible from value of type \p K.
                In trivial case, \p K is equal to \ref key_type.
            - The \ref mapped_type should be default-constructible.

            The function makes RCU lock internally.

            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename K>
        bool insert( const K& key )
        {
            return insert_at( head(), key );
        }

        /// Inserts new node with a key and a value
        /**
            The function creates a node with \p key and value \p val, and then inserts the node created into the list.

            Preconditions:
            - The \ref key_type should be constructible from \p key of type \p K.
            - The \ref mapped_type should be constructible from \p val of type \p V.

            The function makes RCU lock internally.

            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename K, typename V>
        bool insert( const K& key, const V& val )
        {
            return insert_at( head(), key, val );
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
            to the list's item inserted. <tt>item.second</tt> is a reference to item's value that may be changed.
            User-defined functor \p func should guarantee that during changing item's value no any other changes
            could be made on this list's item by concurrent threads.

            The key_type should be constructible from value of type \p K.

            The function allows to split creating of new item into two part:
            - create item from \p key;
            - insert new item into the list;
            - if inserting is successful, initialize the value of item by calling \p func functor

            This can be useful if complete initialization of object of \p mapped_type is heavyweight and
            it is preferable that the initialization should be completed only if inserting is successful.

            The function makes RCU lock internally.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"
        */
        template <typename K, typename Func>
        bool insert_with( const K& key, Func func )
        {
            return insert_with_at( head(), key, func );
        }

        /// Ensures that the \p key exists in the list
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the \p key not found in the list, then the new item created from \p key
            is inserted into the list (note that in this case the \ref key_type should be
            copy-constructible from type \p K).
            Otherwise, the functor \p func is called with item found.
            The functor \p Func may be a function with signature:
            \code
                void func( bool bNew, value_type& item );
            \endcode
            or a functor:
            \code
                struct my_functor {
                    void operator()( bool bNew, value_type& item );
                };
            \endcode

            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the list

            The functor may change any fields of the \p item.second that is \ref mapped_type;
            however, \p func must guarantee that during changing no any other modifications
            could be made on this item by concurrent threads.

            The function makes RCU lock internally.

            Returns <tt> std::pair<bool, bool> </tt> where \p first is true if operation is successfull,
            \p second is true if new item has been added or \p false if the item with \p key
            already is in the list.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"
        */
        template <typename K, typename Func>
        std::pair<bool, bool> ensure( const K& key, Func f )
        {
            return ensure_at( head(), key, f );
        }

        /// Inserts data of type \ref mapped_type constructed with <tt>std::forward<Args>(args)...</tt>
        /**
            Returns \p true if inserting successful, \p false otherwise.

            The function makes RCU lock internally.
        */
        template <typename K, typename... Args>
        bool emplace( K&& key, Args&&... args )
        {
            return emplace_at( head(), std::forward<K>(key), std::forward<Args>(args)... );
        }

        /// Deletes \p key from the list
        /** \anchor cds_nonintrusive_MichaelKVList_rcu_erase

            RCU \p synchronize method can be called. RCU should not be locked.

            Returns \p true if \p key is found and has been deleted, \p false otherwise
        */
        template <typename K>
        bool erase( K const& key )
        {
            return erase_at( head(), key, intrusive_key_comparator() );
        }

        /// Deletes the item from the list using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_MichaelKVList_rcu_erase "erase(K const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename K, typename Less>
        bool erase_with( K const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return erase_at( head(), key, typename maker::template less_wrapper<Less>::type() );
        }

        /// Deletes \p key from the list
        /** \anchor cds_nonintrusive_MichaelKVList_rcu_erase_func
            The function searches an item with key \p key, calls \p f functor
            and deletes the item. If \p key is not found, the functor is not called.

            The functor \p Func interface:
            \code
            struct extractor {
                void operator()(value_type& val) { ... }
            };
            \endcode

            RCU \p synchronize method can be called. RCU should not be locked.

            Return \p true if key is found and deleted, \p false otherwise

            See also: \ref erase
        */
        template <typename K, typename Func>
        bool erase( K const& key, Func f )
        {
            return erase_at( head(), key, intrusive_key_comparator(), f );
        }

        /// Deletes the item from the list using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_MichaelKVList_rcu_erase_func "erase(K const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename K, typename Less, typename Func>
        bool erase_with( K const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return erase_at( head(), key, typename maker::template less_wrapper<Less>::type(), f );
        }

        /// Extracts an item from the list
        /**
        @anchor cds_nonintrusive_MichaelKVList_rcu_extract
            The function searches an item with key equal to \p key in the list,
            unlinks it from the list, and returns \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to the item found.
            If \p key is not found the function returns an empty \p exempt_ptr.

            @note The function does NOT call RCU read-side lock or synchronization,
            and does NOT dispose the item found. It just excludes the item from the list
            and returns a pointer to item found.
            You should lock RCU before calling this function.

            \code
            #include <cds/urcu/general_buffered.h>
            #include <cds/container/michael_kvlist_rcu.h>

            typedef cds::urcu::gc< general_buffered<> > rcu;
            typedef cds::container::MichaelKVList< rcu, int, Foo > rcu_michael_list;

            rcu_michael_list theList;
            // ...

            rcu_michael_list::exempt_ptr p;
            {
                // first, we should lock RCU
                rcu_michael_list::rcu_lock sl;

                // Now, you can apply extract function
                // Note that you must not delete the item found inside the RCU lock
                p = theList.extract( 10 );
                if ( p ) {
                    // do something with p
                    ...
                }
            }
            // Outside RCU lock section we may safely release extracted pointer.
            // release() passes the pointer to RCU reclamation cycle.
            p.release();
            \endcode
        */
        template <typename K>
        exempt_ptr extract( K const& key )
        {
            return exempt_ptr( extract_at( head(), key, intrusive_key_comparator() ));
        }

        /// Extracts an item from the list using \p pred predicate for searching
        /**
            This function is the analog for \p extract(K const&).
            The \p pred is a predicate used for key comparing.
            \p Less has the interface like \p std::less.
            \p pred must imply the same element order as \ref key_comparator.
        */
        template <typename K, typename Less>
        exempt_ptr extract_with( K const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return exempt_ptr( extract_at( head(), key, typename maker::template less_wrapper<Less>::type() ));
        }

        /// Finds the key \p key
        /** \anchor cds_nonintrusive_MichaelKVList_rcu_find_val

            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise

            The function makes RCU lock internally.
        */
        template <typename Q>
        bool find( Q const& key ) const
        {
            return find_at( head(), key, intrusive_key_comparator() );
        }

        /// Finds the key \p key using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_MichaelKVList_rcu_find_val "find(Q const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        bool find_with( Q const& key, Less pred ) const
        {
            CDS_UNUSED( pred );
            return find_at( head(), key, typename maker::template less_wrapper<Less>::type() );
        }

        /// Finds \p key and performs an action with it
        /** \anchor cds_nonintrusive_MichaelKVList_rcu_find_func
            The function searches an item with key equal to \p key and calls the functor \p f for the item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item );
            };
            \endcode
            where \p item is the item found.

            The functor may change <tt>item.second</tt> that is reference to value of node.
            Note that the function is only guarantee that \p item cannot be deleted during functor is executing.
            The function does not serialize simultaneous access to the list \p item. If such access is
            possible you must provide your own synchronization schema to exclude unsafe item modifications.

            The function makes RCU lock internally.

            The function returns \p true if \p key is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q const& key, Func f ) const
        {
            return find_at( head(), key, intrusive_key_comparator(), f );
        }

        /// Finds the key \p val using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_MichaelKVList_rcu_find_func "find(Q const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& key, Less pred, Func f ) const
        {
            CDS_UNUSED( pred );
            return find_at( head(), key, typename maker::template less_wrapper<Less>::type(), f );
        }

        /// Finds \p key and return the item found
        /** \anchor cds_nonintrusive_MichaelKVList_rcu_get
            The function searches the item with \p key and returns the pointer to item found.
            If \p key is not found it returns \p nullptr.

            Note the compare functor should accept a parameter of type \p K that can be not the same as \p key_type.

            RCU should be locked before call of this function.
            Returned item is valid only while RCU is locked:
            \code
            typedef cds::container::MichaelKVList< cds::urcu::gc< cds::urcu::general_buffered<> >, int, foo, my_traits > ord_list;
            ord_list theList;
            // ...
            {
                // Lock RCU
                ord_list::rcu_lock lock;

                ord_list::value_type * pVal = theList.get( 5 );
                if ( pVal ) {
                    // Deal with pVal
                    //...
                }
                // Unlock RCU by rcu_lock destructor
                // pVal can be freed at any time after RCU has been unlocked
            }
            \endcode
        */
        template <typename K>
        value_type * get( K const& key ) const
        {
            return get_at( head(), key, intrusive_key_comparator());
        }

        /// Finds \p key and return the item found
        /**
            The function is an analog of \ref cds_nonintrusive_MichaelKVList_rcu_get "get(K const&)"
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref key_type and \p K
            in any order.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename K, typename Less>
        value_type * get_with( K const& key, Less pred ) const
        {
            CDS_UNUSED( pred );
            return get_at( head(), key, typename maker::template less_wrapper<Less>::type() );
        }

        /// Checks if the list is empty
        bool empty() const
        {
            return base_class::empty();
        }

        /// Returns list's item count
        /**
            The value returned depends on item counter provided by \p Traits. For \p atomicity::empty_item_counter,
            this function always returns 0.

            @note Even if you use real item counter and it returns 0, this fact does not mean that the list
            is empty. To check list emptyness use \p empty() method.
        */
        size_t size() const
        {
            return base_class::size();
        }

        /// Clears the list
        /**
            Post-condition: the list is empty
        */
        void clear()
        {
            base_class::clear();
        }

    protected:
        //@cond
        bool insert_node_at( head_type& refHead, node_type * pNode )
        {
            assert( pNode != nullptr );
            scoped_node_ptr p( pNode );
            if ( base_class::insert_at( refHead, *pNode )) {
                p.release();
                return true;
            }
            return false;
        }

        template <typename K>
        bool insert_at( head_type& refHead, const K& key )
        {
            return insert_node_at( refHead, alloc_node( key ));
        }

        template <typename K, typename V>
        bool insert_at( head_type& refHead, const K& key, const V& val )
        {
            return insert_node_at( refHead, alloc_node( key, val ));
        }

        template <typename K, typename Func>
        bool insert_with_at( head_type& refHead, const K& key, Func f )
        {
            scoped_node_ptr pNode( alloc_node( key ));

            if ( base_class::insert_at( refHead, *pNode, [&f](node_type& node){ f( node.m_Data ); })) {
                pNode.release();
                return true;
            }
            return false;
        }

        template <typename K, typename... Args>
        bool emplace_at( head_type& refHead, K&& key, Args&&... args )
        {
            return insert_node_at( refHead, alloc_node( std::forward<K>(key), std::forward<Args>(args)... ));
        }

        template <typename K, typename Func>
        std::pair<bool, bool> ensure_at( head_type& refHead, const K& key, Func f )
        {
            scoped_node_ptr pNode( alloc_node( key ));

            std::pair<bool, bool> ret = base_class::ensure_at( refHead, *pNode,
                [&f]( bool bNew, node_type& node, node_type& ){ f( bNew, node.m_Data ); });
            if ( ret.first && ret.second )
                pNode.release();

            return ret;
        }

        template <typename K, typename Compare>
        bool erase_at( head_type& refHead, K const& key, Compare cmp )
        {
            return base_class::erase_at( refHead, key, cmp );
        }

        template <typename K, typename Compare, typename Func>
        bool erase_at( head_type& refHead, K const& key, Compare cmp, Func f )
        {
            return base_class::erase_at( refHead, key, cmp, [&f]( node_type const & node ){ f( const_cast<value_type&>(node.m_Data)); });
        }

        template <typename K, typename Compare>
        node_type * extract_at( head_type& refHead, K const& key, Compare cmp )
        {
            return base_class::extract_at( refHead, key, cmp );
        }

        template <typename K, typename Compare>
        bool find_at( head_type& refHead, K const& key, Compare cmp ) const
        {
            return base_class::find_at( refHead, key, cmp, [](node_type&, K const&) {} );
        }

        template <typename K, typename Compare, typename Func>
        bool find_at( head_type& refHead, K& key, Compare cmp, Func f ) const
        {
            return base_class::find_at( refHead, key, cmp, [&f](node_type& node, K const&){ f( node.m_Data ); });
        }

        template <typename K, typename Compare>
        value_type * get_at( head_type& refHead, K const& val, Compare cmp ) const
        {
            node_type * pNode = base_class::get_at( refHead, val, cmp );
            return pNode ? &pNode->m_Data : nullptr;
        }

        //@endcond
    };

}}  // namespace cds::container

#endif  // #ifndef CDSLIB_CONTAINER_MICHAEL_KVLIST_RCU_H
