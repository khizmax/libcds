#ifndef CDSLIB_CONTAINER_SKIP_LIST_SET_H
#define CDSLIB_CONTAINER_SKIP_LIST_SET_H

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
        typedef cds::gc::nogc   gc;
        typedef T               value_type;
        typedef Traits          traits;

        static size_t const c_nMaxHeight = cds::container::lazy_skip_list_set::c_nMaxHeight;
        static size_t const c_nHazardPtrCount = 0;

        typedef typename traits::random_level_generator rand_height;

    protected:
        typedef cds::container::lazy_skip_list_set::node<gc, value_type> node_type;
        typedef typename node_type::key_type key_type;

        node_type *m_Head;
        node_type *m_Tail;

    public:
        LazySkipListSet() {
            m_Head = node_type::min_key();
            m_Tail = node_type::max_key();

            for (unsigned int layer = 0; layer < c_nMaxHeight; layer++)
                m_Head->next(layer) = m_Tail;
        }

        ~LazySkipListSet() {
            destroy();
        }

        bool insert(value_type v) {
            key_type key = std::hash<value_type>{}(v);
            unsigned int topLayer = randomLevel();
            node_type * preds[c_nMaxHeight];
            node_type * succs[c_nMaxHeight];

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
                        new_node->next(layer) = succs[layer];
                        preds[layer]->next(layer) = new_node;
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
                            preds[layer]->next(layer) = nodeToDelete->next(layer);

                        nodeToDelete->lock.unlock();
                        node_type::dispose_node(nodeToDelete);
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
            node_type *succ = m_Head->next(0);

            while (true) {
                if (m_Head->next(0) == m_Tail)
                    return true;

                if (succ->marked())
                    succ = m_Head->next(0);
                else
                    return false;
            }
        }

    protected:
        void destroy() {
            node_type *p = m_Head;
            while (p) {
                node_type *pNext = p->next(0);
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
                node_type *curr = pred->next(layer);

                while (key > curr->node_key()) {
                    pred = curr;
                    curr = pred->next(layer);
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

#endif 	// #ifndef CDSLIB_CONTAINER_SKIP_LIST_SET_H
