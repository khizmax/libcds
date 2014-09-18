//$$CDS-header$$

#ifndef __CDS_INTRUSIVE_STRIPED_SET_BOOST_AVL_SET_ADAPTER_H
#define __CDS_INTRUSIVE_STRIPED_SET_BOOST_AVL_SET_ADAPTER_H

#include <boost/intrusive/avl_set.hpp>
#include <cds/intrusive/striped_set/adapter.h>

//@cond
namespace cds { namespace intrusive { namespace striped_set {

    template <typename T, CDS_BOOST_INTRUSIVE_DECL_OPTIONS4, CDS_SPEC_OPTIONS>
    class adapt< boost::intrusive::avl_set< T, CDS_BOOST_INTRUSIVE_OPTIONS4 >, CDS_OPTIONS >
    {
    public:
        typedef boost::intrusive::avl_set< T, CDS_BOOST_INTRUSIVE_OPTIONS4 >  container_type  ;   ///< underlying intrusive container type

    public:
        typedef details::boost_intrusive_set_adapter<container_type>   type ;  ///< Result of the metafunction

    };
}}} // namespace cds::intrusive::striped_set
//@endcond

#endif // #ifndef __CDS_INTRUSIVE_STRIPED_SET_BOOST_AVL_SET_ADAPTER_H
