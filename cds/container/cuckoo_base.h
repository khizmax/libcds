//$$CDS-header$$

#ifndef __CDS_CONTAINER_CUCKOO_BASE_H
#define __CDS_CONTAINER_CUCKOO_BASE_H

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
            The option specifies probe set type for the CuckooSet and CuckooMap.
            Available \p Type:
            - \p cuckoo::list - the probe-set is a single-linked list.
            - \p cuckoo::vector<Capacity> - the probe-set is a vector
                with constant-size \p Capacity where \p Capacity is an <tt>unsigned int</tt> constant.
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
        struct type_traits
        {
            /// Hash functors tuple
            /**
                This is mandatory type and has no predefined one.

                At least, two hash functors should be provided. All hash functor
                should be orthogonal (different): for each <tt> i,j: i != j => h[i](x) != h[j](x) </tt>.
                The hash functors are defined as <tt> std::tuple< H1, H2, ... Hn > </tt>:
                \@code cds::opt::hash< std::tuple< h1, h2 > > \@endcode
                The number of hash functors specifies the number \p k - the count of hash tables in cuckoo hashing.
                Up to 10 different hash functors are supported.
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
            typedef cds::intrusive::cuckoo::type_traits::item_counter   item_counter;

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
            This is a wrapper for <tt> cds::opt::make_options< type_traits, Options...> </tt>
            \p Options list see CuckooSet and CuckooMap
        */
        template <CDS_DECL_OPTIONS12>
        struct make_traits {
            typedef typename cds::opt::make_options<
                typename cds::opt::find_type_traits< cuckoo::type_traits, CDS_OPTIONS12 >::type
                ,CDS_OPTIONS12
            >::type   type ;    ///< Result of metafunction
        };
    }   // namespace cuckoo
}} // namespace cds::container

#endif  // #ifndef __CDS_CONTAINER_CUCKOO_BASE_H
