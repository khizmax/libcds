#include <atomic>
#include "ts_deque_buffer.h"
#include "threadcontext.h"

template <typename T, typename Timestamp>
class TSDeque
{
private:
    TSDequeBuffer<T, Timestamp> *buffer_;
    Timestamp *timestamping_;

public:
    TSDeque(uint64_t num_threads, uint64_t delay)
    {
        ThreadContext::prepare(num_threads);
        ThreadContext::assign_context();

        timestamping_ = new Timestamp();
        timestamping_->initialize(delay, num_threads);

        buffer_ = new TSDequeBuffer<T, Timestamp>();
        buffer_->initialize(num_threads, timestamping_);
    }

    bool insert_left(T element)
    {
        std::atomic<uint64_t> *item = buffer_->insert_left(element);
        // In the set_timestamp operation first a new timestamp is acquired
        // and then assigned to the item. The operation may not be executed
        // atomically.
        timestamping_->set_timestamp(item);
        return true;
    }

    bool insert_right(T element)
    {
        std::atomic<uint64_t> *item = buffer_->insert_right(element);
        // In the set_timestamp operation first a new timestamp is acquired
        // and then assigned to the item. The operation may not be executed
        // atomically.
        timestamping_->set_timestamp(item);
        return true;
    }

    bool remove_left(T *element)
    {
        // Read the invocation time of this operation, needed for the
        // elimination optimization.
        uint64_t invocation_time[2];
        timestamping_->read_time(invocation_time);
        while (buffer_->try_remove_left(element, invocation_time))
        {

            if (*element != (T)NULL)
            {
                return true;
            }
        }
        // The deque was empty, return false.
        return false;
    }

    bool remove_right(T *element)
    {
        // Read the invocation time of this operation, needed for the
        // elimination optimization.
        uint64_t invocation_time[2];
        timestamping_->read_time(invocation_time);
        while (buffer_->try_remove_right(element, invocation_time))
        {

            if (*element != (T)NULL)
            {
                return true;
            }
        }
        // The deque was empty, return false.
        return false;
    }
};
