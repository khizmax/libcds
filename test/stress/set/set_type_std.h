// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSUNIT_SET_TYPE_STD_H
#define CDSUNIT_SET_TYPE_STD_H

#include <unordered_set>
#include <set>
#include <mutex>    //unique_lock
#include <memory>

#include "set_type.h"

namespace set {

    struct tag_StdSet;

    template <typename Value, typename Hash, typename Less, typename EqualTo, typename Lock,
        class Alloc = typename std::allocator_traits< 
                        CDS_DEFAULT_ALLOCATOR 
                      >::template rebind_alloc<Value>
    >
    class StdHashSet
        : public std::unordered_set<
            Value
            , Hash
            , EqualTo
            , Alloc
        >
    {
    public:
        Lock m_lock;
        typedef std::unique_lock<Lock> scoped_lock;
        typedef std::unordered_set<
            Value
            , Hash
            , EqualTo
            , Alloc
        >   base_class;

    public:
        typedef typename base_class::value_type value_type;

        template <class Config>
        StdHashSet( Config const& )
        {}

        template <typename Key>
        bool contains( const Key& key )
        {
            scoped_lock al( m_lock );
            return base_class::find( value_type(key)) != base_class::end();
        }

        template <typename Key>
        bool insert( Key const& key )
        {
            scoped_lock al( m_lock );
            std::pair<typename base_class::iterator, bool> pRet = base_class::insert( value_type( key ));
            return pRet.second;
        }

        template <typename Key, typename Func>
        bool insert( Key const& key, Func func )
        {
            scoped_lock al( m_lock );
            std::pair<typename base_class::iterator, bool> pRet = base_class::insert( value_type( key ));
            if ( pRet.second ) {
                func( *pRet.first );
                return true;
            }
            return false;
        }

        template <typename T, typename Func>
        std::pair<bool, bool> ensure( const T& key, Func func )
        {
            scoped_lock al( m_lock );
            std::pair<typename base_class::iterator, bool> pRet = base_class::insert( value_type( key ));
            if ( pRet.second ) {
                func( true, *pRet.first, key );
                return std::make_pair( true, true );
            }
            else {
                func( false, *pRet.first, key );
                return std::make_pair( true, false );
            }
        }

        template <typename Key>
        bool erase( const Key& key )
        {
            scoped_lock al( m_lock );
            return base_class::erase( value_type(key)) != 0;
        }

        template <typename T, typename Func>
        bool erase( const T& key, Func func )
        {
            scoped_lock al( m_lock );
            typename base_class::iterator it = base_class::find( value_type(key));
            if ( it != base_class::end()) {
                func( *it );
                return base_class::erase( it ) != base_class::end();
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
        static constexpr bool const c_bEraseExactKey = true;
    };

    template <typename Value, typename Less, typename Lock,
        class Alloc = typename std::allocator_traits<
                        CDS_DEFAULT_ALLOCATOR
                      >::template rebind_alloc<Value>
    >
    class StdSet: public std::set<Value, Less, Alloc>
    {
        Lock m_lock;
        typedef std::unique_lock<Lock> scoped_lock;
        typedef std::set<Value, Less, Alloc> base_class;
    public:
        typedef typename base_class::key_type value_type;

        template <class Config>
        StdSet( Config const& )
        {}

        template <typename Key>
        bool contains( const Key& key )
        {
            value_type v( key );
            scoped_lock al( m_lock );
            return base_class::find( v ) != base_class::end();
        }

        template <typename Key>
        bool insert( Key const& k )
        {
            scoped_lock al( m_lock );
            return base_class::insert( value_type( k )).second;
        }

        template <typename Key, typename Func>
        bool insert( Key const& key, Func func )
        {
            scoped_lock al( m_lock );
            std::pair<typename base_class::iterator, bool> pRet = base_class::insert( value_type( key ));
            if ( pRet.second ) {
                func( *pRet.first );
                return true;
            }
            return false;
        }

        template <typename T, typename Func>
        std::pair<bool, bool> ensure( const T& key, Func func )
        {
            scoped_lock al( m_lock );
            std::pair<typename base_class::iterator, bool> pRet = base_class::insert( value_type( key ));
            if ( pRet.second ) {
                func( true, *pRet.first, key );
                return std::make_pair( true, true );
            }
            else {
                func( false, *pRet.first, key );
                return std::make_pair( true, false );
            }
        }

        template <typename Key>
        bool erase( const Key& key )
        {
            scoped_lock al( m_lock );
            return base_class::erase( value_type(key)) != 0;
        }

        template <typename T, typename Func>
        bool erase( const T& key, Func func )
        {
            scoped_lock al( m_lock );
            typename base_class::iterator it = base_class::find( value_type(key));
            if ( it != base_class::end()) {
                func( *it );

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
    };

    template <typename Key, typename Val>
    struct set_type< tag_StdSet, Key, Val >: public set_type_base< Key, Val >
    {
        typedef set_type_base< Key, Val > base_class;
        typedef typename base_class::key_val key_val;
        typedef typename base_class::less less;
        typedef typename base_class::hash hash;
        typedef typename base_class::equal_to equal_to;

        typedef StdSet< key_val, less, cds::sync::spin > StdSet_Spin;
        typedef StdSet< key_val, less, std::mutex > StdSet_Mutex;
        //typedef StdSet< key_val, less, lock::NoLock>     StdSet_NoLock;

        typedef StdHashSet< key_val, hash, less, equal_to, cds::sync::spin > StdHashSet_Spin;
        typedef StdHashSet< key_val, hash, less, equal_to, std::mutex > StdHashSet_Mutex;
        //typedef StdHashSet< key_val, hash, less, equal_to, lock::NoLock >    StdHashSet_NoLock;
    };

} // namespace set

#define CDSSTRESS_StdSet_case( fixture, test_case, std_set_type, key_type, value_type ) \
    TEST_F( fixture, std_set_type ) \
    { \
        typedef set::set_type< tag_StdSet, key_type, value_type >::std_set_type set_type; \
        test_case<set_type>(); \
    }

#define CDSSTRESS_StdSet( fixture, test_case, key_type, value_type ) \
    CDSSTRESS_StdSet_case( fixture, test_case, StdSet_Spin,      key_type, value_type ) \
    CDSSTRESS_StdSet_case( fixture, test_case, StdSet_Mutex,     key_type, value_type ) \
    CDSSTRESS_StdSet_case( fixture, test_case, StdHashSet_Spin,  key_type, value_type ) \
    CDSSTRESS_StdSet_case( fixture, test_case, StdHashSet_Mutex, key_type, value_type )

#endif // #ifndef CDSUNIT_SET_TYPE_STD_H
