//
// Created by denis on 27.01.18.
//

#ifndef CDS_PURCEL_HASHMAP_H
#define CDS_PURCEL_HASHMAP_H


namespace cds { namespace container {

        template <
                class GC
                ,typename Key
                ,typename T
#ifdef CDS_DOXYGEN_INVOKED
                ,class Traits = feldman_hashmap::traits
#else
                ,class Traits
#endif
        >
        class PurcelHashMap
#ifdef CDS_DOXYGEN_INVOKED
            : protected cds::intrusive::FeldmanHashSet< GC, std::pair<Key const, T>, Traits >
#else
                : protected cds::container::details::make_feldman_hashmap< GC, Key, T, Traits >::type
#endif
        {
            //@cond
            typedef cds::container::details::make_feldman_hashmap< GC, Key, T, Traits > maker;
            typedef typename maker::type base_class;
            //@endcond
        public:
            typedef GC      gc;          ///< Garbage collector
            typedef Key     key_type;    ///< Key type
            typedef T       mapped_type; ///< Mapped type
            typedef std::pair< key_type const, mapped_type> value_type;   ///< Key-value pair to be stored in the map
            typedef Traits  traits;      ///< Map traits
#ifdef CDS_DOXYGEN_INVOKED
            typedef typename traits::hash hasher; ///< Hash functor, see \p feldman_hashmap::traits::hash
#else
            typedef typename maker::hasher hasher;
#endif

            typedef typename maker::hash_type hash_type; ///< Hash type deduced from \p hasher return type
            typedef typename base_class::hash_comparator hash_comparator; ///< hash compare functor based on \p Traits::compare and \p Traits::less

            typedef typename traits::item_counter   item_counter;   ///< Item counter type
            typedef typename traits::allocator      allocator;      ///< Element allocator
            typedef typename traits::node_allocator node_allocator; ///< Array node allocator
            typedef typename traits::memory_model   memory_model;   ///< Memory model
            typedef typename traits::back_off       back_off;       ///< Backoff strategy
            typedef typename traits::stat           stat;           ///< Internal statistics type

            /// Count of hazard pointers required
            static constexpr size_t const c_nHazardPtrCount = base_class::c_nHazardPtrCount;

            /// The size of \p hash_type in bytes, see \p feldman_hashmap::traits::hash_size for explanation
            static constexpr size_t const c_hash_size = base_class::c_hash_size;

            /// Level statistics
            typedef feldman_hashmap::level_statistics level_statistics;

        protected:
            //@cond
            typedef typename maker::node_type node_type;
            typedef typename maker::cxx_node_allocator cxx_node_allocator;
            typedef std::unique_ptr< node_type, typename maker::node_disposer > scoped_node_ptr;

            template <bool IsConst>


        public:
#ifdef CDS_DOXYGEN_INVOKED
            /// Guarded pointer
        typedef typename gc::template guarded_ptr< value_type > guarded_ptr;
#else
            typedef typename gc::template guarded_ptr< node_type,
                    value_type,
                    cds::container::details::guarded_ptr_cast_set<node_type,
                            value_type> > guarded_ptr;
#endif

        protected:
            //@cond
            hasher  m_Hasher;
            //@endcond

        public:
            PurcelHashMap( size_t head_bits = 8, size_t array_bits = 4 )
                    : base_class( head_bits, array_bits )
            {}

            ~PurcelHashMap()
            {}

            template <typename K>
            bool insert( K&& key )
            {
                //todo implement this
            }

            template <typename K, typename V>
            bool insert( K&& key, V&& val )
            {
                //todo implement this
            }


            template <typename K>
            bool erase( K const& key )
            {
                //todo implement this
            }


            template <typename K>
            bool contains( K const& key )
            {
                //todo implement this
            }

            template <typename K, typename Func>
            bool find( K const& key, Func f )
            {
                //todo implement this
            }
    }
}

#endif //CDS_PURCEL_HASHMAP_H
