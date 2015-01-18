/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#ifndef __CDSUNIT_STD_SET_VC_H
#define __CDSUNIT_STD_SET_VC_H

#include <set>
#include <mutex>    //unique_lock

namespace set2 {
    template <typename Value, typename Less, typename Lock,
        class Alloc = typename CDS_DEFAULT_ALLOCATOR::template rebind<Value>::other
    >
    class StdSet: public std::set<Value, Less, Alloc>
    {
        Lock m_lock;
        typedef std::unique_lock<Lock> scoped_lock;
        typedef std::set<Value, Less, Alloc> base_class;
    public:
        typedef typename base_class::key_type value_type;

        StdSet( size_t /*nMapSize*/, size_t /*nLoadFactor*/ )
        {}

        template <typename Key>
        bool find( const Key& key )
        {
            value_type v( key );
            scoped_lock al( m_lock );
            return base_class::find( v ) != base_class::end();
        }

        bool insert( value_type const& v )
        {
            scoped_lock al( m_lock );
            return base_class::insert( v ).second;
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

                base_class::erase( it );
                return true;
            }
            return false;
        }

        std::ostream& dump( std::ostream& stm ) { return stm; }
    };
} // namespace set2

#endif  // #ifndef __CDSUNIT_STD_MAP_VC_H
