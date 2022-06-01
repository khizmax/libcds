// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_HAMT_H
#define CDSLIB_CONTAINER_HAMT_H

#include <cstdint>
#include <string>
#include <vector>
#include <cds/container/details/base.h>
#include <bitset>
#include <set>
#include "atomic"


#define HASH_PIECE_LEN 5
#define MAX_LEVEL_COUNT (int (64 / 5))

using namespace std;

namespace cds {
    namespace container {
        namespace hamt {

            struct traits {
                /// Node allocator
                typedef CDS_DEFAULT_ALLOCATOR allocator;

                /// Back-off strategy
                typedef cds::backoff::empty back_off;

                /// Item counting feature; by default, disabled. Use \p cds::atomicity::item_counter to enable item counting
                typedef atomicity::empty_item_counter item_counter;

                /// Internal statistics (by default, disabled)
                /**
                    Possible option value are: \p optimistic_queue::stat, \p optimistic_queue::empty_stat (the default),
                    user-provided class that supports \p %optimistic_queue::stat interface.
                */

                /// C++ memory ordering model
                /**
                    Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                    or \p opt::v::sequential_consistent (sequentially consisnent memory model).
                */
                typedef opt::v::relaxed_ordering memory_model;

                /// Padding for internal critical atomic data. Default is \p opt::cache_line_padding
                enum {
                    padding = opt::cache_line_padding
                };
            };

            struct LookupResult {
                enum LookupResultStatus {
                    NotFound,
                    Found,
                    Failed
                };

                int value;
                LookupResultStatus status;

                bool operator==(LookupResult b) const {
                    if ((this->status == NotFound) && (b.status == NotFound)) return true;
                    if ((this->status == Failed) && (b.status == Failed)) return true;
                    if ((this->status == Found) && (b.status == Found)) return this->value == b.value;
                    return false;
                }

                bool operator!=(LookupResult b) const {
                    return !(*this == b);
                }
            };

            LookupResult createSuccessfulLookupResult(int value) {
                return {value, LookupResult::Found};
            }

            static const LookupResult LOOKUP_RESTART{0, LookupResult::Failed};

            struct RemoveResult {
                enum Status {
                    Removed,
                    Failed,
                    NotFound,
                };

                int value;
                Status status;

                bool operator==(RemoveResult rightOperand) const {
                    if ((status == NotFound) && (rightOperand.status == NotFound)) return true;
                    if ((status == Failed) && (rightOperand.status == Failed)) return true;
                    if ((status == Removed) && (rightOperand.status == Removed)) return value == rightOperand.value;
                    return false;
                }

                bool operator!=(RemoveResult rightOperand) const {
                    return !(*this == rightOperand);
                }
            };

            const LookupResult LOOKUP_NOT_FOUND{0, LookupResult::NotFound};
            const RemoveResult REMOVE_NOT_FOUND{0, RemoveResult::NotFound};
            const RemoveResult REMOVE_RESTART{0, RemoveResult::Failed};

            RemoveResult createSuccessfulRemoveResult(int value) {
                return {value, RemoveResult::Removed};
            }

        } // namespace hamt

        namespace details {
            template<class GC, typename K, typename V, typename Traits>
            struct make_hamt {
                typedef GC gc;
                typedef K key_type;
                typedef V value_type;
                typedef Traits traits;  ///< Hamt traits

                struct node_type {

                };

                typedef typename std::allocator_traits<
                        typename traits::allocator
                >::template rebind_alloc<node_type> allocator_type;
                typedef cds::details::Allocator<node_type, allocator_type> cxx_allocator;

                struct node_deallocator {
                    void operator()(node_type *pNode) {
                        cxx_allocator().Delete(pNode);
                    }
                };


            };


        }

        template<class GC, typename K, typename V, typename Traits = hamt::traits>
        class Hamt {
            typedef GC gc;
            typedef K key_type;
            typedef V value_type;
            typedef Traits traits;  ///< Hamt traits

            typedef details::make_hamt<GC, K, V, Traits> maker;
            typedef typename maker::allocator_type allocator_type; ///< Allocator type used for allocate/deallocate the nodes
//            typedef typename maker::type base_class;

        protected:
            typedef typename maker::node_type           node_type;   ///< queue node type (derived from intrusive::optimistic_queue::node)
            typedef typename maker::cxx_allocator       cxx_allocator;
            typedef typename maker::node_deallocator    node_deallocator; // deallocate node
//            typedef typename base_class::node_traits    node_traits;


        protected:
            ///@cond
            static node_type *alloc_node() {
                return cxx_allocator().New();
            }

            static node_type *alloc_node(const value_type &val) {
                return cxx_allocator().New(val);
            }

            static void free_node(node_type *p) {
                node_deallocator()(p);
            }

            struct node_disposer {
                void operator()(node_type *pNode) {
                    free_node(pNode);
                }
            };

            typedef std::unique_ptr<node_type, node_disposer> scoped_node_ptr;
            //@endcond


            struct Bitmap {
                uint32_t data;

                bool isSet(uint8_t pos) const {
                    return ((data >> pos) & 1) == 1;
                }

                void set(uint8_t pos) {
                    data = data | (1 << pos);
                }

                void unset(uint8_t pos) {
                    data = data & (~(1 << pos));
                }
            };

            uint8_t extractHashPartByLevel(uint64_t hash, uint8_t level) {
                return (hash >> (level * HASH_PIECE_LEN)) & ((1 << HASH_PIECE_LEN) - 1);
            }

            enum NodeType {
                CNODE,
                SNODE,
                INODE
            };

            class Node {
            public:
                NodeType type;
            protected:

                Node(NodeType type) {
                    this->type = type;
                }
            };


            class SNode : public Node {
            public:
                static uint64_t generateSimpleHash(string key) {
                    uint64_t hash = 0;
                    for (size_t i = 0; i < key.size(); i++) {
                        hash += key[i] * i;
                    }
                    return hash;
                }

                static uint64_t generateSimpleHash(int key) {
                    return key;
                }

                SNode(key_type k, value_type v) : Node(SNODE) {
                    this->pair.insert({k, v});
                    this->hash = generateSimpleHash(k);
                }

                uint64_t getHash() {
                    return hash;
                }

                bool contains(key_type k) {
                    for (auto &p: this->pair) {
                        if (p.key == k) {
                            return true;
                        }
                    }
                    return false;
                }

                bool contains(SNode *node) {
                    for (auto &p: node->pair) {
                        if (this->contains(p.key)) {
                            return true;
                        }
                    }
                    return false;
                }

                int getValue(key_type k) {
                    for (auto &p: this->pair) {
                        if (p.key == k) {
                            return p.value;
                        }
                    }
                    // TODO ???
                    return 0;
                }

                struct Pair {
                    key_type key;
                    value_type value;

                    bool operator<(const Pair &p) const {
                        return this->key < p.key;
                    }
                };

            public:
                uint64_t hash{};
                set <Pair> pair;
            };

            class CNode : public Node {
            public:
                bool isTomb;

                CNode() : Node(CNODE) {
                    isTomb = false;
                }

                Node *getSubNode(uint8_t path) const {
                    if (!bmp.isSet(path)) return nullptr;
                    int index = getArrayIndexByBmp(path);
                    return array[index];
                }

                uint8_t getChildCount() const {
                    return __builtin_popcount(bmp.data);
                }

                Node *getFirstChild() const {
                    return array.front();
                }

                void insertChild(Node *const newChild, uint8_t path) {
                    bmp.set(path);
                    array.insert(array.begin() + getArrayIndexByBmp(path), newChild);
                }

                void replaceChild(Node *const newChild, uint8_t path) {
                    array[getArrayIndexByBmp(path)] = newChild;
                }

                void deleteChild(uint8_t path) {
                    array.erase(array.begin() + getArrayIndexByBmp(path));
                    bmp.unset(path);
                }

                uint8_t getArrayIndexByBmp(uint8_t pos) const {
                    return __builtin_popcount(
                            ((1 << pos) - 1) & bmp.data
                    );
                }

            private:
                Bitmap bmp{};
                vector<Node *> array;
            };


            class INode : public Node {
            public:
                INode() : Node(INODE) {}

                INode(CNode *main) : INode() {
                    this->main.store(main, std::memory_order_seq_cst);
                }

                atomic<CNode *> main;
            };

            SNode *leftMerge(SNode *node1, SNode *node2) {
                auto *merged = new SNode(*node1);
                for (auto &p: node2->pair) {
                    if (!merged->contains(p.key)) {
                        merged->pair.insert(p);
                    }
                }
                return merged;
            }

            CNode *getCopy(CNode *node) {
                return new CNode(*node);
            }

            void transformToContractedParent(CNode *updated, CNode *m, uint8_t path) {
                updated->replaceChild(m->getFirstChild(), path);
            }

            void transformToWithReplacedPair(CNode *updated, SNode *subNode, SNode *newNode, uint8_t path) {
                updated->replaceChild(leftMerge(newNode, subNode), path);
            }

            void transformToWithInsertedChild(CNode *updated, Node *child, uint8_t path) {
                updated->insertChild(child, path);
            }

            void transformToWithMergedChild(CNode *updated, SNode *subNode, SNode *newNode, uint8_t path) {
                updated->replaceChild(leftMerge(newNode, subNode), path);
            }

            void
            transformToWithDownChild(CNode *updated, SNode *newChild, SNode *oldChild, uint8_t level, uint8_t path) {

                if (newChild->getHash() == oldChild->getHash()) {
                    newChild = leftMerge(newChild, oldChild);
                    updated->replaceChild(newChild, path);
                } else {

                    auto *cur_c = new CNode();
                    auto *i = new INode(cur_c);

                    int j = level + 1;
                    uint8_t newChildHashPath = extractHashPartByLevel(newChild->getHash(), j);
                    uint8_t oldChildHashPath = extractHashPartByLevel(oldChild->getHash(), j);

                    while (newChildHashPath == oldChildHashPath) {
                        auto *c = new CNode();
                        cur_c->insertChild(new INode(c), oldChildHashPath);
                        cur_c = c;
                        j++;
                        newChildHashPath = extractHashPartByLevel(newChild->getHash(), j);
                        oldChildHashPath = extractHashPartByLevel(oldChild->getHash(), j);


                    }
                    cur_c->insertChild(newChild, newChildHashPath);
                    cur_c->insertChild(oldChild, oldChildHashPath);

                    updated->replaceChild(i, path);
                }

            }

            void transformToWithDeletedKey(CNode *updated, SNode *subNode, key_type key, uint8_t path) {
                auto *newSubNode = new SNode(*subNode);
                if (newSubNode->pair.size() > 1) {
                    newSubNode->pair.erase({key, subNode->getValue(key)});
                    updated->replaceChild(newSubNode, path);
                } else {
                    updated->deleteChild(path);
                }
            }

            bool isTombed(const CNode *const c, const INode *const root, const INode *const parent) {
                return root != parent &&
                       c->getChildCount() == 1 &&
                       c->getFirstChild()->type == SNODE;
            }

            bool
            contractParent(INode *parent, INode *i, CNode *pm, CNode *m, uint8_t level,
                           uint64_t hash) {
                if (!m->isTomb) return false;

                if (pm->getSubNode(extractHashPartByLevel(hash, level - 1)) != i) {
                    return true;
                }

                CNode *updated = getCopy(pm);
                transformToContractedParent(updated, m, extractHashPartByLevel(hash, level - 1));
                parent->main.compare_exchange_strong(pm, updated);
                return true;
            }

            Node *getRoot() {
                return this->root;
            }

            hamt::RemoveResult remove(key_type key) {
                while (true) {
                    if (root->main.load() == nullptr) {
                        return hamt::REMOVE_NOT_FOUND;
                    }
                    hamt::RemoveResult res = remove(root, nullptr, key, SNode::generateSimpleHash(key), 0);
                    if (res != hamt::REMOVE_RESTART) {
                        return res;
                    }
                }
            }


        public:


            bool insert(key_type key, value_type value) {
                while (true) {
                    CNode *old = root->main.load();
                    if (old == nullptr) {
                        // root -> c -> s
                        auto *c = new CNode();
                        auto *s = new SNode(key, value);
                        c->insertChild(s, extractHashPartByLevel(s->getHash(), 0));
                        if (root->main.compare_exchange_strong(old, c)) {
                            return true;
                        }
                    } else {
                        if (insert(root, nullptr, new SNode(key, value), 0)) {
                            return true;
                        }
                    }
                }
            }

            Hamt() {
                root = new INode(nullptr);
            }

            hamt::LookupResult lookup(key_type key) {
                while (true) {
                    if (root->main.load() == nullptr) {
                        return hamt::LOOKUP_NOT_FOUND;
                    } else {
                        hamt::LookupResult res = lookup(root, nullptr, key, SNode::generateSimpleHash(key), 0);
                        if (res != hamt::LOOKUP_RESTART) {
                            return res;
                        }
                    }
                }
            }

        private:
            INode *root;

            hamt::LookupResult
            lookup(INode *currentNode, INode *parent, key_type key, uint64_t hash, uint8_t level) {
                CNode *pm = parent ? parent->main.load() : nullptr;


                CNode *m = currentNode->main.load();

                if (contractParent(parent, currentNode, pm, m, level, hash)) {
                    return hamt::LOOKUP_RESTART;
                }

                Node *nextNode = m->getSubNode(extractHashPartByLevel(hash, level));
                if (nextNode == nullptr) {
                    return hamt::LOOKUP_NOT_FOUND;
                } else if (nextNode->type == SNODE) {
                    if (static_cast<SNode *>(nextNode)->contains(key)) {
                        return hamt::createSuccessfulLookupResult(static_cast<SNode *>(nextNode)->getValue(key));
                    }
                    return hamt::LOOKUP_NOT_FOUND;
                } else if (nextNode->type == INODE) {
                    return lookup(static_cast<INode *>(nextNode), currentNode, key, hash, level + 1);
                }
            }

            hamt::RemoveResult
            remove(INode *currentNode, INode *parent, key_type key, uint64_t hash, uint8_t level) {
                CNode *pm = parent ? parent->main.load() : nullptr;
                CNode *m = currentNode->main.load();

                if (contractParent(parent, currentNode, pm, m, level, hash)) {
                    return hamt::REMOVE_RESTART;
                }

                CNode *updated = getCopy(m);
                uint8_t path = extractHashPartByLevel(hash, level);
                Node *subNode = updated->getSubNode(path);

                hamt::RemoveResult res{};

                if (subNode == nullptr) {
                    res = hamt::REMOVE_NOT_FOUND;
                } else if (subNode->type == SNODE) {
                    if (static_cast<SNode *>(subNode)->contains(key)) {
                        value_type delVal = static_cast<SNode *>(subNode)->getValue(key);
                        transformToWithDeletedKey(updated, static_cast<SNode *>(subNode), key,
                                                  extractHashPartByLevel(hash, level));
                        updated->isTomb = isTombed(updated, root, currentNode);
                        res = (currentNode->main.compare_exchange_strong(m, updated))
                              ? createSuccessfulRemoveResult(delVal) : hamt::REMOVE_RESTART;
                    } else {
                        res = hamt::REMOVE_NOT_FOUND;
                    }
                } else if (subNode->type == INODE) {
                    res = remove(static_cast<INode *>(subNode), currentNode, key, hash, level + 1);
                }

                if (res == hamt::REMOVE_NOT_FOUND || res == hamt::REMOVE_RESTART) {
                    return res;
                }

                contractParent(parent, currentNode, pm, updated, level, hash);

                return res;
            }

            bool insert(INode *currentNode, INode *parent, SNode *newNode, uint8_t level) {
                CNode *pm = parent ? parent->main.load() : nullptr;
                CNode *m = currentNode->main.load();

                if (contractParent(parent, currentNode, pm, m, level, newNode->getHash())) {
                    return false;
                }


                CNode *updated = getCopy(m);
                uint8_t path = extractHashPartByLevel(newNode->getHash(), level);

                Node *subNode = updated->getSubNode(path);
                if (subNode == nullptr) {
                    transformToWithInsertedChild(updated, newNode, path);
                    updated->isTomb = isTombed(updated, root, currentNode);
                    return currentNode->main.compare_exchange_strong(m, updated);
                } else if (subNode->type == SNODE) {
                    auto *s = static_cast<SNode *>(subNode);
                    if (s->contains(newNode)) {
                        transformToWithReplacedPair(updated, s, newNode, path);
                        updated->isTomb = isTombed(updated, root, currentNode);
                        return currentNode->main.compare_exchange_strong(m, updated);
                    } else if (level == MAX_LEVEL_COUNT) {
                        transformToWithMergedChild(updated, s, newNode, path);
                        updated->isTomb = isTombed(updated, root, currentNode);
                        return currentNode->main.compare_exchange_strong(m, updated);
                    } else {
                        transformToWithDownChild(updated, newNode, s, level, path);
                        updated->isTomb = isTombed(updated, root, currentNode);
                        return currentNode->main.compare_exchange_strong(m, updated);
                    }
                } else if (subNode->type == INODE) {
                    return insert(static_cast<INode *>(subNode), currentNode, newNode, level + 1);
                } else {
                    fprintf(stderr, "Node with unknown type\n");
                    return false;
                }
            }

        };


    }


} // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_HAMT_H
