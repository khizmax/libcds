//$$CDS-header$$

#ifndef CDSUNIT_STD_ELLEN_BINTREE_PQUEUE_H
#define CDSUNIT_STD_ELLEN_BINTREE_PQUEUE_H

#include <cds/container/ellen_bintree_set_hp.h>
#include <cds/container/ellen_bintree_set_dhp.h>
#include <cds/urcu/general_instant.h>
#include <cds/urcu/general_buffered.h>
#include <cds/urcu/general_threaded.h>
#include <cds/urcu/signal_buffered.h>
#include <cds/urcu/signal_threaded.h>
#include <cds/container/ellen_bintree_set_rcu.h>

namespace pqueue {

    template <typename GC>
    struct EllenBinTreePQueue_pop_max
    {
        template <typename T, typename Tree>
        bool operator()( T& dest, Tree& container ) const
        {
            typename Tree::guarded_ptr gp( container.extract_max());
            if ( gp )
                dest = *gp;
            return !gp.empty();
        }
    };

    template <typename RCU>
    struct EllenBinTreePQueue_pop_max< cds::urcu::gc<RCU> >
    {
        template <typename T, typename Tree>
        bool operator()( T& dest, Tree& container ) const
        {
            typename Tree::exempt_ptr ep( container.extract_max());
            if ( ep )
                dest = *ep;
            return !ep.empty();
        }
    };

    template <typename GC>
    struct EllenBinTreePQueue_pop_min
    {
        template <typename T, typename Tree>
        bool operator()( T& dest, Tree& container ) const
        {
            typename Tree::guarded_ptr gp( container.extract_min());
            if ( gp )
                dest = *gp;
            return !gp.empty();
        }
    };

    template <typename RCU>
    struct EllenBinTreePQueue_pop_min< cds::urcu::gc<RCU> >
    {
        template <typename T, typename Tree>
        bool operator()( T& dest, Tree& container ) const
        {
            typename Tree::exempt_ptr ep( container.extract_min());
            if ( ep )
                dest = *ep;
            return !ep.empty();
        }
    };

    template <typename GC, typename Key, typename T, typename Traits, bool Max=true>
    class EllenBinTreePQueue: protected cds::container::EllenBinTreeSet< GC, Key, T, Traits >
    {
        typedef cds::container::EllenBinTreeSet< GC, Key, T, Traits > base_class;
        typedef T value_type;
        template <typename GC2> friend struct EllenBinTreePQueue_pop_max;
        template <typename GC2> friend struct EllenBinTreePQueue_pop_min;

    public:
        bool push( value_type const& val )
        {
            return base_class::insert( val );
        }

        bool pop( value_type& dest )
        {
            return Max ? EllenBinTreePQueue_pop_max< typename base_class::gc >()( dest, *this )
                       : EllenBinTreePQueue_pop_min< typename base_class::gc >()( dest, *this );
        }

        void clear()
        {
            base_class::clear();
        }

        bool empty() const
        {
            return base_class::empty();
        }

        size_t size() const
        {
            return base_class::size();
        }

        typename base_class::stat const& statistics() const
        {
            return base_class::statistics();
        }
    };

} // namespace pqueue


#endif // #ifndef CDSUNIT_STD_ELLEN_BINTREE_PQUEUE_H
