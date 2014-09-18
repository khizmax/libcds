//$$CDS-header$$

#ifndef __CDSUNIT_STD_MAP_VC_H
#define __CDSUNIT_STD_MAP_VC_H

#include <map>
#include <cds/ref.h>

namespace map2 {
    template <typename Key, typename Value, typename Lock, class Alloc = CDS_DEFAULT_ALLOCATOR>
    class StdMap: public std::map<Key, Value, std::less<Key>, Alloc>
    {
        Lock m_lock;
        typedef cds::lock::scoped_lock<Lock> AutoLock;
        typedef std::map<Key, Value, std::less<Key>, Alloc> base_class;
    public:
        typedef typename base_class::mapped_type value_type;
        typedef typename base_class::value_type  pair_type;
        typedef size_t      item_counter;

        StdMap( size_t nMapSize, size_t nLoadFactor )
        {}

        bool find( const Key& key )
        {
            AutoLock al( m_lock );
            return base_class::find( key ) != base_class::end();
        }

        bool insert( const Key& key, const Value& val )
        {
            AutoLock al( m_lock );
            return base_class::insert( base_class::value_type(key, val)).second;
        }

        template <typename T, typename Func>
        bool insert( const Key& key, const T& val, Func func )
        {
            AutoLock al( m_lock );
            std::pair<base_class::iterator, bool> pRet = base_class::insert( base_class::value_type(key, Value() ));
            if ( pRet.second ) {
                cds::unref(func)( pRet.first->second, val );
                return true;
            }
            return false;
        }

        template <typename T, typename Func>
        std::pair<bool, bool> ensure( const T& key, Func func )
        {
            AutoLock al( m_lock );
            std::pair<base_class::iterator, bool> pRet = base_class::insert( base_class::value_type(key, Value() ));
            if ( pRet.second ) {
                cds::unref(func)( true, *pRet.first );
                return std::make_pair( true, true );
            }
            else {
                cds::unref(func)( false, *pRet.first );
                return std::make_pair( true, false );
            }
        }

        bool erase( const Key& key )
        {
            AutoLock al( m_lock );
            return base_class::erase( key ) != 0;
        }

        template <typename T, typename Func>
        bool erase( const T& key, Func func )
        {
            AutoLock al( m_lock );
            base_class::iterator it = base_class::find( key );
            if ( it != base_class::end() ) {
                cds::unref(func)( *it );

                base_class::erase( it );
                return true;
            }
            return false;
        }

        std::ostream& dump( std::ostream& stm ) { return stm; }
    };
}

#endif  // #ifndef __CDSUNIT_STD_MAP_VC_H
