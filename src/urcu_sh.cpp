//$$CDS-header$$

#include <cds/urcu/details/sh.h>

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
namespace cds { namespace urcu { namespace details {

    template<> CDS_EXPORT_API singleton_vtbl * sh_singleton_instance< signal_buffered_tag >::s_pRCU = null_ptr<singleton_vtbl *>();
    template<> CDS_EXPORT_API singleton_vtbl * sh_singleton_instance< signal_threaded_tag >::s_pRCU = null_ptr<singleton_vtbl *>();

}}} // namespace cds::urcu::details

#endif //#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
