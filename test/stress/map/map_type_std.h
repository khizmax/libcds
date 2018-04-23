// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_MAP_TYPE_STD_H
#define CDSUNIT_MAP_TYPE_STD_H

#include "map_type.h"

#include <map>
#include <unordered_map>
#include <memory>

namespace map {

    struct empty_lock
    {
        void lock() {}
        void unlock() {}
    };

    template <typename Key, typename Value, typename Lock,
        class Alloc = typename std::allocator_traits<
                    CDS_DEFAULT_ALLOCATOR
                >::template rebind_alloc<std::pair<Key const, Value> >
    >
        class StdMap: public std::map<Key, Value, std::less<Key>, Alloc>
    {
        Lock m_lock;
        typedef std::unique_lock<Lock> scoped_lock;
        typedef std::map<Key, Value, std::less<Key>, Alloc> base_class;
    public:
        typedef typename base_class::mapped_type value_type;
        typedef typename base_class::value_type  pair_type;
        typedef size_t      item_counter;

        StdMap()
        {}

        template <class Config>
        StdMap( Config const& )
        {}

        bool contains( const Key& key )
        {
            scoped_lock al( m_lock );
            return base_class::find( key ) != base_class::end();
        }

        bool insert( const Key& key, const Value& val )
        {
            scoped_lock al( m_lock );
            return base_class::insert( typename base_class::value_type( key, val )).second;
        }

        template <typename T, typename Func>
        bool insert( const Key& key, const T& val, Func func )
        {
            scoped_lock al( m_lock );
            std::pair<typename base_class::iterator, bool> pRet = base_class::insert( typename base_class::value_type( key, Value()));
            if ( pRet.second ) {
                func( pRet.first->second, val );
                return true;
            }
            return false;
        }

        template <typename T, typename Func>
        std::pair<bool, bool> update( const T& key, Func func, bool /*bAllowInsert*/ = true )
        {
            scoped_lock al( m_lock );
            std::pair<typename base_class::iterator, bool> pRet = base_class::insert( typename base_class::value_type( key, Value()));
            if ( pRet.second ) {
                func( true, *pRet.first );
                return std::make_pair( true, true );
            }
            else {
                func( false, *pRet.first );
                return std::make_pair( true, false );
            }
        }

        bool erase( const Key& key )
        {
            scoped_lock al( m_lock );
            return base_class::erase( key ) != 0;
        }

        template <typename T, typename Func>
        bool erase( const T& key, Func func )
        {
            scoped_lock al( m_lock );
            typename base_class::iterator it = base_class::find( key );
            if ( it != base_class::end()) {
                func( (*it));
                base_class::erase( it );
                return true;
            }
            return false;
        }

        empty_stat statistics() const
        {
            return empty_stat();
        }

        // for testing
        static constexpr bool const c_bExtractSupported = false;
        static constexpr bool const c_bLoadFactorDepended = false;
        static constexpr bool const c_bEraseExactKey = false;
    };

    template <typename Key, typename Value, typename Lock,
        class Alloc = typename std::allocator_traits<
                        CDS_DEFAULT_ALLOCATOR
                      >::template rebind_alloc<std::pair<Key const, Value> >
    >
    class StdHashMap
        : public std::unordered_map<
            Key, Value
            , std::hash<Key>
            , std::equal_to<Key>
            , Alloc
        >
    {
    public:
        Lock m_lock;
        typedef std::unique_lock<Lock> scoped_lock;
        typedef std::unordered_map<
            Key, Value
            , std::hash<Key>
            , std::equal_to<Key>
            , Alloc
        >   base_class;
    public:
        typedef typename base_class::mapped_type value_type;
        typedef size_t      item_counter;

        StdHashMap()
        {}

        template <class Config>
        StdHashMap( Config const& )
        {}

        bool contains( const Key& key )
        {
            scoped_lock al( m_lock );
            return base_class::find( key ) != base_class::end();
        }

        bool insert( const Key& key, const Value& val )
        {
            scoped_lock al( m_lock );
            return base_class::insert( typename base_class::value_type(key, val)).second;
        }

        template <typename T, typename Func>
        bool insert( const Key& key, const T& val, Func func )
        {
            scoped_lock al( m_lock );
            std::pair<typename base_class::iterator, bool> pRet = base_class::insert( typename base_class::value_type(key, Value()));
            if ( pRet.second ) {
                func( pRet.first->second, val );
                return true;
            }
            return false;
        }

        template <typename T, typename Func>
        std::pair<bool, bool> update( const T& key, Func func, bool /*bAllowInsert*/ = true )
        {
            scoped_lock al( m_lock );
            std::pair<typename base_class::iterator, bool> pRet = base_class::insert( typename base_class::value_type( key, Value()));
            if ( pRet.second ) {
                func( true, *pRet.first );
                return std::make_pair( true, true );
            }
            else {
                func( false, *pRet.first );
                return std::make_pair( true, false );
            }
        }

        bool erase( const Key& key )
        {
            scoped_lock al( m_lock );
            return base_class::erase( key ) != 0;
        }

        template <typename T, typename Func>
        bool erase( const T& key, Func func )
        {
            scoped_lock al( m_lock );
            typename base_class::iterator it = base_class::find( key );
            if ( it != base_class::end()) {
                func( *it );
                return base_class::erase( key ) != 0;
            }
            return false;
        }

        empty_stat statistics() const
        {
            return empty_stat();
        }


        // for testing
        static constexpr bool const c_bExtractSupported = false;
        static constexpr bool const c_bLoadFactorDepended = false;
        static constexpr bool const c_bEraseExactKey = false;
    };

    struct tag_StdMap;

    template <typename Key, typename Value>
    struct map_type< tag_StdMap, Key, Value >: public map_type_base< Key, Value >
    {
        typedef map_type_base< Key, Value >      base_class;
        typedef typename base_class::key_compare compare;
        typedef typename base_class::key_less    less;

        typedef StdMap< Key, Value, cds::sync::spin > StdMap_Spin;
        typedef StdMap< Key, Value, std::mutex >      StdMap_Mutex;
        typedef StdMap< Key, Value, empty_lock>       StdMap_NoLock;

        typedef StdHashMap< Key, Value, cds::sync::spin > StdHashMap_Spin;
        typedef StdHashMap< Key, Value, std::mutex >      StdHashMap_Mutex;
        typedef StdHashMap< Key, Value, empty_lock >      StdHashMap_NoLock;
    };
}   // namespace map


#define CDSSTRESS_StdMap_case( fixture, test_case, std_map_type, key_type, value_type ) \
    TEST_F( fixture, std_map_type ) \
    { \
        typedef map::map_type< tag_StdMap, key_type, value_type >::std_map_type map_type; \
        test_case<map_type>(); \
    }

#define CDSSTRESS_StdMap( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_StdMap_case( fixture, test_case, StdMap_Spin,      key_type, value_type ) \
    CDSSTRESS_StdMap_case( fixture, test_case, StdMap_Mutex,     key_type, value_type ) \
    CDSSTRESS_StdMap_case( fixture, test_case, StdHashMap_Spin,  key_type, value_type ) \
    CDSSTRESS_StdMap_case( fixture, test_case, StdHashMap_Mutex, key_type, value_type )

#define CDSSTRESS_StdMap_nolock( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_StdMap_case( fixture, test_case, StdMap_NoLock,      key_type, value_type ) \
    CDSSTRESS_StdMap_case( fixture, test_case, StdHashMap_NoLock,  key_type, value_type )

#endif // ifndef CDSUNIT_MAP_TYPE_STD_H
