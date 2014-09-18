//$$CDS-header$$

#ifndef __CDS_INTRUSIVE_DETAILS_DUMMY_NODE_HOLDER_H
#define __CDS_INTRUSIVE_DETAILS_DUMMY_NODE_HOLDER_H

#include <cds/gc/gc_fwd.h>
#include <cds/details/allocator.h>

//@cond
namespace cds { namespace intrusive { namespace details {

    template <typename GC, typename Node, typename Alloc = CDS_DEFAULT_ALLOCATOR>
    class dummy_node: public Node
    {
        typedef Node    node_type;

        node_type       m_Dummy;
    public:
        node_type *     get()
        {
            return &m_Dummy;
        }

        void retire()
        {}
    };

    template <typename Node, typename Alloc>
    class dummy_node< cds::gc::HRC, Node, Alloc >
    {
        typedef Node    node_type;
        typedef cds::gc::HRC    gc;
        typedef cds::details::Allocator< node_type, Alloc> allocator_type;

        node_type *     m_pDummy;

        struct dummy_node_disposer {
            void operator()( node_type * p )
            {
                assert( p != null_ptr<node_type *>());

                p->m_pNext.store( null_ptr<node_type *>(), CDS_ATOMIC::memory_order_release );
                allocator_type().Delete( p );
            }
        };

    public:
        dummy_node()
            : m_pDummy( allocator_type().New() )
        {}

        node_type *     get()
        {
            return m_pDummy;
        }

        void retire()
        {
            gc::template retire<dummy_node_disposer>( m_pDummy );
        }
    };

}}} // namepace cds::intrusive::details

//@endcond

#endif // #ifndef __CDS_INTRUSIVE_DETAILS_DUMMY_NODE_HOLDER_H
