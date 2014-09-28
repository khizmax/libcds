//$$CDS-header$$

#ifndef __CDS_INTRUSIVE_VYUKOV_MPMC_CYCLE_QUEUE_H
#define __CDS_INTRUSIVE_VYUKOV_MPMC_CYCLE_QUEUE_H

#include <cds/intrusive/details/base.h>
#include <cds/container/vyukov_mpmc_cycle_queue.h>

namespace cds { namespace intrusive {

    /// Vyukov's MPMC bounded queue
    /** @ingroup cds_intrusive_queue
        This algorithm is developed by Dmitry Vyukov (see http://www.1024cores.net)

        Implementation of intrusive version is based on non-intrusive class container::VyukovMPMCCycleQueue.

        Template parameters:
        - \p T - type stored in queue.
        - \p Options - queue's options

        Options \p Options are:
        - opt::buffer - buffer to store items. Mandatory option, see option description for full list of possible types.
        - opt::item_counter - the type of item counting feature. Default is \ref atomicity::empty_item_counter
        - opt::disposer - the functor used for dispose removed items. Default is opt::v::empty_disposer. This option is used
            only in \ref clear function.
        - opt::alignment - the alignment for internal queue data. Default is opt::cache_line_alignment
        - opt::memory_model - C++ memory ordering model. Can be opt::v::relaxed_ordering (relaxed memory model, the default)
            or opt::v::sequential_consistent (sequentially consisnent memory model).


        Instead of saving copy of enqueued data, the intrusive implementation stores pointer to passed data.

        \par Examples:
        \code
        #include <cds/intrusive/vyukov_mpmc_cycle_queue.h>

        struct Foo {
            ...
        };

        // Queue of Foo pointers, capacity is 1024, statically allocated buffer:
        typedef cds::intrusive::VyukovMPMCCycleQueue<
            Foo
            ,cds::opt::buffer< cds::opt::v::static_buffer< Foo, 1024 > >
        > static_queue;
        static_queue    stQueue;

        // Queue of Foo pointers, capacity is 1024, dynamically allocated buffer:
        typedef cds::intrusive::VyukovMPMCCycleQueue<
            Foo
            ,cds::opt::buffer< cds::opt::v::dynamic_buffer< Foo > >
        > dynamic_queue;
        dynamic_queue    dynQueue( 1024 );

        \endcode
    */
    template <typename T, typename... Options>
    class VyukovMPMCCycleQueue
        : private container::VyukovMPMCCycleQueue< T *, Options... >
    {
        //@cond
        typedef container::VyukovMPMCCycleQueue< T *, Options... > base_class;
        //@endcond
    public:
        typedef T value_type    ;   ///< type of data stored in the queue
        typedef typename base_class::item_counter   item_counter    ;   ///< Item counter type
        typedef typename base_class::memory_model   memory_model    ;   ///< Memory ordering. See cds::opt::memory_model option
        typedef typename base_class::options::disposer disposer     ;   ///< Item disposer

        //@cond
        typedef typename base_class::options    options;
        //@endcond

    public:
        /// Rebind template arguments
        template <typename T2, typename... Options2>
        struct rebind {
            typedef VyukovMPMCCycleQueue< T2, Options2...> other   ;   ///< Rebinding result
        };

    public:
        /// Constructs the queue of capacity \p nCapacity
        /**
            For cds::opt::v::static_buffer the \p nCapacity parameter is ignored.
        */
        VyukovMPMCCycleQueue( size_t nCapacity = 0 )
            : base_class( nCapacity )
        {}

        /// Enqueues \p data to queue
        /**
            Note that the intrusive queue stores pointer to \p data passed, not the copy of data.
        */
        bool enqueue( value_type& data )
        {
            return base_class::enqueue( &data );
        }

        /// Dequeues an item from queue
        /**
            If queue is empty, returns \p nullptr.
        */
        value_type * dequeue()
        {
            value_type * p = nullptr;
            return base_class::dequeue( p ) ? p : nullptr;
        }

        /// Synonym of \ref enqueue
        bool push( value_type& data )
        {
            return enqueue( data );
        }

        /// Synonym of \ref dequeue
        value_type * pop()
        {
            return dequeue();
        }

        /// Clears queue in lock-free manner.
        /**
            \p f parameter is a functor to dispose removed items.
            The interface of \p DISPOSER is:
            \code
            struct myDisposer {
                void operator ()( T * val );
            };
            \endcode
            You can pass \p disposer by reference using \p std::ref.
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
            This function uses the disposer that is specified in \p Options.
        */
        void clear()
        {
            clear( disposer() );
        }

        /// Checks if the queue is empty
        bool empty() const
        {
            return base_class::empty();
        }


        /// Returns queue's item count
        /**
            The value returned depends on opt::item_counter option. For atomicity::empty_item_counter,
            this function always returns 0.
        */
        size_t size() const
        {
            return base_class::size();
        }

        /// Returns capacity of cyclic buffer
        size_t capacity() const
        {
            return base_class::capacity();
        }
    };
}}  // namespace cds::intrusive

#endif // #ifndef __CDS_INTRUSIVE_VYUKOV_MPMC_CYCLE_QUEUE_H
