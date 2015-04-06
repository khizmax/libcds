//$$CDS-header$$

#ifndef CDSUNIT_SET_TYPE_STD_H
#define CDSUNIT_SET_TYPE_STD_H

#include <unordered_set>
#include <set>
#include <mutex>    //unique_lock

#include "set2/set_type.h"

namespace set2 {

    struct std_implementation_tag;

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
        typedef std_implementation_tag implementation_tag;

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
        typedef std_implementation_tag implementation_tag;

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

    template <typename Key, typename Val>
    struct set_type< std_implementation_tag, Key, Val >: public set_type_base< Key, Val >
    {
        typedef set_type_base< Key, Val > base_class;
        typedef typename base_class::key_val key_val;
        typedef typename base_class::less less;
        typedef typename base_class::hash hash;
        typedef typename base_class::equal_to equal_to;

        typedef StdSet< key_val, less, cds::sync::spin > StdSet_Spin;
        typedef StdSet< key_val, less, lock::NoLock>     StdSet_NoLock;

        typedef StdHashSet< key_val, hash, less, equal_to, cds::sync::spin > StdHashSet_Spin;
        typedef StdHashSet< key_val, hash, less, equal_to, lock::NoLock >    StdHashSet_NoLock;
    };

} // namespace set2

#endif // #ifndef CDSUNIT_SET_TYPE_STD_H
