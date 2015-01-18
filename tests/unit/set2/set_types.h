/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#ifndef _CDSUNIT_SET2_SET_TYPES_H
#define _CDSUNIT_SET2_SET_TYPES_H

#include <cds/urcu/general_instant.h>
#include <cds/urcu/general_buffered.h>
#include <cds/urcu/general_threaded.h>
#include <cds/urcu/signal_buffered.h>
#include <cds/urcu/signal_threaded.h>

#include <cds/container/michael_list_hp.h>
#include <cds/container/michael_list_dhp.h>
#include <cds/container/michael_list_rcu.h>
#include <cds/container/lazy_list_hp.h>
#include <cds/container/lazy_list_dhp.h>
#include <cds/container/lazy_list_rcu.h>

#include <cds/container/michael_set.h>
#include <cds/container/michael_set_rcu.h>

#include <cds/container/split_list_set.h>
#include <cds/container/split_list_set_rcu.h>

#include <cds/container/cuckoo_set.h>

#include <cds/container/skip_list_set_hp.h>
#include <cds/container/skip_list_set_dhp.h>
#include <cds/container/skip_list_set_rcu.h>

#include <cds/container/ellen_bintree_set_rcu.h>
#include <cds/container/ellen_bintree_set_hp.h>
#include <cds/container/ellen_bintree_set_dhp.h>

#include <cds/container/striped_set/std_list.h>
#include <cds/container/striped_set/std_vector.h>
#include <cds/container/striped_set/std_set.h>
#include <cds/container/striped_set/std_hash_set.h>
#include <cds/container/striped_set/boost_unordered_set.h>

#include <boost/version.hpp>
#if BOOST_VERSION >= 104800
#   include <cds/container/striped_set/boost_slist.h>
#   include <cds/container/striped_set/boost_list.h>
#   include <cds/container/striped_set/boost_vector.h>
#   include <cds/container/striped_set/boost_stable_vector.h>
#   include <cds/container/striped_set/boost_set.h>
#   include <cds/container/striped_set/boost_flat_set.h>
#endif
#include <cds/container/striped_set.h>

#include <cds/lock/spinlock.h>
#include <boost/functional/hash/hash.hpp>

#include "cppunit/cppunit_mini.h"
#include "lock/nolock.h"
#include "set2/std_set.h"
#include "set2/std_hash_set.h"
#include "michael_alloc.h"
#include "print_cuckoo_stat.h"
#include "print_split_list_stat.h"
#include "print_skip_list_stat.h"
#include "print_ellenbintree_stat.h"
#include "ellen_bintree_update_desc_pool.h"

namespace set2 {
    namespace cc = cds::container;
    namespace co = cds::opt;

    typedef cds::urcu::gc< cds::urcu::general_instant<> >   rcu_gpi;
    typedef cds::urcu::gc< cds::urcu::general_buffered<> >  rcu_gpb;
    typedef cds::urcu::gc< cds::urcu::general_threaded<> >  rcu_gpt;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
    typedef cds::urcu::gc< cds::urcu::signal_buffered<> >  rcu_shb;
    typedef cds::urcu::gc< cds::urcu::signal_threaded<> >  rcu_sht;
#endif

    template <typename V, typename Traits>
    class CuckooSet : public cc::CuckooSet< V, Traits >
    {
    public:
        typedef cc::CuckooSet< V, Traits > cuckoo_base_class;

    public:
        CuckooSet( size_t nCapacity, size_t nLoadFactor )
            : cuckoo_base_class( nCapacity / (nLoadFactor * 16), (unsigned int)4 )
        {}

        template <typename Q, typename Pred>
        bool erase_with( Q const& key, Pred /*pred*/ )
        {
            return cuckoo_base_class::erase_with( key, typename std::conditional< cuckoo_base_class::c_isSorted, Pred, typename Pred::equal_to>::type() );
        }
    };

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

    template <typename Key, typename Value>
    struct SetTypes {

        typedef Key     key_type;
        typedef Value   value_type;

        struct key_val {
            key_type    key;
            value_type  val;

            /*explicit*/ key_val( key_type const& k ): key(k), val() {}
            key_val( key_type const& k, value_type const& v ): key(k), val(v) {}

            template <typename K>
            /*explicit*/ key_val( K const& k ): key(k) {}

            template <typename K, typename T>
            key_val( K const& k, T const& v ): key(k), val(v) {}
        };

        typedef co::v::hash<key_type>   key_hash;
        typedef std::less<key_type>     key_less;
        typedef cmp<key_type>           key_compare;

        struct less {
            bool operator()( key_val const& k1, key_val const& k2 ) const
            {
                return key_less()( k1.key, k2.key );
            }
            bool operator()( key_type const& k1, key_val const& k2 ) const
            {
                return key_less()( k1, k2.key );
            }
            bool operator()( key_val const& k1, key_type const& k2 ) const
            {
                return key_less()( k1.key, k2 );
            }
        };

        struct compare {
            int operator()( key_val const& k1, key_val const& k2 ) const
            {
                return key_compare()( k1.key, k2.key );
            }
            int operator()( key_type const& k1, key_val const& k2 ) const
            {
                return key_compare()( k1, k2.key );
            }
            int operator()( key_val const& k1, key_type const& k2 ) const
            {
                return key_compare()( k1.key, k2 );
            }
        };

        struct equal_to {
            bool operator()( key_val const& k1, key_val const& k2 ) const
            {
                return key_compare()( k1.key, k2.key ) == 0;
            }
            bool operator()( key_type const& k1, key_val const& k2 ) const
            {
                return key_compare()( k1, k2.key ) == 0;
            }
            bool operator()( key_val const& k1, key_type const& k2 ) const
            {
                return key_compare()( k1.key, k2 ) == 0;
            }
        };


        struct hash: public key_hash
        {
            size_t operator()( key_val const& v ) const
            {
                return key_hash::operator()( v.key );
            }
            size_t operator()( key_type const& key ) const
            {
                return key_hash::operator()( key );
            }
            template <typename Q>
            size_t operator()( Q const& k ) const
            {
                return key_hash::operator()( k );
            }
        };

        struct hash2: public hash
        {
            size_t operator()( key_val const& k ) const
            {
                size_t seed = ~hash::operator ()( k );
                boost::hash_combine( seed, k.key );
                return seed;
            }
            size_t operator()( key_type const& k ) const
            {
                size_t seed = ~hash::operator ()( k );
                boost::hash_combine( seed, k );
                return seed;
            }
            template <typename Q>
            size_t operator()( Q const& k ) const
            {
                return key_hash::operator()( k );
            }
        };

        // ***************************************************************************
        // MichaelList

        struct traits_MichaelList_cmp_stdAlloc:
            public cc::michael_list::make_traits<
                co::compare< compare >
            >::type
        {};
        typedef cc::MichaelList< cds::gc::HP,  key_val, traits_MichaelList_cmp_stdAlloc > MichaelList_HP_cmp_stdAlloc;
        typedef cc::MichaelList< cds::gc::DHP, key_val, traits_MichaelList_cmp_stdAlloc > MichaelList_DHP_cmp_stdAlloc;
        typedef cc::MichaelList< rcu_gpi, key_val, traits_MichaelList_cmp_stdAlloc > MichaelList_RCU_GPI_cmp_stdAlloc;
        typedef cc::MichaelList< rcu_gpb, key_val, traits_MichaelList_cmp_stdAlloc > MichaelList_RCU_GPB_cmp_stdAlloc;
        typedef cc::MichaelList< rcu_gpt, key_val, traits_MichaelList_cmp_stdAlloc > MichaelList_RCU_GPT_cmp_stdAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelList< rcu_shb, key_val, traits_MichaelList_cmp_stdAlloc > MichaelList_RCU_SHB_cmp_stdAlloc;
        typedef cc::MichaelList< rcu_sht, key_val, traits_MichaelList_cmp_stdAlloc > MichaelList_RCU_SHT_cmp_stdAlloc;
#endif

        struct traits_MichaelList_cmp_stdAlloc_seqcst : public traits_MichaelList_cmp_stdAlloc
        {
            typedef co::v::sequential_consistent memory_model;
        };
        typedef cc::MichaelList< cds::gc::HP,  key_val, traits_MichaelList_cmp_stdAlloc_seqcst > MichaelList_HP_cmp_stdAlloc_seqcst;
        typedef cc::MichaelList< cds::gc::DHP, key_val, traits_MichaelList_cmp_stdAlloc_seqcst > MichaelList_DHP_cmp_stdAlloc_seqcst;
        typedef cc::MichaelList< rcu_gpi, key_val, traits_MichaelList_cmp_stdAlloc_seqcst > MichaelList_RCU_GPI_cmp_stdAlloc_seqcst;
        typedef cc::MichaelList< rcu_gpb, key_val, traits_MichaelList_cmp_stdAlloc_seqcst > MichaelList_RCU_GPB_cmp_stdAlloc_seqcst;
        typedef cc::MichaelList< rcu_gpt, key_val, traits_MichaelList_cmp_stdAlloc_seqcst > MichaelList_RCU_GPT_cmp_stdAlloc_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelList< rcu_shb, key_val, traits_MichaelList_cmp_stdAlloc_seqcst > MichaelList_RCU_SHB_cmp_stdAlloc_seqcst;
        typedef cc::MichaelList< rcu_sht, key_val, traits_MichaelList_cmp_stdAlloc_seqcst > MichaelList_RCU_SHT_cmp_stdAlloc_seqcst;
#endif

        struct traits_MichaelList_less_stdAlloc :
            public cc::michael_list::make_traits<
                co::less< less >
            >::type
        {};
        typedef cc::MichaelList< cds::gc::HP,  key_val, traits_MichaelList_less_stdAlloc > MichaelList_HP_less_stdAlloc;
        typedef cc::MichaelList< cds::gc::DHP, key_val, traits_MichaelList_less_stdAlloc > MichaelList_DHP_less_stdAlloc;
        typedef cc::MichaelList< rcu_gpi, key_val, traits_MichaelList_less_stdAlloc > MichaelList_RCU_GPI_less_stdAlloc;
        typedef cc::MichaelList< rcu_gpb, key_val, traits_MichaelList_less_stdAlloc > MichaelList_RCU_GPB_less_stdAlloc;
        typedef cc::MichaelList< rcu_gpt, key_val, traits_MichaelList_less_stdAlloc > MichaelList_RCU_GPT_less_stdAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelList< rcu_shb, key_val, traits_MichaelList_less_stdAlloc > MichaelList_RCU_SHB_less_stdAlloc;
        typedef cc::MichaelList< rcu_sht, key_val, traits_MichaelList_less_stdAlloc > MichaelList_RCU_SHT_less_stdAlloc;
#endif

        struct traits_MichaelList_less_stdAlloc_seqcst :
            public cc::michael_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        {};
        typedef cc::MichaelList< cds::gc::HP,  key_val, traits_MichaelList_less_stdAlloc_seqcst > MichaelList_HP_less_stdAlloc_seqcst;
        typedef cc::MichaelList< cds::gc::DHP, key_val, traits_MichaelList_less_stdAlloc_seqcst > MichaelList_DHP_less_stdAlloc_seqcst;
        typedef cc::MichaelList< rcu_gpi, key_val, traits_MichaelList_less_stdAlloc_seqcst > MichaelList_RCU_GPI_less_stdAlloc_seqcst;
        typedef cc::MichaelList< rcu_gpb, key_val, traits_MichaelList_less_stdAlloc_seqcst > MichaelList_RCU_GPB_less_stdAlloc_seqcst;
        typedef cc::MichaelList< rcu_gpt, key_val, traits_MichaelList_less_stdAlloc_seqcst > MichaelList_RCU_GPT_less_stdAlloc_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelList< rcu_shb, key_val, traits_MichaelList_less_stdAlloc_seqcst > MichaelList_RCU_SHB_less_stdAlloc_seqcst;
        typedef cc::MichaelList< rcu_sht, key_val, traits_MichaelList_less_stdAlloc_seqcst > MichaelList_RCU_SHT_less_stdAlloc_seqcst;
#endif

        struct traits_MichaelList_cmp_michaelAlloc :
            public cc::michael_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        {};
        typedef cc::MichaelList< cds::gc::HP,  key_val, traits_MichaelList_cmp_michaelAlloc > MichaelList_HP_cmp_michaelAlloc;
        typedef cc::MichaelList< cds::gc::DHP, key_val, traits_MichaelList_cmp_michaelAlloc > MichaelList_DHP_cmp_michaelAlloc;
        typedef cc::MichaelList< rcu_gpi, key_val, traits_MichaelList_cmp_michaelAlloc > MichaelList_RCU_GPI_cmp_michaelAlloc;
        typedef cc::MichaelList< rcu_gpb, key_val, traits_MichaelList_cmp_michaelAlloc > MichaelList_RCU_GPB_cmp_michaelAlloc;
        typedef cc::MichaelList< rcu_gpt, key_val, traits_MichaelList_cmp_michaelAlloc > MichaelList_RCU_GPT_cmp_michaelAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelList< rcu_shb, key_val, traits_MichaelList_cmp_michaelAlloc > MichaelList_RCU_SHB_cmp_michaelAlloc;
        typedef cc::MichaelList< rcu_sht, key_val, traits_MichaelList_cmp_michaelAlloc > MichaelList_RCU_SHT_cmp_michaelAlloc;
#endif

        struct traits_MichaelList_less_michaelAlloc :
            public cc::michael_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        {};
        typedef cc::MichaelList< cds::gc::HP,  key_val, traits_MichaelList_less_michaelAlloc > MichaelList_HP_less_michaelAlloc;
        typedef cc::MichaelList< cds::gc::DHP, key_val, traits_MichaelList_less_michaelAlloc > MichaelList_DHP_less_michaelAlloc;
        typedef cc::MichaelList< rcu_gpi, key_val, traits_MichaelList_less_michaelAlloc > MichaelList_RCU_GPI_less_michaelAlloc;
        typedef cc::MichaelList< rcu_gpb, key_val, traits_MichaelList_less_michaelAlloc > MichaelList_RCU_GPB_less_michaelAlloc;
        typedef cc::MichaelList< rcu_gpt, key_val, traits_MichaelList_less_michaelAlloc > MichaelList_RCU_GPT_less_michaelAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelList< rcu_shb, key_val, traits_MichaelList_less_michaelAlloc > MichaelList_RCU_SHB_less_michaelAlloc;
        typedef cc::MichaelList< rcu_sht, key_val, traits_MichaelList_less_michaelAlloc > MichaelList_RCU_SHT_less_michaelAlloc;
#endif

        // ***************************************************************************
        // MichaelHashSet based on MichaelList

        struct traits_MichaelSet_stdAlloc :
            public cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        {};
        typedef cc::MichaelHashSet< cds::gc::HP,  MichaelList_HP_cmp_stdAlloc,  traits_MichaelSet_stdAlloc > MichaelSet_HP_cmp_stdAlloc;
        typedef cc::MichaelHashSet< cds::gc::DHP, MichaelList_DHP_cmp_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_DHP_cmp_stdAlloc;
        typedef cc::MichaelHashSet< rcu_gpi, MichaelList_RCU_GPI_cmp_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_RCU_GPI_cmp_stdAlloc;
        typedef cc::MichaelHashSet< rcu_gpb, MichaelList_RCU_GPB_cmp_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_RCU_GPB_cmp_stdAlloc;
        typedef cc::MichaelHashSet< rcu_gpt, MichaelList_RCU_GPT_cmp_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_RCU_GPT_cmp_stdAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashSet< rcu_shb, MichaelList_RCU_SHB_cmp_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_RCU_SHB_cmp_stdAlloc;
        typedef cc::MichaelHashSet< rcu_sht, MichaelList_RCU_SHT_cmp_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_RCU_SHT_cmp_stdAlloc;
#endif

        typedef cc::MichaelHashSet< cds::gc::HP, MichaelList_HP_less_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_HP_less_stdAlloc;
        typedef cc::MichaelHashSet< cds::gc::DHP, MichaelList_DHP_less_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_DHP_less_stdAlloc;
        typedef cc::MichaelHashSet< rcu_gpi, MichaelList_RCU_GPI_less_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_RCU_GPI_less_stdAlloc;
        typedef cc::MichaelHashSet< rcu_gpb, MichaelList_RCU_GPB_less_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_RCU_GPB_less_stdAlloc;
        typedef cc::MichaelHashSet< rcu_gpt, MichaelList_RCU_GPT_less_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_RCU_GPT_less_stdAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashSet< rcu_shb, MichaelList_RCU_SHB_less_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_RCU_SHB_less_stdAlloc;
        typedef cc::MichaelHashSet< rcu_sht, MichaelList_RCU_SHT_less_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_RCU_SHT_less_stdAlloc;
#endif

        typedef cc::MichaelHashSet< cds::gc::HP, MichaelList_HP_less_stdAlloc_seqcst, traits_MichaelSet_stdAlloc > MichaelSet_HP_less_stdAlloc_seqcst;
        typedef cc::MichaelHashSet< cds::gc::DHP, MichaelList_DHP_less_stdAlloc_seqcst, traits_MichaelSet_stdAlloc > MichaelSet_DHP_less_stdAlloc_seqcst;
        typedef cc::MichaelHashSet< rcu_gpi, MichaelList_RCU_GPI_less_stdAlloc_seqcst, traits_MichaelSet_stdAlloc > MichaelSet_RCU_GPI_less_stdAlloc_seqcst;
        typedef cc::MichaelHashSet< rcu_gpb, MichaelList_RCU_GPB_less_stdAlloc_seqcst, traits_MichaelSet_stdAlloc > MichaelSet_RCU_GPB_less_stdAlloc_seqcst;
        typedef cc::MichaelHashSet< rcu_gpt, MichaelList_RCU_GPT_less_stdAlloc_seqcst, traits_MichaelSet_stdAlloc > MichaelSet_RCU_GPT_less_stdAlloc_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashSet< rcu_shb, MichaelList_RCU_SHB_less_stdAlloc_seqcst, traits_MichaelSet_stdAlloc > MichaelSet_RCU_SHB_less_stdAlloc_seqcst;
        typedef cc::MichaelHashSet< rcu_sht, MichaelList_RCU_SHT_less_stdAlloc_seqcst, traits_MichaelSet_stdAlloc > MichaelSet_RCU_SHT_less_stdAlloc_seqcst;
#endif

        struct traits_MichaelSet_michaelAlloc :
            public cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        {};
        typedef cc::MichaelHashSet< cds::gc::HP,  MichaelList_HP_cmp_michaelAlloc,  traits_MichaelSet_michaelAlloc > MichaelSet_HP_cmp_michaelAlloc;
        typedef cc::MichaelHashSet< cds::gc::DHP, MichaelList_DHP_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_DHP_cmp_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_gpi, MichaelList_RCU_GPI_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_RCU_GPI_cmp_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_gpb, MichaelList_RCU_GPB_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_RCU_GPB_cmp_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_gpt, MichaelList_RCU_GPT_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_RCU_GPT_cmp_michaelAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashSet< rcu_shb, MichaelList_RCU_SHB_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_RCU_SHB_cmp_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_sht, MichaelList_RCU_SHT_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_RCU_SHT_cmp_michaelAlloc;
#endif

        typedef cc::MichaelHashSet< cds::gc::HP, MichaelList_HP_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_HP_less_michaelAlloc;
        typedef cc::MichaelHashSet< cds::gc::DHP, MichaelList_DHP_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_DHP_less_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_gpi, MichaelList_RCU_GPI_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_RCU_GPI_less_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_gpb, MichaelList_RCU_GPB_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_RCU_GPB_less_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_gpt, MichaelList_RCU_GPT_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_RCU_GPT_less_michaelAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashSet< rcu_shb, MichaelList_RCU_SHB_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_RCU_SHB_less_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_sht, MichaelList_RCU_SHT_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_RCU_SHT_less_michaelAlloc;
#endif


        // ***************************************************************************
        // LazyList

        struct traits_LazyList_cmp_stdAlloc :
            public cc::lazy_list::make_traits<
                co::compare< compare >
            >::type
        {};
        typedef cc::LazyList< cds::gc::HP,  key_val, traits_LazyList_cmp_stdAlloc > LazyList_HP_cmp_stdAlloc;
        typedef cc::LazyList< cds::gc::DHP, key_val, traits_LazyList_cmp_stdAlloc > LazyList_DHP_cmp_stdAlloc;
        typedef cc::LazyList< rcu_gpi, key_val, traits_LazyList_cmp_stdAlloc > LazyList_RCU_GPI_cmp_stdAlloc;
        typedef cc::LazyList< rcu_gpb, key_val, traits_LazyList_cmp_stdAlloc > LazyList_RCU_GPB_cmp_stdAlloc;
        typedef cc::LazyList< rcu_gpt, key_val, traits_LazyList_cmp_stdAlloc > LazyList_RCU_GPT_cmp_stdAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyList< rcu_shb, key_val, traits_LazyList_cmp_stdAlloc > LazyList_RCU_SHB_cmp_stdAlloc;
        typedef cc::LazyList< rcu_sht, key_val, traits_LazyList_cmp_stdAlloc > LazyList_RCU_SHT_cmp_stdAlloc;
#endif
        struct traits_LazyList_cmp_stdAlloc_seqcst :
            public cc::lazy_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        {};
        typedef cc::LazyList< cds::gc::HP, key_val,  traits_LazyList_cmp_stdAlloc_seqcst > LazyList_HP_cmp_stdAlloc_seqcst;
        typedef cc::LazyList< cds::gc::DHP, key_val, traits_LazyList_cmp_stdAlloc_seqcst > LazyList_DHP_cmp_stdAlloc_seqcst;
        typedef cc::LazyList< rcu_gpi, key_val, traits_LazyList_cmp_stdAlloc_seqcst > LazyList_RCU_GPI_cmp_stdAlloc_seqcst;
        typedef cc::LazyList< rcu_gpb, key_val, traits_LazyList_cmp_stdAlloc_seqcst > LazyList_RCU_GPB_cmp_stdAlloc_seqcst;
        typedef cc::LazyList< rcu_gpt, key_val, traits_LazyList_cmp_stdAlloc_seqcst > LazyList_RCU_GPT_cmp_stdAlloc_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyList< rcu_shb, key_val, traits_LazyList_cmp_stdAlloc_seqcst > LazyList_RCU_SHB_cmp_stdAlloc_seqcst;
        typedef cc::LazyList< rcu_sht, key_val, traits_LazyList_cmp_stdAlloc_seqcst > LazyList_RCU_SHT_cmp_stdAlloc_seqcst;
#endif
        struct traits_LazyList_cmp_michaelAlloc :
            public cc::lazy_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        {};
        typedef cc::LazyList< cds::gc::HP,  key_val, traits_LazyList_cmp_michaelAlloc > LazyList_HP_cmp_michaelAlloc;
        typedef cc::LazyList< cds::gc::DHP, key_val, traits_LazyList_cmp_michaelAlloc > LazyList_DHP_cmp_michaelAlloc;
        typedef cc::LazyList< rcu_gpi, key_val, traits_LazyList_cmp_michaelAlloc > LazyList_RCU_GPI_cmp_michaelAlloc;
        typedef cc::LazyList< rcu_gpb, key_val, traits_LazyList_cmp_michaelAlloc > LazyList_RCU_GPB_cmp_michaelAlloc;
        typedef cc::LazyList< rcu_gpt, key_val, traits_LazyList_cmp_michaelAlloc > LazyList_RCU_GPT_cmp_michaelAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyList< rcu_shb, key_val, traits_LazyList_cmp_michaelAlloc > LazyList_RCU_SHB_cmp_michaelAlloc;
        typedef cc::LazyList< rcu_sht, key_val, traits_LazyList_cmp_michaelAlloc > LazyList_RCU_SHT_cmp_michaelAlloc;
#endif

        struct traits_LazyList_less_stdAlloc:
            public cc::lazy_list::make_traits<
                co::less< less >
            >::type
        {};
        typedef cc::LazyList< cds::gc::HP,  key_val, traits_LazyList_less_stdAlloc > LazyList_HP_less_stdAlloc;
        typedef cc::LazyList< cds::gc::DHP, key_val, traits_LazyList_less_stdAlloc > LazyList_DHP_less_stdAlloc;
        typedef cc::LazyList< rcu_gpi, key_val, traits_LazyList_less_stdAlloc > LazyList_RCU_GPI_less_stdAlloc;
        typedef cc::LazyList< rcu_gpb, key_val, traits_LazyList_less_stdAlloc > LazyList_RCU_GPB_less_stdAlloc;
        typedef cc::LazyList< rcu_gpt, key_val, traits_LazyList_less_stdAlloc > LazyList_RCU_GPT_less_stdAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyList< rcu_shb, key_val, traits_LazyList_less_stdAlloc > LazyList_RCU_SHB_less_stdAlloc;
        typedef cc::LazyList< rcu_sht, key_val, traits_LazyList_less_stdAlloc > LazyList_RCU_SHT_less_stdAlloc;
#endif

        struct traits_LazyList_less_stdAlloc_seqcst :
            public cc::lazy_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        {};
        typedef cc::LazyList< cds::gc::HP, key_val,  traits_LazyList_less_stdAlloc_seqcst > LazyList_HP_less_stdAlloc_seqcst;
        typedef cc::LazyList< cds::gc::DHP, key_val, traits_LazyList_less_stdAlloc_seqcst > LazyList_DHP_less_stdAlloc_seqcst;
        typedef cc::LazyList< rcu_gpi, key_val, traits_LazyList_less_stdAlloc_seqcst > LazyList_RCU_GPI_less_stdAlloc_seqcst;
        typedef cc::LazyList< rcu_gpb, key_val, traits_LazyList_less_stdAlloc_seqcst > LazyList_RCU_GPB_less_stdAlloc_seqcst;
        typedef cc::LazyList< rcu_gpt, key_val, traits_LazyList_less_stdAlloc_seqcst > LazyList_RCU_GPT_less_stdAlloc_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyList< rcu_shb, key_val, traits_LazyList_less_stdAlloc_seqcst > LazyList_RCU_SHB_less_stdAlloc_seqcst;
        typedef cc::LazyList< rcu_sht, key_val, traits_LazyList_less_stdAlloc_seqcst > LazyList_RCU_SHT_less_stdAlloc_seqcst;
#endif

        struct traits_LazyList_less_michaelAlloc :
            public cc::lazy_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        {};
        typedef cc::LazyList< cds::gc::HP,  key_val, traits_LazyList_less_michaelAlloc > LazyList_HP_less_michaelAlloc;
        typedef cc::LazyList< cds::gc::DHP, key_val, traits_LazyList_less_michaelAlloc > LazyList_DHP_less_michaelAlloc;
        typedef cc::LazyList< rcu_gpi, key_val, traits_LazyList_less_michaelAlloc > LazyList_RCU_GPI_less_michaelAlloc;
        typedef cc::LazyList< rcu_gpb, key_val, traits_LazyList_less_michaelAlloc > LazyList_RCU_GPB_less_michaelAlloc;
        typedef cc::LazyList< rcu_gpt, key_val, traits_LazyList_less_michaelAlloc > LazyList_RCU_GPT_less_michaelAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyList< rcu_shb, key_val, traits_LazyList_less_michaelAlloc > LazyList_RCU_SHB_less_michaelAlloc;
        typedef cc::LazyList< rcu_sht, key_val, traits_LazyList_less_michaelAlloc > LazyList_RCU_SHT_less_michaelAlloc;
#endif

        // ***************************************************************************
        // MichaelHashSet based on LazyList

        typedef cc::MichaelHashSet< cds::gc::HP, LazyList_HP_cmp_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_HP_cmp_stdAlloc;
        typedef cc::MichaelHashSet< cds::gc::DHP, LazyList_DHP_cmp_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_DHP_cmp_stdAlloc;
        typedef cc::MichaelHashSet< rcu_gpi, LazyList_RCU_GPI_cmp_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_GPI_cmp_stdAlloc;
        typedef cc::MichaelHashSet< rcu_gpb, LazyList_RCU_GPB_cmp_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_GPB_cmp_stdAlloc;
        typedef cc::MichaelHashSet< rcu_gpt, LazyList_RCU_GPT_cmp_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_GPT_cmp_stdAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashSet< rcu_shb, LazyList_RCU_SHB_cmp_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_SHB_cmp_stdAlloc;
        typedef cc::MichaelHashSet< rcu_sht, LazyList_RCU_SHT_cmp_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_SHT_cmp_stdAlloc;
#endif

        typedef cc::MichaelHashSet< cds::gc::HP, LazyList_HP_less_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_HP_less_stdAlloc;
        typedef cc::MichaelHashSet< cds::gc::DHP, LazyList_DHP_less_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_DHP_less_stdAlloc;
        typedef cc::MichaelHashSet< rcu_gpi, LazyList_RCU_GPI_less_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_GPI_less_stdAlloc;
        typedef cc::MichaelHashSet< rcu_gpb, LazyList_RCU_GPB_less_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_GPB_less_stdAlloc;
        typedef cc::MichaelHashSet< rcu_gpt, LazyList_RCU_GPT_less_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_GPT_less_stdAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashSet< rcu_shb, LazyList_RCU_SHB_less_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_SHB_less_stdAlloc;
        typedef cc::MichaelHashSet< rcu_sht, LazyList_RCU_SHT_less_stdAlloc, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_SHT_less_stdAlloc;
#endif

        typedef cc::MichaelHashSet< cds::gc::HP, LazyList_HP_less_stdAlloc_seqcst, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_HP_less_stdAlloc_seqcst;
        typedef cc::MichaelHashSet< cds::gc::DHP, LazyList_DHP_less_stdAlloc_seqcst, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_DHP_less_stdAlloc_seqcst;
        typedef cc::MichaelHashSet< rcu_gpi, LazyList_RCU_GPI_less_stdAlloc_seqcst, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_GPI_less_stdAlloc_seqcst;
        typedef cc::MichaelHashSet< rcu_gpb, LazyList_RCU_GPB_less_stdAlloc_seqcst, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_GPB_less_stdAlloc_seqcst;
        typedef cc::MichaelHashSet< rcu_gpt, LazyList_RCU_GPT_less_stdAlloc_seqcst, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_GPT_less_stdAlloc_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashSet< rcu_shb, LazyList_RCU_SHB_less_stdAlloc_seqcst, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_SHB_less_stdAlloc_seqcst;
        typedef cc::MichaelHashSet< rcu_sht, LazyList_RCU_SHT_less_stdAlloc_seqcst, traits_MichaelSet_stdAlloc > MichaelSet_Lazy_RCU_SHT_less_stdAlloc_seqcst;
#endif

        typedef cc::MichaelHashSet< cds::gc::HP, LazyList_HP_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_Lazy_HP_cmp_michaelAlloc;
        typedef cc::MichaelHashSet< cds::gc::DHP, LazyList_DHP_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_Lazy_DHP_cmp_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_gpi, LazyList_RCU_GPI_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_Lazy_RCU_GPI_cmp_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_gpb, LazyList_RCU_GPB_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_Lazy_RCU_GPB_cmp_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_gpt, LazyList_RCU_GPT_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_Lazy_RCU_GPT_cmp_michaelAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashSet< rcu_shb, LazyList_RCU_SHB_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_Lazy_RCU_SHB_cmp_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_sht, LazyList_RCU_SHT_cmp_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_Lazy_RCU_SHT_cmp_michaelAlloc;
#endif

        typedef cc::MichaelHashSet< cds::gc::HP, LazyList_HP_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_Lazy_HP_less_michaelAlloc;
        typedef cc::MichaelHashSet< cds::gc::DHP, LazyList_DHP_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_Lazy_DHP_less_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_gpi, LazyList_RCU_GPI_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_Lazy_RCU_GPI_less_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_gpb, LazyList_RCU_GPB_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_Lazy_RCU_GPB_less_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_gpt, LazyList_RCU_GPT_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_Lazy_RCU_GPT_less_michaelAlloc;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashSet< rcu_shb, LazyList_RCU_SHB_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_Lazy_RCU_SHB_less_michaelAlloc;
        typedef cc::MichaelHashSet< rcu_sht, LazyList_RCU_SHT_less_michaelAlloc, traits_MichaelSet_michaelAlloc > MichaelSet_Lazy_RCU_SHT_less_michaelAlloc;
#endif

        // ***************************************************************************
        // SplitListSet based on MichaelList

        struct traits_SplitList_Michael_dyn_cmp :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListSet< cds::gc::HP,  key_val, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_HP_dyn_cmp;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_DHP_dyn_cmp;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_RCU_GPI_dyn_cmp;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_RCU_GPB_dyn_cmp;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_RCU_GPT_dyn_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_RCU_SHB_dyn_cmp;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Michael_dyn_cmp > SplitList_Michael_RCU_SHT_dyn_cmp;
#endif

        struct traits_SplitList_Michael_dyn_cmp_stat :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,co::stat< cc::split_list::stat<> >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListSet< cds::gc::HP, key_val, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_HP_dyn_cmp_stat;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_DHP_dyn_cmp_stat;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_RCU_GPI_dyn_cmp_stat;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_RCU_GPB_dyn_cmp_stat;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_RCU_GPT_dyn_cmp_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_RCU_SHB_dyn_cmp_stat;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Michael_dyn_cmp_stat > SplitList_Michael_RCU_SHT_dyn_cmp_stat;
#endif

        struct traits_SplitList_Michael_dyn_cmp_seqcst :
            public cc::split_list::make_traits<
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
        typedef cc::SplitListSet< cds::gc::HP,  key_val, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_HP_dyn_cmp_seqcst;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_DHP_dyn_cmp_seqcst;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_RCU_GPI_dyn_cmp_seqcst;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_RCU_GPB_dyn_cmp_seqcst;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_RCU_GPT_dyn_cmp_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_RCU_SHB_dyn_cmp_seqcst;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Michael_dyn_cmp_seqcst > SplitList_Michael_RCU_SHT_dyn_cmp_seqcst;
#endif

        struct traits_SplitList_Michael_st_cmp :
            public cc::split_list::make_traits<
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
        typedef cc::SplitListSet< cds::gc::HP,  key_val, traits_SplitList_Michael_st_cmp > SplitList_Michael_HP_st_cmp;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Michael_st_cmp > SplitList_Michael_DHP_st_cmp;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Michael_st_cmp > SplitList_Michael_RCU_GPI_st_cmp;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Michael_st_cmp > SplitList_Michael_RCU_GPB_st_cmp;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Michael_st_cmp > SplitList_Michael_RCU_GPT_st_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Michael_st_cmp > SplitList_Michael_RCU_SHB_st_cmp;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Michael_st_cmp > SplitList_Michael_RCU_SHT_st_cmp;
#endif

        struct traits_SplitList_Michael_st_cmp_seqcst :
            public cc::split_list::make_traits<
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
        typedef cc::SplitListSet< cds::gc::HP,  key_val, traits_SplitList_Michael_st_cmp_seqcst> SplitList_Michael_HP_st_cmp_seqcst;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Michael_st_cmp_seqcst> SplitList_Michael_DHP_st_cmp_seqcst;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Michael_st_cmp_seqcst> SplitList_Michael_RCU_GPI_st_cmp_seqcst;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Michael_st_cmp_seqcst> SplitList_Michael_RCU_GPB_st_cmp_seqcst;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Michael_st_cmp_seqcst> SplitList_Michael_RCU_GPT_st_cmp_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Michael_st_cmp_seqcst> SplitList_Michael_RCU_SHB_st_cmp_seqcst;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Michael_st_cmp_seqcst> SplitList_Michael_RCU_SHT_st_cmp_seqcst;
#endif

        //HP + less
        struct traits_SplitList_Michael_dyn_less :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListSet< cds::gc::HP,  key_val, traits_SplitList_Michael_dyn_less > SplitList_Michael_HP_dyn_less;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Michael_dyn_less > SplitList_Michael_DHP_dyn_less;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Michael_dyn_less > SplitList_Michael_RCU_GPI_dyn_less;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Michael_dyn_less > SplitList_Michael_RCU_GPB_dyn_less;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Michael_dyn_less > SplitList_Michael_RCU_GPT_dyn_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Michael_dyn_less > SplitList_Michael_RCU_SHB_dyn_less;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Michael_dyn_less > SplitList_Michael_RCU_SHT_dyn_less;
#endif

        struct traits_SplitList_Michael_dyn_less_seqcst :
            public cc::split_list::make_traits<
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
        typedef cc::SplitListSet< cds::gc::HP, key_val,  traits_SplitList_Michael_dyn_less_seqcst > SplitList_Michael_HP_dyn_less_seqcst;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Michael_dyn_less_seqcst > SplitList_Michael_DHP_dyn_less_seqcst;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Michael_dyn_less_seqcst > SplitList_Michael_RCU_GPI_dyn_less_seqcst;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Michael_dyn_less_seqcst > SplitList_Michael_RCU_GPB_dyn_less_seqcst;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Michael_dyn_less_seqcst > SplitList_Michael_RCU_GPT_dyn_less_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Michael_dyn_less_seqcst > SplitList_Michael_RCU_SHB_dyn_less_seqcst;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Michael_dyn_less_seqcst > SplitList_Michael_RCU_SHT_dyn_less_seqcst;
#endif

        struct traits_SplitList_Michael_st_less :
            public cc::split_list::make_traits<
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
        typedef cc::SplitListSet< cds::gc::HP,  key_val, traits_SplitList_Michael_st_less > SplitList_Michael_HP_st_less;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Michael_st_less > SplitList_Michael_DHP_st_less;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Michael_st_less > SplitList_Michael_RCU_GPI_st_less;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Michael_st_less > SplitList_Michael_RCU_GPB_st_less;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Michael_st_less > SplitList_Michael_RCU_GPT_st_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Michael_st_less > SplitList_Michael_RCU_SHB_st_less;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Michael_st_less > SplitList_Michael_RCU_SHT_st_less;
#endif

        struct traits_SplitList_Michael_st_less_stat :
            public cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,co::stat< cc::split_list::stat<>>
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        {};
        typedef cc::SplitListSet< cds::gc::HP,  key_val, traits_SplitList_Michael_st_less_stat > SplitList_Michael_HP_st_less_stat;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Michael_st_less_stat > SplitList_Michael_DHP_st_less_stat;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Michael_st_less_stat > SplitList_Michael_RCU_GPI_st_less_stat;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Michael_st_less_stat > SplitList_Michael_RCU_GPB_st_less_stat;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Michael_st_less_stat > SplitList_Michael_RCU_GPT_st_less_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Michael_st_less_stat > SplitList_Michael_RCU_SHB_st_less_stat;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Michael_st_less_stat > SplitList_Michael_RCU_SHT_st_less_stat;
#endif

        struct traits_SplitList_Michael_st_less_seqcst :
            public cc::split_list::make_traits<
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
        typedef cc::SplitListSet< cds::gc::HP,  key_val, traits_SplitList_Michael_st_less_seqcst > SplitList_Michael_HP_st_less_seqcst;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Michael_st_less_seqcst > SplitList_Michael_DHP_st_less_seqcst;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Michael_st_less_seqcst > SplitList_Michael_RCU_GPI_st_less_seqcst;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Michael_st_less_seqcst > SplitList_Michael_RCU_GPB_st_less_seqcst;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Michael_st_less_seqcst > SplitList_Michael_RCU_GPT_st_less_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Michael_st_less_seqcst > SplitList_Michael_RCU_SHB_st_less_seqcst;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Michael_st_less_seqcst > SplitList_Michael_RCU_SHT_st_less_seqcst;
#endif

        // ***************************************************************************
        // SplitListSet based on LazyList

        struct traits_SplitList_Lazy_dyn_cmp :
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
        typedef cc::SplitListSet< cds::gc::HP, key_val, traits_SplitList_Lazy_dyn_cmp > SplitList_Lazy_HP_dyn_cmp;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Lazy_dyn_cmp > SplitList_Lazy_DHP_dyn_cmp;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_GPI_dyn_cmp;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_GPB_dyn_cmp;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_GPT_dyn_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_SHB_dyn_cmp;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_SHT_dyn_cmp;
#endif

        struct traits_SplitList_Lazy_dyn_cmp_stat : public traits_SplitList_Lazy_dyn_cmp
        {
            typedef cc::split_list::stat<> stat;
        };
        typedef cc::SplitListSet< cds::gc::HP, key_val, traits_SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_HP_dyn_cmp_stat;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_DHP_dyn_cmp_stat;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_RCU_GPI_dyn_cmp_stat;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_RCU_GPB_dyn_cmp_stat;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Lazy_dyn_cmp_stat > SplitList_Lazy_RCU_GPT_dyn_cmp_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_SHB_dyn_cmp_stat;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Lazy_dyn_cmp > SplitList_Lazy_RCU_SHT_dyn_cmp_stat;
#endif

        struct traits_SplitList_Lazy_dyn_cmp_seqcst :
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
        typedef cc::SplitListSet< cds::gc::HP, key_val, traits_SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_HP_dyn_cmp_seqcst;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_DHP_dyn_cmp_seqcst;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_RCU_GPI_dyn_cmp_seqcst;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_RCU_GPB_dyn_cmp_seqcst;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_RCU_GPT_dyn_cmp_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_RCU_SHB_dyn_cmp_seqcst;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Lazy_dyn_cmp_seqcst > SplitList_Lazy_RCU_SHT_dyn_cmp_seqcst;
#endif

        struct traits_SplitList_Lazy_st_cmp :
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
        typedef cc::SplitListSet< cds::gc::HP, key_val, traits_SplitList_Lazy_st_cmp > SplitList_Lazy_HP_st_cmp;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Lazy_st_cmp > SplitList_Lazy_DHP_st_cmp;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Lazy_st_cmp > SplitList_Lazy_RCU_GPI_st_cmp;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Lazy_st_cmp > SplitList_Lazy_RCU_GPB_st_cmp;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Lazy_st_cmp > SplitList_Lazy_RCU_GPT_st_cmp;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Lazy_st_cmp > SplitList_Lazy_RCU_SHB_st_cmp;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Lazy_st_cmp > SplitList_Lazy_RCU_SHT_st_cmp;
#endif

        struct traits_SplitList_Lazy_st_cmp_seqcst :
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
        typedef cc::SplitListSet< cds::gc::HP, key_val, traits_SplitList_Lazy_st_cmp_seqcst > SplitList_Lazy_HP_st_cmp_seqcst;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Lazy_st_cmp_seqcst > SplitList_Lazy_DHP_st_cmp_seqcst;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Lazy_st_cmp_seqcst > SplitList_Lazy_RCU_GPI_st_cmp_seqcst;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Lazy_st_cmp_seqcst > SplitList_Lazy_RCU_GPB_st_cmp_seqcst;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Lazy_st_cmp_seqcst > SplitList_Lazy_RCU_GPT_st_cmp_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Lazy_st_cmp_seqcst > SplitList_Lazy_RCU_SHB_st_cmp_seqcst;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Lazy_st_cmp_seqcst > SplitList_Lazy_RCU_SHT_st_cmp_seqcst;
#endif

        struct traits_SplitList_Lazy_dyn_less :
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
        typedef cc::SplitListSet< cds::gc::HP, key_val, traits_SplitList_Lazy_dyn_less > SplitList_Lazy_HP_dyn_less;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Lazy_dyn_less > SplitList_Lazy_DHP_dyn_less;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Lazy_dyn_less > SplitList_Lazy_RCU_GPI_dyn_less;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Lazy_dyn_less > SplitList_Lazy_RCU_GPB_dyn_less;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Lazy_dyn_less > SplitList_Lazy_RCU_GPT_dyn_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Lazy_dyn_less > SplitList_Lazy_RCU_SHB_dyn_less;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Lazy_dyn_less > SplitList_Lazy_RCU_SHT_dyn_less;
#endif

        struct traits_SplitList_Lazy_dyn_less_seqcst :
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
        typedef cc::SplitListSet< cds::gc::HP, key_val, traits_SplitList_Lazy_dyn_less_seqcst > SplitList_Lazy_HP_dyn_less_seqcst;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Lazy_dyn_less_seqcst > SplitList_Lazy_DHP_dyn_less_seqcst;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Lazy_dyn_less_seqcst > SplitList_Lazy_RCU_GPI_dyn_less_seqcst;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Lazy_dyn_less_seqcst > SplitList_Lazy_RCU_GPB_dyn_less_seqcst;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Lazy_dyn_less_seqcst > SplitList_Lazy_RCU_GPT_dyn_less_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Lazy_dyn_less_seqcst > SplitList_Lazy_RCU_SHB_dyn_less_seqcst;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Lazy_dyn_less_seqcst > SplitList_Lazy_RCU_SHT_dyn_less_seqcst;
#endif

        struct traits_SplitList_Lazy_st_less :
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
        typedef cc::SplitListSet< cds::gc::HP, key_val, traits_SplitList_Lazy_st_less > SplitList_Lazy_HP_st_less;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Lazy_st_less > SplitList_Lazy_DHP_st_less;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Lazy_st_less > SplitList_Lazy_RCU_GPI_st_less;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Lazy_st_less > SplitList_Lazy_RCU_GPB_st_less;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Lazy_st_less > SplitList_Lazy_RCU_GPT_st_less;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Lazy_st_less > SplitList_Lazy_RCU_SHB_st_less;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Lazy_st_less > SplitList_Lazy_RCU_SHT_st_less;
#endif

        struct traits_SplitList_Lazy_st_less_seqcst :
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
        typedef cc::SplitListSet< cds::gc::HP, key_val, traits_SplitList_Lazy_st_less_seqcst > SplitList_Lazy_HP_st_less_seqcst;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Lazy_st_less_seqcst > SplitList_Lazy_DHP_st_less_seqcst;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Lazy_st_less_seqcst > SplitList_Lazy_RCU_GPI_st_less_seqcst;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Lazy_st_less_seqcst > SplitList_Lazy_RCU_GPB_st_less_seqcst;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Lazy_st_less_seqcst > SplitList_Lazy_RCU_GPT_st_less_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Lazy_st_less_seqcst > SplitList_Lazy_RCU_SHB_st_less_seqcst;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Lazy_st_less_seqcst > SplitList_Lazy_RCU_SHT_st_less_seqcst;
#endif

        struct traits_SplitList_Lazy_st_less_stat : public traits_SplitList_Lazy_st_less
        {
            typedef cc::split_list::stat<> stat;
        };
        typedef cc::SplitListSet< cds::gc::HP, key_val, traits_SplitList_Lazy_st_less_stat > SplitList_Lazy_HP_st_less_stat;
        typedef cc::SplitListSet< cds::gc::DHP, key_val, traits_SplitList_Lazy_st_less_stat > SplitList_Lazy_DHP_st_less_stat;
        typedef cc::SplitListSet< rcu_gpi, key_val, traits_SplitList_Lazy_st_less_stat > SplitList_Lazy_RCU_GPI_st_less_stat;
        typedef cc::SplitListSet< rcu_gpb, key_val, traits_SplitList_Lazy_st_less_stat > SplitList_Lazy_RCU_GPB_st_less_stat;
        typedef cc::SplitListSet< rcu_gpt, key_val, traits_SplitList_Lazy_st_less_stat > SplitList_Lazy_RCU_GPT_st_less_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val, traits_SplitList_Lazy_st_less_stat > SplitList_Lazy_RCU_SHB_st_less_stat;
        typedef cc::SplitListSet< rcu_sht, key_val, traits_SplitList_Lazy_st_less_stat > SplitList_Lazy_RCU_SHT_st_less_stat;
#endif


        // ***************************************************************************
        // StripedSet

        // for sequential containers
        template <class BucketEntry, typename... Options>
        class StripedHashSet_seq:
            public cc::StripedSet< BucketEntry,
                co::mutex_policy< cc::striped_set::striping<> >
                ,co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                , Options...
            >
        {
            typedef cc::StripedSet< BucketEntry,
                co::mutex_policy< cc::striped_set::striping<> >
                ,co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                , Options...
            > base_class;
            typedef typename base_class::resizing_policy resizing_policy_t;

            resizing_policy_t   m_placeHolder;
        public:
            StripedHashSet_seq( size_t nCapacity, size_t nLoadFactor )
                : base_class( nCapacity / nLoadFactor / 16, *(new(&m_placeHolder) resizing_policy_t( nLoadFactor )) )
            {}

            template <typename Q, typename Less>
            bool erase_with( Q const& v, Less /*pred*/ )
            {
                return base_class::erase( v );
            }
        };

        // for non-sequential ordered containers
        template <class BucketEntry, typename... Options>
        class StripedHashSet_ord:
            public cc::StripedSet< BucketEntry,
                co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::striping<> >
                , Options...
            >
        {
            typedef cc::StripedSet< BucketEntry,
               co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::striping<> >
                , Options...
            > base_class;
            typedef typename base_class::resizing_policy resizing_policy_t;

            resizing_policy_t   m_placeHolder;
        public:
            StripedHashSet_ord( size_t /*nCapacity*/, size_t nLoadFactor )
                : base_class( 0, *(new(&m_placeHolder) resizing_policy_t( nLoadFactor * 1024 )) )
            {}

            template <typename Q, typename Less>
            bool erase_with( Q const& v, Less /*pred*/ )
            {
                return base_class::erase( v );
            }
        };

        typedef StripedHashSet_seq<
            std::list< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > StripedSet_list;

        typedef StripedHashSet_seq<
            std::vector< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > StripedSet_vector;

#if BOOST_VERSION >= 104800
        typedef StripedHashSet_seq<
            boost::container::slist< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > StripedSet_boost_slist;

        typedef StripedHashSet_seq<
            boost::container::list< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > StripedSet_boost_list;

        typedef StripedHashSet_seq<
            boost::container::vector< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > StripedSet_boost_vector;

        typedef StripedHashSet_seq<
            boost::container::stable_vector< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > StripedSet_boost_stable_vector;
#endif

        typedef StripedHashSet_ord<
            std::set< key_val, less >
            , co::hash< hash2 >
        > StripedSet_set;

        typedef StripedHashSet_ord<
            std::unordered_set< key_val, hash, equal_to >
            , co::hash< hash2 >
        > StripedSet_hashset;

#if BOOST_VERSION >= 104800
        typedef StripedHashSet_ord<
            boost::container::set< key_val, less >
            , co::hash< hash2 >
        > StripedSet_boost_set;

        typedef StripedHashSet_ord<
            boost::container::flat_set< key_val, less >
            , co::hash< hash2 >
        > StripedSet_boost_flat_set;
#endif

        typedef StripedHashSet_ord<
            boost::unordered_set< key_val, hash, equal_to >
            , co::hash< hash2 >
        > StripedSet_boost_unordered_set;



        // ***************************************************************************
        // RefinableSet

        // for sequential containers
        template <class BucketEntry, typename... Options>
        class RefinableHashSet_seq:
            public cc::StripedSet< BucketEntry,
            co::mutex_policy< cc::striped_set::refinable<> >
            ,co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
            , Options...
            >
        {
            typedef cc::StripedSet< BucketEntry,
                co::mutex_policy< cc::striped_set::refinable<> >
                ,co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                , Options...
            > base_class;
            typedef typename base_class::resizing_policy resizing_policy_t;

            resizing_policy_t   m_placeHolder;
        public:
            RefinableHashSet_seq( size_t nCapacity, size_t nLoadFactor )
                : base_class( nCapacity / nLoadFactor / 16, *(new(&m_placeHolder) resizing_policy_t( nLoadFactor )) )
            {}

            template <typename Q, typename Less>
            bool erase_with( Q const& v, Less /*pred*/ )
            {
                return base_class::erase( v );
            }
        };

        // for non-sequential ordered containers
        template <class BucketEntry, typename... Options>
        class RefinableHashSet_ord:
            public cc::StripedSet< BucketEntry,
                co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::refinable<> >
                , Options...
            >
        {
            typedef cc::StripedSet< BucketEntry,
                co::resizing_policy<cc::striped_set::load_factor_resizing<0> >
                ,co::mutex_policy< cc::striped_set::refinable<> >
                , Options...
            > base_class;
            typedef typename base_class::resizing_policy resizing_policy_t;

            resizing_policy_t   m_placeHolder;
        public:
            RefinableHashSet_ord( size_t /*nCapacity*/, size_t nLoadFactor )
                : base_class( 0, *(new(&m_placeHolder) resizing_policy_t( nLoadFactor * 1024 )) )
            {}

            template <typename Q, typename Less>
            bool erase_with( Q const& v, Less /*pred*/ )
            {
                return base_class::erase( v );
            }
        };

        typedef RefinableHashSet_seq<
            std::list< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > RefinableSet_list;

        typedef RefinableHashSet_seq<
            std::vector< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > RefinableSet_vector;

#if BOOST_VERSION >= 104800
        typedef RefinableHashSet_seq<
            boost::container::slist< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > RefinableSet_boost_slist;

        typedef RefinableHashSet_seq<
            boost::container::list< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > RefinableSet_boost_list;

        typedef RefinableHashSet_seq<
            boost::container::vector< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > RefinableSet_boost_vector;

        typedef RefinableHashSet_seq<
            boost::container::stable_vector< key_val >
            , co::hash< hash2 >
            , co::less< less >
        > RefinableSet_boost_stable_vector;
#endif

        typedef RefinableHashSet_ord<
            std::set< key_val, less >
            , co::hash< hash2 >
        > RefinableSet_set;

        typedef RefinableHashSet_ord<
            std::unordered_set< key_val, hash, equal_to >
            , co::hash< hash2 >
        > RefinableSet_hashset;

#if BOOST_VERSION >= 104800
        typedef RefinableHashSet_ord<
            boost::container::set< key_val, less >
            , co::hash< hash2 >
        > RefinableSet_boost_set;

        typedef RefinableHashSet_ord<
            boost::container::flat_set< key_val, less >
            , co::hash< hash2 >
        > RefinableSet_boost_flat_set;
#endif

        typedef RefinableHashSet_ord<
            boost::unordered_set< key_val, hash, equal_to >
            , co::hash< hash2 >
        > RefinableSet_boost_unordered_set;

        // ***************************************************************************
        // CuckooSet

        template <typename Traits>
        struct traits_CuckooStripedSet : public Traits
        {
            typedef cc::cuckoo::striping<> mutex_policy;
        };
        template <typename Traits>
        struct traits_CuckooRefinableSet : public Traits
        {
            typedef cc::cuckoo::refinable<> mutex_policy;
        };

        struct traits_CuckooSet_list_unord :
            public cc::cuckoo::make_traits <
                cc::cuckoo::probeset_type< cc::cuckoo::list >
                , co::equal_to< equal_to >
                , co::hash< std::tuple< hash, hash2 > >
            > ::type
        {};
        typedef CuckooSet< key_val, traits_CuckooStripedSet<traits_CuckooSet_list_unord>> CuckooStripedSet_list_unord;
        typedef CuckooSet< key_val, traits_CuckooRefinableSet<traits_CuckooSet_list_unord>> CuckooRefinableSet_list_unord;

        struct traits_CuckooSet_list_unord_stat : public traits_CuckooSet_list_unord
        {
            typedef cc::cuckoo::stat stat;
        };
        typedef CuckooSet< key_val, traits_CuckooStripedSet<traits_CuckooSet_list_unord_stat>> CuckooStripedSet_list_unord_stat;
        typedef CuckooSet< key_val, traits_CuckooRefinableSet<traits_CuckooSet_list_unord_stat>> CuckooRefinableSet_list_unord_stat;

        struct traits_CuckooSet_list_unord_storehash : public traits_CuckooSet_list_unord
        {
            static CDS_CONSTEXPR const bool store_hash = true;
        };
        typedef CuckooSet< key_val, traits_CuckooStripedSet<traits_CuckooSet_list_unord_storehash>> CuckooStripedSet_list_unord_storehash;
        typedef CuckooSet< key_val, traits_CuckooRefinableSet<traits_CuckooSet_list_unord_storehash>> CuckooRefinableSet_list_unord_storehash;

        struct traits_CuckooSet_list_ord :
            public cc::cuckoo::make_traits <
                cc::cuckoo::probeset_type< cc::cuckoo::list >
                , co::compare< compare >
                , co::hash< std::tuple< hash, hash2 > >
            > ::type
        {};
        typedef CuckooSet< key_val, traits_CuckooStripedSet<traits_CuckooSet_list_ord>> CuckooStripedSet_list_ord;
        typedef CuckooSet< key_val, traits_CuckooRefinableSet<traits_CuckooSet_list_ord>> CuckooRefinableSet_list_ord;

        struct traits_CuckooSet_list_ord_stat : public traits_CuckooSet_list_ord
        {
            typedef cc::cuckoo::stat stat;
        };
        typedef CuckooSet< key_val, traits_CuckooStripedSet<traits_CuckooSet_list_ord_stat>> CuckooStripedSet_list_ord_stat;
        typedef CuckooSet< key_val, traits_CuckooRefinableSet<traits_CuckooSet_list_ord_stat>> CuckooRefinableSet_list_ord_stat;

        struct traits_CuckooSet_list_ord_storehash : public traits_CuckooSet_list_ord
        {
            static CDS_CONSTEXPR const bool store_hash = true;
        };
        typedef CuckooSet< key_val, traits_CuckooStripedSet<traits_CuckooSet_list_ord_storehash>> CuckooStripedSet_list_ord_storehash;
        typedef CuckooSet< key_val, traits_CuckooRefinableSet<traits_CuckooSet_list_ord_storehash>> CuckooRefinableSet_list_ord_storehash;


        struct traits_CuckooSet_vector_unord :
            public cc::cuckoo::make_traits <
                cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                , co::equal_to< equal_to >
                , co::hash< std::tuple< hash, hash2 > >
            > ::type
        {};
        typedef CuckooSet< key_val, traits_CuckooStripedSet<traits_CuckooSet_vector_unord>> CuckooStripedSet_vector_unord;
        typedef CuckooSet< key_val, traits_CuckooRefinableSet<traits_CuckooSet_vector_unord>> CuckooRefinableSet_vector_unord;

        struct traits_CuckooSet_vector_unord_stat : public traits_CuckooSet_vector_unord
        {
            typedef cc::cuckoo::stat stat;
        };
        typedef CuckooSet< key_val, traits_CuckooStripedSet<traits_CuckooSet_vector_unord_stat>> CuckooStripedSet_vector_unord_stat;
        typedef CuckooSet< key_val, traits_CuckooRefinableSet<traits_CuckooSet_vector_unord_stat>> CuckooRefinableSet_vector_unord_stat;

        struct traits_CuckooSet_vector_unord_storehash : public traits_CuckooSet_vector_unord
        {
            static CDS_CONSTEXPR const bool store_hash = true;
        };
        typedef CuckooSet< key_val, traits_CuckooStripedSet<traits_CuckooSet_vector_unord_storehash>> CuckooStripedSet_vector_unord_storehash;
        typedef CuckooSet< key_val, traits_CuckooRefinableSet<traits_CuckooSet_vector_unord_storehash>> CuckooRefinableSet_vector_unord_storehash;

        struct traits_CuckooSet_vector_ord :
            public cc::cuckoo::make_traits <
                cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
                , co::compare< compare >
                , co::hash< std::tuple< hash, hash2 > >
            > ::type
        {};
        typedef CuckooSet< key_val, traits_CuckooStripedSet<traits_CuckooSet_vector_ord>> CuckooStripedSet_vector_ord;
        typedef CuckooSet< key_val, traits_CuckooRefinableSet<traits_CuckooSet_vector_ord>> CuckooRefinableSet_vector_ord;

        struct traits_CuckooSet_vector_ord_stat : public traits_CuckooSet_vector_ord
        {
            typedef cc::cuckoo::stat stat;
        };
        typedef CuckooSet< key_val, traits_CuckooStripedSet<traits_CuckooSet_vector_ord_stat>> CuckooStripedSet_vector_ord_stat;
        typedef CuckooSet< key_val, traits_CuckooRefinableSet<traits_CuckooSet_vector_ord_stat>> CuckooRefinableSet_vector_ord_stat;

        struct traits_CuckooSet_vector_ord_storehash : public traits_CuckooSet_vector_ord
        {
            static CDS_CONSTEXPR const bool store_hash = true;
        };
        typedef CuckooSet< key_val, traits_CuckooStripedSet<traits_CuckooSet_vector_ord_storehash>> CuckooStripedSet_vector_ord_storehash;
        typedef CuckooSet< key_val, traits_CuckooRefinableSet<traits_CuckooSet_vector_ord_storehash>> CuckooRefinableSet_vector_ord_storehash;


        // ***************************************************************************
        // SkipListSet

        class traits_SkipListSet_less_pascal: public cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_less_pascal > SkipListSet_hp_less_pascal;
        typedef cc::SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_less_pascal > SkipListSet_dhp_less_pascal;
        typedef cc::SkipListSet< rcu_gpi, key_val, traits_SkipListSet_less_pascal > SkipListSet_rcu_gpi_less_pascal;
        typedef cc::SkipListSet< rcu_gpb, key_val, traits_SkipListSet_less_pascal > SkipListSet_rcu_gpb_less_pascal;
        typedef cc::SkipListSet< rcu_gpt, key_val, traits_SkipListSet_less_pascal > SkipListSet_rcu_gpt_less_pascal;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListSet< rcu_shb, key_val, traits_SkipListSet_less_pascal > SkipListSet_rcu_shb_less_pascal;
        typedef cc::SkipListSet< rcu_sht, key_val, traits_SkipListSet_less_pascal > SkipListSet_rcu_sht_less_pascal;
#endif

        class traits_SkipListSet_less_pascal_seqcst: public cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::memory_model< co::v::sequential_consistent >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_less_pascal_seqcst > SkipListSet_hp_less_pascal_seqcst;
        typedef cc::SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_less_pascal_seqcst > SkipListSet_dhp_less_pascal_seqcst;
        typedef cc::SkipListSet< rcu_gpi, key_val, traits_SkipListSet_less_pascal_seqcst > SkipListSet_rcu_gpi_less_pascal_seqcst;
        typedef cc::SkipListSet< rcu_gpb, key_val, traits_SkipListSet_less_pascal_seqcst > SkipListSet_rcu_gpb_less_pascal_seqcst;
        typedef cc::SkipListSet< rcu_gpt, key_val, traits_SkipListSet_less_pascal_seqcst > SkipListSet_rcu_gpt_less_pascal_seqcst;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListSet< rcu_shb, key_val, traits_SkipListSet_less_pascal_seqcst > SkipListSet_rcu_shb_less_pascal_seqcst;
        typedef cc::SkipListSet< rcu_sht, key_val, traits_SkipListSet_less_pascal_seqcst > SkipListSet_rcu_sht_less_pascal_seqcst;
#endif

        class traits_SkipListSet_less_pascal_stat: public cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::stat< cc::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_less_pascal_stat > SkipListSet_hp_less_pascal_stat;
        typedef cc::SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_less_pascal_stat > SkipListSet_dhp_less_pascal_stat;
        typedef cc::SkipListSet< rcu_gpi, key_val, traits_SkipListSet_less_pascal_stat > SkipListSet_rcu_gpi_less_pascal_stat;
        typedef cc::SkipListSet< rcu_gpb, key_val, traits_SkipListSet_less_pascal_stat > SkipListSet_rcu_gpb_less_pascal_stat;
        typedef cc::SkipListSet< rcu_gpt, key_val, traits_SkipListSet_less_pascal_stat > SkipListSet_rcu_gpt_less_pascal_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListSet< rcu_shb, key_val, traits_SkipListSet_less_pascal_stat > SkipListSet_rcu_shb_less_pascal_stat;
        typedef cc::SkipListSet< rcu_sht, key_val, traits_SkipListSet_less_pascal_stat > SkipListSet_rcu_sht_less_pascal_stat;
#endif

        class traits_SkipListSet_cmp_pascal: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_cmp_pascal > SkipListSet_hp_cmp_pascal;
        typedef cc::SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_cmp_pascal > SkipListSet_dhp_cmp_pascal;
        typedef cc::SkipListSet< rcu_gpi, key_val, traits_SkipListSet_cmp_pascal > SkipListSet_rcu_gpi_cmp_pascal;
        typedef cc::SkipListSet< rcu_gpb, key_val, traits_SkipListSet_cmp_pascal > SkipListSet_rcu_gpb_cmp_pascal;
        typedef cc::SkipListSet< rcu_gpt, key_val, traits_SkipListSet_cmp_pascal > SkipListSet_rcu_gpt_cmp_pascal;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListSet< rcu_shb, key_val, traits_SkipListSet_cmp_pascal > SkipListSet_rcu_shb_cmp_pascal;
        typedef cc::SkipListSet< rcu_sht, key_val, traits_SkipListSet_cmp_pascal > SkipListSet_rcu_sht_cmp_pascal;
#endif

        class traits_SkipListSet_cmp_pascal_stat: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            ,co::stat< cc::skip_list::stat<> >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_cmp_pascal_stat > SkipListSet_hp_cmp_pascal_stat;
        typedef cc::SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_cmp_pascal_stat > SkipListSet_dhp_cmp_pascal_stat;
        typedef cc::SkipListSet< rcu_gpi, key_val, traits_SkipListSet_cmp_pascal_stat > SkipListSet_rcu_gpi_cmp_pascal_stat;
        typedef cc::SkipListSet< rcu_gpb, key_val, traits_SkipListSet_cmp_pascal_stat > SkipListSet_rcu_gpb_cmp_pascal_stat;
        typedef cc::SkipListSet< rcu_gpt, key_val, traits_SkipListSet_cmp_pascal_stat > SkipListSet_rcu_gpt_cmp_pascal_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListSet< rcu_shb, key_val, traits_SkipListSet_cmp_pascal_stat > SkipListSet_rcu_shb_cmp_pascal_stat;
        typedef cc::SkipListSet< rcu_sht, key_val, traits_SkipListSet_cmp_pascal_stat > SkipListSet_rcu_sht_cmp_pascal_stat;
#endif

        class traits_SkipListSet_less_xorshift: public cc::skip_list::make_traits <
            co::less< less >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_less_xorshift > SkipListSet_hp_less_xorshift;
        typedef cc::SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_less_xorshift > SkipListSet_dhp_less_xorshift;
        typedef cc::SkipListSet< rcu_gpi, key_val, traits_SkipListSet_less_xorshift > SkipListSet_rcu_gpi_less_xorshift;
        typedef cc::SkipListSet< rcu_gpb, key_val, traits_SkipListSet_less_xorshift > SkipListSet_rcu_gpb_less_xorshift;
        typedef cc::SkipListSet< rcu_gpt, key_val, traits_SkipListSet_less_xorshift > SkipListSet_rcu_gpt_less_xorshift;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListSet< rcu_shb, key_val, traits_SkipListSet_less_xorshift > SkipListSet_rcu_shb_less_xorshift;
        typedef cc::SkipListSet< rcu_sht, key_val, traits_SkipListSet_less_xorshift > SkipListSet_rcu_sht_less_xorshift;
#endif

        class traits_SkipListSet_less_xorshift_stat: public cc::skip_list::make_traits <
            co::less< less >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            ,co::stat< cc::skip_list::stat<> >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_less_xorshift_stat > SkipListSet_hp_less_xorshift_stat;
        typedef cc::SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_less_xorshift_stat > SkipListSet_dhp_less_xorshift_stat;
        typedef cc::SkipListSet< rcu_gpi, key_val, traits_SkipListSet_less_xorshift_stat > SkipListSet_rcu_gpi_less_xorshift_stat;
        typedef cc::SkipListSet< rcu_gpb, key_val, traits_SkipListSet_less_xorshift_stat > SkipListSet_rcu_gpb_less_xorshift_stat;
        typedef cc::SkipListSet< rcu_gpt, key_val, traits_SkipListSet_less_xorshift_stat > SkipListSet_rcu_gpt_less_xorshift_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListSet< rcu_shb, key_val, traits_SkipListSet_less_xorshift_stat > SkipListSet_rcu_shb_less_xorshift_stat;
        typedef cc::SkipListSet< rcu_sht, key_val, traits_SkipListSet_less_xorshift_stat > SkipListSet_rcu_sht_less_xorshift_stat;
#endif

        class traits_SkipListSet_cmp_xorshift: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_cmp_xorshift > SkipListSet_hp_cmp_xorshift;
        typedef cc::SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_cmp_xorshift > SkipListSet_dhp_cmp_xorshift;
        typedef cc::SkipListSet< rcu_gpi, key_val, traits_SkipListSet_cmp_xorshift > SkipListSet_rcu_gpi_cmp_xorshift;
        typedef cc::SkipListSet< rcu_gpb, key_val, traits_SkipListSet_cmp_xorshift > SkipListSet_rcu_gpb_cmp_xorshift;
        typedef cc::SkipListSet< rcu_gpt, key_val, traits_SkipListSet_cmp_xorshift > SkipListSet_rcu_gpt_cmp_xorshift;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListSet< rcu_shb, key_val, traits_SkipListSet_cmp_xorshift > SkipListSet_rcu_shb_cmp_xorshift;
        typedef cc::SkipListSet< rcu_sht, key_val, traits_SkipListSet_cmp_xorshift > SkipListSet_rcu_sht_cmp_xorshift;
#endif

        class traits_SkipListSet_cmp_xorshift_stat: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            ,co::stat< cc::skip_list::stat<> >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_cmp_xorshift_stat > SkipListSet_hp_cmp_xorshift_stat;
        typedef cc::SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_cmp_xorshift_stat > SkipListSet_dhp_cmp_xorshift_stat;
        typedef cc::SkipListSet< rcu_gpi, key_val, traits_SkipListSet_cmp_xorshift_stat > SkipListSet_rcu_gpi_cmp_xorshift_stat;
        typedef cc::SkipListSet< rcu_gpb, key_val, traits_SkipListSet_cmp_xorshift_stat > SkipListSet_rcu_gpb_cmp_xorshift_stat;
        typedef cc::SkipListSet< rcu_gpt, key_val, traits_SkipListSet_cmp_xorshift_stat > SkipListSet_rcu_gpt_cmp_xorshift_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SkipListSet< rcu_shb, key_val, traits_SkipListSet_cmp_xorshift_stat > SkipListSet_rcu_shb_cmp_xorshift_stat;
        typedef cc::SkipListSet< rcu_sht, key_val, traits_SkipListSet_cmp_xorshift_stat > SkipListSet_rcu_sht_cmp_xorshift_stat;
#endif


        // ***************************************************************************
        // EllenBinTreeSet
        struct ellen_bintree_props {
            struct key_extractor {
                void operator()( key_type& dest, key_val const& src ) const
                {
                    dest = src.key;
                }
            };

            struct less {
                bool operator()( key_val const& v1, key_val const& v2 ) const
                {
                    return key_less()( v1.key, v2.key );
                }
                bool operator()( key_type const& k, key_val const& v ) const
                {
                    return key_less()( k, v.key );
                }
                bool operator()( key_val const& v, key_type const& k ) const
                {
                    return key_less()( v.key, k );
                }
                bool operator()( key_type const& k1, key_type const& k2 ) const
                {
                    return key_less()( k1, k2 );
                }
            };

            struct hp_gc {
                typedef cc::ellen_bintree::node<cds::gc::HP, key_val>               leaf_node;
                typedef cc::ellen_bintree::internal_node< key_type, leaf_node >     internal_node;
                typedef cc::ellen_bintree::update_desc< leaf_node, internal_node >  update_desc;
            };

            struct dhp_gc {
                typedef cc::ellen_bintree::node<cds::gc::DHP, key_val>              leaf_node;
                typedef cc::ellen_bintree::internal_node< key_type, leaf_node >     internal_node;
                typedef cc::ellen_bintree::update_desc< leaf_node, internal_node >  update_desc;
            };

            struct gpi {
                typedef cc::ellen_bintree::node<rcu_gpi, key_val>                   leaf_node;
                typedef cc::ellen_bintree::internal_node< key_type, leaf_node >     internal_node;
                typedef cc::ellen_bintree::update_desc< leaf_node, internal_node >  update_desc;
            };
            struct gpb {
                typedef cc::ellen_bintree::node<rcu_gpb, key_val>                   leaf_node;
                typedef cc::ellen_bintree::internal_node< key_type, leaf_node >     internal_node;
                typedef cc::ellen_bintree::update_desc< leaf_node, internal_node >  update_desc;
            };
            struct gpt {
                typedef cc::ellen_bintree::node<rcu_gpt, key_val>                   leaf_node;
                typedef cc::ellen_bintree::internal_node< key_type, leaf_node >     internal_node;
                typedef cc::ellen_bintree::update_desc< leaf_node, internal_node >  update_desc;
            };
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
            struct shb {
                typedef cc::ellen_bintree::node<rcu_shb, key_val>                   leaf_node;
                typedef cc::ellen_bintree::internal_node< key_type, leaf_node >     internal_node;
                typedef cc::ellen_bintree::update_desc< leaf_node, internal_node >  update_desc;
            };
            struct sht {
                typedef cc::ellen_bintree::node<rcu_sht, key_val>                   leaf_node;
                typedef cc::ellen_bintree::internal_node< key_type, leaf_node >     internal_node;
                typedef cc::ellen_bintree::update_desc< leaf_node, internal_node >  update_desc;
            };
#endif
        };

        struct traits_EllenBinTreeSet: public cc::ellen_bintree::make_set_traits<
            cc::ellen_bintree::key_extractor< typename ellen_bintree_props::key_extractor >
            ,co::less< typename ellen_bintree_props::less >
            ,co::node_allocator< ellen_bintree_pool::internal_node_allocator< int > >
        >::type
        {};

        struct traits_EllenBinTreeSet_hp : public traits_EllenBinTreeSet
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::hp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< cds::gc::HP, key_type, key_val, traits_EllenBinTreeSet_hp > EllenBinTreeSet_hp;

        struct traits_EllenBinTreeSet_dhp : public traits_EllenBinTreeSet
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::dhp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< cds::gc::DHP, key_type, key_val, traits_EllenBinTreeSet_dhp > EllenBinTreeSet_dhp;

        struct traits_EllenBinTreeSet_gpi : public traits_EllenBinTreeSet
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpi::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< rcu_gpi, key_type, key_val, traits_EllenBinTreeSet_gpi > EllenBinTreeSet_rcu_gpi;

        struct traits_EllenBinTreeSet_gpb : public traits_EllenBinTreeSet
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< rcu_gpb, key_type, key_val, traits_EllenBinTreeSet_gpb > EllenBinTreeSet_rcu_gpb;

        struct traits_EllenBinTreeSet_gpt : public traits_EllenBinTreeSet
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpt::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< rcu_gpt, key_type, key_val, traits_EllenBinTreeSet_gpt > EllenBinTreeSet_rcu_gpt;

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits_EllenBinTreeSet_shb : public traits_EllenBinTreeSet
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::shb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< rcu_shb, key_type, key_val, traits_EllenBinTreeSet_shb > EllenBinTreeSet_rcu_shb;

        struct traits_EllenBinTreeSet_sht : public traits_EllenBinTreeSet
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::sht::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< rcu_sht, key_type, key_val, traits_EllenBinTreeSet_sht > EllenBinTreeSet_rcu_sht;
#endif

        //
        struct traits_EllenBinTreeSet_yield : public traits_EllenBinTreeSet
        {
            typedef cds::backoff::yield back_off;
        };

        struct traits_EllenBinTreeSet_yield_hp : public traits_EllenBinTreeSet_yield
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::hp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< cds::gc::HP, key_type, key_val, traits_EllenBinTreeSet_yield_hp > EllenBinTreeSet_yield_hp;

        struct traits_EllenBinTreeSet_yield_dhp : public traits_EllenBinTreeSet_yield
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::dhp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< cds::gc::DHP, key_type, key_val, traits_EllenBinTreeSet_yield_dhp > EllenBinTreeSet_yield_dhp;


        struct traits_EllenBinTreeSet_yield_gpb : public traits_EllenBinTreeSet_yield
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< rcu_gpb, key_type, key_val, traits_EllenBinTreeSet_yield_gpb > EllenBinTreeSet_yield_rcu_gpb;


        struct traits_EllenBinTreeSet_stat: public cc::ellen_bintree::make_set_traits<
            cc::ellen_bintree::key_extractor< typename ellen_bintree_props::key_extractor >
            ,co::less< typename ellen_bintree_props::less >
            ,co::node_allocator< ellen_bintree_pool::internal_node_allocator< int > >
            ,co::stat< cc::ellen_bintree::stat<> >
        >::type
        {};

        struct traits_EllenBinTreeSet_stat_hp : public traits_EllenBinTreeSet_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::hp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< cds::gc::HP, key_type, key_val, traits_EllenBinTreeSet_stat_hp > EllenBinTreeSet_hp_stat;

        struct traits_EllenBinTreeSet_stat_dhp : public traits_EllenBinTreeSet_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::dhp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< cds::gc::DHP, key_type, key_val, traits_EllenBinTreeSet_stat_dhp > EllenBinTreeSet_dhp_stat;

        struct traits_EllenBinTreeSet_stat_gpi : public traits_EllenBinTreeSet_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpi::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< rcu_gpi, key_type, key_val, traits_EllenBinTreeSet_stat_gpi > EllenBinTreeSet_rcu_gpi_stat;

        struct traits_EllenBinTreeSet_stat_gpb : public traits_EllenBinTreeSet_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< rcu_gpb, key_type, key_val, traits_EllenBinTreeSet_stat_gpb > EllenBinTreeSet_rcu_gpb_stat;

        struct traits_EllenBinTreeSet_stat_gpt : public traits_EllenBinTreeSet_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::gpt::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< rcu_gpt, key_type, key_val, traits_EllenBinTreeSet_stat_gpt > EllenBinTreeSet_rcu_gpt_stat;

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        struct traits_EllenBinTreeSet_stat_shb : public traits_EllenBinTreeSet_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::shb::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< rcu_shb, key_type, key_val, traits_EllenBinTreeSet_stat_shb > EllenBinTreeSet_rcu_shb_stat;

        struct traits_EllenBinTreeSet_stat_sht : public traits_EllenBinTreeSet_stat
        {
            typedef cds::memory::pool_allocator< typename ellen_bintree_props::sht::update_desc, ellen_bintree_pool::update_desc_pool_accessor > update_desc_allocator;
        };
        typedef cc::EllenBinTreeSet< rcu_sht, key_type, key_val, traits_EllenBinTreeSet_stat_sht > EllenBinTreeSet_rcu_sht_stat;
#endif

        // ***************************************************************************
        // Standard implementations

        typedef StdSet< key_val, less, cds::SpinLock >                  StdSet_Spin;
        typedef StdSet< key_val, less, lock::NoLock>                    StdSet_NoLock;

        typedef StdHashSet< key_val, hash, less, equal_to, cds::SpinLock >    StdHashSet_Spin;
        typedef StdHashSet< key_val, hash, less, equal_to, lock::NoLock >     StdHashSet_NoLock;

    };


    // *************************************************
    // print_stat
    // *************************************************

    template <typename Set>
    static inline void print_stat( Set const& /*s*/ )
    {}

    template <typename GC, typename T, typename Traits>
    static inline void print_stat( cc::SplitListSet<GC, T, Traits> const& s )
    {
        CPPUNIT_MSG( s.statistics() );
    }

    template <typename GC, typename T, typename Traits>
    static inline void print_stat( cc::SkipListSet<GC, T, Traits> const& s )
    {
        CPPUNIT_MSG( s.statistics() );
    }

    template <typename GC, typename Key, typename T, typename Traits>
    static inline void print_stat( cc::EllenBinTreeSet<GC, Key, T, Traits> const& s )
    {
        CPPUNIT_MSG( s.statistics() );
    }

    template <typename T, typename Traits >
    static inline void print_stat( cc::CuckooSet< T, Traits > const& s )
    {
        CPPUNIT_MSG( s.statistics() << s.mutex_policy_statistics() );
    }

    template <typename V, typename Traits>
    static inline void print_stat( CuckooSet< V, Traits > const& s )
    {
        typedef CuckooSet< V, Traits > set_type;
        print_stat( static_cast<typename set_type::cuckoo_base_class const&>(s) );
    }


    //*******************************************************
    // additional_check
    //*******************************************************

    template <typename Set>
    static inline void additional_check( Set& /*set*/ )
    {}

    template <typename Set>
    static inline void additional_cleanup( Set& /*set*/ )
    {}

    namespace ellen_bintree_check {
        static inline void check_stat( cds::intrusive::ellen_bintree::empty_stat const& /*s*/ )
        {
            // Not true for threaded RCU
            /*
            CPPUNIT_CHECK_CURRENT_EX( ellen_bintree_pool::internal_node_counter::m_nAlloc.get() == ellen_bintree_pool::internal_node_counter::m_nFree.get(),
                "m_nAlloc=" << ellen_bintree_pool::internal_node_counter::m_nAlloc.get()
                << ", m_nFree=" << ellen_bintree_pool::internal_node_counter::m_nFree.get()
                );
            */
        }

        static inline void check_stat( cds::intrusive::ellen_bintree::stat<> const& stat )
        {
            CPPUNIT_CHECK_CURRENT( stat.m_nInternalNodeCreated == stat.m_nInternalNodeDeleted );
            CPPUNIT_CHECK_CURRENT( stat.m_nUpdateDescCreated == stat.m_nUpdateDescDeleted );
            //CPPUNIT_CHECK_CURRENT( ellen_bintree_pool::internal_node_counter::m_nAlloc.get() == ellen_bintree_pool::internal_node_counter::m_nFree.get() );
            CPPUNIT_CHECK_CURRENT( ellen_bintree_pool::internal_node_counter::m_nAlloc.get() == stat.m_nInternalNodeCreated );
            // true if RCU is not threaded
            //CPPUNIT_CHECK_CURRENT( stat.m_nInternalNodeDeleted == ellen_bintree_pool::internal_node_counter::m_nFree.get() );
        }
    }   // namespace ellen_bintree_check

    template <typename GC, typename Key, typename T, typename Traits>
    static inline void additional_check( cc::EllenBinTreeSet<GC, Key, T, Traits>& s )
    {
        GC::force_dispose();
        ellen_bintree_check::check_stat( s.statistics() );
    }

    template <typename GC, typename Key, typename T, typename Traits>
    static inline void additional_cleanup( cc::EllenBinTreeSet<GC, Key, T, Traits>& /*s*/ )
    {
        ellen_bintree_pool::internal_node_counter::reset();
    }

}   // namespace set2

#endif // ifndef _CDSUNIT_SET2_SET_TYPES_H
