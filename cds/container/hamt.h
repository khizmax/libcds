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
        template<typename GC, typename K, typename V>
        class Hamt {
            typedef GC gc;
            typedef K key_type;
            typedef V value_type;
        protected:
            struct InsertResult {
                enum class Status {
                    Inserted,
                    Failed
                };

                V value;
                Status status;

                bool operator==(InsertResult b) const {
                    if ((status == Status::Failed) && (b.status == Status::Failed)) return true;
                    if ((status == Status::Inserted) && (b.status == Status::Inserted)) return true;
                    return false;
                }

                InsertResult& operator=(InsertResult b) {
                    this->status = b.status;
                    this->value = value;
                    return *this;
                }

                bool operator!=(InsertResult b) const {
                    return !(*this == b);
                }
            };

            struct LookupResult {
                enum LookupResultStatus {
                    NotFound,
                    Found,
                    Failed
                };

                V value;
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


            struct RemoveResult {
                enum Status {
                    Removed,
                    Failed,
                    NotFound,
                };

                V value;
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

            const LookupResult LOOKUP_RESTART{0, LookupResult::Failed};
            const LookupResult LOOKUP_NOT_FOUND{0, LookupResult::NotFound};
            const RemoveResult REMOVE_NOT_FOUND{0, RemoveResult::NotFound};
            const RemoveResult REMOVE_RESTART{0, RemoveResult::Failed};
            const InsertResult INSERT_RESTART{.status = InsertResult::Status::Failed};
            const InsertResult INSERT_SUCCESSFUL{.status = InsertResult::Status::Inserted};

            RemoveResult createSuccessfulRemoveResult(int value) {
                return {value, RemoveResult::Removed};
            }

            LookupResult createSuccessfulLookupResult(int value) {
                return {value, LookupResult::Found};
            }


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

                explicit Node(NodeType type) {
                    this->type = type;
                }
            };


            class SNode : public Node {
            public:
                static uint64_t generateSimpleHash(K key) {
                    return std::hash<K>{}(key);
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
                    uint8_t index = getArrayIndexByBmp(path);
                    assert(index < array.size());
                    return array[index];
                }

                uint8_t getChildCount() const {
                    return __builtin_popcount(bmp.data);
                }

                Node *getFirstChild() const {
                    assert(array.size() > 0);
                    return array.front();
                }

                void insertChild(Node *const newChild, uint8_t path) {
                    bmp.set(path);
                    array.insert(array.begin() + getArrayIndexByBmp(path), newChild);
                }

                void replaceChild(Node *const newChild, uint8_t path) {
                    uint8_t index = getArrayIndexByBmp(path);
                    assert(index < array.size());
                    array[index] = newChild;
                }

                void deleteChild(uint8_t path) {
                    uint8_t index = getArrayIndexByBmp(path);
                    assert(index < array.size());
                    array.erase(array.begin() + index);
                    bmp.unset(path);
                }

                uint8_t getArrayIndexByBmp(uint8_t pos) const {
                    assert(pos < 32);
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
                for (auto &p: node2->pair) {
                    if (!node1->contains(p.key)) {
                        node1->pair.insert(p);
                    }
                }
                return node1;
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

            void transformToWithDeletedKey(CNode *updated, SNode *updatedSubNode, key_type key, uint8_t path) {
                if (updatedSubNode->pair.size() > 1) {
                    updatedSubNode->pair.erase({key, updatedSubNode->getValue(key)});
                    updated->replaceChild(updatedSubNode, path);
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

                auto *updated = new CNode(*pm);
                transformToContractedParent(updated, m, extractHashPartByLevel(hash, level - 1));
                if (parent->main.compare_exchange_strong(pm, updated)) {
                    gc::template retire<Node>(pm, &dispose);
                } else {
                    delete updated;
                }
                return true;
            }

            static void dispose(void *v) {
                switch (((Node *) v)->type) {
                    case (SNODE): {
                        delete ((SNode *) v);
                        break;
                    }
                    case (CNODE): {
                        delete ((CNode *) v);
                        break;
                    }
                }
            }

        public:

            Hamt() {
                root = new INode(nullptr);
            }

            ~Hamt() {
                dealloc(root);
            }

            RemoveResult remove(key_type key) {
                typename gc::template GuardArray<2> guards;
                while (true) {
                    if (root->main.load() == nullptr) {
                        return REMOVE_NOT_FOUND;
                    }
                    RemoveResult res = remove(root, nullptr, key, SNode::generateSimpleHash(key), 0, &guards);
                    if (res != REMOVE_RESTART) {
                        return res;
                    }
                }
            }

            InsertResult insert(key_type key, value_type value) {
                typename gc::template GuardArray<2> guards;
                auto *s = new SNode(key, value);
                while (true) {
                    CNode *r = root->main.load();
                    if (r == nullptr) {
                        auto *updated = new CNode();
                        updated->insertChild(s, extractHashPartByLevel(s->getHash(), 0));
                        if (root->main.compare_exchange_strong(r, updated)) {
                            return INSERT_SUCCESSFUL;
                        } else {
                            delete updated;
                        }
                    } else {
                        InsertResult res = insert(root, nullptr, s, 0, &guards);
                        if (res != INSERT_RESTART) {
                            return res;
                        }
                    }
                }
            }

            LookupResult lookup(key_type key) {
                typename gc::template GuardArray<2> guards;
                while (true) {
                    if (root->main.load() == nullptr) {
                        return LOOKUP_NOT_FOUND;
                    } else {
                        LookupResult res = lookup(root, nullptr, key, SNode::generateSimpleHash(key), 0, &guards);
                        if (res != LOOKUP_RESTART) {
                            return res;
                        }
                    }
                }
            }

        private:
            INode *root;

            void dealloc(Node *n) {
                switch (n->type) {
                    case CNODE: {
                        auto *c = static_cast<CNode *>(n);
                        for (int i = 0; i < 32; i++) {
                            if (c->getSubNode(i) != NULL) {
                                dealloc(c->getSubNode(i));
                            }
                        }
                        delete c;
                        break;
                    }
                    case INODE: {
                        auto *i = static_cast<INode *>(n);
                        if (i->main != nullptr) {
                            dealloc(i->main.load());
                        }
                        delete i;
                        break;
                    }
                    case SNODE: {
                        auto *s = static_cast<SNode *>(n);
                        delete s;
                        break;
                    }
                }
            }

            void dealloc(Node *n, uint64_t hash, uint8_t level) {

                switch (n->type) {
                    case CNODE: {
                        uint8_t path = extractHashPartByLevel(hash, level);
                        auto *c = static_cast<CNode *>(n);
                        dealloc(c->getSubNode(path), hash, level + 1);
                        delete c;
                        break;
                    }
                    case INODE: {
                        auto *i = static_cast<INode *>(n);
                        dealloc(i->main, hash, level);
                        delete i;
                        break;
                    }
                    case SNODE: {
                        auto *s = static_cast<SNode *>(n);
                        delete s;
                        break;
                    }
                }
            }

            LookupResult
            lookup(INode *currentNode, INode *parent, key_type key, uint64_t hash, uint8_t level,
                   typename gc::template GuardArray<2> *guard) {
                CNode *pm = parent ? parent->main.load() : nullptr;
                CNode *m = currentNode->main.load();

                guard->assign(0, pm);
                guard->assign(1, m);

                if (contractParent(parent, currentNode, pm, m, level, hash)) {
                    return LOOKUP_RESTART;
                }

                Node *nextNode = m->getSubNode(extractHashPartByLevel(hash, level));
                if (nextNode == nullptr) {
                    return LOOKUP_NOT_FOUND;
                } else if (nextNode->type == SNODE) {
                    if (static_cast<SNode *>(nextNode)->contains(key)) {
                        return createSuccessfulLookupResult(static_cast<SNode *>(nextNode)->getValue(key));
                    }
                    return LOOKUP_NOT_FOUND;
                } else if (nextNode->type == INODE) {
                    return lookup(static_cast<INode *>(nextNode), currentNode, key, hash, level + 1, guard);
                }
            }

            RemoveResult
            remove(INode *currentNode, INode *parent, key_type key, uint64_t hash, uint8_t level,
                   typename gc::template GuardArray<2> *guard) {
                CNode *pm = parent ? parent->main.load() : nullptr;
                CNode *m = currentNode->main.load();

                guard->assign(0, pm);
                guard->assign(1, m);

                if (contractParent(parent, currentNode, pm, m, level, hash)) {
                    return REMOVE_RESTART;
                }

                auto *updated = new CNode(*m);
                uint8_t path = extractHashPartByLevel(hash, level);
                Node *subNode = updated->getSubNode(path);

                RemoveResult res{};

                if (subNode == nullptr) {
                    delete updated;
                    return REMOVE_NOT_FOUND;
                } else if (subNode->type == SNODE) {
                    auto *updatedSubNode = new SNode(*static_cast<SNode *>(subNode));
                    if (updatedSubNode->contains(key)) {
                        V delVal = updatedSubNode->getValue(key);
                        transformToWithDeletedKey(updated, updatedSubNode, key, path);
                        updated->isTomb = isTombed(updated, root, currentNode);
                        res = (currentNode->main.compare_exchange_strong(m, updated))
                              ? createSuccessfulRemoveResult(delVal) : REMOVE_RESTART;
                        if (res == REMOVE_RESTART) {
                            if (updated->getSubNode(path) != nullptr) {
                                delete subNode;
                            }
                            delete updated;
                        } else {
                            gc::template retire<Node>(m, &dispose);
                            gc::template retire<Node>(subNode, &dispose);
                            if (updated->getSubNode(path) == nullptr) {
                                delete updatedSubNode;
                            }

                            contractParent(parent, currentNode, pm, updated, level, hash);
                        }
                        return res;
                    } else {
                        delete updated;
                        delete updatedSubNode;
                        return REMOVE_NOT_FOUND;
                    }
                } else if (subNode->type == INODE) {
                    res = remove(static_cast<INode *>(subNode), currentNode, key, hash, level + 1, guard);
                    if (res == REMOVE_RESTART || res == REMOVE_NOT_FOUND) {
                        delete updated;
                        return res;
                    }
                    contractParent(parent, currentNode, pm, updated, level, hash);
                    delete updated;

                    return res;
                }
            }

            InsertResult insert(INode *currentNode, INode *parent, SNode *newNode, uint8_t level,
                                typename gc::template GuardArray<2> *guard
            ) {
                CNode *pm = parent ? parent->main.load() : nullptr;
                CNode *m = currentNode->main.load();

                guard->assign(0, pm);
                guard->assign(1, m);

                if (contractParent(parent, currentNode, pm, m, level, newNode->getHash())) {
                    return INSERT_RESTART;
                }

                guard->clear(0);

                auto *updated = new CNode(*m);
                uint8_t path = extractHashPartByLevel(newNode->getHash(), level);
                InsertResult res{};

                Node *subNode = updated->getSubNode(path);
                if (subNode == nullptr) {
                    transformToWithInsertedChild(updated, newNode, path);
                    updated->isTomb = isTombed(updated, root, currentNode);
                    res = currentNode->main.compare_exchange_strong(m, updated) ? INSERT_SUCCESSFUL
                                                                                : INSERT_RESTART;
                    if (res == INSERT_SUCCESSFUL) {
                        gc::template retire<Node>(m, &dispose);
                    } else {
                        delete updated;
                    }
                    return res;
                } else if (subNode->type == SNODE) {
                    auto *updatedSubNode = new SNode(*static_cast<SNode *>(subNode));
                    if (updatedSubNode->contains(newNode)) {
                        transformToWithReplacedPair(updated, updatedSubNode, newNode, path);
                        updated->isTomb = isTombed(updated, root, currentNode);
                        res = currentNode->main.compare_exchange_strong(m, updated) ? INSERT_SUCCESSFUL
                                                                                    : INSERT_RESTART;
                        if (res == INSERT_SUCCESSFUL) {
                            gc::template retire<Node>(m, &dispose);
                            gc::template retire<Node>(subNode, &dispose);
                            guard->clear(1);
                        } else {
                            delete updated;
                        }
                        delete updatedSubNode;
                        return res;
                    } else if (level == MAX_LEVEL_COUNT) {
                        transformToWithMergedChild(updated, updatedSubNode, newNode, path);
                        updated->isTomb = isTombed(updated, root, currentNode);
                        res = currentNode->main.compare_exchange_strong(m, updated) ? INSERT_SUCCESSFUL
                                                                                    : INSERT_RESTART;
                        if (res == INSERT_SUCCESSFUL) {
                            gc::template retire<Node>(m, &dispose);
                            gc::template retire<Node>(subNode, &dispose);
                            guard->clear(1);
                        } else {
                            delete updated;
                        }
                        delete updatedSubNode;
                        return res;
                    } else {
                        transformToWithDownChild(updated, newNode, updatedSubNode, level, path);
                        updated->isTomb = isTombed(updated, root, currentNode);
                        res = currentNode->main.compare_exchange_strong(m, updated) ? INSERT_SUCCESSFUL
                                                                                    : INSERT_RESTART;
                        if (res == INSERT_SUCCESSFUL) {
                            gc::template retire<Node>(m, &dispose);
                            gc::template retire<Node>(subNode, &dispose);
                            guard->clear(1);
                        } else {
                            dealloc(updated, updatedSubNode->getHash(), level);
                        }
                        return res;
                    }
                } else if (subNode->type == INODE) {
                    delete updated;
                    return insert(static_cast<INode *>(subNode), currentNode, newNode, level + 1, guard);
                }
            }
        };


    }


} // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_HAMT_H
