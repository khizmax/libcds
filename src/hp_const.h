//$$CDS-header$$

#ifndef CDSIMPL_HP_CONST_H
#define CDSIMPL_HP_CONST_H

/*
    File: hp_const.h

    Michael's Hazard Pointer reclamation schema global constants
    Gidenstam's reclamation schema global constants

    Editions:
        2008.03.10    Maxim.Khiszinsky    Created
*/

namespace cds { namespace gc {

    //---------------------------------------------------------------
    // Hazard Pointers reclamation schema constants
    namespace hp {
        // Max number of threads expected
        static const size_t c_nMaxThreadCount     = 100;

        // Number of Hazard Pointers per thread
        static const size_t c_nHazardPointerPerThread = 8;
    } // namespace hp

} /* namespace gc */ }    /* namespace cds */

#endif    // #ifndef CDSIMPL_HP_CONST_H
