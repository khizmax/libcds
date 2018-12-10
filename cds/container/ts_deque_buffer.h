template <typename T, typename TimeStamp>
class TSDequeBuffer
{
  private:
	typedef struct Item
	{
		std::atomic<Item *> left;
		std::atomic<Item *> right;
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
	std::atomic<Item *> **left_;
	std::atomic<Item *> **right_;
	int64_t **next_index_;
	// The pointers for the emptiness check.
	Item ***emptiness_check_left_;
	Item ***emptiness_check_right_;
	TimeStamp *timestamping_;

	// Returns the leftmost not-taken item from the thread-local list
	// indicated by thread_id.
	Item *get_left_item(uint64_t thread_id)
	{
	}

	// Returns the rightmost not-taken item from the thread-local list
	// indicated by thread_id.
	Item *get_right_item(uint64_t thread_id)
	{
	}

  public:
	void initialize(uint64_t num_threads, TimeStamp *timestamping)
	{

		num_threads_ = num_threads;
		timestamping_ = timestamping;

		left_ = new std::atomic<Item *> *[num_threads_];

		right_ = new std::atomic<Item *> *[num_threads_];

		next_index_ = new int64_t *[num_threads_];

		emptiness_check_left_ = new Item **[num_threads_];

		emptiness_check_right_ = new Item **[num_threads_];

		for (uint64_t i = 0; i < num_threads_; i++)
		{

			left_[i] = new std::atomic<Item *>();

			right_[i] = new std::atomic<Item *>();

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

			emptiness_check_left_[i] = new Item *[num_threads_];

			emptiness_check_right_[i] = new Item *[num_threads_];
		}
	}

	inline std::atomic<uint64_t> *insert_left(T element)
	{
	}

	inline std::atomic<uint64_t> *insert_right(T element)
	{
	}

	bool try_remove_left(T *element, uint64_t *invocation_time)
	{
		// Initialize the data needed for the emptiness check.
		uint64_t thread_id = ThreadContext::get().thread_id();
		Item **emptiness_check_left =
			emptiness_check_left_[thread_id];
		Item **emptiness_check_right =
			emptiness_check_right_[thread_id];
		bool empty = true;
		// Initialize the result pointer to NULL, which means that no
		// element has been removed.
		Item *result = NULL;
		// Indicates the index which contains the youngest item.
		uint64_t buffer_index = -1;
		// Memory on the stack frame where timestamps of items can be stored
		// temporarily.
		uint64_t tmp_timestamp[2][2];
		// Index in the tmp_timestamp array which is not used at the moment.
		uint64_t tmp_index = 1;
		timestamping_->init_sentinel(tmp_timestamp[0]);
		uint64_t *timestamp = tmp_timestamp[0];
		// Stores the value of the remove pointer of a thead-local buffer
		// before the buffer is actually accessed.
		Item *old_left = NULL;

		// Read the start time of the iteration. Items which were timestamped
		// after the start time and inserted at the right are not removed.
		uint64_t start_time[2];
		timestamping_->read_time(start_time);
		// We start iterating over the thread-local lists at a random index.
		uint64_t start = hwrand();
		// We iterate over all thead-local buffers
		for (uint64_t i = 0; i < num_threads_; i++)
		{
			uint64_t tmp_buffer_index = (start + i) % num_threads_;
			// We get the remove/insert pointer of the current thread-local buffer.
			Item *tmp_left = left_[tmp_buffer_index]->load();
			// We get the youngest element from that thread-local buffer.
			Item *item = get_left_item(tmp_buffer_index);
			// If we found an element, we compare it to the youngest element
			// we have found until now.
			if (item != NULL)
			{
				empty = false;
				uint64_t *item_timestamp;
				timestamping_->load_timestamp(tmp_timestamp[tmp_index], item->timestamp);
				item_timestamp = tmp_timestamp[tmp_index];

				if (inserted_left(item) && !timestamping_->is_later(invocation_time, item_timestamp))
				{
					uint64_t expected = 0;
					if (item->taken.load() == 0 && item->taken.compare_exchange_weak(expected, 1))
					{
						// Try to adjust the remove pointer. It does not matter if
						// this CAS fails.
						left_[tmp_buffer_index]->compare_exchange_weak(
							tmp_left, (Item *)add_next_aba(item, tmp_left, 0));
						*element = item->data.load();
						return true;
					}
					else
					{
						item = get_left_item(tmp_buffer_index);
						if (item != NULL)
						{
							timestamping_->load_timestamp(tmp_timestamp[tmp_index], item->timestamp);
							item_timestamp = tmp_timestamp[tmp_index];
						}
					}
				}

				if (item != NULL && (result == NULL || is_more_left(item, item_timestamp, result, timestamp)))
				{
					// We found a new leftmost item, so we remember it.
					result = item;
					buffer_index = tmp_buffer_index;
					timestamp = item_timestamp;
					tmp_index ^= 1;
					old_left = tmp_left;

					// Check if we can remove the element immediately.
					if (inserted_left(result) && !timestamping_->is_later(invocation_time, timestamp))
					{
						uint64_t expected = 0;
						if (result->taken.load() == 0)
						{
							if (result->taken.compare_exchange_weak(
									expected, 1))
							{
								// Try to adjust the remove pointer. It does not matter if
								// this CAS fails.
								left_[buffer_index]->compare_exchange_weak(
									old_left, (Item *)add_next_aba(result, old_left, 0));

								*element = result->data.load();
								return true;
							}
						}
					}
				}
			}
			else
			{
				// No element was found, work on the emptiness check.
				if (emptiness_check_left[tmp_buffer_index] != tmp_left)
				{
					empty = false;
					emptiness_check_left[tmp_buffer_index] =
						tmp_left;
				}
				Item *tmp_right = right_[tmp_buffer_index]->load();
				if (emptiness_check_right[tmp_buffer_index] != tmp_right)
				{
					empty = false;
					emptiness_check_right[tmp_buffer_index] =
						tmp_right;
				}
			}
		}

		*element = (T)NULL;
		return !empty;
	}

	bool try_remove_right(T *element, uint64_t *invocation_time)
	{
	}
};
