// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_HAMT_H
#define CDSLIB_CONTAINER_HAMT_H

#include <cstdint>
#include <string>
#include <vector>
#include <bitset>
#include <set>
#include <mutex>
#include "atomic"


#define HASH_PIECE_LEN 5
#define BRANCH_FACTOR 32
#define MAX_LEVEL_COUNT (int (64 / 5))

using namespace std;

namespace cds {
    namespace container {
        namespace hamt {


        } // namespace hamt


        struct Bitmap {
            uint32_t data;

            bool isSet(uint8_t pos) const;

            void set(uint8_t pos);

            void unset(uint8_t pos);
        };

        uint64_t generateSimpleHash(uint64_t key) {
            return key;
        }

        uint64_t generateSimpleHash(string key) {
            uint64_t hash = 0;
            for (size_t i = 0; i < key.size(); i++) {
                hash += key[i] * i;
            }
            return hash;
        }

        uint8_t extractHashPartByLevel(uint64_t hash, uint8_t level) {
            return (hash >> (level * HASH_PIECE_LEN)) & ((1 << HASH_PIECE_LEN) - 1);
        }

        bool Bitmap::isSet(uint8_t pos) const {
            return ((data >> pos) & 1) == 1;
        }

        void Bitmap::set(uint8_t pos) {
            data = data | (1 << pos);
        }

        void Bitmap::unset(uint8_t pos) {
            data = data & (~(1 << pos));
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


        template<class K, class V>
        class SNode : public Node {
        public:

            SNode(K k, V v) : Node(SNODE) {
                this->pair.insert({k, v});
                this->hash = generateSimpleHash(k);
            }

            uint64_t getHash() {
                return hash;
            }

            bool contains(K k) {
                for (auto &p: this->pair) {
                    if (p.key == k) {
                        return true;
                    }
                }
                return false;
            }

            bool contains(SNode<K, V> *node) {
                for (auto &p: node->pair) {
                    if (this->contains(p.key)) {
                        return true;
                    }
                }
                return false;
            }

            int getValue(K k) {
                for (auto &p: this->pair) {
                    if (p.key == k) {
                        return p.value;
                    }
                }
                // TODO ???
                return 0;
            }

            struct Pair {
                K key;
                V value;

                bool operator<(const Pair &p) const {
                    return this->key < p.key;
                }
            };

        public:
            uint64_t hash{};
            set<Pair> pair;
        };

        template<class K, class V>
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


        template<class K, class V>
        class INode : public Node {
        public:
            INode() : Node(INODE) {}

            INode(CNode<K, V> *main) : INode() {
                this->main.store(main, std::memory_order_seq_cst);
            }

            atomic<CNode<K, V> *> main;
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

        const LookupResult LOOKUP_NOT_FOUND{0, LookupResult::NotFound};
        const LookupResult LOOKUP_RESTART{0, LookupResult::Failed};

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

        const RemoveResult REMOVE_NOT_FOUND{0, RemoveResult::NotFound};
        const RemoveResult REMOVE_RESTART{0, RemoveResult::Failed};

        RemoveResult createSuccessfulRemoveResult(int value) {
            return {value, RemoveResult::Removed};
        }

        template<class K, class V>
        SNode<K, V> *leftMerge(SNode<K, V> *node1, SNode<K, V> *node2) {
            auto *merged = new SNode<K, V>(*node1);
            for (auto &p: node2->pair) {
                if (!merged->contains(p.key)) {
                    merged->pair.insert(p);
                }
            }
            return merged;
        }

        template<class K, class V>
        CNode<K, V> *getCopy(CNode<K, V> *node) {
            return new CNode<K, V>(*node);
        }

        template<class K, class V>
        void transformToContractedParent(CNode<K, V> *updated, CNode<K, V> *m, uint8_t path) {
            updated->replaceChild(m->getFirstChild(), path);
        }

        template<class K, class V>
        void
        transformToWithReplacedPair(CNode<K, V> *updated, SNode<K, V> *subNode, SNode<K, V> *newNode, uint8_t path) {
            updated->replaceChild(leftMerge(newNode, subNode), path);
        }

        template<class K, class V>
        void transformToWithInsertedChild(CNode<K, V> *updated, Node *child, uint8_t path) {
            updated->insertChild(child, path);
        }

        template<class K, class V>
        void
        transformToWithMergedChild(CNode<K, V> *updated, SNode<K, V> *subNode, SNode<K, V> *newNode, uint8_t path) {
            updated->replaceChild(leftMerge(newNode, subNode), path);
        }

        template<class K, class V>
        void transformToWithDownChild(CNode<K, V> *updated, SNode<K, V> *newChild, SNode<K, V> *oldChild, uint8_t level,
                                      uint8_t path) {

            if (newChild->getHash() == oldChild->getHash()) {
                newChild = leftMerge(newChild, oldChild);
                updated->replaceChild(newChild, path);
            } else {

                auto *cur_c = new CNode<K, V>();
                auto *i = new INode<K, V>(cur_c);

                int j = level + 1;
                uint8_t newChildHashPath = extractHashPartByLevel(newChild->getHash(), j);
                uint8_t oldChildHashPath = extractHashPartByLevel(oldChild->getHash(), j);

                while (newChildHashPath == oldChildHashPath) {
                    auto *c = new CNode<K, V>();
                    cur_c->insertChild(new INode<K, V>(c), oldChildHashPath);
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

        template<class K, class V>
        void transformToWithDeletedKey(CNode<K, V> *updated, SNode<K, V> *subNode, K key, uint8_t path) {
            auto *newSubNode = new SNode<K, V>(*subNode);
            if (newSubNode->pair.size() > 1) {
                newSubNode->pair.erase({key, subNode->getValue(key)});
                updated->replaceChild(newSubNode, path);
            } else {
                updated->deleteChild(path);
            }
        }

        template<class K, class V>
        bool isTombed(const CNode<K, V> *const c, const INode<K, V> *const root, const INode<K, V> *const parent) {
            return root != parent &&
                   c->getChildCount() == 1 &&
                   c->getFirstChild()->type == SNODE;
        }

        template<class K, class V>
        bool
        contractParent(INode<K, V> *parent, INode<K, V> *i, CNode<K, V> *pm, CNode<K, V> *m, uint8_t level,
                       uint64_t hash) {
            if (!m->isTomb) return false;

            if (pm->getSubNode(extractHashPartByLevel(hash, level - 1)) != i) {
                return true;
            }

            CNode <K, V> *updated = getCopy(pm);
            transformToContractedParent(updated, m, extractHashPartByLevel(hash, level - 1));
            parent->main.compare_exchange_strong(pm, updated);
            return true;
        }


        template<class K, class V>
        class Hamt {
        private:
            INode<K, V> *root;
        public:
            Hamt() {
                root = new INode<K, V>(nullptr);
            }

            Node *getRoot() {
                return this->root;
            }

            LookupResult lookup(K key) {
                while (true) {
                    if (root->main.load() == nullptr) {
                        return LOOKUP_NOT_FOUND;
                    } else {
                        LookupResult res = lookup(root, nullptr, key, generateSimpleHash(key), 0);
                        if (res != LOOKUP_RESTART) {
                            return res;
                        }
                    }
                }
            }

            RemoveResult remove(K key) {
                while (true) {
                    if (root->main.load() == nullptr) {
                        return REMOVE_NOT_FOUND;
                    }
                    RemoveResult res = remove(root, nullptr, key, generateSimpleHash(key), 0);
                    if (res != REMOVE_RESTART) {
                        return res;
                    }
                }
            }


            bool insert(K key, V value) {
                while (true) {
                    CNode <K, V> *old = root->main.load();
                    if (old == nullptr) {
                        // root -> c -> s
                        auto *c = new CNode<K, V>();
                        auto *s = new SNode<K, V>(key, value);
                        c->insertChild(s, extractHashPartByLevel(s->getHash(), 0));
                        if (root->main.compare_exchange_strong(old, c)) {
                            return true;
                        }
                    } else {
                        if (insert(root, nullptr, new SNode<K, V>(key, value), 0)) {
                            return true;
                        }
                    }
                }
            }

        private:
            LookupResult lookup(INode<K, V> *currentNode, INode<K, V> *parent, K key, uint64_t hash, uint8_t level) {
                CNode <K, V> *pm = parent ? parent->main.load() : nullptr;


                CNode <K, V> *m = currentNode->main.load();

                if (contractParent(parent, currentNode, pm, m, level, hash)) {
                    return LOOKUP_RESTART;
                }

                Node *nextNode = m->getSubNode(extractHashPartByLevel(hash, level));
                if (nextNode == nullptr) {
                    return LOOKUP_NOT_FOUND;
                } else if (nextNode->type == SNODE) {
                    if (static_cast<SNode <K, V> *>(nextNode)->contains(key)) {
                        return createSuccessfulLookupResult(static_cast<SNode <K, V> *>(nextNode)->getValue(key));
                    }
                    return LOOKUP_NOT_FOUND;
                } else if (nextNode->type == INODE) {
                    return lookup(static_cast<INode <K, V> *>(nextNode), currentNode, key, hash, level + 1);
                }
            }

            RemoveResult remove(INode<K, V> *currentNode, INode<K, V> *parent, K key, uint64_t hash, uint8_t level) {
                CNode <K, V> *pm = parent ? parent->main.load() : nullptr;
                CNode <K, V> *m = currentNode->main.load();

                if (contractParent(parent, currentNode, pm, m, level, hash)) {
                    return REMOVE_RESTART;
                }

                CNode <K, V> *updated = getCopy(m);
                uint8_t path = extractHashPartByLevel(hash, level);
                Node *subNode = updated->getSubNode(path);

                RemoveResult res{};

                if (subNode == nullptr) {
                    res = REMOVE_NOT_FOUND;
                } else if (subNode->type == SNODE) {
                    if (static_cast<SNode <K, V> *>(subNode)->contains(key)) {
                        V delVal = static_cast<SNode <K, V> *>(subNode)->getValue(key);
                        transformToWithDeletedKey(updated, static_cast<SNode <K, V> *>(subNode), key,
                                                  extractHashPartByLevel(hash, level));
                        updated->isTomb = isTombed(updated, root, currentNode);
                        res = (currentNode->main.compare_exchange_strong(m, updated))
                              ? createSuccessfulRemoveResult(delVal) : REMOVE_RESTART;
                    } else {
                        res = REMOVE_NOT_FOUND;
                    }
                } else if (subNode->type == INODE) {
                    res = remove(static_cast<INode <K, V> *>(subNode), currentNode, key, hash, level + 1);
                }

                if (res == REMOVE_NOT_FOUND || res == REMOVE_RESTART) {
                    return res;
                }

                contractParent(parent, currentNode, pm, updated, level, hash);

                return res;
            }

            bool insert(INode<K, V> *currentNode, INode<K, V> *parent, SNode<K, V> *newNode, uint8_t level) {
                CNode <K, V> *pm = parent ? parent->main.load() : nullptr;
                CNode <K, V> *m = currentNode->main.load();

                if (contractParent(parent, currentNode, pm, m, level, newNode->getHash())) {
                    return false;
                }


                CNode <K, V> *updated = getCopy(m);
                uint8_t path = extractHashPartByLevel(newNode->getHash(), level);

                Node *subNode = updated->getSubNode(path);
                if (subNode == nullptr) {
                    transformToWithInsertedChild(updated, newNode, path);
                    updated->isTomb = isTombed(updated, root, currentNode);
                    return currentNode->main.compare_exchange_strong(m, updated);
                } else if (subNode->type == SNODE) {
                    auto *s = static_cast<SNode <K, V> *>(subNode);
                    if (s->contains(newNode)) {
                        transformToWithReplacedPair(updated, s, newNode, path);
                        updated->isTomb = isTombed(updated, root, currentNode);
                        return currentNode->main.compare_exchange_strong(m, updated);
                    } else if (level == MAX_LEVEL_COUNT) {
                        transformToWithMergedChild(updated, s, newNode, path);
                        updated->isTomb = isTombed(updated, root, currentNode);
                        return currentNode->main.compare_exchange_strong(m, updated);
                    } else {
                        transformToWithDownChild<K, V>(updated, newNode, s, level, path);
                        updated->isTomb = isTombed(updated, root, currentNode);
                        return currentNode->main.compare_exchange_strong(m, updated);
                    }
                } else if (subNode->type == INODE) {
                    return insert(static_cast<INode <K, V> *>(subNode), currentNode, newNode, level + 1);
                } else {
                    fprintf(stderr, "Node with unknown type\n");
                    return false;
                }
            }
        };
    }
} // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_HAMT_H
