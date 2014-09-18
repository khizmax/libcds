//$$CDS-header$$

#ifndef __CDS_CONTAINER_STRIPED_SET_BOOST_UNORDERED_SET_ADAPTER_H
#define __CDS_CONTAINER_STRIPED_SET_BOOST_UNORDERED_SET_ADAPTER_H

#include <cds/container/striped_set/adapter.h>
#include <boost/unordered_set.hpp>

//@cond
namespace cds { namespace container {
    namespace striped_set {

        // Copy policy for boost::unordered_set
        template <typename T, typename Traits, typename Alloc>
        struct copy_item_policy< boost::unordered_set< T, Traits, Alloc > >
            : public details::boost_set_copy_policies< boost::unordered_set< T, Traits, Alloc > >::copy_item_policy
        {};

        template <typename T, typename Traits, typename Alloc>
        struct swap_item_policy< boost::unordered_set< T, Traits, Alloc > >
            : public details::boost_set_copy_policies< boost::unordered_set< T, Traits, Alloc > >::swap_item_policy
        {};

#ifdef CDS_MOVE_SEMANTICS_SUPPORT
        // Move policy for boost::unordered_set
        template <typename T, typename Traits, typename Alloc>
        struct move_item_policy< boost::unordered_set< T, Traits, Alloc > >
            : public details::boost_set_copy_policies< boost::unordered_set< T, Traits, Alloc > >::move_item_policy
        {};
#endif
    }   // namespace striped_set
}} // namespace cds::container

namespace cds { namespace intrusive { namespace striped_set {
    /// boost::unordered_set adapter for hash set bucket
    template <typename T, class Traits, class Alloc, CDS_SPEC_OPTIONS>
    class adapt< boost::unordered_set<T, Traits, Alloc>, CDS_OPTIONS >
    {
    public:
        typedef boost::unordered_set<T, Traits, Alloc>    container_type ;   ///< underlying container type
        typedef cds::container::striped_set::details::boost_set_adapter< container_type, CDS_OPTIONS >    type;
    };
}}} // namespace cds::intrusive::striped_set

//@endcond

#endif // #ifndef __CDS_CONTAINER_STRIPED_SET_BOOST_UNORDERED_SET_ADAPTER_H
