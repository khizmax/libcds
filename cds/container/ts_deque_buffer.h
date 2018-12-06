template<typename T, typename TimeStamp>
class TSDequeBuffer {
  private:

    // Returns the leftmost not-taken item from the thread-local list
    // indicated by thread_id.
    Item* get_left_item(uint64_t thread_id) {

    }

    // Returns the rightmost not-taken item from the thread-local list
    // indicated by thread_id.
    Item* get_right_item(uint64_t thread_id) {

    }

  public:

    inline std::atomic<uint64_t> *insert_left(T element) {

    }

    inline std::atomic<uint64_t> *insert_right(T element) {

    }

    bool try_remove_left(T *element, uint64_t *invocation_time) {

    }

    bool try_remove_right(T *element, uint64_t *invocation_time) {
      
    }
};
