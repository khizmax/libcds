// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_DETAILS_CUCKOO_BASE_H
#define CDSLIB_CONTAINER_DETAILS_CUCKOO_BASE_H

#include <cds/intrusive/cuckoo_set.h>

namespace cds { namespace container {

    /// CuckooSet and CuckooMap related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace cuckoo {
#ifdef CDS_DOXYGEN_INVOKED
        /// Lock striping concurrent access policy. This is typedef for intrusive::cuckoo::striping template
        class striping
        {};
#else
        using intrusive::cuckoo::striping;
#endif

#ifdef CDS_DOXYGEN_INVOKED
        /// Refinable concurrent access policy. This is typedef for intrusive::cuckoo::refinable template
        class refinable
        {};
#else
        using intrusive::cuckoo::refinable;
#endif

#ifdef CDS_DOXYGEN_INVOKED
        /// Striping internal statistics. This is typedef for intrusive::cuckoo::striping_stat
        class striping_stat
        {};
#else
        using intrusive::cuckoo::striping_stat;
#endif

#ifdef CDS_DOXYGEN_INVOKED
        /// Empty striping internal statistics. This is typedef for intrusive::cuckoo::empty_striping_stat
        class empty_striping_stat
        {};
#else
        using intrusive::cuckoo::empty_striping_stat;
#endif

#ifdef CDS_DOXYGEN_INVOKED
        /// Refinable internal statistics. This is typedef for intrusive::cuckoo::refinable_stat
        class refinable_stat
        {};
#else
        using intrusive::cuckoo::refinable_stat;
#endif

#ifdef CDS_DOXYGEN_INVOKED
        /// Empty refinable internal statistics. This is typedef for intrusive::cuckoo::empty_refinable_stat
        class empty_refinable_stat
        {};
#else
        using intrusive::cuckoo::empty_refinable_stat;
#endif

#ifdef CDS_DOXYGEN_INVOKED
        /// Cuckoo statistics. This is typedef for intrusive::cuckoo::stat
        class stat
        {};
#else
        using intrusive::cuckoo::stat;
#endif

#ifdef CDS_DOXYGEN_INVOKED
        /// Cuckoo empty statistics.This is typedef for intrusive::cuckoo::empty_stat
        class empty_stat
        {};
#else
        using intrusive::cuckoo::empty_stat;
#endif

        /// Option specifying whether to store hash values in the node
        /**
             This option reserves additional space in the hook to store the hash value of the object once it's introduced in the container.
             When this option is used, the unordered container will store the calculated hash value in the hook and rehashing operations won't need
             to recalculate the hash of the value. This option will improve the performance of unordered containers
             when rehashing is frequent or hashing the value is a slow operation

             The \p Enable template parameter toggles the feature:
             - the value \p true enables storing the hash values
             - the value \p false disables storing the hash values
        */
        template <bool Enable>
        struct store_hash
        {
            //@cond
            template <typename Base>
            struct pack: public Base {
                static bool const store_hash = Enable;
            };
            //@endcond
        };

#ifdef CDS_DOXYGEN_INVOKED
        /// Probe set type option
        /**
            @copydetails cds::intrusive::cuckoo::probeset_type
        */
        template <typename Type>
        struct probeset_type
        {};
#else
        using intrusive::cuckoo::probeset_type;
#endif

        using intrusive::cuckoo::list;
        using intrusive::cuckoo::vector;

        /// Type traits for CuckooSet and CuckooMap classes
        struct traits
        {
            /// Hash functors tuple
            /**
                This is mandatory type and has no predefined one.

                At least, two hash functors should be provided. All hash functor
                should be orthogonal (different): for each <tt> i,j: i != j => h[i](x) != h[j](x) </tt>.
                The hash functors are defined as <tt> std::tuple< H1, H2, ... Hn > </tt>:
                \@code cds::opt::hash< std::tuple< h1, h2 > > \@endcode
                The number of hash functors specifies the number \p k - the count of hash tables in cuckoo hashing.

                To specify hash tuple in traits you should use \p cds::opt::hash_tuple:
                \code
                struct my_traits: public cds::container::cuckoo::traits {
                    typedef cds::opt::hash_tuple< hash1, hash2 > hash;
                };
                \endcode
            */
            typedef cds::opt::none      hash;

            /// Concurrent access policy
            /**
                Available opt::mutex_policy types:
                - cuckoo::striping - simple, but the lock array is not resizable
                - cuckoo::refinable - resizable lock array, but more complex access to set data.

                Default is cuckoo::striping.
            */
            typedef cuckoo::striping<>               mutex_policy;

            /// Key equality functor
            /**
                Default is <tt>std::equal_to<T></tt>
            */
            typedef opt::none                       equal_to;

            /// Key comparison functor
            /**
                No default functor is provided. If the option is not specified, the \p less is used.
            */
            typedef opt::none                       compare;

            /// specifies binary predicate used for key comparison.
            /**
                Default is \p std::less<T>.
            */
            typedef opt::none                       less;

            /// Item counter
            /**
                The type for item counting feature.
                Default is cds::atomicity::item_counter

                Only atomic item counter type is allowed.
            */
            typedef cds::intrusive::cuckoo::traits::item_counter   item_counter;

            /// Allocator type
            /**
                The allocator type for allocating bucket tables.
                Default is \p CDS_DEFAULT_ALLOCATOR that is \p std::allocator
            */
            typedef CDS_DEFAULT_ALLOCATOR       allocator;

            /// Node allocator type
            /**
                If this type is not set explicitly, the \ref allocator type is used.
            */
            typedef opt::none                   node_allocator;

            /// Store hash value into items. See cuckoo::store_hash for explanation
            static bool const store_hash = false;

            /// Probe-set type. See \ref probeset_type option for explanation
            typedef cuckoo::list                probeset_type;

            /// Internal statistics
            typedef empty_stat                  stat;
        };

        /// Metafunction converting option list to CuckooSet/CuckooMap traits
        /**
            Template argument list \p Options... are:
            - \p opt::hash - hash functor tuple, mandatory option. At least, two hash functors should be provided. All hash functor
                should be orthogonal (different): for each <tt> i,j: i != j => h[i](x) != h[j](x) </tt>.
                The hash functors are passed as <tt> std::tuple< H1, H2, ... Hn > </tt>. The number of hash functors specifies
                the number \p k - the count of hash tables in cuckoo hashing.
            - \p opt::mutex_policy - concurrent access policy.
                Available policies: \p cuckoo::striping, \p cuckoo::refinable.
                Default is \p %cuckoo::striping.
            - \p opt::equal_to - key equality functor like \p std::equal_to.
                If this functor is defined then the probe-set will be unordered.
                If \p %opt::compare or \p %opt::less option is specified too, then the probe-set will be ordered
                and \p %opt::equal_to will be ignored.
            - \p opt::compare - key comparison functor. No default functor is provided.
                If the option is not specified, the \p %opt::less is used.
                If \p %opt::compare or \p %opt::less option is specified, then the probe-set will be ordered.
            - \p opt::less - specifies binary predicate used for key comparison. Default is \p std::less<T>.
                If \p %opt::compare or \p %opt::less option is specified, then the probe-set will be ordered.
            - \p opt::item_counter - the type of item counting feature. Default is \p opt::v::sequential_item_counter.
            - \p opt::allocator - the allocator type using for allocating bucket tables.
                Default is \ref CDS_DEFAULT_ALLOCATOR
            - \p opt::node_allocator - the allocator type using for allocating set's items. If this option
                is not specified then the type defined in \p %opt::allocator option is used.
            - \p cuckoo::store_hash - this option reserves additional space in the node to store the hash value
                of the object once it's introduced in the container. When this option is used,
                the unordered container will store the calculated hash value in the node and rehashing operations won't need
                to recalculate the hash of the value. This option will improve the performance of unordered containers
                when rehashing is frequent or hashing the value is a slow operation. Default value is \p false.
            - \ref intrusive::cuckoo::probeset_type "cuckoo::probeset_type" - type of probe set, may be \p cuckoo::list or <tt>cuckoo::vector<Capacity></tt>,
                Default is \p cuckoo::list.
            - \p opt::stat - internal statistics. Possibly types: \p cuckoo::stat, \p cuckoo::empty_stat.
                Default is \p %cuckoo::empty_stat
        */
        template <typename... Options>
        struct make_traits {
            typedef typename cds::opt::make_options<
                typename cds::opt::find_type_traits< cuckoo::traits, Options... >::type
                ,Options...
            >::type   type ;    ///< Result of metafunction
        };
    }   // namespace cuckoo
}} // namespace cds::container

#endif  // #ifndef CDSLIB_CONTAINER_DETAILS_CUCKOO_BASE_H
