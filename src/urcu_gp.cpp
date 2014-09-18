//$$CDS-header$$

#include <cds/urcu/details/gp.h>

namespace cds { namespace urcu { namespace details {

    template<> CDS_EXPORT_API singleton_vtbl * gp_singleton_instance< general_instant_tag >::s_pRCU = nullptr;
    template<> CDS_EXPORT_API singleton_vtbl * gp_singleton_instance< general_buffered_tag >::s_pRCU = nullptr;
    template<> CDS_EXPORT_API singleton_vtbl * gp_singleton_instance< general_threaded_tag >::s_pRCU = nullptr;

}}} // namespace cds::urcu::details
