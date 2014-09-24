//$$CDS-header$$

#ifndef __CDS_INTRUSIVE_STRIPED_SET_BOOST_SPLAY_SET_ADAPTER_H
#define __CDS_INTRUSIVE_STRIPED_SET_BOOST_SPLAY_SET_ADAPTER_H

#include <boost/intrusive/splay_set.hpp>
#include <cds/intrusive/striped_set/adapter.h>

//@cond
namespace cds { namespace intrusive { namespace striped_set {

    template <typename T, typename... BIOptons, typename... Options>
    class adapt< boost::intrusive::splay_set< T, BIOptons... >, Options... >
    {
    public:
        typedef boost::intrusive::splay_set< T, BIOptons... >  container_type  ;   ///< underlying intrusive container type

    public:
        typedef details::boost_intrusive_set_adapter<container_type>   type ;  ///< Result of the metafunction

    };
}}} // namespace cds::intrusive::striped_set
//@endcond

#endif // #ifndef __CDS_INTRUSIVE_STRIPED_SET_BOOST_SPLAY_SET_ADAPTER_H
