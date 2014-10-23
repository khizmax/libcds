//$$CDS-header$$

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
#include <cds/container/ellen_bintree_set_ptb.h>

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

    template <typename V, typename... Options>
    class CuckooStripedSet:
        public cc::CuckooSet< V,
            typename cc::cuckoo::make_traits<
                co::mutex_policy< cc::cuckoo::striping<> >
                ,Options...
            >::type
        >
    {
    public:
        typedef typename cc::cuckoo::make_traits<
            co::mutex_policy< cc::cuckoo::striping<> >
            ,Options...
        >::type cuckoo_traits;

        typedef cc::CuckooSet< V, cuckoo_traits > cuckoo_base_class;

    public:
        CuckooStripedSet( size_t nCapacity, size_t nLoadFactor )
            : cuckoo_base_class( nCapacity / (nLoadFactor * 16), (unsigned int) 4 )
        {}

        template <typename Q, typename Pred>
        bool erase_with( Q const& key, Pred pred )
        {
            return cuckoo_base_class::erase_with( key, typename std::conditional< cuckoo_base_class::c_isSorted, Pred, typename Pred::equal_to>::type() );
        }
    };

    template <typename V, typename... Options>
    class CuckooRefinableSet:
        public cc::CuckooSet< V,
            typename cc::cuckoo::make_traits<
                co::mutex_policy< cc::cuckoo::refinable<> >
                ,Options...
            >::type
        >
    {
    public:
        typedef typename cc::cuckoo::make_traits<
            co::mutex_policy< cc::cuckoo::refinable<> >
            ,Options...
        >::type cuckoo_traits;

        typedef cc::CuckooSet< V, cuckoo_traits > cuckoo_base_class;

    public:
        CuckooRefinableSet( size_t nCapacity, size_t nLoadFactor )
            : cuckoo_base_class( nCapacity / (nLoadFactor * 16), (unsigned int) 4 )
        {}

        template <typename Q, typename Pred>
        bool erase_with( Q const& key, Pred pred )
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

#if (CDS_COMPILER == CDS_COMPILER_MSVC || (CDS_COMPILER == CDS_COMPILER_INTEL && CDS_OS_INTERFACE == CDS_OSI_WINDOWS)) && _MSC_VER < 1600
        struct hash_less: public stdext::hash_compare< key_type, std::less<key_type> >
        {
            typedef stdext::hash_compare< key_type, std::less<key_type> > base_class;
            size_t operator()(const key_val& kv) const
            {
                return hash()(kv);
            }
            size_t operator()(const key_type& k ) const
            {
                return hash()(k);
            }

            bool operator()(const key_val& kv1, const key_val& kv2) const
            {
                return less()( kv1, kv2 );
            }
            bool operator()(const key_type& k1, const key_val& kv2) const
            {
                return less()( k1, kv2 );
            }
            bool operator()(const key_val& kv1, const key_type& k2) const
            {
                return less()( kv1, k2 );
            }
        };
#endif

        // ***************************************************************************
        // MichaelList

        typedef cc::MichaelList< cds::gc::HP, key_val,
            typename cc::michael_list::make_traits<
                co::compare< compare >
            >::type
        >   MichaelList_HP_cmp_stdAlloc;

        typedef cc::MichaelList< cds::gc::HP, key_val,
            typename cc::michael_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_HP_cmp_stdAlloc_seqcst;

        typedef cc::MichaelList< cds::gc::HP, key_val,
            typename cc::michael_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_HP_cmp_michaelAlloc;

        typedef cc::MichaelList< cds::gc::HP, key_val,
            typename cc::michael_list::make_traits<
                co::less< less >
            >::type
        >   MichaelList_HP_less_stdAlloc;

        typedef cc::MichaelList< cds::gc::HP, key_val,
            typename cc::michael_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_HP_less_stdAlloc_seqcst;

        typedef cc::MichaelList< cds::gc::HP, key_val,
            typename cc::michael_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_HP_less_michaelAlloc;

        typedef cc::MichaelList< cds::gc::DHP, key_val,
            typename cc::michael_list::make_traits<
                co::compare< compare >
            >::type
        >   MichaelList_DHP_cmp_stdAlloc;

        typedef cc::MichaelList< cds::gc::DHP, key_val,
            typename cc::michael_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_DHP_cmp_stdAlloc_seqcst;

        typedef cc::MichaelList< cds::gc::DHP, key_val,
            typename cc::michael_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_DHP_cmp_michaelAlloc;

        typedef cc::MichaelList< cds::gc::DHP, key_val,
            typename cc::michael_list::make_traits<
                co::less< less >
            >::type
        >   MichaelList_DHP_less_stdAlloc;

        typedef cc::MichaelList< cds::gc::DHP, key_val,
            typename cc::michael_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_DHP_less_stdAlloc_seqcst;

        typedef cc::MichaelList< cds::gc::DHP, key_val,
            typename cc::michael_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_DHP_less_michaelAlloc;


        typedef cc::MichaelList< rcu_gpi, key_val,
            typename cc::michael_list::make_traits<
                co::compare< compare >
            >::type
        >   MichaelList_RCU_GPI_cmp_stdAlloc;

        typedef cc::MichaelList< rcu_gpi, key_val,
            typename cc::michael_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_RCU_GPI_cmp_stdAlloc_seqcst;

        typedef cc::MichaelList< rcu_gpi, key_val,
            typename cc::michael_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_RCU_GPI_cmp_michaelAlloc;

        typedef cc::MichaelList< rcu_gpi, key_val,
            typename cc::michael_list::make_traits<
                co::less< less >
            >::type
        >   MichaelList_RCU_GPI_less_stdAlloc;

        typedef cc::MichaelList< rcu_gpi, key_val,
            typename cc::michael_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_RCU_GPI_less_stdAlloc_seqcst;

        typedef cc::MichaelList< rcu_gpi, key_val,
            typename cc::michael_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_RCU_GPI_less_michaelAlloc;


        typedef cc::MichaelList< rcu_gpb, key_val,
            typename cc::michael_list::make_traits<
                co::compare< compare >
            >::type
        >   MichaelList_RCU_GPB_cmp_stdAlloc;

        typedef cc::MichaelList< rcu_gpb, key_val,
            typename cc::michael_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_RCU_GPB_cmp_stdAlloc_seqcst;

        typedef cc::MichaelList< rcu_gpb, key_val,
            typename cc::michael_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_RCU_GPB_cmp_michaelAlloc;

        typedef cc::MichaelList< rcu_gpb, key_val,
            typename cc::michael_list::make_traits<
                co::less< less >
            >::type
        >   MichaelList_RCU_GPB_less_stdAlloc;

        typedef cc::MichaelList< rcu_gpb, key_val,
            typename cc::michael_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_RCU_GPB_less_stdAlloc_seqcst;

        typedef cc::MichaelList< rcu_gpb, key_val,
            typename cc::michael_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_RCU_GPB_less_michaelAlloc;


        typedef cc::MichaelList< rcu_gpt, key_val,
            typename cc::michael_list::make_traits<
                co::compare< compare >
            >::type
        >   MichaelList_RCU_GPT_cmp_stdAlloc;

        typedef cc::MichaelList< rcu_gpt, key_val,
            typename cc::michael_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_RCU_GPT_cmp_stdAlloc_seqcst;

        typedef cc::MichaelList< rcu_gpt, key_val,
            typename cc::michael_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_RCU_GPT_cmp_michaelAlloc;

        typedef cc::MichaelList< rcu_gpt, key_val,
            typename cc::michael_list::make_traits<
                co::less< less >
            >::type
        >   MichaelList_RCU_GPT_less_stdAlloc;

        typedef cc::MichaelList< rcu_gpt, key_val,
            typename cc::michael_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_RCU_GPT_less_stdAlloc_seqcst;

        typedef cc::MichaelList< rcu_gpt, key_val,
            typename cc::michael_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_RCU_GPT_less_michaelAlloc;

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelList< rcu_shb, key_val,
            typename cc::michael_list::make_traits<
                co::compare< compare >
            >::type
        >   MichaelList_RCU_SHB_cmp_stdAlloc;

        typedef cc::MichaelList< rcu_shb, key_val,
            typename cc::michael_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_RCU_SHB_cmp_stdAlloc_seqcst;

        typedef cc::MichaelList< rcu_shb, key_val,
            typename cc::michael_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_RCU_SHB_cmp_michaelAlloc;

        typedef cc::MichaelList< rcu_shb, key_val,
            typename cc::michael_list::make_traits<
                co::less< less >
            >::type
        >   MichaelList_RCU_SHB_less_stdAlloc;

        typedef cc::MichaelList< rcu_shb, key_val,
            typename cc::michael_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_RCU_SHB_less_stdAlloc_seqcst;

        typedef cc::MichaelList< rcu_shb, key_val,
            typename cc::michael_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_RCU_SHB_less_michaelAlloc;


        typedef cc::MichaelList< rcu_sht, key_val,
            typename cc::michael_list::make_traits<
                co::compare< compare >
            >::type
        >   MichaelList_RCU_SHT_cmp_stdAlloc;

        typedef cc::MichaelList< rcu_sht, key_val,
            typename cc::michael_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_RCU_SHT_cmp_stdAlloc_seqcst;

        typedef cc::MichaelList< rcu_sht, key_val,
            typename cc::michael_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_RCU_SHT_cmp_michaelAlloc;

        typedef cc::MichaelList< rcu_sht, key_val,
            typename cc::michael_list::make_traits<
                co::less< less >
            >::type
        >   MichaelList_RCU_SHT_less_stdAlloc;

        typedef cc::MichaelList< rcu_sht, key_val,
            typename cc::michael_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   MichaelList_RCU_SHT_less_stdAlloc_seqcst;

        typedef cc::MichaelList< rcu_sht, key_val,
            typename cc::michael_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelList_RCU_SHT_less_michaelAlloc;
#endif

        // ***************************************************************************
        // MichaelHashSet based on MichaelList

        typedef cc::MichaelHashSet< cds::gc::HP, MichaelList_HP_cmp_stdAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_HP_cmp_stdAlloc;

        typedef cc::MichaelHashSet< cds::gc::HP, MichaelList_HP_cmp_stdAlloc_seqcst,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_HP_cmp_stdAlloc_seqcst;

        typedef cc::MichaelHashSet< cds::gc::HP, MichaelList_HP_cmp_michaelAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelSet_HP_cmp_michaelAlloc;

        typedef cc::MichaelHashSet< cds::gc::HP, MichaelList_HP_less_stdAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_HP_less_stdAlloc;

        typedef cc::MichaelHashSet< cds::gc::HP, MichaelList_HP_less_stdAlloc_seqcst,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_HP_less_stdAlloc_seqcst;

        typedef cc::MichaelHashSet< cds::gc::HP, MichaelList_HP_less_michaelAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelSet_HP_less_michaelAlloc;

        typedef cc::MichaelHashSet< cds::gc::DHP, MichaelList_DHP_cmp_stdAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_DHP_cmp_stdAlloc;

        typedef cc::MichaelHashSet< cds::gc::DHP, MichaelList_DHP_cmp_stdAlloc_seqcst,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_DHP_cmp_stdAlloc_seqcst;

        typedef cc::MichaelHashSet< cds::gc::DHP, MichaelList_DHP_cmp_michaelAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelSet_DHP_cmp_michaelAlloc;

        typedef cc::MichaelHashSet< cds::gc::DHP, MichaelList_DHP_less_stdAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_DHP_less_stdAlloc;

        typedef cc::MichaelHashSet< cds::gc::DHP, MichaelList_DHP_less_stdAlloc_seqcst,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_DHP_less_stdAlloc_seqcst;

        typedef cc::MichaelHashSet< cds::gc::DHP, MichaelList_DHP_less_michaelAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelSet_DHP_less_michaelAlloc;

        // general_instant RCU
        typedef cc::MichaelHashSet< rcu_gpi, MichaelList_RCU_GPI_cmp_stdAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_RCU_GPI_cmp_stdAlloc;

        typedef cc::MichaelHashSet< rcu_gpi, MichaelList_RCU_GPI_cmp_stdAlloc_seqcst,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_RCU_GPI_cmp_stdAlloc_seqcst;

        typedef cc::MichaelHashSet< rcu_gpi, MichaelList_RCU_GPI_cmp_michaelAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelSet_RCU_GPI_cmp_michaelAlloc;

        typedef cc::MichaelHashSet< rcu_gpi, MichaelList_RCU_GPI_less_stdAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_RCU_GPI_less_stdAlloc;

        typedef cc::MichaelHashSet< rcu_gpi, MichaelList_RCU_GPI_less_stdAlloc_seqcst,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_RCU_GPI_less_stdAlloc_seqcst;

        typedef cc::MichaelHashSet< rcu_gpi, MichaelList_RCU_GPI_less_michaelAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelSet_RCU_GPI_less_michaelAlloc;

        // general_buffered RCU
        typedef cc::MichaelHashSet< rcu_gpb, MichaelList_RCU_GPB_cmp_stdAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_RCU_GPB_cmp_stdAlloc;

        typedef cc::MichaelHashSet< rcu_gpb, MichaelList_RCU_GPB_cmp_stdAlloc_seqcst,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_RCU_GPB_cmp_stdAlloc_seqcst;

        typedef cc::MichaelHashSet< rcu_gpb, MichaelList_RCU_GPB_cmp_michaelAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelSet_RCU_GPB_cmp_michaelAlloc;

        typedef cc::MichaelHashSet< rcu_gpb, MichaelList_RCU_GPB_less_stdAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_RCU_GPB_less_stdAlloc;

        typedef cc::MichaelHashSet< rcu_gpb, MichaelList_RCU_GPB_less_stdAlloc_seqcst,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_RCU_GPB_less_stdAlloc_seqcst;

        typedef cc::MichaelHashSet< rcu_gpb, MichaelList_RCU_GPB_less_michaelAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelSet_RCU_GPB_less_michaelAlloc;

        // general_threaded RCU
        typedef cc::MichaelHashSet< rcu_gpt, MichaelList_RCU_GPT_cmp_stdAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_RCU_GPT_cmp_stdAlloc;

        typedef cc::MichaelHashSet< rcu_gpt, MichaelList_RCU_GPT_cmp_stdAlloc_seqcst,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_RCU_GPT_cmp_stdAlloc_seqcst;

        typedef cc::MichaelHashSet< rcu_gpt, MichaelList_RCU_GPT_cmp_michaelAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelSet_RCU_GPT_cmp_michaelAlloc;

        typedef cc::MichaelHashSet< rcu_gpt, MichaelList_RCU_GPT_less_stdAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_RCU_GPT_less_stdAlloc;

        typedef cc::MichaelHashSet< rcu_gpt, MichaelList_RCU_GPT_less_stdAlloc_seqcst,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_RCU_GPT_less_stdAlloc_seqcst;

        typedef cc::MichaelHashSet< rcu_gpt, MichaelList_RCU_GPT_less_michaelAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelSet_RCU_GPT_less_michaelAlloc;

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        // signal_buffered RCU
        typedef cc::MichaelHashSet< rcu_shb, MichaelList_RCU_SHB_cmp_stdAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_RCU_SHB_cmp_stdAlloc;

        typedef cc::MichaelHashSet< rcu_shb, MichaelList_RCU_SHB_cmp_stdAlloc_seqcst,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_RCU_SHB_cmp_stdAlloc_seqcst;

        typedef cc::MichaelHashSet< rcu_shb, MichaelList_RCU_SHB_cmp_michaelAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelSet_RCU_SHB_cmp_michaelAlloc;

        typedef cc::MichaelHashSet< rcu_shb, MichaelList_RCU_SHB_less_stdAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_RCU_SHB_less_stdAlloc;

        typedef cc::MichaelHashSet< rcu_shb, MichaelList_RCU_SHB_less_stdAlloc_seqcst,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_RCU_SHB_less_stdAlloc_seqcst;

        typedef cc::MichaelHashSet< rcu_shb, MichaelList_RCU_SHB_less_michaelAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelSet_RCU_SHB_less_michaelAlloc;

        // signal_threaded RCU
        typedef cc::MichaelHashSet< rcu_sht, MichaelList_RCU_SHT_cmp_stdAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_RCU_SHT_cmp_stdAlloc;

        typedef cc::MichaelHashSet< rcu_sht, MichaelList_RCU_SHT_cmp_stdAlloc_seqcst,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_RCU_SHT_cmp_stdAlloc_seqcst;

        typedef cc::MichaelHashSet< rcu_sht, MichaelList_RCU_SHT_cmp_michaelAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelSet_RCU_SHT_cmp_michaelAlloc;

        typedef cc::MichaelHashSet< rcu_sht, MichaelList_RCU_SHT_less_stdAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_RCU_SHT_less_stdAlloc;

        typedef cc::MichaelHashSet< rcu_sht, MichaelList_RCU_SHT_less_stdAlloc_seqcst,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_RCU_SHT_less_stdAlloc_seqcst;

        typedef cc::MichaelHashSet< rcu_sht, MichaelList_RCU_SHT_less_michaelAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelSet_RCU_SHT_less_michaelAlloc;

#endif

        // ***************************************************************************
        // LazyList

        typedef cc::LazyList< cds::gc::HP, key_val,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
            >::type
        >   LazyList_HP_cmp_stdAlloc;

        typedef cc::LazyList< cds::gc::HP, key_val,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_HP_cmp_stdAlloc_seqcst;

        typedef cc::LazyList< cds::gc::HP, key_val,
            typename cc::lazy_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_HP_cmp_michaelAlloc;

        typedef cc::LazyList< cds::gc::HP, key_val,
            typename cc::lazy_list::make_traits<
                co::less< less >
            >::type
        >   LazyList_HP_less_stdAlloc;

        typedef cc::LazyList< cds::gc::HP, key_val,
            typename cc::lazy_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_HP_less_stdAlloc_seqcst;

        typedef cc::LazyList< cds::gc::HP, key_val,
            typename cc::lazy_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_HP_less_michaelAlloc;

        typedef cc::LazyList< cds::gc::DHP, key_val,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
            >::type
        >   LazyList_DHP_cmp_stdAlloc;

        typedef cc::LazyList< cds::gc::DHP, key_val,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_DHP_cmp_stdAlloc_seqcst;

        typedef cc::LazyList< cds::gc::DHP, key_val,
            typename cc::lazy_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_DHP_cmp_michaelAlloc;

        typedef cc::LazyList< cds::gc::DHP, key_val,
            typename cc::lazy_list::make_traits<
                co::less< less >
            >::type
        >   LazyList_DHP_less_stdAlloc;

        typedef cc::LazyList< cds::gc::DHP, key_val,
            typename cc::lazy_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_DHP_less_stdAlloc_seqcst;

        typedef cc::LazyList< cds::gc::DHP, key_val,
            typename cc::lazy_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_DHP_less_michaelAlloc;

        // RCU
        typedef cc::LazyList< rcu_gpi, key_val,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
            >::type
        >   LazyList_RCU_GPI_cmp_stdAlloc;

        typedef cc::LazyList< rcu_gpi, key_val,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_RCU_GPI_cmp_stdAlloc_seqcst;

        typedef cc::LazyList< rcu_gpi, key_val,
            typename cc::lazy_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_RCU_GPI_cmp_michaelAlloc;

        typedef cc::LazyList< rcu_gpi, key_val,
            typename cc::lazy_list::make_traits<
                co::less< less >
            >::type
        >   LazyList_RCU_GPI_less_stdAlloc;

        typedef cc::LazyList< rcu_gpi, key_val,
            typename cc::lazy_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_RCU_GPI_less_stdAlloc_seqcst;

        typedef cc::LazyList< rcu_gpi, key_val,
            typename cc::lazy_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_RCU_GPI_less_michaelAlloc;

        typedef cc::LazyList< rcu_gpb, key_val,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
            >::type
        >   LazyList_RCU_GPB_cmp_stdAlloc;

        typedef cc::LazyList< rcu_gpb, key_val,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_RCU_GPB_cmp_stdAlloc_seqcst;

        typedef cc::LazyList< rcu_gpb, key_val,
            typename cc::lazy_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_RCU_GPB_cmp_michaelAlloc;

        typedef cc::LazyList< rcu_gpb, key_val,
            typename cc::lazy_list::make_traits<
                co::less< less >
            >::type
        >   LazyList_RCU_GPB_less_stdAlloc;

        typedef cc::LazyList< rcu_gpb, key_val,
            typename cc::lazy_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_RCU_GPB_less_stdAlloc_seqcst;

        typedef cc::LazyList< rcu_gpb, key_val,
            typename cc::lazy_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_RCU_GPB_less_michaelAlloc;

        //
        typedef cc::LazyList< rcu_gpt, key_val,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
            >::type
        >   LazyList_RCU_GPT_cmp_stdAlloc;

        typedef cc::LazyList< rcu_gpt, key_val,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_RCU_GPT_cmp_stdAlloc_seqcst;

        typedef cc::LazyList< rcu_gpt, key_val,
            typename cc::lazy_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_RCU_GPT_cmp_michaelAlloc;

        typedef cc::LazyList< rcu_gpt, key_val,
            typename cc::lazy_list::make_traits<
                co::less< less >
            >::type
        >   LazyList_RCU_GPT_less_stdAlloc;

        typedef cc::LazyList< rcu_gpt, key_val,
            typename cc::lazy_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_RCU_GPT_less_stdAlloc_seqcst;

        typedef cc::LazyList< rcu_gpt, key_val,
            typename cc::lazy_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_RCU_GPT_less_michaelAlloc;

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::LazyList< rcu_shb, key_val,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
            >::type
        >   LazyList_RCU_SHB_cmp_stdAlloc;

        typedef cc::LazyList< rcu_shb, key_val,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_RCU_SHB_cmp_stdAlloc_seqcst;

        typedef cc::LazyList< rcu_shb, key_val,
            typename cc::lazy_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_RCU_SHB_cmp_michaelAlloc;

        typedef cc::LazyList< rcu_shb, key_val,
            typename cc::lazy_list::make_traits<
                co::less< less >
            >::type
        >   LazyList_RCU_SHB_less_stdAlloc;

        typedef cc::LazyList< rcu_shb, key_val,
            typename cc::lazy_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_RCU_SHB_less_stdAlloc_seqcst;

        typedef cc::LazyList< rcu_shb, key_val,
            typename cc::lazy_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_RCU_SHB_less_michaelAlloc;

        //
        typedef cc::LazyList< rcu_sht, key_val,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
            >::type
        >   LazyList_RCU_SHT_cmp_stdAlloc;

        typedef cc::LazyList< rcu_sht, key_val,
            typename cc::lazy_list::make_traits<
                co::compare< compare >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_RCU_SHT_cmp_stdAlloc_seqcst;

        typedef cc::LazyList< rcu_sht, key_val,
            typename cc::lazy_list::make_traits<
                co::compare< compare >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_RCU_SHT_cmp_michaelAlloc;

        typedef cc::LazyList< rcu_sht, key_val,
            typename cc::lazy_list::make_traits<
                co::less< less >
            >::type
        >   LazyList_RCU_SHT_less_stdAlloc;

        typedef cc::LazyList< rcu_sht, key_val,
            typename cc::lazy_list::make_traits<
                co::less< less >
                ,co::memory_model< co::v::sequential_consistent >
            >::type
        >   LazyList_RCU_SHT_less_stdAlloc_seqcst;

        typedef cc::LazyList< rcu_sht, key_val,
            typename cc::lazy_list::make_traits<
                co::less< less >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   LazyList_RCU_SHT_less_michaelAlloc;

#endif

        // ***************************************************************************
        // MichaelHashSet based on LazyList

        typedef cc::MichaelHashSet< cds::gc::HP, LazyList_HP_cmp_stdAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_Lazy_HP_cmp_stdAlloc;

        typedef cc::MichaelHashSet< cds::gc::HP, LazyList_HP_cmp_stdAlloc_seqcst,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_Lazy_HP_cmp_stdAlloc_seqcst;

        typedef cc::MichaelHashSet< cds::gc::HP, LazyList_HP_cmp_michaelAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelSet_Lazy_HP_cmp_michaelAlloc;

        typedef cc::MichaelHashSet< cds::gc::HP, LazyList_HP_less_stdAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_Lazy_HP_less_stdAlloc;

        typedef cc::MichaelHashSet< cds::gc::HP, LazyList_HP_less_stdAlloc_seqcst,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_Lazy_HP_less_stdAlloc_seqcst;

        typedef cc::MichaelHashSet< cds::gc::HP, LazyList_HP_less_michaelAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelSet_Lazy_HP_less_michaelAlloc;

        typedef cc::MichaelHashSet< cds::gc::DHP, LazyList_DHP_cmp_stdAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_Lazy_DHP_cmp_stdAlloc;

        typedef cc::MichaelHashSet< cds::gc::DHP, LazyList_DHP_cmp_stdAlloc_seqcst,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_Lazy_DHP_cmp_stdAlloc_seqcst;

        typedef cc::MichaelHashSet< cds::gc::DHP, LazyList_DHP_cmp_michaelAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelSet_Lazy_DHP_cmp_michaelAlloc;

        typedef cc::MichaelHashSet< cds::gc::DHP, LazyList_DHP_less_stdAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_Lazy_DHP_less_stdAlloc;

        typedef cc::MichaelHashSet< cds::gc::DHP, LazyList_DHP_less_stdAlloc_seqcst,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_Lazy_DHP_less_stdAlloc_seqcst;

        typedef cc::MichaelHashSet< cds::gc::DHP, LazyList_DHP_less_michaelAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelSet_Lazy_DHP_less_michaelAlloc;

        // RCU
        typedef cc::MichaelHashSet< rcu_gpi, LazyList_RCU_GPI_cmp_stdAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_Lazy_RCU_GPI_cmp_stdAlloc;

        typedef cc::MichaelHashSet< rcu_gpi, LazyList_RCU_GPI_cmp_stdAlloc_seqcst,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_Lazy_RCU_GPI_cmp_stdAlloc_seqcst;

        typedef cc::MichaelHashSet< rcu_gpi, LazyList_RCU_GPI_cmp_michaelAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelSet_Lazy_RCU_GPI_cmp_michaelAlloc;

        typedef cc::MichaelHashSet< rcu_gpi, LazyList_RCU_GPI_less_stdAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_Lazy_RCU_GPI_less_stdAlloc;

        typedef cc::MichaelHashSet< rcu_gpi, LazyList_RCU_GPI_less_stdAlloc_seqcst,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_Lazy_RCU_GPI_less_stdAlloc_seqcst;

        typedef cc::MichaelHashSet< rcu_gpi, LazyList_RCU_GPI_less_michaelAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelSet_Lazy_RCU_GPI_less_michaelAlloc;


        typedef cc::MichaelHashSet< rcu_gpb, LazyList_RCU_GPB_cmp_stdAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_Lazy_RCU_GPB_cmp_stdAlloc;

        typedef cc::MichaelHashSet< rcu_gpb, LazyList_RCU_GPB_cmp_stdAlloc_seqcst,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_Lazy_RCU_GPB_cmp_stdAlloc_seqcst;

        typedef cc::MichaelHashSet< rcu_gpb, LazyList_RCU_GPB_cmp_michaelAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelSet_Lazy_RCU_GPB_cmp_michaelAlloc;

        typedef cc::MichaelHashSet< rcu_gpb, LazyList_RCU_GPB_less_stdAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_Lazy_RCU_GPB_less_stdAlloc;

        typedef cc::MichaelHashSet< rcu_gpb, LazyList_RCU_GPB_less_stdAlloc_seqcst,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_Lazy_RCU_GPB_less_stdAlloc_seqcst;

        typedef cc::MichaelHashSet< rcu_gpb, LazyList_RCU_GPB_less_michaelAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelSet_Lazy_RCU_GPB_less_michaelAlloc;


        typedef cc::MichaelHashSet< rcu_gpt, LazyList_RCU_GPT_cmp_stdAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_Lazy_RCU_GPT_cmp_stdAlloc;

        typedef cc::MichaelHashSet< rcu_gpt, LazyList_RCU_GPT_cmp_stdAlloc_seqcst,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_Lazy_RCU_GPT_cmp_stdAlloc_seqcst;

        typedef cc::MichaelHashSet< rcu_gpt, LazyList_RCU_GPT_cmp_michaelAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelSet_Lazy_RCU_GPT_cmp_michaelAlloc;

        typedef cc::MichaelHashSet< rcu_gpt, LazyList_RCU_GPT_less_stdAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_Lazy_RCU_GPT_less_stdAlloc;

        typedef cc::MichaelHashSet< rcu_gpt, LazyList_RCU_GPT_less_stdAlloc_seqcst,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_Lazy_RCU_GPT_less_stdAlloc_seqcst;

        typedef cc::MichaelHashSet< rcu_gpt, LazyList_RCU_GPT_less_michaelAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelSet_Lazy_RCU_GPT_less_michaelAlloc;

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::MichaelHashSet< rcu_shb, LazyList_RCU_SHB_cmp_stdAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_Lazy_RCU_SHB_cmp_stdAlloc;

        typedef cc::MichaelHashSet< rcu_shb, LazyList_RCU_SHB_cmp_stdAlloc_seqcst,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_Lazy_RCU_SHB_cmp_stdAlloc_seqcst;

        typedef cc::MichaelHashSet< rcu_shb, LazyList_RCU_SHB_cmp_michaelAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelSet_Lazy_RCU_SHB_cmp_michaelAlloc;

        typedef cc::MichaelHashSet< rcu_shb, LazyList_RCU_SHB_less_stdAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_Lazy_RCU_SHB_less_stdAlloc;

        typedef cc::MichaelHashSet< rcu_shb, LazyList_RCU_SHB_less_stdAlloc_seqcst,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_Lazy_RCU_SHB_less_stdAlloc_seqcst;

        typedef cc::MichaelHashSet< rcu_shb, LazyList_RCU_SHB_less_michaelAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelSet_Lazy_RCU_SHB_less_michaelAlloc;


        typedef cc::MichaelHashSet< rcu_sht, LazyList_RCU_SHT_cmp_stdAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_Lazy_RCU_SHT_cmp_stdAlloc;

        typedef cc::MichaelHashSet< rcu_sht, LazyList_RCU_SHT_cmp_stdAlloc_seqcst,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_Lazy_RCU_SHT_cmp_stdAlloc_seqcst;

        typedef cc::MichaelHashSet< rcu_sht, LazyList_RCU_SHT_cmp_michaelAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelSet_Lazy_RCU_SHT_cmp_michaelAlloc;

        typedef cc::MichaelHashSet< rcu_sht, LazyList_RCU_SHT_less_stdAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_Lazy_RCU_SHT_less_stdAlloc;

        typedef cc::MichaelHashSet< rcu_sht, LazyList_RCU_SHT_less_stdAlloc_seqcst,
            typename cc::michael_set::make_traits<
                co::hash< hash >
            >::type
        >   MichaelSet_Lazy_RCU_SHT_less_stdAlloc_seqcst;

        typedef cc::MichaelHashSet< rcu_sht, LazyList_RCU_SHT_less_michaelAlloc,
            typename cc::michael_set::make_traits<
                co::hash< hash >,
                co::allocator< memory::MichaelAllocator<int> >
            >::type
        >   MichaelSet_Lazy_RCU_SHT_less_michaelAlloc;
#endif

        // ***************************************************************************
        // SplitListSet based on MichaelList

        // HP
        typedef cc::SplitListSet< cds::gc::HP, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_HP_dyn_cmp;

        typedef cc::SplitListSet< cds::gc::HP, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Michael_HP_dyn_cmp_seqcst;

        typedef cc::SplitListSet< cds::gc::HP, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_HP_st_cmp;

        typedef cc::SplitListSet< cds::gc::HP, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Michael_HP_st_cmp_seqcst;

        //HP + less
        typedef cc::SplitListSet< cds::gc::HP, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_HP_dyn_less;

        typedef cc::SplitListSet< cds::gc::HP, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Michael_HP_dyn_less_seqcst;

        typedef cc::SplitListSet< cds::gc::HP, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_HP_st_less;

        typedef cc::SplitListSet< cds::gc::HP, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Michael_HP_st_less_seqcst;

        // DHP
        typedef cc::SplitListSet< cds::gc::DHP, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_DHP_dyn_cmp;

        typedef cc::SplitListSet< cds::gc::DHP, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Michael_DHP_dyn_cmp_seqcst;

        typedef cc::SplitListSet< cds::gc::DHP, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_DHP_st_cmp;

        typedef cc::SplitListSet< cds::gc::DHP, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Michael_DHP_st_cmp_seqcst;

        // DHP + less
        typedef cc::SplitListSet< cds::gc::DHP, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_DHP_dyn_less;

        typedef cc::SplitListSet< cds::gc::DHP, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Michael_DHP_dyn_less_seqcst;

        typedef cc::SplitListSet< cds::gc::DHP, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_DHP_st_less;

        typedef cc::SplitListSet< cds::gc::DHP, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Michael_DHP_st_less_seqcst;

        // RCU
        typedef cc::SplitListSet< rcu_gpi, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPI_dyn_cmp;

        typedef cc::SplitListSet< rcu_gpi, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Michael_RCU_GPI_dyn_cmp_seqcst;

        typedef cc::SplitListSet< rcu_gpi, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPI_st_cmp;

        typedef cc::SplitListSet< rcu_gpi, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Michael_RCU_GPI_st_cmp_seqcst;

        // RCU_GPI + less
        typedef cc::SplitListSet< rcu_gpi, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPI_dyn_less;

        typedef cc::SplitListSet< rcu_gpi, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Michael_RCU_GPI_dyn_less_seqcst;

        typedef cc::SplitListSet< rcu_gpi, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPI_st_less;

        typedef cc::SplitListSet< rcu_gpi, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Michael_RCU_GPI_st_less_seqcst;


        //
        typedef cc::SplitListSet< rcu_gpb, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPB_dyn_cmp;

        typedef cc::SplitListSet< rcu_gpb, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Michael_RCU_GPB_dyn_cmp_seqcst;

        typedef cc::SplitListSet< rcu_gpb, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPB_st_cmp;

        typedef cc::SplitListSet< rcu_gpb, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Michael_RCU_GPB_st_cmp_seqcst;

        // RCU_GPB + less
        typedef cc::SplitListSet< rcu_gpb, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPB_dyn_less;

        typedef cc::SplitListSet< rcu_gpb, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Michael_RCU_GPB_dyn_less_seqcst;

        typedef cc::SplitListSet< rcu_gpb, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPB_st_less;

        typedef cc::SplitListSet< rcu_gpb, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Michael_RCU_GPB_st_less_seqcst;

        //
        typedef cc::SplitListSet< rcu_gpt, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPT_dyn_cmp;

        typedef cc::SplitListSet< rcu_gpt, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Michael_RCU_GPT_dyn_cmp_seqcst;

        typedef cc::SplitListSet< rcu_gpt, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPT_st_cmp;

        typedef cc::SplitListSet< rcu_gpt, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Michael_RCU_GPT_st_cmp_seqcst;

        // RCU_GPT + less
        typedef cc::SplitListSet< rcu_gpt, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPT_dyn_less;

        typedef cc::SplitListSet< rcu_gpt, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Michael_RCU_GPT_dyn_less_seqcst;

        typedef cc::SplitListSet< rcu_gpt, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_GPT_st_less;

        typedef cc::SplitListSet< rcu_gpt, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Michael_RCU_GPT_st_less_seqcst;

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_SHB_dyn_cmp;

        typedef cc::SplitListSet< rcu_shb, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Michael_RCU_SHB_dyn_cmp_seqcst;

        typedef cc::SplitListSet< rcu_shb, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_SHB_st_cmp;

        typedef cc::SplitListSet< rcu_shb, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Michael_RCU_SHB_st_cmp_seqcst;

        // RCU_SHB + less
        typedef cc::SplitListSet< rcu_shb, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_SHB_dyn_less;

        typedef cc::SplitListSet< rcu_shb, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Michael_RCU_SHB_dyn_less_seqcst;

        typedef cc::SplitListSet< rcu_shb, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_SHB_st_less;

        typedef cc::SplitListSet< rcu_shb, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Michael_RCU_SHB_st_less_seqcst;

        //
        typedef cc::SplitListSet< rcu_sht, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_SHT_dyn_cmp;

        typedef cc::SplitListSet< rcu_sht, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Michael_RCU_SHT_dyn_cmp_seqcst;

        typedef cc::SplitListSet< rcu_sht, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_SHT_st_cmp;

        typedef cc::SplitListSet< rcu_sht, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Michael_RCU_SHT_st_cmp_seqcst;

        // RCU_SHT + less
        typedef cc::SplitListSet< rcu_sht, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_SHT_dyn_less;

        typedef cc::SplitListSet< rcu_sht, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Michael_RCU_SHT_dyn_less_seqcst;

        typedef cc::SplitListSet< rcu_sht, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::michael_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::michael_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Michael_RCU_SHT_st_less;

        typedef cc::SplitListSet< rcu_sht, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Michael_RCU_SHT_st_less_seqcst;
#endif

        // ***************************************************************************
        // SplitListSet based on LazyList

        // HP
        typedef cc::SplitListSet< cds::gc::HP, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_HP_dyn_cmp;

        typedef cc::SplitListSet< cds::gc::HP, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Lazy_HP_dyn_cmp_seqcst;

        typedef cc::SplitListSet< cds::gc::HP, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_HP_st_cmp;

        typedef cc::SplitListSet< cds::gc::HP, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Lazy_HP_st_cmp_seqcst;


        // HP + less
        typedef cc::SplitListSet< cds::gc::HP, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_HP_dyn_less;

        typedef cc::SplitListSet< cds::gc::HP, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Lazy_HP_dyn_less_seqcst;

        typedef cc::SplitListSet< cds::gc::HP, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_HP_st_less;

        typedef cc::SplitListSet< cds::gc::HP, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Lazy_HP_st_less_seqcst;

        // DHP
        typedef cc::SplitListSet< cds::gc::DHP, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_DHP_dyn_cmp;

        typedef cc::SplitListSet< cds::gc::DHP, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Lazy_DHP_dyn_cmp_seqcst;

        typedef cc::SplitListSet< cds::gc::DHP, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_DHP_st_cmp;

        typedef cc::SplitListSet< cds::gc::DHP, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Lazy_DHP_st_cmp_seqcst;

        // DHP + less
        typedef cc::SplitListSet< cds::gc::DHP, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_DHP_dyn_less;

        typedef cc::SplitListSet< cds::gc::DHP, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Lazy_DHP_dyn_less_seqcst;

        typedef cc::SplitListSet< cds::gc::DHP, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_DHP_st_less;

        typedef cc::SplitListSet< cds::gc::DHP, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Lazy_DHP_st_less_seqcst;


        // RCU
        typedef cc::SplitListSet< rcu_gpi, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPI_dyn_cmp;

        typedef cc::SplitListSet< rcu_gpi, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Lazy_RCU_GPI_dyn_cmp_seqcst;

        typedef cc::SplitListSet< rcu_gpi, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPI_st_cmp;

        typedef cc::SplitListSet< rcu_gpi, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Lazy_RCU_GPI_st_cmp_seqcst;

        // RCU_GPI + less
        typedef cc::SplitListSet< rcu_gpi, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPI_dyn_less;

        typedef cc::SplitListSet< rcu_gpi, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Lazy_RCU_GPI_dyn_less_seqcst;

        typedef cc::SplitListSet< rcu_gpi, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPI_st_less;

        typedef cc::SplitListSet< rcu_gpi, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Lazy_RCU_GPI_st_less_seqcst;

        //
        typedef cc::SplitListSet< rcu_gpb, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPB_dyn_cmp;

        typedef cc::SplitListSet< rcu_gpb, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Lazy_RCU_GPB_dyn_cmp_seqcst;

        typedef cc::SplitListSet< rcu_gpb, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPB_st_cmp;

        typedef cc::SplitListSet< rcu_gpb, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Lazy_RCU_GPB_st_cmp_seqcst;

        // RCU_GPB + less
        typedef cc::SplitListSet< rcu_gpb, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPB_dyn_less;

        typedef cc::SplitListSet< rcu_gpb, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Lazy_RCU_GPB_dyn_less_seqcst;

        typedef cc::SplitListSet< rcu_gpb, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPB_st_less;

        typedef cc::SplitListSet< rcu_gpb, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Lazy_RCU_GPB_st_less_seqcst;

        //
        typedef cc::SplitListSet< rcu_gpt, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPT_dyn_cmp;

        typedef cc::SplitListSet< rcu_gpt, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Lazy_RCU_GPT_dyn_cmp_seqcst;

        typedef cc::SplitListSet< rcu_gpt, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPT_st_cmp;

        typedef cc::SplitListSet< rcu_gpt, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Lazy_RCU_GPT_st_cmp_seqcst;

        // RCU_GPT + less
        typedef cc::SplitListSet< rcu_gpt, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPT_dyn_less;

        typedef cc::SplitListSet< rcu_gpt, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Lazy_RCU_GPT_dyn_less_seqcst;

        typedef cc::SplitListSet< rcu_gpt, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_GPT_st_less;

        typedef cc::SplitListSet< rcu_gpt, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Lazy_RCU_GPT_st_less_seqcst;

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cc::SplitListSet< rcu_shb, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_SHB_dyn_cmp;

        typedef cc::SplitListSet< rcu_shb, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Lazy_RCU_SHB_dyn_cmp_seqcst;

        typedef cc::SplitListSet< rcu_shb, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_SHB_st_cmp;

        typedef cc::SplitListSet< rcu_shb, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Lazy_RCU_SHB_st_cmp_seqcst;

        // RCU_SHB + less
        typedef cc::SplitListSet< rcu_shb, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_SHB_dyn_less;

        typedef cc::SplitListSet< rcu_shb, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Lazy_RCU_SHB_dyn_less_seqcst;

        typedef cc::SplitListSet< rcu_shb, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_SHB_st_less;

        typedef cc::SplitListSet< rcu_shb, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Lazy_RCU_SHB_st_less_seqcst;

        //
        typedef cc::SplitListSet< rcu_sht, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_SHT_dyn_cmp;

        typedef cc::SplitListSet< rcu_sht, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Lazy_RCU_SHT_dyn_cmp_seqcst;

        typedef cc::SplitListSet< rcu_sht, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::compare< compare >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_SHT_st_cmp;

        typedef cc::SplitListSet< rcu_sht, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Lazy_RCU_SHT_st_cmp_seqcst;

        // RCU_SHT + less
        typedef cc::SplitListSet< rcu_sht, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_SHT_dyn_less;

        typedef cc::SplitListSet< rcu_sht, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Lazy_RCU_SHT_dyn_less_seqcst;

        typedef cc::SplitListSet< rcu_sht, key_val,
            typename cc::split_list::make_traits<
                cc::split_list::ordered_list<cc::lazy_list_tag>
                ,cc::split_list::dynamic_bucket_table< false >
                ,co::hash< hash >
                ,cc::split_list::ordered_list_traits<
                    typename cc::lazy_list::make_traits<
                        co::less< less >
                    >::type
                >
            >::type
        > SplitList_Lazy_RCU_SHT_st_less;

        typedef cc::SplitListSet< rcu_sht, key_val,
            typename cc::split_list::make_traits<
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
        > SplitList_Lazy_RCU_SHT_st_less_seqcst;

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
            bool erase_with( Q const& v, Less pred )
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
            StripedHashSet_ord( size_t nCapacity, size_t nLoadFactor )
                : base_class( 0, *(new(&m_placeHolder) resizing_policy_t( nLoadFactor * 1024 )) )
            {}

            template <typename Q, typename Less>
            bool erase_with( Q const& v, Less pred )
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

#if (CDS_COMPILER == CDS_COMPILER_MSVC || (CDS_COMPILER == CDS_COMPILER_INTEL && CDS_OS_INTERFACE == CDS_OSI_WINDOWS)) && _MSC_VER < 1600
        typedef StripedHashSet_ord<
            stdext::hash_set< key_val, hash_less >
            , co::hash< hash2 >
        > StripedSet_hashset;
#else
        typedef StripedHashSet_ord<
            std::unordered_set< key_val, hash, equal_to >
            , co::hash< hash2 >
        > StripedSet_hashset;
#endif

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
            bool erase_with( Q const& v, Less pred )
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
            RefinableHashSet_ord( size_t nCapacity, size_t nLoadFactor )
                : base_class( 0, *(new(&m_placeHolder) resizing_policy_t( nLoadFactor * 1024 )) )
            {}

            template <typename Q, typename Less>
            bool erase_with( Q const& v, Less pred )
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

#if (CDS_COMPILER == CDS_COMPILER_MSVC || (CDS_COMPILER == CDS_COMPILER_INTEL && CDS_OS_INTERFACE == CDS_OSI_WINDOWS)) && _MSC_VER < 1600
        typedef RefinableHashSet_ord<
            stdext::hash_set< key_val, hash_less >
            , co::hash< hash2 >
        > RefinableSet_hashset;
#else
        typedef RefinableHashSet_ord<
            std::unordered_set< key_val, hash, equal_to >
            , co::hash< hash2 >
        > RefinableSet_hashset;
#endif

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

        typedef CuckooStripedSet< key_val,
            cc::cuckoo::probeset_type< cc::cuckoo::list >
            ,co::equal_to< equal_to >
            ,co::hash< std::tuple< hash, hash2 > >
        > CuckooStripedSet_list_unord;

        typedef CuckooStripedSet< key_val,
            cc::cuckoo::probeset_type< cc::cuckoo::list >
            ,co::equal_to< equal_to >
            ,co::hash< std::tuple< hash, hash2 > >
            ,co::stat< cc::cuckoo::stat >
        > CuckooStripedSet_list_unord_stat;

        typedef CuckooStripedSet< key_val,
            cc::cuckoo::probeset_type< cc::cuckoo::list >
            ,co::equal_to< equal_to >
            ,co::hash< std::tuple< hash, hash2 > >
            ,cc::cuckoo::store_hash< true >
        > CuckooStripedSet_list_unord_storehash;

        typedef CuckooStripedSet< key_val,
            cc::cuckoo::probeset_type< cc::cuckoo::list >
            ,co::compare< compare >
            ,co::hash< std::tuple< hash, hash2 > >
        > CuckooStripedSet_list_ord;

        typedef CuckooStripedSet< key_val,
            cc::cuckoo::probeset_type< cc::cuckoo::list >
            ,co::compare< compare >
            ,co::hash< std::tuple< hash, hash2 > >
            ,co::stat< cc::cuckoo::stat >
        > CuckooStripedSet_list_ord_stat;

        typedef CuckooStripedSet< key_val,
            cc::cuckoo::probeset_type< cc::cuckoo::list >
            ,co::compare< compare >
            ,co::hash< std::tuple< hash, hash2 > >
            ,cc::cuckoo::store_hash< true >
        > CuckooStripedSet_list_ord_storehash;

        typedef CuckooStripedSet< key_val,
            cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            ,co::equal_to< equal_to >
            ,co::hash< std::tuple< hash, hash2 > >
        > CuckooStripedSet_vector_unord;

        typedef CuckooStripedSet< key_val,
            cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            ,co::equal_to< equal_to >
            ,co::hash< std::tuple< hash, hash2 > >
            ,co::stat< cc::cuckoo::stat >
        > CuckooStripedSet_vector_unord_stat;

        typedef CuckooStripedSet< key_val,
            cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            ,co::equal_to< equal_to >
            ,co::hash< std::tuple< hash, hash2 > >
            ,cc::cuckoo::store_hash< true >
        > CuckooStripedSet_vector_unord_storehash;

        typedef CuckooStripedSet< key_val,
            cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            ,co::compare< compare >
            ,co::hash< std::tuple< hash, hash2 > >
        > CuckooStripedSet_vector_ord;

        typedef CuckooStripedSet< key_val,
            cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            ,co::compare< compare >
            ,co::hash< std::tuple< hash, hash2 > >
            ,co::stat< cc::cuckoo::stat >
        > CuckooStripedSet_vector_ord_stat;

        typedef CuckooStripedSet< key_val,
            cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            ,co::compare< compare >
            ,co::hash< std::tuple< hash, hash2 > >
            ,cc::cuckoo::store_hash< true >
        > CuckooStripedSet_vector_ord_storehash;

        typedef CuckooRefinableSet< key_val,
            cc::cuckoo::probeset_type< cc::cuckoo::list >
            ,co::equal_to< equal_to >
            ,co::hash< std::tuple< hash, hash2 > >
        > CuckooRefinableSet_list_unord;

        typedef CuckooRefinableSet< key_val,
            cc::cuckoo::probeset_type< cc::cuckoo::list >
            ,co::equal_to< equal_to >
            ,co::hash< std::tuple< hash, hash2 > >
            ,co::stat< cc::cuckoo::stat >
        > CuckooRefinableSet_list_unord_stat;

        typedef CuckooRefinableSet< key_val,
            cc::cuckoo::probeset_type< cc::cuckoo::list >
            ,co::equal_to< equal_to >
            ,co::hash< std::tuple< hash, hash2 > >
            ,cc::cuckoo::store_hash< true >
        > CuckooRefinableSet_list_unord_storehash;

        typedef CuckooRefinableSet< key_val,
            cc::cuckoo::probeset_type< cc::cuckoo::list >
            ,co::compare< compare >
            ,co::hash< std::tuple< hash, hash2 > >
        > CuckooRefinableSet_list_ord;

        typedef CuckooRefinableSet< key_val,
            cc::cuckoo::probeset_type< cc::cuckoo::list >
            ,co::compare< compare >
            ,co::hash< std::tuple< hash, hash2 > >
            ,co::stat< cc::cuckoo::stat >
        > CuckooRefinableSet_list_ord_stat;

        typedef CuckooRefinableSet< key_val,
            cc::cuckoo::probeset_type< cc::cuckoo::list >
            ,co::compare< compare >
            ,co::hash< std::tuple< hash, hash2 > >
            ,cc::cuckoo::store_hash< true >
        > CuckooRefinableSet_list_ord_storehash;

        typedef CuckooRefinableSet< key_val,
            cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            ,co::equal_to< equal_to >
            ,co::hash< std::tuple< hash, hash2 > >
        > CuckooRefinableSet_vector_unord;

        typedef CuckooRefinableSet< key_val,
            cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            ,co::equal_to< equal_to >
            ,co::hash< std::tuple< hash, hash2 > >
            ,co::stat< cc::cuckoo::stat >
        > CuckooRefinableSet_vector_unord_stat;

        typedef CuckooRefinableSet< key_val,
            cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            ,co::equal_to< equal_to >
            ,co::hash< std::tuple< hash, hash2 > >
            ,cc::cuckoo::store_hash< true >
        > CuckooRefinableSet_vector_unord_storehash;

        typedef CuckooRefinableSet< key_val,
            cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            ,co::compare< compare >
            ,co::hash< std::tuple< hash, hash2 > >
        > CuckooRefinableSet_vector_ord;

        typedef CuckooRefinableSet< key_val,
            cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            ,co::compare< compare >
            ,co::hash< std::tuple< hash, hash2 > >
            ,co::stat< cc::cuckoo::stat >
        > CuckooRefinableSet_vector_ord_stat;

        typedef CuckooRefinableSet< key_val,
            cc::cuckoo::probeset_type< cc::cuckoo::vector<4> >
            ,co::compare< compare >
            ,co::hash< std::tuple< hash, hash2 > >
            ,cc::cuckoo::store_hash< true >
        > CuckooRefinableSet_vector_ord_storehash;


        // ***************************************************************************
        // SkipListSet - HP

        class traits_SkipListSet_hp_less_pascal: public cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_hp_less_pascal > SkipListSet_hp_less_pascal;

        class traits_SkipListSet_hp_less_pascal_seqcst: public cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::memory_model< co::v::sequential_consistent >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_hp_less_pascal_seqcst > SkipListSet_hp_less_pascal_seqcst;

        class traits_SkipListSet_hp_less_pascal_stat: public cc::skip_list::make_traits <
                co::less< less >
                ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
                ,co::stat< cc::skip_list::stat<> >
                ,co::item_counter< cds::atomicity::item_counter >
            >::type
        {};
        typedef cc::SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_hp_less_pascal_stat > SkipListSet_hp_less_pascal_stat;

        class traits_SkipListSet_hp_cmp_pascal: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_hp_cmp_pascal > SkipListSet_hp_cmp_pascal;

        class traits_SkipListSet_hp_cmp_pascal_stat: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            ,co::stat< cc::skip_list::stat<> >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_hp_cmp_pascal_stat > SkipListSet_hp_cmp_pascal_stat;

        class traits_SkipListSet_hp_less_xorshift: public cc::skip_list::make_traits <
            co::less< less >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_hp_less_xorshift > SkipListSet_hp_less_xorshift;

        class traits_SkipListSet_hp_less_xorshift_stat: public cc::skip_list::make_traits <
            co::less< less >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            ,co::stat< cc::skip_list::stat<> >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_hp_less_xorshift_stat > SkipListSet_hp_less_xorshift_stat;

        class traits_SkipListSet_hp_cmp_xorshift: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_hp_cmp_xorshift > SkipListSet_hp_cmp_xorshift;

        class traits_SkipListSet_hp_cmp_xorshift_stat: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            ,co::stat< cc::skip_list::stat<> >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< cds::gc::HP, key_val, traits_SkipListSet_hp_cmp_xorshift_stat > SkipListSet_hp_cmp_xorshift_stat;

        // ***************************************************************************
        // SkipListSet - DHP

        class traits_SkipListSet_ptb_less_pascal: public cc::skip_list::make_traits <
            co::less< less >
            ,co::item_counter< cds::atomicity::item_counter >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
        >::type
        {};
        typedef cc::SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_ptb_less_pascal > SkipListSet_ptb_less_pascal;

        class traits_SkipListSet_ptb_less_pascal_seqcst: public cc::skip_list::make_traits <
            co::less< less >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            ,co::memory_model< co::v::sequential_consistent >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_ptb_less_pascal_seqcst > SkipListSet_ptb_less_pascal_seqcst;

        class traits_SkipListSet_ptb_less_pascal_stat: public cc::skip_list::make_traits <
            co::less< less >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            ,co::stat< cc::skip_list::stat<> >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_ptb_less_pascal_stat > SkipListSet_ptb_less_pascal_stat;

        class traits_SkipListSet_ptb_cmp_pascal: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_ptb_cmp_pascal > SkipListSet_ptb_cmp_pascal;

        class traits_SkipListSet_ptb_cmp_pascal_stat: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            ,co::stat< cc::skip_list::stat<> >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_ptb_cmp_pascal_stat > SkipListSet_ptb_cmp_pascal_stat;

        class traits_SkipListSet_ptb_less_xorshift: public cc::skip_list::make_traits <
            co::less< less >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_ptb_less_xorshift > SkipListSet_ptb_less_xorshift;

        class traits_SkipListSet_ptb_less_xorshift_stat: public cc::skip_list::make_traits <
            co::less< less >
            ,co::item_counter< cds::atomicity::item_counter >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            ,co::stat< cc::skip_list::stat<> >
        >::type
        {};
        typedef cc::SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_ptb_less_xorshift_stat > SkipListSet_ptb_less_xorshift_stat;

        class traits_SkipListSet_ptb_cmp_xorshift: public cc::skip_list::make_traits <
            co::compare< compare >
            ,co::item_counter< cds::atomicity::item_counter >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
        >::type
        {};
        typedef cc::SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_ptb_cmp_xorshift> SkipListSet_ptb_cmp_xorshift;

        class traits_SkipListSet_ptb_cmp_xorshift_stat: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            ,co::stat< cc::skip_list::stat<> >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< cds::gc::DHP, key_val, traits_SkipListSet_ptb_cmp_xorshift_stat > SkipListSet_ptb_cmp_xorshift_stat;


        // ***************************************************************************
        // SkipListSet - RCU general_instant

        class traits_SkipListSet_rcu_gpi_less_pascal: public cc::skip_list::make_traits <
            co::less< less >
            ,co::item_counter< cds::atomicity::item_counter >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
        >::type
        {};
        typedef cc::SkipListSet< rcu_gpi, key_val, traits_SkipListSet_rcu_gpi_less_pascal > SkipListSet_rcu_gpi_less_pascal;

        class traits_SkipListSet_rcu_gpi_less_pascal_seqcst: public cc::skip_list::make_traits <
            co::less< less >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            ,co::memory_model< co::v::sequential_consistent >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_gpi, key_val, traits_SkipListSet_rcu_gpi_less_pascal_seqcst > SkipListSet_rcu_gpi_less_pascal_seqcst;

        class traits_SkipListSet_rcu_gpi_less_pascal_stat: public cc::skip_list::make_traits <
            co::less< less >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            ,co::stat< cc::skip_list::stat<> >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_gpi, key_val, traits_SkipListSet_rcu_gpi_less_pascal_stat > SkipListSet_rcu_gpi_less_pascal_stat;

        class traits_SkipListSet_rcu_gpi_cmp_pascal: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_gpi, key_val, traits_SkipListSet_rcu_gpi_cmp_pascal > SkipListSet_rcu_gpi_cmp_pascal;

        class traits_SkipListSet_rcu_gpi_cmp_pascal_stat: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            ,co::stat< cc::skip_list::stat<> >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_gpi, key_val, traits_SkipListSet_rcu_gpi_cmp_pascal_stat > SkipListSet_rcu_gpi_cmp_pascal_stat;

        class traits_SkipListSet_rcu_gpi_less_xorshift: public cc::skip_list::make_traits <
            co::less< less >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_gpi, key_val, traits_SkipListSet_rcu_gpi_less_xorshift > SkipListSet_rcu_gpi_less_xorshift;

        class traits_SkipListSet_rcu_gpi_less_xorshift_stat: public cc::skip_list::make_traits <
            co::less< less >
            ,co::item_counter< cds::atomicity::item_counter >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            ,co::stat< cc::skip_list::stat<> >
        >::type
        {};
        typedef cc::SkipListSet< rcu_gpi, key_val, traits_SkipListSet_rcu_gpi_less_xorshift_stat > SkipListSet_rcu_gpi_less_xorshift_stat;

        class traits_SkipListSet_rcu_gpi_cmp_xorshift: public cc::skip_list::make_traits <
            co::compare< compare >
            ,co::item_counter< cds::atomicity::item_counter >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
        >::type
        {};
        typedef cc::SkipListSet< rcu_gpi, key_val, traits_SkipListSet_rcu_gpi_cmp_xorshift > SkipListSet_rcu_gpi_cmp_xorshift;

        class traits_SkipListSet_rcu_gpi_cmp_xorshift_stat: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            ,co::stat< cc::skip_list::stat<> >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_gpi, key_val, traits_SkipListSet_rcu_gpi_cmp_xorshift_stat > SkipListSet_rcu_gpi_cmp_xorshift_stat;


        // ***************************************************************************
        // SkipListSet - RCU general_buffered

        class traits_SkipListSet_rcu_gpb_less_pascal: public cc::skip_list::make_traits <
            co::less< less >
            ,co::item_counter< cds::atomicity::item_counter >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
        >::type
        {};
        typedef cc::SkipListSet< rcu_gpb, key_val, traits_SkipListSet_rcu_gpb_less_pascal > SkipListSet_rcu_gpb_less_pascal;

        class traits_SkipListSet_rcu_gpb_less_pascal_seqcst: public cc::skip_list::make_traits <
            co::less< less >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            ,co::memory_model< co::v::sequential_consistent >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_gpb, key_val, traits_SkipListSet_rcu_gpb_less_pascal_seqcst > SkipListSet_rcu_gpb_less_pascal_seqcst;

        class traits_SkipListSet_rcu_gpb_less_pascal_stat: public cc::skip_list::make_traits <
            co::less< less >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            ,co::stat< cc::skip_list::stat<> >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_gpb, key_val, traits_SkipListSet_rcu_gpb_less_pascal_stat > SkipListSet_rcu_gpb_less_pascal_stat;

        class traits_SkipListSet_rcu_gpb_cmp_pascal: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_gpb, key_val, traits_SkipListSet_rcu_gpb_cmp_pascal > SkipListSet_rcu_gpb_cmp_pascal;

        class traits_SkipListSet_rcu_gpb_cmp_pascal_stat: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            ,co::stat< cc::skip_list::stat<> >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_gpb, key_val, traits_SkipListSet_rcu_gpb_cmp_pascal_stat > SkipListSet_rcu_gpb_cmp_pascal_stat;

        class traits_SkipListSet_rcu_gpb_less_xorshift: public cc::skip_list::make_traits <
            co::less< less >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_gpb, key_val, traits_SkipListSet_rcu_gpb_less_xorshift > SkipListSet_rcu_gpb_less_xorshift;

        class traits_SkipListSet_rcu_gpb_less_xorshift_stat: public cc::skip_list::make_traits <
            co::less< less >
            ,co::item_counter< cds::atomicity::item_counter >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            ,co::stat< cc::skip_list::stat<> >
        >::type
        {};
        typedef cc::SkipListSet< rcu_gpb, key_val, traits_SkipListSet_rcu_gpb_less_xorshift_stat > SkipListSet_rcu_gpb_less_xorshift_stat;

        class traits_SkipListSet_rcu_gpb_cmp_xorshift: public cc::skip_list::make_traits <
            co::compare< compare >
            ,co::item_counter< cds::atomicity::item_counter >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
        >::type
        {};
        typedef cc::SkipListSet< rcu_gpb, key_val, traits_SkipListSet_rcu_gpb_cmp_xorshift > SkipListSet_rcu_gpb_cmp_xorshift;

        class traits_SkipListSet_rcu_gpb_cmp_xorshift_stat: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            ,co::stat< cc::skip_list::stat<> >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_gpb, key_val, traits_SkipListSet_rcu_gpb_cmp_xorshift_stat > SkipListSet_rcu_gpb_cmp_xorshift_stat;

        // ***************************************************************************
        // SkipListSet - RCU general_threaded

        class traits_SkipListSet_rcu_gpt_less_pascal: public cc::skip_list::make_traits <
            co::less< less >
            ,co::item_counter< cds::atomicity::item_counter >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
        >::type
        {};
        typedef cc::SkipListSet< rcu_gpt, key_val, traits_SkipListSet_rcu_gpt_less_pascal > SkipListSet_rcu_gpt_less_pascal;

        class traits_SkipListSet_rcu_gpt_less_pascal_seqcst: public cc::skip_list::make_traits <
            co::less< less >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            ,co::memory_model< co::v::sequential_consistent >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_gpt, key_val, traits_SkipListSet_rcu_gpt_less_pascal_seqcst > SkipListSet_rcu_gpt_less_pascal_seqcst;

        class traits_SkipListSet_rcu_gpt_less_pascal_stat: public cc::skip_list::make_traits <
            co::less< less >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            ,co::stat< cc::skip_list::stat<> >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_gpt, key_val, traits_SkipListSet_rcu_gpt_less_pascal_stat > SkipListSet_rcu_gpt_less_pascal_stat;

        class traits_SkipListSet_rcu_gpt_cmp_pascal: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_gpt, key_val, traits_SkipListSet_rcu_gpt_cmp_pascal > SkipListSet_rcu_gpt_cmp_pascal;

        class traits_SkipListSet_rcu_gpt_cmp_pascal_stat: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            ,co::stat< cc::skip_list::stat<> >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_gpt, key_val, traits_SkipListSet_rcu_gpt_cmp_pascal_stat > SkipListSet_rcu_gpt_cmp_pascal_stat;

        class traits_SkipListSet_rcu_gpt_less_xorshift: public cc::skip_list::make_traits <
            co::less< less >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_gpt, key_val, traits_SkipListSet_rcu_gpt_less_xorshift > SkipListSet_rcu_gpt_less_xorshift;

        class traits_SkipListSet_rcu_gpt_less_xorshift_stat: public cc::skip_list::make_traits <
            co::less< less >
            ,co::item_counter< cds::atomicity::item_counter >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            ,co::stat< cc::skip_list::stat<> >
        >::type
        {};
        typedef cc::SkipListSet< rcu_gpt, key_val, traits_SkipListSet_rcu_gpt_less_xorshift_stat > SkipListSet_rcu_gpt_less_xorshift_stat;

        class traits_SkipListSet_rcu_gpt_cmp_xorshift: public cc::skip_list::make_traits <
            co::compare< compare >
            ,co::item_counter< cds::atomicity::item_counter >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
        >::type
        {};
        typedef cc::SkipListSet< rcu_gpt, key_val, traits_SkipListSet_rcu_gpt_cmp_xorshift > SkipListSet_rcu_gpt_cmp_xorshift;

        class traits_SkipListSet_rcu_gpt_cmp_xorshift_stat: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            ,co::stat< cc::skip_list::stat<> >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_gpt, key_val, traits_SkipListSet_rcu_gpt_cmp_xorshift_stat > SkipListSet_rcu_gpt_cmp_xorshift_stat;

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        // ***************************************************************************
        // SkipListSet - RCU signal_buffered

        class traits_SkipListSet_rcu_shb_less_pascal: public cc::skip_list::make_traits <
            co::less< less >
            ,co::item_counter< cds::atomicity::item_counter >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
        >::type
        {};
        typedef cc::SkipListSet< rcu_shb, key_val, traits_SkipListSet_rcu_shb_less_pascal > SkipListSet_rcu_shb_less_pascal;

        class traits_SkipListSet_rcu_shb_less_pascal_seqcst: public cc::skip_list::make_traits <
            co::less< less >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            ,co::memory_model< co::v::sequential_consistent >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_shb, key_val, traits_SkipListSet_rcu_shb_less_pascal_seqcst > SkipListSet_rcu_shb_less_pascal_seqcst;

        class traits_SkipListSet_rcu_shb_less_pascal_stat: public cc::skip_list::make_traits <
            co::less< less >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            ,co::stat< cc::skip_list::stat<> >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_shb, key_val, traits_SkipListSet_rcu_shb_less_pascal_stat > SkipListSet_rcu_shb_less_pascal_stat;

        class traits_SkipListSet_rcu_shb_cmp_pascal: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_shb, key_val, traits_SkipListSet_rcu_shb_cmp_pascal > SkipListSet_rcu_shb_cmp_pascal;

        class traits_SkipListSet_rcu_shb_cmp_pascal_stat: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            ,co::stat< cc::skip_list::stat<> >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_shb, key_val, traits_SkipListSet_rcu_shb_cmp_pascal_stat > SkipListSet_rcu_shb_cmp_pascal_stat;

        class traits_SkipListSet_rcu_shb_less_xorshift: public cc::skip_list::make_traits <
            co::less< less >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_shb, key_val, traits_SkipListSet_rcu_shb_less_xorshift > SkipListSet_rcu_shb_less_xorshift;

        class traits_SkipListSet_rcu_shb_less_xorshift_stat: public cc::skip_list::make_traits <
            co::less< less >
            ,co::item_counter< cds::atomicity::item_counter >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            ,co::stat< cc::skip_list::stat<> >
        >::type
        {};
        typedef cc::SkipListSet< rcu_shb, key_val, traits_SkipListSet_rcu_shb_less_xorshift_stat > SkipListSet_rcu_shb_less_xorshift_stat;

        class traits_SkipListSet_rcu_shb_cmp_xorshift: public cc::skip_list::make_traits <
            co::compare< compare >
            ,co::item_counter< cds::atomicity::item_counter >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
        >::type
        {};
        typedef cc::SkipListSet< rcu_shb, key_val, traits_SkipListSet_rcu_shb_cmp_xorshift > SkipListSet_rcu_shb_cmp_xorshift;

        class traits_SkipListSet_rcu_shb_cmp_xorshift_stat: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            ,co::stat< cc::skip_list::stat<> >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_shb, key_val, traits_SkipListSet_rcu_shb_cmp_xorshift_stat > SkipListSet_rcu_shb_cmp_xorshift_stat;

        // ***************************************************************************
        // SkipListSet - RCU signal_threaded

        class traits_SkipListSet_rcu_sht_less_pascal: public cc::skip_list::make_traits <
            co::less< less >
            ,co::item_counter< cds::atomicity::item_counter >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
        >::type
        {};
        typedef cc::SkipListSet< rcu_sht, key_val, traits_SkipListSet_rcu_sht_less_pascal > SkipListSet_rcu_sht_less_pascal;

        class traits_SkipListSet_rcu_sht_less_pascal_seqcst: public cc::skip_list::make_traits <
            co::less< less >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            ,co::memory_model< co::v::sequential_consistent >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_sht, key_val, traits_SkipListSet_rcu_sht_less_pascal_seqcst > SkipListSet_rcu_sht_less_pascal_seqcst;

        class traits_SkipListSet_rcu_sht_less_pascal_stat: public cc::skip_list::make_traits <
            co::less< less >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            ,co::stat< cc::skip_list::stat<> >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_sht, key_val, traits_SkipListSet_rcu_sht_less_pascal_stat > SkipListSet_rcu_sht_less_pascal_stat;

        class traits_SkipListSet_rcu_sht_cmp_pascal: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_sht, key_val, traits_SkipListSet_rcu_sht_cmp_pascal > SkipListSet_rcu_sht_cmp_pascal;

        class traits_SkipListSet_rcu_sht_cmp_pascal_stat: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::turbo_pascal >
            ,co::stat< cc::skip_list::stat<> >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_sht, key_val, traits_SkipListSet_rcu_sht_cmp_pascal_stat > SkipListSet_rcu_sht_cmp_pascal_stat;

        class traits_SkipListSet_rcu_sht_less_xorshift: public cc::skip_list::make_traits <
            co::less< less >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_sht, key_val, traits_SkipListSet_rcu_sht_less_xorshift > SkipListSet_rcu_sht_less_xorshift;

        class traits_SkipListSet_rcu_sht_less_xorshift_stat: public cc::skip_list::make_traits <
            co::less< less >
            ,co::item_counter< cds::atomicity::item_counter >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            ,co::stat< cc::skip_list::stat<> >
        >::type
        {};
        typedef cc::SkipListSet< rcu_sht, key_val, traits_SkipListSet_rcu_sht_less_xorshift_stat > SkipListSet_rcu_sht_less_xorshift_stat;

        class traits_SkipListSet_rcu_sht_cmp_xorshift: public cc::skip_list::make_traits <
            co::compare< compare >
            ,co::item_counter< cds::atomicity::item_counter >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
        >::type
        {};
        typedef cc::SkipListSet< rcu_sht, key_val, traits_SkipListSet_rcu_sht_cmp_xorshift > SkipListSet_rcu_sht_cmp_xorshift;

        class traits_SkipListSet_rcu_sht_cmp_xorshift_stat: public cc::skip_list::make_traits <
            co::compare< compare >
            ,cc::skip_list::random_level_generator< cc::skip_list::xorshift >
            ,co::stat< cc::skip_list::stat<> >
            ,co::item_counter< cds::atomicity::item_counter >
        >::type
        {};
        typedef cc::SkipListSet< rcu_sht, key_val, traits_SkipListSet_rcu_sht_cmp_xorshift_stat > SkipListSet_rcu_sht_cmp_xorshift_stat;
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

            struct ptb_gc {
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


        // ***************************************************************************
        // EllenBinTreeSet - HP

        class traits_EllenBinTreeSet_hp: public cc::ellen_bintree::make_set_traits<
            cc::ellen_bintree::key_extractor< typename ellen_bintree_props::key_extractor >
            ,co::less< typename ellen_bintree_props::less >
            ,cc::ellen_bintree::update_desc_allocator<
                cds::memory::pool_allocator< typename ellen_bintree_props::hp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor >
            >
            ,co::node_allocator< ellen_bintree_pool::internal_node_allocator< int > >
        >::type
        {};
        typedef cc::EllenBinTreeSet< cds::gc::HP, key_type, key_val, traits_EllenBinTreeSet_hp > EllenBinTreeSet_hp;

        class traits_EllenBinTreeSet_hp_stat: public cc::ellen_bintree::make_set_traits<
            cc::ellen_bintree::key_extractor< typename ellen_bintree_props::key_extractor >
            ,co::less< typename ellen_bintree_props::less >
            ,cc::ellen_bintree::update_desc_allocator<
                cds::memory::pool_allocator< typename ellen_bintree_props::hp_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor >
            >
            ,co::node_allocator< ellen_bintree_pool::internal_node_allocator< int > >
            ,co::stat< cc::ellen_bintree::stat<> >
        >::type
        {};
        typedef cc::EllenBinTreeSet< cds::gc::HP, key_type, key_val, traits_EllenBinTreeSet_hp_stat > EllenBinTreeSet_hp_stat;

        // ***************************************************************************
        // EllenBinTreeSet - DHP

        class traits_EllenBinTreeSet_ptb: public cc::ellen_bintree::make_set_traits<
            cc::ellen_bintree::key_extractor< typename ellen_bintree_props::key_extractor >
            ,co::less< typename ellen_bintree_props::less >
            ,cc::ellen_bintree::update_desc_allocator<
                cds::memory::pool_allocator< typename ellen_bintree_props::ptb_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor >
            >
            ,co::node_allocator< ellen_bintree_pool::internal_node_allocator< int > >
        >::type
        {};
        typedef cc::EllenBinTreeSet< cds::gc::DHP, key_type, key_val, traits_EllenBinTreeSet_ptb > EllenBinTreeSet_ptb;

        class traits_EllenBinTreeSet_ptb_stat: public cc::ellen_bintree::make_set_traits<
            cc::ellen_bintree::key_extractor< typename ellen_bintree_props::key_extractor >
            ,co::less< typename ellen_bintree_props::less >
            ,cc::ellen_bintree::update_desc_allocator<
                cds::memory::pool_allocator< typename ellen_bintree_props::ptb_gc::update_desc, ellen_bintree_pool::update_desc_pool_accessor >
            >
            ,co::node_allocator< ellen_bintree_pool::internal_node_allocator< int > >
            ,co::stat< cc::ellen_bintree::stat<> >
        >::type
        {};
        typedef cc::EllenBinTreeSet< cds::gc::DHP, key_type, key_val, traits_EllenBinTreeSet_ptb_stat > EllenBinTreeSet_ptb_stat;


        // ***************************************************************************
        // EllenBinTreeSet - RCU

        class traits_EllenBinTreeSet_rcu_gpi: public cc::ellen_bintree::make_set_traits<
            cc::ellen_bintree::key_extractor< typename ellen_bintree_props::key_extractor >
            ,co::less< typename ellen_bintree_props::less >
            ,cc::ellen_bintree::update_desc_allocator<
                cds::memory::pool_allocator< typename ellen_bintree_props::gpi::update_desc, ellen_bintree_pool::bounded_update_desc_pool_accessor >
            >
            ,co::node_allocator< ellen_bintree_pool::internal_node_allocator< int > >
        >::type
        {};
        typedef cc::EllenBinTreeSet< rcu_gpi, key_type, key_val, traits_EllenBinTreeSet_rcu_gpi > EllenBinTreeSet_rcu_gpi;

        class traits_EllenBinTreeSet_rcu_gpi_stat: public cc::ellen_bintree::make_set_traits<
            cc::ellen_bintree::key_extractor< typename ellen_bintree_props::key_extractor >
            ,co::less< typename ellen_bintree_props::less >
            ,cc::ellen_bintree::update_desc_allocator<
                cds::memory::pool_allocator< typename ellen_bintree_props::gpi::update_desc, ellen_bintree_pool::bounded_update_desc_pool_accessor >
            >
            ,co::node_allocator< ellen_bintree_pool::internal_node_allocator< int > >
            ,co::stat< cc::ellen_bintree::stat<> >
        >::type
        {};
        typedef cc::EllenBinTreeSet< rcu_gpi, key_type, key_val, traits_EllenBinTreeSet_rcu_gpi_stat > EllenBinTreeSet_rcu_gpi_stat;

        class traits_EllenBinTreeSet_rcu_gpb: public cc::ellen_bintree::make_set_traits<
            cc::ellen_bintree::key_extractor< typename ellen_bintree_props::key_extractor >
            ,co::less< typename ellen_bintree_props::less >
            ,cc::ellen_bintree::update_desc_allocator<
                cds::memory::pool_allocator< typename ellen_bintree_props::gpb::update_desc, ellen_bintree_pool::update_desc_pool_accessor >
            >
            ,co::node_allocator< ellen_bintree_pool::internal_node_allocator< int > >
        >::type
        {};
        typedef cc::EllenBinTreeSet< rcu_gpb, key_type, key_val, traits_EllenBinTreeSet_rcu_gpb > EllenBinTreeSet_rcu_gpb;

        class traits_EllenBinTreeSet_rcu_gpb_stat: public cc::ellen_bintree::make_set_traits<
            cc::ellen_bintree::key_extractor< typename ellen_bintree_props::key_extractor >
            ,co::less< typename ellen_bintree_props::less >
            ,cc::ellen_bintree::update_desc_allocator<
                cds::memory::pool_allocator< typename ellen_bintree_props::gpb::update_desc, ellen_bintree_pool::update_desc_pool_accessor >
            >
            ,co::node_allocator< ellen_bintree_pool::internal_node_allocator< int > >
            ,co::stat< cc::ellen_bintree::stat<> >
        >::type
        {};
        typedef cc::EllenBinTreeSet< rcu_gpb, key_type, key_val, traits_EllenBinTreeSet_rcu_gpb_stat > EllenBinTreeSet_rcu_gpb_stat;

        class traits_EllenBinTreeSet_rcu_gpt: public cc::ellen_bintree::make_set_traits<
            cc::ellen_bintree::key_extractor< typename ellen_bintree_props::key_extractor >
            ,co::less< typename ellen_bintree_props::less >
            ,cc::ellen_bintree::update_desc_allocator<
                cds::memory::pool_allocator< typename ellen_bintree_props::gpt::update_desc, ellen_bintree_pool::update_desc_pool_accessor >
            >
            ,co::node_allocator< ellen_bintree_pool::internal_node_allocator< int > >
        >::type
        {};
        typedef cc::EllenBinTreeSet< rcu_gpt, key_type, key_val, traits_EllenBinTreeSet_rcu_gpt > EllenBinTreeSet_rcu_gpt;

        class traits_EllenBinTreeSet_rcu_gpt_stat: public cc::ellen_bintree::make_set_traits<
            cc::ellen_bintree::key_extractor< typename ellen_bintree_props::key_extractor >
            ,co::less< typename ellen_bintree_props::less >
            ,cc::ellen_bintree::update_desc_allocator<
                cds::memory::pool_allocator< typename ellen_bintree_props::gpt::update_desc, ellen_bintree_pool::update_desc_pool_accessor >
            >
            ,co::node_allocator< ellen_bintree_pool::internal_node_allocator< int > >
            ,co::stat< cc::ellen_bintree::stat<> >
        >::type
        {};
        typedef cc::EllenBinTreeSet< rcu_gpt, key_type, key_val, traits_EllenBinTreeSet_rcu_gpt_stat > EllenBinTreeSet_rcu_gpt_stat;

#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        class traits_EllenBinTreeSet_rcu_shb: public cc::ellen_bintree::make_set_traits<
            cc::ellen_bintree::key_extractor< typename ellen_bintree_props::key_extractor >
            ,co::less< typename ellen_bintree_props::less >
            ,cc::ellen_bintree::update_desc_allocator<
                cds::memory::pool_allocator< typename ellen_bintree_props::shb::update_desc, ellen_bintree_pool::update_desc_pool_accessor >
            >
            ,co::node_allocator< ellen_bintree_pool::internal_node_allocator< int > >
        >::type
        {};
        typedef cc::EllenBinTreeSet< rcu_shb, key_type, key_val, traits_EllenBinTreeSet_rcu_shb > EllenBinTreeSet_rcu_shb;

        class traits_EllenBinTreeSet_rcu_shb_stat: public cc::ellen_bintree::make_set_traits<
            cc::ellen_bintree::key_extractor< typename ellen_bintree_props::key_extractor >
            ,co::less< typename ellen_bintree_props::less >
            ,cc::ellen_bintree::update_desc_allocator<
                cds::memory::pool_allocator< typename ellen_bintree_props::shb::update_desc, ellen_bintree_pool::update_desc_pool_accessor >
            >
            ,co::node_allocator< ellen_bintree_pool::internal_node_allocator< int > >
            ,co::stat< cc::ellen_bintree::stat<> >
        >::type
        {};
        typedef cc::EllenBinTreeSet< rcu_shb, key_type, key_val, traits_EllenBinTreeSet_rcu_shb_stat > EllenBinTreeSet_rcu_shb_stat;

        class traits_EllenBinTreeSet_rcu_sht: public cc::ellen_bintree::make_set_traits<
            cc::ellen_bintree::key_extractor< typename ellen_bintree_props::key_extractor >
            ,co::less< typename ellen_bintree_props::less >
            ,cc::ellen_bintree::update_desc_allocator<
                cds::memory::pool_allocator< typename ellen_bintree_props::sht::update_desc, ellen_bintree_pool::update_desc_pool_accessor >
            >
            ,co::node_allocator< ellen_bintree_pool::internal_node_allocator< int > >
        >::type
        {};
        typedef cc::EllenBinTreeSet< rcu_sht, key_type, key_val, traits_EllenBinTreeSet_rcu_sht > EllenBinTreeSet_rcu_sht;

        class traits_EllenBinTreeSet_rcu_sht_stat: public cc::ellen_bintree::make_set_traits<
            cc::ellen_bintree::key_extractor< typename ellen_bintree_props::key_extractor >
            ,co::less< typename ellen_bintree_props::less >
            ,cc::ellen_bintree::update_desc_allocator<
                cds::memory::pool_allocator< typename ellen_bintree_props::sht::update_desc, ellen_bintree_pool::update_desc_pool_accessor >
            >
            ,co::node_allocator< ellen_bintree_pool::internal_node_allocator< int > >
            ,co::stat< cc::ellen_bintree::stat<> >
        >::type
        {};
        typedef cc::EllenBinTreeSet< rcu_sht, key_type, key_val, traits_EllenBinTreeSet_rcu_sht_stat > EllenBinTreeSet_rcu_sht_stat;

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
    static inline void print_stat( Set const& s )
    {}

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

    template <typename V, typename... Options>
    static inline void print_stat( CuckooStripedSet< V, Options... > const& s )
    {
        typedef CuckooStripedSet< V, Options... > set_type;
        print_stat( static_cast<typename set_type::cuckoo_base_class const&>(s) );
    }

    template <typename V, typename... Options>
    static inline void print_stat( CuckooRefinableSet< V, Options... > const& s )
    {
        typedef CuckooRefinableSet< V, Options... > set_type;
        print_stat( static_cast<typename set_type::cuckoo_base_class const&>(s) );
    }



    //*******************************************************
    // additional_check
    //*******************************************************

    template <typename Set>
    static inline void additional_check( Set& set )
    {}

    template <typename Set>
    static inline void additional_cleanup( Set& set )
    {}

    namespace ellen_bintree_check {
        static inline void check_stat( cds::intrusive::ellen_bintree::empty_stat const& s )
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
    static inline void additional_cleanup( cc::EllenBinTreeSet<GC, Key, T, Traits>& s )
    {
        ellen_bintree_pool::internal_node_counter::reset();
    }

}   // namespace set2

#endif // ifndef _CDSUNIT_SET2_SET_TYPES_H
