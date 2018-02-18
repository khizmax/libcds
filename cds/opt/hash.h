// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_OPT_HASH_H
#define CDSLIB_OPT_HASH_H

#include <tuple>
#include <functional>
#include <cds/opt/options.h>

namespace cds { namespace opt {

    /// [type-option] Option setter for a hash function
    /**
        This option setter specifies hash functor used in unordered containers.

        The default value  of template argument \p Functor is \p cds::opt::v::hash
        that is synonym for <tt>std::hash</tt> implementation of standard library.
    */
    template <typename Functor>
    struct hash {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef Functor hash;
        };
        //@endcond
    };

    namespace v {
        //@cond
        using std::hash;

        /// Metafunction selecting default hash implementation
        /**
            The metafunction selects appropriate hash functor implementation.
            If \p Hash is not equal to opt::none, then result of metafunction is \p Hash.
            Otherwise, the result is <tt> std::hash<Q> </tt>.

            Note that default hash function like <tt> std::hash<Q> </tt>
            is generally not suitable for complex type \p Q and its derivatives.
            You should manually provide particular hash functor for such types.
        */
        template <typename Hash>
        struct hash_selector
        {
            typedef Hash    type;   ///< resulting implementation of hash functor
        };

        template <>
        struct hash_selector<opt::none>
        {
            struct type {
                template <typename Q>
                size_t operator()( Q const& key ) const
                {
                    return std::hash<Q>()( key );
                }
            };
        };
        //@endcond
    }   // namespace v

    //@cond
    namespace details {
        template <class> struct hash_list;
        template <typename... Functors>
        struct hash_list< std::tuple<Functors...> >
        {
            static size_t const size = sizeof...(Functors);
            typedef size_t values[size];
            typedef std::tuple<Functors...> hash_tuple_type;

            hash_tuple_type hash_tuple;

            hash_list()
            {}

            hash_list( hash_tuple_type const& t)
                : hash_tuple( t )
            {}
            hash_list( hash_tuple_type&& t)
                : hash_tuple( std::forward<hash_tuple_type>(t))
            {}

            template <size_t I, typename T>
            typename std::enable_if< (I == sizeof...(Functors)) >::type apply( size_t * /*dest*/, T const& /*v*/ ) const
            {}

            template <size_t I, typename T>
            typename std::enable_if< (I < sizeof...(Functors)) >::type apply( size_t * dest, T const& v ) const
            {
                dest[I] = std::get<I>( hash_tuple )( v );
                apply<I+1>( dest, v );
            }

            template <typename T>
            void operator()( size_t * dest, T const& v ) const
            {
                apply<0>( dest, v );
            }
        };
    } // namespace details
    //@endcond

    /// Declare tuple for hash functors \p Functors
    template <typename... Functors>
    using hash_tuple = details::hash_list< std::tuple< Functors... >>;

    //@cond
    // At least, two functors must be provided. Single functor is not supported
    template <typename Functor> struct hash< std::tuple<Functor> >;
    //@endcond

    /// Multi-functor hash option setter - specialization for \p std::tuple
    template <typename... Functors>
    struct hash< std::tuple<Functors...> >
    {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef details::hash_list< std::tuple<Functors...> >  hash;
        };
        //@endcond
    };


    //@cond
    namespace details {

        template <class HashList, typename WrappedType, typename Wrapper>
        struct hash_list_wrapper {
            typedef HashList                            hash_list;
            typedef WrappedType                         wrapped_type;
            typedef Wrapper                             wrapper_type;

            typedef typename hash_list::hash_tuple_type hash_tuple_type;
            static size_t const size = hash_list::size;

            hash_list   m_wrappedList;

            hash_list_wrapper()
            {}
            hash_list_wrapper( hash_tuple_type const& t)
                : m_wrappedList( t )
            {}
            hash_list_wrapper( hash_tuple_type&& t)
                : m_wrappedList( std::forward<hash_tuple_type>(t))
            {}

            void operator()( size_t * dest, wrapped_type const& what ) const
            {
                m_wrappedList( dest, wrapper_type()( what ));
            }

            template <typename Q>
            void operator()( size_t * dest, Q const& what) const
            {
                m_wrappedList( dest, what );
            }
        };

    } // namespace details
    //@endcond

}} // namespace cds::opt

#endif // #ifndef CDSLIB_OPT_HASH_H
