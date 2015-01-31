//$$CDS-header$$

#ifndef CDSUNIT_PQUEUE_TYPES_H
#define CDSUNIT_PQUEUE_TYPES_H

#include <cds/container/mspriority_queue.h>
#include <cds/container/fcpriority_queue.h>

#include "pqueue/std_pqueue.h"
#include "pqueue/ellen_bintree_pqueue.h"
#include "pqueue/skiplist_pqueue.h"

#include <vector>
#include <deque>
#include <boost/container/stable_vector.hpp>
#include <boost/container/deque.hpp>
#include <cds/sync/spinlock.h>

#include "print_ellenbintree_stat.h"
#include "print_skip_list_stat.h"
#include "print_mspriorityqueue_stat.h"

namespace pqueue {
    namespace cc = cds::container;
    namespace co = cds::opt;

    template <typename Value>
    struct Types
    {
        static size_t const c_nBoundedCapacity = 1024 * 1024 * 16;

        typedef std::less<Value>    less;

        struct cmp {
            int operator()( Value const& v1, Value const& v2 ) const
            {
                return less()( v1, v2 ) ? -1 : less()( v2, v1 ) ? 1 : 0;
            }
        };

        typedef cds::urcu::gc< cds::urcu::general_instant<> >   rcu_gpi;
        typedef cds::urcu::gc< cds::urcu::general_buffered<> >  rcu_gpb;
        typedef cds::urcu::gc< cds::urcu::general_threaded<> >  rcu_gpt;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef cds::urcu::gc< cds::urcu::signal_buffered<> >  rcu_shb;
        typedef cds::urcu::gc< cds::urcu::signal_threaded<> >  rcu_sht;
#endif


        // MSPriorityQueue
        struct traits_MSPriorityQueue_static_less : public
            cc::mspriority_queue::make_traits <
                co::buffer < co::v::static_buffer< char, c_nBoundedCapacity > >
            > ::type
        {};
        typedef cc::MSPriorityQueue< Value, traits_MSPriorityQueue_static_less > MSPriorityQueue_static_less;

        struct traits_MSPriorityQueue_static_less_stat : public cc::mspriority_queue::traits
        {
            typedef co::v::static_buffer< char, c_nBoundedCapacity > buffer;
            typedef cc::mspriority_queue::stat<> stat;
        };
        typedef cc::MSPriorityQueue< Value, traits_MSPriorityQueue_static_less_stat > MSPriorityQueue_static_less_stat;

        struct traits_MSPriorityQueue_static_cmp : public
            cc::mspriority_queue::make_traits <
                co::buffer< co::v::static_buffer< char, c_nBoundedCapacity > >
                , co::compare < cmp >
            > ::type
        {};
        typedef cc::MSPriorityQueue< Value, traits_MSPriorityQueue_static_cmp > MSPriorityQueue_static_cmp;

        struct traits_MSPriorityQueue_static_mutex : public
            cc::mspriority_queue::make_traits<
                co::buffer< co::v::static_buffer< char, c_nBoundedCapacity > >
                , co::lock_type<std::mutex>
            >::type
        {};
        typedef cc::MSPriorityQueue< Value, traits_MSPriorityQueue_static_mutex > MSPriorityQueue_static_mutex;

        struct traits_MSPriorityQueue_dyn_less : public
            cc::mspriority_queue::make_traits<
                co::buffer< co::v::dynamic_buffer< char > >
            >::type
        {};
        typedef cc::MSPriorityQueue< Value, traits_MSPriorityQueue_dyn_less > MSPriorityQueue_dyn_less;

        struct traits_MSPriorityQueue_dyn_less_stat : public
            cc::mspriority_queue::make_traits <
                co::buffer< co::v::dynamic_buffer< char > >
                , co::stat < cc::mspriority_queue::stat<> >
            > ::type
        {};
        typedef cc::MSPriorityQueue< Value, traits_MSPriorityQueue_dyn_less_stat > MSPriorityQueue_dyn_less_stat;

        struct traits_MSPriorityQueue_dyn_cmp : public
            cc::mspriority_queue::make_traits <
                co::buffer< co::v::dynamic_buffer< char > >
                , co::compare < cmp >
            > ::type
        {};
        typedef cc::MSPriorityQueue< Value, traits_MSPriorityQueue_dyn_cmp > MSPriorityQueue_dyn_cmp;

        struct traits_MSPriorityQueue_dyn_mutex : public
            cc::mspriority_queue::make_traits <
                co::buffer< co::v::dynamic_buffer< char > >
                , co::lock_type < std::mutex >
            > ::type
        {};
        typedef cc::MSPriorityQueue< Value, traits_MSPriorityQueue_dyn_mutex > MSPriorityQueue_dyn_mutex;


        // Priority queue based on EllenBinTreeSet
        struct traits_EllenBinTree_max :
            public cc::ellen_bintree::make_set_traits<
                cc::ellen_bintree::key_extractor< typename Value::key_extractor >
                ,cc::opt::less< std::less<Value> >
                ,co::stat< cc::ellen_bintree::stat<> >
            >::type
        {};
        typedef EllenBinTreePQueue< cds::gc::HP, typename Value::key_type, Value, traits_EllenBinTree_max > EllenBinTree_HP_max;
        typedef EllenBinTreePQueue< cds::gc::DHP, typename Value::key_type, Value, traits_EllenBinTree_max > EllenBinTree_DHP_max;
        typedef EllenBinTreePQueue< rcu_gpi, typename Value::key_type, Value, traits_EllenBinTree_max > EllenBinTree_RCU_gpi_max;
        typedef EllenBinTreePQueue< rcu_gpb, typename Value::key_type, Value, traits_EllenBinTree_max > EllenBinTree_RCU_gpb_max;
        typedef EllenBinTreePQueue< rcu_gpt, typename Value::key_type, Value, traits_EllenBinTree_max > EllenBinTree_RCU_gpt_max;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef EllenBinTreePQueue< rcu_shb, typename Value::key_type, Value, traits_EllenBinTree_max > EllenBinTree_RCU_shb_max;
        typedef EllenBinTreePQueue< rcu_sht, typename Value::key_type, Value, traits_EllenBinTree_max > EllenBinTree_RCU_sht_max;
#endif

        struct traits_EllenBinTree_max_stat :
            public cc::ellen_bintree::make_set_traits<
                cc::ellen_bintree::key_extractor< typename Value::key_extractor >
                ,cc::opt::less< std::less<Value> >
                ,co::stat< cc::ellen_bintree::stat<> >
            >::type
        {};
        typedef EllenBinTreePQueue< cds::gc::HP, typename Value::key_type, Value, traits_EllenBinTree_max_stat > EllenBinTree_HP_max_stat;
        typedef EllenBinTreePQueue< cds::gc::DHP, typename Value::key_type, Value, traits_EllenBinTree_max_stat > EllenBinTree_DHP_max_stat;
        typedef EllenBinTreePQueue< rcu_gpi, typename Value::key_type, Value, traits_EllenBinTree_max_stat > EllenBinTree_RCU_gpi_max_stat;
        typedef EllenBinTreePQueue< rcu_gpb, typename Value::key_type, Value, traits_EllenBinTree_max_stat > EllenBinTree_RCU_gpb_max_stat;
        typedef EllenBinTreePQueue< rcu_gpt, typename Value::key_type, Value, traits_EllenBinTree_max_stat > EllenBinTree_RCU_gpt_max_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef EllenBinTreePQueue< rcu_shb, typename Value::key_type, Value, traits_EllenBinTree_max_stat > EllenBinTree_RCU_shb_max_stat;
        typedef EllenBinTreePQueue< rcu_sht, typename Value::key_type, Value, traits_EllenBinTree_max_stat > EllenBinTree_RCU_sht_max_stat;
#endif

        struct traits_EllenBinTree_min :
            public cc::ellen_bintree::make_set_traits<
                cc::ellen_bintree::key_extractor< typename Value::key_extractor >
                ,cc::opt::less< std::greater<Value> >
            >::type
        {};
        typedef EllenBinTreePQueue< cds::gc::HP, typename Value::key_type, Value, traits_EllenBinTree_min, false > EllenBinTree_HP_min;
        typedef EllenBinTreePQueue< cds::gc::DHP, typename Value::key_type, Value, traits_EllenBinTree_min, false > EllenBinTree_DHP_min;
        typedef EllenBinTreePQueue< rcu_gpi, typename Value::key_type, Value, traits_EllenBinTree_min, false > EllenBinTree_RCU_gpi_min;
        typedef EllenBinTreePQueue< rcu_gpb, typename Value::key_type, Value, traits_EllenBinTree_min, false > EllenBinTree_RCU_gpb_min;
        typedef EllenBinTreePQueue< rcu_gpt, typename Value::key_type, Value, traits_EllenBinTree_min, false > EllenBinTree_RCU_gpt_min;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef EllenBinTreePQueue< rcu_shb, typename Value::key_type, Value, traits_EllenBinTree_min, false > EllenBinTree_RCU_shb_min;
        typedef EllenBinTreePQueue< rcu_sht, typename Value::key_type, Value, traits_EllenBinTree_min, false > EllenBinTree_RCU_sht_min;
#endif

        struct traits_EllenBinTree_min_stat :
            public cc::ellen_bintree::make_set_traits<
                cc::ellen_bintree::key_extractor< typename Value::key_extractor >
                ,cc::opt::less< std::greater<Value> >
                ,co::stat< cc::ellen_bintree::stat<> >
            >::type
        {};
        typedef EllenBinTreePQueue< cds::gc::HP, typename Value::key_type, Value, traits_EllenBinTree_min_stat, false > EllenBinTree_HP_min_stat;
        typedef EllenBinTreePQueue< cds::gc::DHP, typename Value::key_type, Value, traits_EllenBinTree_min_stat, false > EllenBinTree_DHP_min_stat;
        typedef EllenBinTreePQueue< rcu_gpi, typename Value::key_type, Value, traits_EllenBinTree_min_stat, false > EllenBinTree_RCU_gpi_min_stat;
        typedef EllenBinTreePQueue< rcu_gpb, typename Value::key_type, Value, traits_EllenBinTree_min_stat, false > EllenBinTree_RCU_gpb_min_stat;
        typedef EllenBinTreePQueue< rcu_gpt, typename Value::key_type, Value, traits_EllenBinTree_min_stat, false > EllenBinTree_RCU_gpt_min_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef EllenBinTreePQueue< rcu_shb, typename Value::key_type, Value, traits_EllenBinTree_min_stat, false > EllenBinTree_RCU_shb_min_stat;
        typedef EllenBinTreePQueue< rcu_sht, typename Value::key_type, Value, traits_EllenBinTree_min_stat, false > EllenBinTree_RCU_sht_min_stat;
#endif

        // Priority queue based on SkipListSet
        struct traits_SkipList_max :
            public cc::skip_list::make_traits <
            cc::opt::less < std::less<Value> >
            > ::type
        {};
        typedef SkipListPQueue< cds::gc::HP, Value, traits_SkipList_max > SkipList_HP_max;
        typedef SkipListPQueue< cds::gc::DHP, Value, traits_SkipList_max > SkipList_DHP_max;
        typedef SkipListPQueue< rcu_gpi, Value, traits_SkipList_max > SkipList_RCU_gpi_max;
        typedef SkipListPQueue< rcu_gpb, Value, traits_SkipList_max > SkipList_RCU_gpb_max;
        typedef SkipListPQueue< rcu_gpt, Value, traits_SkipList_max > SkipList_RCU_gpt_max;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListPQueue< rcu_shb, Value, traits_SkipList_max > SkipList_RCU_shb_max;
        typedef SkipListPQueue< rcu_sht, Value, traits_SkipList_max > SkipList_RCU_sht_max;
#endif

        struct traits_SkipList_max_stat :
            public cc::skip_list::make_traits<
                cc::opt::less< std::less<Value> >
                ,co::stat< cc::skip_list::stat<> >
            >::type
        {};
        typedef SkipListPQueue< cds::gc::HP, Value, traits_SkipList_max_stat > SkipList_HP_max_stat;
        typedef SkipListPQueue< cds::gc::DHP, Value, traits_SkipList_max_stat > SkipList_DHP_max_stat;
        typedef SkipListPQueue< rcu_gpi, Value, traits_SkipList_max_stat > SkipList_RCU_gpi_max_stat;
        typedef SkipListPQueue< rcu_gpb, Value, traits_SkipList_max_stat > SkipList_RCU_gpb_max_stat;
        typedef SkipListPQueue< rcu_gpt, Value, traits_SkipList_max_stat > SkipList_RCU_gpt_max_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListPQueue< rcu_shb, Value, traits_SkipList_max_stat > SkipList_RCU_shb_max_stat;
        typedef SkipListPQueue< rcu_sht, Value, traits_SkipList_max_stat > SkipList_RCU_sht_max_stat;
#endif

        struct traits_SkipList_min :
            public cc::skip_list::make_traits<
                cc::opt::less< std::greater<Value> >
            >::type
        {};
        typedef SkipListPQueue< cds::gc::HP, Value, traits_SkipList_min, false > SkipList_HP_min;
        typedef SkipListPQueue< cds::gc::DHP, Value, traits_SkipList_min, false > SkipList_DHP_min;
        typedef SkipListPQueue< rcu_gpi, Value, traits_SkipList_min, false > SkipList_RCU_gpi_min;
        typedef SkipListPQueue< rcu_gpb, Value, traits_SkipList_min, false > SkipList_RCU_gpb_min;
        typedef SkipListPQueue< rcu_gpt, Value, traits_SkipList_min, false > SkipList_RCU_gpt_min;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListPQueue< rcu_shb, Value, traits_SkipList_min, false > SkipList_RCU_shb_min;
        typedef SkipListPQueue< rcu_sht, Value, traits_SkipList_min, false > SkipList_RCU_sht_min;
#endif

        struct traits_SkipList_min_stat :
            public cc::skip_list::make_traits<
                cc::opt::less< std::greater<Value> >
                ,co::stat< cc::skip_list::stat<> >
            >::type
        {};
        typedef SkipListPQueue< cds::gc::HP, Value, traits_SkipList_min_stat, false > SkipList_HP_min_stat;
        typedef SkipListPQueue< cds::gc::DHP, Value, traits_SkipList_min_stat, false > SkipList_DHP_min_stat;
        typedef SkipListPQueue< rcu_gpi, Value, traits_SkipList_min_stat, false > SkipList_RCU_gpi_min_stat;
        typedef SkipListPQueue< rcu_gpb, Value, traits_SkipList_min_stat, false > SkipList_RCU_gpb_min_stat;
        typedef SkipListPQueue< rcu_gpt, Value, traits_SkipList_min_stat, false > SkipList_RCU_gpt_min_stat;
#ifdef CDS_URCU_SIGNAL_HANDLING_ENABLED
        typedef SkipListPQueue< rcu_shb, Value, traits_SkipList_min_stat, false > SkipList_RCU_shb_min_stat;
        typedef SkipListPQueue< rcu_sht, Value, traits_SkipList_min_stat, false > SkipList_RCU_sht_min_stat;
#endif


        // FCPriorityQueue
        struct traits_FCPQueue_stat : public
            cds::container::fcpqueue::make_traits <
            cds::opt::stat < cds::container::fcpqueue::stat<> >
            > ::type
        {};

        typedef cds::container::FCPriorityQueue< Value >    FCPQueue_vector;
        typedef cds::container::FCPriorityQueue< Value
            ,std::priority_queue<Value>
            ,traits_FCPQueue_stat
        >    FCPQueue_vector_stat;

        typedef cds::container::FCPriorityQueue< Value
            ,std::priority_queue<Value, std::deque<Value> >
        > FCPQueue_deque;
        typedef cds::container::FCPriorityQueue< Value
            ,std::priority_queue<Value, std::deque<Value> >
            ,traits_FCPQueue_stat
        > FCPQueue_deque_stat;

        typedef cds::container::FCPriorityQueue< Value
            ,std::priority_queue<Value, boost::container::deque<Value> >
        > FCPQueue_boost_deque;
        typedef cds::container::FCPriorityQueue< Value
            ,std::priority_queue<Value, boost::container::deque<Value> >
            ,traits_FCPQueue_stat
        > FCPQueue_boost_deque_stat;

        typedef cds::container::FCPriorityQueue< Value
            ,std::priority_queue<Value, boost::container::stable_vector<Value> >
        > FCPQueue_boost_stable_vector;
        typedef cds::container::FCPriorityQueue< Value
            ,std::priority_queue<Value, boost::container::stable_vector<Value> >
            ,traits_FCPQueue_stat
        > FCPQueue_boost_stable_vector_stat;

        /// Standard priority_queue
        typedef StdPQueue< Value, std::vector<Value>, cds::sync::spin> StdPQueue_vector_spin;
        typedef StdPQueue< Value, std::vector<Value>, std::mutex >  StdPQueue_vector_mutex;
        typedef StdPQueue< Value, std::deque<Value>, cds::sync::spin> StdPQueue_deque_spin;
        typedef StdPQueue< Value, std::deque<Value>,  std::mutex >  StdPQueue_deque_mutex;
    };


    template <typename Stat>
    static inline void check_statistics( Stat const& /*s*/ )
    {}

    static inline void check_statistics( cds::container::ellen_bintree::stat<> const& s )
    {
        CPPUNIT_CHECK_CURRENT( s.m_nInternalNodeCreated.get() == s.m_nInternalNodeDeleted.get() );
        CPPUNIT_CHECK_CURRENT( s.m_nUpdateDescCreated.get() == s.m_nUpdateDescDeleted.get() );
    }
}   // namespace pqueue

namespace std {

    static inline std::ostream& operator <<( std::ostream& o, cds::container::fcpqueue::empty_stat const& )
    {
        return o;
    }

    static inline std::ostream& operator <<( std::ostream& o, cds::container::fcpqueue::stat<> const& s )
    {
        return o << "\tStatistics:\n"
            << "\t              Push: " << s.m_nPush.get()         << "\n"
            << "\t         Push move: " << s.m_nPushMove.get()     << "\n"
            << "\t               Pop: " << s.m_nPop.get()          << "\n"
            << "\t        Failed pop: " << s.m_nFailedPop.get()    << "\n"
            << "\tFlat combining statistics:\n"
            << "\t        Combining factor: " << s.combining_factor()         << "\n"
            << "\t         Operation count: " << s.m_nOperationCount.get()    << "\n"
            << "\t      Combine call count: " << s.m_nCombiningCount.get()    << "\n"
            << "\t        Compact pub-list: " << s.m_nCompactPublicationList.get() << "\n"
            << "\t   Deactivate pub-record: " << s.m_nDeactivatePubRecord.get()    << "\n"
            << "\t     Activate pub-record: " << s.m_nActivatePubRecord.get() << "\n"
            << "\t       Create pub-record: " << s.m_nPubRecordCreated.get()  << "\n"
            << "\t       Delete pub-record: " << s.m_nPubRecordDeteted.get()  << "\n"
            << "\t      Acquire pub-record: " << s.m_nAcquirePubRecCount.get()<< "\n"
            << "\t      Release pub-record: " << s.m_nReleasePubRecCount.get()<< "\n";
    }

} // namespace std

#endif // #ifndef CDSUNIT_PQUEUE_TYPES_H
