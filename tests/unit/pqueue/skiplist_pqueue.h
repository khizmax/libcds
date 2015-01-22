//$$CDS-header$$

#ifndef CDSUNIT_SKIPLIST_PQUEUE_H
#define CDSUNIT_SKIPLIST_PQUEUE_H

#include <cds/container/skip_list_set_hp.h>
#include <cds/container/skip_list_set_dhp.h>
#include <cds/urcu/general_instant.h>
#include <cds/urcu/general_buffered.h>
#include <cds/urcu/general_threaded.h>
#include <cds/urcu/signal_buffered.h>
#include <cds/urcu/signal_threaded.h>
#include <cds/container/skip_list_set_rcu.h>

namespace pqueue {

    template <typename GC>
    struct SkipListPQueue_pop_max
    {
        template <typename T, typename Set>
        bool operator()( T& dest, Set& container ) const
        {
            typename Set::guarded_ptr gp( container.extract_max());
            if ( gp )
                dest = *gp;
            return !gp.empty();
        }
    };

    template <typename RCU>
    struct SkipListPQueue_pop_max< cds::urcu::gc<RCU> >
    {
        template <typename T, typename Set>
        bool operator()( T& dest, Set& container ) const
        {
            typename Set::exempt_ptr ep( container.extract_max());
            if ( ep )
                dest = *ep;
            return !ep.empty();
        }
    };

    template <typename GC>
    struct SkipListPQueue_pop_min
    {
        template <typename T, typename Set>
        bool operator()( T& dest, Set& container ) const
        {
            typename Set::guarded_ptr gp( container.extract_min());
            if ( gp )
                dest = *gp;
            return !gp.empty();
        }
    };

    template <typename RCU>
    struct SkipListPQueue_pop_min< cds::urcu::gc<RCU> >
    {
        template <typename T, typename Set>
        bool operator()( T& dest, Set& container ) const
        {
            typename Set::exempt_ptr ep( container.extract_min());
            if ( ep )
                dest = *ep;
            return !ep.empty();
        }
    };

    template <typename GC, typename T, typename Traits, bool Max=true>
    class SkipListPQueue: protected cds::container::SkipListSet< GC, T, Traits >
    {
        typedef cds::container::SkipListSet< GC, T, Traits > base_class;
        typedef T value_type;
        template <typename GC2> friend struct SkipListPQueue_pop_max;
        template <typename GC2> friend struct SkipListPQueue_pop_min;

    public:
        bool push( value_type const& val )
        {
            return base_class::insert( val );
        }

        bool pop( value_type& dest )
        {
            return Max ? SkipListPQueue_pop_max< typename base_class::gc >()( dest, *this )
                       : SkipListPQueue_pop_min< typename base_class::gc >()( dest, *this );
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

#endif // #ifndef CDSUNIT_SKIPLIST_PQUEUE_H
