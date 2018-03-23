// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_CONTAINER_STRIPED_MAP_STD_HASH_MAP_ADAPTER_H
#define CDSLIB_CONTAINER_STRIPED_MAP_STD_HASH_MAP_ADAPTER_H

#include <cds/container/striped_set/adapter.h>
#include <unordered_map>

//@cond
namespace cds { namespace container {
    namespace striped_set {

        // Copy policy for map
        template <typename Key, typename T, typename Hash, typename Pred, typename Alloc>
        struct copy_item_policy< std::unordered_map< Key, T, Hash, Pred, Alloc > >
        {
            typedef std::unordered_map< Key, T, Hash, Pred, Alloc > map_type;
            typedef typename map_type::value_type   pair_type;
            typedef typename map_type::iterator     iterator;

            void operator()( map_type& map, iterator itWhat )
            {
                map.insert( *itWhat );
            }
        };

        // Swap policy for map
        template <typename Key, typename T, typename Hash, typename Pred, typename Alloc>
        struct swap_item_policy< std::unordered_map< Key, T, Hash, Pred, Alloc > >
        {
            typedef std::unordered_map< Key, T, Hash, Pred, Alloc > map_type;
            typedef typename map_type::value_type   pair_type;
            typedef typename map_type::iterator     iterator;

            void operator()( map_type& map, iterator itWhat )
            {
                pair_type pair( itWhat->first, typename pair_type::second_type());
                std::pair<iterator, bool> res = map.insert( pair );
                assert( res.second );
                std::swap( res.first->second, itWhat->second );
            }
        };

        // Move policy for map
        template <typename Key, typename T, typename Hash, typename Pred, typename Alloc>
        struct move_item_policy< std::unordered_map< Key, T, Hash, Pred, Alloc > >
        {
            typedef std::unordered_map< Key, T, Hash, Pred, Alloc > map_type;
            typedef typename map_type::value_type pair_type;
            typedef typename map_type::iterator     iterator;

            void operator()( map_type& map, iterator itWhat )
            {
                map.insert( std::move( *itWhat ));
            }
        };
    }   // namespace striped_set
}} // namespace cds::container

namespace cds { namespace intrusive { namespace striped_set {

    /// std::unordered_map  adapter for hash map bucket
    template <typename Key, typename T, class Hash, class Pred, class Alloc, typename... Options>
    class adapt< std::unordered_map< Key, T, Hash, Pred, Alloc>, Options... >
    {
    public:
        typedef std::unordered_map< Key, T, Hash, Pred, Alloc>  container_type  ;   ///< underlying container type

    private:
        /// Adapted container type
        class adapted_container: public cds::container::striped_set::adapted_container
        {
        public:
            typedef typename container_type::value_type     value_type  ;   ///< value type stored in the container
            typedef typename container_type::key_type       key_type;
            typedef typename container_type::mapped_type    mapped_type;
            typedef typename container_type::iterator       iterator ;   ///< container iterator
            typedef typename container_type::const_iterator const_iterator ;    ///< container const iterator

            static bool const has_find_with = false;
            static bool const has_erase_with = false;

        private:
            //@cond
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
            //@endcond

        private:
            //@cond
            container_type  m_Map;
            //@endcond

        public:
            template <typename Q, typename Func>
            bool insert( const Q& key, Func f )
            {
                std::pair<iterator, bool> res = m_Map.insert( value_type( key_type( key ), mapped_type()));
                if ( res.second )
                    f( const_cast<value_type&>(*res.first));
                return res.second;
            }

            template <typename Q, typename... Args>
            bool emplace( Q&& key, Args&&... args )
            {
                std::pair<iterator, bool> res = m_Map.emplace( key_type( std::forward<Q>( key )), mapped_type( std::forward<Args>( args )...));
                return res.second;
            }

            template <typename Q, typename Func>
            std::pair<bool, bool> update( const Q& key, Func func, bool bAllowInsert )
            {
                if ( bAllowInsert ) {
                    std::pair<iterator, bool> res = m_Map.insert( value_type( key_type( key ), mapped_type()));
                    func( res.second, const_cast<value_type&>(*res.first));
                    return std::make_pair( true, res.second );
                }
                else {
                    auto it = m_Map.find( key_type( key ));
                    if ( it == end())
                        return std::make_pair( false, false );
                    func( false, *it );
                    return std::make_pair( true, false );
                }
            }

            template <typename Q, typename Func>
            bool erase( const Q& key, Func f )
            {
                iterator it = m_Map.find( key_type( key ));
                if ( it == m_Map.end())
                    return false;
                f( const_cast<value_type&>(*it));
                m_Map.erase( it );
                return true;
            }

            template <typename Q, typename Func>
            bool find( Q& key, Func f )
            {
                iterator it = m_Map.find( key_type( key ));
                if ( it == m_Map.end())
                    return false;
                f( const_cast<value_type&>(*it), key );
                return true;
            }

            void clear()
            {
                m_Map.clear();
            }

            iterator begin()                { return m_Map.begin(); }
            const_iterator begin() const    { return m_Map.begin(); }
            iterator end()                  { return m_Map.end(); }
            const_iterator end() const      { return m_Map.end(); }

            void move_item( adapted_container& /*from*/, iterator itWhat )
            {
                assert( m_Map.find( itWhat->first ) == m_Map.end());
                copy_item()( m_Map, itWhat );
            }

            size_t size() const
            {
                return m_Map.size();
            }
        };

    public:
        typedef adapted_container type ; ///< Result of \p adapt metafunction

    };
}}} // namespace cds::intrusive::striped_set


//@endcond

#endif  // #ifndef CDSLIB_CONTAINER_STRIPED_MAP_STD_HASH_MAP_ADAPTER_H
