// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_URCU_RAW_PTR_H
#define CDSLIB_URCU_RAW_PTR_H

#include <utility> // std::move
#include <type_traits>
#include <cds/details/defs.h>

namespace cds { namespace urcu {

    /// Raw pointer to node of RCU-based container
    /**
        This class is intented for returning a pointer to node of RCU-based container.
        The objects of \p %raw_ptr class is returned by functions like \p get() of that containers.
        Those functions must be called only under RCU-lock, otherwise the node returned can be reclaimed.
        On the other hand, traversing the container can remove a lot of nodes marked as deleted ones.
        Since RCU is locked, such nodes cannot be reclaimed immediately and must be retired only
        outside RCU lock.

        The object of \p %raw_ptr solves that problem: it contains the pointer to the node found
        and a chain of nodes that were be reclaimed during traversing. The \p %raw_ptr object destructor
        frees the chain (but not the node found) passing it to RCU \p batch_retire().

        The object of \p %raw_ptr class must be destructed only outside RCU-lock of current thread.

        Usually, you do not need to use \p %raw_ptr class directly. Each RCU container declares
        a \p %raw_ptr typedef suitable for the container.

        Template arguments:
        - \p RCU - one of \ref cds_urcu_gc "RCU type"
        - \p ValueType - type of values stored in container
        - \p ReclaimedEnumerator - implementation-defined for each type of container

        Example: let \p Container is an RCU container
        @code
            Container c;
            // ...
            // Find a key
            typename Container::raw_ptr pRaw;

            // RCU locked section
            {
                typename Container::rcu_lock l;
                pRaw = c.get( key );
                if ( pRaw ) {
                    // Deal with pRaw
                }
            }
            // Release outside RCU-lock
            pRaw.release();
        @endcode
    */
    template <
        class RCU,
        typename ValueType,
        typename ReclaimedEnumerator
    >
    class raw_ptr
    {
    public:
        typedef RCU rcu;    ///< RCU type - one of <tt>cds::urcu::gc< ... ></tt>
        typedef ValueType   value_type; ///< Value type pointed by \p %raw_ptr
        typedef ReclaimedEnumerator reclaimed_enumerator; ///< implementation-defined, for internal use only

    private:
        //@cond
        value_type *            m_ptr;  ///< pointer to node
        reclaimed_enumerator    m_Enum; ///< reclaimed node enumerator
        //@endcond

    public:
        /// Constructs an empty raw pointer
        raw_ptr()
            : m_ptr( nullptr )
        {}

        /// Move ctor
        raw_ptr( raw_ptr&& p )
            : m_ptr( p.m_ptr )
            , m_Enum(std::move( p.m_Enum ))
        {
            p.m_ptr = nullptr;
        }

        /// Copy ctor is prohibited
        raw_ptr( raw_ptr const& ) = delete;

        //@cond
        // Only for internal use
        raw_ptr( value_type * p, reclaimed_enumerator&& e )
            : m_ptr( p )
            , m_Enum(std::move( e ))
        {}
        raw_ptr( reclaimed_enumerator&& e )
            : m_ptr( nullptr )
            , m_Enum(std::move( e ))
        {}
        //@endcond

        /// Releases the raw pointer
        ~raw_ptr()
        {
            release();
        }

    public:
        /// Move assignment operator
        /**
            This operator may be called only inside RCU-lock.
        */
        raw_ptr& operator=( raw_ptr&& p ) noexcept
        {
            assert( rcu::is_locked());
            m_ptr = p.m_ptr;
            m_Enum.combine(  std::move( p.m_Enum ));
            p.m_ptr = nullptr;
            return *this;
        }

        /// Copy assignment is prohibited
        raw_ptr& operator=( raw_ptr const& ) = delete;

        /// Returns a pointer to stored value
        value_type * operator ->() const noexcept
        {
            return m_ptr;
        }

        /// Returns a reference to stored value
        value_type& operator *()
        {
            assert( m_ptr != nullptr );
            return *m_ptr;
        }

        /// Returns a reference to stored value
        value_type const& operator *() const
        {
            assert( m_ptr != nullptr );
            return *m_ptr;
        }

        /// Checks if the \p %raw_ptr is \p nullptr
        bool empty() const noexcept
        {
            return m_ptr == nullptr;
        }

        /// Checks if the \p %raw_ptr is not empty
        explicit operator bool() const noexcept
        {
            return !empty();
        }

        /// Releases the \p %raw_ptr object
        /**
            This function may be called only outside RCU locked region.
            After \p %release() the object becomes empty and can be reused.
        */
        void release()
        {
            m_Enum.apply();
            m_ptr = nullptr;
        }
    };

    //@cond
    // Adapter of \p raw_ptr for non-intrusive containers based on intrusive counterpart
    template <
        typename ValueType,
        typename RawPtr,
        typename Converter
    >
    class raw_ptr_adaptor: private RawPtr
    {
    public:
        typedef RawPtr      intrusive_raw_ptr;
        typedef ValueType   value_type;
        typedef typename intrusive_raw_ptr::value_type node_type;
        typedef Converter   converter_type;

    public:
        // Constructs an empty raw pointer
        raw_ptr_adaptor()
            : intrusive_raw_ptr()
        {}

        // Move ctor
        raw_ptr_adaptor( intrusive_raw_ptr&& p )
            : intrusive_raw_ptr( std::move(p))
        {}

        // Move ctor
        raw_ptr_adaptor( raw_ptr_adaptor&& p )
            : intrusive_raw_ptr( std::move(p))
        {}

        // Copy ctor is prohibited
        raw_ptr_adaptor( raw_ptr_adaptor const& ) = delete;

        // Releases the raw pointer
        ~raw_ptr_adaptor()
        {
            release();
        }

    public:
        // Move assignment operator
        /*
            This operator may be called only inside RCU-lock.
            The \p this should be empty.

            In general, move assignment is intented for internal use.
        */
        raw_ptr_adaptor& operator=( raw_ptr_adaptor&& p ) noexcept
        {
            intrusive_raw_ptr::operator =(std::move(p));
            return *this;
        }

        // Copy assignment is prohibited
        raw_ptr_adaptor& operator=( raw_ptr_adaptor const& ) = delete;

        // Returns a pointer to stored value
        value_type * operator ->() const noexcept
        {
            return converter_type()( intrusive_raw_ptr::operator->());
        }

        // Returns a reference to stored value
        value_type& operator *()
        {
            return converter_type()( intrusive_raw_ptr::operator*());
        }

        // Returns a reference to stored value
        value_type const& operator *() const
        {
            return converter_type()( intrusive_raw_ptr::operator*());
        }

        // Checks if the \p %raw_ptr is \p nullptr
        bool empty() const noexcept
        {
            return intrusive_raw_ptr::empty();
        }

        // Checks if the \p %raw_ptr is not empty
        explicit operator bool() const noexcept
        {
            return !empty();
        }

        // Releases the \p %raw_ptr object
        /*
            This function may be called only outside RCU section.
            After \p %release() the object can be reused.
        */
        void release()
        {
            intrusive_raw_ptr::release();
        }
    };
    //@endcond

}} // namespace cds::urcu

#endif // #ifndef CDSLIB_URCU_RAW_PTR_H
