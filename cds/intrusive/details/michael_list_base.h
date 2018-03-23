// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_DETAILS_MICHAEL_LIST_BASE_H
#define CDSLIB_INTRUSIVE_DETAILS_MICHAEL_LIST_BASE_H

#include <type_traits>
#include <cds/intrusive/details/base.h>
#include <cds/opt/compare.h>
#include <cds/algo/atomic.h>
#include <cds/details/marked_ptr.h>
#include <cds/urcu/options.h>

namespace cds { namespace intrusive {

    /// MichaelList ordered list related definitions
    /** @ingroup cds_intrusive_helper
    */
    namespace michael_list {
        /// Michael's list node
        /**
            Template parameters:
            - \p GC - garbage collector
            - \p Tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template <class GC, typename Tag = opt::none>
        struct node
        {
            typedef GC              gc  ;   ///< Garbage collector
            typedef Tag             tag ;   ///< tag

            typedef cds::details::marked_ptr<node, 1>   marked_ptr;   ///< marked pointer
            typedef typename gc::template atomic_marked_ptr<marked_ptr> atomic_marked_ptr;   ///< atomic marked pointer specific for GC

            atomic_marked_ptr m_pNext ; ///< pointer to the next node in the container

            constexpr node() noexcept
                : m_pNext( nullptr )
            {}
        };

        //@cond
        template <typename GC, typename Node, typename MemoryModel>
        struct node_cleaner {
            void operator()( Node * p )
            {
                typedef typename Node::marked_ptr marked_ptr;
                p->m_pNext.store( marked_ptr(), MemoryModel::memory_order_release );
            }
        };
        //@endcond

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
            typedef node<gc, tag>   node_type;
            typedef HookType        hook_type;
        };
        //@endcond

        /// Base hook
        /**
            \p Options are:
            - opt::gc - garbage collector used.
            - opt::tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template < typename... Options >
        struct base_hook: public hook< opt::base_hook_tag, Options... >
        {};

        /// Member hook
        /**
            \p MemberOffset defines offset in bytes of \ref node member into your structure.
            Use \p offsetof macro to define \p MemberOffset

            \p Options are:
            - opt::gc - garbage collector used.
            - opt::tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template < size_t MemberOffset, typename... Options >
        struct member_hook: public hook< opt::member_hook_tag, Options... >
        {
            //@cond
            static const size_t c_nMemberOffset = MemberOffset;
            //@endcond
        };

        /// Traits hook
        /**
            \p NodeTraits defines type traits for node.
            See \ref node_traits for \p NodeTraits interface description

            \p Options are:
            - opt::gc - garbage collector used.
            - opt::tag - a \ref cds_intrusive_hook_tag "tag"
        */
        template <typename NodeTraits, typename... Options >
        struct traits_hook: public hook< opt::traits_hook_tag, Options... >
        {
            //@cond
            typedef NodeTraits node_traits;
            //@endcond
        };

        /// Checks link
        template <typename Node>
        struct link_checker
        {
            //@cond
            typedef Node node_type;
            //@endcond

            /// Checks if the link field of node \p pNode is \p nullptr
            /**
                An asserting is generated if \p pNode link field is not \p nullptr
            */
            static void is_empty( const node_type * pNode )
            {
                assert( pNode->m_pNext.load( atomics::memory_order_relaxed ) == nullptr );
                CDS_UNUSED( pNode );
            }
        };

        //@cond
        template <class GC, typename Node, opt::link_check_type LinkType >
        struct link_checker_selector;

        template <typename GC, typename Node>
        struct link_checker_selector< GC, Node, opt::never_check_link >
        {
            typedef intrusive::opt::v::empty_link_checker<Node>  type;
        };

        template <typename GC, typename Node>
        struct link_checker_selector< GC, Node, opt::debug_check_link >
        {
#       ifdef _DEBUG
            typedef link_checker<Node>  type;
#       else
            typedef intrusive::opt::v::empty_link_checker<Node>  type;
#       endif
        };

        template <typename GC, typename Node>
        struct link_checker_selector< GC, Node, opt::always_check_link >
        {
            typedef link_checker<Node>  type;
        };
        //@endcond

        /// Metafunction for selecting appropriate link checking policy
        template < typename Node, opt::link_check_type LinkType >
        struct get_link_checker
        {
            //@cond
            typedef typename link_checker_selector< typename Node::gc, Node, LinkType>::type type;
            //@endcond
        };


        /// \p MichaelList internal statistics
        template <typename EventCounter = cds::atomicity::event_counter>
        struct stat {
            typedef EventCounter event_counter; ///< Event counter type

            event_counter   m_nInsertSuccess;   ///< Number of success \p insert() operations
            event_counter   m_nInsertFailed;    ///< Number of failed \p insert() operations
            event_counter   m_nInsertRetry;     ///< Number of attempts to insert new item
            event_counter   m_nUpdateNew;       ///< Number of new item inserted for \p update()
            event_counter   m_nUpdateExisting;  ///< Number of existing item updates
            event_counter   m_nUpdateFailed;    ///< Number of failed \p update() call
            event_counter   m_nUpdateRetry;     ///< Number of attempts to \p update() the item
            event_counter   m_nUpdateMarked;    ///< Number of attempts to \p update() logically deleted (marked) items
            event_counter   m_nEraseSuccess;    ///< Number of successful \p erase(), \p unlink(), \p extract() operations
            event_counter   m_nEraseFailed;     ///< Number of failed \p erase(), \p unlink(), \p extract() operations
            event_counter   m_nEraseRetry;      ///< Number of attempts to \p erase() an item
            event_counter   m_nFindSuccess;     ///< Number of successful \p find() and \p get() operations
            event_counter   m_nFindFailed;      ///< Number of failed \p find() and \p get() operations

            event_counter   m_nHelpingSuccess;  ///< Number of successful help attempts to remove marked item during searching
            event_counter   m_nHelpingFailed;   ///< Number if failed help attempts to remove marked item during searching

            //@cond
            void onInsertSuccess()      { ++m_nInsertSuccess;   }
            void onInsertFailed()       { ++m_nInsertFailed;    }
            void onInsertRetry()        { ++m_nInsertRetry;     }
            void onUpdateNew()          { ++m_nUpdateNew;       }
            void onUpdateExisting()     { ++m_nUpdateExisting;  }
            void onUpdateFailed()       { ++m_nUpdateFailed;    }
            void onUpdateRetry()        { ++m_nUpdateRetry;     }
            void onUpdateMarked()       { ++m_nUpdateMarked;    }
            void onEraseSuccess()       { ++m_nEraseSuccess;    }
            void onEraseFailed()        { ++m_nEraseFailed;     }
            void onEraseRetry()         { ++m_nEraseRetry;      }
            void onFindSuccess()        { ++m_nFindSuccess;     }
            void onFindFailed()         { ++m_nFindFailed;      }

            void onHelpingSuccess()     { ++m_nHelpingSuccess;  }
            void onHelpingFailed()      { ++m_nHelpingFailed;   }
            //@endcond
        };

        /// \p MichaelList empty internal statistics
        struct empty_stat {
            //@cond
            void onInsertSuccess()              const {}
            void onInsertFailed()               const {}
            void onInsertRetry()                const {}
            void onUpdateNew()                  const {}
            void onUpdateExisting()             const {}
            void onUpdateFailed()               const {}
            void onUpdateRetry()                const {}
            void onUpdateMarked()               const {}
            void onEraseSuccess()               const {}
            void onEraseFailed()                const {}
            void onEraseRetry()                 const {}
            void onFindSuccess()                const {}
            void onFindFailed()                 const {}

            void onHelpingSuccess()             const {}
            void onHelpingFailed()              const {}
            //@endcond
        };

        //@cond
        template <typename Stat = michael_list::stat<>>
        struct wrapped_stat {
            typedef Stat stat_type;

            wrapped_stat( stat_type& st )
                : m_stat( st )
            {}

            void onInsertSuccess()      { m_stat.onInsertSuccess();     }
            void onInsertFailed()       { m_stat.onInsertFailed();      }
            void onInsertRetry()        { m_stat.onInsertRetry();       }
            void onUpdateNew()          { m_stat.onUpdateNew();         }
            void onUpdateExisting()     { m_stat.onUpdateExisting();    }
            void onUpdateFailed()       { m_stat.onUpdateFailed();      }
            void onUpdateRetry()        { m_stat.onUpdateRetry();       }
            void onUpdateMarked()       { m_stat.onUpdateMarked();      }
            void onEraseSuccess()       { m_stat.onEraseSuccess();      }
            void onEraseFailed()        { m_stat.onEraseFailed();       }
            void onEraseRetry()         { m_stat.onEraseRetry();        }
            void onFindSuccess()        { m_stat.onFindSuccess();       }
            void onFindFailed()         { m_stat.onFindFailed();        }

            void onHelpingSuccess()     { m_stat.onHelpingSuccess();    }
            void onHelpingFailed()      { m_stat.onHelpingFailed();     }

            stat_type& m_stat;
        };
        //@endcond

        /// MichaelList traits
        struct traits
        {
            /// Hook used
            /**
                Possible values are: \p michael_list::base_hook, \p michael_list::member_hook, \p michael_list::traits_hook.
            */
            typedef base_hook<>       hook;

            /// Key comparison functor
            /**
                No default functor is provided. If the option is not specified, the \p less is used.
            */
            typedef opt::none                       compare;

            /// Specifies binary predicate used for key compare.
            /**
                Default is \p std::less<T>.
            */
            typedef opt::none                       less;

            /// Back-off strategy
            typedef cds::backoff::Default           back_off;

            /// Disposer for removing items
            typedef opt::v::empty_disposer          disposer;

            /// Item counting feature; by default, disabled. Use \p cds::atomicity::item_counter or \p atomicity::cache_friendly_item_counter to enable item counting
            typedef atomicity::empty_item_counter   item_counter;

            /// Internal statistics
            /**
                By default, internal statistics is disabled (\p michael_list::empty_stat).
                Use \p michael_list::stat to enable it.
            */
            typedef empty_stat                      stat;

            /// Link fields checking feature
            /**
                Default is \p opt::debug_check_link
            */
            static const opt::link_check_type link_checker = opt::debug_check_link;

            /// C++ memory ordering model
            /**
                Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
            */
            typedef opt::v::relaxed_ordering        memory_model;

            /// RCU deadlock checking policy (only for \ref cds_intrusive_MichaelList_rcu "RCU-based MichaelList")
            /**
                List of available policy see \p opt::rcu_check_deadlock
            */
            typedef opt::v::rcu_throw_deadlock      rcu_check_deadlock;
        };

        /// Metafunction converting option list to \p michael_list::traits
        /**
            Supported \p Options are:
            - \p opt::hook - hook used. Possible values are: \p michael_list::base_hook, \p michael_list::member_hook, \p michael_list::traits_hook.
                If the option is not specified, \p %michael_list::base_hook<> and \p gc::HP is used.
            - \p opt::compare - key comparison functor. No default functor is provided.
                If the option is not specified, the \p opt::less is used.
            - \p opt::less - specifies binary predicate used for key comparison. Default is \p std::less<T>.
            - \p opt::back_off - back-off strategy used. If the option is not specified, the \p cds::backoff::Default is used.
            - \p opt::disposer - the functor used for disposing removed items. Default is \p opt::v::empty_disposer. Due the nature
                of GC schema the disposer may be called asynchronously.
            - \p opt::link_checker - the type of node's link fields checking. Default is \p opt::debug_check_link
            - \p opt::item_counter - the type of item counting feature. Default is disabled (\p atomicity::empty_item_counter).
                 To enable item counting use \p atomicity::item_counter or \p atomicity::cache_friendly_item_counter
            - \p opt::stat - internal statistics. By default, it is disabled (\p michael_list::empty_stat).
                To enable it use \p michael_list::stat
            - \p opt::memory_model - C++ memory ordering model. Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consistent memory model).
            - \p opt::rcu_check_deadlock - a deadlock checking policy for \ref cds_intrusive_MichaelList_rcu "RCU-based MichaelList"
                Default is \p opt::v::rcu_throw_deadlock
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


        //@cond
        template <typename Stat>
        struct select_stat_wrapper
        {
            typedef Stat stat;
            typedef michael_list::wrapped_stat<Stat> wrapped_stat;
            enum {
                empty = false
            };
        };

        template <>
        struct select_stat_wrapper< empty_stat >
        {
            typedef empty_stat stat;
            typedef empty_stat wrapped_stat;
            enum {
                empty = true
            };
        };

        template <typename Stat>
        struct select_stat_wrapper< michael_list::wrapped_stat<Stat>>: public select_stat_wrapper< Stat >
        {};

        //@endcond

    } // namespace michael_list

    //@cond
    // Forward declaration
    template < class GC, typename T, class Traits = michael_list::traits >
    class MichaelList;
    //@endcond


    //@cond
    template <typename List>
    struct is_michael_list {
        enum {
            value = false
        };
    };

    template <typename GC, typename T, typename Traits>
    struct is_michael_list< MichaelList< GC, T, Traits >> {
        enum {
            value = true
        };
    };
    //@endcond

}}   // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_DETAILS_MICHAEL_LIST_BASE_H
