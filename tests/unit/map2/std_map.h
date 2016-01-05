/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.     
*/

#ifndef CDSUNIT_STD_MAP_GCC_H
#define CDSUNIT_STD_MAP_GCC_H

#include <map>
#include <mutex>    //unique_lock

namespace map2 {

    template <typename Key, typename Value, typename Lock,
        class Alloc = typename CDS_DEFAULT_ALLOCATOR::template rebind<std::pair<Key const, Value> >::other
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
        std::pair<bool, bool> update( const T& key, Func func, bool /*bAllowInsert*/ = true )
        {
            scoped_lock al( m_lock );
            std::pair<typename base_class::iterator, bool> pRet = base_class::insert( typename base_class::value_type(key, Value() ));
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
                func( (*it) );
                base_class::erase( it );
                return true;
            }
            return false;
        }

        std::ostream& dump( std::ostream& stm ) { return stm; }


        // for testing
        static CDS_CONSTEXPR bool const c_bExtractSupported = false;
        static CDS_CONSTEXPR bool const c_bLoadFactorDepended = false;
        static CDS_CONSTEXPR bool const c_bEraseExactKey = false;
    };
}   // namespace map

#endif  // #ifndef CDSUNIT_STD_MAP_GCC_H
