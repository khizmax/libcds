// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_VYUKOV_MPMC_CYCLE_QUEUE_H
#define CDSLIB_INTRUSIVE_VYUKOV_MPMC_CYCLE_QUEUE_H

#include <cds/intrusive/details/base.h>
#include <cds/container/vyukov_mpmc_cycle_queue.h>

namespace cds { namespace intrusive {

    /// VyukovMPMCCycleQueue related definitions
    /** @ingroup cds_intrusive_helper
    */
    namespace vyukov_queue {

        /// VyukovMPMCCycleQueue traits
        struct traits : public cds::container::vyukov_queue::traits
        {
            /// The functor used for dispose removed items. Default is \p opt::v::empty_disposer. This option is used only in \p clear()
            typedef opt::v::empty_disposer  disposer;
        };

        /// Metafunction converting option list to \p vyukov_queue::traits
        /**
            Supported \p Options are:
            - \p opt::buffer - an uninitialized buffer type for internal cyclic array. Possible types are:
                \p opt::v::uninitialized_dynamic_buffer (the default), \p opt::v::uninitialized_static_buffer. The type of
                element in the buffer is not important: it will be changed via \p rebind metafunction.
            - \p opt::disposer - the functor used for dispose removed items. Default is \p opt::v::empty_disposer.
                This option is used only in \p clear() member function.
            - \p opt::item_counter - the type of item counting feature. Default is \p cds::atomicity::empty_item_counter (item counting disabled)
                To enable item counting use \p cds::atomicity::item_counter
            - \p opt::back_off - back-off strategy used. If the option is not specified, the \p cds::backoff::Default is used.
            - \p opt::padding - padding for internal critical atomic data. Default is \p opt::cache_line_padding
            - \p opt::memory_model - C++ memory ordering model. Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consistent memory model).

            Example: declare \p %VyukovMPMCCycleQueue with item counting and static internal buffer of size 1024:
            \code
            typedef cds::intrusive::VyukovMPMCCycleQueue< Foo,
                typename cds::intrusive::vyukov_queue::make_traits<
                    cds::opt::buffer< cds::opt::v::uninitialized_static_buffer< void *, 1024 >,
                    cds::opt::item_counter< cds::atomicity::item_counter >
                >::type
            > myQueue;
            \endcode
        */
        template <typename... Options>
        struct make_traits {
#   ifdef CDS_DOXYGEN_INVOKED
            typedef implementation_defined type;   ///< Metafunction result
#   else
            typedef typename cds::opt::make_options<
                typename cds::opt::find_type_traits< traits, Options... >::type
                , Options...
            >::type type;
#   endif
        };

    } // namespace vyukov_queue

    /// Vyukov's MPMC bounded queue
    /** @ingroup cds_intrusive_queue
        This algorithm is developed by Dmitry Vyukov (see http://www.1024cores.net)

        Implementation of intrusive version is based on container::VyukovMPMCCycleQueue.

        Template parameters:
        - \p T - type stored in queue.
        - \p Traits - queue traits, default is \p vyukov_queue::traits. You can use \p vyukov_queue::make_traits
            metafunction to make your traits or just derive your traits from \p %vyukov_queue::traits:
            \code
            struct myTraits: public cds::intrusive::vyukov_queue::traits {
                typedef cds::atomicity::item_counter    item_counter;
            };
            typedef cds::intrusive::VyukovMPMCCycleQueue< Foo, myTraits > myQueue;

            // Equivalent make_traits example:
            typedef cds::intrusive::VyukovMPMCCycleQueue< cds::gc::HP, Foo,
                typename cds::intrusive::vyukov_queue::make_traits<
                    cds::opt::item_counter< cds::atomicity::item_counter >
                >::type
            > myQueue;
            \endcode

        Instead of saving copy of enqueued data, the intrusive implementation stores pointer to passed data.

        \par Examples:
        \code
        #include <cds/intrusive/vyukov_mpmc_cycle_queue.h>

        struct Foo {
            ...
        };

        // Queue of Foo pointers, capacity is 1024, statically allocated buffer:
        typedef cds::intrusive::VyukovMPMCCycleQueue< Foo,
            typename cds::intrusive::vyukov_queue::make_traits<
                cds::opt::buffer< cds::opt::v::uninitialized_static_buffer< Foo, 1024 > >
            >::type
        > static_queue;
        static_queue    stQueue;

        // Queue of Foo pointers, capacity is 1024, dynamically allocated buffer:
        struct queue_traits: public cds::intrusive::vyukov_queue::traits
        {
            typedef cds::opt::v::uninitialized_dynamic_buffer< Foo > buffer;
        };
        typedef cds::intrusive::VyukovMPMCCycleQueue< Foo, queue_traits > dynamic_queue;
        dynamic_queue    dynQueue( 1024 );
        \endcode
    */
    template <typename T, typename Traits = vyukov_queue::traits >
    class VyukovMPMCCycleQueue
        : private container::VyukovMPMCCycleQueue< T*, Traits >
    {
        //@cond
        typedef container::VyukovMPMCCycleQueue< T*, Traits > base_class;
        //@endcond
    public:
        typedef T value_type;   ///< type of data to be stored in the queue
        typedef Traits traits;  ///< Queue traits
        typedef typename traits::item_counter item_counter; ///< Item counter type
        typedef typename traits::memory_model memory_model; ///< Memory ordering. See cds::opt::memory_model option
        typedef typename traits::disposer     disposer;     ///< Item disposer
        typedef typename traits::back_off     back_off;     ///< back-off strategy

    public:
        /// Rebind template arguments
        template <typename T2, typename Traits2>
        struct rebind {
            typedef VyukovMPMCCycleQueue< T2, Traits2> other   ;   ///< Rebinding result
        };

    public:
        /// Constructs the queue of capacity \p nCapacity
        /**
            For \p cds::opt::v::uninitialized_static_buffer the \p nCapacity parameter is ignored.
        */
        VyukovMPMCCycleQueue( size_t nCapacity = 0 )
            : base_class( nCapacity )
        {}

        /// Enqueues \p data to queue
        /**
            @note The intrusive queue stores pointer to \p data passed, not the copy of \p data.
        */
        bool enqueue( value_type& data )
        {
            return base_class::enqueue( &data );
        }

        /// Dequeues an item from queue
        /**
            \p Traits::disposer is not called. You may manually delete the returned pointer.

            If queue is empty, returns \p nullptr.
        */
        value_type * dequeue()
        {
            value_type * p = nullptr;
            return base_class::dequeue( p ) ? p : nullptr;
        }

        /// Synonym for \p enqueue()
        bool push( value_type& data )
        {
            return enqueue( data );
        }

        /// Synonym for \p dequeue()
        value_type * pop()
        {
            return dequeue();
        }

        /// Clears queue in lock-free manner.
        /**
            \p f parameter is a functor to dispose removed items.
            The interface of \p Disposer is:
            \code
            struct myDisposer {
                void operator ()( T * val );
            };
            \endcode
            The disposer will be called immediately for each item.
        */
        template <typename Disposer>
        void clear( Disposer f )
        {
            value_type * pv;
            while ( (pv = pop()) != nullptr ) {
                f( pv );
            }
        }

        /// Clears the queue
        /**
            This function uses the disposer that is specified in \p Traits.
        */
        void clear()
        {
            clear( disposer());
        }

        /// Checks if the queue is empty
        bool empty() const
        {
            return base_class::empty();
        }

        /// Returns queue's item count
        /**
            The value returned depends on \p vyukov_queue::traits::item_counter option.
            For \p atomicity::empty_item_counter, this function always returns 0.
        */
        size_t size() const
        {
            return base_class::size();
        }

        /// Returns capacity of the queue
        size_t capacity() const
        {
            return base_class::capacity();
        }
    };
}}  // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_VYUKOV_MPMC_CYCLE_QUEUE_H
