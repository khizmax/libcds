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
#include <cds/opt/compare.h>
#include <cds/details/binary_functor_wrapper.h>

namespace cds {
	namespace container {

		//@cond
		namespace details {
			template <typename KEY, typename DATA, typename Traits>
			struct make_hopscotch_map
			{
				typedef KEY key_type; ///< key type
				typedef DATA   mapped_type; ///< type of value stored in the map
				typedef std::pair<key_type const, mapped_type> value_type; ///< Pair type

				typedef Traits original_traits;
				typedef typename original_traits::probeset_type probeset_type;
				static bool const store_hash = original_traits::store_hash;
				static unsigned int const store_hash_count = store_hash ? ((unsigned int)std::tuple_size< typename original_traits::hash::hash_tuple_type >::value) : 0;

				struct node_type : public intrusive::hopscotch_hashset::node<probeset_type, store_hash_count>
				{
					value_type  m_val;

					template <typename K>
					node_type(K const& key)
						: m_val(std::make_pair(key_type(key), mapped_type()))
					{}

					template <typename K, typename Q>
					node_type(K const& key, Q const& v)
						: m_val(std::make_pair(key_type(key), mapped_type(v)))
					{}

					template <typename K, typename... Args>
					node_type(K&& key, Args&&... args)
						: m_val(std::forward<K>(key), std::move(mapped_type(std::forward<Args>(args)...)))
					{}
				};

				struct key_accessor {
					key_type const& operator()(node_type const& node) const
					{
						return node.m_val.first;
					}
				};

				struct intrusive_traits : public original_traits
				{
					typedef intrusive::hopscotch_hashset::base_hook<
						cds::intrusive::hopscotch_hashset::probeset_type< probeset_type >
						, cds::intrusive::hopscotch_hashset::store_hash< store_hash_count >
					>  hook;

					typedef cds::intrusive::hopscotch_hashset::traits::disposer   disposer;

					typedef typename std::conditional<
						std::is_same< typename original_traits::equal_to, opt::none >::value
						, opt::none
						, cds::details::predicate_wrapper< node_type, typename original_traits::equal_to, key_accessor >
					>::type equal_to;

					typedef typename std::conditional<
						std::is_same< typename original_traits::compare, opt::none >::value
						, opt::none
						, cds::details::compare_wrapper< node_type, typename original_traits::compare, key_accessor >
					>::type compare;

					typedef typename std::conditional<
						std::is_same< typename original_traits::less, opt::none >::value
						, opt::none
						, cds::details::predicate_wrapper< node_type, typename original_traits::less, key_accessor >
					>::type less;

					typedef opt::details::hash_list_wrapper< typename original_traits::hash, node_type, key_accessor >    hash;
				};

				typedef intrusive::HopscotchHashset< node_type, intrusive_traits > type;
			};
		}   // namespace details
		//@endcond

		template<class KEY, class DATA, typename Traits = hopscotch_hashmap_ns::traits>
		class hopscotch_hashmap : protected details::make_hopscotch_map<KEY, DATA, Traits>::type
		{
		private:
			static const int HOP_RANGE = 32;
			static const int ADD_RANGE = 256;
			static const int MAX_SEGMENTS = 1048576;
			KEY* BUSY;
			DATA* BUSYD;
			struct Bucket {

				static const unsigned int _empty_hop_info = 0;
				KEY* _empty_key = NULL;
				DATA* _empty_data = NULL;

				std::atomic<unsigned int> _hop_info;
				void* _key;
				DATA* _data;
				std::atomic<unsigned int> _lock;
				std::atomic<std::size_t> _timestamp;
				std::mutex lock_mutex;

				Bucket() {
					_hop_info = _empty_hop_info;
					_lock = 0;
					_key = _empty_key;
					_data = _empty_data;
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

			std::size_t calc_hash(std::string key) {
				std::hash<int> hash_fn;
				return hash_fn(std::stoi(key)) % MAX_SEGMENTS;
			}

			std::size_t calc_hash(int key) {
				std::hash<int> hash_fn;
				return hash_fn(key) % MAX_SEGMENTS;
			}

			void resize() {
				// TODO need to find out if we need one
			}

			//@cond
			typedef details::make_hopscotch_map<KEY, DATA, Traits> maker;
			typedef typename maker::type base_class;
			//@endcond
			typedef typename base_class::value_type node_type;
			typedef typename base_class::allocator allocator; ///< allocator type used for internal bucket table allocations
			/// Node allocator type
			typedef typename std::conditional<
				std::is_same< typename Traits::node_allocator, opt::none >::value,
				allocator,
				typename Traits::node_allocator
			>::type node_allocator;
			typedef cds::details::Allocator< node_type, node_allocator > cxx_node_allocator;
			//@cond
			template <typename K>
			static node_type * alloc_node(K const& key)
			{
				return cxx_node_allocator().New(key);
			}
			template <typename K, typename... Args>
			static node_type * alloc_node(K&& key, Args&&... args)
			{
				return cxx_node_allocator().MoveNew(std::forward<K>(key), std::forward<Args>(args)...);
			}

			static void free_node(node_type * pNode)
			{
				cxx_node_allocator().Delete(pNode);
			}
			//@endcond
			typedef typename maker::key_accessor key_accessor;

		public:
			static bool const c_isSorted = false; ///< whether the probe set should be ordered
			typedef cds::atomicity::item_counter item_counter;
			item_counter m_item_counter;
			typedef KEY key_type; ///< key type
			typedef DATA mapped_type; ///< type of value stored in the map
			typedef std::pair<key_type const, mapped_type> value_type; ///< Pair type
			//@cond
			struct node_disposer 
			{
				void operator()(node_type *pNode)
				{
					free_node(pNode);
				}
			};

			typedef std::unique_ptr< node_type, node_disposer > scoped_node_ptr;

			//@endcond
			typedef typename base_class::stat stat; ///< internal statistics type
			typedef typename base_class::mutex_policy mutex_policy; ///< Concurrent access policy, see hopscotch_hashmap_ns::traits::mutex_policy

			hopscotch_hashmap() 
			{
			}

			~hopscotch_hashmap() 
			{
				clear();
			}

			/// Returns const reference to internal statistics
			stat const& statistics() const
			{
				return base_class::statistics();
			}

			/// Returns const reference to mutex policy internal statistics
			typename mutex_policy::statistics_type const& mutex_policy_statistics() const
			{
				return base_class::mutex_policy_statistics();
			}

			bool empty() 
			{
				return base_class::empty();
			}

			size_t size() const 
			{
				return base_class::size();
			}

			/// Checks whether the map contains \p key
			/**
			The function searches the item with key equal to \p key
			and returns \p true if it is found, and \p false otherwise.
			*/
			template <typename K>
			bool contains(K const& key)
			{	
				return base_class::contains(key);
			}

			/// Checks whether the map contains \p key using \p pred predicate for searching
			/**
			The function is similar to <tt>contains( key )</tt> but \p pred is used for key comparing.
			\p Less functor has the interface like \p std::less.
			\p Less must imply the same element order as the comparator used for building the map.
			*/
			template <typename K, typename Predicate>
			bool contains(K const& key, Predicate pred)
			{
				CDS_UNUSED(pred);
				return base_class::contains(key);
			}

			/// Find the key \p key
			/** \anchor cds_nonintrusive_CuckooMap_find_func

			The function searches the item with key equal to \p key and calls the functor \p f for item found.
			The interface of \p Func functor is:
			\code
			struct functor {
			void operator()( value_type& item );
			};
			\endcode
			where \p item is the item found.

			The functor may change \p item.second.

			The function returns \p true if \p key is found, \p false otherwise.
			*/
			template <typename K, typename Func>
			bool find_with(K const& key, Func f)
			{
				return contains(key, f);
			}

			template <typename K, typename Predicate>
			bool find(K const& key, Predicate pred)
			{
				return base_class::find(key, [&pred](node_type& item, K const&) { pred(item.m_val); });
			}

			template <typename K>
			bool find(K const& key)
			{
				return contains(key);
			}

			template <typename K, typename Predicate, typename Func>
			bool find_with(K const& key, Predicate pred, Func f)
			{
				CDS_UNUSED(pred);
				return base_class::find(key, [&f](node_type& item, K const&) { f(item.m_val); });
			}

			/// For key \p key inserts data of type \ref value_type constructed with <tt>std::forward<Args>(args)...</tt>
			/**
			Returns \p true if inserting successful, \p false otherwise.
			*/
			template <typename K, typename... Args>
			bool emplace(K&& key, Args&&... args)
			{
				return insert(std::forward<K>(key), mapped_type(std::forward<Args>(args)...));
			}

			/// Clears the map
			void clear()
			{
				base_class::clear_and_dispose(node_disposer());
			}
			/// Updates the node
			/**
			The operation performs inserting or changing data with lock-free manner.

			If \p key is not found in the map, then \p key is inserted iff \p bAllowInsert is \p true.
			Otherwise, the functor \p func is called with item found.
			The functor \p func signature is:
			\code
			struct my_functor {
			void operator()( bool bNew, value_type& item );
			};
			\endcode
			with arguments:
			- \p bNew - \p true if the item has been inserted, \p false otherwise
			- \p item - an item of the map for \p key

			Returns std::pair<bool, bool> where \p first is \p true if operation is successful,
			i.e. the node has been inserted or updated,
			\p second is \p true if new item has been added or \p false if the item with \p key
			already exists.
			*/
			template <typename K, typename Func>
			std::pair<bool, bool> update(K const& key, Func func, bool bAllowInsert = true)
			{
				scoped_node_ptr pNode(alloc_node(key));
				std::pair<bool, bool> res = base_class::update(*pNode,
					[&func](bool bNew, node_type& item, node_type const&) { func(bNew, item.m_val); },
					bAllowInsert
				);
				if (res.first && res.second)
					pNode.release();
				return res;
			}

			template <typename K, typename V, typename Func>
			std::pair<bool, bool> update(K const& key, V const& val)
			{
				return update(key, val, [](V const&) {});
			}

			/// Updates the node
			/**
			The operation performs inserting or changing data with lock-free manner.

			If \p key is not found in the map, then \p key is inserted iff \p bAllowInsert is \p true.
			Otherwise, the functor \p func is called with item found.
			The functor \p func signature is:
			\code
			struct my_functor {
			void operator()( bool bNew, value_type& item );
			};
			\endcode
			with arguments:
			- \p bNew - \p true if the item has been inserted, \p false otherwise
			- \p item - an item of the map for \p key

			Returns std::pair<bool, bool> where \p first is \p true if operation is successful,
			i.e. the node has been inserted or updated,
			\p second is \p true if new item has been added or \p false if the item with \p key
			already exists.
			*/
			template <typename K, typename V, typename Func>
			std::pair<bool, bool> update(K const& key, V const& val, Func func, bool bAllowInsert = true)
			{
				bool inserted = false;

				if (!contains(key)) {
					if (bAllowInsert) {
						if (insert(key, val)) {
							inserted = true;
						}
						else {
							return std::make_pair(false, false);
						}
					}
					else {
						return std::make_pair(false, false);
					}
				}

				std::size_t hash = calc_hash(key);
				Bucket* start_bucket = segments_arys + hash;
				std::size_t timestamp;
				do {
					timestamp = start_bucket->_timestamp;
					unsigned int hop_info = start_bucket->_hop_info;
					Bucket* check_bucket = start_bucket;
					unsigned int temp;
					for (int i = 0; i < HOP_RANGE; i++) {
						temp = hop_info;
						temp = temp >> i;

						if (temp & 1) {
							if (!check_bucket->_key)
								return std::make_pair(true, inserted);
							if (key == *((K *)(check_bucket->_key))) {
								*(check_bucket->_data) = val;
								func(*(check_bucket->_data));
								return std::make_pair(true, inserted);
							}
						}
						++check_bucket;
					}
				} while (timestamp != start_bucket->_timestamp);

				return std::make_pair(false, inserted);
			}

			template<typename K, typename V>
			V get(K const& key) {
				cds_test::striped_map_fixture::cmp cmp = cds_test::striped_map_fixture::cmp();
				return get(key, [&](K const& one, K const& two) { return cmp(one, two); });
			}

			template<typename K, typename V, typename Pred>
			V get(K const& key, Pred cmp) {
				std::size_t hash = calc_hash(key);
				Bucket* start_bucket = segments_arys + hash;
				std::size_t timestamp;
				V found_value;
				do {
					timestamp = start_bucket->_timestamp;
					unsigned int hop_info = start_bucket->_hop_info;
					Bucket* check_bucket = start_bucket;
					unsigned int temp;
					for (int i = 0; i < HOP_RANGE; i++) {
						temp = hop_info;
						temp = temp >> i;

						if (temp & 1) {
							if (cmp(key, *(check_bucket->_key)) == 0) {
								check_bucket->lock();
								if (cmp(key, *(check_bucket->_key)) == 0) {
									found_value = *(check_bucket->_data);
									check_bucket->unlock();
									return found_value;
								}
								else {
									check_bucket->unlock();
									++check_bucket;
									continue;
								}
							}
						}
						++check_bucket;
					}
				} while (timestamp != start_bucket->_timestamp);

				return NULL;
			}
			
			/// Inserts new node and initialize it by a functor
			/**
			This function inserts new node with key \p key and if inserting is successful then it calls
			\p func functor with signature
			\code
			struct functor {
			void operator()( value_type& item );
			};
			\endcode

			The argument \p item of user-defined functor \p func is the reference
			to the map's item inserted:
			- <tt>item.first</tt> is a const reference to item's key that cannot be changed.
			- <tt>item.second</tt> is a reference to item's value that may be changed.

			The key_type should be constructible from value of type \p K.

			The function allows to split creating of new item into two part:
			- create item from \p key;
			- insert new item into the map;
			- if inserting is successful, initialize the value of item by calling \p func functor

			This can be useful if complete initialization of object of \p value_type is heavyweight and
			it is preferable that the initialization should be completed only if inserting is successful.
			*/
			template <typename K, typename Func>
			bool insert_with(const K& key, Func func)
			{
				scoped_node_ptr pNode(alloc_node(key));
				if (base_class::insert(*pNode, [&func](node_type& item) { func(item.m_val); })) {
					pNode.release();
					return true;
				}
				return false;
			}

			/// Inserts new node with key and default value
			/**
			The function creates a node with \p key and default value, and then inserts the node created into the map.

			Preconditions:
			- The \ref key_type should be constructible from a value of type \p K.
			In trivial case, \p K is equal to \ref key_type.
			- The \ref mapped_type should be default-constructible.

			Returns \p true if inserting successful, \p false otherwise.
			*/
			template <typename K>
			bool insert(K const& key)
			{
				return insert_with(key, [](value_type&) {});
			}

			/// Inserts new node with key and default value
			/**
			The function creates a node with \p key and \p value, and then inserts the node created into the map.

			Preconditions:
			- The \ref key_type should be constructible from a value of type \p K.
			In trivial case, \p K is equal to \ref key_type.

			Returns \p true if inserting successful, \p false otherwise.
			*/
			template <typename K, typename V>
			bool insert(K const& key, V const& val)
			{
				return insert_with(key, [&val](value_type& item) { item.second = val; });
			}

			/// Delete \p key from the map
			/** \anchor cds_nonintrusive_CuckooMap_erase_val

			Return \p true if \p key is found and deleted, \p false otherwise
			*/
			template <typename K>
			bool erase(K const& key)
			{
				node_type * pNode = base_class::erase(key);
				if (pNode) {
					free_node(pNode);
					return true;
				}
				return false;
			}

			/// Deletes the item from the list using \p pred predicate for searching
			/**
			The function is an analog of \ref cds_nonintrusive_CuckooMap_erase_val "erase(Q const&)"
			but \p pred is used for key comparing.
			If cuckoo map is ordered, then \p Predicate should have the interface and semantics like \p std::less.
			If cuckoo map is unordered, then \p Predicate should have the interface and semantics like \p std::equal_to.
			\p Predicate must imply the same element order as the comparator used for building the map.
			*/
			template <typename K, typename Predicate>
			bool erase_with(K const& key, Predicate pred)
			{
				CDS_UNUSED(pred);
				node_type * pNode = base_class::erase(key);
				if (pNode) {
					free_node(pNode);
					return true;
				}
				return false;
			}

			/// Delete \p key from the map
			/** \anchor cds_nonintrusive_CuckooMap_erase_func

			The function searches an item with key \p key, calls \p f functor
			and deletes the item. If \p key is not found, the functor is not called.

			The functor \p Func interface:
			\code
			struct extractor {
			void operator()(value_type& item) { ... }
			};
			\endcode

			Return \p true if key is found and deleted, \p false otherwise

			See also: \ref erase
			*/
			template <typename K, typename Func>
			bool erase(K const& key, Func f)
			{
				node_type * pNode = base_class::erase(key);
				if (pNode) {
					f(pNode->m_val);
					free_node(pNode);
					return true;
				}
				return false;
			}

			/// Deletes the item from the list using \p pred predicate for searching
			/**
			The function is an analog of \ref cds_nonintrusive_CuckooMap_erase_func "erase(Q const&, Func)"
			but \p pred is used for key comparing.
			If cuckoo map is ordered, then \p Predicate should have the interface and semantics like \p std::less.
			If cuckoo map is unordered, then \p Predicate should have the interface and semantics like \p std::equal_to.
			\p Predicate must imply the same element order as the comparator used for building the map.
			*/
			template <typename K, typename Predicate, typename Func>
			bool erase_with(K const& key, Predicate pred, Func f)
			{
				CDS_UNUSED(pred);
				node_type * pNode = base_class::erase(key);
				if (pNode) {
					f(pNode->m_val);
					free_node(pNode);
					return true;
				}
				return false;
			}

			void find_closer_bucket(Bucket** free_bucket, int* free_distance, int &val) {
				Bucket* move_bucket = *free_bucket - (HOP_RANGE - 1);
				for (int free_dist = (HOP_RANGE - 1); free_dist > 0; --free_dist) {
					unsigned int start_hop_info = move_bucket->_hop_info;
					int move_free_distance = -1;
					unsigned int mask = 1;
					for (int i = 0; i < free_dist; ++i, mask <<= 1) {
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
							new_free_bucket->_data = BUSYD;
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