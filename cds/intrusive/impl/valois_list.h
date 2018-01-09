//TODO: maybe add DOXYGEN docs

#ifndef CDS_VALOIS_LIST_H
#define CDS_VALOIS_LIST_H

#include <cds/intrusive/details/valois_list_base.h>
#include <cds/details/make_const_type.h>

//CRITICAL: cheak all functions use safe read/write
//CRITICAL: cheak nullptr/NULL values for aux, Head, Tail nodes

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

            typedef typename gc::template guarded_ptr<value_type> guarded_ptr;    ///< Guarded pointer

            static CDS_CONSTEXPR const size_t c_nHazardPtrCount = 4;    ///< Count of hazard pointer required for the algorithm

        protected:
            typedef typename atomics::atomic<node_type *> atomic_node_ptr;
            typedef typename node_type::marked_data_ptr marked_data_ptr;

            atomic_node_ptr m_pHead;        ///< Head pointer
            atomic_node_ptr m_pTail;        ///< Tail pointer

        public:
            friend class iterator;

        protected:
            //template <bool IsConst>
            class iterator {
                friend class ValoisList;

            protected:
                node_type * current_node;   // Valois target - current real node
                node_type * aux_pNode;      // Valois pre_aux - aux node before the real node
                node_type * prev_node;      // Valois pre_cell - real node before the current real node
                typename gc::Guard m_Guard;

                bool next() {
                    //std::cout <<"hello from next "<<std::endl;
                    if (current_node->next == nullptr) {     // if tail
                        return false;
                    }

                    int * value;


//                    if (prev_node->data.load() != nullptr){
//                        value = prev_node->data.load().ptr();
//                        std::cout <<"in next 1 "<< *value <<std::endl;
//                    } else{
//                        std::cout <<"in next 1 NULL" <<std::endl;
//                    }

//                    if (current_node->data.load() != nullptr){
//                        value = current_node->data.load().ptr();
//                        std::cout << "current node " << *value <<std::endl;
//                        //std::cout << "next aux node " << current_node->next <<std::endl;
//                    } else{
//                        std::cout <<"current data is NULL" <<std::endl;
//                    }

                    node_type * new_current_node = new node_type();
                    new_current_node->data.store(current_node->data.load());
                    new_current_node->next.store(current_node->next.load());

                    prev_node = new_current_node;

                    new_current_node = new node_type();
                    new_current_node->data.store(current_node->next.load()->data.load());
                    new_current_node->next.store(current_node->next.load()->next.load());

                    aux_pNode = new_current_node;

                    current_node = aux_pNode->next.load();



                    //update_iterator();

//                    if (prev_node->data.load() != nullptr) {
//                        value = prev_node->data.load().ptr();
//                        std::cout << "in next 2 " << *value << std::endl;
//                    } else{
//                        std::cout <<"in next 2 NULL" <<std::endl;
//                    }
//
//                    std::cout <<"bye from next" <<std::endl;
                    return true;
                }



            public:
                typedef typename cds::details::make_const_type<value_type, false>::pointer value_ptr;
                typedef typename cds::details::make_const_type<value_type, false>::reference value_ref;

                iterator()
                        : current_node(nullptr), aux_pNode(nullptr), prev_node(nullptr) {}

                // node - only m_Head
                iterator( node_type * node) {

                    current_node = new node_type();
                    aux_pNode = new node_type();
                    prev_node = new node_type();

                    aux_pNode->next.store(
                            node->next.load(memory_model::memory_order_seq_cst),
                            memory_model::memory_order_seq_cst
                    );

                    prev_node->next.store(
                            node,
                            memory_model::memory_order_seq_cst
                    );

                    node_type * tempNode = aux_pNode->next.load();
                     if ( tempNode->next.load() == nullptr ){
                        current_node->next.store(NULL, memory_model::memory_order_seq_cst);
                    }
                    else{
                        current_node->next.store( tempNode->next.load(), memory_model::memory_order_relaxed);
                    }

                    update_iterator();
                }

                void update_iterator() {
                    //std::cout << "\t in update iterator " << std::endl;
                    if (aux_pNode->next == current_node) {
                        return;
                    }
                    int * data;

                    node_type * p = aux_pNode;
                    node_type * n = p->next.load(atomics::memory_order_seq_cst);

                    data = n->next.load()->data.load().ptr();
                    //std::cout << "\t data 1 is "<< (n->data.load().ptr()) << " -> " << data << std::endl;
                    int tmp = 0;

                    while ( n->next != nullptr && n->data == nullptr) {    //while not last and is aux node
                        tmp++;

                        prev_node->next.compare_exchange_strong(
                                p,
                                n,
                                atomics::memory_order_seq_cst,
                                atomics::memory_order_seq_cst
                        );
                        p = n;
                        n = p->next.load(atomics::memory_order_seq_cst);
                    }

//                    std::cout << "\t count in while " <<tmp << std::endl;
                    //std::exit(EXIT_FAILURE);
                    data = n->data.load().ptr();
//                    std::cout << "\t data 2 is " << data << std::endl;

                    aux_pNode = p;
                    current_node = n;
//                    std::cout << "\t bye update iterator " << std::endl;
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

            /**
             * DEBUG ONLY
             * @param number
             */

            ValoisList(int number) {
                node_type * aux_temp = new node_type();
                m_pHead.store(new node_type);
                m_pHead.load()->next.store( aux_temp, memory_model::memory_order_seq_cst);  //link to aux node
                m_pTail.store(new node_type);
                aux_temp->next.store( m_pTail, memory_model::memory_order_seq_cst );
                m_pTail.load()->next.store(nullptr, memory_model::memory_order_seq_cst );          //link tail to nullptr


                for(int i = 0 ; i<number ;i++){
                    append(&i);
                }
            }

            ~ValoisList() {
                destroy();
            }

            iterator begin() {
                return iterator(m_pHead);
            }

            /**
             * try insert in the position
             * @param i
             * @param val
             * @return
             */

            template <typename Q, typename Compare >
            bool search_insert(node_type * start_node /*it not used*/, Q* val, Compare cmp) {

                iterator * mIter = new iterator(m_pHead);
                while (mIter->current_node->next.load() != nullptr ) {
                    value_type *nVal = mIter->current_node->data.load( memory_model::memory_order_seq_cst ).ptr();

                    int const nCmp = cmp(*val, *nVal);
                    /*std::cout << "search_insert : finded value "<< *nVal << std::endl;
                    std::cout << "search_insert : inserted value "<< *val << std::endl;
                    std::cout << "search_insert : compare index "<< nCmp << std::endl;*/
                    if (nCmp == 0) {
                        delete mIter;
                        return true;
                    } else if (nCmp > 0) {


                        bool k = try_insert(mIter, val);
                        delete mIter;
                        return k;
                    } else {
                        mIter->next();
                    }
                }

                bool k = try_insert(mIter, val);
                delete mIter;
                return k;
            }

            bool try_insert(iterator *i, value_type * val) {
                //i->update_iterator();
                //std::cout << "try insert "<< *val << std::endl;

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

                //i->update_iterator();
                return insert_status;
            }


            bool insert( value_type &val ){
                return search_insert( m_pHead, &val, key_comparator() );
            }


            /**
             * Print all node in console
             * this function for debug only
             */

            void print_all_by_iterator(){
                iterator * i = new iterator(m_pHead);

                std::cout << "----------start print by iterator---------------" << std::endl;
                while(i->current_node->next.load() != nullptr){
                    value_type * nVal = i->current_node->data.load(memory_model::memory_order_seq_cst ).ptr();
                    std::cout << *nVal << std::endl;
                    i->next();
                }
                std::cout << "----------end---------------" << std::endl;

                delete i;
            }

            void print_all_by_link(){
                std::cout << "----------start print by link---------------" << std::endl;

                node_type * next_node = m_pHead.load();
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

            /**
             * delete value from linked list;
             * @param value
             * @return
             */
            template <typename Q, typename Compare >
            bool erase(Q* val, Compare cmp) {
                iterator * i = new iterator( m_pHead );
                //search node
                while (i->current_node->next.load() != nullptr ) {
                    value_type * nVal = i->current_node->data.load(memory_model::memory_order_seq_cst ).ptr();
                    int const nCmp = cmp( *val , *nVal );

                    if ( nCmp == 0 ){
                        // hard delete operation
                        while (!try_erase(i)) {}
                        delete i;
                        return true;
                    }
                    else if ( nCmp > 0 ){
                        delete i;
                        return true;
                    }
                    else{
                        i->next();
                    }
                }

                delete i;
                return true;
            }

            bool erase(value_type val){
                return erase(&val, key_comparator());
            }



            template <typename Q, typename Compare >
            bool find( Q* val, Compare cmp) {
                iterator * i = new iterator( m_pHead );
                while (i->current_node->next.load() != nullptr ) {
                    value_type * nVal = i->current_node->data.load(memory_model::memory_order_seq_cst ).ptr();
                    int const nCmp = cmp( *val , *nVal );

                    if ( nCmp == 0 ){
                        delete i;
                        return true;
                    }
                    else if ( nCmp > 0 ){
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

            bool find(value_type &val) {
                return find( &val, key_comparator() );
            }

            bool contains(value_type &val) {
                return find( &val, key_comparator() );
            }

            bool empty() {
                /*std::cout << "hello from empty" << std::endl;*/
                iterator * i = new iterator(m_pHead);

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
                m_pHead.store(new node_type);
                m_pHead.load()->next.store( aux_temp, memory_model::memory_order_seq_cst);  //link to aux node
                m_pTail.store(new node_type);
                aux_temp->next.store( m_pTail, memory_model::memory_order_seq_cst );
                m_pTail.load()->next.store(nullptr, memory_model::memory_order_seq_cst );          //link tail to nullptr

            }

            void destroy() {
                //TODO: fix destroy()
                //node_type *tNode = m_Head->next.load(memory_model::memory_order_relaxed);
                //
                //while (tNode != tNode->next.load(memory_model::memory_order_relaxed)) {
                //    value_type *pVal = tNode->data.load(memory_model::memory_order_relaxed).ptr();
                //    if (pVal)
                //        erase(tNode);
                //    node_type *pNext = tNode->next.load(memory_model::memory_order_relaxed);
                //    tNode = pNext;
                //}
            }

            bool try_erase(iterator *i) {

                node_type *for_delete = i->current_node;
                node_type *adjacent = i->current_node->next;

                bool delete_status = i->aux_pNode->next
                        .compare_exchange_strong(
                                for_delete,
                                adjacent,
                                memory_model::memory_order_seq_cst,
                                memory_model::memory_order_seq_cst);

                return delete_status;
            }


            /**
             * Debug only
             * @param number
             */

            void append(int * number){
                node_type * next_node = m_pHead.load();
                node_type * next_aux_node;
                //std::cout << "iter " << *number << std::endl;
                do{
                    next_aux_node = next_node->next.load();
                    next_node = next_aux_node->next.load();
                    //std::cout << "next_aux_node -> " <<next_aux_node << std::endl;
                    //std::cout << "next_node -> " <<next_aux_node->next.load() << std::endl;
                    //std::cout << "next_node.next -> " <<next_node->next.load() << std::endl << std::endl;

                } while (next_node->next.load() != nullptr);

                //std::cout << number << " -> " << *number << std::endl;


                int d = *number;
                int * index = new int32_t(d);
                //std::cout<< "wtf wtf " <<number << " " << index << std::endl;
                node_type * new_next_node = new node_type(index);
                node_type * new_next_aux_node = new node_type();

                new_next_aux_node->next.store(next_node);
                //std::cout << "append 5 " <<new_next_node << std::endl;
                new_next_node->next.store(new_next_aux_node);
                //std::cout << "append 6 " <<next_node << std::endl;
                next_aux_node->next.store(new_next_node);

                //std::cout << "finish" << std::endl << std::endl;
            }
        };

    }
}

#endif //CDS_VALOIS_LIST_H