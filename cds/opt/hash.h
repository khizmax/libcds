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

#ifdef CDS_CXX11_VARIADIC_TEMPLATE_SUPPORT
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
#       ifdef CDS_MOVE_SEMANTICS_SUPPORT
            hash_list( hash_tuple_type&& t)
                : hash_tuple( std::forward<hash_tuple_type>(t) )
            {}
#       endif

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

#else   // no variadic template support
    namespace details {
        template <typename T> struct hash_list;
        template <typename F1, typename F2>
        struct hash_list< std::tuple<F1, F2> >
        {
            static size_t const size = 2;
            typedef size_t values[size];
            typedef std::tuple<F1, F2>  hash_tuple_type;

            hash_tuple_type hash_tuple;

            hash_list()
            {}
            hash_list( hash_tuple_type const& t)
                : hash_tuple( t )
            {}
#       ifdef CDS_MOVE_SEMANTICS_SUPPORT
            hash_list( hash_tuple_type&& t)
                : hash_tuple( t )
            {}
#       endif

            template <typename T>
            void operator()( size_t * dest, T const& v ) const
            {
                dest[0] = std::get<0>( hash_tuple )( v );
                dest[1] = std::get<1>( hash_tuple )( v );
            }
        };

        template <typename F1, typename F2, typename F3>
        struct hash_list< std::tuple<F1, F2, F3> >
        {
            static size_t const size = 3;
            typedef size_t values[size];
            typedef std::tuple<F1, F2, F3> hash_tuple_type;

            hash_tuple_type hash_tuple;

            hash_list()
            {}
            hash_list( hash_tuple_type const& t)
                : hash_tuple( t )
            {}
#       ifdef CDS_MOVE_SEMANTICS_SUPPORT
            hash_list( hash_tuple_type&& t)
                : hash_tuple( t )
            {}
#       endif

            template <typename T>
            void operator()( size_t * dest, T const& v ) const
            {
                dest[0] = std::get<0>( hash_tuple )( v );
                dest[1] = std::get<1>( hash_tuple )( v );
                dest[2] = std::get<2>( hash_tuple )( v );
            }
        };

        template <typename F1, typename F2, typename F3, typename F4>
        struct hash_list< std::tuple<F1, F2, F3, F4> >
        {
            static size_t const size = 4;
            typedef size_t values[size];
            typedef std::tuple<F1, F2, F3, F4> hash_tuple_type;

            hash_tuple_type hash_tuple;

            hash_list()
            {}
            hash_list( hash_tuple_type const& t)
                : hash_tuple( t )
            {}
#       ifdef CDS_MOVE_SEMANTICS_SUPPORT
            hash_list( hash_tuple_type&& t)
                : hash_tuple( t )
            {}
#       endif

            template <typename T>
            void operator()( size_t * dest, T const& v ) const
            {
                dest[0] = std::get<0>( hash_tuple )( v );
                dest[1] = std::get<1>( hash_tuple )( v );
                dest[2] = std::get<2>( hash_tuple )( v );
                dest[3] = std::get<3>( hash_tuple )( v );
            }
        };

        template <typename F1, typename F2, typename F3, typename F4, typename F5>
        struct hash_list< std::tuple<F1, F2, F3, F4, F5> >
        {
            static size_t const size = 5;
            typedef size_t values[size];
            typedef std::tuple<F1, F2, F3, F4, F5> hash_tuple_type;

            hash_tuple_type hash_tuple;

            hash_list()
            {}
            hash_list( hash_tuple_type const& t)
                : hash_tuple( t )
            {}
#       ifdef CDS_MOVE_SEMANTICS_SUPPORT
            hash_list( hash_tuple_type&& t)
                : hash_tuple( t )
            {}
#       endif

            template <typename T>
            void operator()( size_t * dest, T const& v ) const
            {
                dest[0] = std::get<0>( hash_tuple )( v );
                dest[1] = std::get<1>( hash_tuple )( v );
                dest[2] = std::get<2>( hash_tuple )( v );
                dest[3] = std::get<3>( hash_tuple )( v );
                dest[4] = std::get<4>( hash_tuple )( v );
            }
        };

        template <typename F1, typename F2, typename F3, typename F4, typename F5, typename F6>
        struct hash_list< std::tuple<F1, F2, F3, F4, F5, F6> >
        {
            static size_t const size = 6;
            typedef size_t values[size];
            typedef std::tuple<F1, F2, F3, F4, F5, F6> hash_tuple_type;

            hash_tuple_type hash_tuple;

            hash_list()
            {}
            hash_list( hash_tuple_type const& t)
                : hash_tuple( t )
            {}
#       ifdef CDS_MOVE_SEMANTICS_SUPPORT
            hash_list( hash_tuple_type&& t)
                : hash_tuple( t )
            {}
#       endif

            template <typename T>
            void operator()( size_t * dest, T const& v ) const
            {
                dest[0] = std::get<0>( hash_tuple )( v );
                dest[1] = std::get<1>( hash_tuple )( v );
                dest[2] = std::get<2>( hash_tuple )( v );
                dest[3] = std::get<3>( hash_tuple )( v );
                dest[4] = std::get<4>( hash_tuple )( v );
                dest[5] = std::get<5>( hash_tuple )( v );
            }
        };

        template <typename F1, typename F2, typename F3, typename F4, typename F5, typename F6, typename F7>
        struct hash_list< std::tuple<F1, F2, F3, F4, F5, F6, F7> >
        {
            static size_t const size = 7;
            typedef size_t values[size];
            typedef std::tuple<F1, F2, F3, F4, F5, F6, F7> hash_tuple_type;

            hash_tuple_type hash_tuple;

            hash_list()
            {}
            hash_list( hash_tuple_type const& t)
                : hash_tuple( t )
            {}
#       ifdef CDS_MOVE_SEMANTICS_SUPPORT
            hash_list( hash_tuple_type&& t)
                : hash_tuple( t )
            {}
#       endif

            template <typename T>
            void operator()( size_t * dest, T const& v ) const
            {
                dest[0] = std::get<0>( hash_tuple )( v );
                dest[1] = std::get<1>( hash_tuple )( v );
                dest[2] = std::get<2>( hash_tuple )( v );
                dest[3] = std::get<3>( hash_tuple )( v );
                dest[4] = std::get<4>( hash_tuple )( v );
                dest[5] = std::get<5>( hash_tuple )( v );
                dest[6] = std::get<6>( hash_tuple )( v );
            }
        };

        template <typename F1, typename F2, typename F3, typename F4, typename F5, typename F6, typename F7, typename F8>
        struct hash_list< std::tuple<F1, F2, F3, F4, F5, F6, F7, F8> >
        {
            static size_t const size = 8;
            typedef size_t values[size];
            typedef std::tuple<F1, F2, F3, F4, F5, F6, F7, F8> hash_tuple_type;

            hash_tuple_type hash_tuple;

            hash_list()
            {}
            hash_list( hash_tuple_type const& t)
                : hash_tuple( t )
            {}
#       ifdef CDS_MOVE_SEMANTICS_SUPPORT
            hash_list( hash_tuple_type&& t)
                : hash_tuple( t )
            {}
#       endif

            template <typename T>
            void operator()( size_t * dest, T const& v ) const
            {
                dest[0] = std::get<0>( hash_tuple )( v );
                dest[1] = std::get<1>( hash_tuple )( v );
                dest[2] = std::get<2>( hash_tuple )( v );
                dest[3] = std::get<3>( hash_tuple )( v );
                dest[4] = std::get<4>( hash_tuple )( v );
                dest[5] = std::get<5>( hash_tuple )( v );
                dest[6] = std::get<6>( hash_tuple )( v );
                dest[7] = std::get<7>( hash_tuple )( v );
            }
        };

#if !((CDS_COMPILER == CDS_COMPILER_MSVC || CDS_COMPILER == CDS_COMPILER_INTEL) && _MSC_VER == 1700)
        // MSVC 11: max count of argument is 8

        template <typename F1, typename F2, typename F3, typename F4, typename F5, typename F6, typename F7, typename F8, typename F9>
        struct hash_list< std::tuple<F1, F2, F3, F4, F5, F6, F7, F8, F9> >
        {
            static size_t const size = 9;
            typedef size_t values[size];
            typedef std::tuple<F1, F2, F3, F4, F5, F6, F7, F8, F9> hash_tuple_type;

            hash_tuple_type hash_tuple;

            hash_list()
            {}
            hash_list( hash_tuple_type const& t)
                : hash_tuple( t )
            {}
#       ifdef CDS_MOVE_SEMANTICS_SUPPORT
            hash_list( hash_tuple_type&& t)
                : hash_tuple( t )
            {}
#       endif

            template <typename T>
            void operator()( size_t * dest, T const& v ) const
            {
                dest[0] = std::get<0>( hash_tuple )( v );
                dest[1] = std::get<1>( hash_tuple )( v );
                dest[2] = std::get<2>( hash_tuple )( v );
                dest[3] = std::get<3>( hash_tuple )( v );
                dest[4] = std::get<4>( hash_tuple )( v );
                dest[5] = std::get<5>( hash_tuple )( v );
                dest[6] = std::get<6>( hash_tuple )( v );
                dest[7] = std::get<7>( hash_tuple )( v );
                dest[8] = std::get<8>( hash_tuple )( v );
            }
        };

        template <typename F1, typename F2, typename F3, typename F4, typename F5, typename F6, typename F7, typename F8, typename F9,
                  typename F10>
        struct hash_list< std::tuple<F1, F2, F3, F4, F5, F6, F7, F8, F9, F10> >
        {
            static size_t const size = 10;
            typedef size_t values[size];
            typedef std::tuple<F1, F2, F3, F4, F5, F6, F7, F8, F9, F10> hash_tuple_type;

            hash_tuple_type hash_tuple;

            hash_list()
            {}
            hash_list( hash_tuple_type const& t)
                : hash_tuple( t )
            {}
#       ifdef CDS_MOVE_SEMANTICS_SUPPORT
            hash_list( hash_tuple_type&& t)
                : hash_tuple( t )
            {}
#       endif

            template <typename T>
            void operator()( size_t * dest, T const& v ) const
            {
                dest[0] = std::get<0>( hash_tuple )( v );
                dest[1] = std::get<1>( hash_tuple )( v );
                dest[2] = std::get<2>( hash_tuple )( v );
                dest[3] = std::get<3>( hash_tuple )( v );
                dest[4] = std::get<4>( hash_tuple )( v );
                dest[5] = std::get<5>( hash_tuple )( v );
                dest[6] = std::get<6>( hash_tuple )( v );
                dest[7] = std::get<7>( hash_tuple )( v );
                dest[8] = std::get<8>( hash_tuple )( v );
                dest[9] = std::get<9>( hash_tuple )( v );
            }
        };
#endif
    } // namespace details

    template< typename F1, typename F2 >
    struct hash< std::tuple< F1, F2 > >
    {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef details::hash_list< std::tuple<F1, F2> >  hash;
        };
        //@endcond
    };
    template< typename F1, typename F2, typename F3 >
    struct hash< std::tuple< F1, F2, F3 > >
    {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef details::hash_list< std::tuple<F1, F2, F3> >  hash;
        };
        //@endcond
    };
    template< typename F1, typename F2, typename F3, typename F4 >
    struct hash< std::tuple< F1, F2, F3, F4 > >
    {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef details::hash_list< std::tuple<F1, F2, F3, F4> >  hash;
        };
        //@endcond
    };
    template< typename F1, typename F2, typename F3, typename F4, typename F5 >
    struct hash< std::tuple< F1, F2, F3, F4, F5 > >
    {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef details::hash_list< std::tuple<F1, F2, F3, F4, F5> >  hash;
        };
        //@endcond
    };
    template< typename F1, typename F2, typename F3, typename F4, typename F5, typename F6 >
    struct hash< std::tuple< F1, F2, F3, F4, F5, F6 > >
    {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef details::hash_list< std::tuple<F1, F2, F3, F4, F5, F6> >  hash;
        };
        //@endcond
    };
    template< typename F1, typename F2, typename F3, typename F4, typename F5, typename F6, typename F7 >
    struct hash< std::tuple< F1, F2, F3, F4, F5, F6, F7 > >
    {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef details::hash_list< std::tuple<F1, F2, F3, F4, F5, F6, F7> >  hash;
        };
        //@endcond
    };
    template< typename F1, typename F2, typename F3, typename F4, typename F5, typename F6, typename F7, typename F8 >
    struct hash< std::tuple< F1, F2, F3, F4, F5, F6, F7, F8 > >
    {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef details::hash_list< std::tuple<F1, F2, F3, F4, F5, F6, F7, F8> >  hash;
        };
        //@endcond
    };

#if !((CDS_COMPILER == CDS_COMPILER_MSVC || CDS_COMPILER == CDS_COMPILER_INTEL) && _MSC_VER == 1700)
    // MSVC 11: max count of argument is 8

    template< typename F1, typename F2, typename F3, typename F4, typename F5, typename F6, typename F7, typename F8, typename F9 >
    struct hash< std::tuple< F1, F2, F3, F4, F5, F6, F7, F8, F9 > >
    {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef details::hash_list< std::tuple<F1, F2, F3, F4, F5, F6, F7, F8, F9> >  hash;
        };
        //@endcond
    };
    template< typename F1, typename F2, typename F3, typename F4, typename F5, typename F6, typename F7, typename F8, typename F9,
              typename F10 >
    struct hash< std::tuple< F1, F2, F3, F4, F5, F6, F7, F8, F9, F10 > >
    {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef details::hash_list< std::tuple<F1, F2, F3, F4, F5, F6, F7, F8, F9, F10> >  hash;
        };
        //@endcond
    };
#endif  // !MSVC11
#endif  // #ifdef CDS_CXX11_VARIADIC_TEMPLATE_SUPPORT

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
#       ifdef CDS_MOVE_SEMANTICS_SUPPORT
            hash_list_wrapper( hash_tuple_type&& t)
                : m_wrappedList( std::forward<hash_tuple_type>(t) )
            {}
#       endif

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
