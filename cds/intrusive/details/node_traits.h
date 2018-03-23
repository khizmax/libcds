// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_DETAILS_NODE_TRAITS_H
#define CDSLIB_INTRUSIVE_DETAILS_NODE_TRAITS_H

#include <cds/intrusive/options.h>

namespace cds { namespace intrusive {

#ifdef CDS_DOXYGEN_INVOKED
    /// Container's node traits
    /** @ingroup cds_intrusive_helper
        This traits is intended for converting between type \p T of value stored in the intrusive container
        and container's node type \p NodeType.

        There are separate specializations for each \p Hook type.
    */
    template <typename T, typename NodeType, typename Hook>
    struct node_traits
    {
        typedef T        value_type ;  ///< Value type
        typedef NodeType node_type  ;  ///< Node type

        /// Convert value reference to node pointer
        static node_type * to_node_ptr( value_type& v );

        /// Convert value pointer to node pointer
        static node_type * to_node_ptr( value_type * v );

        /// Convert value reference to node pointer (const version)
        static const node_type * to_node_ptr( value_type const& v );

        /// Convert value pointer to node pointer (const version)
        static const node_type * to_node_ptr( value_type const * v );

        /// Convert node refernce to value pointer
        static value_type * to_value_ptr( node_type&  n );

        /// Convert node pointer to value pointer
        static value_type * to_value_ptr( node_type *  n );

        /// Convert node reference to value pointer (const version)
        static const value_type * to_value_ptr( node_type const & n );

        /// Convert node pointer to value pointer (const version)
        static const value_type * to_value_ptr( node_type const * n );
    };

#else
    template <typename T, typename NodeType, class Hook, typename HookType>
    struct node_traits;
#endif

    //@cond
    template <typename T, typename NodeType, class Hook>
    struct node_traits<T, NodeType, Hook, opt::base_hook_tag>
    {
        typedef T        value_type;
        typedef NodeType node_type;

        static node_type * to_node_ptr( value_type& v )
        {
            return static_cast<node_type *>( &v );
        }
        static node_type * to_node_ptr( value_type * v )
        {
            return v ? static_cast<node_type *>(v) : nullptr;
        }
        static const node_type * to_node_ptr( const value_type& v )
        {
            return static_cast<const node_type *>( &v );
        }
        static const node_type * to_node_ptr( const value_type * v )
        {
            return v ? static_cast<const node_type *>(v) : nullptr;
        }
        static value_type * to_value_ptr( node_type&  n )
        {
            return static_cast<value_type *>( &n );
        }
        static value_type * to_value_ptr( node_type *  n )
        {
            return n ? static_cast<value_type *>(n) : nullptr;
        }
        static const value_type * to_value_ptr( const node_type& n )
        {
            return static_cast<const value_type *>( &n );
        }
        static const value_type * to_value_ptr( const node_type * n )
        {
            return n ? static_cast<const value_type *>(n) : nullptr;
        }
    };

    template <typename T, typename NodeType, class Hook>
    struct node_traits<T, NodeType, Hook, opt::member_hook_tag>
    {
        typedef T        value_type;
        typedef NodeType node_type;

        static node_type * to_node_ptr( value_type& v )
        {
            return reinterpret_cast<node_type *>( reinterpret_cast<char *>(&v) + Hook::c_nMemberOffset );
        }
        static node_type * to_node_ptr( value_type * v )
        {
            return v ? to_node_ptr( *v ) : nullptr;
        }
        static const node_type * to_node_ptr( const value_type& v )
        {
            return reinterpret_cast<const node_type *>( reinterpret_cast<const char *>(&v) + Hook::c_nMemberOffset );
        }
        static const node_type * to_node_ptr( const value_type * v )
        {
            return v ? to_node_ptr( *v ) : nullptr;
        }
        static value_type * to_value_ptr( node_type& n )
        {
            return reinterpret_cast<value_type *>( reinterpret_cast<char *>(&n) - Hook::c_nMemberOffset );
        }
        static value_type * to_value_ptr( node_type * n )
        {
            return n ? to_value_ptr( *n ) : nullptr;
        }
        static const value_type * to_value_ptr( const node_type& n )
        {
            return reinterpret_cast<const value_type *>( reinterpret_cast<const char *>(&n) - Hook::c_nMemberOffset );
        }
        static const value_type * to_value_ptr( const node_type * n )
        {
            return n ? to_value_ptr( *n ) : nullptr;
        }
    };

    template <typename T, typename NodeType, class Hook>
    struct node_traits<T, NodeType, Hook, opt::traits_hook_tag>: public Hook::node_traits
    {};
    //@endcond

    /// Node traits selector metafunction
    /** @ingroup cds_intrusive_helper
        The metafunction selects appropriate \ref node_traits specialization based on value type \p T, node type \p NodeType, and hook type \p Hook.
    */
    template <typename T, typename NodeType, class Hook>
    struct get_node_traits
    {
        //@cond
        typedef node_traits<T, NodeType, Hook, typename Hook::hook_type> type;
        //@endcond
    };

    //@cond
    /// Functor converting container's node type to value type
    //TODO: delete
    template <class Container>
    struct node_to_value {
        typename Container::value_type * operator()( typename Container::node_type * p ) const
        {
            typedef typename Container::node_traits node_traits;
            return node_traits::to_value_ptr( p );
        }
    };
    //@endcond

}} // namespace cds::intrusuve

#endif  // #ifndef CDSLIB_INTRUSIVE_DETAILS_NODE_TRAITS_H
