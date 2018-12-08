template<typename T, typename TimeStamp>
class TSDequeBuffer {
  private:

    typedef struct Item {
      std::atomic<Item*> left;
      std::atomic<Item*> right;
      std::atomic<uint64_t> taken;
      std::atomic<T> data;
      std::atomic<uint64_t> timestamp[2];
      // Insertion index, needed for the termination condition in
      // get_left_item. Items inserted at the left get negative
      // indices, items inserted at the right get positive indices.
      std::atomic<int64_t> index;
    } Item;

    // The number of threads.
    uint64_t num_threads_;
    std::atomic<Item*> **left_;
    std::atomic<Item*> **right_;
    int64_t **next_index_;
    // The pointers for the emptiness check.
    Item** *emptiness_check_left_;
    Item** *emptiness_check_right_;
    TimeStamp *timestamping_;

    // Returns the leftmost not-taken item from the thread-local list
    // indicated by thread_id.
    Item* get_left_item(uint64_t thread_id) {

    }

    // Returns the rightmost not-taken item from the thread-local list
    // indicated by thread_id.
    Item* get_right_item(uint64_t thread_id) {

    }

  public:

    void initialize(uint64_t num_threads, TimeStamp *timestamping) {

      num_threads_ = num_threads;
      timestamping_ = timestamping;

      left_ = new std::atomic<Item*>*[num_threads_];

      right_ = new std::atomic<Item*>*[num_threads_];

      next_index_ = new int64_t*[num_threads_];

      emptiness_check_left_ = new Item**[num_threads_];

      emptiness_check_right_ = new Item**[num_threads_];

      for (uint64_t i = 0; i < num_threads_; i++) {

        left_[i] = new std::atomic<Item*>();

        right_[i] = new std::atomic<Item*>();

        next_index_[i] = new int64_t();

        // Add a sentinal node.
        Item *new_item = new Item();
        timestamping_->init_sentinel_atomic(new_item->timestamp);
        new_item->data.store(0);
        new_item->taken.store(1);
        new_item->left.store(new_item);
        new_item->right.store(new_item);
        new_item->index.store(0);
        left_[i]->store(new_item);
        right_[i]->store(new_item);
        *next_index_[i] = 1;

        emptiness_check_left_[i] = new Item*[num_threads_];

        emptiness_check_right_[i] = new Item*[num_threads_];
      }
    }

    inline std::atomic<uint64_t> *insert_left(T element) {

    }

    inline std::atomic<uint64_t> *insert_right(T element) {

    }

    bool try_remove_left(T *element, uint64_t *invocation_time) {

    }

    bool try_remove_right(T *element, uint64_t *invocation_time) {

    }
};
