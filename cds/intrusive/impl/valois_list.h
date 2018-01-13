/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CDS_VALOIS_LIST_H
#define CDS_VALOIS_LIST_H

#include <cds/intrusive/details/valois_list_base.h>
#include <cds/details/make_const_type.h>

namespace cds {
    namespace intrusive {

        template<class GC, typename T, class Traits>
        class ValoisList {

        public:
            typedef GC gc;         ///< Garbage collector
            typedef T value_type; ///< type of value stored in the list
            typedef Traits traits;     ///< Traits template parameter
            typedef valois_list::node<value_type> node_type;  ///< node type

            typedef typename opt::details::make_comparator<value_type, traits>::type key_comparator;

            typedef typename traits::disposer disposer;            ///< disposer for \p value_type
            typedef typename traits::back_off back_off;       ///< back-off strategy
            typedef typename traits::memory_model memory_model;   ///< Memory ordering. See \p cds::opt::memory_model option
            typedef typename traits::node_allocator node_allocator; ///< Node allocator
            typedef typename traits::stat stat;           ///< Internal statistics

            static CDS_CONSTEXPR const size_t c_nHazardPtrCount = 4;    ///< Count of hazard pointer required for the algorithm

        protected:
            typedef typename atomics::atomic<node_type *> atomic_node_ptr;
            typedef typename node_type::marked_data_ptr marked_data_ptr;

            atomic_node_ptr list_head_node;        ///< Head pointer
            atomic_node_ptr m_pTail;        ///< Tail pointer

        public:
            friend class iterator;

        protected:

            class iterator {
                friend class ValoisList;

            protected:
                node_type * current_node;   // Valois target - current real node
                node_type * aux_pNode;      // Valois pre_aux - aux node before the real node
                node_type * prev_node;      // Valois pre_cell - real node before the current real node
                typename gc::Guard m_Guard;

                bool next() {
                    if (current_node->next == nullptr) {     // if tail
                        return false;
                    }
                    prev_node = current_node;
                    aux_pNode = current_node->next.load(atomics::memory_order_relaxed);
                    //current_node = aux_pNode->next.load();
                    update_iterator();
                    return true;
                }



            public:
                typedef typename cds::details::make_const_type<value_type, false>::pointer value_ptr;
                typedef typename cds::details::make_const_type<value_type, false>::reference value_ref;

                iterator()
                        : current_node(nullptr), aux_pNode(nullptr), prev_node(nullptr) {}

                // node - only Head
                iterator( node_type * node) {

                    prev_node    = node;
                    aux_pNode    = node->next.load();

                    update_iterator();
                }

                void update_iterator() {
                    if (aux_pNode->next == current_node) {
                        return;
                    }

                    node_type * p = aux_pNode;
                    node_type * n = p->next.load(atomics::memory_order_acquire);

                    while ( n->next != nullptr && n->data == nullptr) {    //while not last and is aux node

                        prev_node->next.compare_exchange_strong(
                                p,
                                n,
                                atomics::memory_order_seq_cst,
                                atomics::memory_order_seq_cst
                        );
                        p = n;
                        n = p->next.load(atomics::memory_order_acquire);
                    }

                    aux_pNode = p;
                    current_node = n;

                    m_Guard.protect( current_node->data, []( marked_data_ptr ptr ) { return ptr.ptr(); }).ptr();
                }


                iterator &operator++() {
                    iterator temp = *this;
                    next();
                    return temp;
                }

                iterator &operator=(iterator &second) {
                    current_node = second.current_node;
                    aux_pNode = second.aux_pNode;
                    prev_node = second.prev_node;
                    m_Guard.copy(second.m_Guard);
                    return *this;
                }

                bool operator==(const iterator &second) const {
                    return (current_node->data == second.current_node->data);
                }

                bool operator!=(const iterator &second) const {
                    return (current_node->data != second.current_node->data);
                }
            };

        public:
            ValoisList() {
                init_list();
            }


            ~ValoisList() {
                destroy();
            }

            iterator begin() {
                return iterator(list_head_node);
            }

            /**
             * try insert in the position
             * @param i
             * @param val
             * @return
             */

            template <typename Q, typename Compare >
            bool search_insert(Q* val, Compare cmp) {
                typename gc::Guard m_Guard;
                iterator * mIter = new iterator(list_head_node);

                while (true) {

                    m_Guard.protect(
                            mIter->current_node->data,
                            []( marked_data_ptr ptr ) { return ptr.ptr(); }
                    ).ptr();

                    value_type * nVal = mIter->current_node
                            ->data.load(
                                    atomics::memory_order_acquire
                            ).ptr();

                    if(nVal == nullptr){
                        // for last node;
                        try_insert(mIter, val);
                        delete mIter;
                        return true;
                    }

                    int const nCmp = cmp(*val, *nVal);

                    if (nCmp == 0) {
                        delete mIter;
                        return true;
                    } else if (nCmp < 0) {
                        bool k = try_insert(mIter, val);
                        delete mIter;
                        if (k){
                            return k;
                        } else{
                            // find again;
                            mIter = new iterator(list_head_node);
                        }

                    } else {
                        mIter->next();
                    }
                }
            }

            bool try_insert(iterator *i, value_type * val) {

                node_type *real_node = new node_type(val);
                node_type *aux_node = new node_type();

                real_node->next = aux_node;
                aux_node->next = i->current_node;

                bool insert_status = i->aux_pNode->next.compare_exchange_strong(
                        i->current_node,
                        real_node,
                        memory_model::memory_order_seq_cst,
                        memory_model::memory_order_seq_cst
                );

                /*i->prev_node = real_node;*/


                return insert_status;
            }


            bool insert( value_type &val ){
                return search_insert(&val, key_comparator() );
            }


            /**
             * delete value from linked list;
             * @param value
             * @return
             */
            template <typename Q, typename Compare >
            bool erase(Q* val, Compare cmp) {
                iterator * i = new iterator( list_head_node );
                //search node
                while (i->current_node->next.load() != nullptr ) {
                    value_type * nVal = i->current_node->data.load(atomics::memory_order_acquire ).ptr();
                    int const nCmp = cmp( *val , *nVal );

                    if ( nCmp == 0 ){

                        gc::template retire<disposer>( nVal );

                        bool result = try_erase(i);
                        delete i;
                        if (result){
                            return true;
                        }
                        else{
                            //run again
                            i = new iterator( list_head_node );
                        }
                    }
                    else if ( nCmp > 0 ){
                        // not found
                        delete i;
                        return false;
                    }
                    else{
                        i->next();
                    }
                }
                // not found
                delete i;
                return false;
            }

            bool erase(value_type val){
                return erase(&val, key_comparator());
            }



            template <typename Q, typename Compare >
            bool contains( Q* val, Compare cmp) {
                typename gc::Guard m_Guard;
                iterator * i = new iterator( list_head_node );
                while (i->current_node->next.load() != nullptr ) {

                    m_Guard.protect( i->current_node->data, []( marked_data_ptr ptr ) { return ptr.ptr(); }).ptr();

                    value_type * nVal = i->current_node->data.load(atomics::memory_order_acquire ).ptr();
                    int const nCmp = cmp( *val , *nVal );

                    if ( nCmp == 0 ){
                        delete i;
                        return true;
                    }
                    else if ( nCmp < 0 ){
                        delete i;
                        return false;
                    }
                    else{
                        i->next();
                    }
                }
                delete i;
                return false;
            }

            bool contains(value_type &val) {
                return contains( &val, key_comparator() );
            }

            bool empty() {
                iterator * i = new iterator(list_head_node);

                if ( i->next() ) {
                    // if next is not exist() container is empty()
                    delete i;
                    return false;
                } else {
                    delete i;
                    return true;
                }
            }


        private:

            void init_list() {
                node_type * aux_temp = new node_type();
                list_head_node.store(new node_type);
                list_head_node.load()->next.store( aux_temp, atomics::memory_order_release);  //link to aux node
                m_pTail.store(new node_type);
                aux_temp->next.store( m_pTail, atomics::memory_order_release );
                m_pTail.load()->next.store(nullptr, atomics::memory_order_release );          //link tail to nullptr
            }

            void destroy() {
                typename gc::Guard m_Guard;
                m_Guard.clear();

                node_type * tNode = list_head_node.load()->next.load(memory_model::memory_order_relaxed);

                while (tNode->next.load(memory_model::memory_order_relaxed) != nullptr) {
                    value_type * pVal = tNode->data.load(memory_model::memory_order_relaxed).ptr();
                    if (pVal) {

                        erase(*pVal);

                        tNode = list_head_node.load(
                                atomics::memory_order_release)
                                ->next.load(memory_model::memory_order_relaxed);
                    }
                    tNode = tNode->next.load(memory_model::memory_order_relaxed);
                }
            }

            bool try_erase(iterator *i) {

                node_type *d = i->current_node;

                node_type *n = i->current_node->next.load(atomics::memory_order_release);
                bool r = i->aux_pNode->next.compare_exchange_strong(d, n, atomics::memory_order_seq_cst);

                if (!r){
                    return false;
                }

                return true;
            }
        };

    }
}

#endif //CDS_VALOIS_LIST_H
