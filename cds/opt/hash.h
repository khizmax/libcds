//$$CDS-header$$

#ifndef __CDS_OPT_HASH_H
#define __CDS_OPT_HASH_H

#include <tuple>
#include <cds/opt/options.h>
#include <cds/details/hash_functor_selector.h>

namespace cds { namespace opt {

    /// [type-option] Option setter for a hash function
    /**
        This option setter specifies hash functor used in unordered containers.

        The default value  of template argument \p Functor is \p cds::opt::v::hash
        that is synonym for <tt>std::hash</tt> implementation of standard library.
        If standard C++ library of the compiler you use does not provide TR1 implementation
        the \p cds library automatically selects <tt>boost::hash</tt>.
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
        using cds::details::hash;

        /// Metafunction selecting default hash implementation
        /**
            The metafunction selects appropriate hash functor implementation.
            If \p Hash is not equal to opt::none, then result of metafunction is \p Hash.
            Otherwise, the result is <tt> std::hash<Q> </tt> or <tt> boost::hash<Q> </tt>
            depending of compiler you use.

            Note that default hash function like <tt> std::hash<Q> </tt> or <tt> boost::hash<Q> </tt>
            is generally not suitable for complex type \p Q and its derivatives.
            You should manually provide particular hash functor for such types.
        */
        template <typename Hash>
        struct hash_selector
        {
            typedef Hash    type    ;   ///< resulting implementation of hash functor
        };

        template <>
        struct hash_selector<opt::none>
        {
            struct type {
                template <typename Q>
                size_t operator()( Q const& key ) const
                {
                    return hash<Q>()( key );
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
                : hash_tuple( std::forward<hash_tuple_type>(t) )
            {}

            template <size_t I, typename T>
            typename std::enable_if< (I == sizeof...(Functors)) >::type apply( size_t * dest, T const& v ) const
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

    //@cond
    // At least, two functors must be provided. Single functor is not supported
//#if CDS_COMPILER != CDS_COMPILER_INTEL
    // Intel C++ compiler does not support
    template <typename Functor> struct hash< std::tuple<Functor> >;
//#endif
    //@endcond

    /// Multi-functor hash option setter - specialization for std::tuple
    template <typename... Functors>
    struct hash< std::tuple<Functors...> >
    {
//#   if CDS_COMPILER == CDS_COMPILER_INTEL
        //static_assert( sizeof...(Functors) > 1, "At least, two functors must be provided. Single functor is not supported" );
//#   endif
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
                : m_wrappedList( std::forward<hash_tuple_type>(t) )
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

#endif // #ifndef __CDS_OPT_HASH_H
