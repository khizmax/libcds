//$$CDS-header$$

#ifndef __CDS_CONTAINER_DETAILS_BRONSON_AVLTREE_BASE_H
#define __CDS_CONTAINER_DETAILS_BRONSON_AVLTREE_BASE_H

#include <cds/container/details/base.h>
#include <cds/opt/compare.h>
#include <cds/urcu/options.h>
#include <cds/lock/spinlock.h>

namespace cds { namespace container {

    /// BronsonAVLTree related declarations
    namespace bronson_avltree {

        template <typename Key, typename T>
        struct node;

        //@cond
        template <typename Key, typename T, typename Lock>
        struct link
        {
            typedef node<Key, T> node_type;
            typedef uint64_t version_type;
            typedef Lock lock_type;

            enum class version_flags : version_type
            {
                unlinked = 1,
                growing = 2,
                shrinking = 4,
                grow_count_increment = 1 << 3,
                grow_count_mask = 0xff << 3,
                shrink_count_increment = 1 << 11,
                ignore_grow = ~(growing | grow_count_mask)
            };

            atomics::atomic< int >          m_nHeight;  ///< Node height
            atomics::atomic<version_type>   m_nVersion; ///< Version bits
            atomics::atomic<node_type *>    m_pParent;  ///< Parent node
            atomics::atomic<node_type *>    m_pLeft;    ///< Left child
            atomics::atomic<node_type *>    m_pRight;   ///< Right child
            lock_type                       m_Lock;     ///< Node-level lock

            atomics::atomic<node_type *>& child( int nDirection ) const
            {
                assert( nDirection != 0 );
                return nDirection < 0 ? m_pLeft : m_pRight;
            }

            version_type version( atomics::memory_order order ) const
            {
                return m_nVersion.load( order );
            }

            template <typename BackOff>
            void wait_until_shrink_completed( atomics::memory_order order ) const
            {
                BackOff bkoff;
                while ( version( order ) & version_flags::shrinking )
                    bkoff();
            }
        };

        template <typename Key, typename T, typename Lock>
        struct node : public link< Key, T, Lock >
        {
            typedef Key key_type;
            typedef T   mapped_type;
            typedef link< key_type, mapped_type, Lock > base_class;

            key_type const  m_key;
            atomics::atomic<mapped_type *>  m_pValue;

            template <typename Q>
            node( Q&& key )
                : m_key( std::forward<Q>(key) )
                , m_pValue( nullptr )
            {}

            template <typename Q>
            node( Q&& key, mapped_type * pVal )
                : m_key( std::forward<Q>(key) )
                , m_pValue( pVal )
            {}

            T * value( atomics::memory_order order ) const
            {
                return m_pValue.load( order );
            }
        };
        //@endcond

        /// BronsonAVLTreeMap internal statistics
        template <typename Counter = cds::atomicity::event_counter>
        struct stat {
            typedef Counter   event_counter; ///< Event counter type

            event_counter   m_nFindSuccess; ///< Count of success \p find() call
            event_counter   m_nFindFailed;  ///< Count of failed \p find() call
            event_counter   m_nFindRetry;   ///< Count of retries during \p find()
            event_counter   m_nFindWaitShrinking;   ///< Count of waiting until shrinking completed duting \p find() call

            //@cond
            void onFindSuccess()        { ++m_nFindSuccess      ; }
            void onFindFailed()         { ++m_nFindFailed       ; }
            void onFindRetry()          { ++m_nFindRetry        ; }
            void onFindWaitShrinking()  { ++m_nFindWaitShrinking; }

            //@endcond
        };

        /// BronsonAVLTreeMap empty statistics
        struct empty_stat {
            //@cond
            void onFindSuccess()        const {}
            void onFindFailed()         const {}
            void onFindRetry()          const {}
            void onFindWaitShrinking()  const {}
            //@endcond
        };

        /// BronsnAVLTreeMap traits
        /**
            Note that there are two main specialization of Bronson et al AVL-tree:
            - pointer-oriented - the tree node stores an user-provided pointer to value: <tt>BronsonAVLTreeMap<GC, Key, T *, Traits> </tt>
            - data-oriented - the tree node contains a copy of values: <tt>BronsonAVLTreeMap<GC, Key, T, Traits> </tt> where \p T is not a pointer type.

            Depends on tree specialization, different traits member can be used.
        */
        struct traits
        {
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

            /// Allocator for internal node
            typedef CDS_DEFAULT_ALLOCATOR           allocator;

            /// Disposer (only for pointer-oriented tree specialization)
            /**
                The functor used for dispose removed values. 
                The user-provided disposer is used only for pointer-oriented tree specialization
                like \p BronsonAVLTreeMap<GC, Key, T*, Traits>. When the node becomes the rounting node without value,
                the disposer will be called to signal that the memory for the value can be safely freed.
                Default is \ref cds::intrusive::opt::v::delete_disposer "cds::container::opt::v::delete_disposer" which calls \p delete operator.
            */
            typedef opt::v::delete_disposer         disposer;

            /// Node lock
            typedef cds::SpinLock                   lock_type;

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

            //@cond
            // Internal traits, not for direct usage
            typedef opt::none   node_type;
            //@endcond
        };

        /// Metafunction converting option list to BronsonAVLTreeMap traits
        /**
            Note that there are two main specialization of Bronson et al AVL-tree:
            - pointer-oriented - the tree node stores an user-provided pointer to value: <tt>BronsonAVLTreeMap<GC, Key, T *, Traits> </tt>
            - data-oriented - the tree node contains a copy of values: <tt>BronsonAVLTreeMap<GC, Key, T, Traits> </tt> where \p T is not a pointer type.

            Depends on tree specialization, different options can be specified.

            \p Options are:
            - \p opt::compare - key compare functor. No default functor is provided.
                If the option is not specified, \p %opt::less is used.
            - \p opt::less - specifies binary predicate used for key compare. At least \p %opt::compare or \p %opt::less should be defined.
            - \p opt::allocator - the allocator for internal nodes. Default is \ref CDS_DEFAULT_ALLOCATOR.
            - \ref cds::intusive::opt::disposer "container::opt::disposer" - the functor used for dispose removed values.
                The user-provided disposer is used only for pointer-oriented tree specialization
                like \p BronsonAVLTreeMap<GC, Key, T*, Traits>. When the node becomes the rounting node without value,
                the disposer will be called to signal that the memory for the value can be safely freed.
                Default is \ref cds::intrusive::opt::v::delete_disposer "cds::container::opt::v::delete_disposer" which calls \p delete operator.
                Due the nature of GC schema the disposer may be called asynchronously.
            - \p opt::lock_type - node lock type, default is \p cds::SpinLock
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
    template < class GC, typename Key, typename T, class Traits = bronson_avltree::traits >
    class BronsonAVLTreeMap;

}} // namespace cds::container


#endif // #ifndef __CDS_CONTAINER_DETAILS_BRONSON_AVLTREE_BASE_H
