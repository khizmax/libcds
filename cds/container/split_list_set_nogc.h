//$$CDS-header$$

#ifndef __CDS_CONTAINER_SPLIT_LIST_SET_NOGC_H
#define __CDS_CONTAINER_SPLIT_LIST_SET_NOGC_H

#include <cds/intrusive/split_list_nogc.h>
#include <cds/container/split_list_base.h>
#include <cds/gc/nogc.h>
#include <cds/container/details/make_split_list_set.h>

namespace cds { namespace container {

    /// Split-ordered list set (template specialization for gc::nogc)
    /** @ingroup cds_nonintrusive_set
        \anchor cds_nonintrusive_SplitListSet_nogc

        This specialization is intended for so-called persistent usage when no item
        reclamation may be performed. The class does not support deleting of list item.

        See \ref cds_nonintrusive_SplitListSet_hp "SplitListSet" for description of template parameters.

        The interface of the specialization is a slightly different.
    */
    template <
        class T,
#ifdef CDS_DOXYGEN_INVOKED
        class Traits = split_list::type_traits
#else
        class Traits
#endif
    >
    class SplitListSet< cds::gc::nogc, T, Traits>
#ifdef CDS_DOXYGEN_INVOKED
        :protected intrusive::SplitListSet<cds::gc::nogc, typename Traits::ordered_list, Traits>
#else
        :protected details::make_split_list_set< cds::gc::nogc, T, typename Traits::ordered_list, split_list::details::wrap_set_traits<T, Traits> >::type
#endif
    {
    protected:
        //@cond
        typedef details::make_split_list_set< cds::gc::nogc, T, typename Traits::ordered_list, split_list::details::wrap_set_traits<T, Traits> > options;
        typedef typename options::type  base_class;
        //@endcond

    public:
        typedef typename options::gc                gc              ;   ///< Garbage collector
        typedef typename options::value_type        value_type      ;   ///< type of value stored in the list
        typedef typename options::ordered_list      ordered_list    ;   ///< Underlying ordered list class
        typedef typename base_class::key_comparator key_comparator  ;   ///< key comparison functor

        /// Hash functor for \ref value_type and all its derivatives that you use
        typedef typename base_class::hash           hash;
        typedef typename base_class::item_counter   item_counter    ;   ///< Item counter type

    protected:
        //@cond
        typedef typename options::cxx_node_allocator    cxx_node_allocator;
        typedef typename options::node_type             node_type;

        template <typename Q>
        static node_type * alloc_node(Q const& v )
        {
            return cxx_node_allocator().New( v );
        }

#   ifdef CDS_EMPLACE_SUPPORT
        template <typename... Args>
        static node_type * alloc_node( Args&&... args )
        {
            return cxx_node_allocator().MoveNew( std::forward<Args>(args)...);
        }
#   endif

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

        //@endcond

    protected:
        //@cond
#   ifndef CDS_CXX11_LAMBDA_SUPPORT
        struct empty_ensure_functor
        {
            void operator()( bool /*bNew*/, node_type& /*item*/, node_type& /*val*/ )
            {}
        };
#   endif
        //@endcond


    public:
        /// Initialize split-ordered list of default capacity
        /**
            The default capacity is defined in bucket table constructor.
            See intrusive::split_list::expandable_bucket_table, intrusive::split_list::static_ducket_table
            which selects by intrusive::split_list::dynamic_bucket_table option.
        */
        SplitListSet()
            : base_class()
        {}

        /// Initialize split-ordered list
        SplitListSet(
            size_t nItemCount           ///< estimate average of item count
            , size_t nLoadFactor = 1    ///< load factor - average item count per bucket. Small integer up to 10, default is 1.
            )
            : base_class( nItemCount, nLoadFactor )
        {}

    protected:
        /// Forward iterator
        /**
            \p IsConst - constness boolean flag

            The forward iterator has the following features:
            - it has no post-increment operator
            - it depends on underlying ordered list iterator
        */
        template <bool IsConst>
        class iterator_type: protected base_class::template iterator_type<IsConst>
        {
            //@cond
            typedef typename base_class::template iterator_type<IsConst> iterator_base_class;
            friend class SplitListSet;
            //@endcond
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
            //@cond
            explicit iterator_type( iterator_base_class const& src )
                : iterator_base_class( src )
            {}
            //@endcond

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

    public:
        /// Forward iterator
        typedef iterator_type<false>  iterator;

        /// Const forward iterator
        typedef iterator_type<true>    const_iterator;

        /// Returns a forward iterator addressing the first element in a set
        /**
            For empty set \code begin() == end() \endcode
        */
        iterator begin()
        {
            return iterator( base_class::begin() );
        }

        /// Returns an iterator that addresses the location succeeding the last element in a set
        /**
            Do not use the value returned by <tt>end</tt> function to access any item.
            The returned value can be used only to control reaching the end of the set.
            For empty set \code begin() == end() \endcode
        */
        iterator end()
        {
            return iterator( base_class::end() );
        }

        /// Returns a forward const iterator addressing the first element in a set
        const_iterator begin() const
        {
            return const_iterator( base_class::begin() );
        }

        /// Returns an const iterator that addresses the location succeeding the last element in a set
        const_iterator end() const
        {
            return const_iterator( base_class::end() );
        }

    protected:
        //@cond
        iterator insert_node( node_type * pNode )
        {
            assert( pNode != nullptr );
            scoped_node_ptr p(pNode);

            iterator it( base_class::insert_( *pNode ));
            if ( it != end() ) {
                p.release();
                return it;
            }

            return end();
        }
        //@endcond

    public:
        /// Inserts new node
        /**
            The function inserts \p val in the set if it does not contain
            an item with key equal to \p val.
            The \ref value_type should be constructible from a value of type \p Q.

            Return an iterator pointing to inserted item if success \ref end() otherwise
        */
        template <typename Q>
        iterator insert( const Q& val )
        {
            return insert_node( alloc_node( val ) );
        }

#ifdef CDS_EMPLACE_SUPPORT
        /// Inserts data of type \ref value_type constructed with <tt>std::forward<Args>(args)...</tt>
        /**
            Return an iterator pointing to inserted item if success \ref end() otherwise

            This function is available only for compiler that supports
            variadic template and move semantics
        */
        template <typename... Args>
        iterator emplace( Args&&... args )
        {
            return insert_node( alloc_node( std::forward<Args>(args)... ) );
        }
#endif

        /// Ensures that the item \p val exists in the set
        /**
            The operation inserts new item created from \p val if the key \p val is not found in the set.
            Otherwise, the function returns an iterator that points to item found.
            The \p value_type should be constructible from a value of type \p Q.

            Returns <tt> std::pair<iterator, bool>  </tt> where \p first is an iterator pointing to
            item found or inserted, \p second is true if new item has been added or \p false if the item
            already is in the set.
        */
        template <typename Q>
        std::pair<iterator, bool> ensure( const Q& val )
        {
            scoped_node_ptr pNode( alloc_node( val ));

#       ifdef CDS_CXX11_LAMBDA_SUPPORT
            std::pair<typename base_class::iterator, bool> ret = base_class::ensure_( *pNode, [](bool /*bNew*/, node_type& /*item*/, node_type& /*val*/){} );
#       else
            std::pair<typename base_class::iterator, bool> ret = base_class::ensure_( *pNode, empty_ensure_functor() );
#       endif
            if ( ret.first != base_class::end() && ret.second ) {
                pNode.release();
                return std::make_pair( iterator(ret.first), ret.second );
            }

            return std::make_pair( iterator(ret.first), ret.second );
        }

        /// Find the key \p val
        /** \anchor cds_nonintrusive_SplitListSet_nogc_find

            The function searches the item with key equal to \p key
            and returns an iterator pointed to item found if the key is found,
            and \ref end() otherwise
        */
        template <typename Q>
        iterator find( Q const& key )
        {
            return iterator( base_class::find_( key ));
        }

        /// Finds the key \p val using \p pred predicate for searching
        /**
            The function is an analog of \ref cds_nonintrusive_SplitListSet_nogc_find "find(Q const&)"
            but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        iterator find_with( Q const& key, Less pred )
        {
            return iterator( base_class::find_with_( key, typename options::template predicate_wrapper<Less>::type() ));
        }

        /// Checks if the set is empty
        /**
            Emptiness is checked by item counting: if item count is zero then the set is empty.
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
    };

}}  // namespace cds::container

#endif // #ifndef __CDS_CONTAINER_SPLIT_LIST_SET_NOGC_H
