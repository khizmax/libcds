//$$CDS-header$$

#ifndef __CDS_GC_HP_DETAILS_HP_TYPE_H
#define __CDS_GC_HP_DETAILS_HP_TYPE_H

#include <cds/gc/details/retired_ptr.h>

namespace cds {
    namespace gc {
        namespace hp {

            /// Hazard pointer
            typedef void *    hazard_pointer;

            /// Pointer to function to free (destruct and deallocate) retired pointer of specific type
            typedef cds::gc::details::free_retired_ptr_func free_retired_ptr_func;
        }
    }
}

#endif // #ifndef __CDS_GC_HP_DETAILS_HP_TYPE_H


