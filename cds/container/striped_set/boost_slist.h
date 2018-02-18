// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_STRIPED_SET_BOOST_SLIST_ADAPTER_H
#define CDSLIB_CONTAINER_STRIPED_SET_BOOST_SLIST_ADAPTER_H

#include <functional>   // ref
#include <cds/container/striped_set/adapter.h>
#include <boost/container/slist.hpp>

//@cond
namespace cds { namespace container {
    namespace striped_set {

        // Copy policy for boost::container::slist
        template <typename T, typename Alloc>
        struct copy_item_policy< boost::container::slist< T, Alloc > >
        {
            typedef boost::container::slist< T, Alloc > list_type;
            typedef typename list_type::iterator    iterator;

            void operator()( list_type& list, iterator itInsert, iterator itWhat )
            {
                list.insert_after( itInsert, *itWhat );
            }
        };

        // Swap policy for boost::container::slist
        template <typename T, typename Alloc>
        struct swap_item_policy< boost::container::slist< T, Alloc > >
        {
            typedef boost::container::slist< T, Alloc > list_type;
            typedef typename list_type::iterator    iterator;

            void operator()( list_type& list, iterator itInsert, iterator itWhat )
            {
                T newVal;
                itInsert = list.insert_after( itInsert, newVal );
                std::swap( *itInsert, *itWhat );
            }
        };

        // Move policy for boost::container::slist
        template <typename T, typename Alloc>
        struct move_item_policy< boost::container::slist< T, Alloc > >
        {
            typedef boost::container::slist< T, Alloc > list_type;
            typedef typename list_type::iterator    iterator;

            void operator()( list_type& list, iterator itInsert, iterator itWhat )
            {
                list.insert_after( itInsert, std::move( *itWhat ));
            }
        };

    }   // namespace striped_set
}} // namespace cds::container

namespace cds { namespace intrusive { namespace striped_set {

    /// boost::container::slist adapter for hash set bucket
    template <typename T, class Alloc, typename... Options>
    class adapt< boost::container::slist<T, Alloc>, Options... >
    {
    public:
        typedef boost::container::slist<T, Alloc>     container_type          ;   ///< underlying container type

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

            template <typename Q>
            std::pair< iterator, bool > find_prev_item( Q const& key )
            {
                iterator itPrev = m_List.before_begin();
                iterator itEnd = m_List.end();
                for ( iterator it = m_List.begin(); it != itEnd; ++it ) {
                    int nCmp = key_comparator()( key, *it );
                    if ( nCmp < 0 )
                        itPrev = it;
                    else if ( nCmp > 0 )
                        break;
                    else
                        return std::make_pair( itPrev, true );
                }
                return std::make_pair( itPrev, false );
            }

            template <typename Q, typename Less>
            std::pair< iterator, bool > find_prev_item( Q const& key, Less pred )
            {
                iterator itPrev = m_List.before_begin();
                iterator itEnd = m_List.end();
                for ( iterator it = m_List.begin(); it != itEnd; ++it ) {
                    if ( pred( key, *it ))
                        itPrev = it;
                    else if ( pred( *it, key ))
                        break;
                    else
                        return std::make_pair( itPrev, true );
                }
                return std::make_pair( itPrev, false );
            }

            //@endcond

        private:
            //@cond
            container_type  m_List;
            //@endcond

        public:
            adapted_container()
            {}

            template <typename Q, typename Func>
            bool insert( const Q& val, Func f )
            {
                std::pair< iterator, bool > pos = find_prev_item( val );
                if ( !pos.second ) {
                    value_type newItem( val );
                    pos.first = m_List.insert_after( pos.first, newItem );
                    f( *pos.first );
                    return true;
                }

                // key already exists
                return false;
            }

            template <typename... Args>
            bool emplace( Args&&... args )
            {
                value_type val( std::forward<Args>(args)... );
                std::pair< iterator, bool > pos = find_prev_item( val );
                if ( !pos.second ) {
                    m_List.emplace_after( pos.first, std::move( val ));
                    return true;
                }
                return false;
            }

            template <typename Q, typename Func>
            std::pair<bool, bool> update( const Q& val, Func func, bool bAllowInsert )
            {
                std::pair< iterator, bool > pos = find_prev_item( val );
                if ( !pos.second ) {
                    // insert new
                    if ( !bAllowInsert )
                        return std::make_pair( false, false );

                    value_type newItem( val );
                    pos.first = m_List.insert_after( pos.first, newItem );
                    func( true, *pos.first, val );
                    return std::make_pair( true, true );
                }
                else {
                    // already exists
                    func( false, *(++pos.first), val );
                    return std::make_pair( true, false );
                }
            }

            template <typename Q, typename Func>
            bool erase( Q const& key, Func f )
            {
                std::pair< iterator, bool > pos = find_prev_item( key );
                if ( !pos.second )
                    return false;

                // key exists
                iterator it = pos.first;
                f( *(++it));
                m_List.erase_after( pos.first );

                return true;
            }

            template <typename Q, typename Less, typename Func>
            bool erase( Q const& key, Less pred, Func f )
            {
                std::pair< iterator, bool > pos = find_prev_item( key, pred );
                if ( !pos.second )
                    return false;

                // key exists
                iterator it = pos.first;
                f( *(++it));
                m_List.erase_after( pos.first );

                return true;
            }

            template <typename Q, typename Func>
            bool find( Q& val, Func f )
            {
                std::pair< iterator, bool > pos = find_prev_item( val );
                if ( !pos.second )
                    return false;

                // key exists
                f( *(++pos.first), val );
                return true;
            }

            template <typename Q, typename Less, typename Func>
            bool find( Q& val, Less pred, Func f )
            {
                std::pair< iterator, bool > pos = find_prev_item( val, pred );
                if ( !pos.second )
                    return false;

                // key exists
                f( *(++pos.first), val );
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
                std::pair< iterator, bool > pos = find_prev_item( *itWhat );
                assert( !pos.second );

                copy_item()( m_List, pos.first, itWhat );
            }

            size_t size() const
            {
                return m_List.size();
            }
        };

    public:
        typedef adapted_container type ; ///< Result of \p adapt metafunction

    };
}}} // namespace cds::intrusive::striped_set


//@endcond

#endif // #ifndef CDSLIB_CONTAINER_STRIPED_SET_BOOST_SLIST_ADAPTER_H
