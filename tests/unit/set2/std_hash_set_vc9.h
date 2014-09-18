//$$CDS-header$$

#ifndef __CDSUNIT_STD_HASH_SET_VC_H
#define __CDSUNIT_STD_HASH_SET_VC_H

#include <hash_set>
//#include <cds/ref.h>

namespace set2 {

    template <typename KeyVal, typename Hash, typename Less>
    struct hash_less: public stdext::hash_compare< KeyVal, Less >
    {
        typedef stdext::hash_compare< KeyVal, Less> base_class;
        size_t operator()(const KeyVal& kv) const
        {
            return Hash()(kv);
        }

        bool operator()(const KeyVal& kv1, const KeyVal& kv2) const
        {
            return base_class::operator()( kv1, kv2 );
        }
    };

    template <typename Value, typename Hash, typename Less, typename EqualTo, typename Lock, class Alloc = CDS_DEFAULT_ALLOCATOR>
    class StdHashSet: public stdext::hash_set<Value, hash_less<Value, Hash, Less >, Alloc>
    {
    public:
        Lock m_lock;
        typedef cds::lock::scoped_lock<Lock> AutoLock;
        typedef stdext::hash_set<Value, hash_less<Value, Hash, Less >, Alloc> base_class;

    public:
        typedef typename base_class::value_type  pair_type;

        StdHashSet( size_t nSetSize, size_t nLoadFactor )
        {}

        template <typename Key>
        bool find( const Key& key )
        {
            AutoLock al( m_lock );
            return base_class::find( value_type(key) ) != base_class::end();
        }

        template <typename Key>
        bool insert( Key const& key )
        {
            AutoLock al( m_lock );
            std::pair<base_class::iterator, bool> pRet = base_class::insert( value_type( key ));
            return pRet.second;
        }

        template <typename Key, typename Func>
        bool insert( Key const& key, Func func )
        {
            AutoLock al( m_lock );
            std::pair<base_class::iterator, bool> pRet = base_class::insert( value_type( key ));
            if ( pRet.second ) {
                cds::unref(func)( *pRet.first );
                return true;
            }
            return false;
        }

        template <typename T, typename Func>
        std::pair<bool, bool> ensure( const T& key, Func func )
        {
            AutoLock al( m_lock );
            std::pair<typename base_class::iterator, bool> pRet = base_class::insert( value_type( key ));
            if ( pRet.second ) {
                cds::unref(func)( true, *pRet.first, key );
                return std::make_pair( true, true );
            }
            else {
                cds::unref(func)( false, *pRet.first, key );
                return std::make_pair( true, false );
            }
        }

        template <typename Key>
        bool erase( const Key& key )
        {
            AutoLock al( m_lock );
            return base_class::erase( value_type(key) ) != 0;
        }

        template <typename T, typename Func>
        bool erase( const T& key, Func func )
        {
            AutoLock al( m_lock );
            base_class::iterator it = base_class::find( key );
            if ( it != base_class::end() ) {
                cds::unref(func)( *it );
                return base_class::erase( it ) != base_class::end();
            }
            return false;
        }


        std::ostream& dump( std::ostream& stm ) { return stm; }
    };
}   // namespace set2

#endif  // #ifndef __CDSUNIT_STD_HASH_SET_VC_H
