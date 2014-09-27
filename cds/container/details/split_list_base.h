//$$CDS-header$$

#ifndef __CDS_CONTAINER_DETAILS_SPLIT_LIST_BASE_H
#define __CDS_CONTAINER_DETAILS_SPLIT_LIST_BASE_H

#include <cds/intrusive/details/split_list_base.h>

namespace cds { namespace container {

    // forward declaration
    struct michael_list_tag;

    /// SplitListSet related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace split_list {
        using intrusive::split_list::dynamic_bucket_table;

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


        /// Type traits for SplitListSet class
        /**
            Note, the SplitListSet type traits is based on intrusive::split_list::type_traits.
            Any member declared in intrusive::split_list::type_traits is also applied to
            container::split_list::type_traits.
        */
        struct type_traits: public intrusive::split_list::type_traits
        {
            // Ordered list implementation
            /**
                This option selects appropriate ordered-list implementation for split-list.
                It may be \ref michael_list_tag or \ref lazy_list_tag.
            */
            typedef michael_list_tag    ordered_list;

            // Ordered list traits
            /**
                With this option you can specify type traits for selected ordered list class.
                If this option is opt::none, the ordered list traits is combined with default
                ordered list traits and split-list traits.

                For \p michael_list_tag, the default traits is \ref container::michael_list::type_traits.

                For \p lazy_list_tag, the default traits is \ref container::lazy_list::type_traits.
            */
            typedef opt::none           ordered_list_traits;

            //@cond
            typedef opt::none           key_accessor;
            //@endcond
        };

        /// Option to select ordered list class for split-list
        /**
            This option selects appropriate ordered list class for containers based on split-list.
            Template parameter \p Type may be \ref michael_list_tag or \ref lazy_list_tag.
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
            - split_list::ordered_list - a tag for ordered list implementation.
                See split_list::ordered_list for possible values.
            - split_list::ordered_list_traits - type traits for ordered list implementation.
                For MichaelList use container::michael_list::type_traits,
                for LazyList use container::lazy_list::type_traits.
            - plus any option from intrusive::split_list::make_traits
        */
        template <typename... Options>
        struct make_traits {
            typedef typename cds::opt::make_options< type_traits, Options...>::type type  ;   ///< Result of metafunction
        };
    }   // namespace split_list

    //@cond
    // Forward declarations
    template <class GC, class T, class Traits = split_list::type_traits>
    class SplitListSet;

    template <class GC, typename Key, typename Value, class Traits = split_list::type_traits>
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


#endif // #ifndef __CDS_CONTAINER_DETAILS_SPLIT_LIST_BASE_H
