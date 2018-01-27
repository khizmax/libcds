#ifndef CDSLIB_CONTAINER_SKIP_LIST_SET_DHP_H
#define CDSLIB_CONTAINER_SKIP_LIST_SET_DHP_H

#include <mutex>

#include <cds/container/details/lazy_skip_list_set_base.h>

namespace cds { namespace container {

    template <
        typename GC,
        typename T,
        typename Traits = lazy_skip_list_set::traits
    >
    class LazySkipListSet
    {
    public:
        typedef GC      gc;
        typedef T       value_type;
        typedef Traits  traits;

        static size_t const c_nMaxHeight = cds::container::lazy_skip_list_set::c_nMaxHeight;
        // static size_t const c_nHazardPtrCount = c_nMaxHeight * 2 + 3;

        typedef typename traits::random_level_generator rand_height;

    protected:
        typedef cds::container::lazy_skip_list_set::node<gc, value_type> node_type;
        typedef typename node_type::key_type key_type;

        typedef cds::details::marked_ptr<node_type, 1> marked_ptr;

        node_type *m_Head;
        node_type *m_Tail;

    public:
        LazySkipListSet() {
            m_Head = node_type::min_key();
            m_Tail = node_type::max_key();

            for (unsigned int layer = 0; layer < c_nMaxHeight; layer++)
                m_Head->next(layer).store(marked_ptr(m_Tail), traits::memory_model::memory_order_relaxed);
        }

        ~LazySkipListSet() {
            destroy();
        }

        bool insert(value_type v) {
            key_type key = std::hash<value_type>{}(v);
            unsigned int topLayer = randomLevel();
            node_type * preds[c_nMaxHeight];
            node_type * succs[c_nMaxHeight];
            //auto **preds = new node_type *[c_nMaxHeight];
            //auto **succs = new node_type *[c_nMaxHeight];

            while (true) {
                int lFound = find(key, preds, succs);

                if (lFound != -1) {
                    node_type *nodeFound = succs[lFound];
                    if (!nodeFound->marked()) {
                        while (!nodeFound->fully_linked()) {
                        }
                        return false;
                    }
                    continue;
                }

                int highestLocked = -1;
                try {
                    node_type *pred, *succ, *prevPred = nullptr;
                    bool valid = true;

                    for (unsigned int layer = 0; valid && (layer <= topLayer); layer++) {
                        pred = preds[layer];
                        succ = succs[layer];

                        if (pred != prevPred) {
                            pred->lock.lock();
                            highestLocked = layer;
                            prevPred = pred;
                        }

                        valid = !pred->marked() && !succ->marked() && pred->next(layer) == succ;
                    }

                    if (!valid)
                        continue;

                    node_type *new_node = node_type::allocate_node(v, topLayer);
                    for (unsigned int layer = 0; layer <= topLayer; layer++) {
                        new_node->next(layer).store(marked_ptr(succs[layer]));
                        preds[layer]->next(layer).store(marked_ptr(new_node));
                    }

                    new_node->set_fully_linked(true);
                    unlock(preds, highestLocked);

                    return true;
                } catch (int e) {
                    unlock(preds, highestLocked);
                }
            }
        }

        bool remove(value_type v) {
            key_type key = std::hash<value_type>{}(v);
            node_type *nodeToDelete = nullptr;
            bool isMarked = false;
            unsigned int topLayer = 0;
            node_type *preds[c_nMaxHeight];
            node_type *succs[c_nMaxHeight];
            /*auto **preds = new node_type *[c_nMaxHeight];
            auto **succs = new node_type *[c_nMaxHeight];*/

            while (true) {
                int lFound = find(key, preds, succs);

                if (isMarked || (lFound != -1 && okToDelete(succs[lFound], lFound))) {
                    if (!isMarked) {
                        nodeToDelete = succs[lFound];
                        topLayer = nodeToDelete->height();
                        nodeToDelete->lock.lock();

                        if (nodeToDelete->marked()) {
                            nodeToDelete->lock.unlock();
                            return false;
                        }

                        nodeToDelete->mark();
                        isMarked = true;
                    }

                    int highestLocked = -1;
                    try {
                        node_type *pred, *succ, *prevPred = nullptr;
                        bool valid = true;

                        for (unsigned int layer = 0; valid && (layer <= topLayer); layer++) {
                            pred = preds[layer];
                            succ = succs[layer];

                            if (pred != prevPred) {
                                pred->lock.lock();
                                highestLocked = layer;
                                prevPred = pred;
                            }

                            valid = !pred->marked() && pred->next(layer) == succ;
                        }

                        if (!valid)
                            continue;

                        for (unsigned int layer = topLayer; layer >= 0 && layer < c_nMaxHeight; layer--)
                            preds[layer]->next(layer).store(nodeToDelete->next(layer), atomics::memory_order_relaxed);

                        nodeToDelete->lock.unlock();
                        nodeToDelete->mark_hard();
                        unlock(preds, highestLocked);
                        return true;
                    } catch (int e) {
                        unlock(preds, highestLocked);
                    }
                } else
                    return false;
            }
        }

        bool contains(value_type v) {
            key_type key = std::hash<value_type>{}(v);
            node_type *preds[c_nMaxHeight];
            node_type *succs[c_nMaxHeight];
            int lFound = find(key, preds, succs);

            if (lFound == -1)
                return false;

            bool linked = succs[lFound]->fully_linked();
            bool marked = succs[lFound]->marked();

            return (linked && !marked);
        }

        bool empty() {
            // FIXME
            node_type *succ = m_Head->next(0).load(traits::memory_model::memory_order_relaxed).ptr();

            while (true) {
                if (m_Head->next(0) == m_Tail)
                    return true;

                if (succ->marked())
                    succ = m_Head->next(0).load(traits::memory_model::memory_order_relaxed).ptr();
                else
                    return false;
            }
        }

    protected:
        void destroy() {
            node_type *p = m_Head; //->next(0).load(atomics::memory_order_relaxed).ptr();
            while (p) {
                node_type *pNext = p->next(0).load(atomics::memory_order_relaxed).ptr();
                node_type::dispose_node(p);
                p = pNext;
            }
        }

        unsigned int randomLevel() {
            rand_height gen;

            return gen();
        }

        bool okToDelete(node_type *candidate, int lFound) {
            return (candidate->fully_linked() && candidate->height() == lFound && !candidate->marked());
        }

        int find(key_type key, node_type **preds, node_type **succs) {
            int lFound = -1;
            node_type *pred = m_Head;

            for (unsigned int layer = c_nMaxHeight - 1; layer >= 0 && layer < c_nMaxHeight; layer--) {
                node_type *curr = pred->next(layer).load(traits::memory_model::memory_order_relaxed).ptr();

                while (key > curr->node_key()) {
                    pred = curr;
                    curr = pred->next(layer).load(traits::memory_model::memory_order_relaxed).ptr();
                }

                if (lFound == -1 && key == curr->node_key())
                    lFound = layer;

                preds[layer] = pred;
                succs[layer] = curr;
            }

            return lFound;
        }

        void unlock(node_type **preds, int highestLocked) {
            for (int layer = 0; layer <= highestLocked; layer++)
                preds[layer]->lock.unlock();
        }

    };

}}  // namespace cds::container

#endif 	// #ifndef CDSLIB_CONTAINER_SKIP_LIST_SET_DHP_H
