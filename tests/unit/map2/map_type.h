//$$CDS-header$$

#ifndef CDSUNIT_MAP_TYPE_H
#define CDSUNIT_MAP_TYPE_H

#include <cds/urcu/general_instant.h>
#include <cds/urcu/general_buffered.h>
#include <cds/urcu/general_threaded.h>
#include <cds/urcu/signal_buffered.h>
#include <cds/urcu/signal_threaded.h>

#include <cds/sync/spinlock.h>
#include <cds/opt/hash.h>
#include <boost/functional/hash/hash.hpp>

#include "cppunit/cppunit_mini.h"

namespace map2 {
    namespace cc = cds::container;
    namespace co = cds::opt;

    typedef cds::urcu::gc< cds::urcu::general_instant<> >   rcu_gpi;
    typedef cds::urcu::gc< cds::urcu::general_buffered<> >  rcu_gpb;
    typedef cds::urcu::gc< cds::urcu::general_threaded<> >  rcu_gpt;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    typedef cds::urcu::gc< cds::urcu::signal_buffered<> >  rcu_shb;
    typedef cds::urcu::gc< cds::urcu::signal_threaded<> >  rcu_sht;
#endif

    template <typename Key>
    struct cmp {
        int operator ()(Key const& k1, Key const& k2) const
        {
            if ( std::less<Key>( k1, k2 ) )
                return -1;
            return std::less<Key>( k2, k1 ) ? 1 : 0;
        }
    };

#define CDSUNIT_INT_COMPARE(t)  template <> struct cmp<t> { int operator()( t k1, t k2 ){ return (int)(k1 - k2); } }
    CDSUNIT_INT_COMPARE(char);
    CDSUNIT_INT_COMPARE(unsigned char);
    CDSUNIT_INT_COMPARE(int);
    CDSUNIT_INT_COMPARE(unsigned int);
    CDSUNIT_INT_COMPARE(long);
    CDSUNIT_INT_COMPARE(unsigned long);
    CDSUNIT_INT_COMPARE(long long);
    CDSUNIT_INT_COMPARE(unsigned long long);
#undef CDSUNIT_INT_COMPARE

    template <>
    struct cmp<std::string>
    {
        int operator()(std::string const& s1, std::string const& s2)
        {
            return s1.compare( s2 );
        }
        int operator()(std::string const& s1, char const * s2)
        {
            return s1.compare( s2 );
        }
        int operator()(char const * s1, std::string const& s2)
        {
            return -s2.compare( s1 );
        }
    };

    // forward
    template <typename ImplSelector, typename Key, typename Value>
    struct map_type;

    template <typename Key, typename Value>
    struct map_type_base
    {
        typedef co::v::hash<Key>    key_hash;
        typedef std::less<Key>      less;
        typedef cmp<Key>            compare;

        struct equal_to {
            bool operator()( Key const& k1, Key const& k2 ) const
            {
                return compare()( k1, k2 ) == 0;
            }
        };

        struct hash: public key_hash
        {
            size_t operator()( Key const& k ) const
            {
                return key_hash::operator()( k );
            }
            template <typename Q>
            size_t operator()( Q const& k ) const
            {
                return key_hash::operator()( k );
            }
        };

        struct hash2: public key_hash
        {
            size_t operator()( Key const& k ) const
            {
                size_t seed = ~key_hash::operator ()( k );
                boost::hash_combine( seed, k );
                return seed;
            }
            template <typename Q>
            size_t operator()( Q const& k ) const
            {
                size_t seed = ~key_hash::operator()( k );
                boost::hash_combine( seed, k );
                return seed;
            }
        };
    };

    template <typename Map>
    static inline void print_stat( Map const& /*m*/ )
    {}

    template <typename Map>
    static inline void check_before_cleanup( Map& /*m*/ )
    {}

    template <typename Map>
    static inline void additional_cleanup( Map& /*m*/ )
    {}

    template <typename Map>
    static inline void additional_check( Map& /*m*/ )
    {}

}   // namespace map2

#endif // ifndef CDSUNIT_MAP_TYPE_H
