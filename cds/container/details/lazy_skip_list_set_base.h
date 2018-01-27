#ifndef CDSLIB_LAZY_SKIP_LIST_BASE_H
#define CDSLIB_LAZY_SKIP_LIST_BASE_H

#include <cds/intrusive/details/skip_list_base.h>

namespace cds { namespace container {

    namespace lazy_skip_list_set {

        static size_t const c_nMaxHeight = 32;

        typedef cds::intrusive::skip_list::traits traits;

        template <
            typename GC,
            typename T,
            typename Lock = std::recursive_mutex
        >
        class node
        {
        public:
            typedef GC  gc;
            typedef T   value_type;
            typedef Lock lock_type;

            typedef std::size_t key_type;

            typedef node* node_ptr;
            typedef std::numeric_limits<key_type> limits;
            typedef cds::details::Allocator<node> node_allocator;

            typedef cds::details::marked_ptr<node, 1> marked_ptr;
            typedef typename gc::template atomic_marked_ptr<marked_ptr> atomic_marked_ptr;

            typedef cds::details::Allocator<atomic_marked_ptr> tower_allocator;

        protected:
            value_type value;
            key_type key;
            unsigned int m_nHeight;
            atomic_marked_ptr * m_arrNext;
            atomics::atomic<bool> _marked;
            atomics::atomic<bool> fullyLinked;

            key_type hash() {
                return std::hash<value_type>{}(value);
            }

        public:
            lock_type lock;

            node() : _marked(false), fullyLinked(false) {

            }

            ~node(){
                dispose_tower(this);
            }

            bool marked() {
                return _marked;
            }

            bool mark() {
                _marked = true;
            }

            bool mark_hard() {
                marked_ptr next_marked(next(0).load(atomics::memory_order_relaxed).ptr());
                next(0).compare_exchange_strong(next_marked, next_marked | 1, atomics::memory_order_release, atomics::memory_order_acquire);
            }

            bool fully_linked() {
                return fullyLinked.load(atomics::memory_order_relaxed);
            }

            void set_fully_linked(bool value) {
                fullyLinked.store(value, atomics::memory_order_relaxed);
            }

            key_type node_key() {
                return key;
            }

            atomic_marked_ptr& next(unsigned int nLevel) {
                return m_arrNext[nLevel];
            }

            unsigned int height() {
                return m_nHeight;
            }

            void allocate_tower(int nHeight) {
                tower_allocator ta;
                m_arrNext = ta.NewArray(nHeight, nullptr);
            }

            static void dispose_node(node * pNode) {
                node_allocator allocator;
                allocator.Delete(pNode);
            }

            static void dispose_tower(node * pNode) {
                unsigned int topLayer = pNode->height();
                tower_allocator ta;
                if (topLayer > 0)
                    ta.Delete(pNode->release_tower(), topLayer + 1);
            }

            atomic_marked_ptr * release_tower() {
                atomic_marked_ptr * pTower = m_arrNext;
                m_arrNext = nullptr;
                m_nHeight = 0;

                return pTower;
            }

            static node_ptr allocate_node(key_type key) {
                node_allocator alloc;
                node_ptr new_node = alloc.New();
                new_node->key = key;
                new_node->m_nHeight = cds::container::lazy_skip_list_set::c_nMaxHeight;
                new_node->allocate_tower(new_node->m_nHeight);
                new_node->fullyLinked = false;

                return new_node;
            }

            static node_ptr allocate_node(value_type v, unsigned int topLayer) {
                node_allocator alloc;
                node_ptr new_node = alloc.New();

                new_node->value = v;
                new_node->key = new_node->hash();
                new_node->m_nHeight = topLayer;
                new_node->fullyLinked = false;

                new_node->allocate_tower(topLayer + 1);

                return new_node;
            }

            static node * min_key() {
                node_ptr new_node = allocate_node(limits::min());

                return new_node;
            }

            static node * max_key() {
                node_ptr new_node = allocate_node(limits::max() / 1000);

                return new_node;
            }

        };
    }   // namespace lazy_skip_list_set

}}

#endif //CDSLIB_LAZY_SKIP_LIST_BASE_H
