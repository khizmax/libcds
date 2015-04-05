//$$CDS-header$$

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
