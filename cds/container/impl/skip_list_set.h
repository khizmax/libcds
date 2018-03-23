// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_IMPL_SKIP_LIST_SET_H
#define CDSLIB_CONTAINER_IMPL_SKIP_LIST_SET_H

#include <cds/details/binary_functor_wrapper.h>
#include <cds/container/details/guarded_ptr_cast.h>

namespace cds { namespace container {

    /// Lock-free skip-list set
    /** @ingroup cds_nonintrusive_set
        \anchor cds_nonintrusive_SkipListSet_hp

        The implementation of well-known probabilistic data structure called skip-list
        invented by W.Pugh in his papers:
            - [1989] W.Pugh Skip Lists: A Probabilistic Alternative to Balanced Trees
            - [1990] W.Pugh A Skip List Cookbook

        A skip-list is a probabilistic data structure that provides expected logarithmic
        time search without the need of rebalance. The skip-list is a collection of sorted
        linked list. Nodes are ordered by key. Each node is linked into a subset of the lists.
        Each list has a level, ranging from 0 to 32. The bottom-level list contains
        all the nodes, and each higher-level list is a sublist of the lower-level lists.
        Each node is created with a random top level (with a random height), and belongs
        to all lists up to that level. The probability that a node has the height 1 is 1/2.
        The probability that a node has the height N is 1/2 ** N (more precisely,
        the distribution depends on an random generator provided, but our generators
        have this property).

        The lock-free variant of skip-list is implemented according to book
            - [2008] M.Herlihy, N.Shavit "The Art of Multiprocessor Programming",
                chapter 14.4 "A Lock-Free Concurrent Skiplist"

        Template arguments:
        - \p GC - Garbage collector used.
        - \p T - type to be stored in the list.
        - \p Traits - set traits, default is \p skip_list::traits.
            It is possible to declare option-based list with \p cds::container::skip_list::make_traits metafunction
            istead of \p Traits template argument.

        @warning The skip-list requires up to 67 hazard pointers that may be critical for some GCs for which
            the guard count is limited (like as \p gc::HP). Those GCs should be explicitly initialized with
            hazard pointer enough: \code cds::gc::HP myhp( 67 ) \endcode. Otherwise an run-time exception may be raised
            when you try to create skip-list object.

        @note There are several specializations of \p %SkipListSet for each \p GC. You should include:
        - <tt><cds/container/skip_list_set_hp.h></tt> for \p gc::HP garbage collector
        - <tt><cds/container/skip_list_set_dhp.h></tt> for \p gc::DHP garbage collector
        - <tt><cds/container/skip_list_set_rcu.h></tt> for \ref cds_nonintrusive_SkipListSet_rcu "RCU type"
        - <tt><cds/container/skip_list_set_nogc.h></tt> for \ref cds_nonintrusive_SkipListSet_nogc "non-deletable SkipListSet"

        <b>Iterators</b>

        The class supports a forward iterator (\ref iterator and \ref const_iterator).
        The iteration is ordered.
        The iterator object is thread-safe: the element pointed by the iterator object is guarded,
        so, the element cannot be reclaimed while the iterator object is alive.
        However, passing an iterator object between threads is dangerous.

        \warning Due to concurrent nature of skip-list set it is not guarantee that you can iterate
        all elements in the set: any concurrent deletion can exclude the element
        pointed by the iterator from the set, and your iteration can be terminated
        before end of the set. Therefore, such iteration is more suitable for debugging purpose only

        Remember, each iterator object requires 2 additional hazard pointers, that may be
        a limited resource for \p GC like \p gc::HP (for \p gc::DHP the count of
        guards is unlimited).

        The iterator class supports the following minimalistic interface:
        \code
        struct iterator {
            // Default ctor
            iterator();

            // Copy ctor
            iterator( iterator const& s);

            value_type * operator ->() const;
            value_type& operator *() const;

            // Pre-increment
            iterator& operator ++();

            // Copy assignment
            iterator& operator = (const iterator& src);

            bool operator ==(iterator const& i ) const;
            bool operator !=(iterator const& i ) const;
        };
        \endcode
        Note, the iterator object returned by \p end(), \p cend() member functions points to \p nullptr and should not be dereferenced.
    */
    template <
        typename GC,
        typename T,
#ifdef CDS_DOXYGEN_INVOKED
        typename Traits = skip_list::traits
#else
        typename Traits
#endif
    >
    class SkipListSet:
#ifdef CDS_DOXYGEN_INVOKED
        protected intrusive::SkipListSet< GC, T, Traits >
#else
        protected details::make_skip_list_set< GC, T, Traits >::type
#endif
    {
        //@cond
        typedef details::make_skip_list_set< GC, T, Traits > maker;
        typedef typename maker::type base_class;
        //@endcond
    public:
        typedef GC     gc;          ///< Garbage collector used
        typedef T      value_type;  ///< @anchor cds_containewr_SkipListSet_value_type Value type to be stored in the set
        typedef Traits traits;      ///< Options specified

        typedef typename base_class::back_off     back_off;       ///< Back-off strategy
        typedef typename traits::allocator        allocator_type; ///< Allocator type used for allocate/deallocate the skip-list nodes
        typedef typename base_class::item_counter item_counter;   ///< Item counting policy used
        typedef typename maker::key_comparator    key_comparator; ///< key comparison functor
        typedef typename base_class::memory_model memory_model;   ///< Memory ordering. See cds::opt::memory_model option
        typedef typename traits::random_level_generator random_level_generator; ///< random level generator
        typedef typename traits::stat             stat;           ///< internal statistics type

        static size_t const c_nHazardPtrCount = base_class::c_nHazardPtrCount; ///< Count of hazard pointer required for the skip-list

    protected:
        //@cond
        typedef typename maker::node_type           node_type;
        typedef typename maker::node_allocator      node_allocator;

        typedef std::unique_ptr< node_type, typename maker::node_deallocator >    scoped_node_ptr;
        //@endcond

    public:
        /// Guarded pointer
        typedef typename gc::template guarded_ptr< node_type, value_type, details::guarded_ptr_cast_set<node_type, value_type> > guarded_ptr;

    protected:
        //@cond
        unsigned int random_level()
        {
            return base_class::random_level();
        }
        //@endcond

    public:
        /// Default ctor
        SkipListSet()
            : base_class()
        {}

        /// Destructor destroys the set object
        ~SkipListSet()
        {}

    public:
    ///@name Forward iterators (only for debugging purpose)
    //@{
        /// Iterator type
        /**
            The forward iterator has some features:
            - it has no post-increment operator
            - to protect the value, the iterator contains a GC-specific guard + another guard is required locally for increment operator.
              For some GC (like as \p gc::HP), a guard is a limited resource per thread, so an exception (or assertion) "no free guard"
              may be thrown if the limit of guard count per thread is exceeded.
            - The iterator cannot be moved across thread boundary because it contains thread-private GC's guard.
            - Iterator ensures thread-safety even if you delete the item the iterator points to. However, in case of concurrent
              deleting operations there is no guarantee that you iterate all item in the list.
              Moreover, a crash is possible when you try to iterate the next element that has been deleted by concurrent thread.

            @warning Use this iterator on the concurrent container for debugging purpose only.

            The iterator interface:
            \code
            class iterator {
            public:
                // Default constructor
                iterator();

                // Copy construtor
                iterator( iterator const& src );

                // Dereference operator
                value_type * operator ->() const;

                // Dereference operator
                value_type& operator *() const;

                // Preincrement operator
                iterator& operator ++();

                // Assignment operator
                iterator& operator = (iterator const& src);

                // Equality operators
                bool operator ==(iterator const& i ) const;
                bool operator !=(iterator const& i ) const;
            };
            \endcode
        */
        typedef skip_list::details::iterator< typename base_class::iterator >  iterator;

        /// Const iterator type
        typedef skip_list::details::iterator< typename base_class::const_iterator >   const_iterator;

        /// Returns a forward iterator addressing the first element in a set
        iterator begin()
        {
            return iterator( base_class::begin());
        }

        /// Returns a forward const iterator addressing the first element in a set
        const_iterator begin() const
        {
            return const_iterator( base_class::begin());
        }

        /// Returns a forward const iterator addressing the first element in a set
        const_iterator cbegin() const
        {
            return const_iterator( base_class::cbegin());
        }

        /// Returns a forward iterator that addresses the location succeeding the last element in a set.
        iterator end()
        {
            return iterator( base_class::end());
        }

        /// Returns a forward const iterator that addresses the location succeeding the last element in a set.
        const_iterator end() const
        {
            return const_iterator( base_class::end());
        }

        /// Returns a forward const iterator that addresses the location succeeding the last element in a set.
        const_iterator cend() const
        {
            return const_iterator( base_class::cend());
        }
    //@}

    public:
        /// Inserts new node
        /**
            The function creates a node with copy of \p val value
            and then inserts the node created into the set.

            The type \p Q should contain as minimum the complete key for the node.
            The object of \ref value_type should be constructible from a value of type \p Q.
            In trivial case, \p Q is equal to \ref value_type.

            Returns \p true if \p val is inserted into the set, \p false otherwise.
        */
        template <typename Q>
        bool insert( Q const& val )
        {
            scoped_node_ptr sp( node_allocator().New( random_level(), val ));
            if ( base_class::insert( *sp.get())) {
                sp.release();
                return true;
            }
            return false;
        }

        /// Inserts new node
        /**
            The function allows to split creating of new item into two part:
            - create item with key only
            - insert new item into the set
            - if inserting is success, calls  \p f functor to initialize value-fields of \p val.

            The functor signature is:
            \code
                void func( value_type& val );
            \endcode
            where \p val is the item inserted. User-defined functor \p f should guarantee that during changing
            \p val no any other changes could be made on this set's item by concurrent threads.
            The user-defined functor is called only if the inserting is success.
        */
        template <typename Q, typename Func>
        bool insert( Q const& val, Func f )
        {
            scoped_node_ptr sp( node_allocator().New( random_level(), val ));
            if ( base_class::insert( *sp.get(), [&f]( node_type& v ) { f( v.m_Value ); } )) {
                sp.release();
                return true;
            }
            return false;
        }

        /// Updates the item
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the \p val key not found in the set, then the new item created from \p val
            will be inserted into the set iff \p bInsert is \p true.
            Otherwise, if \p val is found, the functor \p func will be called with the item found.

            The functor \p Func signature:
            \code
                struct my_functor {
                    void operator()( bool bNew, value_type& item, const Q& val );
                };
            \endcode
            where:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the set
            - \p val - argument \p key passed into the \p %update() function

            The functor may change non-key fields of the \p item; however, \p func must guarantee
            that during changing no any other modifications could be made on this item by concurrent threads.

            Returns <tt> std::pair<bool, bool> </tt> where \p first is \p true if operation is successful,
            i.e. the item has been inserted or updated,
            \p second is \p true if new item has been added or \p false if the item with key equal to \p val
            already exists.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting"
        */
        template <typename Q, typename Func>
        std::pair<bool, bool> update( const Q& val, Func func, bool bInsert = true )
        {
            scoped_node_ptr sp( node_allocator().New( random_level(), val ));
            std::pair<bool, bool> bRes = base_class::update( *sp,
                [&func, &val](bool bNew, node_type& node, node_type&){ func( bNew, node.m_Value, val ); },
                bInsert );
            if ( bRes.first && bRes.second )
                sp.release();
            return bRes;
        }
        //@cond
        template <typename Q, typename Func>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<bool, bool> ensure( const Q& val, Func func )
        {
            return update( val, func, true );
        }
        //@endcond

        /// Inserts data of type \p value_type created in-place from <tt>std::forward<Args>(args)...</tt>
        /**
            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename... Args>
        bool emplace( Args&&... args )
        {
            scoped_node_ptr sp( node_allocator().New( random_level(), std::forward<Args>(args)... ));
            if ( base_class::insert( *sp.get())) {
                sp.release();
                return true;
            }
            return false;
        }

        /// Delete \p key from the set
        /** \anchor cds_nonintrusive_SkipListSet_erase_val

            The set item comparator should be able to compare the type \p value_type
            and the type \p Q.

            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename Q>
        bool erase( Q const& key )
        {
            return base_class::erase( key );
        }

        /// Deletes the item from the set using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_SkipListSet_erase_val "erase(Q const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        bool erase_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return base_class::erase_with( key, cds::details::predicate_wrapper< node_type, Less, typename maker::value_accessor >());
        }

        /// Delete \p key from the set
        /** \anchor cds_nonintrusive_SkipListSet_erase_func

            The function searches an item with key \p key, calls \p f functor
            and deletes the item. If \p key is not found, the functor is not called.

            The functor \p Func interface:
            \code
            struct extractor {
                void operator()(value_type const& val);
            };
            \endcode

            Since the key of \p value_type is not explicitly specified,
            template parameter \p Q defines the key type to search in the list.
            The list item comparator should be able to compare the type \p T of list item
            and the type \p Q.

            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename Q, typename Func>
        bool erase( Q const& key, Func f )
        {
            return base_class::erase( key, [&f]( node_type const& node) { f( node.m_Value ); } );
        }

        /// Deletes the item from the set using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_SkipListSet_erase_func "erase(Q const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less, typename Func>
        bool erase_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return base_class::erase_with( key, cds::details::predicate_wrapper< node_type, Less, typename maker::value_accessor >(),
                [&f]( node_type const& node) { f( node.m_Value ); } );
        }

        /// Extracts the item from the set with specified \p key
        /** \anchor cds_nonintrusive_SkipListSet_hp_extract
            The function searches an item with key equal to \p key in the set,
            unlinks it from the set, and returns it as \p guarded_ptr.
            If \p key is not found the function returns an empty guarded pointer.

            Note the compare functor should accept a parameter of type \p Q that can be not the same as \p value_type.

            The item extracted is freed automatically by garbage collector \p GC
            when returned \p guarded_ptr object will be destroyed or released.
            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.

            Usage:
            \code
            typedef cds::container::SkipListSet< cds::gc::HP, foo, my_traits >  skip_list;
            skip_list theList;
            // ...
            {
                skip_list::guarded_ptr gp(theList.extract( 5 ))
                if (  gp ) {
                    // Deal with gp
                    // ...
                }
                // Destructor of gp releases internal HP guard and frees the pointer
            }
            \endcode
        */
        template <typename Q>
        guarded_ptr extract( Q const& key )
        {
            return base_class::extract_( key, typename base_class::key_comparator());
        }

        /// Extracts the item from the set with comparing functor \p pred
        /**
            The function is an analog of \ref cds_nonintrusive_SkipListSet_hp_extract "extract(Q const&)"
            but \p pred predicate is used for key comparing.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        guarded_ptr extract_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            typedef cds::details::predicate_wrapper< node_type, Less, typename maker::value_accessor >  wrapped_less;
            return base_class::extract_( key, cds::opt::details::make_comparator_from_less<wrapped_less>());
        }

        /// Extracts an item with minimal key from the set
        /**
            The function searches an item with minimal key, unlinks it, and returns pointer to the item found as \p guarded_ptr.
            If the skip-list is empty the function returns an empty guarded pointer.

            The item extracted is freed automatically by garbage collector \p GC
            when returned \p guarded_ptr object will be destroyed or released.
            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.

            Usage:
            \code
            typedef cds::continer::SkipListSet< cds::gc::HP, foo, my_traits >  skip_list;
            skip_list theList;
            // ...
            {
                skip_list::guarded_ptr gp( theList.extract_min());
                if ( gp ) {
                    // Deal with gp
                    //...
                }
                // Destructor of gp releases internal HP guard and then frees the pointer
            }
            \endcode
        */
        guarded_ptr extract_min()
        {
            return base_class::extract_min_();
        }

        /// Extracts an item with maximal key from the set
        /**
            The function searches an item with maximal key, unlinks it, and returns the pointer to item found as \p guarded_ptr.
            If the skip-list is empty the function returns an empty guarded pointer.

            The item found is freed by garbage collector \p GC automatically
            when returned \p guarded_ptr object will be destroyed or released.
            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.

            Usage:
            \code
            typedef cds::container::SkipListSet< cds::gc::HP, foo, my_traits >  skip_list;
            skip_list theList;
            // ...
            {
                skip_list::guarded_ptr gp( theList.extract_max());
                if ( gp ) {
                    // Deal with gp
                    //...
                }
                // Destructor of gp releases internal HP guard and then frees the pointer
            }
            \endcode
        */
        guarded_ptr extract_max()
        {
            return base_class::extract_max_();
        }

        /// Find the \p key
        /** \anchor cds_nonintrusive_SkipListSet_find_func

            The function searches the item with key equal to \p key and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item, Q& key );
            };
            \endcode
            where \p item is the item found, \p key is the <tt>find</tt> function argument.

            The functor may change non-key fields of \p item. Note that the functor is only guarantee
            that \p item cannot be disposed during functor is executing.
            The functor does not serialize simultaneous access to the set's \p item. If such access is
            possible you must provide your own synchronization schema on item level to exclude unsafe item modifications.

            Note the hash functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that may be not the same as \p value_type.

            The function returns \p true if \p key is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q& key, Func f )
        {
            return base_class::find( key, [&f]( node_type& node, Q& v ) { f( node.m_Value, v ); });
        }
        //@cond
        template <typename Q, typename Func>
        bool find( Q const& key, Func f )
        {
            return base_class::find( key, [&f]( node_type& node, Q& v ) { f( node.m_Value, v ); } );
        }
        //@endcond

        /// Finds \p key using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_SkipListSet_find_func "find(Q&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return base_class::find_with( key, cds::details::predicate_wrapper< node_type, Less, typename maker::value_accessor >(),
                [&f]( node_type& node, Q& v ) { f( node.m_Value, v ); } );
        }
        //@cond
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return base_class::find_with( key, cds::details::predicate_wrapper< node_type, Less, typename maker::value_accessor >(),
                                          [&f]( node_type& node, Q const& v ) { f( node.m_Value, v ); } );
        }
        //@endcond

        /// Checks whether the set contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.
        */
        template <typename Q>
        bool contains( Q const& key )
        {
            return base_class::contains( key );
        }
        //@cond
        template <typename Q>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find( Q const& key )
        {
            return contains( key );
        }
        //@endcond

        /// Checks whether the set contains \p key using \p pred predicate for searching
        /**
            The function is similar to <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        bool contains( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return base_class::contains( key, cds::details::predicate_wrapper< node_type, Less, typename maker::value_accessor >());
        }
        //@cond
        template <typename Q, typename Less>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find_with( Q const& key, Less pred )
        {
            return contains( key, pred );
        }
        //@endcond

        /// Finds \p key and return the item found
        /** \anchor cds_nonintrusive_SkipListSet_hp_get
            The function searches the item with key equal to \p key
            and returns a guarded pointer to the item found.
            If \p key is not found the function returns an empty guarded pointer.

            It is safe when a concurrent thread erases the item returned in \p result guarded pointer.
            In this case the item will be freed later by garbage collector \p GC automatically
            when \p guarded_ptr object will be destroyed or released.
            @note Each \p guarded_ptr object uses one GC's guard which can be limited resource.

            Usage:
            \code
            typedef cds::container::SkipListSet< cds::gc::HP, foo, my_traits >  skip_list;
            skip_list theList;
            // ...
            {
                skip_list::guarded_ptr gp( theList.get( 5 ));
                if ( theList.get( 5 )) {
                    // Deal with gp
                    //...
                }
                // Destructor of guarded_ptr releases internal HP guard
            }
            \endcode

            Note the compare functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that can be not the same as \p value_type.
        */
        template <typename Q>
        guarded_ptr get( Q const& key )
        {
            return base_class::get_with_( key, typename base_class::key_comparator());
        }

        /// Finds \p key and return the item found
        /**
            The function is an analog of \ref cds_nonintrusive_SkipListSet_hp_get "get(Q const&)"
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        guarded_ptr get_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            typedef cds::details::predicate_wrapper< node_type, Less, typename maker::value_accessor >  wrapped_less;
            return base_class::get_with_( key, cds::opt::details::make_comparator_from_less< wrapped_less >());
        }

        /// Clears the set (not atomic).
        /**
            The function deletes all items from the set.
            The function is not atomic, thus, in multi-threaded environment with parallel insertions
            this sequence
            \code
            set.clear();
            assert( set.empty());
            \endcode
            the assertion could be raised.

            For each item the \ref disposer provided by \p Traits template parameter will be called.
        */
        void clear()
        {
            base_class::clear();
        }

        /// Checks if the set is empty
        bool empty() const
        {
            return base_class::empty();
        }

        /// Returns item count in the set
        /**
            The value returned depends on item counter type provided by \p Traits template parameter.
            If it is \p atomicity::empty_item_counter this function always returns 0.
            Therefore, the function is not suitable for checking the set emptiness, use \p empty()
            member function for this purpose.
        */
        size_t size() const
        {
            return base_class::size();
        }

        /// Returns const reference to internal statistics
        stat const& statistics() const
        {
            return base_class::statistics();
        }
    };

}} // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_IMPL_SKIP_LIST_SET_H
