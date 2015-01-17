/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#ifndef __CDSUNIT_STD_HASH_SET_STD_H
#define __CDSUNIT_STD_HASH_SET_STD_H

#include <unordered_set>
#include <mutex>    //unique_lock

namespace set2 {

    template <typename Value, typename Hash, typename Less, typename EqualTo, typename Lock,
        class Alloc = typename CDS_DEFAULT_ALLOCATOR::template rebind<Value>::other
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

        StdHashSet( size_t /*nSetSize*/, size_t /*nLoadFactor*/ )
        {}

        template <typename Key>
        bool find( const Key& key )
        {
            scoped_lock al( m_lock );
            return base_class::find( value_type(key) ) != base_class::end();
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
            return base_class::erase( value_type(key) ) != 0;
        }

        template <typename T, typename Func>
        bool erase( const T& key, Func func )
        {
            scoped_lock al( m_lock );
            typename base_class::iterator it = base_class::find( value_type(key) );
            if ( it != base_class::end() ) {
                func( *it );
                return base_class::erase( it ) != base_class::end();
            }
            return false;
        }

        std::ostream& dump( std::ostream& stm ) { return stm; }
    };
}   // namespace set2

#endif  // #ifndef __CDSUNIT_STD_HASH_SET_STD_H
