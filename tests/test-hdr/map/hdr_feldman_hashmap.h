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

#ifndef CDSTEST_HDR_FELDMAN_HASHMAP_H
#define CDSTEST_HDR_FELDMAN_HASHMAP_H

#include "cppunit/cppunit_proxy.h"

// forward declaration
namespace cds {
    namespace container {}
    namespace opt {}
}

namespace map {
    namespace cc = cds::container;
    namespace co = cds::opt;

    class FeldmanHashMapHdrTest : public CppUnitMini::TestCase
    {
        struct Item
        {
            unsigned int nInsertCall;
            unsigned int nFindCall;
            unsigned int nEraseCall;
            mutable unsigned int nIteratorCall;

            Item()
                : nInsertCall(0)
                , nFindCall(0)
                , nEraseCall(0)
                , nIteratorCall(0)
            {}

            explicit Item( unsigned int n )
                : nInsertCall(n)
                , nFindCall(0)
                , nEraseCall(0)
                , nIteratorCall(0)
            {}
        };

        struct hash128
        {
            size_t lo;
            size_t hi;

            hash128() {}
            hash128(size_t l, size_t h) : lo(l), hi(h) {}
            hash128( hash128 const& h) : lo(h.lo), hi(h.hi) {}

            struct make {
                hash128 operator()( size_t n ) const
                {
                    return hash128( std::hash<size_t>()( n ), std::hash<size_t>()( ~n ));
                }
                hash128 operator()( hash128 const& n ) const
                {
                    return hash128( std::hash<size_t>()( n.lo ), std::hash<size_t>()( ~n.hi ));
                }
            };

            struct less {
                bool operator()( hash128 const& lhs, hash128 const& rhs ) const
                {
                    if ( lhs.hi != rhs.hi )
                        return lhs.hi < rhs.hi;
                    return lhs.lo < rhs.lo;
                }
            };

            struct cmp {
                int operator()( hash128 const& lhs, hash128 const& rhs ) const
                {
                    if ( lhs.hi != rhs.hi )
                        return lhs.hi < rhs.hi ? -1 : 1;
                    return lhs.lo < rhs.lo ? -1 : lhs.lo == rhs.lo ? 0 : 1;
                }
            };

            friend bool operator==( hash128 const& lhs, hash128 const& rhs )
            {
                return cmp()( lhs, rhs ) == 0;
            }
            friend bool operator!=(hash128 const& lhs, hash128 const& rhs)
            {
                return !( lhs == rhs );
            }
        };

        template <typename Map>
        void test_hp( size_t nHeadBits, size_t nArrayBits )
        {
            typedef typename Map::hash_type hash_type;
            typedef typename Map::key_type key_type;
            typedef typename Map::mapped_type mapped_type;
            typedef typename Map::value_type value_type;
            typedef typename Map::guarded_ptr guarded_ptr;

            size_t const capacity = 1000;

            Map m( nHeadBits, nArrayBits );
            CPPUNIT_MSG("Array size: head=" << m.head_size() << ", array_node=" << m.array_node_size());
            //CPPUNIT_ASSERT(m.head_size() >= (size_t(1) << nHeadBits));
            //CPPUNIT_ASSERT(m.array_node_size() == (size_t(1) << nArrayBits));

            CPPUNIT_ASSERT(m.empty());
            CPPUNIT_ASSERT(m.size() == 0);

            // insert( key )/update()/get()/find()
            for ( size_t i = 0; i < capacity; ++i ) {
                size_t key = i * 57;
                CPPUNIT_ASSERT(!m.contains( key ))
                CPPUNIT_ASSERT(m.insert( key ));
                CPPUNIT_ASSERT(m.contains( key ));
                CPPUNIT_ASSERT(m.size() == i + 1);

                auto ret = m.update(key, [] ( value_type& v, value_type * old ) {
                    CPPUNIT_ASSERT_CURRENT( old != nullptr );
                    ++v.second.nInsertCall;
                }, false );
                CPPUNIT_ASSERT( ret.first );
                CPPUNIT_ASSERT( !ret.second );

                CPPUNIT_ASSERT(m.find(key, [](value_type& v) { ++v.second.nFindCall;} ));

                guarded_ptr gp{ m.get( key ) };
                CPPUNIT_ASSERT( gp );
                CPPUNIT_ASSERT( gp->first == key );
                CPPUNIT_ASSERT( gp->second.nInsertCall == 1 );
                CPPUNIT_ASSERT( gp->second.nFindCall == 1 );
            }
            CPPUNIT_ASSERT(!m.empty());
            CPPUNIT_ASSERT(m.size() == capacity);

            // iterator test
            size_t nCount = 0;
            for ( auto it = m.begin(), itEnd = m.end(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( it->second.nIteratorCall == 0 );
                CPPUNIT_ASSERT( it->second.nInsertCall == 1 );
                CPPUNIT_ASSERT( (*it).second.nFindCall == 1 );
                it->second.nIteratorCall += 1;
                ++nCount;
            }
            CPPUNIT_ASSERT( nCount == capacity );

            nCount = 0;
            for ( auto it = m.rbegin(), itEnd = m.rend(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( it->second.nInsertCall == 1 );
                CPPUNIT_ASSERT( (*it).second.nFindCall == 1 );
                CPPUNIT_ASSERT( it->second.nIteratorCall == 1 );
                (*it).second.nIteratorCall += 1;
                ++nCount;
            }
            CPPUNIT_ASSERT( nCount == capacity );

            nCount = 0;
            for ( auto it = m.cbegin(), itEnd = m.cend(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( it->second.nInsertCall == 1 );
                CPPUNIT_ASSERT( (*it).second.nFindCall == 1 );
                CPPUNIT_ASSERT( it->second.nIteratorCall == 2 );
                (*it).second.nIteratorCall += 1;
                ++nCount;
            }
            CPPUNIT_ASSERT( nCount == capacity );

            nCount = 0;
            for ( auto it = m.crbegin(), itEnd = m.crend(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( it->second.nInsertCall == 1 );
                CPPUNIT_ASSERT( (*it).second.nFindCall == 1 );
                CPPUNIT_ASSERT( it->second.nIteratorCall == 3 );
                (*it).second.nIteratorCall += 1;
                ++nCount;
            }
            CPPUNIT_ASSERT( nCount == capacity );

            // find
            for ( size_t i = 0; i < capacity; i++ ) {
                size_t key = i * 57;
                CPPUNIT_ASSERT( m.find( key, [key]( value_type& v ) {
                    CPPUNIT_ASSERT_CURRENT( v.first == key );
                    CPPUNIT_ASSERT_CURRENT( v.second.nInsertCall == 1 );
                    CPPUNIT_ASSERT_CURRENT( v.second.nFindCall == 1 );
                    CPPUNIT_ASSERT_CURRENT( v.second.nIteratorCall == 4 );
                }));
            }

            // erase
            for ( size_t i = capacity; i > 0; --i ) {
                size_t key = (i -1) * 57;
                guarded_ptr gp = m.get( key );
                CPPUNIT_ASSERT( gp );
                CPPUNIT_ASSERT( gp->first == key );
                CPPUNIT_ASSERT( gp->second.nInsertCall == 1 );
                CPPUNIT_ASSERT( gp->second.nFindCall == 1 );
                CPPUNIT_ASSERT( (*gp).second.nIteratorCall == 4 );

                CPPUNIT_ASSERT(m.erase( key ));

                gp = m.get( key );
                CPPUNIT_ASSERT( !gp );
                CPPUNIT_ASSERT(!m.contains( key ));
            }
            CPPUNIT_ASSERT( m.empty());
            CPPUNIT_ASSERT(m.size() == 0);

            // Iterators on empty map
            CPPUNIT_ASSERT(m.begin() == m.end());
            CPPUNIT_ASSERT(m.cbegin() == m.cend());
            CPPUNIT_ASSERT(m.rbegin() == m.rend());
            CPPUNIT_ASSERT(m.crbegin() == m.crend());

            // insert( key, val )
            for ( size_t i = 0; i < capacity; ++i ) {
                CPPUNIT_ASSERT(!m.contains(i));
                CPPUNIT_ASSERT(m.insert( i, (unsigned int) i * 100));
                CPPUNIT_ASSERT( m.contains(i));
                CPPUNIT_ASSERT( m.find( i, [i]( value_type& v ) {
                    CPPUNIT_ASSERT_CURRENT( v.first == i );
                    CPPUNIT_ASSERT_CURRENT( v.second.nInsertCall == i * 100 );
                }));
            }
            CPPUNIT_ASSERT( !m.empty());
            CPPUNIT_ASSERT(m.size() == capacity);

            // erase( key, func )
            for ( size_t i = 0; i < capacity; ++i ) {
                CPPUNIT_ASSERT( m.contains(i));
                CPPUNIT_ASSERT( m.erase( i, [i]( value_type& v ) {
                    CPPUNIT_ASSERT_CURRENT( v.first == i );
                    CPPUNIT_ASSERT_CURRENT( v.second.nInsertCall == i * 100 );
                    v.second.nInsertCall = 0;
                }));
            }
            CPPUNIT_ASSERT( m.empty());
            CPPUNIT_ASSERT(m.size() == 0 );

            // insert_with
            for ( size_t i = 0; i < capacity; ++i ) {
                size_t key = i * 121;
                CPPUNIT_ASSERT(!m.contains(key));
                CPPUNIT_ASSERT( m.insert_with( key, [key]( value_type& v ) {
                    CPPUNIT_ASSERT_CURRENT( v.first == key );
                    CPPUNIT_ASSERT_CURRENT( v.second.nInsertCall == 0 );
                    v.second.nInsertCall = decltype(v.second.nInsertCall)( key );
                }));
                CPPUNIT_ASSERT(m.find(key, [key] (value_type& v ) {
                    CPPUNIT_ASSERT_CURRENT( v.first == key );
                    CPPUNIT_ASSERT_CURRENT( v.second.nInsertCall == key );
                }));
                CPPUNIT_ASSERT(m.size() == i + 1);
            }
            CPPUNIT_ASSERT( !m.empty());
            CPPUNIT_ASSERT(m.size() == capacity);

            nCount = 0;
            for ( auto it = m.begin(), itEnd = m.end(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( it->first == it->second.nInsertCall );
                CPPUNIT_ASSERT( it->second.nIteratorCall == 0 );
                it->second.nIteratorCall += 1;
                ++nCount;
            }
            CPPUNIT_ASSERT( nCount == capacity );

            nCount = 0;
            for ( auto it = m.rbegin(), itEnd = m.rend(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( it->first == it->second.nInsertCall );
                CPPUNIT_ASSERT( it->second.nIteratorCall == 1 );
                it->second.nIteratorCall += 1;
                ++nCount;
            }
            CPPUNIT_ASSERT( nCount == capacity );

            // erase_at( iterator )
            nCount = 0;
            for ( auto it = m.begin(), itEnd = m.end(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( it->first == it->second.nInsertCall );
                CPPUNIT_ASSERT( it->second.nIteratorCall == 2 );
                CPPUNIT_ASSERT(m.erase_at( it ));
                ++nCount;
                CPPUNIT_ASSERT(!m.contains( it->first ));
            }
            CPPUNIT_ASSERT( nCount == capacity );
            CPPUNIT_ASSERT( m.empty());
            CPPUNIT_ASSERT(m.size() == 0 );

            // emplace
            for ( size_t i = 0; i < capacity; ++i ) {
                size_t key = i * 1023;
                CPPUNIT_ASSERT(!m.contains(key));
                CPPUNIT_ASSERT( m.emplace( key, (unsigned int) i ));
                CPPUNIT_ASSERT(m.find(key, [key] (value_type& v ) {
                    CPPUNIT_ASSERT_CURRENT( v.first == key );
                    CPPUNIT_ASSERT_CURRENT( v.second.nInsertCall * 1023 == key );
                }));
                CPPUNIT_ASSERT(m.size() == i + 1);
            }
            CPPUNIT_ASSERT( !m.empty());
            CPPUNIT_ASSERT(m.size() == capacity);

            // erase_at( reverse_iterator )
            nCount = 0;
            for ( auto it = m.rbegin(), itEnd = m.rend(); it != itEnd; ++it ) {
                CPPUNIT_ASSERT( it->first == it->second.nInsertCall * 1023 );
                CPPUNIT_ASSERT(m.erase_at( it ));
                ++nCount;
                CPPUNIT_ASSERT(!m.contains( it->first ));
            }
            CPPUNIT_ASSERT( nCount == capacity );
            CPPUNIT_ASSERT( m.empty());
            CPPUNIT_ASSERT(m.size() == 0 );


            // extract
            for ( size_t i = 0; i < capacity; ++i ) {
                size_t key = i * 711;
                CPPUNIT_ASSERT(!m.contains(key));
                auto ret = m.update( key, [i]( value_type& v, value_type * old ) {
                    CPPUNIT_ASSERT_CURRENT( old == nullptr );
                    v.second.nInsertCall = (unsigned int) i;
                });
                CPPUNIT_ASSERT( ret.first );
                CPPUNIT_ASSERT( ret.second );
                CPPUNIT_ASSERT(m.find(key, [i, key] (value_type& v ) {
                    CPPUNIT_ASSERT_CURRENT( v.first == key );
                    CPPUNIT_ASSERT_CURRENT( v.second.nInsertCall == i );
                }));
                CPPUNIT_ASSERT(m.size() == i + 1);
            }
            CPPUNIT_ASSERT( !m.empty());
            CPPUNIT_ASSERT(m.size() == capacity);

            for ( size_t i = capacity; i > 0; --i ) {
                size_t key = (i-1) * 711;
                guarded_ptr gp{ m.extract(key) };
                CPPUNIT_ASSERT( gp );
                CPPUNIT_ASSERT( gp->first == key );
                CPPUNIT_ASSERT((*gp).second.nInsertCall == i - 1 );
                gp = m.extract(key);
                CPPUNIT_ASSERT( !gp );
            }
            CPPUNIT_ASSERT( m.empty());
            CPPUNIT_ASSERT(m.size() == 0 );

            // clear
            for ( size_t i = 0; i < capacity; ++i ) {
                CPPUNIT_ASSERT(!m.contains( i ))
                CPPUNIT_ASSERT(m.insert( i ));
                CPPUNIT_ASSERT(m.contains( i ));
                CPPUNIT_ASSERT(m.size() == i + 1);
            }
            CPPUNIT_ASSERT( !m.empty());
            CPPUNIT_ASSERT(m.size() == capacity );

            m.clear();
            CPPUNIT_ASSERT( m.empty());
            CPPUNIT_ASSERT(m.size() == 0 );


            CPPUNIT_MSG( m.statistics() );
        }

        template <typename Map>
        void test_rcu(size_t nHeadBits, size_t nArrayBits)
        {
            typedef typename Map::hash_type hash_type;
            typedef typename Map::key_type key_type;
            typedef typename Map::mapped_type mapped_type;
            typedef typename Map::value_type value_type;
            typedef typename Map::exempt_ptr exempt_ptr;
            typedef typename Map::rcu_lock rcu_lock;

            size_t const capacity = 1000;

            Map m(nHeadBits, nArrayBits);
            CPPUNIT_MSG("Array size: head=" << m.head_size() << ", array_node=" << m.array_node_size());
            CPPUNIT_ASSERT(m.head_size() >= (size_t(1) << nHeadBits));
            CPPUNIT_ASSERT(m.array_node_size() == (size_t(1) << nArrayBits));

            CPPUNIT_ASSERT(m.empty());
            CPPUNIT_ASSERT(m.size() == 0);

            // insert( key )/update()/get()/find()
            for (size_t i = 0; i < capacity; ++i) {
                size_t key = i * 57;
                CPPUNIT_ASSERT(!m.contains(key))
                CPPUNIT_ASSERT(m.insert(key));
                CPPUNIT_ASSERT(m.contains(key));
                CPPUNIT_ASSERT(m.size() == i + 1);

                auto ret = m.update(key, [](value_type& v, value_type * old) {
                    CPPUNIT_ASSERT_CURRENT(old != nullptr);
                    ++v.second.nInsertCall;
                }, false);
                CPPUNIT_ASSERT(ret.first);
                CPPUNIT_ASSERT(!ret.second);

                CPPUNIT_ASSERT(m.find(key, [](value_type& v) { ++v.second.nFindCall;}));

                {
                    rcu_lock l;
                    value_type* p{ m.get(key) };
                    CPPUNIT_ASSERT(p);
                    CPPUNIT_ASSERT(p->first == key);
                    CPPUNIT_ASSERT(p->second.nInsertCall == 1);
                    CPPUNIT_ASSERT(p->second.nFindCall == 1);
                }
            }
            CPPUNIT_ASSERT(!m.empty());
            CPPUNIT_ASSERT(m.size() == capacity);

            // iterator test
            size_t nCount = 0;
            {
                rcu_lock l;
                for (auto it = m.begin(), itEnd = m.end(); it != itEnd; ++it) {
                    CPPUNIT_ASSERT(it->second.nIteratorCall == 0);
                    CPPUNIT_ASSERT(it->second.nInsertCall == 1);
                    CPPUNIT_ASSERT((*it).second.nFindCall == 1);
                    it->second.nIteratorCall += 1;
                    ++nCount;
                }
            }
            CPPUNIT_ASSERT(nCount == capacity);

            nCount = 0;
            {
                rcu_lock l;
                for (auto it = m.rbegin(), itEnd = m.rend(); it != itEnd; ++it) {
                    CPPUNIT_ASSERT(it->second.nInsertCall == 1);
                    CPPUNIT_ASSERT((*it).second.nFindCall == 1);
                    CPPUNIT_ASSERT(it->second.nIteratorCall == 1);
                    (*it).second.nIteratorCall += 1;
                    ++nCount;
                }
            }
            CPPUNIT_ASSERT(nCount == capacity);

            nCount = 0;
            {
                rcu_lock l;
                for (auto it = m.cbegin(), itEnd = m.cend(); it != itEnd; ++it) {
                    CPPUNIT_ASSERT(it->second.nInsertCall == 1);
                    CPPUNIT_ASSERT((*it).second.nFindCall == 1);
                    CPPUNIT_ASSERT(it->second.nIteratorCall == 2);
                    (*it).second.nIteratorCall += 1;
                    ++nCount;
                }
            }
            CPPUNIT_ASSERT(nCount == capacity);

            nCount = 0;
            {
                rcu_lock l;
                for (auto it = m.crbegin(), itEnd = m.crend(); it != itEnd; ++it) {
                    CPPUNIT_ASSERT(it->second.nInsertCall == 1);
                    CPPUNIT_ASSERT((*it).second.nFindCall == 1);
                    CPPUNIT_ASSERT(it->second.nIteratorCall == 3);
                    (*it).second.nIteratorCall += 1;
                    ++nCount;
                }
            }
            CPPUNIT_ASSERT(nCount == capacity);

            // find
            for (size_t i = 0; i < capacity; i++) {
                size_t key = i * 57;
                CPPUNIT_ASSERT(m.find(key, [key](value_type& v) {
                    CPPUNIT_ASSERT_CURRENT(v.first == key);
                    CPPUNIT_ASSERT_CURRENT(v.second.nInsertCall == 1);
                    CPPUNIT_ASSERT_CURRENT(v.second.nFindCall == 1);
                    CPPUNIT_ASSERT_CURRENT(v.second.nIteratorCall == 4);
                }));
            }

            // erase
            for (size_t i = capacity; i > 0; --i) {
                size_t key = (i - 1) * 57;
                {
                    rcu_lock l;
                    value_type* p = m.get(key);
                    CPPUNIT_ASSERT(p);
                    CPPUNIT_ASSERT(p->first == key);
                    CPPUNIT_ASSERT(p->second.nInsertCall == 1);
                    CPPUNIT_ASSERT(p->second.nFindCall == 1);
                    CPPUNIT_ASSERT(p->second.nIteratorCall == 4);
                }

                CPPUNIT_ASSERT(m.erase(key));

                {
                    rcu_lock l;
                    value_type* p = m.get(key);
                    CPPUNIT_ASSERT(!p);
                }
                CPPUNIT_ASSERT(!m.contains(key));
            }
            CPPUNIT_ASSERT(m.empty());
            CPPUNIT_ASSERT(m.size() == 0);

            // Iterators on empty map
            {
                rcu_lock l;
                CPPUNIT_ASSERT(m.begin() == m.end());
                CPPUNIT_ASSERT(m.cbegin() == m.cend());
                CPPUNIT_ASSERT(m.rbegin() == m.rend());
                CPPUNIT_ASSERT(m.crbegin() == m.crend());
            }

            // insert( key, val )
            for (size_t i = 0; i < capacity; ++i) {
                CPPUNIT_ASSERT(!m.contains(i));
                CPPUNIT_ASSERT(m.insert(i, (unsigned int)i * 100));
                CPPUNIT_ASSERT(m.contains(i));
                CPPUNIT_ASSERT(m.find(i, [i](value_type& v) {
                    CPPUNIT_ASSERT_CURRENT(v.first == i);
                    CPPUNIT_ASSERT_CURRENT(v.second.nInsertCall == i * 100);
                }));
            }
            CPPUNIT_ASSERT(!m.empty());
            CPPUNIT_ASSERT(m.size() == capacity);

            // erase( key, func )
            for (size_t i = 0; i < capacity; ++i) {
                CPPUNIT_ASSERT(m.contains(i));
                CPPUNIT_ASSERT(m.erase(i, [i](value_type& v) {
                    CPPUNIT_ASSERT_CURRENT(v.first == i);
                    CPPUNIT_ASSERT_CURRENT(v.second.nInsertCall == i * 100);
                    v.second.nInsertCall = 0;
                }));
            }
            CPPUNIT_ASSERT(m.empty());
            CPPUNIT_ASSERT(m.size() == 0);

            // insert_with
            for (size_t i = 0; i < capacity; ++i) {
                size_t key = i * 121;
                CPPUNIT_ASSERT(!m.contains(key));
                CPPUNIT_ASSERT(m.insert_with(key, [key](value_type& v) {
                    CPPUNIT_ASSERT_CURRENT(v.first == key);
                    CPPUNIT_ASSERT_CURRENT(v.second.nInsertCall == 0);
                    v.second.nInsertCall = decltype(v.second.nInsertCall)(key);
                }));
                CPPUNIT_ASSERT(m.find(key, [key](value_type& v) {
                    CPPUNIT_ASSERT_CURRENT(v.first == key);
                    CPPUNIT_ASSERT_CURRENT(v.second.nInsertCall == key);
                }));
                CPPUNIT_ASSERT(m.size() == i + 1);
            }
            CPPUNIT_ASSERT(!m.empty());
            CPPUNIT_ASSERT(m.size() == capacity);

            nCount = 0;
            {
                rcu_lock l;
                for (auto it = m.begin(), itEnd = m.end(); it != itEnd; ++it) {
                    CPPUNIT_ASSERT(it->first == it->second.nInsertCall);
                    CPPUNIT_ASSERT(it->second.nIteratorCall == 0);
                    it->second.nIteratorCall += 1;
                    ++nCount;
                }
            }
            CPPUNIT_ASSERT(nCount == capacity);

            nCount = 0;
            {
                rcu_lock l;
                for (auto it = m.rbegin(), itEnd = m.rend(); it != itEnd; ++it) {
                    CPPUNIT_ASSERT(it->first == it->second.nInsertCall);
                    CPPUNIT_ASSERT(it->second.nIteratorCall == 1);
                    it->second.nIteratorCall += 1;
                    ++nCount;
                }
            }
            CPPUNIT_ASSERT(nCount == capacity);

            // clear()
            m.clear();
            CPPUNIT_ASSERT(m.empty());
            CPPUNIT_ASSERT(m.size() == 0);

            // emplace
            for (size_t i = 0; i < capacity; ++i) {
                size_t key = i * 1023;
                CPPUNIT_ASSERT(!m.contains(key));
                CPPUNIT_ASSERT(m.emplace(key, static_cast<unsigned int>(i)));
                CPPUNIT_ASSERT(m.find(key, [key](value_type& v) {
                    CPPUNIT_ASSERT_CURRENT(v.first == key);
                    CPPUNIT_ASSERT_CURRENT(v.second.nInsertCall * 1023 == key);
                }));
                CPPUNIT_ASSERT(m.size() == i + 1);
            }
            CPPUNIT_ASSERT(!m.empty());
            CPPUNIT_ASSERT(m.size() == capacity);

            // extract
            for (size_t i = capacity; i > 0; --i) {
                size_t key = (i - 1) * 1023;
                exempt_ptr xp{ m.extract(key) };
                CPPUNIT_ASSERT(xp);
                CPPUNIT_ASSERT(xp->first == key);
                CPPUNIT_ASSERT((*xp).second.nInsertCall == static_cast<unsigned int>(i - 1));
                xp = m.extract(key);
                CPPUNIT_ASSERT(!xp);
            }
            CPPUNIT_ASSERT(m.empty());
            CPPUNIT_ASSERT(m.size() == 0);

            CPPUNIT_MSG(m.statistics());
        }

        void hp_nohash();
        void hp_nohash_stat();
        void hp_nohash_5_3();
        void hp_nohash_5_3_stat();
        void hp_stdhash();
        void hp_stdhash_stat();
        void hp_stdhash_5_3();
        void hp_stdhash_5_3_stat();
        void hp_hash128();
        void hp_hash128_stat();
        void hp_hash128_4_3();
        void hp_hash128_4_3_stat();

        void dhp_nohash();
        void dhp_nohash_stat();
        void dhp_nohash_5_3();
        void dhp_nohash_5_3_stat();
        void dhp_stdhash();
        void dhp_stdhash_stat();
        void dhp_stdhash_5_3();
        void dhp_stdhash_5_3_stat();
        void dhp_hash128();
        void dhp_hash128_stat();
        void dhp_hash128_4_3();
        void dhp_hash128_4_3_stat();

        void rcu_gpb_nohash();
        void rcu_gpb_nohash_stat();
        void rcu_gpb_nohash_5_3();
        void rcu_gpb_nohash_5_3_stat();
        void rcu_gpb_stdhash();
        void rcu_gpb_stdhash_stat();
        void rcu_gpb_stdhash_5_3();
        void rcu_gpb_stdhash_5_3_stat();
        void rcu_gpb_hash128();
        void rcu_gpb_hash128_stat();
        void rcu_gpb_hash128_4_3();
        void rcu_gpb_hash128_4_3_stat();

        void rcu_gpi_nohash();
        void rcu_gpi_nohash_stat();
        void rcu_gpi_nohash_5_3();
        void rcu_gpi_nohash_5_3_stat();
        void rcu_gpi_stdhash();
        void rcu_gpi_stdhash_stat();
        void rcu_gpi_stdhash_5_3();
        void rcu_gpi_stdhash_5_3_stat();
        void rcu_gpi_hash128();
        void rcu_gpi_hash128_stat();
        void rcu_gpi_hash128_4_3();
        void rcu_gpi_hash128_4_3_stat();

        void rcu_gpt_nohash();
        void rcu_gpt_nohash_stat();
        void rcu_gpt_nohash_5_3();
        void rcu_gpt_nohash_5_3_stat();
        void rcu_gpt_stdhash();
        void rcu_gpt_stdhash_stat();
        void rcu_gpt_stdhash_5_3();
        void rcu_gpt_stdhash_5_3_stat();
        void rcu_gpt_hash128();
        void rcu_gpt_hash128_stat();
        void rcu_gpt_hash128_4_3();
        void rcu_gpt_hash128_4_3_stat();

        void rcu_shb_nohash();
        void rcu_shb_nohash_stat();
        void rcu_shb_nohash_5_3();
        void rcu_shb_nohash_5_3_stat();
        void rcu_shb_stdhash();
        void rcu_shb_stdhash_stat();
        void rcu_shb_stdhash_5_3();
        void rcu_shb_stdhash_5_3_stat();
        void rcu_shb_hash128();
        void rcu_shb_hash128_stat();
        void rcu_shb_hash128_4_3();
        void rcu_shb_hash128_4_3_stat();

        void rcu_sht_nohash();
        void rcu_sht_nohash_stat();
        void rcu_sht_nohash_5_3();
        void rcu_sht_nohash_5_3_stat();
        void rcu_sht_stdhash();
        void rcu_sht_stdhash_stat();
        void rcu_sht_stdhash_5_3();
        void rcu_sht_stdhash_5_3_stat();
        void rcu_sht_hash128();
        void rcu_sht_hash128_stat();
        void rcu_sht_hash128_4_3();
        void rcu_sht_hash128_4_3_stat();

        CPPUNIT_TEST_SUITE(FeldmanHashMapHdrTest)
            CPPUNIT_TEST(hp_nohash)
            CPPUNIT_TEST(hp_nohash_stat)
            CPPUNIT_TEST(hp_nohash_5_3)
            CPPUNIT_TEST(hp_nohash_5_3_stat)
            CPPUNIT_TEST(hp_stdhash)
            CPPUNIT_TEST(hp_stdhash_stat)
            CPPUNIT_TEST(hp_stdhash_5_3)
            CPPUNIT_TEST(hp_stdhash_5_3_stat)
            CPPUNIT_TEST(hp_hash128)
            CPPUNIT_TEST(hp_hash128_stat)
            CPPUNIT_TEST(hp_hash128_4_3)
            CPPUNIT_TEST(hp_hash128_4_3_stat)

            CPPUNIT_TEST(dhp_nohash)
            CPPUNIT_TEST(dhp_nohash_stat)
            CPPUNIT_TEST(dhp_nohash_5_3)
            CPPUNIT_TEST(dhp_nohash_5_3_stat)
            CPPUNIT_TEST(dhp_stdhash)
            CPPUNIT_TEST(dhp_stdhash_stat)
            CPPUNIT_TEST(dhp_stdhash_5_3)
            CPPUNIT_TEST(dhp_stdhash_5_3_stat)
            CPPUNIT_TEST(dhp_hash128)
            CPPUNIT_TEST(dhp_hash128_stat)
            CPPUNIT_TEST(dhp_hash128_4_3)
            CPPUNIT_TEST(dhp_hash128_4_3_stat)

            CPPUNIT_TEST(rcu_gpb_nohash)
            CPPUNIT_TEST(rcu_gpb_nohash_stat)
            CPPUNIT_TEST(rcu_gpb_nohash_5_3)
            CPPUNIT_TEST(rcu_gpb_nohash_5_3_stat)
            CPPUNIT_TEST(rcu_gpb_stdhash)
            CPPUNIT_TEST(rcu_gpb_stdhash_stat)
            CPPUNIT_TEST(rcu_gpb_stdhash_5_3)
            CPPUNIT_TEST(rcu_gpb_stdhash_5_3_stat)
            CPPUNIT_TEST(rcu_gpb_hash128)
            CPPUNIT_TEST(rcu_gpb_hash128_stat)
            CPPUNIT_TEST(rcu_gpb_hash128_4_3)
            CPPUNIT_TEST(rcu_gpb_hash128_4_3_stat)

            CPPUNIT_TEST(rcu_gpi_nohash)
            CPPUNIT_TEST(rcu_gpi_nohash_stat)
            CPPUNIT_TEST(rcu_gpi_nohash_5_3)
            CPPUNIT_TEST(rcu_gpi_nohash_5_3_stat)
            CPPUNIT_TEST(rcu_gpi_stdhash)
            CPPUNIT_TEST(rcu_gpi_stdhash_stat)
            CPPUNIT_TEST(rcu_gpi_stdhash_5_3)
            CPPUNIT_TEST(rcu_gpi_stdhash_5_3_stat)
            CPPUNIT_TEST(rcu_gpi_hash128)
            CPPUNIT_TEST(rcu_gpi_hash128_stat)
            CPPUNIT_TEST(rcu_gpi_hash128_4_3)
            CPPUNIT_TEST(rcu_gpi_hash128_4_3_stat)

            CPPUNIT_TEST(rcu_gpt_nohash)
            CPPUNIT_TEST(rcu_gpt_nohash_stat)
            CPPUNIT_TEST(rcu_gpt_nohash_5_3)
            CPPUNIT_TEST(rcu_gpt_nohash_5_3_stat)
            CPPUNIT_TEST(rcu_gpt_stdhash)
            CPPUNIT_TEST(rcu_gpt_stdhash_stat)
            CPPUNIT_TEST(rcu_gpt_stdhash_5_3)
            CPPUNIT_TEST(rcu_gpt_stdhash_5_3_stat)
            CPPUNIT_TEST(rcu_gpt_hash128)
            CPPUNIT_TEST(rcu_gpt_hash128_stat)
            CPPUNIT_TEST(rcu_gpt_hash128_4_3)
            CPPUNIT_TEST(rcu_gpt_hash128_4_3_stat)

            CPPUNIT_TEST(rcu_shb_nohash)
            CPPUNIT_TEST(rcu_shb_nohash_stat)
            CPPUNIT_TEST(rcu_shb_nohash_5_3)
            CPPUNIT_TEST(rcu_shb_nohash_5_3_stat)
            CPPUNIT_TEST(rcu_shb_stdhash)
            CPPUNIT_TEST(rcu_shb_stdhash_stat)
            CPPUNIT_TEST(rcu_shb_stdhash_5_3)
            CPPUNIT_TEST(rcu_shb_stdhash_5_3_stat)
            CPPUNIT_TEST(rcu_shb_hash128)
            CPPUNIT_TEST(rcu_shb_hash128_stat)
            CPPUNIT_TEST(rcu_shb_hash128_4_3)
            CPPUNIT_TEST(rcu_shb_hash128_4_3_stat)

            CPPUNIT_TEST(rcu_sht_nohash)
            CPPUNIT_TEST(rcu_sht_nohash_stat)
            CPPUNIT_TEST(rcu_sht_nohash_5_3)
            CPPUNIT_TEST(rcu_sht_nohash_5_3_stat)
            CPPUNIT_TEST(rcu_sht_stdhash)
            CPPUNIT_TEST(rcu_sht_stdhash_stat)
            CPPUNIT_TEST(rcu_sht_stdhash_5_3)
            CPPUNIT_TEST(rcu_sht_stdhash_5_3_stat)
            CPPUNIT_TEST(rcu_sht_hash128)
            CPPUNIT_TEST(rcu_sht_hash128_stat)
            CPPUNIT_TEST(rcu_sht_hash128_4_3)
            CPPUNIT_TEST(rcu_sht_hash128_4_3_stat)
        CPPUNIT_TEST_SUITE_END()

    };

} // namespace map

#endif //#ifndef CDSTEST_HDR_FELDMAN_HASHMAP_H
