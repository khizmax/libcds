//$$CDS-header$$

#ifndef __CDS_GC_HP_H
#define __CDS_GC_HP_H

#include <cds/gc/impl/hp_decl.h>
#include <cds/gc/impl/hp_impl.h>
#include <cds/details/lib.h>

/**
    @page cds_garbage_collectors_comparison GC comparison
    @ingroup cds_garbage_collector

    <table>
        <tr>
            <th>Feature</th>
            <th>%cds::gc::HP</th>
            <th>%cds::gc::DHP</th>
        </tr>
        <tr>
            <td>Implementation quality</td>
            <td>stable</td>
            <td>stable</td>
        </tr>
        <tr>
            <td>Performance rank (1 - slowest, 5 - fastest)</td>
            <td>5</td>
            <td>4</td>
        </tr>
        <tr>
            <td>Max number of guarded (hazard) pointers per thread</td>
            <td>limited (specifies in GC object ctor)</td>
            <td>unlimited (dynamically allocated when needed)</td>
        </tr>
        <tr>
            <td>Max number of retired pointers<sup>1</sup></td>
            <td>bounded</td>
            <td>bounded</td>
        </tr>
        <tr>
            <td>Array of retired pointers</td>
            <td>preallocated for each thread, limited in size</td>
            <td>global for the entire process, unlimited (dynamically allocated when needed)</td>
        </tr>
        <tr>
            <td>Support direct pointer to item of lock-free container (useful for iterators)</td>
            <td>not supported</td>
            <td>not supported</td>
        </tr>
    </table>

    <sup>1</sup>Unbounded count of retired pointer means a possibility of memory exhaustion.
*/

namespace cds {

    /// Different safe memory reclamation schemas (garbage collectors)
    /** @ingroup cds_garbage_collector

        This namespace specifies different safe memory reclamation (SMR) algorithms.
        See \ref cds_garbage_collector "Garbage collectors"
    */
    namespace gc {
    } // namespace gc

} // namespace cds


#endif  // #ifndef __CDS_GC_HP_H
