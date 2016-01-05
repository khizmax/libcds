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

#ifndef CDSLIB_GC_IMPL_HP_IMPL_H
#define CDSLIB_GC_IMPL_HP_IMPL_H

#include <cds/threading/model.h>
#include <cds/details/static_functor.h>

//@cond
namespace cds { namespace gc {

    namespace hp {
        inline guard::guard()
            : m_hp( cds::threading::getGC<HP>().allocGuard() )
        {}

        template <typename T>
        inline guard::guard( T * p )
            : m_hp( cds::threading::getGC<HP>().allocGuard() )
        {
            m_hp = p;
        }

        inline guard::~guard()
        {
            cds::threading::getGC<HP>().freeGuard( m_hp );
        }

        template <size_t Count>
        inline array<Count>::array()
        {
            cds::threading::getGC<HP>().allocGuard( *this );
        }

        template <size_t Count>
        inline array<Count>::~array()
        {
            cds::threading::getGC<HP>().freeGuard( *this );
        }



    } // namespace hp

    inline HP::thread_gc::thread_gc(
        bool    bPersistent
        )
        : m_bPersistent( bPersistent )
    {
        if ( !threading::Manager::isThreadAttached() )
            threading::Manager::attachThread();
    }

    inline HP::thread_gc::~thread_gc()
    {
        if ( !m_bPersistent )
            cds::threading::Manager::detachThread();
    }

    inline /*static*/ cds::gc::hp::details::hp_guard& HP::thread_gc::alloc_guard()
    {
        return cds::threading::getGC<HP>().allocGuard();
    }

    inline /*static*/ void HP::thread_gc::free_guard( cds::gc::hp::details::hp_guard& g )
    {
        cds::threading::getGC<HP>().freeGuard( g );
    }

    template <typename T>
    inline T * HP::Guard::assign( T * p )
    {
        T * pp = base_class::operator =(p);
        cds::threading::getGC<HP>().sync();
        return pp;
    }

    template <size_t Count>
    template <typename T>
    inline T * HP::GuardArray<Count>::assign( size_t nIndex, T * p )
    {
        base_class::set(nIndex, p);
        cds::threading::getGC<HP>().sync();
        return p;
    }

    template <typename T>
    inline void HP::retire( T * p, void (* pFunc)(T *) )
    {
        cds::threading::getGC<HP>().retirePtr( p, pFunc );
    }

    template <class Disposer, typename T>
    inline void HP::retire( T * p )
    {
        cds::threading::getGC<HP>().retirePtr( p, cds::details::static_functor<Disposer, T>::call );
    }

    inline void HP::scan()
    {
        cds::threading::getGC<HP>().scan();
    }

}} // namespace cds::gc
//@endcond

#endif // #ifndef CDSLIB_GC_IMPL_HP_IMPL_H
