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

    //---------------------------------------------------------------
    // HRC (Gidenstam) reclamation schema constants
    namespace hrc {
        using cds::gc::hzp::c_nMaxThreadCount;
        using cds::gc::hzp::c_nHazardPointerPerThread;

        /// Number of Hazard Pointers per thread for Node::CleanUp methods
        static const size_t c_nCleanUpHazardPointerPerThread = 2;

        /// Max number of links for HRC node
        static const size_t c_nHRCMaxNodeLinkCount = 4;

        /// Max number of links in live node that may transiently point to a deleted node
        static const size_t c_nHRCMaxTransientLinks = c_nHRCMaxNodeLinkCount;
    }    // namespace hrc
} /* namespace gc */ }    /* namespace cds */

#endif    // #ifndef __CDSIMPL_HZP_CONST_H
