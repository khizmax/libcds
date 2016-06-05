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

#ifndef CDSLIB_INTRUSIVE_STRIPED_SET_BOOST_UNORDERED_SET_ADAPTER_H
#define CDSLIB_INTRUSIVE_STRIPED_SET_BOOST_UNORDERED_SET_ADAPTER_H

#include <boost/intrusive/unordered_set.hpp>
#include <cds/intrusive/striped_set/adapter.h>
#include <cds/opt/buffer.h>

//@cond
namespace cds { namespace intrusive { namespace striped_set {

    namespace details {
        template <class Set, typename... Options>
        class adapt_boost_unordered_set
        {
        public:
            typedef Set container_type;   ///< underlying intrusive container type

        private:
            class adapted_container
            {
            public:
                typedef typename container_type::value_type     value_type;   ///< value type stored in the container
                typedef typename container_type::iterator       iterator;   ///< container iterator
                typedef typename container_type::const_iterator const_iterator;   ///< container const iterator

                typedef typename opt::value <
                    typename opt::find_option <
                        opt::buffer< opt::v::initialized_static_buffer< cds::any_type, 256 > >,
                        Options...
                    > ::type
                > ::buffer    initial_buffer_type;
                typedef typename initial_buffer_type::template rebind< typename container_type::bucket_type >::other buffer_type;
                typedef cds::intrusive::striped_set::load_factor_resizing<256>   default_resizing_policy;

            private:
                template <typename Compare>
                struct equal_from_compare
                {
                    Compare& m_cmp;
                    equal_from_compare( Compare& cmp )
                        : m_cmp( cmp )
                    {}

                    equal_from_compare( equal_from_compare const& src )
                        : m_cmp( src.m_cmp )
                    {}

                    template <typename A, typename B>
                    bool operator()( A& a, B& b ) const
                    {
                        return !m_cmp( a, b ) && !m_cmp( b, a );
                    }

                    template <typename A, typename B>
                    bool operator()( A& a, B& b )
                    {
                        return !m_cmp( a, b ) && !m_cmp( b, a );
                    }
                };

                buffer_type     m_Buckets;   // buffer should be declared first since it is used in m_Set ctor.
                container_type  m_Set;

            public:
                adapted_container()
                    : m_Set( typename container_type::bucket_traits( m_Buckets.buffer(), m_Buckets.capacity() ) )
                {}

                container_type& base_container()
                {
                    return m_Set;
                }

                template <typename Func>
                bool insert( value_type& val, Func f )
                {
                    std::pair<iterator, bool> res = m_Set.insert( val );
                    if ( res.second )
                        f( val );
                    return res.second;
                }

                template <typename Func>
                std::pair<bool, bool> update( value_type& val, Func f, bool bAllowInsert )
                {
                    if ( bAllowInsert ) {
                        std::pair<iterator, bool> res = m_Set.insert( val );
                        f( res.second, *res.first, val );
                        return std::make_pair( true, res.second );
                    }
                    else {
                        auto it = m_Set.find( val );
                        if ( it == m_Set.end() )
                            return std::make_pair( false, false );
                        f( false, *it, val );
                        return std::make_pair( true, false );
                    }
                }

                bool unlink( value_type& val )
                {
                    iterator it = m_Set.find( value_type( val ) );
                    if ( it == m_Set.end() || &(*it) != &val )
                        return false;
                    m_Set.erase( it );
                    return true;
                }

                template <typename Q, typename Func>
                value_type * erase( Q const& key, Func f )
                {
                    iterator it = m_Set.find( key, typename container_type::hasher(), typename container_type::key_equal() );
                    if ( it == m_Set.end() )
                        return nullptr;
                    value_type& val = *it;
                    f( val );
                    m_Set.erase( it );
                    return &val;
                }

                template <typename Q, typename Less, typename Func>
                value_type * erase( Q const& key, Less pred, Func f )
                {
                    iterator it = m_Set.find( key, typename container_type::hasher(), equal_from_compare<Less>( pred ) );
                    if ( it == m_Set.end() )
                        return nullptr;
                    value_type& val = *it;
                    f( val );
                    m_Set.erase( it );
                    return &val;
                }

                template <typename Q, typename Func>
                bool find( Q& key, Func f )
                {
                    iterator it = m_Set.find( key, typename container_type::hasher(), typename container_type::key_equal() );
                    if ( it == m_Set.end() )
                        return false;
                    f( *it, key );
                    return true;
                }

                template <typename Q, typename Less, typename Func>
                bool find( Q& key, Less pred, Func f )
                {
                    iterator it = m_Set.find( key, typename container_type::hasher(), equal_from_compare<Less>( pred ) );
                    if ( it == m_Set.end() )
                        return false;
                    f( *it, key );
                    return true;
                }

                void clear()
                {
                    m_Set.clear();
                }

                template <typename Disposer>
                void clear( Disposer disposer )
                {
                    m_Set.clear_and_dispose( disposer );
                }

                iterator begin() { return m_Set.begin(); }
                const_iterator begin() const { return m_Set.begin(); }
                iterator end() { return m_Set.end(); }
                const_iterator end() const { return m_Set.end(); }

                size_t size() const
                {
                    return (size_t)m_Set.size();
                }

                void move_item( adapted_container& from, iterator itWhat )
                {
                    value_type& val = *itWhat;
                    from.base_container().erase( itWhat );
                    insert( val, []( value_type& ) {} );
                }
            };

        public:
            typedef adapted_container   type;  ///< Result of the metafunction
        };
    } // namespace details

#if CDS_COMPILER == CDS_COMPILER_INTEL && CDS_COMPILER_VERSION <= 1500
    template <typename T,
        typename O1, typename O2, typename O3, typename O4, typename O5,
        typename O6, typename O7, typename O8, typename O9, typename O10,
        typename... Options
    >
    class adapt < boost::intrusive::unordered_set< T, O1, O2, O3, O4, O5, O6, O7, O8, O9, O10 >, Options... >
        : public details::adapt_boost_unordered_set < boost::intrusive::unordered_set< T, O1, O2, O3, O4, O5, O6, O7, O8, O9, O10 >, Options... >
    {};
#else
    template <typename T, typename... BIOptons, typename... Options>
    class adapt < boost::intrusive::unordered_set< T, BIOptons... >, Options... >
        : public details::adapt_boost_unordered_set < boost::intrusive::unordered_set< T, BIOptons... >, Options... >
    {};
#endif

}}} // namespace cds::intrusive::striped_set
//@endcond

#endif // #ifndef CDSLIB_INTRUSIVE_STRIPED_SET_BOOST_UNORDERED_SET_ADAPTER_H
