inline uint64_t get_hwptime(void)
{
    uint64_t aux;
    uint64_t rax,rdx;
    asm volatile ( "rdtscp\n" : "=a" (rax), "=d" (rdx), "=c" (aux) : : );
    return (rdx << 32) + rax;
}

class HardwareTimestamp {
  public:
    inline void initialize(uint64_t delay, uint64_t num_threads) {
    }

    inline void init_sentinel(uint64_t *result) {
      result[0] = 0;
    }

    inline void init_sentinel_atomic(std::atomic<uint64_t> *result) {
      result[0].store(0);
    }

    inline void init_top_atomic(std::atomic<uint64_t> *result) {
      result[0].store(UINT64_MAX);
    }

    inline void init_top(uint64_t *result) {
      result[0] = UINT64_MAX;
    }

    inline void load_timestamp(uint64_t *result, std::atomic<uint64_t> *source) {
      result[0] = source[0].load();
    }

    inline void set_timestamp(std::atomic<uint64_t> *result) {
      result[0].store(get_hwptime());
    }

    inline void read_time(uint64_t *result) {
      result[0] = get_hwptime();
    }

    inline bool is_later(uint64_t *timestamp1, uint64_t *timestamp2) {
      return timestamp2[0] < timestamp1[0];
    }
};