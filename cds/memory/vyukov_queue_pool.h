// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_MEMORY_VYUKOV_QUEUE_ALLOCATOR_H
#define CDSLIB_MEMORY_VYUKOV_QUEUE_ALLOCATOR_H

#include <cds/details/allocator.h>
#include <cds/intrusive/vyukov_mpmc_cycle_queue.h>
#include <cds/details/throw_exception.h>
#include <memory>

namespace cds { namespace memory {

    /// \p vyukov_queue_pool traits
    /** @ingroup cds_memory_pool
    */
    struct vyukov_queue_pool_traits : public cds::intrusive::vyukov_queue::traits
    {
        /// Allocator type
        typedef CDS_DEFAULT_ALLOCATOR allocator;
    };

    /// Free-list based on bounded lock-free queue \p cds::intrusive::VyukovMPMCCycleQueue
    /** @ingroup cds_memory_pool
        Template parameters:
        - \p T - the type of object maintaining by free-list. \p T must be default constructible.
        - \p Traits - traits for \p cds::intrusive::VyukovMPMCCycleQueue class plus
            \p cds::opt::allocator option, defaul is \p vyukov_queue_pool_traits

        \b Internals

        This free-list is very simple.
        At construction time, the free-list allocates the array of N items
        and stores them into queue, where N is the queue capacity.
        When allocating the free-list tries to pop an object from
        internal queue i.e. from preallocated pool. If success the popped object is returned.
        Otherwise a new one is allocated. When deallocating, the free-list checks whether
        the object is from preallocated pool. If so, the object is pushed into queue, otherwise
        it is deallocated by using the allocator provided.
        The pool can manage more than \p N items but only \p N items is contained in the free-list.

        \b Usage

        \p %vyukov_queue_pool should be used together with \ref pool_allocator.
        You should declare an static object of type \p %vyukov_queue_pool, provide
        an accessor to that object and use \p pool_allocator as an allocator:
        \code
        #include <cds/memory/vyukov_queue_pool.h>
        #include <cds/memory/pool_allocator.h>

        // Pool of Foo object of size 1024.
        struct pool_traits: public cds::memory::vyukov_queue_pool_traits
        {
            typedef cds::opt::v::uninitialized_static_buffer< Foo, 1024 > buffer;
        };
        typedef cds::memory::vyukov_queue_pool< Foo, pool_traits > pool_type;
        static pool_type thePool;

        struct pool_accessor {
            typedef typename pool_type::value_type  value_type;

            pool_type& operator()() const
            {
                return thePool;
            }
        };

        // Declare pool allocator
        typedef cds::memory::pool_allocator< Foo, pool_accessor >   pool_allocator;

        // Use pool_allocator
        // Allocate an object
        Foo * p = pool_allocator().allocate( 1 );

        // construct object
        new(p) Foo;

        //...

        // Destruct object
        p->~Foo();

        // Deallocate object
        pool_allocator().deallocate( p , 1 );
        \endcode
    */
    template <typename T, typename Traits = vyukov_queue_pool_traits >
    class vyukov_queue_pool
    {
    public:
        typedef cds::intrusive::VyukovMPMCCycleQueue< T, Traits > queue_type  ;   ///< Queue type

    public:
        typedef T  value_type ; ///< Value type
        typedef Traits traits;  ///< Traits type
        typedef typename std::allocator_traits<typename traits::allocator>::template rebind_alloc<value_type> allocator_type;   ///< allocator type
        typedef typename traits::back_off back_off; ///< back-off strategy

    protected:
        //@cond
        typedef cds::details::Allocator< value_type, allocator_type >   cxx_allocator;
        typedef typename cxx_allocator::allocator_type std_allocator;

        queue_type      m_Queue;
        value_type *    m_pFirst;
        value_type *    m_pLast;
        //@endcond

    protected:
        //@cond
        void preallocate_pool()
        {
            m_pFirst = std_allocator().allocate( m_Queue.capacity());
            m_pLast = m_pFirst + m_Queue.capacity();

            for ( value_type * p = m_pFirst; p < m_pLast; ++p ) {
                CDS_VERIFY( m_Queue.push( *p )) ;   // must be true
            }
        }

        bool from_pool( value_type * p ) const
        {
            return m_pFirst <= p && p < m_pLast;
        }
        //@endcond

    public:
        /// Preallocates the pool of object
        /**
            \p nCapacity argument is the queue capacity. It should be passed
            if the queue is based on dynamically-allocated buffer.
            See \p cds::intrusive::VyukovMPMCCycleQueue for explanation.
        */
        vyukov_queue_pool( size_t nCapacity = 0 )
            : m_Queue( nCapacity )
        {
            preallocate_pool();
        }

        /// Deallocates the pool.
        ~vyukov_queue_pool()
        {
            m_Queue.clear();
            std_allocator().deallocate( m_pFirst, m_Queue.capacity());
        }

        /// Allocates an object from pool
        /**
            The pool supports allocation only single object (\p n = 1).
            If \p n > 1 the behavior is undefined.

            If the queue is not empty, the popped value is returned.
            Otherwise, a new value allocated.
        */
        value_type * allocate( size_t n )
        {
            assert( n == 1 );
            CDS_UNUSED(n);

            value_type * p = m_Queue.pop();
            if ( p ) {
                assert( from_pool(p));
                return new( p ) value_type;
            }
            // The pool is empty - allocate new from the heap
            return cxx_allocator().New();
        }

        /// Deallocated the object \p p
        /**
            The pool supports allocation only single object (\p n = 1).
            If \p n > 1 the behavior is undefined.

            If \p p is from preallocated pool, it pushes into the queue.
            Otherwise, \p p is deallocated by allocator provided.
        */
        void deallocate( value_type * p, size_t n )
        {
            assert( n == 1 );
            CDS_UNUSED(n);

            if ( p ) {
                if ( from_pool(p)) {
                    p->~value_type();
                    // The queue can notify about false fullness state
                    // so we push in loop
                    back_off bkoff;
                    while ( !m_Queue.push( *p ))
                        bkoff();
                }
                else
                    cxx_allocator().Delete( p );
            }
        }
    };


    /// Lazy free-list based on bounded lock-free queue \p cds::intrusive::VyukovMPMCCycleQueue
    /** @ingroup cds_memory_pool
        Template parameters:
        - \p T - the type of object maintaining by free-list. \p T must be default constructible
        - \p Traits - traits for \p cds::intrusive::VyukovMPMCCycleQueue class plus
            \p cds::opt::allocator option, default is \p vyukov_queue_pool_traits

        \b Internals

        This free-list is very simple.
        At construction time the pool is empty.
        When allocating the free-list tries to pop an object from
        internal queue. If success the popped object is returned.
        Otherwise a new one is allocated.
        When deallocating, the free-list tries to push the object into the pool.
        If internal queue is full, the object is deallocated by using the allocator provided.
        The pool can manage more than \p N items but only \p N items is placed in the free-list.

        \b Usage

        \p %lazy_vyukov_queue_pool should be used together with \ref pool_allocator.
        You should declare an static object of type \p %lazy_vyukov_queue_pool, provide
        an accessor functor to this object and use \p pool_allocator as an allocator:
        \code
        #include <cds/memory/vyukov_queue_pool.h>
        #include <cds/memory/pool_allocator.h>

        // Pool of Foo object of size 1024.
        typedef cds::memory::lazy_vyukov_queue_pool< Foo > pool_type;
        static pool_type thePool( 1024 );

        struct pool_accessor {
            typedef typename pool_type::value_type  value_type;

            pool_type& operator()() const
            {
                return thePool;
            }
        };

        // Declare pool allocator
        typedef cds::memory::pool_allocator< Foo, pool_accessor >   pool_allocator;

        // Use pool_allocator
        // Allocate an object
        Foo * p = pool_allocator().allocate( 1 );

        // construct object
        new(p) Foo;

        //...

        // Destruct object
        p->~Foo();

        // Deallocate object
        pool_allocator().deallocate( p , 1 );
        \endcode

    */
    template <typename T, typename Traits = vyukov_queue_pool_traits>
    class lazy_vyukov_queue_pool
    {
    public:
        typedef cds::intrusive::VyukovMPMCCycleQueue< T, Traits > queue_type  ;   ///< Queue type

    public:
        typedef T  value_type ; ///< Value type
        typedef Traits traits;  ///< Pool traits
        typedef typename std::allocator_traits<typename traits::allocator>::template rebind_alloc<value_type> allocator_type;   ///< allocator type

    protected:
        //@cond
        typedef cds::details::Allocator< value_type, allocator_type >   cxx_allocator;
        typedef typename cxx_allocator::allocator_type std_allocator;

        queue_type      m_Queue;
        //@endcond

    public:
        /// Constructs empty pool
        lazy_vyukov_queue_pool( size_t nCapacity = 0 )
            : m_Queue( nCapacity )
        {}

        /// Deallocates all objects from the pool
        ~lazy_vyukov_queue_pool()
        {
            std_allocator a;
            while ( !m_Queue.empty())
                a.deallocate( m_Queue.pop(), 1 );
        }

        /// Allocates an object from pool
        /**
            The pool supports allocation only single object (\p n = 1).
            If \p n > 1 the behavior is undefined.

            If the queue is not empty, the popped value is returned.
            Otherwise, a new value allocated.
        */
        value_type * allocate( size_t n )
        {
            assert( n == 1 );
            CDS_UNUSED(n);

            value_type * p = m_Queue.pop();
            if ( p )
                return new( p ) value_type;

            return cxx_allocator().New();
        }

        /// Deallocates the object \p p
        /**
            The pool supports allocation only single object (\p n = 1).
            If \p n > 1 the behaviour is undefined.

            If the queue is not full, \p p is pushed into the queue.
            Otherwise, \p p is deallocated by allocator provided.
        */
        void deallocate( value_type * p, size_t n )
        {
            assert( n == 1 );
            CDS_UNUSED(n);

            if ( p ) {
                p->~value_type();
                // Here we ignore false fullness state of the queue
                if ( !m_Queue.push( *p ))
                    std_allocator().deallocate( p, 1 );
            }
        }

    };

    /// Bounded free-list based on bounded lock-free queue \p cds::intrusive::VyukovMPMCCycleQueue
    /** @ingroup cds_memory_pool
        Template parameters:
        - \p T - the type of object maintaining by free-list. \p T must be default-constructible
        - \p Traits - traits for \p cds::intrusive::VyukovMPMCCycleQueue class plus
            \p cds::opt::allocator option, defaul is \p vyukov_queue_pool_traits

        \b Internals

        At construction time, the free-list allocates the array of N items
        and stores them into the queue, where N is the queue capacity.
        When allocating the free-list tries to pop an object from
        internal queue i.e. from preallocated pool. If success the popped object is returned.
        Otherwise a \p std::bad_alloc exception is raised.
        So, the pool can contain up to \p N items.
        When deallocating, the object is pushed into the queue.
        In debug mode \p deallocate() member function asserts
        that the pointer is from preallocated pool.

        \b Usage

        \p %bounded_vyukov_queue_pool should be used together with \ref pool_allocator.
        You should declare an static object of type \p %bounded_vyukov_queue_pool, provide
        an accessor functor to this object and use \p pool_allocator as an allocator:
        \code
        #include <cds/memory/vyukov_queue_pool.h>
        #include <cds/memory/pool_allocator.h>

        // Pool of Foo object of size 1024.
        struct pool_traits: public cds::memory::vyukov_queue_pool_traits
        {
            typedef cds::opt::v::uninitialized_static_buffer< Foo, 1024 > buffer;
        };
        typedef cds::memory::bounded_vyukov_queue_pool< Foo, pool_traits > pool_type;
        static pool_type thePool;

        struct pool_accessor {
            typedef typename pool_type::value_type  value_type;

            pool_type& operator()() const
            {
                return thePool;
            }
        };

        // Declare pool allocator
        typedef cds::memory::pool_allocator< Foo, pool_accessor >   pool_allocator;

        // Use pool_allocator
        // Allocate an object
        Foo * p = pool_allocator().allocate( 1 );

        // construct object
        new(p) Foo;

        //...

        // Destruct object
        p->~Foo();

        // Deallocate object
        pool_allocator().deallocate( p , 1 );
        \endcode
    */
    template <typename T, typename Traits = vyukov_queue_pool_traits >
    class bounded_vyukov_queue_pool
    {
        //@cond
        struct internal_traits : public Traits {
            typedef cds::atomicity::item_counter item_counter;
        };
        //@endcond
    public:
        typedef cds::intrusive::VyukovMPMCCycleQueue< T, internal_traits > queue_type  ;   ///< Queue type

    public:
        typedef T  value_type;  ///< Value type
        typedef Traits traits;  ///< Pool traits
        typedef typename std::allocator_traits<typename traits::allocator>::template rebind_alloc<value_type> allocator_type;   ///< allocator type
        typedef typename traits::back_off back_off; ///< back-off strategy

    protected:
        //@cond
        typedef cds::details::Allocator< value_type, allocator_type > cxx_allocator;
        typedef typename cxx_allocator::allocator_type std_allocator;

        queue_type      m_Queue;
        value_type *    m_pFirst;
        value_type *    m_pLast;
        //@endcond

    protected:
        //@cond
        void preallocate_pool()
        {
            size_t const nCount = m_Queue.capacity();
            m_pFirst = std_allocator().allocate( nCount );
            m_pLast = m_pFirst + nCount;

            for ( value_type * p = m_pFirst; p < m_pLast; ++p )
                CDS_VERIFY( m_Queue.push( *p )) ;   // must be true
        }

        bool from_pool( value_type * p ) const
        {
            return m_pFirst <= p && p < m_pLast;
        }
        //@endcond

    public:
        /// Preallocates the pool of object
        /**
            \p nCapacity argument is the queue capacity. It should be passed
            if the queue is based on dynamically-allocated buffer.
            See \p cds::intrusive::VyukovMPMCCycleQueue for explanation.
        */
        bounded_vyukov_queue_pool( size_t nCapacity = 0 )
            : m_Queue( nCapacity )
        {
            preallocate_pool();
        }

        /// Deallocates the pool.
        ~bounded_vyukov_queue_pool()
        {
            m_Queue.clear();
            std_allocator().deallocate( m_pFirst, m_Queue.capacity());
        }

        /// Allocates an object from pool
        /**
            The pool supports allocation only single object (\p n = 1).
            If \p n > 1 the behaviour is undefined.

            If the queue is not empty, the popped value is returned.
            Otherwise, a \p std::bad_alloc exception is raised.
        */
        value_type * allocate( size_t n )
        {
            assert( n == 1 );
            CDS_UNUSED( n );

            value_type * p = m_Queue.pop();

            if ( !p ) {
                back_off bkoff;
                while ( m_Queue.size()) {
                    p = m_Queue.pop();
                    if ( p )
                        goto ok;
                    bkoff();
                }

                // The pool is empty
                CDS_THROW_EXCEPTION( std::bad_alloc());
            }

        ok:
            assert( from_pool(p));
            return p;
        }

        /// Deallocates the object \p p
        /**
            The pool supports allocation only single object (\p n = 1).
            If \p n > 1 the behaviour is undefined.

            \p p should be from preallocated pool.
        */
        void deallocate( value_type * p, size_t n )
        {
            assert( n == 1 );
            CDS_UNUSED( n );

            if ( p ) {
                assert( from_pool( p ));
                back_off bkoff;
                // The queue can notify it is full but that is false fullness state
                // So, we push in loop
                while ( !m_Queue.push(*p))
                    bkoff();
            }
        }
    };


}}  // namespace cds::memory


#endif // #ifndef CDSLIB_MEMORY_VYUKOV_QUEUE_ALLOCATOR_H
