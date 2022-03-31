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

#include <type_traits>
#include <cds/intrusive/details/base.h>
#include <cds/opt/compare.h>
#include <cds/algo/atomic.h>
#include <cds/details/marked_ptr.h>
#include <cds/urcu/options.h>

#ifndef CDSLIB_INTRUSIVE_DETAILS_VALOIS_LIST_BASE_H
#define CDSLIB_INTRUSIVE_DETAILS_VALOIS_LIST_BASE_H

namespace cds { namespace intrusive {

    namespace valois_list{

        template <typename T>
        struct node
        {
            typedef T value_type; ///< Value type
            typedef cds::details::marked_ptr<T, 1>   marked_data_ptr; ///< marked pointer to the value

            atomics::atomic< node* >            next;  ///< pointer to next node in the list
            atomics::atomic< marked_data_ptr >  data;  ///< pointer to user data, \p nullptr if the node is free

            node()
            {
                next.store( nullptr, atomics::memory_order_relaxed );
                data.store( marked_data_ptr( NULL ), atomics::memory_order_release );
            }

            node( value_type * pVal )
            {
                next.store( nullptr, atomics::memory_order_relaxed );
                data.store( marked_data_ptr( pVal ), atomics::memory_order_release );
            }

        };


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
        template <typename Stat = valois_list::stat<>>
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
             It's disabled
             */
            typedef empty_stat                      stat;

            /// Item counting feature; by default, disabled. Use \p cds::atomicity::item_counter or \p atomicity::cache_friendly_item_counter to enable item counting
            typedef atomicity::empty_item_counter   item_counter;

            /// C++ memory ordering model
            /**
                Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
            */
            typedef opt::v::relaxed_ordering        memory_model;
        };

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

    }
}}

#endif // #ifndef CDSLIB_INTRUSIVE_DETAILS_VALOIS_LIST_BASE_H
