//$$CDS-header$$

#ifndef __CDS_CONTAINER_STRIPED_MAP_STD_HASH_MAP_STD_ADAPTER_H
#define __CDS_CONTAINER_STRIPED_MAP_STD_HASH_MAP_STD_ADAPTER_H

#ifndef __CDS_CONTAINER_STRIPED_MAP_STD_HASH_MAP_ADAPTER_H
#   error <cds/container/striped_map/std_hash_map.h> must be included instead of <cds/container/striped_map/std_hash_map_std.h> header
#endif

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
                pair_type pair( itWhat->first, typename pair_type::second_type() );
                std::pair<iterator, bool> res = map.insert( pair );
                assert( res.second );
                std::swap( res.first->second, itWhat->second );
            }
        };

#ifdef CDS_MOVE_SEMANTICS_SUPPORT
        // Move policy for map
        template <typename Key, typename T, typename Hash, typename Pred, typename Alloc>
        struct move_item_policy< std::unordered_map< Key, T, Hash, Pred, Alloc > >
        {
            typedef std::unordered_map< Key, T, Hash, Pred, Alloc > map_type;
            typedef typename map_type::value_type pair_type;
            typedef typename map_type::iterator     iterator;

            void operator()( map_type& map, iterator itWhat )
            {
                map.insert( std::move( *itWhat ) );
            }
        };
#endif
    }   // namespace striped_set
}} // namespace cds::container

namespace cds { namespace intrusive { namespace striped_set {

    /// std::unordered_map  adapter for hash map bucket
    template <typename Key, typename T, class Hash, class Pred, class Alloc, CDS_SPEC_OPTIONS>
    class adapt< std::unordered_map< Key, T, Hash, Pred, Alloc>, CDS_OPTIONS >
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
                        , CDS_OPTIONS
                    >::type
                >::copy_policy
                , cds::container::striped_set::copy_item, cds::container::striped_set::copy_item_policy<container_type>
                , cds::container::striped_set::swap_item, cds::container::striped_set::swap_item_policy<container_type>
#ifdef CDS_MOVE_SEMANTICS_SUPPORT
                , cds::container::striped_set::move_item, cds::container::striped_set::move_item_policy<container_type>
#endif
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
                std::pair<iterator, bool> res = m_Map.insert( value_type( key, mapped_type() ));
                if ( res.second )
                    ::cds::unref(f)( const_cast<value_type&>(*res.first) );
                return res.second;
            }

#       ifdef CDS_EMPLACE_SUPPORT
            template <typename Q, typename... Args>
            bool emplace( Q&& key, Args&&... args )
            {
#           if CDS_COMPILER == CDS_COMPILER_GCC && CDS_COMPILER_VERSION < 40800 || CDS_COMPILER == CDS_COMPILER_CLANG && !defined(__LIBCPP_VERSION)
                // GCC < 4.8: std::map has no "emplace" member function. Emulate it
                std::pair<iterator, bool> res = m_Map.insert( value_type( std::forward<Q>(key), mapped_type( std::forward<Args>(args)...)));
#           else
                std::pair<iterator, bool> res = m_Map.emplace( std::forward<Q>(key), std::move( mapped_type(std::forward<Args>(args)...)) );
#           endif
                return res.second;
            }
#       endif

            template <typename Q, typename Func>
            std::pair<bool, bool> ensure( const Q& key, Func func )
            {
                std::pair<iterator, bool> res = m_Map.insert( value_type( key, mapped_type() ) );
                cds::unref(func)( res.second, const_cast<value_type&>(*res.first));
                return std::make_pair( true, res.second );
            }

            template <typename Q, typename Func>
            bool erase( const Q& key, Func f )
            {
                iterator it = m_Map.find( key_type(key) );
                if ( it == m_Map.end() )
                    return false;
                ::cds::unref(f)( const_cast<value_type&>(*it) );
                m_Map.erase( it );
                return true;
            }

            template <typename Q, typename Func>
            bool find( Q& val, Func f )
            {
                iterator it = m_Map.find( key_type(val) );
                if ( it == m_Map.end() )
                    return false;
                ::cds::unref(f)( const_cast<value_type&>(*it), val );
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
                assert( m_Map.find( itWhat->first ) == m_Map.end() );
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

#endif  // #ifndef __CDS_CONTAINER_STRIPED_MAP_STD_HASH_MAP_STD_ADAPTER_H
