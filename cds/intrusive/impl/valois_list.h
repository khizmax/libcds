//

#ifndef CDS_VALOIS_LIST_H
#define CDS_VALOIS_LIST_H

#include <cds/intrusive/details/base.h>
#include <cds/gc/default_gc.h>
#include <cds/algo/atomic.h>

namespace cds
{
    namespace intrusive{
        namespace valois_list{

            struct traits
            {
                /// Key comparison functor
                /**
                    No default functor is provided. If the option is not specified, the \p less is used.
                */
                typedef opt::none                       compare;

                /// Specifies binary predicate used for key compare.
                /**
                    Default is \p std::less<T>
                */
                typedef opt::none                       less;

                /// Node allocator
                typedef CDS_DEFAULT_ALLOCATOR           node_allocator;

                /// Back-off strategy
                typedef cds::backoff::Default           back_off;

                /// Disposer for removing items
                typedef opt::v::empty_disposer          disposer;


                /// Item counting feature; by default, disabled. Use \p cds::atomicity::item_counter or \p atomicity::cache_friendly_item_counter to enable item counting
                typedef atomicity::empty_item_counter   item_counter;

                /// C++ memory ordering model
                /**
                    Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                    or \p opt::v::sequential_consistent (sequentially consisnent memory model).
                */
                typedef opt::v::relaxed_ordering        memory_model;
            };

            template <typename T>
            struct node
            {
                typedef T value_type; ///< Value type
                typedef cds::details::marked_ptr<T, 1>   marked_data_ptr; ///< marked pointer to the value

                atomics::atomic< node* >            next;  ///< pointer to next node in the list
                atomics::atomic< node* >            prev;  ///< pointer to previous node in the list
                atomics::atomic< marked_data_ptr >  data;  ///< pointer to user data, \p nullptr if the node is free

                node()
                {
                    next.store( nullptr, atomics::memory_order_release );
                    prev.store( nullptr, atomics::memory_order_release );
                    data.store( marked_data_ptr(), atomics::memory_order_release );
                }

                node( value_type * pVal )
                {
                    next.store( nullptr, atomics::memory_order_release );
                    prev.store( nullptr, atomics::memory_order_release );
                    data.store( marked_data_ptr( pVal ), atomics::memory_order_release );
                }

            };

            template<class CG, typename T, class Traits = valois_list::traits>
            class ValoisList{
            public:

                typedef T       value_type; ///< type of value stored in the list
                typedef Traits  traits;     ///< Traits template parameter

                typedef valois_list::node< value_type > node_type; ///< node type

                typedef typename opt::details::make_comparator< value_type, traits >::type key_comparator;

                typedef typename traits::disposer  disposer; ///< disposer for \p value_type

                typedef GC  gc;   ///< Garbage collector
                typedef typename traits::back_off       back_off;       ///< back-off strategy
                typedef typename traits::item_counter   item_counter;   ///< Item counting policy used
                typedef typename traits::memory_model   memory_model;   ///< Memory ordering. See \p cds::opt::memory_model option
                typedef typename traits::node_allocator node_allocator; ///< Node allocator
                typedef typename traits::stat           stat;           ///< Internal statistics

                typedef typename gc::template guarded_ptr< value_type > guarded_ptr; ///< Guarded pointer

                static constexpr const size_t c_nHazardPtrCount = 4; ///< Count of hazard pointer required for the algorithm

            protected:
                node_type *head;
                node_type *tail;

                typedef atomics::atomic< node_type* > atomic_node_ptr;  ///< Atomic node pointer
                typedef atomic_node_ptr               auxiliary_head;   ///< Auxiliary head type (for split-list support)
                typedef typename node_type::marked_data_ptr marked_data_ptr;

                item_counter    m_ItemCounter;  ///< Item counter

                typedef cds::details::Allocator< node_type, node_allocator > cxx_node_allocator;

                /// Position pointer for item search
                struct position {
                    node_type const*  pHead;
                    node_type *       pPrev;  ///< Previous node
                    node_type *       pCur;   ///< Current node

                    value_type *      pFound;       ///< Value of \p pCur->data, valid only if data found

                    typename gc::Guard guard;       ///< guard for \p pFound
                };

                struct insert_position: public position
                {
                    value_type *        pPrevVal;     ///< Value of \p pPrev->data, can be \p nullptr
                    typename gc::Guard  prevGuard;   ///< guard for \p pPrevVal
                };

            protected:
                class iterator
                {
                    friend class ValoisList;
                protected:
                    node_type * consider_node;

                    iterator(node_type * consider_node){
                        if (!m_Guard.protect(consider_node->data, []( marked_data_ptr p ) {return p.ptr(); }).ptr())
                            next();
                    }

                    void next()
                    {
                        for ( node_type* p = consider_node->next.load( memory_model::memory_order_relaxed ); p != m_pNode; p = p->next.load( memory_model::memory_order_relaxed ))
                        {
                            m_pNode = p;
                            if ( m_Guard.protect( p->data, []( marked_data_ptr ptr ) { return ptr.ptr(); }).ptr())
                                return;
                        }
                        m_Guard.clear();
                    }

                    value_type* data() const
                    {
                        return m_Guard.template get<value_type>();
                    }

                };

            public:
                ValoisList(){
                    head.next.store( &tail, memory_model::memory_order_relaxed );
                    tail.next.store( NULL, memory_model::memory_order_release );
                }
                ~ValoisList(){
                    node_type * consider_node = m_Head.next.load( memory_model::memory_order_relaxed );
                    while ( consider_node != consider_node->next.load( memory_model::memory_order_relaxed )) {
                        value_type * pVal = consider_node->data.load( memory_model::memory_order_relaxed ).ptr();
                        if ( pVal )
                            retire_data( pVal );
                        node_type * pNext = consider_node->next.load( memory_model::memory_order_relaxed );
                        delete_node( consider_node );
                        consider_node = pNext;
                    }
                }

                iterator begin(){
                    return iterator(&head);
                }

                iterator end(){
                    return iterator(&tail);
                }

                bool insert( node_type * consider_node ){

                    assert( consider_node != nullptr );
                    assert( consider_node->data.load( memory_model::memory_order_relaxed ) != nullptr );

                    insert_position pos;

                    pos.pHead = pHead;
                    node_type*  pPrev = const_cast<node_type*>(pHead);
                    value_type* pPrevVal = pPrev->data.load( memory_model::memory_order_relaxed ).ptr();

                    node_type * pCur = pPrev->next.load( memory_model::memory_order_relaxed );


                    value_type * pVal = pos.guard.protect( pCur->data,
                                                           []( marked_data_ptr p ) -> value_type*
                                                           {
                                                               return p.ptr();
                                                           } ).ptr();


                    pPrev = pCur;
                    pPrevVal = pVal;
                    pos.prevGuard.copy( pos.guard );

//                    if ( connect_node(consider_node) ) {
                        ++m_ItemCounter;
                        return true;
//                    }
                }

                bool update(){
                    return true;
                }

                bool erase(node_type * consider_node){

                    assert( consider_node != nullptr );
                    assert( consider_node->data.load( memory_model::memory_order_relaxed ) != nullptr );

                    return true;
                }


            protected:

                node_type* head(){
                    return head;
                }

                node_type* tail(){
                    return tail;
                }

            private:

                node_type * alloc_node( value_type * pVal ){
                    return cxx_node_allocator().New( pVal );
                }

                void delete_node( node_type * consider_node ){
                    cxx_node_allocator().Delete( consider_node );
                }

                void retire_data( value_type * pVal ){
                    assert( pVal != nullptr );
                    gc::template retire<disposer>( pVal );
                }

            };
        }
    }
}

#endif //CDS_VALOIS_LIST_H
