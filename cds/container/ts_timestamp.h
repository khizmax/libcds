class HardwareTimestamp {
  public:
    inline void initialize(uint64_t delay, uint64_t num_threads) {
    }

    inline void init_sentinel(uint64_t *result) {
    }

    inline void init_top(uint64_t *result) {
    }

    inline void load_timestamp(uint64_t *result, std::atomic<uint64_t> *source) {
    }

    inline void set_timestamp(std::atomic<uint64_t> *result) {
    }

    inline void read_time(uint64_t *result) {
    }

    inline bool is_later(uint64_t *timestamp1, uint64_t *timestamp2) {
    }
};
