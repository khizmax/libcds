// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_DETAILS_MICHAEL_SET_BASE_H
#define CDSLIB_INTRUSIVE_DETAILS_MICHAEL_SET_BASE_H

#include <cds/intrusive/details/base.h>
#include <cds/opt/compare.h>
#include <cds/opt/hash.h>
#include <cds/algo/bitop.h>
#include <cds/algo/atomic.h>

namespace cds { namespace intrusive {

    /// MichaelHashSet related definitions
    /** @ingroup cds_intrusive_helper
    */
    namespace michael_set {
        /// MichaelHashSet traits
        struct traits {
            /// Hash function
            /**
                Hash function converts the key fields of struct \p T stored in the hash-set
                into value of type \p size_t called hash value that is an index of hash table.

                This is mandatory type and has no predefined one.
            */
            typedef opt::none hash;

            /// Item counter
            /**
                The item counting is an important part of \p MichaelHashSet algorithm:
                the \p empty() member function depends on correct item counting.
                You may use \p atomicity::empty_item_counter if don't need \p empty() and \p size()
                member functions.

                Default is \p atomicity::item_counter; to avoid false sharing you may use \p atomicity::cache_friendly_item_counter
            */
            typedef cds::atomicity::item_counter item_counter;

            /// Bucket table allocator
            /**
                Allocator for bucket table. Default is \ref CDS_DEFAULT_ALLOCATOR
                The allocator uses only in constructor for allocating bucket table
                and in destructor for destroying bucket table
            */
            typedef CDS_DEFAULT_ALLOCATOR   allocator;
        };

        /// Metafunction converting option list to traits struct
        /**
            Available \p Options:
            - \p opt::hash - mandatory option, specifies hash functor.
            - \p opt::item_counter - optional, specifies item counting policy. See \p traits::item_counter
                for default type.
            - \p opt::allocator - optional, bucket table allocator. Default is \ref CDS_DEFAULT_ALLOCATOR.
        */
        template <typename... Options>
        struct make_traits {
            typedef typename cds::opt::make_options< traits, Options...>::type type;   ///< Metafunction result
        };

        //@cond
        namespace details {
            static inline size_t init_hash_bitmask( size_t nMaxItemCount, size_t nLoadFactor )
            {
                if ( nLoadFactor == 0 )
                    nLoadFactor = 1;
                if ( nMaxItemCount == 0 )
                    nMaxItemCount = 4;
                const size_t nBucketCount = nMaxItemCount / nLoadFactor;
                const size_t exp2 = size_t( 1 ) << cds::bitop::MSB( nBucketCount );

                return ( exp2 < nBucketCount ? exp2 * 2 : exp2 ) - 1;
            }

            template <typename OrderedList, bool IsConst>
            struct list_iterator_selector;

            template <typename OrderedList>
            struct list_iterator_selector< OrderedList, false>
            {
                typedef OrderedList * bucket_ptr;
                typedef typename OrderedList::iterator  type;
            };

            template <typename OrderedList>
            struct list_iterator_selector< OrderedList, true>
            {
                typedef OrderedList const * bucket_ptr;
                typedef typename OrderedList::const_iterator  type;
            };

            template <typename OrderedList, bool IsConst>
            class iterator
            {
                friend class iterator< OrderedList, !IsConst >;

            protected:
                typedef OrderedList bucket_type;
                typedef typename list_iterator_selector< bucket_type, IsConst>::bucket_ptr bucket_ptr;
                typedef typename list_iterator_selector< bucket_type, IsConst>::type list_iterator;

                bucket_ptr      m_pCurBucket;
                list_iterator   m_itList;
                bucket_ptr      m_pEndBucket;

                void next()
                {
                    if ( m_pCurBucket < m_pEndBucket ) {
                        if ( ++m_itList != m_pCurBucket->end())
                            return;
                        while ( ++m_pCurBucket < m_pEndBucket ) {
                            m_itList = m_pCurBucket->begin();
                            if ( m_itList != m_pCurBucket->end())
                                return;
                        }
                    }
                    m_pCurBucket = m_pEndBucket - 1;
                    m_itList = m_pCurBucket->end();
                }

            public:
                typedef typename list_iterator::value_ptr   value_ptr;
                typedef typename list_iterator::value_ref   value_ref;

            public:
                iterator()
                    : m_pCurBucket( nullptr )
                    , m_itList()
                    , m_pEndBucket( nullptr )
                {}

                iterator( list_iterator const& it, bucket_ptr pFirst, bucket_ptr pLast )
                    : m_pCurBucket( pFirst )
                    , m_itList( it )
                    , m_pEndBucket( pLast )
                {
                    if ( it == pFirst->end())
                        next();
                }

                iterator( iterator const& src )
                    : m_pCurBucket( src.m_pCurBucket )
                    , m_itList( src.m_itList )
                    , m_pEndBucket( src.m_pEndBucket )
                {}

                value_ptr operator ->() const
                {
                    assert( m_pCurBucket != nullptr );
                    return m_itList.operator ->();
                }

                value_ref operator *() const
                {
                    assert( m_pCurBucket != nullptr );
                    return m_itList.operator *();
                }

                /// Pre-increment
                iterator& operator ++()
                {
                    next();
                    return *this;
                }

                iterator& operator = (const iterator& src)
                {
                    m_pCurBucket = src.m_pCurBucket;
                    m_pEndBucket = src.m_pEndBucket;
                    m_itList = src.m_itList;
                    return *this;
                }

                bucket_ptr bucket() const
                {
                    return m_pCurBucket != m_pEndBucket ? m_pCurBucket : nullptr;
                }

                list_iterator const& underlying_iterator() const
                {
                    return m_itList;
                }

                template <bool C>
                bool operator ==(iterator<bucket_type, C> const& i) const
                {
                    return m_pCurBucket == i.m_pCurBucket && m_itList == i.m_itList;
                }
                template <bool C>
                bool operator !=(iterator<OrderedList, C> const& i ) const
                {
                    return !( *this == i );
                }
            };
        }
        //@endcond
    } // namespace michael_set

    //@cond
    // Forward declarations
    template <class GC, class OrderedList, class Traits = michael_set::traits>
    class MichaelHashSet;
    //@endcond

}} // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_DETAILS_MICHAEL_SET_BASE_H
