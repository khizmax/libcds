// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_SKIP_LIST_SET_NOGC_H
#define CDSLIB_CONTAINER_SKIP_LIST_SET_NOGC_H

#include <cds/intrusive/skip_list_nogc.h>
#include <cds/container/details/skip_list_base.h>
#include <cds/details/binary_functor_wrapper.h>
#include <cds/gc/nogc.h>
#include <cds/details/allocator.h>

namespace cds { namespace container {
    //@cond
    namespace skip_list { namespace details {
        struct set_key_accessor
        {
            template <typename NodeType>
            typename NodeType::stored_value_type const& operator()( NodeType const& node ) const
            {
                return node.m_Value;
            }
        };
    }} // namespace skip_list::details

    namespace details {
        template <typename T, typename Traits >
        struct make_skip_list_set_nogc
        {
            typedef cds::gc::nogc   gc;
            typedef T               value_type;
            typedef Traits          traits;

            typedef cds::intrusive::skip_list::node< gc >   intrusive_node_type;
            struct node_type: public intrusive_node_type
            {
                typedef intrusive_node_type             base_class;
                typedef typename base_class::atomic_ptr atomic_ptr;
                typedef atomic_ptr                      tower_item_type;
                typedef value_type                      stored_value_type;

                value_type m_Value;
                //atomic_ptr m_arrTower[] ;  // allocated together with node_type in single memory block

                template <typename Q>
                node_type( unsigned int nHeight, atomic_ptr * pTower, Q const& v )
                    : m_Value(v)
                {
                    if ( nHeight > 1 ) {
                        new (pTower) atomic_ptr[ nHeight - 1 ];
                        base_class::make_tower( nHeight, pTower );
                    }
                }

                template <typename Q, typename... Args>
                node_type( unsigned int nHeight, atomic_ptr * pTower, Q&& q, Args&&... args )
                    : m_Value( std::forward<Q>(q), std::forward<Args>(args)... )
                {
                    if ( nHeight > 1 ) {
                        new (pTower) atomic_ptr[ nHeight - 1 ];
                        base_class::make_tower( nHeight, pTower );
                    }
                }

                node_type() = delete;   // no default ctor
            };

            typedef skip_list::details::node_allocator< node_type, traits> node_allocator;

            struct node_deallocator {
                void operator ()( node_type * pNode )
                {
                    node_allocator().Delete( pNode );
                }
            };

            typedef skip_list::details::dummy_node_builder<intrusive_node_type> dummy_node_builder;

            typedef typename traits::key_accessor key_accessor;
            typedef typename opt::details::make_comparator< value_type, traits >::type key_comparator;

            /*
            template <typename Less>
            using less_wrapper = compare_wrapper< node_type, cds::opt::details::make_comparator_from_less<Less>, key_accessor >;
            */

            typedef typename cds::intrusive::skip_list::make_traits<
                cds::opt::type_traits< traits >
                ,cds::intrusive::opt::hook< intrusive::skip_list::base_hook< cds::opt::gc< gc > > >
                ,cds::intrusive::opt::disposer< node_deallocator >
                ,cds::intrusive::skip_list::internal_node_builder< dummy_node_builder >
                ,cds::opt::compare< cds::details::compare_wrapper< node_type, key_comparator, key_accessor > >
            >::type intrusive_type_traits;

            typedef cds::intrusive::SkipListSet< gc, node_type, intrusive_type_traits>   type;
        };
    } // namespace details
    //@endcond

    /// Lock-free skip-list set (template specialization for gc::nogc)
    /** @ingroup cds_nonintrusive_set
        \anchor cds_nonintrusive_SkipListSet_nogc

        This specialization is intended for so-called persistent usage when no item
        reclamation may be performed. The class does not support deleting of list item.
        See \ref cds_nonintrusive_SkipListSet_hp "SkipListSet" for detailed description.

        Template arguments:
        - \p T - type to be stored in the list.
        - \p Traits - type traits. See skip_list::traits for explanation.

        It is possible to declare option-based list with cds::container::skip_list::make_traits metafunction istead of \p Traits template
        argument. \p Options template arguments of cds::container::skip_list::make_traits metafunction are:
        - opt::compare - key comparison functor. No default functor is provided.
            If the option is not specified, the opt::less is used.
        - opt::less - specifies binary predicate used for key comparison. Default is \p std::less<T>.
        - opt::item_counter - the type of item counting feature. Default is \ref atomicity::empty_item_counter that is no item counting.
        - opt::memory_model - C++ memory ordering model. Can be opt::v::relaxed_ordering (relaxed memory model, the default)
            or opt::v::sequential_consistent (sequentially consisnent memory model).
        - skip_list::random_level_generator - random level generator. Can be \p skip_list::xor_shift, \p skip_list::turbo or
            user-provided one. See skip_list::random_level_generator option description for explanation.
            Default is \p skip_list::turbo32.
        - opt::allocator - allocator for skip-list node. Default is \ref CDS_DEFAULT_ALLOCATOR.
        - opt::back_off - back-off strategy used. If the option is not specified, the cds::backoff::Default is used.
        - opt::stat - internal statistics. Available types: skip_list::stat, skip_list::empty_stat (the default)
    */
    template <
        typename T,
#ifdef CDS_DOXYGEN_INVOKED
        class Traits = skip_list::traits
#else
        class Traits
#endif
    >
    class SkipListSet< gc::nogc, T, Traits >:
#ifdef CDS_DOXYGEN_INVOKED
        protected intrusive::SkipListSet< cds::gc::nogc, T, Traits >
#else
        protected details::make_skip_list_set_nogc< T, typename cds::opt::replace_key_accessor< Traits, skip_list::details::set_key_accessor >::type >::type
#endif
    {
        //@cond
        typedef details::make_skip_list_set_nogc< T, typename cds::opt::replace_key_accessor< Traits, skip_list::details::set_key_accessor >::type >    maker;
        typedef typename maker::type base_class;
        //@endcond
    public:
        typedef typename base_class::gc gc  ; ///< Garbage collector used
        typedef T       value_type  ;   ///< Value type stored in the set
        typedef Traits  options     ;   ///< Options specified

        typedef typename base_class::back_off       back_off        ;   ///< Back-off strategy used
        typedef typename options::allocator         allocator_type  ;   ///< Allocator type used for allocate/deallocate the skip-list nodes
        typedef typename base_class::item_counter   item_counter    ;   ///< Item counting policy used
        typedef typename maker::key_comparator      key_comparator  ;   ///< key compare functor
        typedef typename base_class::memory_model   memory_model    ;   ///< Memory ordering. See cds::opt::memory_model option
        typedef typename options::stat              stat            ;   ///< internal statistics type
        typedef typename base_class::random_level_generator random_level_generator  ;   ///< random level generator

    protected:
        //@cond
        typedef typename maker::node_type           node_type;
        typedef typename maker::node_allocator      node_allocator;
        typedef typename std::conditional<
            std::is_same< typename options::key_accessor, opt::none >::value,
            skip_list::details::set_key_accessor,
            typename options::key_accessor
        >::type     key_accessor;

        typedef std::unique_ptr< node_type, typename maker::node_deallocator >    scoped_node_ptr;
        //@endcond

    public:
    ///@name Forward iterators
    //@{
        /// Forward ordered iterator
        /**
            The forward iterator for a split-list has some features:
            - it has no post-increment operator
            - it depends on iterator of underlying \p OrderedList
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

    protected:
        //@cond
        static iterator node_to_iterator( node_type * pNode )
        {
            assert( pNode );
            return iterator( base_class::iterator::from_node( pNode ));
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

        /// Inserts new node
        /**
            The function inserts \p val in the set if it does not contain
            an item with key equal to \p val.

            Return an iterator pointing to inserted item if success, otherwise \ref end()
        */
        template <typename Q>
        iterator insert( const Q& val )
        {
            scoped_node_ptr sp( node_allocator().New( base_class::random_level(), val ));
            if ( base_class::insert( *sp.get())) {
                return node_to_iterator( sp.release());
            }
            return end();
        }

        /// Inserts data of type \ref value_type constructed with <tt>std::forward<Args>(args)...</tt>
        /**
            Return an iterator pointing to inserted item if success \ref end() otherwise
        */
        template <typename... Args>
        iterator emplace( Args&&... args )
        {
            scoped_node_ptr sp( node_allocator().New( base_class::random_level(), std::forward<Args>(args)... ));
            if ( base_class::insert( *sp.get())) {
                return node_to_iterator( sp.release());
            }
            return end();
        }

        /// Updates the item
        /**
            The operation inserts new item if \p val is not found in the set and \p bInsert is \p true.
            Otherwise, if that key exists, the function returns an iterator that points to item found.

            Returns <tt> std::pair<iterator, bool> </tt> where \p first is an iterator pointing to
            item found or inserted or \p end() if \p val is not found and \p bInsert is \p false,
            \p second is \p true if new item has been added or \p false if the item
            already is in the set.
        */
        template <typename Q>
        std::pair<iterator, bool> update( const Q& val, bool bInsert = true )
        {
            scoped_node_ptr sp( node_allocator().New( base_class::random_level(), val ));
            node_type * pNode;
            std::pair<bool, bool> bRes = base_class::update( *sp, [&pNode](bool, node_type& item, node_type&) { pNode = &item; }, bInsert );
            if ( bRes.first && bRes.second )
                sp.release();
            else if ( !bRes.first )
                return std::make_pair( end(), false );
            assert( pNode );
            return std::make_pair( node_to_iterator( pNode ), bRes.second );
        }
        //@cond
        template <typename Q>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<iterator, bool> ensure( const Q& val )
        {
            return update( val, true );
        }
        //@endcond

        /// Checks whether the set contains \p key
        /**
            The function searches the item with key equal to \p key
            and returns an iterator to item found or \p end() if the key is not fund
        */
        template <typename Q>
        iterator contains( Q const& key ) const
        {
            node_type * pNode = base_class::contains( key );
            if ( pNode )
                return node_to_iterator( pNode );
            return base_class::nonconst_end();
        }
        //@cond
        template <typename Q>
        CDS_DEPRECATED("deprecated, use contains()")
        iterator find( Q const& key ) const
        {
            return contains( key );
        }
        //@edncond

        /// Checks whether the set contains \p key using \p pred predicate for searching
        /**
            The function is similar to <tt>contains( key )</tt> but \p pred is used for key comparing.
            \p Less functor has the interface like \p std::less.
            \p Less must imply the same element order as the comparator used for building the set.
        */
        template <typename Q, typename Less>
        iterator contains( Q const& key, Less pred ) const
        {
            CDS_UNUSED( pred );
            node_type * pNode = base_class::contains( key, cds::details::predicate_wrapper< node_type, Less, key_accessor>());
            if ( pNode )
                return node_to_iterator( pNode );
            return base_class::nonconst_end();
        }
        //@cond
        template <typename Q, typename Less>
        CDS_DEPRECATED("deprecated, use contains()")
        iterator find_with( Q const& key, Less pred ) const
        {
            return contains( key, pred );
        }
        //@endcond

        /// Gets minimum key from the set
        /**
            If the set is empty the function returns \p nullptr
        */
        value_type * get_min() const
        {
            node_type * pNode = base_class::get_min();
            return pNode ? &pNode->m_Value : nullptr;
        }

        /// Gets maximum key from the set
        /**
            The function returns \p nullptr if the set is empty
        */
        value_type * get_max() const
        {
            node_type * pNode = base_class::get_max();
            return pNode ? &pNode->m_Value : nullptr;
        }

        /// Clears the set (non-atomic)
        /**
            The function is not atomic.
            Finding and/or inserting is prohibited while clearing.
            Otherwise an unpredictable result may be encountered.
            Thus, \p clear() may be used only for debugging purposes.
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
            If it is atomicity::empty_item_counter this function always returns 0.
            The function is not suitable for checking the set emptiness, use \ref empty
            member function for this purpose.
        */
        size_t size() const
        {
            return base_class::size();
        }

        /// Returns maximum height of skip-list. The max height is a constant for each object and does not exceed 32.
        static constexpr unsigned int max_height() noexcept
        {
            return base_class::max_height();
        }

        /// Returns const reference to internal statistics
        stat const& statistics() const
        {
            return base_class::statistics();
        }
    };

}} // cds::container

#endif // ifndef CDSLIB_CONTAINER_SKIP_LIST_SET_NOGC_H
