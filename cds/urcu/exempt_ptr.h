//$$CDS-header$$

#ifndef __CDS_URCU_EXEMPT_PTR_H
#define __CDS_URCU_EXEMPT_PTR_H

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
        For non-intrusive containers from \p cds::container namespace \p Disposer is an invocation
        of node deallocator. For intrusive containers the disposer can be empty or it can trigger an event "node can be reused safely".
        In any case, the exempt pointer concept keeps RCU semantics.

        You don't need use this helper class directly. Any RCU-based container defines a proper typedef for this template.

        Template arguments:
        - \p RCU - one of \ref cds_urcu_gc "RCU type"
        - \p NodeType - container's node type
        - \p ValueType - value type stored in container's node. For intrusive containers it is the same as \p NodeType
        - \p Disposer - a disposer functor
        - \p Cast - a functor for casting from \p NodeType to \p ValueType. Usually, for intrusive containers \p Cast may be \p void.
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
        //TODO: use move semantics and explicit operator bool!
    public:
        typedef RCU         rcu         ;   ///< RCU type - one of <tt>cds::urcu::gc< ... ></tt>
        typedef NodeType    node_type   ;   ///< Node type
        typedef ValueType   value_type  ;   ///< Value type
        typedef Disposer    disposer    ;   ///< Disposer calling when release
        typedef Cast        node_to_value_cast  ;   ///< Functor converting \p node_type to \p value_type

    private:
        //@cond
        node_type *     m_pNode;
        //@endcond

    private:
        //@cond
        // No copy-constructible
        exempt_ptr( exempt_ptr const& );
        exempt_ptr& operator=( exempt_ptr const& );
        //@endcond

    public:
        /// Constructs empty pointer
        exempt_ptr() CDS_NOEXCEPT
            : m_pNode( null_ptr<node_type *>())
        {}

        /// Releases the pointer
        ~exempt_ptr()
        {
            release();
        }

        /// Checks if the pointer is \p NULL
        bool empty() const CDS_NOEXCEPT
        {
            return m_pNode == null_ptr<node_type *>();
        }

        /// Dereference operator
        value_type * operator->() const CDS_NOEXCEPT
        {
            return !empty() ? node_to_value_cast()( m_pNode ) : null_ptr<value_type *>();
        }

        /// Returns a reference to the value
        value_type& operator *() CDS_NOEXCEPT
        {
            assert( !empty());
            return *node_to_value_cast()( m_pNode );
        }

        //@cond
        /// Assignment operator, the object should be empty. For internal use only
        exempt_ptr& operator =( node_type * pNode )
        {
            // release() cannot be called in this point since RCU should be locked
            assert( empty() );
            assert( rcu::is_locked() );
            m_pNode = pNode;
            return *this;
        }
        //@endcond

        /// Disposes the pointer. Should be called only outside of RCU critical section
        void release()
        {
            assert( !rcu::is_locked() );
            if ( !empty() ) {
                rcu::template retire_ptr<disposer>( m_pNode );
                m_pNode = null_ptr<node_type *>();
            }
        }
    };

    //@cond
    // Intrusive container specialization
    template <
        class RCU,
            typename NodeType,
            typename Disposer
    >
    class exempt_ptr< RCU, NodeType, NodeType, Disposer, void >
    {
        //TODO: use move semantics and explicit operator bool!
    public:
        typedef RCU         rcu         ;   ///< RCU type - one of <tt>cds::urcu::gc< ... ></tt>
        typedef NodeType    node_type   ;   ///< Node type
        typedef NodeType    value_type  ;   ///< Node type
        typedef Disposer    disposer    ;   ///< Disposer calling when release
        typedef void        node_to_value_cast; ///< No casting is needed

    private:
        node_type *     m_pNode;

    private:
        // No copy-constructible
        exempt_ptr( exempt_ptr const& );
        exempt_ptr& operator=( exempt_ptr const& );

    public:
        /// Constructs empty pointer
        exempt_ptr() CDS_NOEXCEPT
            : m_pNode( null_ptr<node_type *>())
        {}

        /// Releases the pointer
        ~exempt_ptr()
        {
            release();
        }

        /// Checks if the pointer is \p NULL
        bool empty() const CDS_NOEXCEPT
        {
            return m_pNode == null_ptr<node_type *>();
        }

        /// Dereference operator.
        value_type * operator->() const CDS_NOEXCEPT
        {
            return !empty() ? m_pNode : null_ptr<value_type *>();
        }

        /// Returns a reference to the value
        value_type& operator *() CDS_NOEXCEPT
        {
            assert( !empty());
            return *m_pNode;
        }

        /// Assignment operator, the object should be empty. For internal use only
        exempt_ptr& operator =( node_type * pNode )
        {
            // release() cannot be called in this point since RCU should be locked
            assert( empty() );
            assert( rcu::is_locked() );
            m_pNode = pNode;
            return *this;
        }

        /// Disposes the pointer. Should be called only outside of RCU critical section
        void release()
        {
            assert( !rcu::is_locked() );
            if ( !empty() ) {
                rcu::template retire_ptr<disposer>( m_pNode );
                m_pNode = null_ptr<node_type *>();
            }
        }
    };
    //@endcond

}} // namespace cds::urcu

#endif //#ifndef __CDS_URCU_EXEMPT_PTR_H
