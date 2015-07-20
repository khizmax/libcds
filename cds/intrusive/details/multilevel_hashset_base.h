//$$CDS-header$$

#ifndef CDSLIB_INTRUSIVE_DETAILS_MULTILEVEL_HASHSET_BASE_H
#define CDSLIB_INTRUSIVE_DETAILS_MULTILEVEL_HASHSET_BASE_H

#include <memory.h> // memcmp
#include <type_traits>
#include <cds/intrusive/details/base.h>
#include <cds/opt/compare.h>
#include <cds/algo/atomic.h>
#include <cds/details/marked_ptr.h>
#include <cds/urcu/options.h>

namespace cds { namespace intrusive {

    /// MultiLevelHashSet ordered list related definitions
    /** @ingroup cds_intrusive_helper
    */
    namespace multilevel_hashset {

        /// Hash accessor option
        /**
            @copydetails traits::hash_accessor
        */
        template <typename Accessor>
        struct hash_accessor {
            //@cond
            template <typename Base> struct pack: public Base
            {
                typedef Accessor hash_accessor;
            };
            //@endcond
        };

        /// Head node allocator option
        /**
            @copydetails traits::head_node_allocator
        */
        template <typename Accessor>
        struct head_node_allocator {
            //@cond
            template <typename Base> struct pack: public Base
            {
                typedef Accessor head_node_allocator;
            };
            //@endcond
        };

        /// Array node allocator option
        /**
            @copydetails traits::array_node_allocator
        */
        template <typename Accessor>
        struct array_node_allocator {
            //@cond
            template <typename Base> struct pack: public Base
            {
                typedef Accessor array_node_allocator;
            };
            //@endcond
        };

        /// \p MultiLevelHashSet internal statistics
        template <typename EventCounter = cds::atomicity::event_counter>
        struct stat {
            typedef EventCounter event_counter ; ///< Event counter type
        };

        /// \p MultiLevelHashSet empty internal statistics
        struct empty_stat {
        };

        /// MultiLevelHashSet traits
        struct traits 
        {
            /// Mandatory functor to get hash value from data node
            /**
                It is most-important feature of \p MultiLevelHashSet.
                That functor must return a reference to fixed-sized hash value of data node.
                The return value of that functor specifies the type of hash value.

                Example:
                \code
                typedef uint8_t hash_type[32]; // 256-bit hash type
                struct foo {
                    hash_type  hash; // 256-bit hash value
                    // ... other fields
                };

                struct foo_hash_functor {
                    hash_type const& operator()( foo const& d ) const
                    {
                        return d.hash;
                    }
                };
                \endcode
            */
            typedef opt::none hash_accessor;

            /// Disposer for removing data nodes
            typedef opt::v::empty_disposer disposer;

            /// Hash comparing functor
            /**
                No default functor is provided.
                If the option is not specified, the \p less option is used.
            */
            typedef opt::none compare;

            /// Specifies binary predicate used for hash compare.
            /**
                If the option is not specified, \p memcmp() -like bit-wise hash comparator is used
                because the hash value is treated as fixed-sized bit-string.
            */
            typedef opt::none less;

            /// Item counter
            /**
                The item counting is an important part of \p MultiLevelHashSet algorithm:
                the \p empty() member function depends on correct item counting.
                Therefore, \p atomicity::empty_item_counter is not allowed as a type of the option.

                Default is \p atomicity::item_counter.
            */
            typedef cds::atomicity::item_counter item_counter;

            /// Head node allocator
            /**
                Allocator for head node. That allocator uses only in the set's constructor for allocating
                main head array and in the destructor for destroying the head array.
                Default is \ref CDS_DEFAULT_ALLOCATOR
            */
            typedef CDS_DEFAULT_ALLOCATOR head_node_allocator;

            /// Array node allocator
            /**
                Allocator for array nodes. That allocator is used for creating \p arrayNode when the set grows.
                The size of each array node is fixed.
                Default is \ref CDS_DEFAULT_ALLOCATOR
            */
            typedef CDS_DEFAULT_ALLOCATOR array_node_allocator;

            /// C++ memory ordering model
            /**
                Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
            */
            typedef opt::v::relaxed_ordering memory_model;

            /// Back-off strategy
            typedef cds::backoff::Default back_off;

            /// Internal statistics
            /**
                By default, internal statistics is disabled (\p multilevel_hashset::empty_stat).
                Use \p multilevel_hashset::stat to enable it.
            */
            typedef empty_stat stat;

            /// RCU deadlock checking policy (only for \ref cds_intrusive_MultilevelHashSet_rcu "RCU-based MultilevelHashSet")
            /**
                List of available policy see \p opt::rcu_check_deadlock
            */
            typedef opt::v::rcu_throw_deadlock rcu_check_deadlock;
        };

        /// Metafunction converting option list to \p multilevel_hashset::traits
        /**
            Supported \p Options are:
            - \p multilevel_hashset::hash_accessor - mandatory option, hash accessor functor.
                @copydetails traits::hash_accessor
            - \p multilevel_hashset::head_node_allocator - head node allocator.
                @copydetails traits::head_node_allocator
            - \p multilevel_hashset::array_node_allocator - array node allocator.
                @copydetails traits::array_node_allocator
            - \p opt::compare - hash comparison functor. No default functor is provided.
                If the option is not specified, the \p opt::less is used.
            - \p opt::less - specifies binary predicate used for hash comparison. 
                If the option is not specified, \p memcmp() -like bit-wise hash comparator is used
                because the hash value is treated as fixed-sized bit-string.
            - \p opt::back_off - back-off strategy used. If the option is not specified, the \p cds::backoff::Default is used.
            - \p opt::disposer - the functor used for disposing removed data node. Default is \p opt::v::empty_disposer. Due the nature
                of GC schema the disposer may be called asynchronously.
            - \p opt::item_counter - the type of item counting feature.
                 The item counting is an important part of \p MultiLevelHashSet algorithm:
                 the \p empty() member function depends on correct item counting.
                 Therefore, \p atomicity::empty_item_counter is not allowed as a type of the option.
                 Default is \p atomicity::item_counter.
            - \p opt::memory_model - C++ memory ordering model. Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
            - \p opt::stat - internal statistics. By default, it is disabled (\p multilevel_hashset::empty_stat).
                To enable it use \p multilevel_hashset::stat
            - \p opt::rcu_check_deadlock - a deadlock checking policy for \ref cds_intrusive_MultilevelHashSet_rcu "RCU-based MultilevelHashSet"
                Default is \p opt::v::rcu_throw_deadlock
        */
        template <typename... Options>
        struct make_traits
        {
#   ifdef CDS_DOXYGEN_INVOKED
            typedef implementation_defined type ;   ///< Metafunction result
#   else
            typedef typename cds::opt::make_options<
                typename cds::opt::find_type_traits< traits, Options... >::type
                ,Options...
            >::type   type;
#   endif
        };

        /// Bit-wise memcmp-based comparator for hash value \p T
        template <typename T>
        struct bitwise_compare
        {
            int operator()( T const& lhs, T const& rhs ) const
            {
                return memcmp( &lhs, &rhs, sizeof(T));
            }
        };

    } // namespace multilevel_hashset

    //@cond
    // Forward declaration
    template < class GC, typename T, class Traits = multilevel_hashset::traits >
    class MultiLevelHashSet;
    //@endcond

}} // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_DETAILS_MULTILEVEL_HASHSET_BASE_H
