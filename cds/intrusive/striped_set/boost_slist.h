// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_INTRUSIVE_STRIPED_SET_BOOST_SLIST_ADAPTER_H
#define CDSLIB_INTRUSIVE_STRIPED_SET_BOOST_SLIST_ADAPTER_H

#include <boost/intrusive/slist.hpp>
#include <cds/intrusive/striped_set/adapter.h>

//@cond
namespace cds { namespace intrusive { namespace striped_set {

    namespace details {
        template <class List, typename... Options>
        class adapt_boost_slist
        {
        public:
            typedef List  container_type;   ///< underlying intrusive container type

        private:
            /// Adapted intrusive container
            class adapted_container : public cds::intrusive::striped_set::adapted_sequential_container
            {
            public:
                typedef typename container_type::value_type     value_type;   ///< value type stored in the container
                typedef typename container_type::iterator       iterator;   ///< container iterator
                typedef typename container_type::const_iterator const_iterator;    ///< container const iterator

                typedef typename cds::opt::details::make_comparator_from_option_list< value_type, Options... >::type key_comparator;

            private:

                template <typename Q, typename Less>
                std::pair< iterator, bool > find_prev_item( Q const& key, Less pred )
                {
                    iterator itPrev = m_List.before_begin();
                    iterator itEnd = m_List.end();
                    for ( iterator it = m_List.begin(); it != itEnd; ++it ) {
                        if ( pred( key, *it ))
                            itPrev = it;
                        else if ( pred( *it, key ))
                            break;
                        else
                            return std::make_pair( itPrev, true );
                    }
                    return std::make_pair( itPrev, false );
                }

                template <typename Q>
                std::pair< iterator, bool > find_prev_item( Q const& key )
                {
                    return find_prev_item_cmp( key, key_comparator());
                }

                template <typename Q, typename Compare>
                std::pair< iterator, bool > find_prev_item_cmp( Q const& key, Compare cmp )
                {
                    iterator itPrev = m_List.before_begin();
                    iterator itEnd = m_List.end();
                    for ( iterator it = m_List.begin(); it != itEnd; ++it ) {
                        int nCmp = cmp( key, *it );
                        if ( nCmp < 0 )
                            itPrev = it;
                        else if ( nCmp > 0 )
                            break;
                        else
                            return std::make_pair( itPrev, true );
                    }
                    return std::make_pair( itPrev, false );
                }

                template <typename Q, typename Compare, typename Func>
                value_type * erase_( Q const& key, Compare cmp, Func f )
                {
                    std::pair< iterator, bool > pos = find_prev_item_cmp( key, cmp );
                    if ( !pos.second )
                        return nullptr;

                    // key exists
                    iterator it = pos.first;
                    value_type& val = *(++it);
                    f( val );
                    m_List.erase_after( pos.first );

                    return &val;
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
                    std::pair< iterator, bool > pos = find_prev_item( val );
                    if ( !pos.second ) {
                        m_List.insert_after( pos.first, val );
                        f( val );
                        return true;
                    }

                    // key already exists
                    return false;
                }

                template <typename Func>
                std::pair<bool, bool> update( value_type& val, Func f, bool bAllowInsert )
                {
                    std::pair< iterator, bool > pos = find_prev_item( val );
                    if ( !pos.second ) {
                        // insert new
                        if ( !bAllowInsert )
                            return std::make_pair( false, false );

                        m_List.insert_after( pos.first, val );
                        f( true, val, val );
                        return std::make_pair( true, true );
                    }
                    else {
                        // already exists
                        f( false, *(++pos.first), val );
                        return std::make_pair( true, false );
                    }
                }

                bool unlink( value_type& val )
                {
                    std::pair< iterator, bool > pos = find_prev_item( val );
                    if ( !pos.second )
                        return false;

                    ++pos.first;
                    if ( &(*pos.first) != &val )
                        return false;

                    m_List.erase( pos.first );
                    return true;
                }

                template <typename Q, typename Func>
                value_type * erase( Q const& key, Func f )
                {
                    return erase_( key, key_comparator(), f );
                }

                template <typename Q, typename Less, typename Func>
                value_type * erase( Q const& key, Less /*pred*/, Func f )
                {
                    return erase_( key, cds::opt::details::make_comparator_from_less<Less>(), f );
                }

                template <typename Q, typename Func>
                bool find( Q& key, Func f )
                {
                    std::pair< iterator, bool > pos = find_prev_item( key );
                    if ( !pos.second )
                        return false;

                    // key exists
                    f( *(++pos.first), key );
                    return true;
                }

                template <typename Q, typename Less, typename Func>
                bool find( Q& key, Less pred, Func f )
                {
                    std::pair< iterator, bool > pos = find_prev_item( key, pred );
                    if ( !pos.second )
                        return false;

                    // key exists
                    f( *(++pos.first), key );
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

                iterator begin() { return m_List.begin(); }
                const_iterator begin() const { return m_List.begin(); }
                iterator end() { return m_List.end(); }
                const_iterator end() const { return m_List.end(); }

                size_t size() const
                {
                    return (size_t)m_List.size();
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
    template <typename T, typename P1, typename P2, typename P3, typename P4, typename P5, typename... Options>
    class adapt< boost::intrusive::slist< T, P1, P2, P3, P4, P5 >, Options... >
        : public details::adapt_boost_slist< boost::intrusive::slist< T, P1, P2, P3, P4, P5 >, Options... >
    {};
#else
    template <typename T, typename... BIOptions, typename... Options>
    class adapt< boost::intrusive::slist< T, BIOptions... >, Options... >
        : public details::adapt_boost_slist< boost::intrusive::slist< T, BIOptions... >, Options... >
    {};
#endif

}}} // namespace cds::intrusive::striped_set
//@endcond

#endif // #ifndef CDSLIB_INTRUSIVE_STRIPED_SET_BOOST_SLIST_ADAPTER_H
