// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_STRIPED_MAP_STD_LIST_ADAPTER_H
#define CDSLIB_CONTAINER_STRIPED_MAP_STD_LIST_ADAPTER_H

#include <list>
#include <functional>   // ref
#include <algorithm>    // std::lower_bound
#include <utility>      // std::pair
#include <cds/container/striped_set/adapter.h>

#undef CDS_STD_LIST_SIZE_CXX11_CONFORM
#if !( defined(__GLIBCXX__ ) && (!defined(_GLIBCXX_USE_CXX11_ABI) || _GLIBCXX_USE_CXX11_ABI == 0 ))
#   define CDS_STD_LIST_SIZE_CXX11_CONFORM
#endif

//@cond
namespace cds { namespace container {
    namespace striped_set {

        // Copy policy for map
        template <typename K, typename T, typename Alloc>
        struct copy_item_policy< std::list< std::pair< K const, T >, Alloc > >
        {
            typedef std::pair< K const, T>  pair_type;
            typedef std::list< pair_type, Alloc > list_type;
            typedef typename list_type::iterator    iterator;

            void operator()( list_type& list, iterator itInsert, iterator itWhat )
            {
                list.insert( itInsert, *itWhat );
            }
        };

        // Swap policy for map
        template <typename K, typename T, typename Alloc>
        struct swap_item_policy< std::list< std::pair< K const, T >, Alloc > >
        {
            typedef std::pair< K const, T>  pair_type;
            typedef std::list< pair_type, Alloc > list_type;
            typedef typename list_type::iterator    iterator;

            void operator()( list_type& list, iterator itInsert, iterator itWhat )
            {
                pair_type newVal( itWhat->first, typename pair_type::second_type());
                itInsert = list.insert( itInsert, newVal );
                std::swap( itInsert->second, itWhat->second );
            }
        };

        // Move policy for map
        template <typename K, typename T, typename Alloc>
        struct move_item_policy< std::list< std::pair< K const, T >, Alloc > >
        {
            typedef std::pair< K const, T>          pair_type;
            typedef std::list< pair_type, Alloc >   list_type;
            typedef typename list_type::iterator    iterator;

            void operator()( list_type& list, iterator itInsert, iterator itWhat )
            {
                list.insert( itInsert, std::move( *itWhat ));
            }
        };
    } // namespace striped_set
}} // namespace cds:container

namespace cds { namespace intrusive { namespace striped_set {

    /// std::list adapter for hash map bucket
    template <typename Key, typename T, class Alloc, typename... Options>
    class adapt< std::list< std::pair<Key const, T>, Alloc>, Options... >
    {
    public:
        typedef std::list< std::pair<Key const, T>, Alloc>     container_type          ;   ///< underlying container type

    private:
        /// Adapted container type
        class adapted_container: public cds::container::striped_set::adapted_sequential_container
        {
        public:
            typedef typename container_type::value_type     value_type  ;   ///< value type stored in the container
            typedef typename value_type::first_type         key_type;
            typedef typename value_type::second_type        mapped_type;
            typedef typename container_type::iterator       iterator    ;   ///< container iterator
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
                    return key_comparator()( i1.first, i2.first ) < 0;
                }

                template <typename Q>
                bool operator()( Q const& i1, value_type const& i2) const
                {
                    return key_comparator()( i1, i2.first ) < 0;
                }

                template <typename Q>
                bool operator()( value_type const& i1, Q const& i2) const
                {
                    return key_comparator()( i1.first, i2 ) < 0;
                }
            };
            //@endcond

        private:
            //@cond
            container_type  m_List;
#       if !defined(CDS_STD_LIST_SIZE_CXX11_CONFORM)
            // GCC C++ lib bug:
            // In GCC (at least up to 4.7.x), the complexity of std::list::size() is O(N)
            // (see http://gcc.gnu.org/bugzilla/show_bug.cgi?id=49561)
            // Fixed in GCC 5
            size_t          m_nSize ;   // list size
#       endif
            //@endcond

        public:
            adapted_container()
#       if !defined(CDS_STD_LIST_SIZE_CXX11_CONFORM)
                : m_nSize(0)
#       endif
            {}

            template <typename Q, typename Func>
            bool insert( const Q& key, Func f )
            {
                iterator it = std::lower_bound( m_List.begin(), m_List.end(), key, find_predicate());
                if ( it == m_List.end() || key_comparator()( key, it->first ) != 0 ) {
                    it = m_List.insert( it, value_type( key_type( key ), mapped_type()));
                    f( *it );

#           if !defined(CDS_STD_LIST_SIZE_CXX11_CONFORM)
                    ++m_nSize;
#           endif
                    return true;
                }

                // key already exists
                return false;
            }

            template <typename K, typename... Args>
            bool emplace( K&& key, Args&&... args )
            {
                value_type val( key_type( std::forward<K>( key )), mapped_type( std::forward<Args>( args )... ));
                iterator it = std::lower_bound( m_List.begin(), m_List.end(), val.first, find_predicate());
                if ( it == m_List.end() || key_comparator()( val.first, it->first ) != 0 ) {
                    it = m_List.emplace( it, std::move( val ));

#           if !defined(CDS_STD_LIST_SIZE_CXX11_CONFORM)
                    ++m_nSize;
#           endif
                    return true;
                }
                return false;
            }

            template <typename Q, typename Func>
            std::pair<bool, bool> update( const Q& key, Func func, bool bAllowInsert )
            {
                iterator it = std::lower_bound( m_List.begin(), m_List.end(), key, find_predicate());
                if ( it == m_List.end() || key_comparator()( key, it->first ) != 0 ) {
                    // insert new
                    if ( !bAllowInsert )
                        return std::make_pair( false, false );

                    it = m_List.insert( it, value_type( key_type( key ), mapped_type()));
                    func( true, *it );
#           if !defined(CDS_STD_LIST_SIZE_CXX11_CONFORM)
                    ++m_nSize;
#           endif
                    return std::make_pair( true, true );
                }
                else {
                    // already exists
                    func( false, *it );
                    return std::make_pair( true, false );
                }
            }

            template <typename Q, typename Func>
            bool erase( Q const& key, Func f )
            {
                iterator it = std::lower_bound( m_List.begin(), m_List.end(), key, find_predicate());
                if ( it == m_List.end() || key_comparator()( key, it->first ) != 0 )
                    return false;

                // key exists
                f( *it );
                m_List.erase( it );
#           if !defined(CDS_STD_LIST_SIZE_CXX11_CONFORM)
                --m_nSize;
#           endif

                return true;
            }

            template <typename Q, typename Less, typename Func>
            bool erase( Q const& key, Less pred, Func f )
            {
                iterator it = std::lower_bound( m_List.begin(), m_List.end(), key, pred );
                if ( it == m_List.end() || pred( key, it->first ) || pred( it->first, key ))
                    return false;

                // key exists
                f( *it );
                m_List.erase( it );
#           if !defined(CDS_STD_LIST_SIZE_CXX11_CONFORM)
                --m_nSize;
#           endif

                return true;
            }

            template <typename Q, typename Func>
            bool find( Q& val, Func f )
            {
                iterator it = std::lower_bound( m_List.begin(), m_List.end(), val, find_predicate());
                if ( it == m_List.end() || key_comparator()( val, it->first ) != 0 )
                    return false;

                // key exists
                f( *it, val );
                return true;
            }

            template <typename Q, typename Less, typename Func>
            bool find( Q& val, Less pred, Func f )
            {
                iterator it = std::lower_bound( m_List.begin(), m_List.end(), val, pred );
                if ( it == m_List.end() || pred( val, it->first ) || pred( it->first, val ))
                    return false;

                // key exists
                f( *it, val );
                return true;
            }

            void clear()
            {
                m_List.clear();
            }

            iterator begin()                { return m_List.begin(); }
            const_iterator begin() const    { return m_List.begin(); }
            iterator end()                  { return m_List.end(); }
            const_iterator end() const      { return m_List.end(); }

            void move_item( adapted_container& /*from*/, iterator itWhat )
            {
                iterator it = std::lower_bound( m_List.begin(), m_List.end(), *itWhat, find_predicate());
                assert( it == m_List.end() || key_comparator()( itWhat->first, it->first ) != 0 );

                copy_item()( m_List, it, itWhat );
#           if !defined(CDS_STD_LIST_SIZE_CXX11_CONFORM)
                ++m_nSize;
#           endif
            }

            size_t size() const
            {
#           if !defined(CDS_STD_LIST_SIZE_CXX11_CONFORM)
                return m_nSize;
#           else
                return m_List.size();
#           endif

            }
        };

    public:
        typedef adapted_container type ; ///< Result of \p adapt metafunction

    };
}}} // namespace cds::intrusive::striped_set

//@endcond

#endif // #ifndef CDSLIB_CONTAINER_STRIPED_MAP_STD_LIST_ADAPTER_H
