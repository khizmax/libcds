// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_STRIPED_SET_BOOST_VECTOR_ADAPTER_H
#define CDSLIB_CONTAINER_STRIPED_SET_BOOST_VECTOR_ADAPTER_H

#include <boost/version.hpp>
#if BOOST_VERSION < 104800
#   error "For boost::container::vector you must use boost 1.48 or above"
#endif

#include <functional>   // ref
#include <algorithm>    // std::lower_bound
#include <utility>      // std::pair
#include <cds/container/striped_set/adapter.h>     // lower_bound
#include <boost/container/vector.hpp>

//@cond
namespace cds { namespace container {
    namespace striped_set {

        // Copy policy for boost::container::vector
        template <typename T, typename Alloc>
        struct copy_item_policy< boost::container::vector< T, Alloc > >
        {
            typedef boost::container::vector< T, Alloc > vector_type;
            typedef typename vector_type::iterator iterator;

            void operator()( vector_type& vec, iterator itInsert, iterator itWhat )
            {
                vec.insert( itInsert, *itWhat );
            }
        };

        // Swap policy for boost::container::vector
        template <typename T, typename Alloc>
        struct swap_item_policy< boost::container::vector< T, Alloc > >
        {
            typedef boost::container::vector< T, Alloc > vector_type;
            typedef typename vector_type::iterator iterator;

            void operator()( vector_type& vec, iterator itInsert, iterator itWhat )
            {
                typename vector_type::value_type newVal;
                itInsert = vec.insert( itInsert, newVal );
                std::swap( *itInsert, *itWhat );
            }
        };

        // Move policy for boost::container::vector
        template <typename T, typename Alloc>
        struct move_item_policy< boost::container::vector< T, Alloc > >
        {
            typedef boost::container::vector< T, Alloc > vector_type;
            typedef typename vector_type::iterator iterator;

            void operator()( vector_type& vec, iterator itInsert, iterator itWhat )
            {
                vec.insert( itInsert, std::move( *itWhat ));
            }
        };
    }   // namespace striped_set
}} // namespace cds::container

namespace cds { namespace intrusive { namespace striped_set {

    /// boost::container::vector adapter for hash set bucket
    template <typename T, class Alloc, typename... Options>
    class adapt< boost::container::vector<T, Alloc>, Options... >
    {
    public:
        typedef boost::container::vector<T, Alloc>     container_type          ;   ///< underlying container type

    private:
        /// Adapted container type
        class adapted_container: public cds::container::striped_set::adapted_sequential_container
        {
        public:
            typedef typename container_type::value_type value_type  ;   ///< value type stored in the container
            typedef typename container_type::iterator      iterator ;   ///< container iterator
            typedef typename container_type::const_iterator const_iterator ;    ///< container const iterator

            static bool const has_find_with = true;
            static bool const has_erase_with = true;

        private:
            //@cond
            typedef typename cds::opt::details::make_comparator_from_option_list< value_type, Options... >::type key_comparator;

            typedef typename cds::opt::select<
                typename cds::opt::value<
                    typename cds::opt::find_option<
                        cds::opt::copy_policy< cds::container::striped_set::move_item >
                        , Options...
                    >::type
                >::copy_policy
                , cds::container::striped_set::copy_item, cds::container::striped_set::copy_item_policy<container_type>
                , cds::container::striped_set::swap_item, cds::container::striped_set::swap_item_policy<container_type>
                , cds::container::striped_set::move_item, cds::container::striped_set::move_item_policy<container_type>
            >::type copy_item;

            struct find_predicate
            {
                bool operator()( value_type const& i1, value_type const& i2) const
                {
                    return key_comparator()( i1, i2 ) < 0;
                }

                template <typename Q>
                bool operator()( Q const& i1, value_type const& i2) const
                {
                    return key_comparator()( i1, i2 ) < 0;
                }

                template <typename Q>
                bool operator()( value_type const& i1, Q const& i2) const
                {
                    return key_comparator()( i1, i2 ) < 0;
                }
            };
            //@endcond

        private:
            //@cond
            container_type  m_Vector;
            //@endcond

        public:
            template <typename Q, typename Func>
            bool insert( const Q& val, Func f )
            {
                iterator it = std::lower_bound( m_Vector.begin(), m_Vector.end(), val, find_predicate());
                if ( it == m_Vector.end() || key_comparator()( val, *it ) != 0 ) {
                    value_type newItem( val );
                    it = m_Vector.insert( it, newItem );
                    f( *it );
                    return true;
                }
                return false;
            }

            template <typename... Args>
            bool emplace( Args&&... args )
            {
                value_type val( std::forward<Args>(args)... );
                iterator it = std::lower_bound( m_Vector.begin(), m_Vector.end(), val, find_predicate());
                if ( it == m_Vector.end() || key_comparator()( val, *it ) != 0 ) {
                    it = m_Vector.emplace( it, std::move( val ));
                    return true;
                }
                return false;
            }

            template <typename Q, typename Func>
            std::pair<bool, bool> update( const Q& val, Func func, bool bAllowInsert )
            {
                iterator it = std::lower_bound( m_Vector.begin(), m_Vector.end(), val, find_predicate());
                if ( it == m_Vector.end() || key_comparator()( val, *it ) != 0 ) {
                    // insert new
                    if ( !bAllowInsert )
                        return std::make_pair( false, false );

                    value_type newItem( val );
                    it = m_Vector.insert( it, newItem );
                    func( true, *it, val );
                    return std::make_pair( true, true );
                }
                else {
                    // already exists
                    func( false, *it, val );
                    return std::make_pair( true, false );
                }
            }

            template <typename Q, typename Func>
            bool erase( const Q& key, Func f )
            {
                iterator it = std::lower_bound( m_Vector.begin(), m_Vector.end(), key, find_predicate());
                if ( it == m_Vector.end() || key_comparator()( key, *it ) != 0 )
                    return false;

                // key exists
                f( *it );
                m_Vector.erase( it );
                return true;
            }

            template <typename Q, typename Less, typename Func>
            bool erase( Q const& key, Less pred, Func f )
            {
                iterator it = std::lower_bound( m_Vector.begin(), m_Vector.end(), key, pred );
                if ( it == m_Vector.end() || pred( key, *it ) || pred( *it, key ))
                    return false;

                // key exists
                f( *it );
                m_Vector.erase( it );
                return true;
            }

            template <typename Q, typename Func>
            bool find( Q& val, Func f )
            {
                iterator it = std::lower_bound( m_Vector.begin(), m_Vector.end(), val, find_predicate());
                if ( it == m_Vector.end() || key_comparator()( val, *it ) != 0 )
                    return false;

                // key exists
                f( *it, val );
                return true;
            }

            template <typename Q, typename Less, typename Func>
            bool find( Q& val, Less pred, Func f )
            {
                iterator it = std::lower_bound( m_Vector.begin(), m_Vector.end(), val, pred );
                if ( it == m_Vector.end() || pred( val, *it ) || pred( *it, val ))
                    return false;

                // key exists
                f( *it, val );
                return true;
            }

            /// Clears the container
            void clear()
            {
                m_Vector.clear();
            }

            iterator begin()                { return m_Vector.begin(); }
            const_iterator begin() const    { return m_Vector.begin(); }
            iterator end()                  { return m_Vector.end(); }
            const_iterator end() const      { return m_Vector.end(); }

            void move_item( adapted_container& /*from*/, iterator itWhat )
            {
                iterator it = std::lower_bound( m_Vector.begin(), m_Vector.end(), *itWhat, find_predicate());
                assert( it == m_Vector.end() || key_comparator()( *itWhat, *it ) != 0 );

                copy_item()( m_Vector, it, itWhat );
            }

            size_t size() const
            {
                return m_Vector.size();
            }
        };

    public:
        typedef adapted_container type ; ///< Result of \p adapt metafunction

    };
}}} // namespace cds::intrusive::striped_set


//@endcond

#endif // #ifndef CDSLIB_CONTAINER_STRIPED_SET_BOOST_VECTOR_ADAPTER_H
