//$$CDS-header$$

#ifndef CDSLIB_DETAILS_COMPARATOR_H
#define CDSLIB_DETAILS_COMPARATOR_H

#include <functional>
#include <string>

namespace cds {
    /// Helper classes and functions
    namespace details {
        /// Comparator
        /**
            Comparator is a functor (a class with binary @a operator() ) that compares two values.
            The comparator is based on \p std::less<T> functor and returns the result
            of comparing of two values:
            \li -1 if a < b
            \li 0 if a == b
            \li 1 if a > b
        */
        template <typename T>
        class Comparator {
            //@cond
            std::less< T > m_cmp;
            //@endcond
        public:
            typedef T        value_type    ;    ///< Type of values to compare

            /// Compare method
            /**
                @return -1 if @p p1 < @p p2 \n
                0 if @p p1 == @p p2 \n
                1 if @p p1 > @p p2 \n
            */
            int operator()( const T& p1, const T& p2 ) const
            {
                if ( m_cmp( p1, p2 ) )
                    return -1;
                if ( m_cmp( p2, p1 ))
                    return 1;
                return 0;
            }
        };

        /// String specialization. It uses @a string::compare method
        template <>
        class Comparator< std::string > {
        public:
            //@cond
            typedef std::string        value_type    ;    ///< Type of values to compare

            int operator()( const std::string& str1, const std::string& str2 ) const
            {
                return str1.compare( str2 );
            }
            //@endcond
        };
    }    // namespace details
}    // namespace cds

#endif // #ifndef CDSLIB_DETAILS_COMPARATOR_H
