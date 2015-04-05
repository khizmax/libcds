//$$CDS-header$$

#ifndef CDSLIB_CONTAINER_DETAILS_GUARDED_PTR_CAST_H
#define CDSLIB_CONTAINER_DETAILS_GUARDED_PTR_CAST_H
//@cond

#include <cds/details/defs.h>

namespace cds { namespace container { namespace details {

    template <typename Node, typename T>
    struct guarded_ptr_cast_set {
        T * operator()(Node* pNode ) const CDS_NOEXCEPT
        {
            return &(pNode->m_Value);
        }
    };

    template <typename Node, typename T>
    struct guarded_ptr_cast_map {
        T * operator()(Node* pNode ) const CDS_NOEXCEPT
        {
            return &(pNode->m_Data);
        }
    };

}}} // namespace cds::container::details

//@endcond
#endif // #ifndef CDSLIB_CONTAINER_DETAILS_GUARDED_PTR_CAST_H
