//$$CDS-header$$

#ifndef CDSLIB_CONTAINER_MICHAEL_LIST_RCU_H
#define CDSLIB_CONTAINER_MICHAEL_LIST_RCU_H

#include <memory>
#include <cds/container/details/michael_list_base.h>
#include <cds/intrusive/michael_list_rcu.h>
#include <cds/container/details/make_michael_list.h>
#include <cds/details/binary_functor_wrapper.h>

namespace cds { namespace container {

    /// Michael's ordered list (template specialization for \ref cds_urcu_desc "RCU")
    /** @ingroup cds_nonintrusive_list
        \anchor cds_nonintrusive_MichaelList_rcu

        Usually, ordered single-linked list is used as a building block for the hash table implementation.
        The complexity of searching is <tt>O(N)</tt>.

        Source:
        - [2002] Maged Michael "High performance dynamic lock-free hash tables and list-based sets"

        This class is non-intrusive version of \ref cds_intrusive_MichaelList_rcu "cds::intrusive::MichaelList" RCU specialization.

        Template arguments:
        - \p RCU - one of \ref cds_urcu_gc "RCU type"
        - \p T - type stored in the list. The type must be default- and copy-constructible.
        - \p Traits - type traits, default is michael_list::traits

        The implementation does not divide type \p T into key and value part and
        may be used as a main building block for hash set containers.
        The key is a function (or a part) of type \p T, and this function is specified by <tt>Traits::compare</tt> functor
        or <tt>Traits::less</tt> predicate.

        \ref cds_nonintrusive_MichaelKVList_rcu "MichaelKVList" is a key-value version of Michael's
        non-intrusive list that is closer to the C++ std library approach.

        @note Before including <tt><cds/container/michael_list_rcu.h></tt> you should include appropriate RCU header file,
        see \ref cds_urcu_gc "RCU type" for list of existing RCU class and corresponding header files.

        It is possible to declare option-based list with cds::container::michael_list::make_traits metafunction istead of \p Traits template
        argument. For example, the following traits-based declaration of Michael's list

        \code
        #include <cds/urcu/general_buffered.h>
        #include <cds/container/michael_list_rcu.h>
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
        typedef cds::container::MichaelList< cds::urcu::gc< cds::urcu::general_buffered<> >, int, my_traits >     traits_based_list;
        \endcode

        is equivalent for the following option-based list
        \code
        #include <cds/urcu/general_buffered.h>
        #include <cds/container/michael_list_rcu.h>

        // my_compare is the same

        // Declare option-based list
        typedef cds::container::MichaelList< cds::urcu::gc< cds::urcu::general_buffered<> >, int,
            typename cds::container::michael_list::make_traits<
                cds::container::opt::compare< my_compare >     // item comparator option
            >::type
        >     option_based_list;
        \endcode

        Template argument list \p Options of cds::container::michael_list::make_traits metafunction are:
        - opt::compare - key comparison functor. No default functor is provided.
            If the option is not specified, the opt::less is used.
        - opt::less - specifies binary predicate used for key comparison. Default is \p std::less<T>.
        - opt::back_off - back-off strategy used. If the option is not specified, the cds::backoff::empty is used.
        - opt::item_counter - the type of item counting feature. Default is \ref atomicity::empty_item_counter that is no item counting.
        - opt::allocator - the allocator used for creating and freeing list's item. Default is \ref CDS_DEFAULT_ALLOCATOR macro.
        - opt::memory_model - C++ memory ordering model. Can be opt::v::relaxed_ordering (relaxed memory model, the default)
            or opt::v::sequential_consistent (sequentially consisnent memory model).
        - opt::rcu_check_deadlock - a deadlock checking policy. Default is opt::v::rcu_throw_deadlock
    */
    template <
        typename RCU,
        typename T,
#ifdef CDS_DOXYGEN_INVOKED
        typename Traits = michael_list::traits
#else
        typename Traits
#endif
    >
    class MichaelList< cds::urcu::gc<RCU>, T, Traits > :
#ifdef CDS_DOXYGEN_INVOKED
        protected intrusive::MichaelList< cds::urcu::gc<RCU>, T, Traits >
#else
        protected details::make_michael_list< cds::urcu::gc<RCU>, T, Traits >::type
#endif
    {
        //@cond
        typedef details::make_michael_list< cds::urcu::gc<RCU>, T, Traits > maker;
        typedef typename maker::type  base_class;
        //@endcond

    public:
        typedef cds::urcu::gc<RCU> gc;          ///< RCU
        typedef T                  value_type;  ///< Type of value stored in the list
        typedef Traits             traits;      ///< List traits

        typedef typename base_class::back_off     back_off;       ///< Back-off strategy used
        typedef typename maker::allocator_type    allocator_type; ///< Allocator type used for allocate/deallocate the nodes
        typedef typename base_class::item_counter item_counter;   ///< Item counting policy used
        typedef typename maker::key_comparator    key_comparator; ///< key comparison functor
        typedef typename base_class::memory_model memory_model;   ///< Memory ordering. See cds::opt::memory_model option
        typedef typename base_class::rcu_check_deadlock rcu_check_deadlock ; ///< RCU deadlock checking policy

        typedef typename gc::scoped_lock    rcu_lock ;  ///< RCU scoped lock
        static CDS_CONSTEXPR const bool c_bExtractLockExternal = base_class::c_bExtractLockExternal; ///< Group of \p extract_xxx functions require external locking

    protected:
        //@cond
        typedef typename base_class::value_type   node_type;
        typedef typename maker::cxx_allocator     cxx_allocator;
        typedef typename maker::node_deallocator  node_deallocator;
        typedef typename maker::intrusive_traits::compare  intrusive_key_comparator;

        typedef typename base_class::atomic_node_ptr      head_type;
        //@endcond

    public:
        using exempt_ptr = cds::urcu::exempt_ptr< gc, node_type, value_type, typename maker::intrusive_traits::disposer >; ///< pointer to extracted node

    private:
        //@cond
        static value_type& node_to_value( node_type& n )
        {
            return n.m_Value;
        }
        static value_type const& node_to_value( node_type const& n )
        {
            return n.m_Value;
        }
        //@endcond

    protected:
        //@cond
        template <typename Q>
        static node_type * alloc_node( Q const& v )
        {
            return cxx_allocator().New( v );
        }

        template <typename... Args>
        static node_type * alloc_node( Args&&... args )
        {
            return cxx_allocator().MoveNew( std::forward<Args>(args)... );
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

            friend class MichaelList;

        public:
            typedef typename cds::details::make_const_type<value_type, IsConst>::pointer   value_ptr;
            typedef typename cds::details::make_const_type<value_type, IsConst>::reference value_ref;

            iterator_type()
            {}

            iterator_type( iterator_type const& src )
                : iterator_base( src )
            {}

            value_ptr operator ->() const
            {
                typename iterator_base::value_ptr p = iterator_base::operator ->();
                return p ? &(p->m_Value) : nullptr;
            }

            value_ref operator *() const
            {
                return (iterator_base::operator *()).m_Value;
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
            Initialize empty list
        */
        MichaelList()
        {}

        /// List destructor
        /**
            Clears the list
        */
        ~MichaelList()
        {
            clear();
        }

        /// Inserts new node
        /**
            The function creates a node with copy of \p val value
            and then inserts the node created into the list.

            The type \p Q should contain as minimum the complete key of the node.
            The object of \ref value_type should be constructible from \p val of type \p Q.
            In trivial case, \p Q is equal to \ref value_type.

            The function makes RCU lock internally.

            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename Q>
        bool insert( Q const& val )
        {
            return insert_at( head(), val );
        }

        /// Inserts new node
        /**
            This function inserts new node with default-constructed value and then it calls
            \p func functor with signature
            \code void func( value_type& itemValue ) ;\endcode

            The argument \p itemValue of user-defined functor \p func is the reference
            to the list's item inserted. User-defined functor \p func should guarantee that during changing
            item's value no any other changes could be made on this list's item by concurrent threads.

            The type \p Q should contain the complete key of the node.
            The object of \ref value_type should be constructible from \p key of type \p Q.

            The function allows to split creating of new item into two part:
            - create item from \p key with initializing key-fields only;
            - insert new item into the list;
            - if inserting is successful, initialize non-key fields of item by calling \p f functor

            This can be useful if complete initialization of object of \p value_type is heavyweight and
            it is preferable that the initialization should be completed only if inserting is successful.

            The function makes RCU lock internally.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"
        */
        template <typename Q, typename Func>
        bool insert( Q const& key, Func func )
        {
            return insert_at( head(), key, func );
        }

        /// Ensures that the \p key exists in the list
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the \p key not found in the list, then the new item created from \p key
            is inserted into the list. Otherwise, the functor \p func is called with the item found.
            The functor \p Func should be a function with signature:
            \code
                void func( bool bNew, value_type& item, const Q& val );
            \endcode
            or a functor:
            \code
                struct my_functor {
                    void operator()( bool bNew, value_type& item, const Q& val );
                };
            \endcode

            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the list
            - \p val - argument \p key passed into the \p ensure function

            The functor may change non-key fields of the \p item; however, \p func must guarantee
            that during changing no any other modifications could be made on this item by concurrent threads.

            The function makes RCU lock internally.

            Returns <tt> std::pair<bool, bool> </tt> where \p first is true if operation is successfull,
            \p second is true if new item has been added or \p false if the item with \p key
            already is in the list.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"
        */
        template <typename Q, typename Func>
        std::pair<bool, bool> ensure( Q const& key, Func f )
        {
            return ensure_at( head(), key, f );
        }

        /// Inserts data of type \ref value_type constructed from \p args
        /**
            Returns \p true if inserting successful, \p false otherwise.

            The function makes RCU lock internally.
        */
        template <typename... Args>
        bool emplace( Args&&... args )
        {
            return emplace_at( head(), std::forward<Args>(args)... );
        }

        /// Deletes \p key from the list
        /** \anchor cds_nonintrusive_MichealList_rcu_erase_val
            Since the key of MichaelList's item type \p value_type is not explicitly specified,
            template parameter \p Q defines the key type searching in the list.
            The list item comparator should be able to compare values of the type \p value_type
            and \p Q in any order.

            RCU \p synchronize method can be called. RCU should not be locked.

            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename Q>
        bool erase( Q const& key )
        {
            return erase_at( head(), key, intrusive_key_comparator(),  [](value_type const&){} );
        }

        /// Deletes the item from the list using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_MichealList_rcu_erase_val "erase(Q const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        bool erase_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return erase_at( head(), key, typename maker::template less_wrapper<Less>::type(), [](value_type const&){} );
        }

        /// Deletes \p key from the list
        /** \anchor cds_nonintrusive_MichaelList_rcu_erase_func
            The function searches an item with key \p key, calls \p f functor with item found
            and deletes it. If \p key is not found, the functor is not called.

            The functor \p Func interface:
            \code
            struct extractor {
                void operator()(const value_type& val) { ... }
            };
            \endcode

            Since the key of MichaelList's item type \p value_type is not explicitly specified,
            template parameter \p Q defines the key type searching in the list.
            The list item comparator should be able to compare the values of type \p value_type
            and \p Q in any order.

            RCU \p synchronize method can be called. RCU should not be locked.

            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename Q, typename Func>
        bool erase( Q const& key, Func f )
        {
            return erase_at( head(), key, intrusive_key_comparator(), f );
        }

        /// Deletes the item from the list using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_MichaelList_rcu_erase_func "erase(Q const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less, typename Func>
        bool erase_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return erase_at( head(), key, typename maker::template less_wrapper<Less>::type(), f );
        }

        /// Extracts an item from the list
        /**
        @anchor cds_nonintrusive_MichaelList_rcu_extract
            The function searches an item with key equal to \p key in the list,
            unlinks it from the list, and returns \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to the item found.
            If the item with the key equal to \p key is not found the function returns an empty \p exempt_ptr.

            @note The function does NOT call RCU read-side lock or synchronization,
            and does NOT dispose the item found. It just excludes the item from the list
            and returns a pointer to item found.
            You should lock RCU before calling this function.

            \code
            #include <cds/urcu/general_buffered.h>
            #include <cds/container/michael_list_rcu.h>

            typedef cds::urcu::gc< general_buffered<> > rcu;
            typedef cds::container::MichaelList< rcu, Foo > rcu_michael_list;

            rcu_michael_list theList;
            // ...

            rcu_michael_list::exempt_ptr p;
            {
                // first, we should lock RCU
                rcu::scoped_lock sl;

                // Now, you can apply extract function
                // Note that you must not delete the item found inside the RCU lock
                p = theList.extract( 10 )
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
        template <typename Q>
        exempt_ptr extract( Q const& key )
        {
            return exempt_ptr( extract_at( head(), key, intrusive_key_comparator() ));
        }

        /// Extracts an item from the list using \p pred predicate for searching
        /**
            This function is the analog for \p extract(Q const&).

            The \p pred is a predicate used for key comparing.
            \p Less has the interface like \p std::less.
            \p pred must imply the same element order as \ref key_comparator.
        */
        template <typename Q, typename Less>
        exempt_ptr extract_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return exempt_ptr( extract_at( head(), key, typename maker::template less_wrapper<Less>::type() ));
        }

        /// Finds the key \p key
        /** \anchor cds_nonintrusive_MichaelList_rcu_find_val
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.

            The function makes RCU lock internally.
        */
        template <typename Q>
        bool find( Q const& key ) const
        {
            return find_at( head(), key, intrusive_key_comparator() );
        }

        /// Finds the key \p val using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_MichaelList_rcu_find_val "find(Q const&)"
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

        /// Finds the key \p key and performs an action with it
        /** \anchor cds_nonintrusive_MichaelList_rcu_find_func
            The function searches an item with key equal to \p key and calls the functor \p f for the item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q& key );
            };
            \endcode
            where \p item is the item found, \p key is the \p %find() function argument.

            The functor may change non-key fields of \p item. Note that the function is only guarantee
            that \p item cannot be deleted during functor is executing.
            The function does not serialize simultaneous access to the list \p item. If such access is
            possible you must provide your own synchronization schema to exclude unsafe item modifications.

            The function makes RCU lock internally.

            The function returns \p true if \p val is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q& key, Func f ) const
        {
            return find_at( head(), key, intrusive_key_comparator(), f );
        }
        //@cond
        template <typename Q, typename Func>
        bool find( Q const& key, Func f ) const
        {
            return find_at( head(), key, intrusive_key_comparator(), f );
        }
        //@endcond

        /// Finds the key \p key using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_MichaelList_rcu_find_func "find(Q&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q& key, Less pred, Func f ) const
        {
            CDS_UNUSED( pred );
            return find_at( head(), key, typename maker::template less_wrapper<Less>::type(), f );
        }
        //@cond
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& key, Less pred, Func f ) const
        {
            CDS_UNUSED( pred );
            return find_at( head(), key, typename maker::template less_wrapper<Less>::type(), f );
        }
        //@endcond

        /// Finds the key \p key and return the item found
        /** \anchor cds_nonintrusive_MichaelList_rcu_get
            The function searches the item with key equal to \p key and returns the pointer to item found.
            If \p key is not found it returns \p nullptr.

            Note the compare functor should accept a parameter of type \p Q that can be not the same as \p value_type.

            RCU should be locked before call of this function.
            Returned item is valid only while RCU is locked:
            \code
            typedef cds::container::MichaelList< cds::urcu::gc< cds::urcu::general_buffered<> >, foo, my_traits > ord_list;
            ord_list theList;
            // ...
            {
                // Lock RCU
                ord_list::rcu_lock lock;

                foo * pVal = theList.get( 5 );
                if ( pVal ) {
                    // Deal with pVal
                    //...
                }
                // Unlock RCU by rcu_lock destructor
                // pVal can be freed at any time after RCU has been unlocked
            }
            \endcode
        */
        template <typename Q>
        value_type * get( Q const& key ) const
        {
            return get_at( head(), key, intrusive_key_comparator());
        }

        /// Finds \p key and return the item found
        /**
            The function is an analog of \ref cds_nonintrusive_MichaelList_rcu_get "get(Q const&)"
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        value_type * get_with( Q const& key, Less pred ) const
        {
            CDS_UNUSED( pred );
            return get_at( head(), key, typename maker::template less_wrapper<Less>::type());
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
        void clear()
        {
            base_class::clear();
        }

    protected:
        //@cond
        bool insert_node_at( head_type& refHead, node_type * pNode )
        {
            assert( pNode );
            scoped_node_ptr p(pNode);
            if ( base_class::insert_at( refHead, *pNode )) {
                p.release();
                return true;
            }

            return false;
        }

        template <typename Q>
        bool insert_at( head_type& refHead, Q const& val )
        {
            return insert_node_at( refHead, alloc_node( val ));
        }

        template <typename Q, typename Func>
        bool insert_at( head_type& refHead, Q const& key, Func f )
        {
            scoped_node_ptr pNode( alloc_node( key ));

            if ( base_class::insert_at( refHead, *pNode, [&f]( node_type& node ) { f( node_to_value(node) ); } )) {
                pNode.release();
                return true;
            }
            return false;
        }

        template <typename... Args>
        bool emplace_at( head_type& refHead, Args&&... args )
        {
            return insert_node_at( refHead, alloc_node( std::forward<Args>(args) ... ));
        }

        template <typename Q, typename Compare, typename Func>
        bool erase_at( head_type& refHead, Q const& key, Compare cmp, Func f )
        {
            return base_class::erase_at( refHead, key, cmp, [&f](node_type const& node){ f( node_to_value(node) ); } );
        }

        template <typename Q, typename Func>
        std::pair<bool, bool> ensure_at( head_type& refHead, Q const& key, Func f )
        {
            scoped_node_ptr pNode( alloc_node( key ));

            std::pair<bool, bool> ret = base_class::ensure_at( refHead, *pNode,
                [&f, &key](bool bNew, node_type& node, node_type&){ f( bNew, node_to_value(node), key ); });
            if ( ret.first && ret.second )
                pNode.release();

            return ret;
        }

        template <typename Q, typename Compare>
        node_type * extract_at( head_type& refHead, Q const& key, Compare cmp )
        {
            return base_class::extract_at( refHead, key, cmp );
        }

        template <typename Q, typename Compare>
        bool find_at( head_type& refHead, Q const& key, Compare cmp ) const
        {
            return base_class::find_at( refHead, key, cmp, [](node_type&, Q const &) {} );
        }

        template <typename Q, typename Compare, typename Func>
        bool find_at( head_type& refHead, Q& val, Compare cmp, Func f ) const
        {
            return base_class::find_at( refHead, val, cmp, [&f](node_type& node, Q& v){ f( node_to_value(node), v ); });
        }

        template <typename Q, typename Compare>
        value_type * get_at( head_type& refHead, Q const& val, Compare cmp ) const
        {
            node_type * pNode = base_class::get_at( refHead, val, cmp );
            return pNode ? &pNode->m_Value : nullptr;
        }

        //@endcond
    };

}}  // namespace cds::container

#endif  // #ifndef CDSLIB_CONTAINER_MICHAEL_LIST_RCU_H
