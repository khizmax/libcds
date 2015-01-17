/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#ifndef _CDSUNIT_MAP2_MAP_TYPES_H
#define _CDSUNIT_MAP2_MAP_TYPES_H

#include <cds/urcu/general_instant.h>
#include <cds/urcu/general_buffered.h>
#include <cds/urcu/general_threaded.h>
#include <cds/urcu/signal_buffered.h>
#include <cds/urcu/signal_threaded.h>

#include <cds/container/michael_kvlist_hp.h>
#include <cds/container/michael_kvlist_dhp.h>
#include <cds/container/michael_kvlist_rcu.h>
#include <cds/container/michael_kvlist_nogc.h>

#include <cds/container/lazy_kvlist_hp.h>
#include <cds/container/lazy_kvlist_dhp.h>
#include <cds/container/lazy_kvlist_rcu.h>
#include <cds/container/lazy_kvlist_nogc.h>

#include <cds/container/michael_map.h>
#include <cds/container/michael_map_rcu.h>
#include <cds/container/michael_map_nogc.h>

#include <cds/container/split_list_map.h>
#include <cds/container/split_list_map_rcu.h>
#include <cds/container/split_list_map_nogc.h>

#include <cds/container/striped_map/std_list.h>
#include <cds/container/striped_map/std_map.h>
#include <cds/container/striped_map/std_hash_map.h>
#include <cds/container/cuckoo_map.h>

#include <cds/container/skip_list_map_hp.h>
#include <cds/container/skip_list_map_dhp.h>
#include <cds/container/skip_list_map_rcu.h>
#include <cds/container/skip_list_map_nogc.h>

#include <cds/container/ellen_bintree_map_rcu.h>
#include <cds/container/ellen_bintree_map_hp.h>
#include <cds/container/ellen_bintree_map_dhp.h>

#include <boost/version.hpp>
#if BOOST_VERSION >= 104800
#   include <cds/container/striped_map/boost_list.h>
#   include <cds/container/striped_map/boost_slist.h>
#   include <cds/container/striped_map/boost_map.h>
#   include <cds/container/striped_map/boost_flat_map.h>
#endif
#include <cds/container/striped_map/boost_unordered_map.h>
#include <cds/container/striped_map.h>

#include <cds/lock/spinlock.h>

#include "cppunit/cppunit_mini.h"
#include "lock/nolock.h"
#include "map2/std_map.h"
#include "map2/std_hash_map.h"
#include "michael_alloc.h"
#include "print_cuckoo_stat.h"
#include "print_split_list_stat.h"
#include "print_skip_list_stat.h"
#include "print_ellenbintree_stat.h"
#include "ellen_bintree_update_desc_pool.h"

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

    template <typename K, typename V, typename Traits>
    class CuckooMap :
        public cc::CuckooMap< K, V, Traits >
    {
    public:
        typedef cc::CuckooMap< K, V, Traits > cuckoo_base_class;

    public:
        CuckooMap( size_t nCapacity, size_t nLoadFactor )
            : cuckoo_base_class( nCapacity / (nLoadFactor * 16), (unsigned int) 4 )
        {}

        template <typename Q, typename Pred>
        bool erase_with( Q const& key, Pred /*pred*/ )
        {
            return cuckoo_base_class::erase_with( key, typename std::conditional< cuckoo_base_class::c_isSorted, Pred, typename Pred::equal_to>::type() );
        }
    };

    template <typename Key, typename Value>
    struct MapTypes {
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

        // ***************************************************************************
        // MichaelKVList

        struct traits_MichaelList_cmp_stdAlloc :
            public cc::michael_list::make_traits<
                co::compare< compare >
            >::type
        {};
        typedef cc::MichaelKVList< cds::gc::HP,  Key, Value, traits_MichaelList_cmp_stdAlloc > MichaelList_HP_cmp_stdAlloc;
        typedef cc::MichaelKVList< cds::gc::DHP, Key, Value, traits_MichaelList_cmp_stdAlloc > MichaelList_DHP_cmp_stdAlloc;
        typedef cc::MichaelKVList< cds::gc::nogc, Key, Value, traits_MichaelList_cmp_stdAlloc > MichaelList_NOGC_cmp_stdAlloc;
        typedef cc::MichaelKVList< rcu_gpi, Key, Value, traits_MichaelList_cmp_stdAlloc > MichaelList_RCU_GPI_cmp_stdAlloc;
        typedef cc::MichaelKVList< rcu_gpb, Key, Value, traits_MichaelList_cmp_stdAlloc > MichaelList_RCU_GPB_cmp_stdAlloc;
        typedef cc::MichaelKVList< rcu_gpt, Key, Value, traits_MichaelList_cmp_stdAlloc > MichaelList_RCU_GPT_cmp_stdAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelKVList< rcu_shb, Key, Value, traits_MichaelList_cmp_stdAlloc > MichaelList_RCU_SHB_cmp_stdAlloc;
        typedef cc::MichaelKVList< rcu_sht, Key, Value, traits_MichaelList_cmp_stdAlloc > MichaelList_RCU_SHT_cmp_stdAlloc;
#endif

        struct traits_MichaelList_cmp_stdAlloc_seqcst :
            public cc::michael_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        {};
        typedef cc::MichaelKVList< cds::gc::HP,  Key, Value, traits_MichaelList_cmp_stdAlloc_seqcst > MichaelList_HP_cmp_stdAlloc_seqcst;
        typedef cc::MichaelKVList< cds::gc::DHP, Key, Value, traits_MichaelList_cmp_stdAlloc_seqcst > MichaelList_DHP_cmp_stdAlloc_seqcst;
        typedef cc::MichaelKVList< cds::gc::nogc, Key, Value, traits_MichaelList_cmp_stdAlloc_seqcst > MichaelList_NOGC_cmp_stdAlloc_seqcst;
        typedef cc::MichaelKVList< rcu_gpi, Key, Value, traits_MichaelList_cmp_stdAlloc_seqcst > MichaelList_RCU_GPI_cmp_stdAlloc_seqcst;
        typedef cc::MichaelKVList< rcu_gpb, Key, Value, traits_MichaelList_cmp_stdAlloc_seqcst > MichaelList_RCU_GPB_cmp_stdAlloc_seqcst;
        typedef cc::MichaelKVList< rcu_gpt, Key, Value, traits_MichaelList_cmp_stdAlloc_seqcst > MichaelList_RCU_GPT_cmp_stdAlloc_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelKVList< rcu_shb, Key, Value, traits_MichaelList_cmp_stdAlloc_seqcst > MichaelList_RCU_SHB_cmp_stdAlloc_seqcst;
        typedef cc::MichaelKVList< rcu_sht, Key, Value, traits_MichaelList_cmp_stdAlloc_seqcst > MichaelList_RCU_SHT_cmp_stdAlloc_seqcst;
#endif

        struct traits_MichaelList_cmp_michaelAlloc :
            public cc::michael_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        {};
        typedef cc::MichaelKVList< cds::gc::HP,  Key, Value, traits_MichaelList_cmp_michaelAlloc > MichaelList_HP_cmp_michaelAlloc;
        typedef cc::MichaelKVList< cds::gc::DHP, Key, Value, traits_MichaelList_cmp_michaelAlloc > MichaelList_DHP_cmp_michaelAlloc;
        typedef cc::MichaelKVList< cds::gc::nogc, Key, Value, traits_MichaelList_cmp_michaelAlloc > MichaelList_NOGC_cmp_michaelAlloc;
        typedef cc::MichaelKVList< rcu_gpi, Key, Value, traits_MichaelList_cmp_michaelAlloc > MichaelList_RCU_GPI_cmp_michaelAlloc;
        typedef cc::MichaelKVList< rcu_gpb, Key, Value, traits_MichaelList_cmp_michaelAlloc > MichaelList_RCU_GPB_cmp_michaelAlloc;
        typedef cc::MichaelKVList< rcu_gpt, Key, Value, traits_MichaelList_cmp_michaelAlloc > MichaelList_RCU_GPT_cmp_michaelAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelKVList< rcu_shb, Key, Value, traits_MichaelList_cmp_michaelAlloc > MichaelList_RCU_SHB_cmp_michaelAlloc;
        typedef cc::MichaelKVList< rcu_sht, Key, Value, traits_MichaelList_cmp_michaelAlloc > MichaelList_RCU_SHT_cmp_michaelAlloc;
#endif

        struct traits_MichaelList_less_stdAlloc :
            public cc::michael_list::make_traits<
                co::less< less >
            >::type
        {};
        typedef cc::MichaelKVList< cds::gc::HP,  Key, Value, traits_MichaelList_less_stdAlloc > MichaelList_HP_less_stdAlloc;
        typedef cc::MichaelKVList< cds::gc::DHP, Key, Value, traits_MichaelList_less_stdAlloc > MichaelList_DHP_less_stdAlloc;
        typedef cc::MichaelKVList< cds::gc::nogc, Key, Value, traits_MichaelList_less_stdAlloc > MichaelList_NOGC_less_stdAlloc;
        typedef cc::MichaelKVList< rcu_gpi, Key, Value, traits_MichaelList_less_stdAlloc > MichaelList_RCU_GPI_less_stdAlloc;
        typedef cc::MichaelKVList< rcu_gpb, Key, Value, traits_MichaelList_less_stdAlloc > MichaelList_RCU_GPB_less_stdAlloc;
        typedef cc::MichaelKVList< rcu_gpt, Key, Value, traits_MichaelList_less_stdAlloc > MichaelList_RCU_GPT_less_stdAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelKVList< rcu_shb, Key, Value, traits_MichaelList_less_stdAlloc > MichaelList_RCU_SHB_less_stdAlloc;
        typedef cc::MichaelKVList< rcu_sht, Key, Value, traits_MichaelList_less_stdAlloc > MichaelList_RCU_SHT_less_stdAlloc;
#endif

        struct traits_MichaelList_less_stdAlloc_seqcst :
            public cc::michael_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        {};
        typedef cc::MichaelKVList< cds::gc::HP,  Key, Value, traits_MichaelList_less_stdAlloc_seqcst > MichaelList_HP_less_stdAlloc_seqcst;
        typedef cc::MichaelKVList< cds::gc::DHP, Key, Value, traits_MichaelList_less_stdAlloc_seqcst > MichaelList_DHP_less_stdAlloc_seqcst;
        typedef cc::MichaelKVList< cds::gc::nogc, Key, Value, traits_MichaelList_less_stdAlloc_seqcst > MichaelList_NOGC_less_stdAlloc_seqcst;
        typedef cc::MichaelKVList< rcu_gpi, Key, Value, traits_MichaelList_less_stdAlloc_seqcst > MichaelList_RCU_GPI_less_stdAlloc_seqcst;
        typedef cc::MichaelKVList< rcu_gpb, Key, Value, traits_MichaelList_less_stdAlloc_seqcst > MichaelList_RCU_GPB_less_stdAlloc_seqcst;
        typedef cc::MichaelKVList< rcu_gpt, Key, Value, traits_MichaelList_less_stdAlloc_seqcst > MichaelList_RCU_GPT_less_stdAlloc_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelKVList< rcu_shb, Key, Value, traits_MichaelList_less_stdAlloc_seqcst > MichaelList_RCU_SHB_less_stdAlloc_seqcst;
        typedef cc::MichaelKVList< rcu_sht, Key, Value, traits_MichaelList_less_stdAlloc_seqcst > MichaelList_RCU_SHT_less_stdAlloc_seqcst;
#endif

        struct traits_MichaelList_less_michaelAlloc :
            public cc::michael_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        {};
        typedef cc::MichaelKVList< cds::gc::HP,  Key, Value, traits_MichaelList_less_michaelAlloc > MichaelList_HP_less_michaelAlloc;
        typedef cc::MichaelKVList< cds::gc::DHP, Key, Value, traits_MichaelList_less_michaelAlloc > MichaelList_DHP_less_michaelAlloc;
        typedef cc::MichaelKVList< cds::gc::nogc, Key, Value, traits_MichaelList_less_michaelAlloc > MichaelList_NOGC_less_michaelAlloc;
        typedef cc::MichaelKVList< rcu_gpi, Key, Value, traits_MichaelList_less_michaelAlloc > MichaelList_RCU_GPI_less_michaelAlloc;
        typedef cc::MichaelKVList< rcu_gpb, Key, Value, traits_MichaelList_less_michaelAlloc > MichaelList_RCU_GPB_less_michaelAlloc;
        typedef cc::MichaelKVList< rcu_gpt, Key, Value, traits_MichaelList_less_michaelAlloc > MichaelList_RCU_GPT_less_michaelAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelKVList< rcu_shb, Key, Value, traits_MichaelList_less_michaelAlloc > MichaelList_RCU_SHB_less_michaelAlloc;
        typedef cc::MichaelKVList< rcu_sht, Key, Value, traits_MichaelList_less_michaelAlloc > MichaelList_RCU_SHT_less_michaelAlloc;
#endif

        template <typename Base>
        class NogcMapWrapper: public Base
        {
            typedef Base    base_class;
        public:
            NogcMapWrapper( size_t nMaxItemCount, size_t nLoadFactor )
                : base_class( nMaxItemCount, nLoadFactor )
            {}

            template <typename K>
            bool insert( K const& key )
            {
                return base_class::insert( key ) != base_class::end();
            }

            template <typename K, typename V>
            bool insert( K const& key, V const& val )
            {
                return base_class::insert( key, val ) != base_class::end();
            }

            template <typename K, typename Func>
            bool insert_with( K const& key, Func func )
            {
                return base_class::insert_with( key, func ) != base_class::end();
            }

            template <typename K>
            bool find( K const& key )
            {
                return base_class::find( key ) != base_class::end();
            }

            void clear()
            {
                base_class::clear();
            }
        };

        template <typename Base>
        class NogcMapWrapper_dctor: public Base
        {
            typedef Base    base_class;
        public:
            NogcMapWrapper_dctor()
            {}

            template <typename K>
            bool insert( K const& key )
            {
                return base_class::insert( key ) != base_class::end();
            }

            template <typename K, typename V>
            bool insert( K const& key, V const& val )
            {
                return base_class::insert( key, val ) != base_class::end();
            }

            template <typename K, typename Func>
            bool insert_with( K const& key, Func func )
            {
                return base_class::insert_with( key, func ) != base_class::end();
            }

            template <typename K>
            bool find( K const& key )
            {
                return base_class::find( key ) != base_class::end();
            }
        };

        // SplitListMap<gc::nogc> has no clear() method
        template <typename Base>
        class NogcSplitMapWrapper: public Base
        {
            typedef Base    base_class;
        public:
            NogcSplitMapWrapper( size_t nMaxItemCount, size_t nLoadFactor )
                : base_class( nMaxItemCount, nLoadFactor )
            {}

            template <typename K>
            bool insert( K const& key )
            {
                return base_class::insert( key ) != base_class::end();
            }

            template <typename K, typename V>
            bool insert( K const& key, V const& val )
            {
                return base_class::insert( key, val ) != base_class::end();
            }

            template <typename K, typename Func>
            bool insert_with( K const& key, Func func )
            {
                return base_class::insert_with( key, func ) != base_class::end();
            }

            template <typename K>
            bool find( K const& key )
            {
                return base_class::find( key ) != base_class::end();
            }

            void clear()
            {}
        };


        // ***************************************************************************
        // MichaelHashMap based on MichaelKVList

        struct traits_MichaelMap_hash :
            public cc::michael_map::make_traits<
                co::hash< hash >
            >::type
        {};
        typedef cc::MichaelHashMap< cds::gc::HP,  MichaelList_HP_cmp_stdAlloc,  traits_MichaelMap_hash > MichaelMap_HP_cmp_stdAlloc;
        typedef cc::MichaelHashMap< cds::gc::DHP, MichaelList_DHP_cmp_stdAlloc, traits_MichaelMap_hash > MichaelMap_DHP_cmp_stdAlloc;
        typedef cc::MichaelHashMap< cds::gc::nogc, MichaelList_NOGC_cmp_stdAlloc, traits_MichaelMap_hash > MichaelMap_NOGC_cmp_stdAlloc;
        typedef cc::MichaelHashMap< rcu_gpi, MichaelList_RCU_GPI_cmp_stdAlloc, traits_MichaelMap_hash > MichaelMap_RCU_GPI_cmp_stdAlloc;
        typedef cc::MichaelHashMap< rcu_gpb, MichaelList_RCU_GPB_cmp_stdAlloc, traits_MichaelMap_hash > MichaelMap_RCU_GPB_cmp_stdAlloc;
        typedef cc::MichaelHashMap< rcu_gpt, MichaelList_RCU_GPT_cmp_stdAlloc, traits_MichaelMap_hash > MichaelMap_RCU_GPT_cmp_stdAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashMap< rcu_shb, MichaelList_RCU_SHB_cmp_stdAlloc, traits_MichaelMap_hash > MichaelMap_RCU_SHB_cmp_stdAlloc;
        typedef cc::MichaelHashMap< rcu_sht, MichaelList_RCU_SHT_cmp_stdAlloc, traits_MichaelMap_hash > MichaelMap_RCU_SHT_cmp_stdAlloc;
#endif

        typedef cc::MichaelHashMap< cds::gc::HP, MichaelList_HP_less_stdAlloc, traits_MichaelMap_hash > MichaelMap_HP_less_stdAlloc;
        typedef cc::MichaelHashMap< cds::gc::DHP, MichaelList_DHP_less_stdAlloc, traits_MichaelMap_hash > MichaelMap_DHP_less_stdAlloc;
        typedef cc::MichaelHashMap< cds::gc::nogc, MichaelList_NOGC_less_stdAlloc, traits_MichaelMap_hash > MichaelMap_NOGC_less_stdAlloc;
        typedef cc::MichaelHashMap< rcu_gpi, MichaelList_RCU_GPI_less_stdAlloc, traits_MichaelMap_hash > MichaelMap_RCU_GPI_less_stdAlloc;
        typedef cc::MichaelHashMap< rcu_gpb, MichaelList_RCU_GPB_less_stdAlloc, traits_MichaelMap_hash > MichaelMap_RCU_GPB_less_stdAlloc;
        typedef cc::MichaelHashMap< rcu_gpt, MichaelList_RCU_GPT_less_stdAlloc, traits_MichaelMap_hash > MichaelMap_RCU_GPT_less_stdAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashMap< rcu_shb, MichaelList_RCU_SHB_less_stdAlloc, traits_MichaelMap_hash > MichaelMap_RCU_SHB_less_stdAlloc;
        typedef cc::MichaelHashMap< rcu_sht, MichaelList_RCU_SHT_less_stdAlloc, traits_MichaelMap_hash > MichaelMap_RCU_SHT_less_stdAlloc;
#endif

        typedef cc::MichaelHashMap< cds::gc::HP, MichaelList_HP_cmp_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_HP_cmp_stdAlloc_seqcst;
        typedef cc::MichaelHashMap< cds::gc::DHP, MichaelList_DHP_cmp_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_DHP_cmp_stdAlloc_seqcst;
        typedef cc::MichaelHashMap< cds::gc::nogc, MichaelList_NOGC_cmp_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_NOGC_cmp_stdAlloc_seqcst;
        typedef cc::MichaelHashMap< rcu_gpi, MichaelList_RCU_GPI_cmp_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_RCU_GPI_cmp_stdAlloc_seqcst;
        typedef cc::MichaelHashMap< rcu_gpb, MichaelList_RCU_GPB_cmp_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_RCU_GPB_cmp_stdAlloc_seqcst;
        typedef cc::MichaelHashMap< rcu_gpt, MichaelList_RCU_GPT_cmp_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_RCU_GPT_cmp_stdAlloc_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashMap< rcu_shb, MichaelList_RCU_SHB_cmp_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_RCU_SHB_cmp_stdAlloc_seqcst;
        typedef cc::MichaelHashMap< rcu_sht, MichaelList_RCU_SHT_cmp_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_RCU_SHT_cmp_stdAlloc_seqcst;
#endif

        typedef cc::MichaelHashMap< cds::gc::HP, MichaelList_HP_less_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_HP_less_stdAlloc_seqcst;
        typedef cc::MichaelHashMap< cds::gc::DHP, MichaelList_DHP_less_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_DHP_less_stdAlloc_seqcst;
        typedef cc::MichaelHashMap< cds::gc::nogc, MichaelList_NOGC_less_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_NOGC_less_stdAlloc_seqcst;
        typedef cc::MichaelHashMap< rcu_gpi, MichaelList_RCU_GPI_less_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_RCU_GPI_less_stdAlloc_seqcst;
        typedef cc::MichaelHashMap< rcu_gpb, MichaelList_RCU_GPB_less_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_RCU_GPB_less_stdAlloc_seqcst;
        typedef cc::MichaelHashMap< rcu_gpt, MichaelList_RCU_GPT_less_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_RCU_GPT_less_stdAlloc_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashMap< rcu_shb, MichaelList_RCU_SHB_less_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_RCU_SHB_less_stdAlloc_seqcst;
        typedef cc::MichaelHashMap< rcu_sht, MichaelList_RCU_SHT_less_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_RCU_SHT_less_stdAlloc_seqcst;
#endif

        struct traits_MichaelSet_michaelAlloc :
            public traits_MichaelMap_hash
        {
            typedef memory::MichaelAllocator<int> allocator;
        };
        typedef cc::MichaelHashMap< cds::gc::HP, MichaelList_HP_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_HP_cmp_michaelAlloc;
        typedef cc::MichaelHashMap< cds::gc::DHP, MichaelList_DHP_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_DHP_cmp_michaelAlloc;
        typedef cc::MichaelHashMap< cds::gc::nogc, MichaelList_NOGC_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_NOGC_cmp_michaelAlloc;
        typedef cc::MichaelHashMap< rcu_gpi, MichaelList_RCU_GPI_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_RCU_GPI_cmp_michaelAlloc;
        typedef cc::MichaelHashMap< rcu_gpb, MichaelList_RCU_GPB_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_RCU_GPB_cmp_michaelAlloc;
        typedef cc::MichaelHashMap< rcu_gpt, MichaelList_RCU_GPT_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_RCU_GPT_cmp_michaelAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashMap< rcu_shb, MichaelList_RCU_SHB_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_RCU_SHB_cmp_michaelAlloc;
        typedef cc::MichaelHashMap< rcu_sht, MichaelList_RCU_SHT_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_RCU_SHT_cmp_michaelAlloc;
#endif
        typedef cc::MichaelHashMap< cds::gc::HP, MichaelList_HP_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_HP_less_michaelAlloc;
        typedef cc::MichaelHashMap< cds::gc::DHP, MichaelList_DHP_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_DHP_less_michaelAlloc;
        typedef cc::MichaelHashMap< cds::gc::nogc, MichaelList_NOGC_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_NOGC_less_michaelAlloc;
        typedef cc::MichaelHashMap< rcu_gpi, MichaelList_RCU_GPI_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_RCU_GPI_less_michaelAlloc;
        typedef cc::MichaelHashMap< rcu_gpb, MichaelList_RCU_GPB_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_RCU_GPB_less_michaelAlloc;
        typedef cc::MichaelHashMap< rcu_gpt, MichaelList_RCU_GPT_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_RCU_GPT_less_michaelAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashMap< rcu_shb, MichaelList_RCU_SHB_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_RCU_SHB_less_michaelAlloc;
        typedef cc::MichaelHashMap< rcu_sht, MichaelList_RCU_SHT_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_RCU_SHT_less_michaelAlloc;
#endif

        // ***************************************************************************
        // LazyKVList

        struct traits_LazyList_cmp_stdAlloc :
            public cc::lazy_list::make_traits<
                co::compare< compare >
            >::type
        {};
        typedef cc::LazyKVList< cds::gc::HP, Key, Value, traits_LazyList_cmp_stdAlloc > LazyList_HP_cmp_stdAlloc;
        typedef cc::LazyKVList< cds::gc::DHP, Key, Value, traits_LazyList_cmp_stdAlloc > LazyList_DHP_cmp_stdAlloc;
        typedef cc::LazyKVList< cds::gc::nogc, Key, Value, traits_LazyList_cmp_stdAlloc > LazyList_NOGC_cmp_stdAlloc;
        typedef cc::LazyKVList< rcu_gpi, Key, Value, traits_LazyList_cmp_stdAlloc > LazyList_RCU_GPI_cmp_stdAlloc;
        typedef cc::LazyKVList< rcu_gpb, Key, Value, traits_LazyList_cmp_stdAlloc > LazyList_RCU_GPB_cmp_stdAlloc;
        typedef cc::LazyKVList< rcu_gpt, Key, Value, traits_LazyList_cmp_stdAlloc > LazyList_RCU_GPT_cmp_stdAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyKVList< rcu_shb, Key, Value, traits_LazyList_cmp_stdAlloc > LazyList_RCU_SHB_cmp_stdAlloc;
        typedef cc::LazyKVList< rcu_sht, Key, Value, traits_LazyList_cmp_stdAlloc > LazyList_RCU_SHT_cmp_stdAlloc;
#endif

        struct traits_LazyList_cmp_stdAlloc_seqcst :
            public cc::lazy_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        {};
        typedef cc::LazyKVList< cds::gc::HP, Key, Value, traits_LazyList_cmp_stdAlloc_seqcst > LazyList_HP_cmp_stdAlloc_seqcst;
        typedef cc::LazyKVList< cds::gc::DHP, Key, Value, traits_LazyList_cmp_stdAlloc_seqcst > LazyList_DHP_cmp_stdAlloc_seqcst;
        typedef cc::LazyKVList< cds::gc::nogc, Key, Value, traits_LazyList_cmp_stdAlloc_seqcst > LazyList_NOGC_cmp_stdAlloc_seqcst;
        typedef cc::LazyKVList< rcu_gpi, Key, Value, traits_LazyList_cmp_stdAlloc_seqcst > LazyList_RCU_GPI_cmp_stdAlloc_seqcst;
        typedef cc::LazyKVList< rcu_gpb, Key, Value, traits_LazyList_cmp_stdAlloc_seqcst > LazyList_RCU_GPB_cmp_stdAlloc_seqcst;
        typedef cc::LazyKVList< rcu_gpt, Key, Value, traits_LazyList_cmp_stdAlloc_seqcst > LazyList_RCU_GPT_cmp_stdAlloc_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyKVList< rcu_shb, Key, Value, traits_LazyList_cmp_stdAlloc_seqcst > LazyList_RCU_SHB_cmp_stdAlloc_seqcst;
        typedef cc::LazyKVList< rcu_sht, Key, Value, traits_LazyList_cmp_stdAlloc_seqcst > LazyList_RCU_SHT_cmp_stdAlloc_seqcst;
#endif

        struct traits_LazyList_cmp_michaelAlloc :
            public cc::lazy_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        {};
        typedef cc::LazyKVList< cds::gc::HP, Key, Value, traits_LazyList_cmp_michaelAlloc > LazyList_HP_cmp_michaelAlloc;
        typedef cc::LazyKVList< cds::gc::DHP, Key, Value, traits_LazyList_cmp_michaelAlloc > LazyList_DHP_cmp_michaelAlloc;
        typedef cc::LazyKVList< cds::gc::nogc, Key, Value, traits_LazyList_cmp_michaelAlloc > LazyList_NOGC_cmp_michaelAlloc;
        typedef cc::LazyKVList< rcu_gpi, Key, Value, traits_LazyList_cmp_michaelAlloc > LazyList_RCU_GPI_cmp_michaelAlloc;
        typedef cc::LazyKVList< rcu_gpb, Key, Value, traits_LazyList_cmp_michaelAlloc > LazyList_RCU_GPB_cmp_michaelAlloc;
        typedef cc::LazyKVList< rcu_gpt, Key, Value, traits_LazyList_cmp_michaelAlloc > LazyList_RCU_GPT_cmp_michaelAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyKVList< rcu_shb, Key, Value, traits_LazyList_cmp_michaelAlloc > LazyList_RCU_SHB_cmp_michaelAlloc;
        typedef cc::LazyKVList< rcu_sht, Key, Value, traits_LazyList_cmp_michaelAlloc > LazyList_RCU_SHT_cmp_michaelAlloc;
#endif
        struct traits_LazyList_less_stdAlloc :
            public cc::lazy_list::make_traits<
                co::less< less >
            >::type
        {};
        typedef cc::LazyKVList< cds::gc::HP, Key, Value, traits_LazyList_less_stdAlloc > LazyList_HP_less_stdAlloc;
        typedef cc::LazyKVList< cds::gc::DHP, Key, Value, traits_LazyList_less_stdAlloc > LazyList_DHP_less_stdAlloc;
        typedef cc::LazyKVList< cds::gc::nogc, Key, Value, traits_LazyList_less_stdAlloc > LazyList_NOGC_less_stdAlloc;
        typedef cc::LazyKVList< rcu_gpi, Key, Value, traits_LazyList_less_stdAlloc > LazyList_RCU_GPI_less_stdAlloc;
        typedef cc::LazyKVList< rcu_gpb, Key, Value, traits_LazyList_less_stdAlloc > LazyList_RCU_GPB_less_stdAlloc;
        typedef cc::LazyKVList< rcu_gpt, Key, Value, traits_LazyList_less_stdAlloc > LazyList_RCU_GPT_less_stdAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyKVList< rcu_shb, Key, Value, traits_LazyList_less_stdAlloc > LazyList_RCU_SHB_less_stdAlloc;
        typedef cc::LazyKVList< rcu_sht, Key, Value, traits_LazyList_less_stdAlloc > LazyList_RCU_SHT_less_stdAlloc;
#endif

        struct traits_LazyList_less_stdAlloc_seqcst :
            public cc::lazy_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        {};
        typedef cc::LazyKVList< cds::gc::HP, Key, Value, traits_LazyList_less_stdAlloc_seqcst > LazyList_HP_less_stdAlloc_seqcst;
        typedef cc::LazyKVList< cds::gc::DHP, Key, Value, traits_LazyList_less_stdAlloc_seqcst > LazyList_DHP_less_stdAlloc_seqcst;
        typedef cc::LazyKVList< cds::gc::nogc, Key, Value, traits_LazyList_less_stdAlloc_seqcst > LazyList_NOGC_less_stdAlloc_seqcst;
        typedef cc::LazyKVList< rcu_gpi, Key, Value, traits_LazyList_less_stdAlloc_seqcst > LazyList_RCU_GPI_less_stdAlloc_seqcst;
        typedef cc::LazyKVList< rcu_gpb, Key, Value, traits_LazyList_less_stdAlloc_seqcst > LazyList_RCU_GPB_less_stdAlloc_seqcst;
        typedef cc::LazyKVList< rcu_gpt, Key, Value, traits_LazyList_less_stdAlloc_seqcst > LazyList_RCU_GPT_less_stdAlloc_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyKVList< rcu_shb, Key, Value, traits_LazyList_less_stdAlloc_seqcst > LazyList_RCU_SHB_less_stdAlloc_seqcst;
        typedef cc::LazyKVList< rcu_sht, Key, Value, traits_LazyList_less_stdAlloc_seqcst > LazyList_RCU_SHT_less_stdAlloc_seqcst;
#endif

        struct traits_LazyList_less_michaelAlloc :
            public cc::lazy_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        {};
        typedef cc::LazyKVList< cds::gc::HP, Key, Value, traits_LazyList_less_michaelAlloc > LazyList_HP_less_michaelAlloc;
        typedef cc::LazyKVList< cds::gc::DHP, Key, Value, traits_LazyList_less_michaelAlloc > LazyList_DHP_less_michaelAlloc;
        typedef cc::LazyKVList< cds::gc::nogc, Key, Value, traits_LazyList_less_michaelAlloc > LazyList_NOGC_less_michaelAlloc;
        typedef cc::LazyKVList< rcu_gpi, Key, Value, traits_LazyList_less_michaelAlloc > LazyList_RCU_GPI_less_michaelAlloc;
        typedef cc::LazyKVList< rcu_gpb, Key, Value, traits_LazyList_less_michaelAlloc > LazyList_RCU_GPB_less_michaelAlloc;
        typedef cc::LazyKVList< rcu_gpt, Key, Value, traits_LazyList_less_michaelAlloc > LazyList_RCU_GPT_less_michaelAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyKVList< rcu_shb, Key, Value, traits_LazyList_less_michaelAlloc > LazyList_RCU_SHB_less_michaelAlloc;
        typedef cc::LazyKVList< rcu_sht, Key, Value, traits_LazyList_less_michaelAlloc > LazyList_RCU_SHT_less_michaelAlloc;
#endif

        // ***************************************************************************
        // MichaelHashMap based on LazyKVList
        typedef cc::MichaelHashMap< cds::gc::HP, LazyList_HP_cmp_stdAlloc, traits_MichaelMap_hash > MichaelMap_Lazy_HP_cmp_stdAlloc;
        typedef cc::MichaelHashMap< cds::gc::DHP, LazyList_DHP_cmp_stdAlloc, traits_MichaelMap_hash > MichaelMap_Lazy_DHP_cmp_stdAlloc;
        typedef cc::MichaelHashMap< cds::gc::nogc, LazyList_NOGC_cmp_stdAlloc, traits_MichaelMap_hash > MichaelMap_Lazy_NOGC_cmp_stdAlloc;
        typedef cc::MichaelHashMap< rcu_gpi, LazyList_RCU_GPI_cmp_stdAlloc, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPI_cmp_stdAlloc;
        typedef cc::MichaelHashMap< rcu_gpb, LazyList_RCU_GPB_cmp_stdAlloc, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPB_cmp_stdAlloc;
        typedef cc::MichaelHashMap< rcu_gpt, LazyList_RCU_GPT_cmp_stdAlloc, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPT_cmp_stdAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashMap< rcu_shb, LazyList_RCU_SHB_cmp_stdAlloc, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_SHB_cmp_stdAlloc;
        typedef cc::MichaelHashMap< rcu_sht, LazyList_RCU_SHT_cmp_stdAlloc, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_SHT_cmp_stdAlloc;
#endif

        typedef cc::MichaelHashMap< cds::gc::HP, LazyList_HP_less_stdAlloc, traits_MichaelMap_hash > MichaelMap_Lazy_HP_less_stdAlloc;
        typedef cc::MichaelHashMap< cds::gc::DHP, LazyList_DHP_less_stdAlloc, traits_MichaelMap_hash > MichaelMap_Lazy_DHP_less_stdAlloc;
        typedef cc::MichaelHashMap< cds::gc::nogc, LazyList_NOGC_less_stdAlloc, traits_MichaelMap_hash > MichaelMap_Lazy_NOGC_less_stdAlloc;
        typedef cc::MichaelHashMap< rcu_gpi, LazyList_RCU_GPI_less_stdAlloc, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPI_less_stdAlloc;
        typedef cc::MichaelHashMap< rcu_gpb, LazyList_RCU_GPB_less_stdAlloc, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPB_less_stdAlloc;
        typedef cc::MichaelHashMap< rcu_gpt, LazyList_RCU_GPT_less_stdAlloc, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPT_less_stdAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashMap< rcu_shb, LazyList_RCU_SHB_less_stdAlloc, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_SHB_less_stdAlloc;
        typedef cc::MichaelHashMap< rcu_sht, LazyList_RCU_SHT_less_stdAlloc, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_SHT_less_stdAlloc;
#endif

        typedef cc::MichaelHashMap< cds::gc::HP, LazyList_HP_cmp_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_HP_cmp_stdAlloc_seqcst;
        typedef cc::MichaelHashMap< cds::gc::DHP, LazyList_DHP_cmp_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_DHP_cmp_stdAlloc_seqcst;
        typedef cc::MichaelHashMap< cds::gc::nogc, LazyList_NOGC_cmp_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_NOGC_cmp_stdAlloc_seqcst;
        typedef cc::MichaelHashMap< rcu_gpi, LazyList_RCU_GPI_cmp_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPI_cmp_stdAlloc_seqcst;
        typedef cc::MichaelHashMap< rcu_gpb, LazyList_RCU_GPB_cmp_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPB_cmp_stdAlloc_seqcst;
        typedef cc::MichaelHashMap< rcu_gpt, LazyList_RCU_GPT_cmp_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPT_cmp_stdAlloc_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashMap< rcu_shb, LazyList_RCU_SHB_cmp_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_SHB_cmp_stdAlloc_seqcst;
        typedef cc::MichaelHashMap< rcu_sht, LazyList_RCU_SHT_cmp_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_SHT_cmp_stdAlloc_seqcst;
#endif

        typedef cc::MichaelHashMap< cds::gc::HP, LazyList_HP_less_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_HP_less_stdAlloc_seqcst;
        typedef cc::MichaelHashMap< cds::gc::DHP, LazyList_DHP_less_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_DHP_less_stdAlloc_seqcst;
        typedef cc::MichaelHashMap< cds::gc::nogc, LazyList_NOGC_less_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_NOGC_less_stdAlloc_seqcst;
        typedef cc::MichaelHashMap< rcu_gpi, LazyList_RCU_GPI_less_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPI_less_stdAlloc_seqcst;
        typedef cc::MichaelHashMap< rcu_gpb, LazyList_RCU_GPB_less_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPB_less_stdAlloc_seqcst;
        typedef cc::MichaelHashMap< rcu_gpt, LazyList_RCU_GPT_less_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_GPT_less_stdAlloc_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashMap< rcu_shb, LazyList_RCU_SHB_less_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_SHB_less_stdAlloc_seqcst;
        typedef cc::MichaelHashMap< rcu_sht, LazyList_RCU_SHT_less_stdAlloc_seqcst, traits_MichaelMap_hash > MichaelMap_Lazy_RCU_SHT_less_stdAlloc_seqcst;
#endif

        typedef cc::MichaelHashMap< cds::gc::HP, LazyList_HP_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_Lazy_HP_cmp_michaelAlloc;
        typedef cc::MichaelHashMap< cds::gc::DHP, LazyList_DHP_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_Lazy_DHP_cmp_michaelAlloc;
        typedef cc::MichaelHashMap< cds::gc::nogc, LazyList_NOGC_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_Lazy_NOGC_cmp_michaelAlloc;
        typedef cc::MichaelHashMap< rcu_gpi, LazyList_RCU_GPI_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_Lazy_RCU_GPI_cmp_michaelAlloc;
        typedef cc::MichaelHashMap< rcu_gpb, LazyList_RCU_GPB_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_Lazy_RCU_GPB_cmp_michaelAlloc;
        typedef cc::MichaelHashMap< rcu_gpt, LazyList_RCU_GPT_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_Lazy_RCU_GPT_cmp_michaelAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashMap< rcu_shb, LazyList_RCU_SHB_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_Lazy_RCU_SHB_cmp_michaelAlloc;
        typedef cc::MichaelHashMap< rcu_sht, LazyList_RCU_SHT_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_Lazy_RCU_SHT_cmp_michaelAlloc;
#endif
        typedef cc::MichaelHashMap< cds::gc::HP, LazyList_HP_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_Lazy_HP_less_michaelAlloc;
        typedef cc::MichaelHashMap< cds::gc::DHP, LazyList_DHP_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_Lazy_DHP_less_michaelAlloc;
        typedef cc::MichaelHashMap< cds::gc::nogc, LazyList_NOGC_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_Lazy_NOGC_less_michaelAlloc;
        typedef cc::MichaelHashMap< rcu_gpi, LazyList_RCU_GPI_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_Lazy_RCU_GPI_less_michaelAlloc;
        typedef cc::MichaelHashMap< rcu_gpb, LazyList_RCU_GPB_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_Lazy_RCU_GPB_less_michaelAlloc;
        typedef cc::MichaelHashMap< rcu_gpt, LazyList_RCU_GPT_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_Lazy_RCU_GPT_less_michaelAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashMap< rcu_shb, LazyList_RCU_SHB_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_Lazy_RCU_SHB_less_michaelAlloc;
        typedef cc::MichaelHashMap< rcu_sht, LazyList_RCU_SHT_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelMap_Lazy_RCU_SHT_less_michaelAlloc;
#endif

        // ***************************************************************************
        // SplitListMap based on MichaelList

        // HP
        struct traits_SplitList_Michael_dyn_cmp: public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListMap< cds::gc::HP, Key, Value, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_HP_dyn_cmp;
        typedef cc::SplitListMap< cds::gc::DHP, Key, Value, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_DHP_dyn_cmp;
        typedef NogcSplitMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value, traits_SplitList_Michael_dyn_cmp >> SplitList_Michael_NOGC_dyn_cmp;
        typedef cc::SplitListMap< rcu_gpi, Key, Value, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_RCU_GPI_dyn_cmp;
        typedef cc::SplitListMap< rcu_gpb, Key, Value, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_RCU_GPB_dyn_cmp;
        typedef cc::SplitListMap< rcu_gpt, Key, Value, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_RCU_GPT_dyn_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListMap< rcu_shb, Key, Value, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_RCU_SHB_dyn_cmp;
        typedef cc::SplitListMap< rcu_sht, Key, Value, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_RCU_SHT_dyn_cmp;
#endif

        struct traits_SplitList_Michael_dyn_cmp_stat : public traits_SplitList_Michael_dyn_cmp
        {
            typedef cc::split_list::stat<> stat;
        };
        typedef cc::SplitListMap< cds::gc::HP, Key, Value, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_HP_dyn_cmp_stat;
        typedef cc::SplitListMap< cds::gc::DHP, Key, Value, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_DHP_dyn_cmp_stat;
        typedef NogcSplitMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value, traits_SplitList_Michael_dyn_cmp_stat >> SplitList_Michael_NOGC_dyn_cmp_stat;
        typedef cc::SplitListMap< rcu_gpi, Key, Value, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_RCU_GPI_dyn_cmp_stat;
        typedef cc::SplitListMap< rcu_gpb, Key, Value, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_RCU_GPB_dyn_cmp_stat;
        typedef cc::SplitListMap< rcu_gpt, Key, Value, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_RCU_GPT_dyn_cmp_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListMap< rcu_shb, Key, Value, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_RCU_SHB_dyn_cmp_stat;
        typedef cc::SplitListMap< rcu_sht, Key, Value, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_RCU_SHT_dyn_cmp_stat;
#endif

        struct traits_SplitList_Michael_dyn_cmp_seqcst: public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListMap< cds::gc::HP, Key, Value, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_HP_dyn_cmp_seqcst;
        typedef cc::SplitListMap< cds::gc::DHP, Key, Value, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_DHP_dyn_cmp_seqcst;
        typedef NogcSplitMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value, traits_SplitList_Michael_dyn_cmp_seqcst >> SplitList_Michael_NOGC_dyn_cmp_seqcst;
        typedef cc::SplitListMap< rcu_gpi, Key, Value, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_RCU_GPI_dyn_cmp_seqcst;
        typedef cc::SplitListMap< rcu_gpb, Key, Value, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_RCU_GPB_dyn_cmp_seqcst;
        typedef cc::SplitListMap< rcu_gpt, Key, Value, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_RCU_GPT_dyn_cmp_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListMap< rcu_shb, Key, Value, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_RCU_SHB_dyn_cmp_seqcst;
        typedef cc::SplitListMap< rcu_sht, Key, Value, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_RCU_SHT_dyn_cmp_seqcst;
#endif

        struct traits_SplitList_Michael_st_cmp: public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListMap< cds::gc::HP, Key, Value, traits_SplitList_Michael_st_cmp > SplitList_Michael_HP_st_cmp;
        typedef cc::SplitListMap< cds::gc::DHP, Key, Value, traits_SplitList_Michael_st_cmp > SplitList_Michael_DHP_st_cmp;
        typedef NogcSplitMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value, traits_SplitList_Michael_st_cmp >> SplitList_Michael_NOGC_st_cmp;
        typedef cc::SplitListMap< rcu_gpi, Key, Value, traits_SplitList_Michael_st_cmp > SplitList_Michael_RCU_GPI_st_cmp;
        typedef cc::SplitListMap< rcu_gpb, Key, Value, traits_SplitList_Michael_st_cmp > SplitList_Michael_RCU_GPB_st_cmp;
        typedef cc::SplitListMap< rcu_gpt, Key, Value, traits_SplitList_Michael_st_cmp > SplitList_Michael_RCU_GPT_st_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListMap< rcu_shb, Key, Value, traits_SplitList_Michael_st_cmp > SplitList_Michael_RCU_SHB_st_cmp;
        typedef cc::SplitListMap< rcu_sht, Key, Value, traits_SplitList_Michael_st_cmp > SplitList_Michael_RCU_SHT_st_cmp;
#endif

        struct traits_SplitList_Michael_st_cmp_seqcst: public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListMap< cds::gc::HP, Key, Value, traits_SplitList_Michael_st_cmp_seqcst > SplitList_Michael_HP_st_cmp_seqcst;
        typedef cc::SplitListMap< cds::gc::DHP, Key, Value, traits_SplitList_Michael_st_cmp_seqcst > SplitList_Michael_DHP_st_cmp_seqcst;
        typedef NogcSplitMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value, traits_SplitList_Michael_st_cmp_seqcst >> SplitList_Michael_NOGC_st_cmp_seqcst;
        typedef cc::SplitListMap< rcu_gpi, Key, Value, traits_SplitList_Michael_st_cmp_seqcst > SplitList_Michael_RCU_GPI_st_cmp_seqcst;
        typedef cc::SplitListMap< rcu_gpb, Key, Value, traits_SplitList_Michael_st_cmp_seqcst > SplitList_Michael_RCU_GPB_st_cmp_seqcst;
        typedef cc::SplitListMap< rcu_gpt, Key, Value, traits_SplitList_Michael_st_cmp_seqcst > SplitList_Michael_RCU_GPT_st_cmp_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListMap< rcu_shb, Key, Value, traits_SplitList_Michael_st_cmp_seqcst > SplitList_Michael_RCU_SHB_st_cmp_seqcst;
        typedef cc::SplitListMap< rcu_sht, Key, Value, traits_SplitList_Michael_st_cmp_seqcst > SplitList_Michael_RCU_SHT_st_cmp_seqcst;
#endif

        //HP + less
        struct traits_SplitList_Michael_dyn_less: public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListMap< cds::gc::HP, Key, Value, traits_SplitList_Michael_dyn_less > SplitList_Michael_HP_dyn_less;
        typedef cc::SplitListMap< cds::gc::DHP, Key, Value, traits_SplitList_Michael_dyn_less > SplitList_Michael_DHP_dyn_less;
        typedef NogcSplitMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value, traits_SplitList_Michael_dyn_less >> SplitList_Michael_NOGC_dyn_less;
        typedef cc::SplitListMap< rcu_gpi, Key, Value, traits_SplitList_Michael_dyn_less > SplitList_Michael_RCU_GPI_dyn_less;
        typedef cc::SplitListMap< rcu_gpb, Key, Value, traits_SplitList_Michael_dyn_less > SplitList_Michael_RCU_GPB_dyn_less;
        typedef cc::SplitListMap< rcu_gpt, Key, Value, traits_SplitList_Michael_dyn_less > SplitList_Michael_RCU_GPT_dyn_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListMap< rcu_shb, Key, Value, traits_SplitList_Michael_dyn_less > SplitList_Michael_RCU_SHB_dyn_less;
        typedef cc::SplitListMap< rcu_sht, Key, Value, traits_SplitList_Michael_dyn_less > SplitList_Michael_RCU_SHT_dyn_less;
#endif


        struct traits_SplitList_Michael_dyn_less_seqcst: public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListMap< cds::gc::HP, Key, Value, traits_SplitList_Michael_dyn_less_seqcst > SplitList_Michael_HP_dyn_less_seqcst;
        typedef cc::SplitListMap< cds::gc::DHP, Key, Value, traits_SplitList_Michael_dyn_less_seqcst > SplitList_Michael_DHP_dyn_less_seqcst;
        typedef NogcSplitMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value, traits_SplitList_Michael_dyn_less_seqcst >> SplitList_Michael_NOGC_dyn_less_seqcst;
        typedef cc::SplitListMap< rcu_gpi, Key, Value, traits_SplitList_Michael_dyn_less_seqcst > SplitList_Michael_RCU_GPI_dyn_less_seqcst;
        typedef cc::SplitListMap< rcu_gpb, Key, Value, traits_SplitList_Michael_dyn_less_seqcst > SplitList_Michael_RCU_GPB_dyn_less_seqcst;
        typedef cc::SplitListMap< rcu_gpt, Key, Value, traits_SplitList_Michael_dyn_less_seqcst > SplitList_Michael_RCU_GPT_dyn_less_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListMap< rcu_shb, Key, Value, traits_SplitList_Michael_dyn_less_seqcst > SplitList_Michael_RCU_SHB_dyn_less_seqcst;
        typedef cc::SplitListMap< rcu_sht, Key, Value, traits_SplitList_Michael_dyn_less_seqcst > SplitList_Michael_RCU_SHT_dyn_less_seqcst;
#endif

        struct traits_SplitList_Michael_st_less: public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListMap< cds::gc::HP, Key, Value, traits_SplitList_Michael_st_less > SplitList_Michael_HP_st_less;
        typedef cc::SplitListMap< cds::gc::DHP, Key, Value, traits_SplitList_Michael_st_less > SplitList_Michael_DHP_st_less;
        typedef NogcSplitMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value, traits_SplitList_Michael_st_less >> SplitList_Michael_NOGC_st_less;
        typedef cc::SplitListMap< rcu_gpi, Key, Value, traits_SplitList_Michael_st_less > SplitList_Michael_RCU_GPI_st_less;
        typedef cc::SplitListMap< rcu_gpb, Key, Value, traits_SplitList_Michael_st_less > SplitList_Michael_RCU_GPB_st_less;
        typedef cc::SplitListMap< rcu_gpt, Key, Value, traits_SplitList_Michael_st_less > SplitList_Michael_RCU_GPT_st_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListMap< rcu_shb, Key, Value, traits_SplitList_Michael_st_less > SplitList_Michael_RCU_SHB_st_less;
        typedef cc::SplitListMap< rcu_sht, Key, Value, traits_SplitList_Michael_st_less > SplitList_Michael_RCU_SHT_st_less;
#endif

        struct traits_SplitList_Michael_st_less_stat : traits_SplitList_Michael_st_less
        {
            typedef cc::split_list::stat<> stat;
        };
        typedef cc::SplitListMap< cds::gc::HP, Key, Value, traits_SplitList_Michael_st_less_stat > SplitList_Michael_HP_st_less_stat;
        typedef cc::SplitListMap< cds::gc::DHP, Key, Value, traits_SplitList_Michael_st_less_stat > SplitList_Michael_DHP_st_less_stat;
        typedef NogcSplitMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value, traits_SplitList_Michael_st_less_stat >> SplitList_Michael_NOGC_st_less_stat;
        typedef cc::SplitListMap< rcu_gpi, Key, Value, traits_SplitList_Michael_st_less_stat > SplitList_Michael_RCU_GPI_st_less_stat;
        typedef cc::SplitListMap< rcu_gpb, Key, Value, traits_SplitList_Michael_st_less_stat > SplitList_Michael_RCU_GPB_st_less_stat;
        typedef cc::SplitListMap< rcu_gpt, Key, Value, traits_SplitList_Michael_st_less_stat > SplitList_Michael_RCU_GPT_st_less_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListMap< rcu_shb, Key, Value, traits_SplitList_Michael_st_less_stat > SplitList_Michael_RCU_SHB_st_less_stat;
        typedef cc::SplitListMap< rcu_sht, Key, Value, traits_SplitList_Michael_st_less_stat > SplitList_Michael_RCU_SHT_st_less_stat;
#endif


        class traits_SplitList_Michael_st_less_seqcst: public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListMap< cds::gc::HP, Key, Value, traits_SplitList_Michael_st_less_seqcst > SplitList_Michael_HP_st_less_seqcst;
        typedef cc::SplitListMap< cds::gc::DHP, Key, Value, traits_SplitList_Michael_st_less_seqcst > SplitList_Michael_DHP_st_less_seqcst;
        typedef NogcSplitMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value, traits_SplitList_Michael_st_less_seqcst >> SplitList_Michael_NOGC_st_less_seqcst;
        typedef cc::SplitListMap< rcu_gpi, Key, Value, traits_SplitList_Michael_st_less_seqcst > SplitList_Michael_RCU_GPI_st_less_seqcst;
        typedef cc::SplitListMap< rcu_gpb, Key, Value, traits_SplitList_Michael_st_less_seqcst > SplitList_Michael_RCU_GPB_st_less_seqcst;
        typedef cc::SplitListMap< rcu_gpt, Key, Value, traits_SplitList_Michael_st_less_seqcst > SplitList_Michael_RCU_GPT_st_less_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListMap< rcu_shb, Key, Value, traits_SplitList_Michael_st_less_seqcst > SplitList_Michael_RCU_SHB_st_less_seqcst;
        typedef cc::SplitListMap< rcu_sht, Key, Value, traits_SplitList_Michael_st_less_seqcst > SplitList_Michael_RCU_SHT_st_less_seqcst;
#endif


        // ***************************************************************************
        // SplitListMap based on LazyKVList

        struct SplitList_Lazy_dyn_cmp :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListMap< cds::gc::HP, Key, Value, SplitList_Lazy_dyn_cmp > SplitList_Lazy_HP_dyn_cmp;
        typedef cc::SplitListMap< cds::gc::DHP, Key, Value, SplitList_Lazy_dyn_cmp > SplitList_Lazy_DHP_dyn_cmp;
        typedef NogcSplitMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value, SplitList_Lazy_dyn_cmp >> SplitList_Lazy_NOGC_dyn_cmp;
        typedef cc::SplitListMap< rcu_gpi, Key, Value, SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_GPI_dyn_cmp;
        typedef cc::SplitListMap< rcu_gpb, Key, Value, SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_GPB_dyn_cmp;
        typedef cc::SplitListMap< rcu_gpt, Key, Value, SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_GPT_dyn_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListMap< rcu_shb, Key, Value, SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_SHB_dyn_cmp;
        typedef cc::SplitListMap< rcu_sht, Key, Value, SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_SHT_dyn_cmp;
#endif

        struct SplitList_Lazy_dyn_cmp_stat : public SplitList_Lazy_dyn_cmp
        {
            typedef cc::split_list::stat<> stat;
        };
        typedef cc::SplitListMap< cds::gc::HP, Key, Value, SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_HP_dyn_cmp_stat;
        typedef cc::SplitListMap< cds::gc::DHP, Key, Value, SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_DHP_dyn_cmp_stat;
        typedef NogcSplitMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value, SplitList_Lazy_dyn_cmp_stat >> SplitList_Lazy_NOGC_dyn_cmp_stat;
        typedef cc::SplitListMap< rcu_gpi, Key, Value, SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_RCU_GPI_dyn_cmp_stat;
        typedef cc::SplitListMap< rcu_gpb, Key, Value, SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_RCU_GPB_dyn_cmp_stat;
        typedef cc::SplitListMap< rcu_gpt, Key, Value, SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_RCU_GPT_dyn_cmp_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListMap< rcu_shb, Key, Value, SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_RCU_SHB_dyn_cmp_stat;
        typedef cc::SplitListMap< rcu_sht, Key, Value, SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_RCU_SHT_dyn_cmp_stat;
#endif

        struct SplitList_Lazy_dyn_cmp_seqcst :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListMap< cds::gc::HP, Key, Value, SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_HP_dyn_cmp_seqcst;
        typedef cc::SplitListMap< cds::gc::DHP, Key, Value, SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_DHP_dyn_cmp_seqcst;
        typedef NogcSplitMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value, SplitList_Lazy_dyn_cmp_seqcst >> SplitList_Lazy_NOGC_dyn_cmp_seqcst;
        typedef cc::SplitListMap< rcu_gpi, Key, Value, SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_RCU_GPI_dyn_cmp_seqcst;
        typedef cc::SplitListMap< rcu_gpb, Key, Value, SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_RCU_GPB_dyn_cmp_seqcst;
        typedef cc::SplitListMap< rcu_gpt, Key, Value, SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_RCU_GPT_dyn_cmp_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListMap< rcu_shb, Key, Value, SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_RCU_SHB_dyn_cmp_seqcst;
        typedef cc::SplitListMap< rcu_sht, Key, Value, SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_RCU_SHT_dyn_cmp_seqcst;
#endif

        struct SplitList_Lazy_st_cmp :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListMap< cds::gc::HP, Key, Value, SplitList_Lazy_st_cmp > SplitList_Lazy_HP_st_cmp;
        typedef cc::SplitListMap< cds::gc::DHP, Key, Value, SplitList_Lazy_st_cmp > SplitList_Lazy_DHP_st_cmp;
        typedef NogcSplitMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value, SplitList_Lazy_st_cmp >> SplitList_Lazy_NOGC_st_cmp;
        typedef cc::SplitListMap< rcu_gpi, Key, Value, SplitList_Lazy_st_cmp > SplitList_Lazy_RCU_GPI_st_cmp;
        typedef cc::SplitListMap< rcu_gpb, Key, Value, SplitList_Lazy_st_cmp > SplitList_Lazy_RCU_GPB_st_cmp;
        typedef cc::SplitListMap< rcu_gpt, Key, Value, SplitList_Lazy_st_cmp > SplitList_Lazy_RCU_GPT_st_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListMap< rcu_shb, Key, Value, SplitList_Lazy_st_cmp > SplitList_Lazy_RCU_SHB_st_cmp;
        typedef cc::SplitListMap< rcu_sht, Key, Value, SplitList_Lazy_st_cmp > SplitList_Lazy_RCU_SHT_st_cmp;
#endif


        struct SplitList_Lazy_st_cmp_seqcst :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListMap< cds::gc::HP, Key, Value, SplitList_Lazy_st_cmp_seqcst> SplitList_Lazy_HP_st_cmp_seqcst;
        typedef cc::SplitListMap< cds::gc::DHP, Key, Value, SplitList_Lazy_st_cmp_seqcst> SplitList_Lazy_DHP_st_cmp_seqcst;
        typedef NogcSplitMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value, SplitList_Lazy_st_cmp_seqcst >> SplitList_Lazy_NOGC_st_cmp_seqcst;
        typedef cc::SplitListMap< rcu_gpi, Key, Value, SplitList_Lazy_st_cmp_seqcst> SplitList_Lazy_RCU_GPI_st_cmp_seqcst;
        typedef cc::SplitListMap< rcu_gpb, Key, Value, SplitList_Lazy_st_cmp_seqcst> SplitList_Lazy_RCU_GPB_st_cmp_seqcst;
        typedef cc::SplitListMap< rcu_gpt, Key, Value, SplitList_Lazy_st_cmp_seqcst> SplitList_Lazy_RCU_GPT_st_cmp_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListMap< rcu_shb, Key, Value, SplitList_Lazy_st_cmp_seqcst> SplitList_Lazy_RCU_SHB_st_cmp_seqcst;
        typedef cc::SplitListMap< rcu_sht, Key, Value, SplitList_Lazy_st_cmp_seqcst> SplitList_Lazy_RCU_SHT_st_cmp_seqcst;
#endif


        struct SplitList_Lazy_dyn_less :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListMap< cds::gc::HP, Key, Value, SplitList_Lazy_dyn_less > SplitList_Lazy_HP_dyn_less;
        typedef cc::SplitListMap< cds::gc::DHP, Key, Value, SplitList_Lazy_dyn_less > SplitList_Lazy_DHP_dyn_less;
        typedef NogcSplitMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value, SplitList_Lazy_dyn_less >> SplitList_Lazy_NOGC_dyn_less;
        typedef cc::SplitListMap< rcu_gpi, Key, Value, SplitList_Lazy_dyn_less > SplitList_Lazy_RCU_GPI_dyn_less;
        typedef cc::SplitListMap< rcu_gpb, Key, Value, SplitList_Lazy_dyn_less > SplitList_Lazy_RCU_GPB_dyn_less;
        typedef cc::SplitListMap< rcu_gpt, Key, Value, SplitList_Lazy_dyn_less > SplitList_Lazy_RCU_GPT_dyn_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListMap< rcu_shb, Key, Value, SplitList_Lazy_dyn_less > SplitList_Lazy_RCU_SHB_dyn_less;
        typedef cc::SplitListMap< rcu_sht, Key, Value, SplitList_Lazy_dyn_less > SplitList_Lazy_RCU_SHT_dyn_less;
#endif

        struct SplitList_Lazy_dyn_less_seqcst:
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListMap< cds::gc::HP, Key, Value, SplitList_Lazy_dyn_less_seqcst > SplitList_Lazy_HP_dyn_less_seqcst;
        typedef cc::SplitListMap< cds::gc::DHP, Key, Value, SplitList_Lazy_dyn_less_seqcst > SplitList_Lazy_DHP_dyn_less_seqcst;
        typedef NogcSplitMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value, SplitList_Lazy_dyn_less_seqcst >> SplitList_Lazy_NOGC_dyn_less_seqcst;
        typedef cc::SplitListMap< rcu_gpi, Key, Value, SplitList_Lazy_dyn_less_seqcst > SplitList_Lazy_RCU_GPI_dyn_less_seqcst;
        typedef cc::SplitListMap< rcu_gpb, Key, Value, SplitList_Lazy_dyn_less_seqcst > SplitList_Lazy_RCU_GPB_dyn_less_seqcst;
        typedef cc::SplitListMap< rcu_gpt, Key, Value, SplitList_Lazy_dyn_less_seqcst > SplitList_Lazy_RCU_GPT_dyn_less_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListMap< rcu_shb, Key, Value, SplitList_Lazy_dyn_less_seqcst > SplitList_Lazy_RCU_SHB_dyn_less_seqcst;
        typedef cc::SplitListMap< rcu_sht, Key, Value, SplitList_Lazy_dyn_less_seqcst > SplitList_Lazy_RCU_SHT_dyn_less_seqcst;
#endif

        struct SplitList_Lazy_st_less :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListMap< cds::gc::HP, Key, Value, SplitList_Lazy_st_less > SplitList_Lazy_HP_st_less;
        typedef cc::SplitListMap< cds::gc::DHP, Key, Value, SplitList_Lazy_st_less > SplitList_Lazy_DHP_st_less;
        typedef NogcSplitMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value, SplitList_Lazy_st_less >> SplitList_Lazy_NOGC_st_less;
        typedef cc::SplitListMap< rcu_gpi, Key, Value, SplitList_Lazy_st_less > SplitList_Lazy_RCU_GPI_st_less;
        typedef cc::SplitListMap< rcu_gpb, Key, Value, SplitList_Lazy_st_less > SplitList_Lazy_RCU_GPB_st_less;
        typedef cc::SplitListMap< rcu_gpt, Key, Value, SplitList_Lazy_st_less > SplitList_Lazy_RCU_GPT_st_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListMap< rcu_shb, Key, Value, SplitList_Lazy_st_less > SplitList_Lazy_RCU_SHB_st_less;
        typedef cc::SplitListMap< rcu_sht, Key, Value, SplitList_Lazy_st_less > SplitList_Lazy_RCU_SHT_st_less;
#endif

        struct SplitList_Lazy_st_less_stat : public SplitList_Lazy_st_less
        {
            typedef cc::split_list::stat<> stat;
        };
        typedef cc::SplitListMap< cds::gc::HP, Key, Value, SplitList_Lazy_st_less_stat > SplitList_Lazy_HP_st_less_stat;
        typedef cc::SplitListMap< cds::gc::DHP, Key, Value, SplitList_Lazy_st_less_stat > SplitList_Lazy_DHP_st_less_stat;
        typedef NogcSplitMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value, SplitList_Lazy_st_less_stat >> SplitList_Lazy_NOGC_st_less_stat;
        typedef cc::SplitListMap< rcu_gpi, Key, Value, SplitList_Lazy_st_less_stat > SplitList_Lazy_RCU_GPI_st_less_stat;
        typedef cc::SplitListMap< rcu_gpb, Key, Value, SplitList_Lazy_st_less_stat > SplitList_Lazy_RCU_GPB_st_less_stat;
        typedef cc::SplitListMap< rcu_gpt, Key, Value, SplitList_Lazy_st_less_stat > SplitList_Lazy_RCU_GPT_st_less_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListMap< rcu_shb, Key, Value, SplitList_Lazy_st_less_stat > SplitList_Lazy_RCU_SHB_st_less_stat;
        typedef cc::SplitListMap< rcu_sht, Key, Value, SplitList_Lazy_st_less_stat > SplitList_Lazy_RCU_SHT_st_less_stat;
#endif

        struct SplitList_Lazy_st_less_seqcst :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::memory_model< co::v::sequential_consistent >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                        ,co::memory_model< co::v::sequential_consistent >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListMap< cds::gc::HP, Key, Value, SplitList_Lazy_st_less_seqcst > SplitList_Lazy_HP_st_less_seqcst;
        typedef cc::SplitListMap< cds::gc::DHP, Key, Value, SplitList_Lazy_st_less_seqcst > SplitList_Lazy_DHP_st_less_seqcst;
        typedef NogcSplitMapWrapper< cc::SplitListMap< cds::gc::nogc, Key, Value, SplitList_Lazy_st_less_seqcst >> SplitList_Lazy_NOGC_st_less_seqcst;
        typedef cc::SplitListMap< rcu_gpi, Key, Value, SplitList_Lazy_st_less_seqcst > SplitList_Lazy_RCU_GPI_st_less_seqcst;
        typedef cc::SplitListMap< rcu_gpb, Key, Value, SplitList_Lazy_st_less_seqcst > SplitList_Lazy_RCU_GPB_st_less_seqcst;
        typedef cc::SplitListMap< rcu_gpt, Key, Value, SplitList_Lazy_st_less_seqcst > SplitList_Lazy_RCU_GPT_st_less_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListMap< rcu_shb, Key, Value, SplitList_Lazy_st_less_seqcst > SplitList_Lazy_RCU_SHB_st_less_seqcst;
        typedef cc::SplitListMap< rcu_sht, Key, Value, SplitList_Lazy_st_less_seqcst > SplitList_Lazy_RCU_SHT_st_less_seqcst;
#endif



        // ***************************************************************************
        // StripedHashMap

        // for sequential containers
        template <class BucketEntry, typename... Options>
        class StripedHashMap_seq:
            public cc::StripedMap< BucketEntry,
                co::mutex_policy< cc::striped_set::striping<> >
                ,co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                , Options...
            >
        {
            typedef cc::StripedMap< BucketEntry,
                co::mutex_policy< cc::striped_set::striping<> >
                ,co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                , Options...
            > base_class;
            typedef typename base_class::resizing_policy resizing_policy_t;

            resizing_policy_t   m_placeHolder;
        public:
            StripedHashMap_seq( size_t nCapacity, size_t nLoadFactor )
                : base_class( nCapacity / nLoadFactor / 16, *(new(&m_placeHolder) resizing_policy_t( nLoadFactor )) )
            {}
        };

        // for non-sequential ordered containers
        template <class BucketEntry, typename... Options>
        class StripedHashMap_ord:
            public cc::StripedMap< BucketEntry,
                co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::striping<> >
                , Options...
            >
        {
            typedef cc::StripedMap< BucketEntry,
               co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::striping<> >
                , Options...
            > base_class;
            typedef typename base_class::resizing_policy resizing_policy_t;

            resizing_policy_t   m_placeHolder;
        public:
            StripedHashMap_ord( size_t /*nCapacity*/, size_t nLoadFactor )
                : base_class( 0, *(new(&m_placeHolder) resizing_policy_t( nLoadFactor * 1024 )) )
            {}
        };


        typedef StripedHashMap_seq<
            std::list< std::pair< Key const, Value > >
            , co::hash< hash2 >
            , co::less< less >
        > StripedMap_list;

#if (CDS_COMPILER == CDS_COMPILER_MSVC || (CDS_COMPILER == CDS_COMPILER_INTEL && CDS_OS_INTERFACE == CDS_OSI_WINDOWS)) && _MSC_VER < 1600
        typedef StripedHashMap_ord<
            stdext::hash_map< Key, Value, stdext::hash_compare<Key, less > >
            , co::hash< hash2 >
        > StripedMap_hashmap;
#else
        typedef StripedHashMap_ord<
            std::unordered_map< Key, Value, hash, equal_to >
            , co::hash< hash2 >
        > StripedMap_hashmap;
#endif

        typedef StripedHashMap_ord<
            std::map< Key, Value, less >
            , co::hash< hash2 >
        > StripedMap_map;

        typedef StripedHashMap_ord<
            boost::unordered_map< Key, Value, hash, equal_to >
            , co::hash< hash2 >
        > StripedMap_boost_unordered_map;

#if BOOST_VERSION >= 104800
        typedef StripedHashMap_seq<
            boost::container::slist< std::pair< Key const, Value > >
            , co::hash< hash2 >
            , co::less< less >
        > StripedMap_slist;

        typedef StripedHashMap_seq<
            boost::container::list< std::pair< Key const, Value > >
            , co::hash< hash2 >
            , co::less< less >
        > StripedMap_boost_list;

        typedef StripedHashMap_ord<
            boost::container::map< Key, Value, less >
            , co::hash< hash2 >
        > StripedMap_boost_map;

//#   ifdef CDS_UNIT_MAP_TYPES_ENABLE_BOOST_FLAT_CONTAINERS
        typedef StripedHashMap_ord<
            boost::container::flat_map< Key, Value, less >
            , co::hash< hash2 >
        > StripedMap_boost_flat_map;
//#   endif
#endif  // BOOST_VERSION >= 104800

        // ***************************************************************************
        // RefinableHashMap

        // for sequential containers
        template <class BucketEntry, typename... Options>
        class RefinableHashMap_seq:
            public cc::StripedMap< BucketEntry,
                co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::refinable<> >
                , Options...
            >
        {
            typedef cc::StripedMap< BucketEntry,
               co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::refinable<> >
                , Options...
            > base_class;
            typedef typename base_class::resizing_policy resizing_policy_t;

            resizing_policy_t   m_placeHolder;
        public:
            RefinableHashMap_seq( size_t nCapacity, size_t nLoadFactor )
                : base_class( nCapacity / nLoadFactor / 16, *(new(&m_placeHolder) resizing_policy_t( nLoadFactor )) )
            {}
        };

        // for non-sequential ordered containers
        template <class BucketEntry, typename... Options>
        class RefinableHashMap_ord:
            public cc::StripedMap< BucketEntry,
                co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::refinable<> >
                , Options...
            >
        {
            typedef cc::StripedMap< BucketEntry,
               co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::refinable<> >
                , Options...
            > base_class;
            typedef typename base_class::resizing_policy resizing_policy_t;

            resizing_policy_t   m_placeHolder;
        public:
            RefinableHashMap_ord( size_t /*nCapacity*/, size_t nLoadFactor )
                : base_class( 0, *(new(&m_placeHolder) resizing_policy_t( nLoadFactor * 1024 )) )
            {}
        };


        typedef RefinableHashMap_seq<
            std::list< std::pair< Key const, Value > >
            , co::hash< hash2 >
            , co::less< less >
        > RefinableMap_list;

#if BOOST_VERSION >= 104800
        typedef RefinableHashMap_seq<
            boost::container::slist< std::pair< Key const, Value > >
            , co::hash< hash2 >
            , co::less< less >
        > RefinableMap_slist;
#endif

        typedef RefinableHashMap_ord<
            std::map< Key, Value, less >
            , co::hash< hash2 >
        > RefinableMap_map;

        typedef RefinableHashMap_ord<
            std::unordered_map< Key, Value, hash, equal_to >
            , co::hash< hash2 >
        > RefinableMap_hashmap;

        typedef RefinableHashMap_ord<
            boost::unordered_map< Key, Value, hash, equal_to >
            , co::hash< hash2 >
        > RefinableMap_boost_unordered_map;

#if BOOST_VERSION >= 104800
        typedef RefinableHashMap_seq<
            boost::container::list< std::pair< Key const, Value > >
            , co::hash< hash2 >
            , co::less< less >
        > RefinableMap_boost_list;

        typedef RefinableHashMap_ord<
            boost::container::map< Key, Value, less >
            , co::hash< hash2 >
        > RefinableMap_boost_map;

        typedef RefinableHashMap_ord<
            boost::container::flat_map< Key, Value, less >
            , co::hash< hash2 >
        > RefinableMap_boost_flat_map;
#endif // #if BOOST_VERSION >= 104800


        // ***************************************************************************
        // CuckooMap

        template <typename Traits>
        struct traits_CuckooStripedMap: public Traits
        {
            typedef cc::cuckoo::striping<> mutex_policy;
        };
        template <typename Traits>
        struct traits_CuckooRefinableMap : public Traits
        {
            typedef cc::cuckoo::refinable<> mutex_policy;
        };

        struct traits_CuckooMap_list_unord :
            public cc::cuckoo::make_traits <
                cc::cuckoo::probeset_type< cc::cuckoo::list >
                , co::equal_to< equal_to >
                , co::hash< std::tuple< hash, hash2 > >
            > ::type
        {};
        typedef CuckooMap< Key, Value, traits_CuckooStripedMap<traits_CuckooMap_list_unord>> CuckooStripedMap_list_unord;
        typedef CuckooMap< Key, Value, traits_CuckooRefinableMap<traits_CuckooMap_list_unord>> CuckooRefinableMap_list_unord;

        struct traits_CuckooMap_list_unord_stat : public traits_CuckooMap_list_unord
        {
            typedef cc::cuckoo::stat stat;
        };
        typedef CuckooMap< Key, Value, traits_CuckooStripedMap<traits_CuckooMap_list_unord_stat>> CuckooStripedMap_list_unord_stat;
        typedef CuckooMap< Key, Value, traits_CuckooRefinableMap<traits_CuckooMap_list_unord_stat>> CuckooRefinableMap_list_unord_stat;

        struct traits_CuckooMap_list_unord_storehash : public traits_CuckooMap_list_unord
        {
            static CDS_CONSTEXPR const bool store_hash = true;
        };
        typedef CuckooMap< Key, Value, traits_CuckooStripedMap<traits_CuckooMap_list_unord_storehash>> CuckooStripedMap_list_unord_storehash;
        typedef CuckooMap< Key, Value, traits_CuckooRefinableMap<traits_CuckooMap_list_unord_storehash>> CuckooRefinableMap_list_unord_storehash;

        struct traits_CuckooMap_list_ord :
            public cc::cuckoo::make_traits <
                cc::cuckoo::probeset_type< cc::cuckoo::list >
                , co::compare< compare >
                , co::hash< std::tuple< hash, hash2 > >
            >::type
        {};
        typedef CuckooMap< Key, Value, traits_CuckooStripedMap<traits_CuckooMap_list_ord>> CuckooStripedMap_list_ord;
        typedef CuckooMap< Key, Value, traits_CuckooRefinableMap<traits_CuckooMap_list_ord>> CuckooRefinableMap_list_ord;

        struct traits_CuckooMap_list_ord_stat : public traits_CuckooMap_list_ord
        {
            typedef cc::cuckoo::stat stat;
        };
        typedef CuckooMap< Key, Value, traits_CuckooStripedMap<traits_CuckooMap_list_ord_stat>> CuckooStripedMap_list_ord_stat;
        typedef CuckooMap< Key, Value, traits_CuckooRefinableMap<traits_CuckooMap_list_ord_stat>> CuckooRefinableMap_list_ord_stat;

        struct traits_CuckooMap_list_ord_storehash : public traits_CuckooMap_list_ord
        {
            static CDS_CONSTEXPR const bool store_hash = true;
        };
        typedef CuckooMap< Key, Value, traits_CuckooStripedMap<traits_CuckooMap_list_ord_storehash>> CuckooStripedMap_list_ord_storehash;
        typedef CuckooMap< Key, Value, traits_CuckooRefinableMap<traits_CuckooMap_list_ord_storehash>> CuckooRefinableMap_list_ord_storehash;

        struct traits_CuckooMap_vector_unord :
            public cc::cuckoo::make_traits <
                cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                , co::equal_to< equal_to >
                , co::hash< std::tuple< hash, hash2 > >
            >::type
        {};
        typedef CuckooMap< Key, Value, traits_CuckooStripedMap<traits_CuckooMap_vector_unord>> CuckooStripedMap_vector_unord;
        typedef CuckooMap< Key, Value, traits_CuckooRefinableMap<traits_CuckooMap_vector_unord>> CuckooRefinableMap_vector_unord;

        struct traits_CuckooMap_vector_unord_stat : public traits_CuckooMap_vector_unord
        {
            typedef cc::cuckoo::stat stat;
        };
        typedef CuckooMap< Key, Value, traits_CuckooStripedMap<traits_CuckooMap_vector_unord_stat>> CuckooStripedMap_vector_unord_stat;
        typedef CuckooMap< Key, Value, traits_CuckooRefinableMap<traits_CuckooMap_vector_unord_stat>> CuckooRefinableMap_vector_unord_stat;

        struct traits_CuckooMap_vector_unord_storehash : public traits_CuckooMap_vector_unord
        {
            static CDS_CONSTEXPR const bool store_hash = true;
        };
        typedef CuckooMap< Key, Value, traits_CuckooStripedMap<traits_CuckooMap_vector_unord_storehash>> CuckooStripedMap_vector_unord_storehash;
        typedef CuckooMap< Key, Value, traits_CuckooRefinableMap<traits_CuckooMap_vector_unord_storehash>> CuckooRefinableMap_vector_unord_storehash;

        struct traits_CuckooMap_vector_ord :
            public cc::cuckoo::make_traits <
                cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                , co::compare< compare >
                , co::hash< std::tuple< hash, hash2 > >
            >::type
        {};
        typedef CuckooMap< Key, Value, traits_CuckooStripedMap<traits_CuckooMap_vector_ord>> CuckooStripedMap_vector_ord;
        typedef CuckooMap< Key, Value, traits_CuckooRefinableMap<traits_CuckooMap_vector_ord>> CuckooRefinableMap_vector_ord;

        struct traits_CuckooMap_vector_ord_stat : public traits_CuckooMap_vector_ord
        {
            typedef cc::cuckoo::stat stat;
        };
        typedef CuckooMap< Key, Value, traits_CuckooStripedMap<traits_CuckooMap_vector_ord_stat>> CuckooStripedMap_vector_ord_stat;
        typedef CuckooMap< Key, Value, traits_CuckooRefinableMap<traits_CuckooMap_vector_ord_stat>> CuckooRefinableMap_vector_ord_stat;

        struct traits_CuckooMap_vector_ord_storehash : public traits_CuckooMap_vector_ord
        {
            static CDS_CONSTEXPR const bool store_hash = true;
        };
        typedef CuckooMap< Key, Value, traits_CuckooStripedMap<traits_CuckooMap_vector_ord_storehash>> CuckooStripedMap_vector_ord_storehash;
        typedef CuckooMap< Key, Value, traits_CuckooRefinableMap<traits_CuckooMap_vector_ord_storehash>> CuckooRefinableMap_vector_ord_storehash;

        // ***************************************************************************
        // SkipListMap

        class traits_SkipListMap_less_pascal: public cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_less_pascal > SkipListMap_hp_less_pascal;
        typedef cc::SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_less_pascal > SkipListMap_dhp_less_pascal;
        typedef cc::SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_less_pascal > SkipListMap_nogc_less_pascal;
        typedef cc::SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_less_pascal > SkipListMap_rcu_gpi_less_pascal;
        typedef cc::SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_less_pascal > SkipListMap_rcu_gpb_less_pascal;
        typedef cc::SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_less_pascal > SkipListMap_rcu_gpt_less_pascal;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_less_pascal > SkipListMap_rcu_shb_less_pascal;
        typedef cc::SkipListMap< rcu_sht, Key, Value, traits_SkipListMap_less_pascal > SkipListMap_rcu_sht_less_pascal;
#endif

        class traits_SkipListMap_less_pascal_seqcst: public cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::memory_model< co::v::sequential_consistent >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_less_pascal_seqcst > SkipListMap_hp_less_pascal_seqcst;
        typedef cc::SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_less_pascal_seqcst > SkipListMap_dhp_less_pascal_seqcst;
        typedef cc::SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_less_pascal_seqcst > SkipListMap_nogc_less_pascal_seqcst;
        typedef cc::SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_less_pascal_seqcst > SkipListMap_rcu_gpi_less_pascal_seqcst;
        typedef cc::SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_less_pascal_seqcst > SkipListMap_rcu_gpb_less_pascal_seqcst;
        typedef cc::SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_less_pascal_seqcst > SkipListMap_rcu_gpt_less_pascal_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_less_pascal_seqcst > SkipListMap_rcu_shb_less_pascal_seqcst;
        typedef cc::SkipListMap< rcu_sht, Key, Value, traits_SkipListMap_less_pascal_seqcst > SkipListMap_rcu_sht_less_pascal_seqcst;
#endif

        class traits_SkipListMap_less_pascal_stat: public cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::stat< cc::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_less_pascal_stat > SkipListMap_hp_less_pascal_stat;
        typedef cc::SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_less_pascal_stat > SkipListMap_dhp_less_pascal_stat;
        typedef cc::SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_less_pascal_stat > SkipListMap_nogc_less_pascal_stat;
        typedef cc::SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_less_pascal_stat > SkipListMap_rcu_gpi_less_pascal_stat;
        typedef cc::SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_less_pascal_stat > SkipListMap_rcu_gpb_less_pascal_stat;
        typedef cc::SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_less_pascal_stat > SkipListMap_rcu_gpt_less_pascal_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_less_pascal_stat > SkipListMap_rcu_shb_less_pascal_stat;
        typedef cc::SkipListMap< rcu_sht, Key, Value, traits_SkipListMap_less_pascal_stat > SkipListMap_rcu_sht_less_pascal_stat;
#endif

        class traits_SkipListMap_cmp_pascal: public cc::skip_list::make_traits <
                co::compare< compare >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_cmp_pascal > SkipListMap_hp_cmp_pascal;
        typedef cc::SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_cmp_pascal > SkipListMap_dhp_cmp_pascal;
        typedef cc::SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_cmp_pascal > SkipListMap_nogc_cmp_pascal;
        typedef cc::SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_cmp_pascal > SkipListMap_rcu_gpi_cmp_pascal;
        typedef cc::SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_cmp_pascal > SkipListMap_rcu_gpb_cmp_pascal;
        typedef cc::SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_cmp_pascal > SkipListMap_rcu_gpt_cmp_pascal;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_cmp_pascal > SkipListMap_rcu_shb_cmp_pascal;
        typedef cc::SkipListMap< rcu_sht, Key, Value, traits_SkipListMap_cmp_pascal > SkipListMap_rcu_sht_cmp_pascal;
#endif

        class traits_SkipListMap_cmp_pascal_stat: public cc::skip_list::make_traits <
                co::compare< compare >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::stat< cc::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_cmp_pascal_stat > SkipListMap_hp_cmp_pascal_stat;
        typedef cc::SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_cmp_pascal_stat > SkipListMap_dhp_cmp_pascal_stat;
        typedef cc::SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_cmp_pascal_stat > SkipListMap_nogc_cmp_pascal_stat;
        typedef cc::SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_cmp_pascal_stat > SkipListMap_rcu_gpi_cmp_pascal_stat;
        typedef cc::SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_cmp_pascal_stat > SkipListMap_rcu_gpb_cmp_pascal_stat;
        typedef cc::SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_cmp_pascal_stat > SkipListMap_rcu_gpt_cmp_pascal_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_cmp_pascal_stat > SkipListMap_rcu_shb_cmp_pascal_stat;
        typedef cc::SkipListMap< rcu_sht, Key, Value, traits_SkipListMap_cmp_pascal_stat > SkipListMap_rcu_sht_cmp_pascal_stat;
#endif

        class traits_SkipListMap_less_xorshift: public cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_less_xorshift > SkipListMap_hp_less_xorshift;
        typedef cc::SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_less_xorshift > SkipListMap_dhp_less_xorshift;
        typedef cc::SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_less_xorshift > SkipListMap_nogc_less_xorshift;
        typedef cc::SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_less_xorshift > SkipListMap_rcu_gpi_less_xorshift;
        typedef cc::SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_less_xorshift > SkipListMap_rcu_gpb_less_xorshift;
        typedef cc::SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_less_xorshift > SkipListMap_rcu_gpt_less_xorshift;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_less_xorshift > SkipListMap_rcu_shb_less_xorshift;
        typedef cc::SkipListMap< rcu_sht, Key, Value, traits_SkipListMap_less_xorshift > SkipListMap_rcu_sht_less_xorshift;
#endif

        class traits_SkipListMap_less_xorshift_stat: public cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::stat< cc::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_less_xorshift_stat > SkipListMap_hp_less_xorshift_stat;
        typedef cc::SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_less_xorshift_stat > SkipListMap_dhp_less_xorshift_stat;
        typedef cc::SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_less_xorshift_stat > SkipListMap_nogc_less_xorshift_stat;
        typedef cc::SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_less_xorshift_stat > SkipListMap_rcu_gpi_less_xorshift_stat;
        typedef cc::SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_less_xorshift_stat > SkipListMap_rcu_gpb_less_xorshift_stat;
        typedef cc::SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_less_xorshift_stat > SkipListMap_rcu_gpt_less_xorshift_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_less_xorshift_stat > SkipListMap_rcu_shb_less_xorshift_stat;
        typedef cc::SkipListMap< rcu_sht, Key, Value, traits_SkipListMap_less_xorshift_stat > SkipListMap_rcu_sht_less_xorshift_stat;
#endif

        class traits_SkipListMap_cmp_xorshift: public cc::skip_list::make_traits <
                co::compare< compare >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_cmp_xorshift > SkipListMap_hp_cmp_xorshift;
        typedef cc::SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_cmp_xorshift > SkipListMap_dhp_cmp_xorshift;
        typedef cc::SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_cmp_xorshift > SkipListMap_nogc_cmp_xorshift;
        typedef cc::SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_cmp_xorshift > SkipListMap_rcu_gpi_cmp_xorshift;
        typedef cc::SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_cmp_xorshift > SkipListMap_rcu_gpb_cmp_xorshift;
        typedef cc::SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_cmp_xorshift > SkipListMap_rcu_gpt_cmp_xorshift;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_cmp_xorshift > SkipListMap_rcu_shb_cmp_xorshift;
        typedef cc::SkipListMap< rcu_sht, Key, Value, traits_SkipListMap_cmp_xorshift > SkipListMap_rcu_sht_cmp_xorshift;
#endif

        class traits_SkipListMap_cmp_xorshift_stat: public cc::skip_list::make_traits <
                co::compare< compare >
                ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
                ,co::stat< cc::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::SkipListMap< cds::gc::HP, Key, Value, traits_SkipListMap_cmp_xorshift_stat > SkipListMap_hp_cmp_xorshift_stat;
        typedef cc::SkipListMap< cds::gc::DHP, Key, Value, traits_SkipListMap_cmp_xorshift_stat > SkipListMap_dhp_cmp_xorshift_stat;
        typedef cc::SkipListMap< cds::gc::nogc, Key, Value, traits_SkipListMap_cmp_xorshift_stat > SkipListMap_nogc_cmp_xorshift_stat;
        typedef cc::SkipListMap< rcu_gpi, Key, Value, traits_SkipListMap_cmp_xorshift_stat > SkipListMap_rcu_gpi_cmp_xorshift_stat;
        typedef cc::SkipListMap< rcu_gpb, Key, Value, traits_SkipListMap_cmp_xorshift_stat > SkipListMap_rcu_gpb_cmp_xorshift_stat;
        typedef cc::SkipListMap< rcu_gpt, Key, Value, traits_SkipListMap_cmp_xorshift_stat > SkipListMap_rcu_gpt_cmp_xorshift_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListMap< rcu_shb, Key, Value, traits_SkipListMap_cmp_xorshift_stat > SkipListMap_rcu_shb_cmp_xorshift_stat;
        typedef cc::SkipListMap< rcu_sht, Key, Value, traits_SkipListMap_cmp_xorshift_stat > SkipListMap_rcu_sht_cmp_xorshift_stat;
#endif


        // ***************************************************************************
        // EllenBinTreeMap
        struct ellen_bintree_props {
            struct hp_gc {
                typedef cc::ellen_bintree::map_node<cds::gc::HP, Key, Value>        leaf_node;
                typedef cc::ellen_bintree::internal_node< Key, leaf_node >          internal_node;
                typedef cc::ellen_bintree::update_desc< leaf_node, internal_node >  update_desc;
            };
            struct dhp_gc {
                typedef cc::ellen_bintree::map_node<cds::gc::DHP, Key, Value>       leaf_node;
                typedef cc::ellen_bintree::internal_node< Key, leaf_node >          internal_node;
                typedef cc::ellen_bintree::update_desc< leaf_node, internal_node >  update_desc;
            };
            struct gpi {
                typedef cc::ellen_bintree::map_node<rcu_gpi, Key, Value>            leaf_node;
                typedef cc::ellen_bintree::internal_node< Key, leaf_node >          internal_node;
                typedef cc::ellen_bintree::update_desc< leaf_node, internal_node >  update_desc;
            };
            struct gpb {
                typedef cc::ellen_bintree::map_node<rcu_gpb, Key, Value>            leaf_node;
                typedef cc::ellen_bintree::internal_node< Key, leaf_node >          internal_node;
                typedef cc::ellen_bintree::update_desc< leaf_node, internal_node >  update_desc;
            };
            struct gpt {
                typedef cc::ellen_bintree::map_node<rcu_gpt, Key, Value>            leaf_node;
                typedef cc::ellen_bintree::internal_node< Key, leaf_node >          internal_node;
                typedef cc::ellen_bintree::update_desc< leaf_node, internal_node >  update_desc;
            };
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
            struct shb {
                typedef cc::ellen_bintree::map_node<rcu_shb, Key, Value>            leaf_node;
                typedef cc::ellen_bintree::internal_node< Key, leaf_node >          internal_node;
                typedef cc::ellen_bintree::update_desc< leaf_node, internal_node >  update_desc;
            };
            struct sht {
                typedef cc::ellen_bintree::map_node<rcu_sht, Key, Value>            leaf_node;
                typedef cc::ellen_bintree::internal_node< Key, leaf_node >          internal_node;
                typedef cc::ellen_bintree::update_desc< leaf_node, internal_node >  update_desc;
            };
#endif
        };

        struct traits_EllenBinTreeMap: public cc::ellen_bintree::make_set_traits<
                co::less< less >
                ,co::node_allocator< ellen_bintree_pool::internal_node_allocator< int > >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        struct traits_EllenBinTreeMap_hp : traits_EllenBinTreeMap {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::hp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< cds::gc::HP, Key, Value, traits_EllenBinTreeMap_hp >EllenBinTreeMap_hp;

        struct traits_EllenBinTreeMap_dhp : traits_EllenBinTreeMap {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::dhp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< cds::gc::DHP, Key, Value, traits_EllenBinTreeMap_dhp >EllenBinTreeMap_dhp;

        struct traits_EllenBinTreeMap_gpi : traits_EllenBinTreeMap {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpi::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< rcu_gpi, Key, Value, traits_EllenBinTreeMap_gpi >EllenBinTreeMap_rcu_gpi;

        struct traits_EllenBinTreeMap_gpb : traits_EllenBinTreeMap {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< rcu_gpb, Key, Value, traits_EllenBinTreeMap_gpb >EllenBinTreeMap_rcu_gpb;

        struct traits_EllenBinTreeMap_gpt : traits_EllenBinTreeMap {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpt::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< rcu_gpt, Key, Value, traits_EllenBinTreeMap_gpt >EllenBinTreeMap_rcu_gpt;

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits_EllenBinTreeMap_shb : traits_EllenBinTreeMap {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::shb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< rcu_shb, Key, Value, traits_EllenBinTreeMap_shb >EllenBinTreeMap_rcu_shb;

        struct traits_EllenBinTreeMap_sht : traits_EllenBinTreeMap {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::sht::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< rcu_sht, Key, Value, traits_EllenBinTreeMap_sht >EllenBinTreeMap_rcu_sht;
#endif

        struct traits_EllenBinTreeMap_yield : public traits_EllenBinTreeMap
        {
            typedef cds::backoff::yield back_off;
        };
        struct traits_EllenBinTreeMap_hp_yield : traits_EllenBinTreeMap_yield {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::hp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< cds::gc::HP, Key, Value, traits_EllenBinTreeMap_hp_yield >EllenBinTreeMap_hp_yield;

        struct traits_EllenBinTreeMap_dhp_yield : traits_EllenBinTreeMap_yield {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::dhp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< cds::gc::DHP, Key, Value, traits_EllenBinTreeMap_dhp_yield >EllenBinTreeMap_dhp_yield;

        struct traits_EllenBinTreeMap_gpb_yield : traits_EllenBinTreeMap_yield {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< rcu_gpb, Key, Value, traits_EllenBinTreeMap_gpb_yield >EllenBinTreeMap_rcu_gpb_yield;


        struct traits_EllenBinTreeMap_stat: public cc::ellen_bintree::make_set_traits<
                co::less< less >
                ,cc::ellen_bintree::update_desc_allocator<
                    cds::memory::pool_allocator< typename ellen_bintree_props::hp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor >
                >
                ,co::node_allocator< ellen_bintree_pool::internal_node_allocator< int > >
                ,co::stat< cc::ellen_bintree::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};

        struct traits_EllenBinTreeMap_stat_hp : public traits_EllenBinTreeMap_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::hp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< cds::gc::HP, Key, Value, traits_EllenBinTreeMap_stat_hp > EllenBinTreeMap_hp_stat;

        struct traits_EllenBinTreeMap_stat_dhp : public traits_EllenBinTreeMap_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::dhp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< cds::gc::HP, Key, Value, traits_EllenBinTreeMap_stat_dhp > EllenBinTreeMap_dhp_stat;

        struct traits_EllenBinTreeMap_stat_gpi : public traits_EllenBinTreeMap_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpi::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< rcu_gpi, Key, Value, traits_EllenBinTreeMap_stat_gpi > EllenBinTreeMap_rcu_gpi_stat;

        struct traits_EllenBinTreeMap_stat_gpb : public traits_EllenBinTreeMap_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< rcu_gpb, Key, Value, traits_EllenBinTreeMap_stat_gpb > EllenBinTreeMap_rcu_gpb_stat;

        struct traits_EllenBinTreeMap_stat_gpt : public traits_EllenBinTreeMap_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpt::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< rcu_gpt, Key, Value, traits_EllenBinTreeMap_stat_gpt > EllenBinTreeMap_rcu_gpt_stat;

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits_EllenBinTreeMap_stat_shb : public traits_EllenBinTreeMap_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::shb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< rcu_shb, Key, Value, traits_EllenBinTreeMap_stat_shb > EllenBinTreeMap_rcu_shb_stat;

        struct traits_EllenBinTreeMap_stat_sht : public traits_EllenBinTreeMap_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::sht::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeMap< rcu_sht, Key, Value, traits_EllenBinTreeMap_stat_sht > EllenBinTreeMap_rcu_sht_stat;
#endif

        // ***************************************************************************
        // Standard implementations

        typedef StdMap< Key, Value, cds::SpinLock >             StdMap_Spin;
        typedef StdMap< Key, Value, lock::NoLock>               StdMap_NoLock;

        typedef StdHashMap< Key, Value, cds::SpinLock >         StdHashMap_Spin;
        typedef StdHashMap< Key, Value, lock::NoLock >          StdHashMap_NoLock;

    };

    template <typename Map>
    static inline void print_stat( Map const& /*m*/ )
    {}

    template <typename Map>
    static inline void additional_cleanup( Map& /*m*/ )
    {}

    template <typename Map>
    static inline void additional_check( Map& /*m*/ )
    {}


    template <typename K, typename T, typename Traits >
    static inline void print_stat( cc::CuckooMap< K, T, Traits > const& m )
    {
        CPPUNIT_MSG( m.statistics() << m.mutex_policy_statistics() );
    }

    template <typename GC, typename K, typename T, typename Traits >
    static inline void print_stat( cc::SkipListMap< GC, K, T, Traits > const& m )
    {
        CPPUNIT_MSG( m.statistics() );
    }

    template <typename GC, typename K, typename T, typename Traits >
    static inline void print_stat( cc::SplitListMap< GC, K, T, Traits > const& m )
    {
        CPPUNIT_MSG( m.statistics() );
    }

    // EllenBinTreeMap
    template <typename GC, typename Key, typename T, typename Traits>
    static inline void print_stat( cc::EllenBinTreeMap<GC, Key, T, Traits> const& s )
    {
        CPPUNIT_MSG( s.statistics() );
    }
    template <typename GC, typename Key, typename T, typename Traits>
    static inline void additional_cleanup( cc::EllenBinTreeMap<GC, Key, T, Traits>& /*s*/ )
    {
        ellen_bintree_pool::internal_node_counter::reset();
    }
    namespace ellen_bintree_check {
        static inline void check_stat( cds::intrusive::ellen_bintree::empty_stat const& /*s*/ )
        {
            // This check is not valid for thread-based RCU
            /*
            CPPUNIT_CHECK_CURRENT_EX( ellen_bintree_pool::internal_node_counter::m_nAlloc.get() == ellen_bintree_pool::internal_node_counter::m_nFree.get(),
                "m_nAlloc=" << ellen_bintree_pool::internal_node_counter::m_nAlloc.get()
                << ", m_nFree=" << ellen_bintree_pool::internal_node_counter::m_nFree.get()
                );
            */
        }

        static inline void check_stat( cds::intrusive::ellen_bintree::stat<> const& stat )
        {
            CPPUNIT_CHECK_CURRENT_EX( stat.m_nInternalNodeCreated == stat.m_nInternalNodeDeleted,
                "m_nInternalNodeCreated=" << stat.m_nInternalNodeCreated
                << " m_nInternalNodeDeleted=" << stat.m_nInternalNodeDeleted );
            CPPUNIT_CHECK_CURRENT_EX( stat.m_nUpdateDescCreated == stat.m_nUpdateDescDeleted,
                "m_nUpdateDescCreated=" << stat.m_nUpdateDescCreated
                << " m_nUpdateDescDeleted=" << stat.m_nUpdateDescDeleted );
            CPPUNIT_CHECK_CURRENT_EX( ellen_bintree_pool::internal_node_counter::m_nAlloc.get() == stat.m_nInternalNodeCreated,
                "allocated=" << ellen_bintree_pool::internal_node_counter::m_nAlloc.get()
                << "m_nInternalNodeCreated=" << stat.m_nInternalNodeCreated );
        }
    }   // namespace ellen_bintree_check
    template <typename GC, typename Key, typename T, typename Traits>
    static inline void additional_check( cc::EllenBinTreeMap<GC, Key, T, Traits>& s )
    {
        GC::force_dispose();
        ellen_bintree_check::check_stat( s.statistics() );
    }


    template <typename K, typename V, typename Traits>
    static inline void print_stat( CuckooMap< K, V, Traits > const& m )
    {
        typedef CuckooMap< K, V, Traits > map_type;
        print_stat( static_cast<typename map_type::cuckoo_base_class const&>(m) );
    }
}   // namespace map2

#endif // ifndef _CDSUNIT_MAP2_MAP_TYPES_H
