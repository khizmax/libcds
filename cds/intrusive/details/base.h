//$$CDS-header$$

#ifndef __CDS_INTRUSIVE_DETAILS_BASE_H
#define __CDS_INTRUSIVE_DETAILS_BASE_H

#include <cds/intrusive/details/node_traits.h>
#include <cds/details/allocator.h>
#include <cds/algo/backoff_strategy.h>

namespace cds {

/// Intrusive containers
/**
    @ingroup cds_intrusive_containers
    The namespace cds::intrusive contains intrusive lock-free containers.
    The idea comes from \p boost::intrusive library, see http://boost.org/doc/ as a good introduction to intrusive approach.
    The intrusive containers of libcds library is developed as close to \p boost::intrusive

    In terms of lock-free approach, the main advantage of intrusive containers is
    that no memory allocation is performed to maintain container items.
    However, additional requirements is imposed for types and values that can be stored in intrusive container.
    See the container documentation for details.

    \anchor cds_intrusive_hook_tag
    \par Tags
    Many hooks and nodes for intrusive containers contain template argument \p Tag.
    This argument serves as a tag, so you can derive from more than one container's node and hence put an object in multiple intrusive containers 
    at the same time. An incomplete type can serve as a tag. If you specify two hooks, you must specify a different tag for each one. 
    Example: 
    \code
    struct tag1;
    cds::intrusive::treiber_stack::node< cds::gc::HP, tag<tag1> > 
    \endcode
    If no tag is specified еру default \p cds::opt::none will be used.

    \anchor cds_intrusive_item_destroying
    \par Destroying items

    It should be very careful when destroying an item removed from intrusive container.
    In other threads the references to popped item may exists some time after removing.
    To destroy the removed item in thread-safe manner you should call static function \p retire
    of garbage collector you use, for example:
    \code
    struct destroyer  {
        void operator ()( my_type * p )
        {
            delete p;
        }
    };

    typedef cds::intrusive::TreiberStack< cds::gc::HP, my_type, cds::opt::disposer< destroyer > > stack;
    stack s;

    // ....

    my_type * p = s.pop();

    if ( p ) {
        // It is wrong
        // delete p;

        // It is correct
        cds::gc:HP::retire< destroyer >( p );
    }
    \endcode
    The situation becomes even more complicated when you want store items in different intrusive containers.
    In this case the best way is using reference counting:
    \code
    struct my_type {
        ...
        std::atomic<unsigned int> nRefCount;

        my_type()
            : nRefCount(0)
        {}
    };

    struct destroyer  {
        void operator ()( my_type * p )
        {
            if ( --p->nRefCount == 0 )
                delete p    ;   // delete only after no reference pointing to p
        }
    };

    typedef cds::intrusive::TreiberStack< cds::gc::HP, my_type, cds::opt::disposer< destroyer > > stack;
    typedef cds::intrusive::MSQueue< cds::gc::HP, my_type, cds::opt::disposer< destroyer > > queue;
    stack s;
    queue q;

    my_type * v = new my_type();

    v.nRefCount++   ; // increment counter before pushing the item to the stack
    s.push(v);

    v.nRefCount++   ; // increment counter before pushing the item to the queue
    q.push(v);

    // ....

    my_type * ps = s.pop();
    if ( ps ) {
        // It is wrong
        // delete ps;

        // It is correct
        cds::gc:HP::retire< destroyer >( ps );
    }

    my_type * pq = q.pop();
    if ( pq ) {
        // It is wrong
        // delete pq;

        // It is correct
        cds::gc:HP::retire< destroyer >( pq );
    }
    \endcode
    Violation of these rules may lead to a crash.

    \par Intrusive containers and Hazard Pointer-like garbage collectors

    If you develop your intrusive container based on <b>libcds</b> library framework, you should
    take in the account the following.
    The main idea of garbage collectors (GC) based on Hazard Pointer schema is protecting a shared pointer
    by publishing it as a "hazard" one i.e. as a pointer that is changing at the current time and cannot be
    deleted at this moment. In intrusive container paradigm, the pointer to the node of the container
    and the pointer to the item stored in the container are not equal in the general case.
    However, any pointer to the node should be castable to the appropriate pointer to the container's item.
    In general, any item can be placed to some different intrusive containers simultaneously,
    and each of those container holds a unique pointer to its node that refers to the same item.
    When we protect a pointer, we want to protect an <b>item</b> pointer that is the invariant
    for any container stored that item. In your intrusive container, instead of protecting by GC's guard a pointer to an node
    you should convert it to the pointer to the item and then protect resulting item pointer.
    Otherwise an unpredictable result may occur.
*/
namespace intrusive {

    /// @defgroup cds_intrusive_containers Intrusive containers
    /** @defgroup cds_intrusive_helper Helper structs for intrusive containers
        @ingroup cds_intrusive_containers
    */
    /** @defgroup cds_intrusive_stack Stack
        @ingroup cds_intrusive_containers
    */
    /** @defgroup cds_intrusive_queue Queue
        @ingroup cds_intrusive_containers
    */
    /** @defgroup cds_intrusive_priority_queue Priority queue
        @ingroup cds_intrusive_containers
    */
    /** @defgroup cds_intrusive_deque Deque
        @ingroup cds_intrusive_containers
    */
    /** @defgroup cds_intrusive_map Set
        @ingroup cds_intrusive_containers
    */
    /** @defgroup cds_intrusive_tree Tree
        @ingroup cds_intrusive_containers
    */
    /** @defgroup cds_intrusive_list List
        @ingroup cds_intrusive_containers
    */

}} // namespace cds::intrusuve

#endif  // #ifndef __CDS_INTRUSIVE_DETAILS_BASE_H
