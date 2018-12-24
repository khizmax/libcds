#ifndef CDSLIB_CONTAINER_TS_TIMESTAMP_H
#define CDSLIB_CONTAINER_TS_TIMESTAMP_H

#include <atomic>
#include <cds/compiler/ts_hardwaretimestamp.h>

namespace cds { namespace container {

    using namespace cds::tshardwaretimestamp;

    class HardwareTimestamp
    {
    public:
        inline void initialize(uint64_t delay, uint64_t num_threads)
        {
        }

        inline void init_sentinel(uint64_t *result)
        {
            result[0] = 0;
        }

        inline void init_sentinel_atomic(std::atomic<uint64_t> *result)
        {
            result[0].store(0);
        }

        inline void init_top_atomic(std::atomic<uint64_t> *result)
        {
            result[0].store(UINT64_MAX);
        }

        inline void init_top(uint64_t *result)
        {
            result[0] = UINT64_MAX;
        }

        inline void load_timestamp(uint64_t *result, std::atomic<uint64_t> *source)
        {
            result[0] = source[0].load();
        }

        inline void set_timestamp(std::atomic<uint64_t> *result)
        {
            result[0].store(platform::get_hwptime());
        }

        inline void read_time(uint64_t *result)
        {
            result[0] = platform::get_hwptime();
        }

        inline bool is_later(uint64_t *timestamp1, uint64_t *timestamp2)
        {
            return timestamp2[0] < timestamp1[0];
        }
    };

    class HardwareIntervalTimestamp
    {
    private:
        uint64_t delay_;

    public:
        inline void initialize(uint64_t delay, uint64_t num_threads)
        {
            delay_ = delay;
        }

        inline void init_sentinel(uint64_t *result)
        {
            result[0] = 0;
            result[1] = 0;
        }

        inline void init_sentinel_atomic(std::atomic<uint64_t> *result)
        {
            result[0].store(0);
            result[1].store(0);
        }

        inline void init_top_atomic(std::atomic<uint64_t> *result)
        {
            result[0].store(UINT64_MAX);
            result[1].store(UINT64_MAX);
        }

        inline void init_top(uint64_t *result)
        {
            result[0] = UINT64_MAX;
            result[1] = UINT64_MAX;
        }

        inline void load_timestamp(uint64_t *result, std::atomic<uint64_t> *source)
        {
            result[0] = source[0].load();
            result[1] = source[1].load();
        }

        inline void set_timestamp(std::atomic<uint64_t> *result)
        {
            result[0].store(platform::get_hwptime());
            uint64_t wait = platform::get_hwtime() + delay_;
            while (platform::get_hwtime() < wait)
            {
            }
            result[1].store(platform::get_hwptime());
        }

        inline void read_time(uint64_t *result)
        {
            result[0] = platform::get_hwptime();
            result[1] = result[0];
        }

        inline bool is_later(uint64_t *timestamp1, uint64_t *timestamp2)
        {
            return timestamp2[1] < timestamp1[0];
        }
    };

    class AtomicCounterTimestamp
    {
    private:
        std::atomic<uint64_t> *clock_;

    public:
        inline void initialize(uint64_t delay, uint64_t num_threads)
        {
            clock_ = new std::atomic<uint64_t>();
            clock_->store(1);
        }

        inline void init_sentinel(uint64_t *result)
        {
            result[0] = 0;
        }

        inline void init_sentinel_atomic(std::atomic<uint64_t> *result)
        {
            result[0].store(0);
        }

        inline void init_top_atomic(std::atomic<uint64_t> *result)
        {
            result[0].store(UINT64_MAX);
        }

        inline void init_top(uint64_t *result)
        {
            result[0] = UINT64_MAX;
        }

        inline void load_timestamp(uint64_t *result, std::atomic<uint64_t> *source)
        {
            result[0] = source[0].load();
        }

        inline void set_timestamp(std::atomic<uint64_t> *result)
        {
            result[0].store(clock_->fetch_add(1));
        }

        inline void set_timestamp_local(uint64_t *result)
        {
            result[0] = clock_->fetch_add(1);
        }

        inline void read_time(uint64_t *result)
        {
            result[0] = clock_->load();
        }

        inline bool is_later(uint64_t *timestamp1, uint64_t *timestamp2)
        {
            return timestamp2[0] < timestamp1[0];
        }
    };

}}  // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_TS_TIMESTAMP_H
