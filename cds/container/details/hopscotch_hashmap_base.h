/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CDSLIB_CONTAINER_DETAILS_HOPSCOTCH_HASHMAP_BASE_H
#define CDSLIB_CONTAINER_DETAILS_HOPSCOTCH_HASHMAP_BASE_H

#include <cds/intrusive/hopscotch_hashset.h>

namespace cds { namespace container {

    /// Hopscotch hashmap related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace hopscotch_hashmap {
#ifdef CDS_DOXYGEN_INVOKED
        /// Lock striping concurrent access policy. This is typedef for intrusive::hopscotch_hashset::striping template
        class striping
        {};
#else
        using intrusive::hopscotch_hashset::striping;
#endif

#ifdef CDS_DOXYGEN_INVOKED
        /// Refinable concurrent access policy. This is typedef for intrusive::hopscotch_hashset::refinable template
        class refinable
        {};
#else
        using intrusive::hopscotch_hashset::refinable;
#endif

#ifdef CDS_DOXYGEN_INVOKED
        /// Striping internal statistics. This is typedef for intrusive::hopscotch_hashset::striping_stat
        class striping_stat
        {};
#else
        using intrusive::hopscotch_hashset::striping_stat;
#endif

#ifdef CDS_DOXYGEN_INVOKED
        /// Empty striping internal statistics. This is typedef for intrusive::hopscotch_hashset::empty_striping_stat
        class empty_striping_stat
        {};
#else
        using intrusive::hopscotch_hashset::empty_striping_stat;
#endif

#ifdef CDS_DOXYGEN_INVOKED
        /// Refinable internal statistics. This is typedef for intrusive::hopscotch_hashset::refinable_stat
        class refinable_stat
        {};
#else
        using intrusive::hopscotch_hashset::refinable_stat;
#endif

#ifdef CDS_DOXYGEN_INVOKED
        /// Empty refinable internal statistics. This is typedef for intrusive::hopscotch_hashset::empty_refinable_stat
        class empty_refinable_stat
        {};
#else
        using intrusive::hopscotch_hashset::empty_refinable_stat;
#endif

#ifdef CDS_DOXYGEN_INVOKED
        /// Hopscotch hashmap statistics. This is typedef for intrusive::hopscotch_hashset::stat
        class stat
        {};
#else
        using intrusive::hopscotch_hashset::stat;
#endif

#ifdef CDS_DOXYGEN_INVOKED
        /// Hopscotch hashmap empty statistics.This is typedef for intrusive::hopscotch_hashset::empty_stat
        class empty_stat
        {};
#else
        using intrusive::hopscotch_hashset::empty_stat;
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
            @copydetails cds::intrusive::hopscotch_hashset::probeset_type
        */
        template <typename Type>
        struct probeset_type
        {};
#else
        using intrusive::hopscotch_hashset::probeset_type;
#endif

        using intrusive::hopscotch_hashset::list;
        using intrusive::hopscotch_hashset::vector;

        /// Type traits for hopscotch hashmap class
        struct traits
        {
            /// Hash functors tuple
            /**
                This is mandatory type and has no predefined one.

                At least, two hash functors should be provided. All hash functor
                should be orthogonal (different): for each <tt> i,j: i != j => h[i](x) != h[j](x) </tt>.
                The hash functors are defined as <tt> std::tuple< H1, H2, ... Hn > </tt>:
                \@code cds::opt::hash< std::tuple< h1, h2 > > \@endcode
                The number of hash functors specifies the number \p k - the count of hash tables in hopscotch hashing.

                To specify hash tuple in traits you should use \p cds::opt::hash_tuple:
                \code
                struct my_traits: public cds::container::hopscotch_hashmap::traits {
                    typedef cds::opt::hash_tuple< hash1, hash2 > hash;
                };
                \endcode
            */
            typedef cds::opt::none      hash;

            /// Concurrent access policy
            /**
                Available opt::mutex_policy types:
                - hopscotch_hashmap::striping - simple, but the lock array is not resizable
                - hopscotch_hashmap::refinable - resizable lock array, but more complex access to set data.

                Default is hopscotch_hashmap::striping.
            */
            typedef hopscotch_hashmap::striping<>               mutex_policy;

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
            typedef cds::intrusive::hopscotch_hashset::traits::item_counter   item_counter;

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

            /// Store hash value into items. See hopscotch_hashmap::store_hash for explanation
            static bool const store_hash = false;

            /// Probe-set type. See \ref probeset_type option for explanation
            typedef hopscotch_hashmap::list                probeset_type;

            /// Internal statistics
            typedef empty_stat                  stat;
        };

        /// Metafunction converting option list to hopscotch hashmap traits
        /**
            Template argument list \p Options... are:
            - \p opt::hash - hash functor tuple, mandatory option. At least, two hash functors should be provided. All hash functor
                should be orthogonal (different): for each <tt> i,j: i != j => h[i](x) != h[j](x) </tt>.
                The hash functors are passed as <tt> std::tuple< H1, H2, ... Hn > </tt>. The number of hash functors specifies
                the number \p k - the count of hash tables in hopscotch hashing.
            - \p opt::mutex_policy - concurrent access policy.
                Available policies: \p hopscotch_hashmap::striping, \p hopscotch_hashmap::refinable.
                Default is \p %hopscotch_hashmap::striping.
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
            - \p hopscotch_hashmap::store_hash - this option reserves additional space in the node to store the hash value
                of the object once it's introduced in the container. When this option is used,
                the unordered container will store the calculated hash value in the node and rehashing operations won't need
                to recalculate the hash of the value. This option will improve the performance of unordered containers
                when rehashing is frequent or hashing the value is a slow operation. Default value is \p false.
            - \ref intrusive::hopscotch_hashset::probeset_type "hopscotch_hashset::probeset_type" - type of probe set, may be \p hopscotch_hashmap::list or <tt>hopscotch_hashmap::vector<Capacity></tt>,
                Default is \p hopscotch_hashmap::list.
            - \p opt::stat - internal statistics. Possibly types: \p hopscotch_hashmap::stat, \p hopscotch_hashmap::empty_stat.
                Default is \p %hopscotch_hashmap::empty_stat
        */
        template <typename... Options>
        struct make_traits {
            typedef typename cds::opt::make_options<
                typename cds::opt::find_type_traits< hopscotch_hashmap::traits, Options... >::type
                ,Options...
            >::type   type ;    ///< Result of metafunction
        };
    }   // namespace hopscotch_hashmap
}} // namespace cds::container

#endif  // #ifndef CDSLIB_CONTAINER_DETAILS_HOPSCOTCH_HASHMAP_BASE_H
