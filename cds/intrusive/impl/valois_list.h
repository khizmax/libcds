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
                atomic_node_ptr current_node;   // Valois target - current real node
                atomic_node_ptr aux_pNode;      // Valois pre_aux - aux node before the real node
                atomic_node_ptr prev_node;      // Valois pre_cell - real node before the current real node
                typename gc::Guard current_guard;
                typename gc::Guard prev_guard;
                typename gc::Guard aux_guard;

                bool next() {
                    if (current_node.load()->next.load() == nullptr) {     // if tail
                        current_guard.clear();
                        prev_guard.clear();
                        aux_guard.clear();
                        return false;
                    }
                    prev_guard.copy(current_guard);
                    prev_node = prev_guard.protect(current_node);
                    aux_pNode = aux_guard.protect(current_node.load()->next);

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
                    set(node);
                }

                void set(node_type * node){
                    prev_node    = node;
                    aux_pNode    = node->next.load();

                    update_iterator();
                }

                void update_iterator() {
                    if (aux_pNode.load()->next == current_node) {
                        return;
                    }
                    node_type * new_aux = aux_pNode.load();
                    node_type * new_cur = new_aux->next.load(atomics::memory_order_acquire);

                    while ( new_cur->next != nullptr && new_cur->data == nullptr) {    //while not last and is aux node

                        prev_node.load()->next.compare_exchange_strong(
                                new_aux,
                                new_cur,
                                atomics::memory_order_seq_cst,
                                atomics::memory_order_seq_cst
                        );

                        new_aux = new_cur;

                        new_cur = new_aux->next.load(atomics::memory_order_acquire);

                    }

                    current_node.store(new_cur);
                    aux_pNode.store(new_aux);
                }

                /*void print(){
                    std::cout << "iterator" << std::endl;
                    if (current_node->data.load() != nullptr){
                        std::cout << "current    " << current_node << " value " << *(current_node->data.load().ptr()) << std::endl;
                    } else{
                        std::cout << "current    " << current_node << " value " << "NULL" << std::endl;
                    }
                    std::cout << "aux     " << aux_pNode << std::endl;
                    if (prev_node->data.load() != nullptr){
                        std::cout << "prev    " << prev_node << " value " << *(prev_node->data.load().ptr()) << std::endl;
                    } else{
                        std::cout << "prev    " << prev_node << " value " << "NULL" << std::endl;
                    }
                    std::cout << "--------------------" << std::endl;
                }*/
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
                iterator mIter;
                mIter.set(list_head_node);
                while (true) {
                    Q * nVal = mIter.current_node.load()
                            ->data.load(
                                    atomics::memory_order_acquire
                            ).ptr();

                    if(nVal == nullptr){
                        // for last node;
                        try_insert(mIter, val);
                        return true;
                    }

                    int const nCmp = cmp(*val, *nVal);

                    if (nCmp == 0) {
                        return true;
                    } else if (nCmp < 0) {
                        bool k = try_insert(mIter, val);

                        if (k){
                            return k;
                        } else{
                            // find again;
                            mIter.set(list_head_node);
                        }

                    } else {
                        mIter.next();
                    }
                }
            }

            bool try_insert(iterator& i, value_type * val) {

                node_type * real_node = new node_type(val);
                node_type * aux_node = new node_type();

                real_node->next = aux_node;
                aux_node->next.store(i.current_node.load());

                node_type * cur_node = i.current_node.load();

                bool insert_status = i.aux_pNode.load()->next.compare_exchange_strong(
                        cur_node,
                        real_node,
                        memory_model::memory_order_seq_cst,
                        memory_model::memory_order_seq_cst
                );

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
                iterator mIter;
                mIter.set( list_head_node );
                //search node
                while (mIter.current_node.load()->next.load() != nullptr ) {
                    value_type * nVal = mIter.current_node.load()->data.load(atomics::memory_order_acquire ).ptr();
                    int const nCmp = cmp( *val , *nVal );

                    if ( nCmp == 0 ){

                        bool result = try_erase(mIter);
                        if (result){
                            return true;
                        }
                        else{
                            //run again
                            mIter.set( list_head_node );
                        }
                    }
                    else if ( nCmp > 0 ){
                        // not found
                        return false;
                    }
                    else{
                        mIter.next();
                    }
                }
                // not found
                return false;
            }

            bool erase(value_type val){
                return erase(&val, key_comparator());
            }

            bool erase(value_type * val){
                return erase(val, key_comparator());
            }


            template <typename Q, typename Compare >
            bool contains( Q* val, Compare cmp) {
                typename gc::Guard m_Guard;
                iterator mIter;
                mIter.set(list_head_node);
                while (mIter.current_node.load()->next.load() != nullptr ) {
                    value_type * nVal = mIter.current_node.load()->data.load(atomics::memory_order_acquire ).ptr();
                    int const nCmp = cmp( *val , *nVal );

                    if ( nCmp == 0 ){
                        return true;
                    }
                    else if ( nCmp < 0 ){
                        return false;
                    }
                    else{
                        mIter.next();
                    }
                }
                return false;
            }

            bool contains(value_type &val) {
                return contains( &val, key_comparator() );
            }

            bool empty() {
                iterator mIter;
                mIter.set(list_head_node);

                if ( mIter.next() ) {
                    // if next is not exist() container is empty()
                    return false;
                } else {
                    return true;
                }
            }
            /*
            void print_all_by_iterator(){
                iterator mIter;
                mIter.set(list_head_node);
                std::cout << "----------start print by iterator---------------" << std::endl;
                while(mIter.current_node.load()->next.load() != nullptr){
                    value_type * nVal = mIter.current_node.load()->data.load(memory_model::memory_order_seq_cst ).ptr();
                    std::cout << *nVal << std::endl;
                    mIter.next();
                }
                std::cout << "----------end---------------" << std::endl;
            }
            void print_all_by_link(){
                std::cout << "----------start print by link---------------" << std::endl;
                node_type * next_node = list_head_node.load();
                node_type * next_aux_node;
                int number = 0;
                do{
                    next_aux_node = next_node->next.load();
                    next_node = next_aux_node->next.load();
                    std::cout << "next_aux_node -> " <<next_aux_node << std::endl;
                    std::cout << "next_node -> " <<next_aux_node->next.load() << std::endl;
                    std::cout << "next_node.next -> " <<next_node->next.load() << std::endl << std::endl;
                    if (next_node->next){
                        value_type * nVal = next_node->data.load(memory_model::memory_order_seq_cst ).ptr();
                        value_type * nVala = next_aux_node->data.load(memory_model::memory_order_seq_cst ).ptr();
                        std::cout << number << " aux -> " << nVala <<  " -> "<<  std::endl;
                        std::cout << number << " -> " << nVal <<  " -> "<< *nVal  << std::endl;
                    }
                    number++;
                } while (next_node->next != nullptr);
                std::cout << "----------finish print by link---------------" << std::endl;
            }
            void print_all_pointers(){
                node_type * selected_node = list_head_node.load();
                int number = 0;
                std::string type;
                do{
                    if (selected_node->data.load() == nullptr)
                        std::cout << number << "\t" << selected_node << "\t AUX"  << std::endl;
                    else
                        std::cout << number << "\t" << selected_node << "\t " << *(selected_node->data.load().ptr()) << std::endl;
                    number++;
                    selected_node = selected_node->next.load();
                } while (selected_node != nullptr);
            }*/



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

                node_type * tNode = list_head_node.load()->next.load(memory_model::memory_order_relaxed);

                while (tNode->next.load(memory_model::memory_order_relaxed) != nullptr) {
                    value_type * pVal = tNode->data.load(memory_model::memory_order_relaxed).ptr();
                    if (pVal) {

                        deleted(*pVal);

                        tNode = list_head_node.load(
                                atomics::memory_order_release)
                                ->next.load(memory_model::memory_order_relaxed);
                    }
                    tNode = tNode->next.load(memory_model::memory_order_relaxed);
                }
            }

            bool try_erase(iterator& i) {

                node_type *d = i.current_node.load();

                node_type *n = i.current_node.load()->next.load(atomics::memory_order_release);
                bool r = i.aux_pNode.load()->next.compare_exchange_strong(d, n->next.load(), atomics::memory_order_seq_cst);

                if (!r){
                    return false;
                }
                gc::template retire<disposer>( d );
                gc::template retire<disposer>( n );

                /*delete d;
                delete n;*/

                return true;
            }



/*
            void append(int& number){
                node_type * next_node = list_head_node.load();
                node_type * next_aux_node;
                do{
                    next_aux_node = next_node->next.load();
                    next_node = next_aux_node->next.load();
                } while (next_aux_node->next != nullptr && next_node->next.load() != nullptr);
                int * index = new int32_t(number);
                node_type * new_next_node = new node_type(index);
                node_type * new_next_aux_node = new node_type();
                new_next_aux_node->next.store(next_node);
                new_next_node->next.store(new_next_aux_node);
                next_aux_node->next.store(new_next_node);
            }
            void append_in_first(const int& number){
                node_type * next_node = list_head_node.load();
                node_type * next_aux_node = next_node->next.load();
                int d = number;
                int * index = new int32_t(d);
                node_type * new_next_node = new node_type(index);
                node_type * new_next_aux_node = new node_type();
                new_next_aux_node->next.store(next_aux_node->next.load());
                new_next_node->next.store(new_next_aux_node);
                next_aux_node->next.store(new_next_node);
            }
            void append_in_position(const int& number, const int& position){
                node_type * next_node = list_head_node.load();
                node_type * next_aux_node;
                int ind = 0;
                do{
                    next_aux_node = next_node->next.load();
                    next_node = next_aux_node->next.load();
                    ind++;
                } while (next_node->next.load() != nullptr && ind < position + 1);
                std::cout << "value " <<ind << std::endl;
                node_type * new_next_node = new node_type(new int32_t(number));
                node_type * new_next_aux_node = new node_type();
                new_next_aux_node->next.store(next_node);
                new_next_node->next.store(new_next_aux_node);
                next_aux_node->next.store(new_next_node);
            }
            int search(const int& number){
                node_type * next_node = list_head_node.load();
                node_type * next_aux_node;
                int ind = 0;
                do{
                    next_aux_node = next_node->next.load();
                    next_node = next_aux_node->next.load();
                    if (number == *(next_node->data.load().ptr())){
                        return ind;
                    }
                    ind++;
                } while (next_node->next.load() != nullptr);
                return -1;
            }
*/
            bool deleted (const value_type & number){
                node_type * next_node = list_head_node.load();
                node_type * next_aux_node;
                int ind = 0;
                do{
                    next_aux_node = next_node->next.load();
                    next_node = next_aux_node->next.load();
                    if (number == *(next_node->data.load().ptr())){
                        next_aux_node->next.store(next_node->next.load()->next.load());
                        return true;
                    }
                    ind++;
                } while (next_node->next.load() != nullptr);
                return false;
            }
        };

    }
}

#endif //CDS_VALOIS_LIST_H
