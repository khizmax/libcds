//$$CDS-header$$

#ifndef __CDS_BOUNDED_CONTAINER_H
#define __CDS_BOUNDED_CONTAINER_H

namespace cds {
    /// Bounded container
    /**
        If a container has upper limit of item then it should be based on bounded_container class.
        Example of those containers: cyclic queue (cds::container::TsigasCycleQueue)
    */
    struct bounded_container {};
}    // namespace cds

#endif    // __CDS_BOUNDED_CONTAINER_H
