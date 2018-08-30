// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_LAZY_LIST_NOGC_H
#define CDSLIB_CONTAINER_LAZY_LIST_NOGC_H

#include <memory>
#include <cds/container/details/lazy_list_base.h>
#include <cds/intrusive/lazy_list_nogc.h>
#include <cds/container/details/make_lazy_list.h>

namespace cds { namespace container {

    /// Lazy ordered single-linked list (template specialization for gc::nogc)
    /** @ingroup cds_nonintrusive_list
        \anchor cds_nonintrusive_LazyList_nogc

        This specialization is so-called append-only when no item
        reclamation may be performed. The class does not support deleting of list item.

        The list can be ordered if \p Traits::sort is \p true that is default
        or unordered otherwise. Unordered list can be maintained by \p equal_to
        relationship (\p Traits::equal_to), but for the ordered list \p less
        or \p compare relations should be specified in \p Traits.

        See @ref cds_nonintrusive_LazyList_gc "cds::container::LazyList<cds::gc::nogc, T, Traits>"
    */
    template <
        typename T,
#ifdef CDS_DOXYGEN_INVOKED
        typename Traits = lazy_list::traits
#else
        typename Traits
#endif
    >
    class LazyList<cds::gc::nogc, T, Traits>:
#ifdef CDS_DOXYGEN_INVOKED
        protected intrusive::LazyList< gc::nogc, T, Traits >
#else
        protected details::make_lazy_list< cds::gc::nogc, T, Traits >::type
#endif
    {
        //@cond
        typedef details::make_lazy_list< cds::gc::nogc, T, Traits > maker;
        typedef typename maker::type  base_class;
        //@endcond

    public:
        typedef cds::gc::nogc gc;  ///< Garbage collector
        typedef T      value_type; ///< Type of value stored in the list
        typedef Traits traits;     ///< List traits

        typedef typename base_class::back_off     back_off;         ///< Back-off strategy used
        typedef typename maker::allocator_type    allocator_type;   ///< Allocator type used for allocate/deallocate the nodes
        typedef typename base_class::item_counter item_counter;     ///< Item counting policy used
        typedef typename maker::key_comparator    key_comparator;   ///< key comparing functor
        typedef typename base_class::memory_model memory_model;     ///< Memory ordering. See cds::opt::memory_model option
        typedef typename base_class::stat         stat;             ///< Internal statistics

        static constexpr bool const c_bSort = base_class::c_bSort; ///< List type: ordered (\p true) or unordered (\p false)

        //@cond
        // Rebind traits (split-list support)
        template <typename... Options>
        struct rebind_traits {
            typedef LazyList<
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
        typedef typename base_class::value_type     node_type;
        typedef typename maker::cxx_allocator       cxx_allocator;
        typedef typename maker::node_deallocator    node_deallocator;
        typedef typename base_class::key_comparator intrusive_key_comparator;

        typedef typename base_class::node_type      head_type;

        struct node_disposer {
            void operator()( node_type * pNode )
            {
                free_node( pNode );
            }
        };
        typedef std::unique_ptr< node_type, node_disposer >     scoped_node_ptr;
        //@endcond

    protected:
        //@cond
        template <bool IsConst>
        class iterator_type: protected base_class::template iterator_type<IsConst>
        {
            typedef typename base_class::template iterator_type<IsConst>    iterator_base;

            iterator_type( head_type const& pNode )
                : iterator_base( const_cast<head_type *>(&pNode))
            {}

            explicit iterator_type( const iterator_base& it )
                : iterator_base( it )
            {}

            friend class LazyList;

        protected:
            explicit iterator_type( node_type& pNode )
                : iterator_base( &pNode )
            {}

        public:
            typedef typename cds::details::make_const_type<value_type, IsConst>::pointer   value_ptr;
            typedef typename cds::details::make_const_type<value_type, IsConst>::reference value_ref;

            iterator_type()
            {}

            iterator_type( const iterator_type& src )
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
        /// Returns a forward iterator addressing the first element in a list
        /**
            For empty list \code begin() == end() \endcode
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
            ++it    ;   // skip dummy head node
            return it;
        }

        /// Returns an iterator that addresses the location succeeding the last element in a list
        /**
            Do not use the value returned by <tt>end</tt> function to access any item.

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
            ++it    ;   // skip dummy head node
            return it;
        }

        /// Returns a forward const iterator addressing the first element in a list
        const_iterator cbegin() const
        {
            const_iterator it( head());
            ++it    ;   // skip dummy head node
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

    public:
        /// Default constructor
        LazyList()
        {}

        //@cond
        template <typename Stat, typename = std::enable_if<std::is_same<stat, lazy_list::wrapped_stat<Stat>>::value >>
        explicit LazyList( Stat& st )
            : base_class( st )
        {}
        //@endcond

        /// Desctructor clears the list
        ~LazyList()
        {
            clear();
        }

        /// Inserts new node
        /**
            The function inserts \p val in the list if the list does not contain
            an item with key equal to \p val.

            Return an iterator pointing to inserted item if success \ref end() otherwise
        */
        template <typename Q>
        iterator insert( Q&& val )
        {
            return node_to_iterator( insert_at( head(), std::forward<Q>( val )));
        }

        /// Inserts data of type \p value_type created from \p args
        /**
            Return an iterator pointing to inserted item if success \ref end() otherwise
        */
        template <typename... Args>
        iterator emplace( Args&&... args )
        {
            return node_to_iterator( emplace_at( head(), std::forward<Args>(args)... ));
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
        template <typename Q>
        std::pair<iterator, bool> update( Q&& val, bool bAllowInsert = true )
        {
            std::pair< node_type *, bool > ret = update_at( head(), std::forward<Q>( val ), bAllowInsert );
            return std::make_pair( node_to_iterator( ret.first ), ret.second );
        }
        //@cond
        template <typename Q>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<iterator, bool> ensure( Q const& val )
        {
            return update( val, true );
        }
        //@endcond

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
            The value returned depends on \p Traits::item_counter type. For \p atomicity::empty_item_counter,
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
        void clear()
        {
            base_class::clear();
        }

    protected:
        //@cond
        static value_type& node_to_value( node_type& n )
        {
            return n.m_Value;
        }

        static node_type * alloc_node()
        {
            return cxx_allocator().New();
        }

        static node_type * alloc_node( value_type const& v )
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

        iterator node_to_iterator( node_type * pNode )
        {
            if ( pNode )
                return iterator( *pNode );
            return end();
        }

        iterator insert_node( node_type * pNode )
        {
            return node_to_iterator( insert_node_at( head(), pNode ));
        }

        node_type * insert_node_at( head_type& refHead, node_type * pNode )
        {
            assert( pNode != nullptr );
            scoped_node_ptr p( pNode );
            if ( base_class::insert_at( &refHead, *p ))
                return p.release();

            return nullptr;
        }

        template <typename Q>
        node_type * insert_at( head_type& refHead, Q&& val )
        {
            return insert_node_at( refHead, alloc_node( std::forward<Q>( val )));
        }

        template <typename... Args>
        node_type * emplace_at( head_type& refHead, Args&&... args )
        {
            return insert_node_at( refHead, alloc_node( std::forward<Args>(args)... ));
        }

        template <typename Q>
        std::pair< node_type *, bool > update_at( head_type& refHead, Q&& val, bool bAllowInsert )
        {
            scoped_node_ptr pNode( alloc_node( std::forward<Q>( val )));
            node_type * pItemFound = nullptr;

            std::pair<bool, bool> ret = base_class::update_at( &refHead, *pNode,
                [&pItemFound](bool, node_type& item, node_type&) { pItemFound = &item; },
                bAllowInsert );

            if ( ret.second )
                pNode.release();

            return std::make_pair( pItemFound, ret.second );
        }

        template <typename Q, typename Compare>
        node_type * find_at( head_type& refHead, Q const& key, Compare cmp )
        {
            return base_class::find_at( &refHead, key, cmp );
        }

        //@endcond
    };
}} // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_LAZY_LIST_NOGC_H
