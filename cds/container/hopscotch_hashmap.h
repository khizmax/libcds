/*
* concurrent_hopscotch_hash_set.h
*
* Created on: 23.12.2017
* Original idea by Maurice Herlihy, Nir Shavit, Moran Tzafrir: http://people.csail.mit.edu/shanir/publications/disc2008_submission_98.pdf
* Based on implementation by Sathya Hariesh Prakash, Royston Monteiro, Mohan Pandiyan: https://github.com/harieshsathya/Hopscotch-Hashing
* 
* This version modifiers: Leonid Skorospelov, Andrey Chulanov, Roman Stetskevich
*/

#ifndef HOPSCOTCHHASHMAP_H_
#define HOPSCOTCHHASHMAP_H_
#include <iostream>
#include <atomic>
#include <mutex>
#include <cstdlib>

namespace cds {
	namespace container {
		template<class KEY, class DATA>
		class hopscotch_hashmap {
		private:
			static const int HOP_RANGE = 32;
			static const int ADD_RANGE = 256;
			static const int MAX_SEGMENTS = 1048576;
			static const int MAX_TRIES = 2;
			KEY* BUSY;
			struct Bucket {

				std::atomic<unsigned int> _hop_info;
				std::atomic<KEY *> volatile _key;
				std::atomic<DATA *> volatile _data;
				std::atomic<unsigned int> _lock;
				std::atomic<unsigned int> _timestamp;
				std::mutex lock_mutex;

				Bucket() {
					_hop_info = 0;
					_lock = 0;
					_key = NULL;
					_data = NULL;
					_timestamp = 0;
				}

				void lock() {
					lock_mutex.lock();
					while (1) {
						if (_lock == 0) {
							_lock = 1;
							lock_mutex.unlock();
							break;
						}
					}
				}

				void unlock() {
					lock_mutex.lock();
					_lock = 0;
					lock_mutex.unlock();
				}

			};

			Bucket* segments_arys;

			int calc_hash(KEY* key) {
				std::hash<KEY*> hash_fn;
				return hash_fn(key) % MAX_SEGMENTS;
			}

			void resize() {
				// TODO need to find out if we need one
			}

		public:
			hopscotch_hashmap() {
				segments_arys = new Bucket[MAX_SEGMENTS + 256];
				BUSY = (KEY*)std::malloc(sizeof(KEY));
			}

			~hopscotch_hashmap() {
				std::free(BUSY);
				std::free(segments_arys);
			}

			bool contains(KEY* key) {
				return get(key) != NULL;
			}

			DATA* get(KEY* key) {
				unsigned int hash = calc_hash(key);
				Bucket* start_bucket = segments_arys + hash;
				unsigned int try_counter = 0;
				unsigned int timestamp;
				do {
					timestamp = start_bucket->_timestamp;
					unsigned int hop_info = start_bucket->_hop_info;
					Bucket* check_bucket = start_bucket;
					unsigned int temp;
					for (int i = 0; i < HOP_RANGE; i++) {
						temp = hop_info;
						temp = temp >> i;

						if (temp & 1) {
							if (*key == *(check_bucket->_key)) {
								return check_bucket->_data;
							}
						}
						++check_bucket;
					}
					++try_counter;
				} while (timestamp != start_bucket->_timestamp && try_counter < MAX_TRIES);

				if (timestamp != start_bucket->_timestamp) {
					Bucket* check_bucket = start_bucket;
					for (int i = 0; i<HOP_RANGE; i++) {
						if (*key == *(check_bucket->_key)) {
							return check_bucket->_data;
						}
						++check_bucket;
					}
				}
				return NULL;
			}

			bool add(KEY *key, DATA *data) {
				int val = 1;
				unsigned int hash = calc_hash(key);
				Bucket* start_bucket = segments_arys + hash;
				start_bucket->lock();
				if (contains(key)) {
					start_bucket->unlock();
					return false;
				}

				Bucket* free_bucket = start_bucket;
				int free_distance = 0;
				for (; free_distance<ADD_RANGE; ++free_distance) {
					std::atomic<KEY*> _atomic = free_bucket->_key;
					KEY* _null_key = NULL;
					if (NULL == free_bucket->_key && _atomic.compare_exchange_strong(_null_key, BUSY)) {
						break;
					}
					++free_bucket;
				}

				if (free_distance < ADD_RANGE) {
					do {
						if (free_distance < HOP_RANGE) {
							start_bucket->_hop_info |= (1 << free_distance);
							free_bucket->_data = data;
							free_bucket->_key = key;
							start_bucket->unlock();
							return true;
						}
						find_closer_bucket(&free_bucket, &free_distance, val);
					} while (0 != val);
				}
				start_bucket->unlock();

				this->resize();

				return false;
			}

			DATA* remove(KEY *key) {
				unsigned int hash = calc_hash(key);
				Bucket* start_bucket = segments_arys + hash;
				start_bucket->lock();

				unsigned int hop_info = start_bucket->_hop_info;
				unsigned int mask = 1;
				for (int i = 0; i<HOP_RANGE; ++i, mask <<= 1) {
					if (mask & hop_info) {
						Bucket* check_bucket = start_bucket + i;
						if (*key == *(check_bucket->_key)) {
							DATA* rc = check_bucket->_data;
							check_bucket->_key = NULL;
							check_bucket->_data = NULL;
							start_bucket->_hop_info &= ~(1 << i);
							start_bucket->unlock();
							return rc;
						}
					}
				}
				start_bucket->unlock();
				return NULL;
			}

			void find_closer_bucket(Bucket** free_bucket, int* free_distance, int &val) {
				Bucket* move_bucket = *free_bucket - (HOP_RANGE - 1);
				for (int free_dist = (HOP_RANGE - 1); free_dist>0; --free_dist) {
					unsigned int start_hop_info = move_bucket->_hop_info;
					int move_free_distance = -1;
					unsigned int mask = 1;
					for (int i = 0; i<free_dist; ++i, mask <<= 1) {
						if (mask & start_hop_info) {
							move_free_distance = i;
							break;
						}
					}
					if (-1 != move_free_distance) {
						move_bucket->lock();
						if (start_hop_info == move_bucket->_hop_info) {
							Bucket* new_free_bucket = move_bucket + move_free_distance;
							move_bucket->_hop_info |= (1 << free_dist);
							(*free_bucket)->_data = new_free_bucket->_data;
							(*free_bucket)->_key = new_free_bucket->_key;
							++(move_bucket->_timestamp);
							new_free_bucket->_key = BUSY;
							new_free_bucket->_data = BUSY;
							move_bucket->_hop_info &= ~(1 << move_free_distance);
							*free_bucket = new_free_bucket;
							*free_distance -= free_dist;
							move_bucket->unlock();
							return;
						}
						move_bucket->unlock();
					}
					++move_bucket;
				}
				(*free_bucket)->_key = NULL;
				val = 0;
				*free_distance = 0;
			}

		};
	}
}

#endif /* HOPSCOTCHHASHMAP_H_ */
