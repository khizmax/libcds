// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_IMPL_MICHAEL_LIST_H
#define CDSLIB_CONTAINER_IMPL_MICHAEL_LIST_H

#include <memory>
#include <cds/container/details/guarded_ptr_cast.h>

namespace cds { namespace container {

    /// Michael's ordered list
    /** @ingroup cds_nonintrusive_list
        \anchor cds_nonintrusive_MichaelList_gc

        Usually, ordered single-linked list is used as a building block for the hash table implementation.
        The complexity of searching is <tt>O(N)</tt>, where \p N is the item count in the list, not in the
        hash table.

        Source:
        - [2002] Maged Michael "High performance dynamic lock-free hash tables and list-based sets"

        This class is non-intrusive version of cds::intrusive::MichaelList class

        Template arguments:
        - \p GC - garbage collector used
        - \p T - type stored in the list. The type must be default- and copy-constructible.
        - \p Traits - type traits, default is \p michael_list::traits

        Unlike standard container, this implementation does not divide type \p T into key and value part and
        may be used as a main building block for hash set algorithms.
        The key is a function (or a part) of type \p T, and this function is specified by <tt>Traits::compare</tt> functor
        or <tt>Traits::less</tt> predicate

        MichaelKVList is a key-value version of Michael's non-intrusive list that is closer to the C++ std library approach.

        It is possible to declare option-based list with cds::container::michael_list::make_traits metafunction istead of \p Traits template
        argument. For example, the following traits-based declaration of gc::HP Michael's list
        \code
        #include <cds/container/michael_list_hp.h>
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
        typedef cds::container::MichaelList< cds::gc::HP, int, my_traits >     traits_based_list;
        \endcode

        is equivalent for the following option-based list
        \code
        #include <cds/container/michael_list_hp.h>

        // my_compare is the same

        // Declare option-based list
        typedef cds::container::MichaelList< cds::gc::HP, int,
            typename cds::container::michael_list::make_traits<
                cds::container::opt::compare< my_compare >     // item comparator option
            >::type
        >     option_based_list;
        \endcode

        \par Usage
        There are different specializations of this template for each garbage collecting schema used.
        You should include appropriate .h-file depending on GC you are using:
        - for gc::HP: \code #include <cds/container/michael_list_hp.h> \endcode
        - for gc::DHP: \code #include <cds/container/michael_list_dhp.h> \endcode
        - for \ref cds_urcu_desc "RCU": \code #include <cds/container/michael_list_rcu.h> \endcode
        - for gc::nogc: \code #include <cds/container/michael_list_nogc.h> \endcode
    */
    template <
        typename GC,
        typename T,
#ifdef CDS_DOXYGEN_INVOKED
        typename Traits = michael_list::traits
#else
        typename Traits
#endif
    >
    class MichaelList:
#ifdef CDS_DOXYGEN_INVOKED
        protected intrusive::MichaelList< GC, T, Traits >
#else
        protected details::make_michael_list< GC, T, Traits >::type
#endif
    {
        //@cond
        typedef details::make_michael_list< GC, T, Traits > maker;
        typedef typename maker::type base_class;
        //@endcond

    public:
        typedef T value_type;   ///< Type of value stored in the list
        typedef Traits traits;  ///< List traits

        typedef typename base_class::gc             gc;             ///< Garbage collector used
        typedef typename base_class::back_off       back_off;       ///< Back-off strategy used
        typedef typename maker::allocator_type      allocator_type; ///< Allocator type used for allocate/deallocate the nodes
        typedef typename base_class::item_counter   item_counter;   ///< Item counting policy used
        typedef typename maker::key_comparator      key_comparator; ///< key comparison functor
        typedef typename base_class::memory_model   memory_model;   ///< Memory ordering. See \p cds::opt::memory_model option
        typedef typename base_class::stat           stat;           ///< Internal statistics

        static constexpr const size_t c_nHazardPtrCount = base_class::c_nHazardPtrCount; ///< Count of hazard pointer required for the algorithm

        //@cond
        // Rebind traits (split-list support)
        template <typename... Options>
        struct rebind_traits {
            typedef MichaelList<
                gc
                , value_type
                , typename cds::opt::make_options< traits, Options...>::type
            > type;
        };

        // Stat selector
        template <typename Stat>
        using select_stat_wrapper = typename base_class::template select_stat_wrapper< Stat >;
        //@endcond

    protected:
        //@cond
        typedef typename base_class::value_type      node_type;
        typedef typename maker::cxx_allocator        cxx_allocator;
        typedef typename maker::node_deallocator     node_deallocator;
        typedef typename maker::intrusive_traits::compare intrusive_key_comparator;

        typedef typename base_class::atomic_node_ptr head_type;

        struct node_disposer {
            void operator()( node_type * pNode )
            {
                free_node( pNode );
            }
        };
        typedef std::unique_ptr< node_type, node_disposer > scoped_node_ptr;
        //@endcond

    public:
        /// Guarded pointer
        typedef typename gc::template guarded_ptr< node_type, value_type, details::guarded_ptr_cast_set<node_type, value_type> > guarded_ptr;

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
    ///@name Forward iterators (only for debugging purpose)
    //@{
        /// Forward iterator
        /**
            The forward iterator for Michael's list has some features:
            - it has no post-increment operator
            - to protect the value, the iterator contains a GC-specific guard + another guard is required locally for increment operator.
              For some GC (\p gc::HP), a guard is limited resource per thread, so an exception (or assertion) "no free guard"
              may be thrown if a limit of guard count per thread is exceeded.
            - The iterator cannot be moved across thread boundary since it contains GC's guard that is thread-private GC data.
            - Iterator ensures thread-safety even if you delete the item that iterator points to. However, in case of concurrent
              deleting operations it is no guarantee that you iterate all item in the list.
              Moreover, a crash is possible when you try to iterate the next element that has been deleted by concurrent thread.

            @warning Use this iterator on the concurrent container for debugging purpose only.
        */
        typedef iterator_type<false>    iterator;

        /// Const forward iterator
        /**
            For iterator's features and requirements see \ref iterator
        */
        typedef iterator_type<true>     const_iterator;

        /// Returns a forward iterator addressing the first element in a list
        /**
            For empty list \code begin() == end() \endcode
        */
        iterator begin()
        {
            return iterator( head());
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
        const_iterator begin() const
        {
            return const_iterator( head());
        }

        /// Returns a forward const iterator addressing the first element in a list
        const_iterator cbegin() const
        {
            return const_iterator( head());
        }

        /// Returns an const iterator that addresses the location succeeding the last element in a list
        const_iterator end() const
        {
            return const_iterator();
        }

        /// Returns an const iterator that addresses the location succeeding the last element in a list
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

        //@cond
        template <typename Stat, typename = std::enable_if<std::is_same<stat, michael_list::wrapped_stat<Stat>>::value >>
        explicit MichaelList( Stat& st )
            : base_class( st )
        {}
        //@endcond

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

            The type \p Q should contain least the complete key of the node.
            The object of \ref value_type should be constructible from \p val of type \p Q.
            In trivial case, \p Q is equal to \ref value_type.

            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename Q>
        bool insert( Q&& val )
        {
            return insert_at( head(), std::forward<Q>( val ));
        }

        /// Inserts new node
        /**
            This function inserts new node with default-constructed value and then it calls
            \p func functor with signature
            \code void func( value_type& itemValue ) ;\endcode

            The argument \p itemValue of user-defined functor \p func is the reference
            to the list's item inserted. User-defined functor \p func should guarantee that during changing
            item's value no any other changes could be made on this list's item by concurrent threads.
            The user-defined functor is called only if inserting is success.

            The type \p Q should contain the complete key of the node.
            The object of \p value_type should be constructible from \p key of type \p Q.

            The function allows to split creating of new item into two part:
            - create item from \p key with initializing key-fields only;
            - insert new item into the list;
            - if inserting is successful, initialize non-key fields of item by calling \p func functor

            The method can be useful if complete initialization of object of \p value_type is heavyweight and
            it is preferable that the initialization should be completed only if inserting is successful.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"
        */
        template <typename Q, typename Func>
        bool insert( Q&& key, Func func )
        {
            return insert_at( head(), std::forward<Q>(key), func );
        }

        /// Updates data by \p key
        /**
            The operation performs inserting or replacing the element with lock-free manner.

            If the \p key not found in the list, then the new item created from \p key
            will be inserted iff \p bAllowInsert is \p true.
            Otherwise, if \p key is found, the functor \p func is called with item found.

            The functor \p Func signature is:
            \code
                struct my_functor {
                    void operator()( bool bNew, value_type& item, Q const& key );
                };
            \endcode

            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the list
            - \p key - argument \p key passed into the \p %update() function

            The functor may change non-key fields of the \p item; however, \p func must guarantee
            that during changing no any other modifications could be made on this item by concurrent threads.

            Returns <tt> std::pair<bool, bool> </tt> where \p first is true if operation is successful,
            \p second is true if new item has been added or \p false if the item with \p key
            already exists.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"
        */
        template <typename Q, typename Func>
        std::pair<bool, bool> update( Q const& key, Func func, bool bAllowInsert = true )
        {
            return update_at( head(), key, func, bAllowInsert );
        }
        //@cond
        template <typename Q, typename Func>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<bool, bool> ensure( Q const& key, Func func )
        {
            return update( key, func );
        }
        //@endcond

        /// Inserts data of type \p value_type constructed with <tt>std::forward<Args>(args)...</tt>
        /**
            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename... Args>
        bool emplace( Args&&... args )
        {
            return emplace_at( head(), std::forward<Args>(args)... );
        }

        /// Delete \p key from the list
        /** \anchor cds_nonintrusive_MichealList_hp_erase_val
            Since the key of MichaelList's item type \p value_type is not explicitly specified,
            template parameter \p Q sould contain the complete key to search in the list.
            The list item comparator should be able to compare the type \p value_type
            and the type \p Q.

            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename Q>
        bool erase( Q const& key )
        {
            return erase_at( head(), key, intrusive_key_comparator(), [](value_type const&){} );
        }

        /// Deletes the item from the list using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_MichealList_hp_erase_val "erase(Q const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        bool erase_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return erase_at( head(), key, typename maker::template less_wrapper<Less>(), [](value_type const&){} );
        }

        /// Deletes \p key from the list
        /** \anchor cds_nonintrusive_MichaelList_hp_erase_func
            The function searches an item with key \p key, calls \p f functor with item found
            and deletes it. If \p key is not found, the functor is not called.

            The functor \p Func interface:
            \code
            struct extractor {
                void operator()(const value_type& val) { ... }
            };
            \endcode

            Since the key of MichaelList's item type \p value_type is not explicitly specified,
            template parameter \p Q should contain the complete key to search in the list.
            The list item comparator should be able to compare the type \p value_type of list item
            and the type \p Q.

            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename Q, typename Func>
        bool erase( Q const& key, Func f )
        {
            return erase_at( head(), key, intrusive_key_comparator(), f );
        }

        /// Deletes the item from the list using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_MichaelList_hp_erase_func "erase(Q const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less, typename Func>
        bool erase_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return erase_at( head(), key, typename maker::template less_wrapper<Less>(), f );
        }

        /// Extracts the item from the list with specified \p key
        /** \anchor cds_nonintrusive_MichaelList_hp_extract
            The function searches an item with key equal to \p key,
            unlinks it from the list, and returns it as \p guarded_ptr.
            If \p key is not found the function returns an empty guarded pointer.

            Note the compare functor should accept a parameter of type \p Q that can be not the same as \p value_type.

            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.

            Usage:
            \code
            typedef cds::container::MichaelList< cds::gc::HP, foo, my_traits >  ord_list;
            ord_list theList;
            // ...
            {
                ord_list::guarded_ptr gp(theList.extract( 5 ));
                if ( gp ) {
                    // Deal with gp
                    // ...
                }
                // Destructor of gp releases internal HP guard and frees the item
            }
            \endcode
        */
        template <typename Q>
        guarded_ptr extract( Q const& key )
        {
            return extract_at( head(), key, intrusive_key_comparator());
        }

        /// Extracts the item from the list with comparing functor \p pred
        /**
            The function is an analog of \ref cds_nonintrusive_MichaelList_hp_extract "extract(Q const&)"
            but \p pred predicate is used for key comparing.

            \p Less functor has the semantics like \p std::less but it should accept arguments of type \p value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        guarded_ptr extract_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return extract_at( head(), key, typename maker::template less_wrapper<Less>());
        }

        /// Checks whether the list contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.
        */
        template <typename Q>
        bool contains( Q const& key )
        {
            return find_at( head(), key, intrusive_key_comparator());
        }
        //@cond
        template <typename Q>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find( Q const& key )
        {
            return contains( key );
        }
        //@endcond

        /// Checks whether the list contains \p key using \p pred predicate for searching
        /**
            The function is an analog of <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        bool contains( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return find_at( head(), key, typename maker::template less_wrapper<Less>());
        }
        //@cond
        template <typename Q, typename Less>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find_with( Q const& key, Less pred )
        {
            return contains( key, pred );
        }
        //@endcond

        /// Finds \p key and perform an action with it
        /** \anchor cds_nonintrusive_MichaelList_hp_find_func
            The function searches an item with key equal to \p key and calls the functor \p f for the item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q& key );
            };
            \endcode
            where \p item is the item found, \p key is the <tt>find</tt> function argument.

            The functor may change non-key fields of \p item. Note that the function is only guarantee
            that \p item cannot be deleted during functor is executing.
            The function does not serialize simultaneous access to the list \p item. If such access is
            possible you must provide your own synchronization schema to exclude unsafe item modifications.

            The function returns \p true if \p key is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q& key, Func f )
        {
            return find_at( head(), key, intrusive_key_comparator(), f );
        }
        //@cond
        template <typename Q, typename Func>
        bool find( Q const& key, Func f )
        {
            return find_at( head(), key, intrusive_key_comparator(), f );
        }
        //@endcond

        /// Finds \p key using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_MichaelList_hp_find_func "find(Q&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return find_at( head(), key, typename maker::template less_wrapper<Less>(), f );
        }
        //@cond
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return find_at( head(), key, typename maker::template less_wrapper<Less>(), f );
        }
        //@endcond

        /// Finds \p key and return the item found
        /**
            The function searches the item with key equal to \p key
            and returns it as \p guarded_ptr.
            If \p key is not found the function returns an empty guarded pointer.

            @note Each \p guarded_ptr object uses one GC's guard which can be limited resource.

            Usage:
            \code
            typedef cds::container::MichaelList< cds::gc::HP, foo, my_traits >  ord_list;
            ord_list theList;
            // ...
            {
                ord_list::guarded_ptr gp(theList.get( 5 ));
                if ( gp ) {
                    // Deal with gp
                    //...
                }
                // Destructor of guarded_ptr releases internal HP guard and frees the item
            }
            \endcode

            Note the compare functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that can be not the same as \p value_type.
        */
        template <typename Q>
        guarded_ptr get( Q const& key )
        {
            return get_at( head(), key, intrusive_key_comparator());
        }

        /// Finds \p key and return the item found
        /**
            The function is an analog of \p get( Q const&)
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should accept arguments of type \p value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less>
        guarded_ptr get_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return get_at( head(), key, typename maker::template less_wrapper<Less>());
        }

        /// Check if the list is empty
        bool empty() const
        {
            return base_class::empty();
        }

        /// Returns list's item count
        /**
            The value returned depends on item counter provided by \p Traits. For \p atomicity::empty_item_counter,
            this function always returns 0.

            @note Even if you use real item counter and it returns 0, this fact is not mean that the list
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

        /// Returns const reference to internal statistics
        stat const& statistics() const
        {
            return base_class::statistics();
        }

    protected:
        //@cond
        static value_type& node_to_value( node_type& n )
        {
            return n.m_Value;
        }
        static value_type const& node_to_value( node_type const& n )
        {
            return n.m_Value;
        }

        template <typename Q>
        static node_type * alloc_node( Q const& v )
        {
            return cxx_allocator().New( v );
        }

        template <typename... Args>
        static node_type * alloc_node( Args&&... args )
        {
            return cxx_allocator().MoveNew( std::forward<Args>( args )... );
        }

        static void free_node( node_type * pNode )
        {
            cxx_allocator().Delete( pNode );
        }

        head_type& head()
        {
            return base_class::m_pHead;
        }

        head_type const& head() const
        {
            return base_class::m_pHead;
        }

        bool insert_node( node_type * pNode )
        {
            return insert_node_at( head(), pNode );
        }

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
        bool insert_at( head_type& refHead, Q&& val )
        {
            return insert_node_at( refHead, alloc_node( std::forward<Q>(val)));
        }

        template <typename Q, typename Func>
        bool insert_at( head_type& refHead, Q&& key, Func f )
        {
            scoped_node_ptr pNode( alloc_node( std::forward<Q>( key )));

            if ( base_class::insert_at( refHead, *pNode, [&f]( node_type& node ) { f( node_to_value(node)); } )) {
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
            return base_class::erase_at( refHead, key, cmp, [&f](node_type const& node){ f( node_to_value(node)); } );
        }

        template <typename Q, typename Compare>
        guarded_ptr extract_at( head_type& refHead, Q const& key, Compare cmp )
        {
            return base_class::extract_at( refHead, key, cmp );
        }

        template <typename Q, typename Func>
        std::pair<bool, bool> update_at( head_type& refHead, Q const& key, Func f, bool bAllowInsert )
        {
            scoped_node_ptr pNode( alloc_node( key ));

            std::pair<bool, bool> ret = base_class::update_at( refHead, *pNode,
                [&f, &key](bool bNew, node_type& node, node_type&){ f( bNew, node_to_value(node), key );},
                bAllowInsert );
            if ( ret.first && ret.second )
                pNode.release();

            return ret;
        }

        template <typename Q, typename Compare>
        bool find_at( head_type& refHead, Q const& key, Compare cmp )
        {
            return base_class::find_at( refHead, key, cmp );
        }

        template <typename Q, typename Compare, typename Func>
        bool find_at( head_type& refHead, Q& val, Compare cmp, Func f )
        {
            return base_class::find_at( refHead, val, cmp, [&f](node_type& node, Q& v){ f( node_to_value(node), v ); });
        }

        template <typename Q, typename Compare>
        guarded_ptr get_at( head_type& refHead, Q const& key, Compare cmp )
        {
            return base_class::get_at( refHead, key, cmp );
        }

        //@endcond
    };

}}  // namespace cds::container

#endif  // #ifndef CDSLIB_CONTAINER_IMPL_MICHAEL_LIST_H
