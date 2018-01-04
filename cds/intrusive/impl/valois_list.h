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
                    data.store( marked_data_ptr(), atomics::memory_order_release );
                }

                node( value_type * pVal )
                {
                    next.store( nullptr, atomics::memory_order_release );
                    data.store( marked_data_ptr( pVal ), atomics::memory_order_release );
                }

            };

            template<class CG, typename T, class Traits>
            class ValoisList{
            public:

                typedef T       value_type; ///< type of value stored in the list
                typedef Traits  traits;     ///< Traits template parameter

//                typedef typename traits::hook    hook;      ///< hook type
                typedef valois_list::node< value_type > node_type; ///< node type

                typedef typename opt::details::make_comparator< value_type, traits >::type key_comparator;

                typedef typename traits::disposer  disposer; ///< disposer used
//                typedef typename traits::stat      stat;     ///< Internal statistics
//                typedef typename get_node_traits< value_type, node_type, hook>::type node_traits ;    ///< node traits
//                typedef typename michael_list::get_link_checker< node_type, traits::link_checker >::type link_checker;   ///< link checker

                typedef GC  gc;   ///< Garbage collector
                typedef typename traits::back_off       back_off;       ///< back-off strategy
                typedef typename traits::item_counter   item_counter;   ///< Item counting policy used
                typedef typename traits::memory_model   memory_model;   ///< Memory ordering. See \p cds::opt::memory_model option
//                typedef typename traits::node_allocator node_allocator; ///< Node allocator

                typedef typename gc::template guarded_ptr< value_type > guarded_ptr; ///< Guarded pointer

                static CDS_CONSTEXPR const size_t c_nHazardPtrCount = 4; ///< Count of hazard pointer required for the algorithm

            protected:
//                typedef typename node_type::atomic_marked_ptr   atomic_node_ptr;   ///< Atomic node pointer
//                typedef typename node_type::marked_ptr          marked_node_ptr;   ///< Node marked pointer
//
//                atomic_node_ptr m_pHead;        ///< Head pointer
                item_counter    m_ItemCounter;  ///< Item counter
//
//                node_type *head;
//                node_type *tail;

//                typedef atomics::atomic< node_type* > atomic_node_ptr;  ///< Atomic node pointer
//                typedef atomic_node_ptr               auxiliary_head;   ///< Auxiliary head type (for split-list support)
//                typedef typename node_type::marked_data_ptr marked_data_ptr;


//                typedef cds::details::Allocator< node_type, node_allocator > cxx_node_allocator;

                /// Position pointer for item search
//                struct position {
//                    node_type const*  pHead;
//                    node_type *       pPrev;  ///< Previous node
//                    node_type *       pCur;   ///< Current node
//
//                    value_type *      pFound;       ///< Value of \p pCur->data, valid only if data found
//
//                    typename gc::Guard guard;       ///< guard for \p pFound
//                };
//
//                struct insert_position: public position
//                {
//                    value_type *        pPrevVal;     ///< Value of \p pPrev->data, can be \p nullptr
//                    typename gc::Guard  prevGuard;   ///< guard for \p pPrevVal
//                };

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
                    init_list();
                }

                ~ValoisList(){
                    destroy();
                }

                iterator begin(){
                    return iterator( &head );
                }

                iterator end(){
                    return iterator( &tail );
                }

                bool insert( value_type &val ){

                    return true;
                }
//TODO: template Q - key
                bool erase(  ){

                    return true;
                }

                bool empty(){
                    return size() == 0;
                }

                size_t size() const{
                    return m_ItemCounter.value();
                }

            protected:

                node_type* head(){
                    return head;
                }

                node_type* tail(){
                    return tail;
                }

            private:

                void init_list(){
                    head.next.store( &tail, memory_model::memory_order_relaxed );
                    tail.next.store( NULL, memory_model::memory_order_release );
                }

                void destroy(){
                    typename gc::Guard guard;
                    marked_node_ptr head;
                    while ( true ) {
                        head = m_pHead.load(memory_model::memory_order_relaxed);
                        if ( head.ptr())
                            guard.assign( node_traits::to_value_ptr( *head.ptr()));
                        if ( cds_likely( m_pHead.load(memory_model::memory_order_acquire) == head )) {
                            if ( head.ptr() == nullptr )
                                break;
                            value_type& val = *node_traits::to_value_ptr( *head.ptr());
                            unlink( val );
                        }
                    }
                }


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
