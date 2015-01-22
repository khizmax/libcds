//$$CDS-header$$

#ifndef CDSLIB_CONTAINER_DETAILS_MAKE_MICHAEL_KVLIST_H
#define CDSLIB_CONTAINER_DETAILS_MAKE_MICHAEL_KVLIST_H

#include <cds/details/binary_functor_wrapper.h>

namespace cds { namespace container {

    //@cond
    namespace details {

        template <class GC, typename K, typename T, class Traits>
        struct make_michael_kvlist
        {
            typedef Traits original_type_traits;

            typedef GC       gc;
            typedef K        key_type;
            typedef T        value_type;
            typedef std::pair<key_type const, value_type>       pair_type;

            struct node_type: public intrusive::michael_list::node<gc>
            {
                pair_type               m_Data;

                template <typename Q>
                node_type( Q const& key )
                    : m_Data( key, value_type() )
                {}

                template <typename Q, typename R>
                explicit node_type( std::pair<Q, R> const& pair )
                    : m_Data( pair )
                {}

                template <typename Q, typename R>
                node_type( Q const& key, R const& value )
                    : m_Data( key, value )
                {}

                template< typename Ky, typename... Args>
                node_type( Ky&& key, Args&&... args )
                    : m_Data( std::forward<Ky>(key), std::move( value_type( std::forward<Args>(args)...)))
                {}
            };

            typedef typename original_type_traits::allocator::template rebind<node_type>::other  allocator_type;
            typedef cds::details::Allocator< node_type, allocator_type >                cxx_allocator;

            struct node_deallocator
            {
                void operator ()( node_type * pNode )
                {
                    cxx_allocator().Delete( pNode );
                }
            };

            struct key_field_accessor {
                key_type const& operator()( node_type const& pair )
                {
                    return pair.m_Data.first;
                }
            };

            typedef typename opt::details::make_comparator< key_type, original_type_traits >::type key_comparator;

            template <typename Less>
            struct less_wrapper {
                typedef cds::details::compare_wrapper< node_type, cds::opt::details::make_comparator_from_less<Less>, key_field_accessor >    type;
            };

            struct intrusive_traits: public original_type_traits
            {
                typedef intrusive::michael_list::base_hook< opt::gc<gc> >  hook;
                typedef node_deallocator                    disposer;
                typedef cds::details::compare_wrapper< node_type, key_comparator, key_field_accessor > compare;
                static const opt::link_check_type link_checker = intrusive::michael_list::traits::link_checker;
            };

            typedef intrusive::MichaelList<gc, node_type, intrusive_traits>  type;
        };
    }   // namespace details
    //@endcond

}}  // namespace cds::container

#endif  // #ifndef CDSLIB_CONTAINER_DETAILS_MAKE_MICHAEL_KVLIST_H
