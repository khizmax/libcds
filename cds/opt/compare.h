//$$CDS-header$$

#ifndef CDSLIB_OPT_COMPARE_H
#define CDSLIB_OPT_COMPARE_H

/*
    Editions:
        2011.05.05 khizmax  Created
*/

#include <type_traits>
#include <functional>
#include <string>
#include <cds/opt/options.h>

namespace cds { namespace opt {

    /// [type-option] Option setter for key comparing
    /**
        The option sets a type of a functor to compare keys.
        For comparing two keys \p k1 and \p k2 the functor must return:
        - 1 if k1 > k2
        - 0 if k1 == k2
        - -1 if k1 < k2

        \p Functor is a functor with following interface:
        \code
        template <typename T>
        struct Comparator {
            int operator ()(const T& r1, const T& r2)
            {
                // Comparator body
            }
        };
        \endcode
        Note that the functor must return \p int, not a \p bool value.

        There are predefined type for \p Functor:
        - the functor v::less_comparator that implements comparing functor through \p std::less predicate.
        - the specialization of v::less_comparator functor intended for the string comparison

        You may implement your own comparing functor that satisfies \p Functor interface.

        About relation between \ref opt::less and \ref opt::compare option setters see opt::less description.
    */
    template <typename Functor>
    struct compare {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef Functor compare;
        };
        //@endcond
    };

    namespace v {

        /// Comparator based on \p std::less predicate
        /**
            This functor is predefined type for \p opt::compare option setter.
            It is based on \p std::less predicate.
        */
        template <typename T>
        struct less_comparator {
            /// Operator that compares two value of type \p T
            int operator()(T const& v1, T const& v2)
            {
                if ( std::less<T>()( v1, v2 ) )
                    return -1;
                if ( std::less<T>()( v2, v1 ))
                    return 1;
                return 0;
            }
        };

        /// Comparator specialization for \p std::string
        /**
            This functor uses \p std::string::compare method instead of \p std::less predicate.
        */
        template <typename T, typename Traits, typename Alloc>
        struct less_comparator< std::basic_string<T, Traits, Alloc> >
        {
            //@cond
            typedef std::basic_string<T, Traits, Alloc> string_type;
            int operator()(string_type const& v1, string_type const& v2)
            {
                return v1.compare( v2 );
            }
            //@endcond
        };
    }   // namespace v

    /// [type-option] Option setter for \p less predicate
    /**
        The option sets a binary predicate that tests whether a value of a specified type is less than another value of that type.
        \p Functor interface is similar to \p std::less predicate interface.
        The standard predicate \p std::less can act as \p Functor:
        \code typedef cds::opt::less< std::less< int > > opt_less \endcode

        In addition, the option setter may sets non-standard 2-type predicate (\p std::binary_function):
        \code

        struct foo {
            int n;
        };

        template <typename T, typename Q>
        struct pred_less {
            bool operator ()( const T& t, const Q& q )
            { return t.n < q ; }
            bool operator ()( const Q& q, const T& t )
            { return q < t.n ; }
            bool operator ()( const T& t1, const T& t2 )
            { return t1.n < t2.n ; }
            bool operator ()( const Q& q1, const Q& q2 )
            { return q1 < q2 ; }
        };

        typedef cds::opt::less< pred_less< foo, int > > opt_less;
        \endcode

        Generally, the default type for \p Functor is \p std::less but it depends on the container used.

        \par Relation between \p opt::less and opt::compare option setters
        Unless otherwise specified, \p compare option setter has high priority. If opt::compare and opt::less options are specified
        for a container, the opt::compare option is used:
        \code
        // Suppose, hypothetical map_type allows to specify
        // cds::opt::less and cds::opt::compare options

        typedef map_type< std::string, int,
            cds::opt::compare< cds::opt::v::less_comparator< std::string > >,
            cds::opt::less< std::less< std::string > >
        > my_map_type;

        // For my_map_type, the cds::opt::compare comparator will be used,
        // the cds::opt::less option is ignored without any warnings.
        \endcode
    */
    template <typename Functor>
    struct less {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef Functor less;
        };
        //@endcond
    };

    //@cond
    namespace details {
        template <typename Less>
        struct make_comparator_from_less
        {
            typedef Less less_functor;

            template <typename T, typename Q>
            int operator ()( T const& t, Q const& q ) const
            {
                less_functor f;
                if ( f( t, q ) )
                    return -1;
                if ( f( q, t ) )
                    return 1;
                return 0;
            }
        };

        template <typename T, typename Traits, bool Forced = true >
        struct make_comparator
        {
            typedef typename Traits::compare compare;
            typedef typename Traits::less less;

            typedef typename std::conditional<
                std::is_same< compare, opt::none >::value,
                typename std::conditional<
                    std::is_same< less, opt::none >::value,
                    typename std::conditional< Forced, make_comparator_from_less< std::less<T> >, opt::none >::type,
                    make_comparator_from_less< less >
                >::type,
                compare
            >::type type;
        };

        template <typename T, typename... Options>
        struct make_comparator_from_option_list
        {
            struct default_traits {
                typedef opt::none   compare;
                typedef opt::none   less;
            };

            typedef typename make_comparator< T,
                typename opt::make_options<
                    typename opt::find_type_traits< default_traits, Options... >::type
                    ,Options...
                >::type
            >::type type;
        };
    }   // namespace details
    //@endcond

    /// [type-option] Option setter for \p equal_to predicate
    /**
        The option sets a binary predicate that tests whether a value of a specified type is equal to another value of that type.
        \p Functor interface is similar to \p std::equal_to predicate interface.
        The standard predicate \p std::equal_to can act as \p Functor:
        \code typedef cds::opt::equal_to< std::equal_to< int > > opt_equal_to \endcode

        In addition, the option setter may sets non-standard 2-type (or even N-type) predicate (\p std::binary_function):
        \code

        struct foo {
            int n;
        };

        template <typename T, typename Q>
        struct pred_equal_to {
            bool operator ()( const T& t, const Q& q )
            { return t.n == q ; }
            bool operator ()( const Q& q, const T& t )
            { return q == t.n ; }
            bool operator ()( const T& t1, const T& t2 )
            { return t1.n == t2.n ; }
            bool operator ()( const Q& q1, const Q& q2 )
            { return q1 == q2 ; }
        };

        typedef cds::opt::equal_to< pred_equal_to< foo, int > > opt_equal_to;
        \endcode

        Generally, the default type for \p Functor is \p std::equal_to but it depends on the container used.
    */
    template <typename Functor>
    struct equal_to {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef Functor equal_to;
        };
        //@endcond
    };

    //@cond
    namespace details {
        template <typename Compare>
        struct make_equal_to_from_compare
        {
            typedef Compare compare_functor;

            template <typename T, typename Q>
            bool operator()( T const& t, Q const& q ) const
            {
                return compare_functor()(t, q) == 0;
            }
        };

        template <typename Less>
        struct make_equal_to_from_less
        {
            typedef Less less_functor;

            template <typename T, typename Q>
            bool operator()( T const& t, Q const& q ) const
            {
                less_functor less;
                return !less(t, q) && !less(q, t);
            }
        };

        template <typename T, typename Traits, bool Forced = true>
        struct make_equal_to
        {
            typedef typename Traits::equal_to equal_to;
            typedef typename Traits::compare  compare;
            typedef typename Traits::less     less;

            typedef typename std::conditional<
                std::is_same< equal_to, opt::none >::value,
                typename std::conditional<
                    std::is_same< compare, opt::none >::value,
                    typename std::conditional<
                        std::is_same< less, opt::none >::value,
                        typename std::conditional<
                            Forced,
                            std::equal_to<T>,
                            opt::none >::type,
                        make_equal_to_from_less< less > >::type,
                    make_equal_to_from_compare< compare > >::type,
                equal_to 
            >::type type;
        };
    }
    //@endcond

}}  // namespace cds::opt

#endif // #ifndef CDSLIB_OPT_COMPARE_H
