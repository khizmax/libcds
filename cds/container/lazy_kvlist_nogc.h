// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_LAZY_KVLIST_NOGC_H
#define CDSLIB_CONTAINER_LAZY_KVLIST_NOGC_H

#include <memory>
#include <cds/container/details/lazy_list_base.h>
#include <cds/intrusive/lazy_list_nogc.h>
#include <cds/container/details/make_lazy_kvlist.h>

namespace cds { namespace container {

    /// Lazy ordered list (key-value pair, template specialization for gc::nogc)
    /** @ingroup cds_nonintrusive_list
        @anchor cds_nonintrusive_LazyKVList_nogc

        This specialization is append-only list when no item
        reclamation may be performed. The class does not support deleting of list's item.

        See @ref cds_nonintrusive_LazyList_gc "cds::container::LazyList<cds::gc::nogc, T, Traits>"
    */
    template <
        typename Key,
        typename Value,
#ifdef CDS_DOXYGEN_INVOKED
        typename Traits = lazy_list::traits
#else
        typename Traits
#endif
    >
    class LazyKVList<gc::nogc, Key, Value, Traits>:
#ifdef CDS_DOXYGEN_INVOKED
        protected intrusive::LazyList< gc::nogc, implementation_defined, Traits >
#else
        protected details::make_lazy_kvlist< cds::gc::nogc, Key, Value, Traits >::type
#endif
    {
        //@cond
        typedef details::make_lazy_kvlist< cds::gc::nogc, Key, Value, Traits > maker;
        typedef typename maker::type  base_class;
        //@endcond

    public:
        typedef Traits traits;    ///< List traits
        typedef cds::gc::nogc gc; ///< Garbage collector
#ifdef CDS_DOXYGEN_INVOKED
        typedef Key                                 key_type        ;   ///< Key type
        typedef Value                               mapped_type     ;   ///< Type of value stored in the list
        typedef std::pair<key_type const, mapped_type> value_type   ;   ///< key/value pair stored in the list
#else
        typedef typename maker::key_type    key_type;
        typedef typename maker::mapped_type mapped_type;
        typedef typename maker::value_type  value_type;
#endif
        typedef typename base_class::back_off     back_off;       ///< Back-off strategy used
        typedef typename maker::allocator_type    allocator_type; ///< Allocator type used for allocate/deallocate the nodes
        typedef typename base_class::item_counter item_counter;   ///< Item counting policy used
        typedef typename maker::key_comparator    key_comparator; ///< key comparison functor
        typedef typename base_class::memory_model memory_model;   ///< Memory ordering. See cds::opt::memory_model option
        typedef typename base_class::stat         stat;           ///< Internal statistics
        static constexpr bool const c_bSort = base_class::c_bSort; ///< List type: ordered (\p true) or unordered (\p false)

        //@cond
        // Rebind traits (split-list support)
        template <typename... Options>
        struct rebind_traits {
            typedef LazyKVList<
                gc
                , key_type, mapped_type
                , typename cds::opt::make_options< traits, Options...>::type
            > type;
        };

        // Stat selector
        template <typename Stat>
        using select_stat_wrapper = typename base_class::template select_stat_wrapper< Stat >;
        //@endcond

    protected:
        //@cond
        typedef typename base_class::value_type     node_type;
        typedef typename maker::cxx_allocator       cxx_allocator;
        typedef typename maker::node_deallocator    node_deallocator;
        typedef typename base_class::key_comparator intrusive_key_comparator;
        typedef typename base_class::node_type      head_type;
        //@endcond

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
            return base_class::m_Head;
        }

        head_type const& head() const
        {
            return base_class::m_Head;
        }

        head_type& tail()
        {
            return base_class::m_Tail;
        }

        head_type const& tail() const
        {
            return base_class::m_Tail;
        }
        //@endcond

    protected:
        //@cond
        template <bool IsConst>
        class iterator_type: protected base_class::template iterator_type<IsConst>
        {
            typedef typename base_class::template iterator_type<IsConst>    iterator_base;

            iterator_type( head_type const& refNode )
                : iterator_base( const_cast<head_type *>( &refNode ))
            {}

            explicit iterator_type( const iterator_base& it )
                : iterator_base( it )
            {}

            friend class LazyKVList;

        protected:
            explicit iterator_type( node_type& pNode )
                : iterator_base( &pNode )
            {}

        public:
            typedef typename cds::details::make_const_type<mapped_type, IsConst>::reference  value_ref;
            typedef typename cds::details::make_const_type<mapped_type, IsConst>::pointer    value_ptr;

            typedef typename cds::details::make_const_type<value_type,  IsConst>::reference  pair_ref;
            typedef typename cds::details::make_const_type<value_type,  IsConst>::pointer    pair_ptr;

            iterator_type()
                : iterator_base()
            {}

            iterator_type( const iterator_type& src )
                : iterator_base( src )
            {}

            key_type const& key() const
            {
                typename iterator_base::value_ptr p = iterator_base::operator ->();
                assert( p != nullptr );
                return p->m_Data.first;
            }

            value_ref val() const
            {
                typename iterator_base::value_ptr p = iterator_base::operator ->();
                assert( p != nullptr );
                return p->m_Data.second;
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

            /// Pre-increment
            iterator_type& operator ++()
            {
                iterator_base::operator ++();
                return *this;
            }

            /// Post-increment
            iterator_type operator ++(int)
            {
                return iterator_base::operator ++(0);
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
    ///@name Forward iterators
    //@{
        /// Forward iterator
        /**
        The forward iterator is safe: you may use it in multi-threaded enviromnent without any synchronization.

        The forward iterator for lazy list based on \p gc::nogc has pre- and post-increment operators.

            The iterator interface to access item data:
            - <tt> operator -> </tt> - returns a pointer to \p value_type
            - <tt> operator *</tt> - returns a reference (a const reference for \p const_iterator) to \p value_type
            - <tt> const key_type& key() </tt> - returns a key reference for iterator
            - <tt> mapped_type& val() </tt> - retuns a value reference for iterator (const reference for \p const_iterator)

            For both functions the iterator should not be equal to \p end()
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
            iterator it( head());
            ++it ;  // skip dummy head
            return it;
        }

        /// Returns an iterator that addresses the location succeeding the last element in a list
        /**
            Do not use the value returned by <tt>end</tt> function to access any item.
            Internally, <tt>end</tt> returning value equals to nullptr.

            The returned value can be used only to control reaching the end of the list.
            For empty list \code begin() == end() \endcode
        */
        iterator end()
        {
            return iterator( tail());
        }

        /// Returns a forward const iterator addressing the first element in a list
        const_iterator begin() const
        {
            const_iterator it( head());
            ++it ;  // skip dummy head
            return it;
        }
        /// Returns a forward const iterator addressing the first element in a list
        const_iterator cbegin() const
        {
            const_iterator it( head());
            ++it ;  // skip dummy head
            return it;
        }

        /// Returns an const iterator that addresses the location succeeding the last element in a list
        const_iterator end() const
        {
            return const_iterator( tail());
        }
        /// Returns an const iterator that addresses the location succeeding the last element in a list
        const_iterator cend() const
        {
            return const_iterator( tail());
        }
    //@}

    protected:
        //@cond
        iterator node_to_iterator( node_type * pNode )
        {
            if ( pNode )
                return iterator( *pNode );
            return end();
        }
        //@endcond

    public:
        /// Default constructor
        LazyKVList()
        {}

        //@cond
        template <typename Stat, typename = std::enable_if<std::is_same<stat, lazy_list::wrapped_stat<Stat>>::value >>
        explicit LazyKVList( Stat& st )
            : base_class( st )
        {}
        //@endcond

        /// Desctructor clears the list
        ~LazyKVList()
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

            Returns an iterator pointed to inserted value, or \p end() if inserting is failed
        */
        template <typename K>
        iterator insert( const K& key )
        {
            return node_to_iterator( insert_at( head(), key ));
        }

        /// Inserts new node with a key and a value
        /**
            The function creates a node with \p key and value \p val, and then inserts the node created into the list.

            Preconditions:
            - The \ref key_type should be constructible from \p key of type \p K.
            - The \ref mapped_type should be constructible from \p val of type \p V.

            Returns an iterator pointed to inserted value, or \p end() if inserting is failed
        */
        template <typename K, typename V>
        iterator insert( const K& key, const V& val )
        {
            // We cannot use insert with functor here
            // because we cannot lock inserted node for updating
            // Therefore, we use separate function
            return node_to_iterator( insert_at( head(), key, val ));
        }

        /// Inserts new node and initialize it by a functor
        /**
            This function inserts new node with key \p key and if inserting is successful then it calls
            \p func functor with signature
            \code void func( value_type& item ) ; endcode
            or
            \code
            struct functor {
                void operator()( value_type& item );
            };
            \endcode

            The argument \p item of user-defined functor \p func is the reference
            to the list's item inserted. <tt>item.second</tt> is a reference to item's value that may be changed.
            The user-defined functor is called only if the inserting is successful.

            The key_type should be constructible from value of type \p K.

            The function allows to split creating of new item into two part:
            - create item from \p key;
            - insert new item into the list;
            - if inserting is successful, initialize the value of item by calling \p f functor

            This can be useful if complete initialization of object of \p mapped_type is heavyweight and
            it is preferable that the initialization should be completed only if inserting is successful.

            Returns an iterator pointed to inserted value, or \p end() if inserting is failed
        */
        template <typename K, typename Func>
        iterator insert_with( const K& key, Func func )
        {
            return node_to_iterator( insert_with_at( head(), key, func ));
        }

        /// Updates the item
        /**
            If \p key is not in the list and \p bAllowInsert is \p true,

            the function inserts a new item.
            Otherwise, the function returns an iterator pointing to the item found.

            Returns <tt> std::pair<iterator, bool>  </tt> where \p first is an iterator pointing to
            item found or inserted, \p second is true if new item has been added or \p false if the item
            already is in the list.
        */
        template <typename K>
        std::pair<iterator, bool> update( const K& key, bool bAllowInsert = true )
        {
            std::pair< node_type *, bool > ret = update_at( head(), key, bAllowInsert );
            return std::make_pair( node_to_iterator( ret.first ), ret.second );
        }
        //@cond
        template <typename K>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<iterator, bool> ensure( const K& key )
        {
            return update( key, true );
        }
        //@endcond

        /// Inserts data of type \ref mapped_type constructed with <tt>std::forward<Args>(args)...</tt>
        /**
            Returns an iterator pointed to inserted value, or \p end() if inserting is failed
        */
        template <typename... Args>
        iterator emplace( Args&&... args )
        {
            return node_to_iterator( emplace_at( head(), std::forward<Args>(args)... ));
        }

        /// Checks whether the list contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns an iterator pointed to item found if the key is found,
            and \ref end() otherwise
        */
        template <typename Q>
        iterator contains( Q const& key )
        {
            return node_to_iterator( find_at( head(), key, intrusive_key_comparator()));
        }
        //@cond
        template <typename Q>
        CDS_DEPRECATED("deprecated, use contains()")
        iterator find( Q const& key )
        {
            return contains( key );
        }
        //@endcond

        /// Checks whether the map contains \p key using \p pred predicate for searching (ordered list version)
        /**
            The function is an analog of <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the list.
        */
        template <typename Q, typename Less, bool Sort = c_bSort>
        typename std::enable_if<Sort, iterator>::type contains( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return node_to_iterator( find_at( head(), key, typename maker::template less_wrapper<Less>()));
        }
        //@cond
        template <typename Q, typename Less, bool Sort = c_bSort>
        CDS_DEPRECATED("deprecated, use contains()")
        typename std::enable_if<Sort, iterator>::type find_with( Q const& key, Less pred )
        {
            return contains( key, pred );
        }
        //@endcond

        /// Finds the key \p val using \p equal predicate for searching (unordered list version)
        /**
            The function is an analog of <tt>contains( key )</tt> but \p equal is used for key comparing.
            \p Equal functor has the interface like \p std::equal_to.
        */
        template <typename Q, typename Equal, bool Sort = c_bSort>
        typename std::enable_if<!Sort, iterator>::type contains( Q const& key, Equal equal )
        {
            CDS_UNUSED( equal );
            return node_to_iterator( find_at( head(), key, typename maker::template equal_to_wrapper<Equal>::type()));
        }
        //@cond
        template <typename Q, typename Equal, bool Sort = c_bSort>
        CDS_DEPRECATED("deprecated, use contains()")
        typename std::enable_if<!Sort, iterator>::type find_with( Q const& key, Equal equal )
        {
            return contains( key, equal );
        }
        //@endcond

        /// Check if the list is empty
        bool empty() const
        {
            return base_class::empty();
        }

        /// Returns list's item count
        /**
            The value returned depends on opt::item_counter option. For atomicity::empty_item_counter,
            this function always returns 0.

            @note Even if you use real item counter and it returns 0, this fact is not mean that the list
            is empty. To check list emptyness use \ref empty() method.
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
        node_type * insert_node_at( head_type& refHead, node_type * pNode )
        {
            assert( pNode != nullptr );
            scoped_node_ptr p( pNode );
            if ( base_class::insert_at( &refHead, *p ))
                return p.release();

            return nullptr;
        }

        template <typename K>
        node_type * insert_at( head_type& refHead, const K& key )
        {
            return insert_node_at( refHead, alloc_node( key ));
        }

        template <typename K, typename V>
        node_type * insert_at( head_type& refHead, const K& key, const V& val )
        {
            return insert_node_at( refHead, alloc_node( key, val ));
        }

        template <typename K, typename Func>
        node_type * insert_with_at( head_type& refHead, const K& key, Func f )
        {
            scoped_node_ptr pNode( alloc_node( key ));

            if ( base_class::insert_at( &refHead, *pNode )) {
                f( pNode->m_Data );
                return pNode.release();
            }

            return nullptr;
        }


        template <typename K>
        std::pair< node_type *, bool > update_at( head_type& refHead, const K& key, bool bAllowInsert )
        {
            scoped_node_ptr pNode( alloc_node( key ));
            node_type * pItemFound = nullptr;

            std::pair<bool, bool> ret = base_class::update_at( &refHead, *pNode,
                [&pItemFound](bool, node_type& item, node_type&){ pItemFound = &item; },
                bAllowInsert );

            if ( ret.second )
                pNode.release();

            return std::make_pair( pItemFound, ret.second );
        }

        template <typename... Args>
        node_type * emplace_at( head_type& refHead, Args&&... args )
        {
            return insert_node_at( refHead, alloc_node( std::forward<Args>(args)... ));
        }

        template <typename K, typename Compare>
        node_type * find_at( head_type& refHead, const K& key, Compare cmp )
        {
            return base_class::find_at( &refHead, key, cmp );
        }

        /*
        template <typename K, typenam Compare, typename Func>
        bool find_at( head_type& refHead, K& key, Compare cmp, Func f )
        {
            return base_class::find_at( &refHead, key, cmp, [&f]( node_type& node, K const& ){ f( node.m_Data ); });
        }
        */
        //@endcond
    };

}} // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_LAZY_KVLIST_NOGC_H
