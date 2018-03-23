// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_DETAILS_BASE_H
#define CDSLIB_INTRUSIVE_DETAILS_BASE_H

#include <cds/intrusive/details/node_traits.h>
#include <cds/details/allocator.h>
#include <cds/algo/backoff_strategy.h>

namespace cds {

/// Intrusive containers
/**
    @ingroup cds_intrusive_containers
    The namespace \p cds::intrusive contains intrusive lock-free containers.
    The idea comes from \p boost::intrusive library, see http://boost.org/doc/ as a good introduction to intrusive approach.
    The intrusive containers of libcds library is developed as close to \p boost::intrusive

    In terms of lock-free approach, the main advantage of intrusive containers is
    that no memory allocation is performed to maintain container elements.
    However, additional requirements are imposed for types and values that can be stored in intrusive container.
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
    If no tag is specified the default \p cds::opt::none will be used.

    \anchor cds_intrusive_item_creating
    \par Inserting items
    Many intrusive and non-intrusive (standard-like) containers in the library have the member functions
    that take a functor argument to initialize the inserted item after it has been successfully inserted,
    for example:
    \code
    template <typename Q, typename Func>
    bool insert( Q& key, Func f );

    template <typename Q, typename Func>
    std::pair<bool, bool> update( Q& key, Func f, bool bAllowInsert = true );
    \endcode
    The first member function calls \p f functor iif a new item has been inserted. The functor takes two parameter: a reference to inserted item and
    \p key.

    The second member function, \p update(), allows to insert a new item to the container if \p key is not found, or to find the item with \p key and
    to perform some action with it. The \p f signature is:
    \code
    void f( bool bNew, item_type& item, Q& key );
    \endcode
    where \p bNew is a flag to indicate whether \p item is a new created node or not.

    Such functions should be used with caution in multi-threaded environment
    since they can cause races. The library does not synchronize access
    to container's items, so many threads can access to one item simultaneously.
    For example, for \p insert member function the following race is possible:
    \code
        // Suppose, Foo is a complex structure with int key field
        SomeContainer<Foo> q;

        Thread 1                                  Thread 2

        q.insert( Foo(5),                         q.find( 5, []( Foo& item ) {
            []( Foo& item ){                         // access to item fields
               // complex initialization             ...
               item.f1 = ...;                     });
               ...
            });
    \endcode
    Execute sequence:
    \code
        Find 5 in the container.
        Key 5 is not found
        Create a new item                         Find key 5
            with calling Foo(5) ctor
        Insert the new item
                                                  The key 5 is found -
                                                     call the functor     (!)
        Perform complex
           initialization -
           call the functor
    \endcode
    (!): Thread 2 found the key and call its functor on incomplete initialized item.
    Simultaneous access to the item also is possible. In this case Thread 1 is
    initializing the item, thread 2 is reading (or writing) the item's fields.
    In any case, Thread 2 can read uninitialized or incomplete initialized fields.

    \p update() member function race. Suppose, thread 1 and thread 2 perform
    the
    following code:
    \code
        q.update( 5, []( bool bNew, Foo& item, int  arg )
           {
              // bNew: true if the new element has been created
              //       false otherwise
              if ( bNew ) {
                 // initialize item
                 item.f1=...;
                 //...
              }
              else {
                 // do some work
                 if ( !item.f1 )
                    item.f1 = ...;
                 else {
                   //...
                 }
                 //...
              }
           }
        );
    \endcode
    Execute sequence:
    \code
        Thread 1                                  Thread 2
        key 5 not found
        insert new item Foo(5)                    Find 5
                                                  Key 5 found
                                                  call the functor with
                                                     bNew = false        (!)
        call the functor with
           bNew = true
    \endcode
    (!): Thread 2 executes its functor on incomplete initialized item.

    To protect your code from such races you can use some item-level synchronization,
    for example:
    \code
    struct Foo {
       spinlock lock;       // item-level lock
       bool initialized = false;    // initialization flag
       // other fields
       // ....
    };

    q.update( 5, []( bool bNew, Foo& item, int  arg )
        {
            // Lock access to the item
            std::unique_lock( item.lock );

            if ( !item.initialized ) {
                // initialize item
                item.f1=...;
                //...
                item.initialized = true; // mark the item as initialized
            }
            else {
                // do some work
                if ( !item.f1 )
                    item.f1 = ...;
                else {
                    //...
                }
                //...
            }
        }
    );
    \endcode
    If the item-level synchronization is not suitable, you should not use any inserting member function
    with post-insert functor argument.

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
    by publishing it as a "hazard" i.e. as a pointer that is changing at the current time and cannot be
    deleted at this moment. In intrusive container paradigm, the pointer to a node of the container
    and the pointer to a item stored in the container are not equal in the general case.
    However, any pointer to node should be castable to appropriate pointer to container's item.
    In general, any item can be placed to two or more intrusive containers simultaneously,
    and each of those container holds an unique pointer to its node that refers to the same item.
    When we protect a pointer, we want to protect an <b>item</b> pointer that is the invariant
    for any container stored that item. In your intrusive container, instead of protecting by GC's guard a pointer to node
    you should cast it to the pointer to item and then protect that item pointer.
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
    /** @defgroup cds_intrusive_freelist Free-list
        @ingroup cds_intrusive_containers
    */

    //@cond
    class iterable_list_tag
    {};

    template <typename List>
    struct is_iterable_list: public std::is_base_of< iterable_list_tag, List>
    {};
    //@endcond

}} // namespace cds::intrusuve

#endif  // #ifndef CDSLIB_INTRUSIVE_DETAILS_BASE_H
