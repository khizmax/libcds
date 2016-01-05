/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.     
*/

#ifndef CDSLIB_INTRUSIVE_STRIPED_SET_BOOST_SG_SET_ADAPTER_H
#define CDSLIB_INTRUSIVE_STRIPED_SET_BOOST_SG_SET_ADAPTER_H

#include <boost/intrusive/sg_set.hpp>
#include <cds/intrusive/striped_set/adapter.h>

//@cond
namespace cds { namespace intrusive { namespace striped_set {

#if CDS_COMPILER == CDS_COMPILER_INTEL && CDS_COMPILER_VERSION <= 1500
    template <typename T, typename O1, typename O2, typename O3, typename O4, typename... Options>
    class adapt< boost::intrusive::sg_set< T, O1, O2, O3, O4 >, Options... >
    {
    public:
        typedef boost::intrusive::sg_set< T, O1, O2, O3, O4 >  container_type;   ///< underlying intrusive container type

    public:
        typedef details::boost_intrusive_set_adapter<container_type>   type;  ///< Result of the metafunction
    };
#else
    template <typename T, typename... BIOptons, typename... Options>
    class adapt< boost::intrusive::sg_set< T, BIOptons... >, Options... >
    {
    public:
        typedef boost::intrusive::sg_set< T, BIOptons... >  container_type  ;   ///< underlying intrusive container type

    public:
        typedef details::boost_intrusive_set_adapter<container_type>   type ;  ///< Result of the metafunction
    };
#endif
}}} // namespace cds::intrusive::striped_set
//@endcond

#endif // #ifndef CDSLIB_INTRUSIVE_STRIPED_SET_BOOST_SG_SET_ADAPTER_H
