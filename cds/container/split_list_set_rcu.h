// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_SPLIT_LIST_SET_RCU_H
#define CDSLIB_CONTAINER_SPLIT_LIST_SET_RCU_H

#include <cds/intrusive/split_list_rcu.h>
#include <cds/container/details/make_split_list_set.h>
#include <cds/urcu/exempt_ptr.h>

namespace cds { namespace container {

    //@cond
    namespace split_list { namespace details {

        template <
            typename T,
            class OrdList,
            typename OrdListTag
        >
        class make_raw_ptr;

#ifdef CDSLIB_CONTAINER_DETAILS_MICHAEL_LIST_BASE_H
        template <typename T, class RawPtr>
        class make_raw_ptr< T, RawPtr, cds::container::michael_list_tag >
        {
            typedef RawPtr intrusive_raw_ptr;
            typedef typename intrusive_raw_ptr::value_type node_type;
            typedef T value_type;

            struct raw_ptr_converter
            {
                value_type * operator()( node_type * p ) const
                {
                   return p ? &p->m_Value : nullptr;
                }

                value_type& operator()( node_type& n ) const
                {
                    return n.m_Value;
                }

                value_type const& operator()( node_type const& n ) const
                {
                    return n.m_Value;
                }
            };
        public:
            typedef cds::urcu::raw_ptr_adaptor< value_type, intrusive_raw_ptr, raw_ptr_converter > raw_ptr;

            static raw_ptr make( intrusive_raw_ptr&& p )
            {
                return raw_ptr(std::move( p ));
            }
        };
#endif

#ifdef CDSLIB_CONTAINER_DETAILS_LAZY_LIST_BASE_H
        template <typename T, typename RawPtr>
        class make_raw_ptr< T, RawPtr, cds::container::lazy_list_tag >
        {
            typedef RawPtr node_type_pointer;
            typedef T value_type;

        public:
            typedef value_type * raw_ptr;

            static raw_ptr make( node_type_pointer p )
            {
                return p ? &p->m_Value : nullptr;
            }
        };
#endif
    }} //namespace split_list::details
    //@endcond

    /// Split-ordered list set (template specialization for \ref cds_urcu_desc "RCU")
    /** @ingroup cds_nonintrusive_set
        \anchor cds_nonintrusive_SplitListSet_rcu

        Hash table implementation based on split-ordered list algorithm discovered by Ori Shalev and Nir Shavit, see
        - [2003] Ori Shalev, Nir Shavit "Split-Ordered Lists - Lock-free Resizable Hash Tables"
        - [2008] Nir Shavit "The Art of Multiprocessor Programming"

        See \p intrusive::SplitListSet for a brief description of the split-list algorithm.

        Template parameters:
        - \p RCU - one of \ref cds_urcu_gc "RCU type"
        - \p T - type of the value to be stored in the split-list.
        - \p Traits - type traits, default is \p split_list::traits. Instead of declaring \p split_list::traits -based
            struct you can apply option-based notation with \p split_list::make_traits metafunction.

        <b>Iterators</b>

        The class supports a forward iterator (\ref iterator and \ref const_iterator).
        The iteration is unordered.

        You may iterate over split-list set items only under RCU lock.
        Only in this case the iterator is thread-safe since
        while RCU is locked any set's item cannot be reclaimed.

        @warning The iterator object cannot be passed between threads

        \warning Due to concurrent nature of skip-list set it is not guarantee that you can iterate
        all elements in the set: any concurrent deletion can exclude the element
        pointed by the iterator from the set, and your iteration can be terminated
        before end of the set. Therefore, such iteration is more suitable for debugging purposes

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

        \par Usage

        You should decide what garbage collector you want, and what ordered list you want to use. Split-ordered list
        is an original data structure based on an ordered list. Suppose, you want construct split-list set based on \p cds::urcu::general_buffered<> GC
        and \p LazyList as ordered list implementation. So, you beginning your program with following include:
        \code
        #include <cds/urcu/general_buffered.h>
        #include <cds/container/lazy_list_rcu.h>
        #include <cds/container/split_list_set_rcu.h>

        namespace cc = cds::container;

        // The data belonged to split-ordered list
        sturuct foo {
            int     nKey;   // key field
            std::string strValue    ;   // value field
        };
        \endcode
        The inclusion order is important:
        - first, include one of \ref cds_urcu_gc "RCU implementation" (<tt>cds/urcu/general_buffered.h</tt> in our case)
        - second, include file for ordered-list implementation (for this example, <tt>cds/container/lazy_list_rcu.h</tt>),
        - then, the header for RCU-based split-list set <tt>cds/container/split_list_set_rcu.h</tt>.

        Now, you should declare traits for split-list set. The main parts of traits are a hash functor for the set and a comparing functor for ordered list.
        Note that we define several function in \p foo_hash and \p foo_less functors for different argument types since we want call our \p %SplitListSet
        object by the key of type \p int and by the value of type \p foo.

        The second attention: instead of using \p %LazyList in \p %SplitListSet traits we use \p cds::contaner::lazy_list_tag tag for the lazy list.
        The split-list requires significant support from underlying ordered list class and it is not good idea to dive you
        into deep implementation details of split-list and ordered list interrelations. The tag paradigm simplifies split-list interface.

        \code
        // foo hash functor
        struct foo_hash {
            size_t operator()( int key ) const { return std::hash( key ) ; }
            size_t operator()( foo const& item ) const { return std::hash( item.nKey ) ; }
        };

        // foo comparator
        struct foo_less {
            bool operator()(int i, foo const& f ) const { return i < f.nKey ; }
            bool operator()(foo const& f, int i ) const { return f.nKey < i ; }
            bool operator()(foo const& f1, foo const& f2) const { return f1.nKey < f2.nKey; }
        };

        // SplitListSet traits
        struct foo_set_traits: public cc::split_list::traits
        {
            typedef cc::lazy_list_tag   ordered_list    ;   // what type of ordered list we want to use
            typedef foo_hash            hash            ;   // hash functor for our data stored in split-list set

            // Type traits for our LazyList class
            struct ordered_list_traits: public cc::lazy_list::traits
            {
                typedef foo_less less   ;   // use our foo_less as comparator to order list nodes
            };
        };
        \endcode

        Now you are ready to declare our set class based on \p %SplitListSet:
        \code
        typedef cc::SplitListSet< cds::urcu::gc<cds::urcu::general_buffered<> >, foo, foo_set_traits > foo_set;
        \endcode

        You may use the modern option-based declaration instead of classic type-traits-based one:
        \code
        typedef cc::SplitListSet<
            cds::urcu::gc<cds::urcu::general_buffered<> >   // RCU type used
            ,foo                                            // type of data stored
            ,cc::split_list::make_traits<      // metafunction to build split-list traits
                cc::split_list::ordered_list<cc::lazy_list_tag>     // tag for underlying ordered list implementation
                ,cc::opt::hash< foo_hash >              // hash functor
                ,cc::split_list::ordered_list_traits<   // ordered list traits
                    cc::lazy_list::make_traits<         // metafunction to build lazy list traits
                        cc::opt::less< foo_less >       // less-based compare functor
                    >::type
                >
            >::type
        >  foo_set;
        \endcode
        In case of option-based declaration using \p split_list::make_traits metafunction
        the struct \p foo_set_traits is not required.

        Now, the set of type \p foo_set is ready to use in your program.

        Note that in this example we show only mandatory \p traits parts, optional ones is the default and they are inherited
        from \p container::split_list::traits.
        There are many other options for deep tuning of the split-list and ordered-list containers.
    */
    template <
        class RCU,
        class T,
#ifdef CDS_DOXYGEN_INVOKED
        class Traits = split_list::traits
#else
        class Traits
#endif
    >
    class SplitListSet< cds::urcu::gc< RCU >, T, Traits >:
#ifdef CDS_DOXYGEN_INVOKED
        protected intrusive::SplitListSet< cds::urcu::gc< RCU >, T, typename Traits::ordered_list, Traits >
#else
        protected details::make_split_list_set< cds::urcu::gc< RCU >, T, typename Traits::ordered_list, split_list::details::wrap_set_traits<T, Traits> >::type
#endif
    {
    protected:
        //@cond
        typedef details::make_split_list_set< cds::urcu::gc< RCU >, T, typename Traits::ordered_list, split_list::details::wrap_set_traits<T, Traits> > maker;
        typedef typename maker::type  base_class;
        //@endcond

    public:
        typedef cds::urcu::gc< RCU >  gc; ///< RCU-based garbage collector
        typedef T       value_type; ///< Type of value to be storedin the set
        typedef Traits  traits;    ///< \p Traits template argument

        // Note: ordered_list is not real ordered list type. Actual type is base_class::ordered_list
        typedef typename maker::ordered_list        ordered_list;   ///< Underlying ordered list class
        typedef typename base_class::key_comparator key_comparator; ///< key compare functor

        /// Hash functor for \ref value_type and all its derivatives that you use
        typedef typename base_class::hash           hash;
        typedef typename base_class::item_counter   item_counter; ///< Item counter type
        typedef typename base_class::stat           stat; ///< Internal statistics

        typedef typename base_class::rcu_lock      rcu_lock   ; ///< RCU scoped lock
        /// Group of \p extract_xxx functions require external locking if underlying ordered list requires that
        static constexpr const bool c_bExtractLockExternal = base_class::c_bExtractLockExternal;

    protected:
        //@cond
        typedef typename maker::cxx_node_allocator    cxx_node_allocator;
        typedef typename maker::node_type             node_type;
        //@endcond

    public:
        /// pointer to extracted node
        using exempt_ptr = cds::urcu::exempt_ptr< gc, node_type, value_type, typename maker::ordered_list_traits::disposer >;
#   ifdef CDS_DOXYGEN_INVOKED
        /// pointer to the node for \p get() function
        /**
            For \p LazyList, \p %raw_ptr is just pointer to \p value_type.

            For \p MichaelList, \p %raw_ptr is \p cds::urcu::raw_ptr object giving access to \p value_type.
        */
        typedef implementation_defined raw_ptr;
#   else
    private:
        typedef split_list::details::make_raw_ptr< value_type, typename base_class::ordered_list::raw_ptr, typename traits::ordered_list > raw_ptr_maker;
    public:
        typedef typename raw_ptr_maker::raw_ptr raw_ptr;
#endif

    protected:
        //@cond
        template <typename Q, typename Func>
        bool find_( Q& val, Func f )
        {
            return base_class::find( val, [&f]( node_type& item, Q& v ) { f(item.m_Value, v) ; } );
        }

        template <typename Q, typename Less, typename Func>
        bool find_with_( Q& val, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return base_class::find_with( val, typename maker::template predicate_wrapper<Less>(),
                [&f]( node_type& item, Q& v ) { f(item.m_Value, v) ; } );
        }

        template <typename Q>
        static node_type * alloc_node( Q const& v )
        {
            return cxx_node_allocator().New( v );
        }

        template <typename... Args>
        static node_type * alloc_node( Args&&... args )
        {
            return cxx_node_allocator().MoveNew( std::forward<Args>(args)...);
        }

        static void free_node( node_type * pNode )
        {
            cxx_node_allocator().Delete( pNode );
        }

        struct node_disposer {
            void operator()( node_type * pNode )
            {
                free_node( pNode );
            }
        };
        typedef std::unique_ptr< node_type, node_disposer >     scoped_node_ptr;

        bool insert_node( node_type * pNode )
        {
            assert( pNode != nullptr );
            scoped_node_ptr p(pNode);

            if ( base_class::insert( *pNode )) {
                p.release();
                return true;
            }

            return false;
        }
        //@endcond

    protected:
        //@cond
        template <bool IsConst>
        class iterator_type: protected base_class::template iterator_type<IsConst>
        {
            typedef typename base_class::template iterator_type<IsConst> iterator_base_class;
            friend class SplitListSet;

        public:
            /// Value pointer type (const for const iterator)
            typedef typename cds::details::make_const_type<value_type, IsConst>::pointer   value_ptr;
            /// Value reference type (const for const iterator)
            typedef typename cds::details::make_const_type<value_type, IsConst>::reference value_ref;

        public:
            /// Default ctor
            iterator_type()
            {}

            /// Copy ctor
            iterator_type( iterator_type const& src )
                : iterator_base_class( src )
            {}

        protected:
            explicit iterator_type( iterator_base_class const& src )
                : iterator_base_class( src )
            {}

        public:
            /// Dereference operator
            value_ptr operator ->() const
            {
                return &(iterator_base_class::operator->()->m_Value);
            }

            /// Dereference operator
            value_ref operator *() const
            {
                return iterator_base_class::operator*().m_Value;
            }

            /// Pre-increment
            iterator_type& operator ++()
            {
                iterator_base_class::operator++();
                return *this;
            }

            /// Assignment operator
            iterator_type& operator = (iterator_type const& src)
            {
                iterator_base_class::operator=(src);
                return *this;
            }

            /// Equality operator
            template <bool C>
            bool operator ==(iterator_type<C> const& i ) const
            {
                return iterator_base_class::operator==(i);
            }

            /// Equality operator
            template <bool C>
            bool operator !=(iterator_type<C> const& i ) const
            {
                return iterator_base_class::operator!=(i);
            }
        };
        //@endcond

    public:
        /// Initializes split-ordered list of default capacity
        /**
            The default capacity is defined in bucket table constructor.
            See \p intrusive::split_list::expandable_bucket_table, \p intrusive::split_list::static_bucket_table
            which selects by \p container::split_list::dynamic_bucket_table option.
        */
        SplitListSet()
            : base_class()
        {}

        /// Initializes split-ordered list
        SplitListSet(
            size_t nItemCount           ///< estimated average of item count
            , size_t nLoadFactor = 1    ///< load factor - average item count per bucket. Small integer up to 8, default is 1.
            )
            : base_class( nItemCount, nLoadFactor )
        {}

    public:
    ///@name Forward iterators (thread-safe under RCU lock)
    //@{
        /// Forward iterator
        /**
            The forward iterator for Michael's set is based on \p OrderedList forward iterator and has some features:
            - it has no post-increment operator
            - it iterates items in unordered fashion

            You may safely use iterators in multi-threaded environment only under RCU lock.
            Otherwise, a crash is possible if another thread deletes the element the iterator points to.

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
        typedef iterator_type<false>  iterator;

        /// Forward const iterator
        typedef iterator_type<true>   const_iterator;

        /// Returns a forward iterator addressing the first element in a set
        /**
            For empty set \code begin() == end() \endcode
        */
        iterator begin()
        {
            return iterator( base_class::begin());
        }

        /// Returns an iterator that addresses the location succeeding the last element in a set
        /**
            Do not use the value returned by <tt>end</tt> function to access any item.
            The returned value can be used only to control reaching the end of the set.
            For empty set \code begin() == end() \endcode
        */
        iterator end()
        {
            return iterator( base_class::end());
        }

        /// Returns a forward const iterator addressing the first element in a set
        const_iterator begin() const
        {
            return cbegin();
        }
        /// Returns a forward const iterator addressing the first element in a set
        const_iterator cbegin() const
        {
            return const_iterator( base_class::cbegin());
        }

        /// Returns an const iterator that addresses the location succeeding the last element in a set
        const_iterator end() const
        {
            return cend();
        }
        /// Returns an const iterator that addresses the location succeeding the last element in a set
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
            The object of \p value_type should be constructible from a value of type \p Q.
            In trivial case, \p Q is equal to \p value_type.

            The function applies RCU lock internally.

            Returns \p true if \p val is inserted into the set, \p false otherwise.
        */
        template <typename Q>
        bool insert( Q const& val )
        {
            return insert_node( alloc_node( val ));
        }

        /// Inserts new node
        /**
            The function allows to split creating of new item into two part:
            - create item with key only
            - insert new item into the set
            - if inserting is success, calls  \p f functor to initialize value-field of \p val.

            The functor signature is:
            \code
                void func( value_type& val );
            \endcode
            where \p val is the item inserted. User-defined functor \p f should guarantee that during changing
            \p val no any other changes could be made on this set's item by concurrent threads.
            The user-defined functor is called only if the inserting is success.

            The function applies RCU lock internally.
        */
        template <typename Q, typename Func>
        bool insert( Q const& key, Func f )
        {
            scoped_node_ptr pNode( alloc_node( key ));

            if ( base_class::insert( *pNode, [&f](node_type& node) { f( node.m_Value ) ; } )) {
                pNode.release();
                return true;
            }
            return false;
        }

        /// Inserts data of type \p value_type created from \p args
        /**
            Returns \p true if inserting successful, \p false otherwise.

            The function applies RCU lock internally.
        */
        template <typename... Args>
        bool emplace( Args&&... args )
        {
            return insert_node( alloc_node( std::forward<Args>(args)...));
        }

        /// Updates an element with given \p val
        /**
            The operation performs inserting or changing data with lock-free manner.

            If the \p val key not found in the set, then the new item created from \p val
            is inserted into the set. Otherwise, the functor \p func is called with the item found.
            The functor \p Func signature is:
            \code
                struct my_functor {
                    void operator()( bool bNew, value_type& item, const Q& val );
                };
            \endcode

            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the set
            - \p val - argument \p val passed into the \p %ensure() function

            The functor may change non-key fields of the \p item; however, \p func must guarantee
            that during changing no any other modifications could be made on this item by concurrent threads.

            The function applies RCU lock internally.

            Returns <tt> std::pair<bool, bool> </tt> where \p first is true if operation is successful,
            \p second is true if new item has been added or \p false if the item with \p key
            already is in the set.
        */
        /// Updates the node
        /**
            The operation performs inserting or changing data with lock-free manner.

            If \p key is not found in the set, then \p key is inserted iff \p bAllowInsert is \p true.
            Otherwise, the functor \p func is called with item found.

            The functor signature is:
            \code
                struct my_functor {
                    void operator()( bool bNew, value_type& item, const Q& val );
                };
            \endcode

            with arguments:
            - \p bNew - \p true if the item has been inserted, \p false otherwise
            - \p item - item of the set
            - \p val - argument \p val passed into the \p %update() function

            The functor may change non-key fields of the \p item.

            The function applies RCU lock internally.

            Returns <tt> std::pair<bool, bool> </tt> where \p first is true if operation is successful,
            \p second is true if new item has been added or \p false if the item with \p key
            already is in the map.

            @warning For \ref cds_intrusive_MichaelList_rcu "MichaelList" as the bucket see \ref cds_intrusive_item_creating "insert item troubleshooting".
            \ref cds_intrusive_LazyList_rcu "LazyList" provides exclusive access to inserted item and does not require any node-level
            synchronization.
        */
        template <typename Q, typename Func>
        std::pair<bool, bool> update( Q const& val, Func func, bool bAllowInsert = true )
        {
            scoped_node_ptr pNode( alloc_node( val ));

            std::pair<bool, bool> bRet = base_class::update( *pNode,
                [&func, &val]( bool bNew, node_type& item,  node_type const& /*val*/ ) {
                    func( bNew, item.m_Value, val );
                }, bAllowInsert );
            if ( bRet.first && bRet.second )
                pNode.release();
            return bRet;
        }
        //@cond
        // Dprecated, use update()
        template <typename Q, typename Func>
        std::pair<bool, bool> ensure( Q const& val, Func func )
        {
            return update( val, func, true );
        }
        //@endcond

        /// Deletes \p key from the set
        /** \anchor cds_nonintrusive_SplitListSet_rcu_erase_val

            Template parameter of type \p Q defines the key type searching in the list.
            The set item comparator should be able to compare the values of type \p value_type
            and the type \p Q.

            RCU \p synchronize method can be called. RCU should not be locked.

            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename Q>
        bool erase( Q const& key )
        {
            return base_class::erase( key );
        }

        /// Deletes the item from the set using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_SplitListSet_rcu_erase_val "erase(Q const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        bool erase_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
             return base_class::erase_with( key, typename maker::template predicate_wrapper<Less>());
        }

        /// Deletes \p key from the set
        /** \anchor cds_nonintrusive_SplitListSet_rcu_erase_func

            The function searches an item with key \p key, calls \p f functor
            and deletes the item. If \p key is not found, the functor is not called.

            The functor \p Func interface:
            \code
            struct extractor {
                void operator()(value_type const& val);
            };
            \endcode

            Template parameter of type \p Q defines the key type searching in the list.
            The list item comparator should be able to compare the values of the type \p value_type
            and the type \p Q.

            RCU \p synchronize method can be called. RCU should not be locked.

            Return \p true if key is found and deleted, \p false otherwise
        */
        template <typename Q, typename Func>
        bool erase( Q const& key, Func f )
        {
            return base_class::erase( key, [&f](node_type& node) { f( node.m_Value ); } );
        }

        /// Deletes the item from the set using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_SplitListSet_rcu_erase_func "erase(Q const&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less, typename Func>
        bool erase_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return base_class::erase_with( key, typename maker::template predicate_wrapper<Less>(),
                [&f](node_type& node) { f( node.m_Value ); } );
        }

        /// Extracts an item from the set
        /** \anchor cds_nonintrusive_SplitListSet_rcu_extract
            The function searches an item with key equal to \p key in the set,
            unlinks it from the set, and returns \ref cds::urcu::exempt_ptr "exempt_ptr" pointer to the item found.
            If the item with the key equal to \p key is not found the function returns an empty \p exempt_ptr.

            Depends on \p bucket_type you should or should not lock RCU before calling of this function:
            - for the set based on \ref cds_intrusive_MichaelList_rcu "MichaelList" RCU should not be locked
            - for the set based on \ref cds_intrusive_LazyList_rcu "LazyList" RCU should be locked
            See ordered list implementation for details.

            \code
            typedef cds::urcu::gc< general_buffered<> > rcu;

            // Split-list set based on MichaelList by default
            typedef cds::container::SplitListSet< rcu, Foo > splitlist_set;

            splitlist_set theSet;
            // ...

            splitlist_set::exempt_ptr p;

            // For MichaelList we should not lock RCU

            // Now, you can apply extract function
            p = theSet.extract( 10 );
            if ( p ) {
                // do something with p
                ...
            }

            // We may safely release p here
            // release() passes the pointer to RCU reclamation cycle
            p.release();
            \endcode
        */
        template <typename Q>
        exempt_ptr extract( Q const& key )
        {
            return exempt_ptr( base_class::extract_( key, key_comparator()));
        }

        /// Extracts an item from the set using \p pred predicate for searching
        /**
            The function is an analog of \p extract(Q const&) but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        exempt_ptr extract_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return exempt_ptr( base_class::extract_with_( key, typename maker::template predicate_wrapper<Less>()));
        }

        /// Finds the key \p key
        /** \anchor cds_nonintrusive_SplitListSet_rcu_find_func

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
            should accept a parameter of type \p Q that can be not the same as \p value_type.

            The function makes RCU lock internally.

            The function returns \p true if \p key is found, \p false otherwise.
        */
        template <typename Q, typename Func>
        bool find( Q& key, Func f )
        {
            return find_( key, f );
        }
        //@cond
        template <typename Q, typename Func>
        bool find( Q const& key, Func f )
        {
            return find_( key, f );
        }
        //@endcond

        /// Finds the key \p key using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_SplitListSet_rcu_find_func "find(Q&, Func)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less, typename Func>
        bool find_with( Q& key, Less pred, Func f )
        {
            return find_with_( key, pred, f );
        }
        //@cond
        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& key, Less pred, Func f )
        {
            return find_with_( key, pred, f );
        }
        //@endcond

        /// Checks whether the set contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns \p true if it is found, and \p false otherwise.

            Note the hash functor specified for class \p Traits template parameter
            should accept a parameter of type \p Q that can be not the same as \p value_type.
            Otherwise, you may use \p contains( Q const&, Less pred ) functions with explicit predicate for key comparing.

            The function applies RCU lock internally.
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

        /// Checks whether the map contains \p key using \p pred predicate for searching
        /**
            The function is similar to <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the map.
        */
        template <typename Q, typename Less>
        bool contains( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return base_class::contains( key, typename maker::template predicate_wrapper<Less>());
        }
        //@cond
        template <typename Q, typename Less>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find_with( Q const& key, Less pred )
        {
            return contains( key, pred );
        }
        //@endcond

        /// Finds the key \p key and return the item found
        /** \anchor cds_nonintrusive_SplitListSet_rcu_get
            The function searches the item with key equal to \p key and returns the pointer to item found.
            If \p key is not found it returns \p nullptr.

            Note the compare functor should accept a parameter of type \p Q that can be not the same as \p value_type.

            RCU should be locked before call of this function.
            Returned item is valid only while RCU is locked:
            \code
            typedef cds::urcu::gc< general_buffered<> > rcu;
            typedef cds::container::SplitListSet< rcu, Foo > splitlist_set;
            splitlist_set theSet;
            // ...
            {
                // Lock RCU
                splitlist_set::rcu_lock lock;

                foo * pVal = theSet.get( 5 );
                if ( pVal ) {
                    // Deal with pVal
                    //...
                }
                // Unlock RCU by rcu_lock destructor
                // pVal can be retired by disposer at any time after RCU has been unlocked
            }
            \endcode
        */
        template <typename Q>
        raw_ptr get( Q const& key )
        {
            return raw_ptr_maker::make( base_class::get( key ));
        }

        /// Finds the key \p key and return the item found
        /**
            The function is an analog of \ref cds_nonintrusive_SplitListSet_rcu_get "get(Q const&)"
            but \p pred is used for comparing the keys.

            \p Less functor has the semantics like \p std::less but should take arguments of type \ref value_type and \p Q
            in any order.
            \p pred must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        raw_ptr get_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return raw_ptr_maker::make( base_class::get_with( key, typename maker::template predicate_wrapper<Less>()));
        }

        /// Clears the set (not atomic)
        void clear()
        {
            base_class::clear();
        }

        /// Checks if the set is empty
        /**
            Emptiness is checked by item counting: if item count is zero then assume that the set is empty.
            Thus, the correct item counting feature is an important part of split-list set implementation.
        */
        bool empty() const
        {
            return base_class::empty();
        }

        /// Returns item count in the set
        size_t size() const
        {
            return base_class::size();
        }

        /// Returns internal statistics
        stat const& statistics() const
        {
            return base_class::statistics();
        }

        /// Returns internal statistics for \p ordered_list
        typename ordered_list::stat const& list_statistics() const
        {
            return base_class::list_statistics();
        }
    };
}}  // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_SPLIT_LIST_SET_RCU_H
