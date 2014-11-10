//$$CDS-header$$

#ifndef __CDSIMPL_HZP_CONST_H
#define __CDSIMPL_HZP_CONST_H

/*
    File: hzp_const.h

    Michael's Hazard Pointer reclamation schema global constants
    Gidenstam's reclamation schema global constants

    Editions:
        2008.03.10    Maxim.Khiszinsky    Created
*/

namespace cds { namespace gc {

    //---------------------------------------------------------------
    // Hazard Pointers reclamation schema constants
    namespace hzp {
        // Max number of threads expected
        static const size_t c_nMaxThreadCount     = 100;

        // Number of Hazard Pointers per thread
        static const size_t c_nHazardPointerPerThread = 8;
    } // namespace hzp

} /* namespace gc */ }    /* namespace cds */

#endif    // #ifndef __CDSIMPL_HZP_CONST_H
