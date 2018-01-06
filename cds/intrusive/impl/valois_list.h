//TODO: maybe add DOXYGEN docs

#ifndef CDS_VALOIS_LIST_H
#define CDS_VALOIS_LIST_H

#include <cds/intrusive/details/valois_list_base.h>
#include <cds/details/make_const_type.h>

//CRITICAL: cheak all functions use safe read/write
//CRITICAL: cheak nullptr/NULL values for aux, Head, Tail nodes

namespace cds { namespace intrusive {

        template<class GC, typename T, class Traits>
    class ValoisList{

        friend class iterator;
    public:
        typedef GC                                  gc;         ///< Garbage collector
        typedef T                                   value_type; ///< type of value stored in the list
        typedef Traits                              traits;     ///< Traits template parameter
        typedef valois_list::node< value_type >     node_type;  ///< node type

        typedef typename opt::details::make_comparator< value_type, traits >::type key_comparator;

        typedef typename traits::disposer  disposer;            ///< disposer for \p value_type
        typedef typename traits::back_off       back_off;       ///< back-off strategy
        typedef typename traits::item_counter   item_counter;   ///< Item counting policy used
        typedef typename traits::memory_model   memory_model;   ///< Memory ordering. See \p cds::opt::memory_model option
        typedef typename traits::node_allocator node_allocator; ///< Node allocator
        typedef typename traits::stat           stat;           ///< Internal statistics

        typedef typename gc::template guarded_ptr< value_type > guarded_ptr;    ///< Guarded pointer

        static CDS_CONSTEXPR const size_t c_nHazardPtrCount = 4;    ///< Count of hazard pointer required for the algorithm

    protected:
        typedef typename atomics::atomic< node_type* >  atomic_node_ptr;
        typedef typename node_type::marked_data_ptr     marked_data_ptr;

        atomic_node_ptr m_pHead;        ///< Head pointer
        atomic_node_ptr m_pTail;        ///< Tail pointer
        item_counter    m_ItemCounter;  ///< Item counter

        node_type * m_Head;
        node_type * m_Tail;

    protected:
        //template <bool IsConst>
        class iterator {
            friend class ValoisList;

        protected:
            node_type *m_pNode;         // Valois target - current real node
            node_type *aux_pNode;       // Valois pre_aux - aux node before the real node
            node_type *cell_pNode;      // Valois pre_cell - real node before the current real node
            typename gc::Guard m_Guard;

            bool next() {
                if ( m_pNode->next == nullptr ) {     // if tail
                    return false;
                }

                cell_pNode->next.store( &m_pNode, memory_model::memory_order_consume );
                aux_pNode->next.store( m_pNode->next, memory_model::memory_order_consume );
                update_iterator();
                return true;
            }

            iterator begin(){
                m_pNode->next.store( NULL, memory_model::memory_order_consume );
                aux_pNode->next.store( m_Head->next.load( memory_model::memory_order_seq_cst ), memory_model::memory_order_consume );
                cell_pNode->next.store( m_pHead.load( memory_model::memory_order_seq_cst ), memory_model::memory_order_comsume );
            }

        public:
            typedef typename cds::details::make_const_type<value_type, false>::pointer value_ptr;
            typedef typename cds::details::make_const_type<value_type, false>::reference value_ref;

            iterator()
                    : m_pNode(nullptr), aux_pNode(nullptr), cell_pNode(nullptr) {}

            void update_iterator(){
                if ( aux_pNode->next == m_pNode ){
                    return;
                }
                node_type * p = aux_pNode;
                node_type * n = p->next.load( atomics::memory_order_consume );

                while( n != m_Tail && n->data == NULL ){    //while not last and is aux node
                    cell_pNode->next.compare_exchange_strong( p, n, memory_model::memory_order_release, atomics::memory_order_relaxed );
                    p = n;
                    n = p->next.load( atomics::memory_order_consume );
                }

                aux_pNode = p;
                m_pNode = n;
            }

            value_ptr operator ->() const {
                return m_Guard.template get<value_type>();
            }

            value_ref operator *() const {
                assert( m_Guard.get_native() != nullptr );
                return * m_Guard.template get<value_type>();
            }

            iterator& operator ++(){
                iterator temp  = *this;
                next();
                return temp;
            }

            iterator& operator =( iterator & second ){
                m_pNode = second.m_pNode;
                aux_pNode = second.aux_pNode;
                cell_pNode = second.cell_pNode;
                m_Guard.copy( second.m_Guard );
                return * this;
            }

            bool operator ==( const iterator & second ) const {
                return (m_pNode->data == second.m_pNode->data );
            }

            bool operator !=( const iterator & second ) const {
                return (m_pNode->data != second.m_pNode->data );
            }
        };

    public:
        ValoisList(){
            init_list();
        }

        ~ValoisList(){
            destroy();
        }

        iterator begin(){
            return iterator().begin();
        }

        //TODO: implement insert( value_type & val ), which will make sorted insert

        bool insert( iterator i, value_type & val ){
            i.update_iterator();

            node_type * real_node = new node_type(val);
            node_type * aux_node  = new node_type();

            real_node->next = aux_node;
            aux_node->next = i.m_pNode;

            bool insert_status = i.aux_pNode->next.compare_exchange_strong(i.m_pNode, real_node, memory_model::memory_order_release, memory_model::memory_order_relaxed );
            if( insert_status ){
                ++m_ItemCounter;
            }
            return insert_status;
        }

        bool erase( iterator & i ){
            if( i.cell_pNode != nullptr ){      // if not Head
                while( true ){
                    i.update_iterator();
                    if( delete_node( i ) ){
                        return true;
                    }
                    i.update_iterator();
                }
            }
            return false;
        }

        bool contains( value_type & val ){
            return find( iterator().begin(), val );
        }

        bool find( iterator * i, value_type & val ) {
            while ( i->m_pNode != nullptr ){
                //FIX: safe read
                if ( i->m_pNode.data == val ){
                    return true;
                }
                else if ( i->m_pNode.data > val ){
                    return false;
                }
                else{
                    i->next();
                }
            }
            return false;
        }

        //TODO: implement get_iterator( val )

        bool empty() const {
            return size() == 0;
        }

        size_t size() const {
            return m_ItemCounter.value();
        }

    private:

        void init_list(){
            m_Head->next.store( new node_type(), memory_model::memory_order_relaxed );  //link to aux node
            m_Head->next->next.store( &m_Tail, memory_model::memory_order_relaxed );    //link aux node to tail
            m_Tail->next.store( nullptr, memory_model::memory_order_release );          //link tail to nullptr
        }

        void destroy(){

            node_type * pNode = m_Head.next.load( memory_model::memory_order_relaxed );

            while ( pNode != pNode->next.load( memory_model::memory_order_relaxed )) {
                value_type * pVal = pNode->data.load( memory_model::memory_order_relaxed ).ptr();
                if ( pVal )
                    erase( pNode );
                node_type * pNext = pNode->next.load( memory_model::memory_order_relaxed );
                pNode = pNext;
            }
        }

        bool delete_node( iterator i ){

            node_type * for_delete = i.m_pNode;
            node_type * adjacent = i.m_pNode->next;

            bool delete_status = i.aux_pNode->next.compare_exchange_strong( for_delete, adjacent,
                                                                            memory_model::memory_order_relaxed,
                                                                            memory_model::memory_order_release );

            if ( delete_status ){
                --m_ItemCounter;
            }

            return delete_status;
        }
    };

}}

#endif //CDS_VALOIS_LIST_H
