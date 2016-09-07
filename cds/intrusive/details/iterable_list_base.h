/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

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

#ifndef CDSLIB_INTRUSIVE_DETAILS_ITERABLE_LIST_BASE_H
#define CDSLIB_INTRUSIVE_DETAILS_ITERABLE_LIST_BASE_H

#include <type_traits>
#include <cds/intrusive/details/base.h>
#include <cds/opt/compare.h>
#include <cds/algo/atomic.h>
#include <cds/details/marked_ptr.h>
#include <cds/urcu/options.h>

namespace cds { namespace intrusive {

    /// \p IterableList ordered list related definitions
    /** @ingroup cds_intrusive_helper
    */
    namespace iterable_list {

        /// Node type
        template <typename T>
        struct node
        {
            typedef T value_type; ///< Value type

            atomics::atomic< node* >  next;       ///< pointer to next node in the list
            atomics::atomic< value_type* > data; ///< pointer to user data, \p nullptr if the node is free

            //@cond
            node()
                : next( nullptr )
                , data( nullptr )
            {}

            node( value_type * pVal )
                : next( nullptr )
                , data( pVal )
            {}
            //@endcond
        };

        /// \p IterableList internal statistics
        template <typename EventCounter = cds::atomicity::event_counter>
        struct stat {
            typedef EventCounter event_counter; ///< Event counter type

            event_counter   m_nInsertSuccess;   ///< Number of success \p insert() operations
            event_counter   m_nInsertFailed;    ///< Number of failed \p insert() operations
            event_counter   m_nInsertRetry;     ///< Number of attempts to insert new item
            event_counter   m_nUpdateNew;       ///< Number of new item inserted for \p update()
            event_counter   m_nUpdateExisting;  ///< Number of existing item updates
            event_counter   m_nUpdateFailed;    ///< Number of failed \p update() call
            event_counter   m_nUpdateRetry;     ///< Number of attempts to update the item
            event_counter   m_nEraseSuccess;    ///< Number of successful \p erase(), \p unlink(), \p extract() operations
            event_counter   m_nEraseFailed;     ///< Number of failed \p erase(), \p unlink(), \p extract() operations
            event_counter   m_nEraseRetry;      ///< Number of attempts to \p erase() an item
            event_counter   m_nFindSuccess;     ///< Number of successful \p find() and \p get() operations
            event_counter   m_nFindFailed;      ///< Number of failed \p find() and \p get() operations

            event_counter   m_nNodeCreated;     ///< Number of created internal nodes
            event_counter   m_nNodeRemoved;     ///< Number of removed internal nodes

            //@cond
            void onInsertSuccess()      { ++m_nInsertSuccess;   }
            void onInsertFailed()       { ++m_nInsertFailed;    }
            void onInsertRetry()        { ++m_nInsertRetry;     }
            void onUpdateNew()          { ++m_nUpdateNew;       }
            void onUpdateExisting()     { ++m_nUpdateExisting;  }
            void onUpdateFailed()       { ++m_nUpdateFailed;    }
            void onUpdateRetry()        { ++m_nUpdateRetry;     }
            void onEraseSuccess()       { ++m_nEraseSuccess;    }
            void onEraseFailed()        { ++m_nEraseFailed;     }
            void onEraseRetry()         { ++m_nEraseRetry;      }
            void onFindSuccess()        { ++m_nFindSuccess;     }
            void onFindFailed()         { ++m_nFindFailed;      }

            void onNodeCreated()        { ++m_nNodeCreated;     }
            void onNodeRemoved()        { ++m_nNodeRemoved;     }
            //@endcond
        };

        /// \p IterableList empty internal statistics
        struct empty_stat {
            //@cond
            void onInsertSuccess()              const {}
            void onInsertFailed()               const {}
            void onInsertRetry()                const {}
            void onUpdateNew()                  const {}
            void onUpdateExisting()             const {}
            void onUpdateFailed()               const {}
            void onUpdateRetry()                const {}
            void onEraseSuccess()               const {}
            void onEraseFailed()                const {}
            void onEraseRetry()                 const {}
            void onFindSuccess()                const {}
            void onFindFailed()                 const {}

            void onNodeCreated()                const {}
            void onNodeRemoved()                const {}
            //@endcond
        };

        //@cond
        template <typename Stat = iterable_list::stat<>>
        struct wrapped_stat {
            typedef Stat stat_type;

            wrapped_stat( stat_type& st )
                : m_stat( st )
            {}

            void onInsertSuccess()   { m_stat.onInsertSuccess(); }
            void onInsertFailed()    { m_stat.onInsertFailed();  }
            void onInsertRetry()     { m_stat.onInsertRetry();   }
            void onUpdateNew()       { m_stat.onUpdateNew();     }
            void onUpdateExisting()  { m_stat.onUpdateExisting();}
            void onUpdateFailed()    { m_stat.onUpdateFailed();  }
            void onUpdateRetry()     { m_stat.onUpdateRetry();   }
            void onEraseSuccess()    { m_stat.onEraseSuccess();  }
            void onEraseFailed()     { m_stat.onEraseFailed();   }
            void onEraseRetry()      { m_stat.onEraseRetry();    }
            void onFindSuccess()     { m_stat.onFindSuccess();   }
            void onFindFailed()      { m_stat.onFindFailed();    }

            void onNodeCreated()     { m_stat.onNodeCreated();   }
            void onNodeRemoved()     { m_stat.onNodeRemoved();   }

            stat_type& m_stat;
        };
        //@endcond


        /// \p IterableList traits
        struct traits
        {
            /// Key comparison functor
            /**
                No default functor is provided. If the option is not specified, the \p less is used.
            */
            typedef opt::none                       compare;

            /// Specifies binary predicate used for key compare.
            /**
                Default is \p std::less<T>
            */
            typedef opt::none                       less;

            /// Node allocator
            typedef CDS_DEFAULT_ALLOCATOR           node_allocator;

            /// Back-off strategy
            typedef cds::backoff::Default           back_off;

            /// Disposer for removing items
            typedef opt::v::empty_disposer          disposer;

            /// Internal statistics
            /**
                By default, internal statistics is disabled (\p iterable_list::empty_stat).
                Use \p iterable_list::stat to enable it.
            */
            typedef empty_stat                      stat;

            /// Item counting feature; by default, disabled. Use \p cds::atomicity::item_counter to enable item counting
            typedef atomicity::empty_item_counter     item_counter;

            /// C++ memory ordering model
            /**
                Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
            */
            typedef opt::v::relaxed_ordering        memory_model;

            /// RCU deadlock checking policy (only for \ref cds_intrusive_IterableList_rcu "RCU-based IterableList")
            /**
                List of available policy see \p opt::rcu_check_deadlock
            */
            typedef opt::v::rcu_throw_deadlock      rcu_check_deadlock;
        };

        /// Metafunction converting option list to \p iterable_list::traits
        /**
            Supported \p Options are:
            - \p opt::compare - key comparison functor. No default functor is provided.
                If the option is not specified, the \p opt::less is used.
            - \p opt::less - specifies binary predicate used for key comparison. Default is \p std::less<T>.
            - \p opt::node_allocator - node allocator, default is \p std::allocator.
            - \p opt::back_off - back-off strategy used. If the option is not specified, the \p cds::backoff::Default is used.
            - \p opt::disposer - the functor used for disposing removed items. Default is \p opt::v::empty_disposer. Due the nature
                of GC schema the disposer may be called asynchronously.
            - \p opt::item_counter - the type of item counting feature. Default is disabled (\p atomicity::empty_item_counter).
                 To enable item counting use \p atomicity::item_counter.
            - \p opt::stat - internal statistics. By default, it is disabled (\p iterable_list::empty_stat).
                To enable it use \p iterable_list::stat
            - \p opt::memory_model - C++ memory ordering model. Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consistent memory model).
            - \p opt::rcu_check_deadlock - a deadlock checking policy for \ref cds_intrusive_IterableList_rcu "RCU-based IterableList"
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
            typedef iterable_list::wrapped_stat<Stat> wrapped_stat;
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
        struct select_stat_wrapper< iterable_list::wrapped_stat<Stat>>: public select_stat_wrapper<Stat>
        {};
        //@endcond

    } // namespace iterable_list

    //@cond
    // Forward declaration
    template < class GC, typename T, class Traits = iterable_list::traits >
    class IterableList;
    //@endcond

    //@cond
    template <typename GC, typename T, typename Traits>
    struct is_iterable_list< IterableList< GC, T, Traits >> {
        enum {
            value = true
        };
    };
    //@endcond

}}   // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_DETAILS_ITERABLE_LIST_BASE_H
