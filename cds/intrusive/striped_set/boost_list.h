//$$CDS-header$$

#ifndef __CDS_INTRUSIVE_STRIPED_SET_BOOST_LIST_ADAPTER_H
#define __CDS_INTRUSIVE_STRIPED_SET_BOOST_LIST_ADAPTER_H

#include <boost/intrusive/list.hpp>
#include <cds/intrusive/striped_set/adapter.h>

//@cond
namespace cds { namespace intrusive { namespace striped_set {

    template <typename T, CDS_BOOST_INTRUSIVE_DECL_OPTIONS3, typename... Options>
    class adapt< boost::intrusive::list< T, CDS_BOOST_INTRUSIVE_OPTIONS3 >, Options... >
    {
    public:
        typedef boost::intrusive::list< T, CDS_BOOST_INTRUSIVE_OPTIONS3 >  container_type  ;   ///< underlying intrusive container type

    private:
        /// Adapted intrusive container
        class adapted_container: public cds::intrusive::striped_set::adapted_sequential_container
        {
        public:
            typedef typename container_type::value_type     value_type  ;   ///< value type stored in the container
            typedef typename container_type::iterator       iterator ;   ///< container iterator
            typedef typename container_type::const_iterator const_iterator ;    ///< container const iterator
            typedef typename cds::opt::details::make_comparator_from_option_list< value_type, Options... >::type key_comparator;

        private:
            struct find_predicate
            {
                bool operator()( value_type const& i1, value_type const& i2) const
                {
                    return key_comparator()( i1, i2 ) < 0;
                }

                template <typename Q>
                bool operator()( Q const& i1, value_type const& i2) const
                {
                    return key_comparator()( i1, i2 ) < 0;
                }

                template <typename Q>
                bool operator()( value_type const& i1, Q const& i2) const
                {
                    return key_comparator()( i1, i2 ) < 0;
                }
            };

#       ifndef CDS_CXX11_LAMBDA_SUPPORT
            struct empty_insert_functor {
                void operator()( value_type& )
                {}
            };
#       endif

            template <typename Q, typename Pred>
            iterator find_key( Q const& key, Pred pred)
            {
                iterator itEnd = m_List.end();
                iterator it;
                for ( it = m_List.begin(); it != itEnd; ++it ) {
                    if ( !pred( *it, key ) )
                        break;
                }
                return it;
            }

        private:
            container_type  m_List;

        public:
            adapted_container()
            {}

            container_type& base_container()
            {
                return m_List;
            }

            template <typename Func>
            bool insert( value_type& val, Func f )
            {
                iterator it = find_key( val, find_predicate() );
                if ( it == m_List.end() || key_comparator()( val, *it ) != 0 ) {
                    m_List.insert( it, val );
                    cds::unref( f )( val );

                    return true;
                }

                // key already exists
                return false;
            }

            template <typename Func>
            std::pair<bool, bool> ensure( value_type& val, Func f )
            {
                iterator it = find_key( val, find_predicate() );
                if ( it == m_List.end() || key_comparator()( val, *it ) != 0 ) {
                    // insert new
                    m_List.insert( it, val );
                    cds::unref( f )( true, val, val );
                    return std::make_pair( true, true );
                }
                else {
                    // already exists
                    cds::unref( f )( false, *it, val );
                    return std::make_pair( true, false );
                }
            }

            bool unlink( value_type& val )
            {
                iterator it = find_key( val, find_predicate() );
                if ( it == m_List.end() || &(*it) != &val )
                    return false;

                m_List.erase( it );
                return true;
            }

            template <typename Q, typename Func>
            value_type * erase( Q const& key, Func f )
            {
                iterator it = find_key( key, find_predicate() );
                if ( it == m_List.end() || key_comparator()( key, *it ) != 0 )
                    return nullptr;

                // key exists
                value_type& val = *it;
                cds::unref( f )( val );
                m_List.erase( it );

                return &val;
            }

            template <typename Q, typename Less, typename Func>
            value_type * erase( Q const& key, Less pred, Func f )
            {
                iterator it = find_key( key, pred );
                if ( it == m_List.end() || pred( key, *it ) || pred( *it, key ) )
                    return nullptr;

                // key exists
                value_type& val = *it;
                cds::unref( f )( val );
                m_List.erase( it );

                return &val;
            }

            template <typename Q, typename Func>
            bool find( Q& key, Func f )
            {
                return find( key, find_predicate(), f );
            }

            template <typename Q, typename Less, typename Func>
            bool find( Q& key, Less pred, Func f )
            {
                iterator it = find_key( key, pred );
                if ( it == m_List.end() || pred( key, *it ) || pred( *it, key ))
                    return false;

                // key exists
                cds::unref( f )( *it, key );
                return true;
            }

            void clear()
            {
                m_List.clear();
            }

            template <typename Disposer>
            void clear( Disposer disposer )
            {
                m_List.clear_and_dispose( disposer );
            }

            iterator begin()                { return m_List.begin(); }
            const_iterator begin() const    { return m_List.begin(); }
            iterator end()                  { return m_List.end(); }
            const_iterator end() const      { return m_List.end(); }

            size_t size() const
            {
                return (size_t) m_List.size();
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

#endif // #ifndef __CDS_INTRUSIVE_STRIPED_SET_BOOST_LIST_ADAPTER_H
