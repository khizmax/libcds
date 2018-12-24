#ifndef CDSLIB_CONTAINER_TS_DEQUE_H
#define CDSLIB_CONTAINER_TS_DEQUE_H

#include <atomic>
#include <cds/opt/options.h>
#include <cds/container/ts_deque_buffer.h>

namespace cds { namespace container {

    /// TSDeque related definitions
    /** @ingroup cds_nonintrusive_helper
    */
    namespace tsdeque {
        /// TSDeque default type traits
        struct traits
        {
            /// Item counting feature; by default, disabled. Use \p cds::atomicity::item_counter to enable item counting
            typedef cds::atomicity::empty_item_counter item_counter;

            /// Random engine to generate a random position in array of thread-local buffers
            typedef opt::v::c_rand random_engine;
        };

        /// Metafunction converting option list to \p tsdeque::traits
        /**
            Supported \p Options are:
            - opt::item_counter - the type of item counting feature.
                Default is \p cds::atomicity::empty_item_counter (item counting disabled).
            - opt::random_engine - a random engine to generate a random position in array of thread-local buffers.
                Default is \p opt::v::c_rand.

            Example: declare \p %TSDeque with item counting
            \code
            typedef cds::container::TSDeque< Foo,
                typename cds::container::tsdeque::make_traits<
                    cds::opt::item_counter< cds::atomicity::item_counter >
                >::type
            > myDeque;
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

    } // namespace tsdeque

    /// Fast Concurrent Deque Through Explicit Timestamping
    /** @ingroup cds_nonintrusive_deque

        <b>Source</b>
        - [2014] Mike Dodds, Andreas Haas, Christoph M. Kirsch
            "Fast Concurrent Data-Structures Through Explicit Timestamping"

        <b>Template arguments</b>
        - \p T - value type to be stored in the deque
        - \p Timestamp - the way to acquire timestamps
        - \p Traits - deque traits, default is \p tsdeque::traits. You can use \p tsdeque::make_traits
            metafunction to make your traits or just derive your traits from \p %tsdeque::traits:
            \code
            struct myTraits: public cds::container::tsdeque::traits {
                typedef cds::atomicity::item_counter    item_counter;
            };
            typedef cds::container::TSDeque< Foo, Timestamp, myTraits > myDeque;

            // Equivalent make_traits example:
            typedef cds::container::TSDeque< Foo, Timestamp
                typename cds::container::tsdeque::make_traits<
                    cds::opt::item_counter< cds::atomicity::item_counter >
                >::type
            > myDeque;
            \endcode
    */
    template <typename T, typename Timestamp, typename Traits = tsdeque::traits>
    class TSDeque
    {
    public:
        typedef T         value_type; ///< Type of value to be stored in the deque
        typedef Timestamp timestamp;  ///< Algorithm of acquiring timestamps
        typedef Traits    traits;     ///< Deque traits

        typedef typename traits::item_counter item_counter;   ///< Item counting policy used
        typedef typename traits::random_engine random_engine; ///< Random engine used

    private:
        TSDequeBuffer<value_type, timestamp, random_engine> *buffer_;
        timestamp *timestamping_;
        item_counter item_counter_;

    public:
        TSDeque(uint64_t num_threads, uint64_t delay)
        {
            timestamping_ = new timestamp();
            timestamping_->initialize(delay, num_threads);

            buffer_ = new TSDequeBuffer<value_type, timestamp, random_engine>(num_threads, timestamping_);
        }

        ~TSDeque()
        {
            clear();

            delete timestamping_;
            delete buffer_;
        }

        /// Inserts a new element at the left end of the deque container
        /**
            The function always returns \p true
        */
        bool insert_left(value_type element)
        {
            std::atomic<uint64_t> *item = buffer_->insert_left(element);
            // In the set_timestamp operation first a new timestamp is acquired
            // and then assigned to the item. The operation may not be executed
            // atomically.
            timestamping_->set_timestamp(item);
            ++item_counter_;
            return true;
        }

        /// Inserts a new element at the right end of the deque container
        /**
            The function always returns \p true
        */
        bool insert_right(value_type element)
        {
            std::atomic<uint64_t> *item = buffer_->insert_right(element);
            // In the set_timestamp operation first a new timestamp is acquired
            // and then assigned to the item. The operation may not be executed
            // atomically.
            timestamping_->set_timestamp(item);
            ++item_counter_;
            return true;
        }

        /// Removes the element from the left end of the deque container
        /**
            The function returns \p false if the deque is empty, \p true otherwise.
        */
        bool remove_left(value_type *element)
        {
            // Read the invocation time of this operation, needed for the
            // elimination optimization.
            uint64_t invocation_time[2];
            timestamping_->read_time(invocation_time);
            while (buffer_->try_remove_left(element, invocation_time))
            {
                if (element != NULL)
                {
                    --item_counter_;
                    return true;
                }
            }
            // The deque was empty, return false.
            return false;
        }

        /// Removes the element from the right end of the deque container
        /**
            The function returns \p false if the deque is empty, \p true otherwise.
        */
        bool remove_right(value_type *element)
        {
            // Read the invocation time of this operation, needed for the
            // elimination optimization.
            uint64_t invocation_time[2];
            timestamping_->read_time(invocation_time);
            while (buffer_->try_remove_right(element, invocation_time))
            {
                if (element != NULL)
                {
                    --item_counter_;
                    return true;
                }
            }
            // The deque was empty, return false.
            return false;
        }

        /// Clears the deque (non-atomic)
        /**
            The function erases all items from the deque.

            The function is not atomic. It cleans up the deque and then resets the item counter to zero.
            If there are a thread that performs insertion while \p clear is working the result is undefined in general case:
            <tt> empty() </tt> may return \p true but the deque may contain item(s).
            Therefore, \p clear may be used only for debugging purposes.
        */
        void clear()
        {
            value_type item;
            while (remove_right(&item)) {}
            item_counter_.reset();
        }

        /// Checks if the deque is empty
        /**
            @warning If you use \p atomicity::empty_item_counter in \p traits::item_counter,
            the function always returns \p true.
        */
        bool empty() const
        {
            return size() == 0;
        }

        /// Returns item count in the deque
        /**
            @warning If you use \p atomicity::empty_item_counter in \p traits::item_counter,
            the function always returns 0.
        */
        size_t size() const
        {
            return item_counter_;
        }
    };

}}  // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_TS_DEQUE_H
