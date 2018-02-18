// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_DETAILS_SPLIT_LIST_BASE_H
#define CDSLIB_CONTAINER_DETAILS_SPLIT_LIST_BASE_H

#include <cds/intrusive/details/split_list_base.h>

namespace cds { namespace container {

    // forward declaration
    struct michael_list_tag;

    /// SplitListSet related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace split_list {
        /// Internal statistics, see \p cds::intrusive::split_list::stat
        template <typename Counter = cds::intrusive::split_list::stat<>::counter_type >
        using stat = cds::intrusive::split_list::stat<Counter>;

        /// Disabled internal statistics, see \p cds::intrusive::split_list::empty_stat
        typedef cds::intrusive::split_list::empty_stat empty_stat;

        /// Selector of bucket table implementation = typedef for \p intrusive::split_list::dynamic_bucket_table
        template <bool Value>
        using dynamic_bucket_table = cds::intrusive::split_list::dynamic_bucket_table<Value>;

        /// @copydoc cds::intrusive::split_list::bit_reversal
        template <typename Type>
        using bit_reversal = cds::intrusive::split_list::bit_reversal<Type>;

        using cds::intrusive::split_list::static_bucket_table;
        using cds::intrusive::split_list::expandable_bucket_table;

        //@cond
        namespace details {

            template <typename Key, typename Value, typename Traits, typename Opt>
            struct wrap_map_traits_helper {
                typedef Opt key_accessor;
            };

            template <typename Key, typename Value, typename Traits >
            struct wrap_map_traits_helper<Key, Value, Traits, opt::none>
            {
                struct key_accessor
                {
                    typedef Key     key_type;
                    key_type const & operator()( std::pair<Key const, Value> const & val ) const
                    {
                        return val.first;
                    }
                };
            };

            template <typename Key, typename Value, typename Traits>
            struct wrap_map_traits: public Traits
            {
                typedef typename wrap_map_traits_helper<Key, Value, Traits, typename Traits::key_accessor>::key_accessor    key_accessor;
            };

            template <typename Value, typename Traits, typename Opt>
            struct wrap_set_traits_helper {
                typedef Opt key_accessor;
            };

            template <typename Value, typename Traits >
            struct wrap_set_traits_helper<Value, Traits, opt::none>
            {
                struct key_accessor
                {
                    typedef Value     key_type;
                    key_type const& operator()( Value const& val ) const
                    {
                        return val;
                    }
                };
            };

            template <typename Value, typename Traits>
            struct wrap_set_traits: public Traits
            {
                typedef typename wrap_set_traits_helper<Value, Traits, typename Traits::key_accessor>::key_accessor key_accessor;
            };
        }  // namespace details
        //@endcond


        /// \p SplitListSet traits
        struct traits: public intrusive::split_list::traits
        {
            // Ordered list implementation
            /**
                Selects appropriate ordered-list implementation for split-list.
                Supported types are:
                - \p michael_list_tag - for \p MichaelList
                - \p lazy_list_tag - for \p LazyList
                - \p iterable_list_tag - for \p IterableList
            */
            typedef michael_list_tag    ordered_list;

            // Ordered list traits
            /**
                Specifyes traits for selected ordered list type, default type:
                - for \p michael_list_tag: \p container::michael_list::traits.
                - for \p lazy_list_tag: \p container::lazy_list::traits.
                - for \p iterable_list_tag: \p container::iterable_list::traits.

                If this type is \p opt::none, the ordered list traits is combined with default
                ordered list traits and split-list traits.
            */
            typedef opt::none           ordered_list_traits;

            //@cond
            typedef opt::none           key_accessor;
            //@endcond
        };

        /// Option to select ordered list class for split-list
        /**
            This option selects appropriate ordered list class for containers based on split-list.
            Template parameter \p Type may be \p michael_list_tag or \p lazy_list_tag.
        */
        template <class Type>
        struct ordered_list
        {
            //@cond
            template<class Base> struct pack: public Base
            {
                typedef Type ordered_list;
            };
            //@endcond
        };

        /// Option to specify ordered list type traits
        /**
            The \p Type template parameter specifies ordered list type traits.
            It depends on type of ordered list selected.
        */
        template <class Type>
        struct ordered_list_traits
        {
            //@cond
            template<class Base> struct pack: public Base
            {
                typedef Type ordered_list_traits;
            };
            //@endcond
        };

        /// Metafunction converting option list to traits struct
        /**
            Available \p Options:
            - \p split_list::ordered_list - a tag for ordered list implementation.
            - \p split_list::ordered_list_traits - type traits for ordered list implementation.
                For \p MichaelList use \p container::michael_list::traits or derivatives,
                for \p LazyList use \p container::lazy_list::traits or derivatives.
            - plus any option from \p intrusive::split_list::make_traits
        */
        template <typename... Options>
        struct make_traits {
            typedef typename cds::opt::make_options< traits, Options...>::type type  ;   ///< Result of metafunction
        };
    }   // namespace split_list

    //@cond
    // Forward declarations
    template <class GC, class T, class Traits = split_list::traits>
    class SplitListSet;

    template <class GC, typename Key, typename Value, class Traits = split_list::traits>
    class SplitListMap;
    //@endcond

    //@cond
    // Forward declaration
    namespace details {
        template <typename GC, typename T, typename OrderedListTag, typename Traits>
        struct make_split_list_set;

        template <typename GC, typename Key, typename Value, typename OrderedListTag, typename Traits>
        struct make_split_list_map;
    }
    //@endcond

}}  // namespace cds::container


#endif // #ifndef CDSLIB_CONTAINER_DETAILS_SPLIT_LIST_BASE_H
