//$$CDS-header$$

#ifndef __CDS_INTRUSIVE_DETAILS_BRONSON_AVLTREE_BASE_H
#define __CDS_INTRUSIVE_DETAILS_BRONSON_AVLTREE_BASE_H

#include <type_traits>
#include <cds/intrusive/details/base.h>
#include <cds/opt/options.h>
#include <cds/urcu/options.h>
#include <cds/opt/value_cleaner.h>

namespace cds { namespace intrusive {

    /// BronsonAVLTree related declarations
    namespace bronson_avltree {

        //@cond
        template <typename Node>
        struct base_node
        {
            typedef Node node_type;

            atomics::atomic< int >          m_nHeight;  ///< Node height
            atomics::atomic<uint64_t>       m_nVersion; ///< Version bits
            atomics::atomic<node_type *>    m_pParent;  ///< Parent node
            atomics::atomic<node_type *>    m_pLeft;    ///< Left child
            atomics::atomic<node_type *>    m_pRight;   ///< Right child
            atomics::atomic<uint32_t>       m_flags;    ///< Internal flags

            enum class version : uint64_t
            {
                unlinked = 1,
                growing = 2,
                shrinking = 4,
                grow_count_increment = 1 << 3,
                grow_count_mask = 0xff << 3,
                shrink_count_increment = 1 << 11,
                ignore_grow = ~(growing | grow_count_mask)
            };

            enum class flags : uint32_t
            {
                lock_bit = 1,       ///< spin-lock bit
                value_null = 2,     ///< "value is null" flag
            };

            base_node()
                : m_nHeight( 0 )
                , m_nVersion( 0 )
                , m_pParent( nullptr )
                , m_pLeft( nullptr )
                , m_pRight( nullptr )
                , m_flags( 0 )
            {}

            atomics::atomic<node_type *>& child( int nDirection )
            {
                assert( nDirection != 0 );
                return nDirection < 0 ? m_pLeft : m_pRight;
            }
        };
        //@endcond

        /// Bronson's AVLtree node
        /**
            Template parameters:
            - \p GC - one of \ref cds_garbage_collector "garbage collector type"
            - \p Tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template <typename GC, typename Tag = opt::none>
        struct node : public base_node< node >
        {};

        //@cond
        struct undefined_gc;
        struct default_hook {
            typedef undefined_gc    gc;
            typedef opt::none       tag;
        };
        //@endcond

        //@cond
        template < typename HookType, typename... Options>
        struct hook
        {
            typedef typename opt::make_options< default_hook, Options...>::type  options;
            typedef typename options::gc    gc;
            typedef typename options::tag   tag;
            typedef node<gc, tag>           node_type;
            typedef HookType                hook_type;
        };
        //@endcond

        /// Base hook
        /**
            \p Options are:
            - \p opt::gc - garbage collector
            - \p opt::tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template < typename... Options >
        struct base_hook: public hook< opt::base_hook_tag, Options... >
        {};

        /// Member hook
        /**
            \p MemberOffset defines offset in bytes of \ref node member into your structure.
            Use \p offsetof macro to define \p MemberOffset

            \p Options are:
            - \p opt::gc - garbage collector
            - \p opt::tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template < size_t MemberOffset, typename... Options >
        struct member_hook: public hook< opt::member_hook_tag, Options... >
        {
            //@cond
            static CDS_CONSTEXPR const size_t c_nMemberOffset = MemberOffset;
            //@endcond
        };

        /// Traits hook
        /**
            \p NodeTraits defines type traits for node.
            See \ref node_traits for \p NodeTraits interface description

            \p Options are:
            - opt::gc - garbage collector
            - \p opt::tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template <typename NodeTraits, typename... Options >
        struct traits_hook: public hook< opt::traits_hook_tag, Options... >
        {
            //@cond
            typedef NodeTraits node_traits;
            //@endcond
        };

        /// BronsonAVLTree internal statistics
        template <typename Counter = cds::atomicity::event_counter>
        struct stat {
            typedef Counter   event_counter; ///< Event counter type
        };

        /// BronsonAVLTree empty statistics
        struct empty_stat {
            //@cond
            //@endcond
        };

        /// BronsnAVLTree traits
        struct traits
        {
            /// Hook used
            /**
                Possible values are: \p bronson_avltree::base_hook, \p bronson_avltree::member_hook, \p bronson_avltree::traits_hook.
            */
            typedef base_hook<>       hook;

            /// Key comparison functor
            /**
                No default functor is provided. If the option is not specified, the \p less is used.

                See \p cds::opt::compare option description for functor interface.

                You should provide \p compare or \p less functor.
            */
            typedef opt::none                       compare;

            /// Specifies binary predicate used for key compare.
            /**
                See \p cds::opt::less option description for predicate interface.

                You should provide \p compare or \p less functor.
            */
            typedef opt::none                       less;

            /// Value cleaner
            /**
                The Bronson et al AVLTree algorithm uses partially extenal trees, in witch routing nodes are
                only created during the removal of a node with two children. Routing nodes are never created
                during insertion, and routing nodes with fewer then two children are unlinked during rebalancing.
                Routing nodes should contain only key field(s), other data field(s) is unneeded. When a leaf
                becomes the routing node, the \p %value_cleaner functor is called under the node lock
                for that node to clean all fields except key.
                By default, the functor is disabled (\p opt::v::empty_cleaner).
                \p opt::v::destruct_cleaner is prohibited since it can destruct key field.
            */
            typedef opt::v::empty_cleaner           value_cleaner;

            /// Disposer
            /**
                The functor used for dispose removed items. Default is \p opt::v::empty_disposer.
            */
            typedef opt::v::empty_disposer          disposer;

            /// Item counter
            /**
                The type for item counter, by default it is disabled (\p atomicity::empty_item_counter).
                To enable it use \p atomicity::item_counter
            */
            typedef atomicity::empty_item_counter     item_counter;

            /// C++ memory ordering model
            /**
                List of available memory ordering see \p opt::memory_model
            */
            typedef opt::v::relaxed_ordering        memory_model;

            /// Internal statistics
            /**
                By default, internal statistics is disabled (\p ellen_bintree::empty_stat).
                To enable it use \p ellen_bintree::stat.
            */
            typedef empty_stat                      stat;

            /// Back-off strategy
            typedef cds::backoff::empty             back_off;

            /// RCU deadlock checking policy (only for \ref cds_intrusive_BronsonAVLTree_rcu "RCU-based BronsonAVLTree")
            /**
                List of available options see \p opt::rcu_check_deadlock
            */
            typedef cds::opt::v::rcu_throw_deadlock      rcu_check_deadlock;
        };

        /// Metafunction converting option list to BronsonAVLTree traits
        /**
            \p Options are:
            - \p opt::hook - hook used. Possible values are: \p bronson_avltree::base_hook, \p bronson_avltree::member_hook, \p bronson_avltree::traits_hook.
                If the option is not specified, <tt>bronson_avltree::base_hook<></tt> is used.
            - \p opt::compare - key compare functor. No default functor is provided.
                If the option is not specified, \p %opt::less is used.
            - \p opt::less - specifies binary predicate used for key compare. At least \p %opt::compare or \p %opt::less should be defined.
            - \p opt::value_cleaner - the functor is used to clean all value fields except key, see \p traits::value_cleaner.
                By default, the emoty cleaner \p opt::v::empty_cleaner is used. Note that \p opt::v::destruct_cleaner is prohibited
                since it can clean the key field(s).
            - \p opt::disposer - the functor used for dispose removed nodes. Default is \p opt::v::empty_disposer. Due the nature
                of GC schema the disposer may be called asynchronously.
            - \p opt::item_counter - the type of item counting feature, by default it is disabled (\p atomicity::empty_item_counter)
                To enable it use \p atomicity::item_counter
            - \p opt::memory_model - C++ memory ordering model. Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
            - \p opt::stat - internal statistics, by default it is disabled (\p bronson_avltree::empty_stat)
                To enable statistics use \p \p bronson_avltree::stat
            - \p opt::backoff - back-off strategy, by default no strategy is used (\p cds::backoff::empty)
            - \p opt::rcu_check_deadlock - a deadlock checking policy for RCU-based tree, default is \p opt::v::rcu_throw_deadlock
        */
        template <typename... Options>
        struct make_traits {
#   ifdef CDS_DOXYGEN_INVOKED
            typedef implementation_defined type ;   ///< Metafunction result
#   else
            typedef typename cds::opt::make_options<
                typename cds::opt::find_type_traits< traits, Options... >::type
                ,Options...
            >::type   type;
#   endif
        };

    } // namespace bronson_avltree

    // Forwards
    template < class GC, typename T, class Traits = bronson_avltree::traits >
    class BronsonAVLTree;

}} // namespce cds::intrusive

#endif // #ifndef __CDS_INTRUSIVE_DETAILS_BRONSON_AVLTREE_BASE_H
