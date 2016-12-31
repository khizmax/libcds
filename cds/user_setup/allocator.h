/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

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

#ifndef CDSLIB_USER_SETUP_ALLOCATOR_H
#define CDSLIB_USER_SETUP_ALLOCATOR_H

/** \file
    \brief Default implementation of allocator classes.

    If you want to use your allocator implementation you should define all or one of the following macro before
    including CDS library headers:
    \li CDS_DEFAULT_ALLOCATOR
    \li CDS_DEFAULT_ALIGNED_ALLOCATOR
*/

#ifndef CDS_DEFAULT_ALLOCATOR

    /** \def CDS_DEFAULT_ALLOCATOR
        The macro defines standard allocator implementation. Default is std::allocator.

        The CDS library uses \p std::allocator<int> to emulate template template parameter.
        The library uses the <tt> std::allocator<int>::rebind<Q>::other </tt> to redefine allocator's type to proper type \p Q.
    */
#define CDS_DEFAULT_ALLOCATOR        std::allocator<int>

#    include <memory>
#endif


#ifndef CDS_DEFAULT_ALIGNED_ALLOCATOR

    /** \def CDS_DEFAULT_ALIGNED_ALLOCATOR
        The macro defines aligned allocator implementation.

        The default is \ref cds::OS::aligned_allocator "cds::OS::aligned_allocator<int>".
        The library uses the <tt> cds::OS::aligned_allocator<int>::rebind<Q>::other </tt> to redefine allocator's type \p int to proper type \p Q.
    */
#define CDS_DEFAULT_ALIGNED_ALLOCATOR    cds::OS::aligned_allocator<int>

#   include <cds/os/alloc_aligned.h>
#endif

#endif // #ifndef CDSLIB_USER_SETUP_ALLOCATOR_H
