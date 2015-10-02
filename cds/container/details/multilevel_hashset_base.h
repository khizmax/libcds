//$$CDS-header$$

#ifndef CDSLIB_CONTAINER_DETAILS_MULTILEVEL_HASHSET_BASE_H
#define CDSLIB_CONTAINER_DETAILS_MULTILEVEL_HASHSET_BASE_H

#include <cds/intrusive/details/multilevel_hashset_base.h>
#include <cds/container/details/base.h>

namespace cds { namespace container {
    /// \p MultiLevelHashSet related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace multilevel_hashset {
        /// Hash accessor option
        /**
            @copydetails cds::intrusive::multilevel_hashset::traits::hash_accessor
        */
        template <typename Accessor>
        using hash_accessor = cds::intrusive::multilevel_hashset::hash_accessor< Accessor >;

        /// \p MultiLevelHashSet internal statistics, see cds::intrusive::multilevel_hashset::stat
        template <typename EventCounter = cds::atomicity::event_counter>
        using stat = cds::intrusive::multilevel_hashset::stat< EventCounter >;

        /// \p MultiLevelHashSet empty internal statistics
        typedef cds::intrusive::multilevel_hashset::empty_stat empty_stat;

        /// Bit-wise memcmp-based comparator for hash value \p T
        template <typename T>
        using bitwise_compare = cds::intrusive::multilevel_hashset::bitwise_compare< T >;

        /// \p MultiLevelHashSet traits
        struct traits
        {
            /// Mandatory functor to get hash value from data node
            /**
                @copydetails cds::intrusive::multilevel_hashset::traits::hash_accessor
            */
            typedef cds::opt::none hash_accessor;

            /// Hash comparing functor
            /**
                @copydetails cds::intrusive::multilevel_hashset::traits::compare
            */
            typedef cds::opt::none compare;

            /// Specifies binary predicate used for hash compare.
            /**
                @copydetails cds::intrusive::multilevel_hashset::traits::less
            */
            typedef cds::opt::none less;

            /// Item counter
            /**
                @copydetails cds::intrusive::multilevel_hashset::traits::item_counter
            */
            typedef cds::atomicity::item_counter item_counter;

            /// Item allocator
            /**
                Default is \ref CDS_DEFAULT_ALLOCATOR
            */
            typedef CDS_DEFAULT_ALLOCATOR allocator;

            /// Array node allocator
            /**
                @copydetails cds::intrusive::multilevel_hashset::traits::node_allocator
            */
            typedef CDS_DEFAULT_ALLOCATOR node_allocator;

            /// C++ memory ordering model
            /**
                @copydetails cds::intrusive::multilevel_hashset::traits::memory_model
            */
            typedef cds::opt::v::relaxed_ordering memory_model;

            /// Back-off strategy
            typedef cds::backoff::Default back_off;

            /// Internal statistics
            /**
                @copydetails cds::intrusive::multilevel_hashset::traits::stat
            */
            typedef empty_stat stat;

            /// RCU deadlock checking policy (only for \ref cds_container_MultilevelHashSet_rcu "RCU-based MultilevelHashSet")
            /**
                @copydetails cds::intrusive::multilevel_hashset::traits::rcu_check_deadlock
            */
            typedef cds::opt::v::rcu_throw_deadlock rcu_check_deadlock;
        };

        /// Metafunction converting option list to \p multilevel_hashset::traits
        /**
            Supported \p Options are:
            - \p multilevel_hashset::hash_accessor - mandatory option, hash accessor functor.
                @copydetails traits::hash_accessor
            - \p opt::allocator - item allocator
                @copydetails traits::allocator
            - \p opt::node_allocator - array node allocator.
                @copydetails traits::node_allocator
            - \p opt::compare - hash comparison functor. No default functor is provided.
                If the option is not specified, the \p opt::less is used.
            - \p opt::less - specifies binary predicate used for hash comparison.
                @copydetails cds::container::multilevel_hashset::traits::less
            - \p opt::back_off - back-off strategy used. If the option is not specified, the \p cds::backoff::Default is used.
            - \p opt::item_counter - the type of item counting feature.
                @copydetails cds::intrusive::multilevel_hashset::traits::item_counter
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
    } // namespace multilevel_hashset

    //@cond
    // Forward declaration
    template < class GC, typename T, class Traits = cds::container::multilevel_hashset::traits >
    class MultiLevelHashSet;
    //@endcond

    //@cond
    namespace details {

        template <typename GC, typename T, typename Traits>
        struct make_multilevel_hashset
        {
            typedef GC      gc;
            typedef T       value_type;
            typedef Traits  original_traits;

            typedef cds::details::Allocator< value_type, typename original_traits::allocator > cxx_node_allocator;

            struct node_disposer
            {
                void operator()( value_type * p ) const
                {
                    cxx_node_allocator().Delete( p );
                }
            };

            struct intrusive_traits: public original_traits
            {
                typedef node_disposer disposer;
            };

            // Metafunction result
            typedef cds::intrusive::MultiLevelHashSet< GC, T, intrusive_traits > type;
        };
    } // namespace details
    //@endcond

}} // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_DETAILS_MULTILEVEL_HASHSET_BASE_H
