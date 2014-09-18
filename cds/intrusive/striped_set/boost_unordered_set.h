//$$CDS-header$$

#ifndef __CDS_INTRUSIVE_STRIPED_SET_BOOST_UNORDERED_SET_ADAPTER_H
#define __CDS_INTRUSIVE_STRIPED_SET_BOOST_UNORDERED_SET_ADAPTER_H

#include <boost/intrusive/unordered_set.hpp>
#include <cds/intrusive/striped_set/adapter.h>
#include <cds/opt/buffer.h>

//@cond
namespace cds { namespace intrusive { namespace striped_set {

    template <typename T, CDS_BOOST_INTRUSIVE_DECL_OPTIONS10, CDS_SPEC_OPTIONS>
    class adapt< boost::intrusive::unordered_set< T, CDS_BOOST_INTRUSIVE_OPTIONS10 >, CDS_OPTIONS >
    {
    public:
        typedef boost::intrusive::unordered_set< T, CDS_BOOST_INTRUSIVE_OPTIONS10 >  container_type  ;   ///< underlying intrusive container type

    private:
        class adapted_container
        {
        public:
            typedef typename container_type::value_type     value_type      ;   ///< value type stored in the container
            typedef typename container_type::iterator       iterator        ;   ///< container iterator
            typedef typename container_type::const_iterator const_iterator  ;   ///< container const iterator

            typedef typename opt::value<
                typename opt::find_option<
                    opt::buffer< opt::v::static_buffer< cds::any_type, 256 > >,
                    CDS_OPTIONS
                >::type
            >::buffer    initial_buffer_type;
            typedef typename initial_buffer_type::template rebind< typename container_type::bucket_type >::other    buffer_type;
            typedef cds::intrusive::striped_set::load_factor_resizing<256>   default_resizing_policy;

        private:
#       ifndef CDS_CXX11_LAMBDA_SUPPORT
            struct empty_insert_functor {
                void operator()( value_type& )
                {}
            };
#       endif

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

            buffer_type     m_Buckets   ;   // buffer should be declared first since it is used in m_Set ctor.
            container_type  m_Set;

        public:
            adapted_container()
                : m_Set( typename container_type::bucket_traits( m_Buckets.buffer(), m_Buckets.capacity() ))
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
                    cds::unref(f)( val );
                return res.second;
            }

            template <typename Func>
            std::pair<bool, bool> ensure( value_type& val, Func f )
            {
                std::pair<iterator, bool> res = m_Set.insert( val );
                cds::unref(f)( res.second, *res.first, val );
                return std::make_pair( true, res.second );
            }

            bool unlink( value_type& val )
            {
                iterator it = m_Set.find( value_type(val) );
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
                cds::unref(f)( val );
                m_Set.erase( it );
                return &val;
            }

            template <typename Q, typename Less, typename Func>
            value_type * erase( Q const& key, Less pred, Func f )
            {
                iterator it = m_Set.find( key, typename container_type::hasher(), equal_from_compare<Less>(pred) );
                if ( it == m_Set.end() )
                    return nullptr;
                value_type& val = *it;
                cds::unref(f)( val );
                m_Set.erase( it );
                return &val;
            }

            template <typename Q, typename Func>
            bool find( Q& key, Func f )
            {
                iterator it = m_Set.find( key, typename container_type::hasher(), typename container_type::key_equal() );
                if ( it == m_Set.end() )
                    return false;
                cds::unref(f)( *it, key );
                return true;
            }

            template <typename Q, typename Less, typename Func>
            bool find( Q& key, Less pred, Func f )
            {
                iterator it = m_Set.find( key, typename container_type::hasher(), equal_from_compare<Less>(pred) );
                if ( it == m_Set.end() )
                    return false;
                cds::unref(f)( *it, key );
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

            iterator begin()                { return m_Set.begin(); }
            const_iterator begin() const    { return m_Set.begin(); }
            iterator end()                  { return m_Set.end(); }
            const_iterator end() const      { return m_Set.end(); }

            size_t size() const
            {
                return (size_t) m_Set.size();
            }

            void move_item( adapted_container& from, iterator itWhat )
            {
                value_type& val = *itWhat;
                from.base_container().erase( itWhat );
#           ifdef CDS_CXX11_LAMBDA_SUPPORT
                insert( val, []( value_type& ) {} );
#           else
                insert( val, empty_insert_functor() );
#           endif
            }
        };

    public:
        typedef adapted_container   type ;  ///< Result of the metafunction

    };
}}} // namespace cds::intrusive::striped_set
//@endcond

#endif // #ifndef __CDS_INTRUSIVE_STRIPED_SET_BOOST_UNORDERED_SET_ADAPTER_H
