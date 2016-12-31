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

#ifndef CDSLIB_GC_IMPL_HP_IMPL_H
#define CDSLIB_GC_IMPL_HP_IMPL_H

#include <cds/threading/model.h>
#include <cds/details/static_functor.h>

//@cond
namespace cds { namespace gc {

    namespace hp {
        static inline ThreadGC& get_thread_gc()
        {
            return cds::threading::getGC<HP>();
        }
    } // namespace hp

    inline HP::thread_gc::thread_gc(
        bool    bPersistent
        )
        : m_bPersistent( bPersistent )
    {
        if ( !threading::Manager::isThreadAttached())
            threading::Manager::attachThread();
    }

    inline HP::thread_gc::~thread_gc()
    {
        if ( !m_bPersistent )
            cds::threading::Manager::detachThread();
    }

    inline /*static*/ cds::gc::hp::details::hp_guard* HP::thread_gc::alloc_guard()
    {
        return hp::get_thread_gc().allocGuard();
    }

    inline /*static*/ void HP::thread_gc::free_guard( cds::gc::hp::details::hp_guard* g )
    {
        hp::get_thread_gc().freeGuard( g );
    }

    inline HP::Guard::Guard()
        : m_guard( hp::get_thread_gc().allocGuard())
    {
        if ( !m_guard )
            throw too_many_hazard_ptr_exception();
    }

    template <typename T>
    inline T * HP::Guard::assign( T * p )
    {
        assert( m_guard != nullptr );

        T * pp = ( *m_guard = p );
        hp::get_thread_gc().sync();
        return pp;
    }

    inline void HP::Guard::link()
    {
        if ( !m_guard ) {
            m_guard = hp::get_thread_gc().allocGuard();
            if ( !m_guard )
                throw too_many_hazard_ptr_exception();
        }
    }

    inline void HP::Guard::unlink()
    {
        if ( m_guard ) {
            hp::get_thread_gc().freeGuard( m_guard );
            m_guard = nullptr;
        }
    }

    template <size_t Count>
    inline HP::GuardArray<Count>::GuardArray()
    {
        if ( hp::get_thread_gc().allocGuard( m_arr ) != Count )
            throw too_many_hazard_ptr_exception();
    }

    template <size_t Count>
    inline HP::GuardArray<Count>::~GuardArray()
    {
        hp::get_thread_gc().freeGuard( m_arr );
    }

    template <size_t Count>
    template <typename T>
    inline T * HP::GuardArray<Count>::assign( size_t nIndex, T* p )
    {
        assert( nIndex < capacity());

        m_arr.set(nIndex, p);
        hp::get_thread_gc().sync();
        return p;
    }

    template <typename T>
    inline void HP::retire( T * p, void (* pFunc)(T *))
    {
        cds::threading::getGC<HP>().retirePtr( p, pFunc );
    }

    template <class Disposer, typename T>
    inline void HP::retire( T * p )
    {
        hp::get_thread_gc().retirePtr( p, cds::details::static_functor<Disposer, T>::call );
    }

    inline void HP::scan()
    {
        hp::get_thread_gc().scan();
    }

}} // namespace cds::gc
//@endcond

#endif // #ifndef CDSLIB_GC_IMPL_HP_IMPL_H
