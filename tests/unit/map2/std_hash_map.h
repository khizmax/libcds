//$$CDS-header$$

#ifndef CDSUNIT_STD_HASH_MAP_GCC_H
#define CDSUNIT_STD_HASH_MAP_GCC_H

#include <mutex>    //unique_lock
#include <unordered_map>

namespace map2 {

    template <typename Key, typename Value, typename Lock,
        class Alloc = typename CDS_DEFAULT_ALLOCATOR::template rebind<std::pair<Key const, Value> >::other
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

        StdHashMap( size_t /*nMapSize*/, size_t /*nLoadFactor*/ )
        {}

        bool find( const Key& key )
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
            std::pair<typename base_class::iterator, bool> pRet = base_class::insert( typename base_class::value_type(key, Value() ));
            if ( pRet.second ) {
                func( pRet.first->second, val );
                return true;
            }
            return false;
        }

        template <typename T, typename Func>
        std::pair<bool, bool> ensure( const T& key, Func func )
        {
            scoped_lock al( m_lock );
            std::pair<typename base_class::iterator, bool> pRet = base_class::insert( typename base_class::value_type( key, Value() ));
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
            if ( it != base_class::end() ) {
                func( *it );
                return base_class::erase( key ) != 0;
            }
            return false;
        }

        std::ostream& dump( std::ostream& stm ) { return stm; }
    };
}   // namespace map2

#endif  // #ifndef CDSUNIT_STD_HASH_MAP_GCC_H
