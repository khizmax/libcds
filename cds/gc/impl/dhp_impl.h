//$$CDS-header$$

#ifndef __CDS_GC_IMPL_DHP_IMPL_H
#define __CDS_GC_IMPL_DHP_IMPL_H

#include <cds/threading/model.h>

//@cond
namespace cds { namespace gc {

    inline DHP::thread_gc::thread_gc(
        bool    bPersistent
        )
        : m_bPersistent( bPersistent )
    {
        if ( !cds::threading::Manager::isThreadAttached() )
            cds::threading::Manager::attachThread();
    }

    inline DHP::thread_gc::~thread_gc()
    {
        if ( !m_bPersistent )
            cds::threading::Manager::detachThread();
    }

    inline DHP::Guard::Guard()
        : Guard::base_class( cds::threading::getGC<DHP>() )
    {}

    template <size_t Count>
    inline DHP::GuardArray<Count>::GuardArray()
        : GuardArray::base_class( cds::threading::getGC<DHP>() )
    {}

    inline void DHP::scan()
    {
        cds::threading::getGC<DHP>().scan();
    }

}} // namespace cds::gc
//@endcond

#endif // #ifndef __CDS_GC_IMPL_DHP_IMPL_H
