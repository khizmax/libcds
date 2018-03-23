// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_URCU_EXEMPT_PTR_H
#define CDSLIB_URCU_EXEMPT_PTR_H

#include <type_traits>
#include <cds/details/defs.h>

namespace cds { namespace urcu {

    //@cond
    namespace details {
        template <typename Node, typename Value>
        struct conventional_exempt_member_cast
        {
            Value * operator()( Node * p ) const
            {
                return &p->m_Value;
            }
        };

        template <typename Node, typename Value>
        struct conventional_exempt_pair_cast
        {
            Value * operator()( Node * p ) const
            {
                return &p->m_Data;
            }
        };
    } // namespace details
    //@endcond

    /// Exempt pointer for RCU
    /**
        This special pointer class is intended for returning extracted node from RCU-based container.
        The destructor (and \p release() member function) invokes <tt>RCU::retire_ptr< Disposer >()</tt> function to dispose the node.
        For non-intrusive containers from \p cds::container namespace \p Disposer is usually an invocation
        of node deallocator. For intrusive containers the disposer can be empty or it can trigger an event "node can be reused safely".
        In any case, the exempt pointer concept keeps RCU semantics.
        The destructor or \p release() should be called outside the RCU lock of current thread.

        You don't need use this helper class directly. Any RCU-based container typedefs a simplified version of this template.

        Template arguments:
        - \p RCU - one of \ref cds_urcu_gc "RCU type"
        - \p NodeType - container's node type
        - \p ValueType - value type stored in container's node. For intrusive containers it is the same as \p NodeType
        - \p Disposer - a disposer functor
        - \p Cast - a functor for casting from \p NodeType to \p ValueType. For intrusive containers
            the casting is usually disabled, i.e. \p Cast is \p void.
    */
    template <
        class RCU,
        typename NodeType,
        typename ValueType,
        typename Disposer,
#ifdef CDS_DOXYGEN_INVOKED
        typename Cast
#else
        typename Cast=details::conventional_exempt_member_cast<NodeType, ValueType>
#endif
    >
    class exempt_ptr
    {
        //@cond
        struct trivial_cast {
            ValueType * operator()( NodeType * p ) const
            {
                return p;
            }
        };
        //@endcond
    public:
        typedef RCU         rcu         ;   ///< RCU type - one of <tt>cds::urcu::gc< ... ></tt>
        typedef NodeType    node_type   ;   ///< Node type
        typedef ValueType   value_type  ;   ///< Value type
        typedef Disposer    disposer    ;   ///< Disposer calling when release
        /// Functor converting \p node_type to \p value_type
        typedef typename std::conditional< std::is_same< Cast, void >::value, trivial_cast, Cast>::type node_to_value_cast;

    private:
        //@cond
        node_type *     m_pNode;
        //@endcond

    public:
        /// Constructs empty pointer
        exempt_ptr() noexcept
            : m_pNode( nullptr )
        {}

        //@cond
        /// Creates exempt pointer for \p pNode. Only for internal use.
        explicit exempt_ptr( node_type * pNode ) noexcept
            : m_pNode( pNode )
        {}
        explicit exempt_ptr( std::nullptr_t ) noexcept
            : m_pNode( nullptr )
        {}
        //@endcond

        /// Move ctor
        exempt_ptr( exempt_ptr&& p ) noexcept
            : m_pNode( p.m_pNode )
        {
            p.m_pNode = nullptr;
        }

        /// The exempt pointer is not copy-constructible
        exempt_ptr( exempt_ptr const& ) = delete;

        /// Releases the pointer, see \p release()
        ~exempt_ptr()
        {
            release();
        }

        /// Checks if the pointer is \p nullptr
        bool empty() const noexcept
        {
            return m_pNode == nullptr;
        }

        /// \p bool operator returns <tt>!empty()</tt>
        explicit operator bool() const noexcept
        {
            return !empty();
        }

        /// Dereference operator
        value_type * operator->() const noexcept
        {
            return !empty() ? node_to_value_cast()(m_pNode) : nullptr;
        }

        /// Returns a reference to the value
        value_type& operator *() noexcept
        {
            assert( !empty());
            return *node_to_value_cast()( m_pNode );
        }

        /// Move assignment. Can be called only outside of RCU critical section
        exempt_ptr& operator =( exempt_ptr&& p ) noexcept
        {
            release();
            m_pNode = p.m_pNode;
            p.m_pNode = nullptr;
            return *this;
        }

        /// The exempt pointer is not copy-assignable
        exempt_ptr& operator=(exempt_ptr const&) = delete;

        /// Disposes the pointer. Should be called only outside of RCU critical section
        void release()
        {
            if ( !empty()) {
                assert( !rcu::is_locked());
                rcu::template retire_ptr<disposer>( m_pNode );
                m_pNode = nullptr;
            }
        }
    };
}} // namespace cds::urcu

#endif //#ifndef CDSLIB_URCU_EXEMPT_PTR_H
