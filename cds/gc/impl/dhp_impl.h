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

#ifndef CDSLIB_GC_IMPL_DHP_IMPL_H
#define CDSLIB_GC_IMPL_DHP_IMPL_H

#include <cds/threading/model.h>

//@cond
namespace cds { namespace gc {

    namespace dhp {

        inline Guard::Guard()
        {
            cds::threading::getGC<DHP>().allocGuard( *this );
        }

        inline Guard::~Guard()
        {
            cds::threading::getGC<DHP>().freeGuard( *this );
        }

        template <size_t Count>
        inline GuardArray<Count>::GuardArray()
        {
            cds::threading::getGC<DHP>().allocGuard( *this );
        }

        template <size_t Count>
        inline GuardArray<Count>::~GuardArray()
        {
            cds::threading::getGC<DHP>().freeGuard( *this );
        }
    } // namespace dhp


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

    inline /*static*/ void DHP::thread_gc::alloc_guard( cds::gc::dhp::details::guard& g )
    {
        return cds::threading::getGC<DHP>().allocGuard(g);
    }
    inline /*static*/ void DHP::thread_gc::free_guard( cds::gc::dhp::details::guard& g )
    {
        cds::threading::getGC<DHP>().freeGuard(g);
    }

    inline void DHP::scan()
    {
        cds::threading::getGC<DHP>().scan();
    }

}} // namespace cds::gc
//@endcond

#endif // #ifndef CDSLIB_GC_IMPL_DHP_IMPL_H
