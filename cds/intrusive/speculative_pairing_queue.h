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

#ifndef CDSLIB_INTRUSIVE_SPECULATIVE_PAIRING_QUEUE_H
#define CDSLIB_INTRUSIVE_SPECULATIVE_PAIRING_QUEUE_H

#include <cds/details/bit_reverse_counter.h>
#include <cds/intrusive/options.h>
#include <cds/opt/buffer.h>
#include <cds/opt/compare.h>
#include <cds/algo/atomic.h>
#include <cds/intrusive/details/sp_queue_base.h>

//#include <boost/random/mersenne_twister.hpp>
//#include <boost/random/uniform_int.hpp>
//#include <boost/random/variate_generator.hpp>
//#include <list>

#include <string>
#include <iostream>
#include <sstream>

namespace cds { namespace intrusive {

        /// MSPriorityQueue related definitions
        /** @ingroup cds_intrusive_helper
        */
        namespace speculative_pairing_queue {
             //@cond
            /// Slot type
            template < class GC, typename Tag = opt::none>
            using node = cds::intrusive::sp_queue::node< GC, Tag >;

            template < typename... Options >
            using base_hook = cds::intrusive::sp_queue::base_hook< Options...>;

            template <typename NodeTraits, typename... Options >
            using traits_hook = cds::intrusive::sp_queue::traits_hook< NodeTraits, Options... >;

            template < size_t MemberOffset, typename... Options >
            using member_hook = cds::intrusive::sp_queue::member_hook< MemberOffset, Options... >;

            /// MSPriorityQueue statistics
            template <typename Counter = cds::atomicity::event_counter>
            struct stat {
                typedef Counter   counter_type ; ///< Event counter type

                counter_type   m_nEnqueCount;           ///< Count of success enque operation
                counter_type   m_nQueueCreatingCount;   ///< Count of ccd reating Queue on enque
                counter_type   m_nRepeatEnqueCount;	 ///< Count of repeat iteration

                counter_type   m_nDequeCount;           ///< Count of success deque operation
                counter_type   m_nReturnEmptyInvalid;   ///< Count of EMPTY returning because of invalid queue
                counter_type   m_nClosingQueue;		 ///< Count of closing queue(made it invalid)

                //@cond
                void onEnqueSuccess()           { ++m_nEnqueCount           ;}
                void onDequeSuccess()           { ++m_nDequeCount           ;}

                void onQueueCreate()			{ ++m_nQueueCreatingCount   ;}
                void onRepeatEnque()			{ ++m_nRepeatEnqueCount	    ;}

                void onReturnEmpty()			{ ++m_nReturnEmptyInvalid   ;}
                void onCloseQueue()				{ ++m_nClosingQueue			;}
                //@endcond

                //@cond
                /*void reset()
                {
                    m_EnqueueCount.reset();
                    m_DequeueCount.reset();
                    m_EnqueueRace.reset();
                    m_DequeueRace.reset();
                    m_AdvanceTailError.reset();
                    m_BadTail.reset();
                    m_EmptyDequeue.reset();
                }

                stat& operator +=(stat const& s)
                {
                    m_EnqueueCount += s.m_EnqueueCount.get();
                    m_DequeueCount += s.m_DequeueCount.get();
                    m_EnqueueRace += s.m_EnqueueRace.get();
                    m_DequeueRace += s.m_DequeueRace.get();
                    m_AdvanceTailError += s.m_AdvanceTailError.get();
                    m_BadTail += s.m_BadTail.get();
                    m_EmptyDequeue += s.m_EmptyDequeue.get();

                    return *this;
                }*/
                //@endcond
            };

            /// MSPriorityQueue empty statistics
            struct empty_stat {
                //@cond
                void onEnqueSuccess()           const {}
                void onDequeSuccess()           const {}

                void onQueueCreate()            const {}
                void onRepeatEnque()            const {}

                void onReturnEmpty()			const {}
                void onCloseQueue()				const {}

                void reset() {}
                empty_stat& operator +=(empty_stat const&)
                {
                    return *this;
                }
                //@endcond
            };

            /// MSQueue default traits
            struct traits
            {
                typedef speculative_pairing_queue::base_hook<>        hook;
                /// Back-off strategy
                typedef cds::backoff::empty         back_off;

                /// The functor used for dispose removed items. Default is \p opt::v::empty_disposer. This option is used for dequeuing
                typedef opt::v::empty_disposer      disposer;

                /// Item counting feature; by default, disabled. Use \p cds::atomicity::item_counter to enable item counting
                typedef atomicity::empty_item_counter   item_counter;

                /// Internal statistics (by default, disabled)
                /**
                Possible option value are: \p msqueue::stat, \p msqueue::empty_stat (the default),
                user-provided class that supports \p %msqueue::stat interface.
                */
                typedef speculative_pairing_queue::empty_stat         stat;

                /// C++ memory ordering model
                /**
                Can be \p opt::v::relaxed_ordering (relaxed memory model, the default)
                or \p opt::v::sequential_consistent (sequentially consisnent memory model).
                */
                typedef opt::v::sequential_consistent	memory_model;

                /// Link checking, see \p cds::opt::link_checker
                static constexpr const opt::link_check_type link_checker = opt::debug_check_link;

                /// Padding for internal critical atomic data. Default is \p opt::cache_line_padding
                enum { padding = opt::cache_line_padding };
            };

            /// Metafunction converting option list to traits
            /**
                \p Options:
                - \p opt::buffer - the buffer type for heap array. Possible type are: \p opt::v::initialized_static_buffer, \p opt::v::initialized_dynamic_buffer.
                    Default is \p %opt::v::initialized_dynamic_buffer.
                    You may specify any type of value for the buffer since at instantiation time
                    the \p buffer::rebind member metafunction is called to change the type of values stored in the buffer.
                - \p opt::compare - priority compare functor. No default functor is provided.
                    If the option is not specified, the \p opt::less is used.
                - \p opt::less - specifies binary predicate used for priority compare. Default is \p std::less<T>.
                - \p opt::lock_type - lock type. Default is \p cds::sync::spin
                - \p opt::back_off - back-off strategy. Default is \p cds::backoff::yield
                - \p opt::stat - internal statistics. Available types: \p mspriority_queue::stat, \p mspriority_queue::empty_stat (the default, no overhead)
            */
            template <typename... Options>
            struct make_traits {
#   ifdef CDS_DOXYGEN_INVOKED
                typedef implementation_defined type ;   ///< Metafunction result
#   else
                typedef typename cds::opt::make_options<
                        typename cds::opt::find_type_traits< traits, Options... >::type
                        ,Options...
                >::type   type;
#   endif
            };

        }   // namespace mspriority_queue

        /// Michael & Scott array-based lock-based concurrent priority queue heap
        /** @ingroup cds_intrusive_priority_queue
            Source:
                - [2013] Henzinger,Payer,Sezgin "
                    Replacing competition with cooperation to achieve scalable lock-free FIFO queues"

            Template parameters:
            - \p T - type to be stored in the queue. The priority is a part of \p T type.
            - \p Traits - type traits. See \p speculative_pairing_queue::traits for explanation.
                It is possible to declare option-based queue with \p cds::container::speculative_pairing_queue::make_traits
                metafunction instead of \p Traits template argument.
        */
        template <typename GC, typename T, class Traits = speculative_pairing_queue::traits >
        class SPQueue
        {
        public:
            typedef GC			gc			;   ///< Garbage collector
            typedef T           value_type  ;   ///< Value type stored in the queue
            typedef Traits      traits      ;   ///< Traits template parameter

            typedef typename traits::hook hook;
            typedef typename hook::node_type    node_type;  ///< node type
            typedef typename get_node_traits< value_type, node_type, hook>::type node_traits;   ///< node traits
            typedef typename sp_queue::get_link_checker< node_type, traits::link_checker >::type link_checker;   ///< link checker
            typedef typename traits::back_off   back_off;       ///< back-off strategy
            typedef typename traits::item_counter item_counter; ///< Item counter class
            typedef typename traits::stat       stat;           ///< Internal statistics
            typedef typename traits::memory_model memory_model; ///< Memory ordering. See \p cds::opt::memory_model option
            typedef typename traits::disposer   disposer;   ///< disposer used

            /// Rebind template arguments
            template <typename GC2, typename T2, typename Traits2>
            struct rebind {
                typedef SPQueue< GC2, T2, Traits2 > other;   ///< Rebinding result
            };

            static constexpr const size_t c_nHazardPtrCount = 5; ///< Count of hazard pointer required for the algorithm
        protected:
            typedef typename node_type::atomic_node_ptr atomic_node_ptr;
			static_assert((std::is_same<gc, typename node_type::gc>::value),"GC and node_type::gc must be the same");
			
			
            //@cond
            /// Slot type
            typedef struct SlotType {
                atomic_node_ptr m_pHead;
                atomic_node_ptr m_pLast;
                atomic_node_ptr m_pRemoved;
				
				SlotType(){
					m_pHead.store(nullptr, memory_model::memory_order_release);
					m_pLast.store(nullptr, memory_model::memory_order_release);
					m_pRemoved.store(nullptr, memory_model::memory_order_release);
				}
            } Slot;
            //@endcond
            const static size_t C_SIZE   = 10; ///< size
			int probStale = 0; //< propability of dispose stale nodes
            //@cond
            /// Queue type
            typedef struct QueueType {
				typedef typename gc::template atomic_type<size_t> atomic_int;
				typedef typename gc::template atomic_type<bool> atomic_bool;
				
                atomic_bool  m_Invalid;
                atomic_int  m_Cntdeq;
                atomic_int  m_Tail;
                Slot m_pair[10];
				
				QueueType(){
					m_Invalid.store(false, memory_model::memory_order_release);
					m_Tail.store(0, memory_model::memory_order_release);
					m_Cntdeq.store(0, memory_model::memory_order_release);
				}
				
				QueueType(int tail){
					m_Invalid.store(false, memory_model::memory_order_release);
					m_Tail.store(tail, memory_model::memory_order_release);
					m_Cntdeq.store(0, memory_model::memory_order_release);
				}
            } Queue;
            //@endcond
			
			typedef typename gc::template atomic_ref<Queue> atomic_queue_ptr;
			
			item_counter        			m_ItemCounter   ;   ///< Item counter
            stat               			    m_Stat          ;   ///< internal statistics accumulator
            atomic_queue_ptr			    m_Queue			;	///< Global queue

            static node_type* PICKET;


            
            static void clear_links( node_type * pNode )
            {
                pNode->m_pNext.store( nullptr, memory_model::memory_order_release );
				pNode->m_nVer.store(0, memory_model::memory_order_release);
				pNode->m_removed = false;
            }

			struct disposer_node_thunk {
                    void operator()( value_type * p ) const
                    {
                        assert( p != nullptr );
                        SPQueue::clear_links( node_traits::to_node_ptr( p ));
                        disposer()(p);
                    }
                };
				
            static void dispose_node( node_type * p )
            {			
				//retire all except PICKET
                if (p != PICKET)
                {
                    gc::template retire<disposer_node_thunk>( node_traits::to_value_ptr( p ));
                }
            }


            static void dispose_queue( Queue* queue)
            {
				
				struct disposer_thunk {
                    void operator()( Queue* queue ) const
                    {
						assert(queue != nullptr);
							
						node_type* current_node;
						for (int i = 0; i < C_SIZE; i++) 
						{	
							current_node = queue->m_pair[i].m_pHead.load(memory_model::memory_order_acquire);	
							while (current_node != nullptr) 
							{
								node_type* next_node =  current_node->m_pNext.load(memory_model::memory_order_acquire);
								if (current_node != PICKET)
									disposer_node_thunk()(node_traits::to_value_ptr( current_node ));
								current_node = next_node;
							}
						}
						//there must be queue disposer, buuuuuut...	
						delete queue;
                    }
                };

				gc::template retire<disposer_thunk>(queue);
            }
			
/*			void dispose_stale_nodes( Slot &slot){
				boost::mt19937 gen;
				boost::uniform_int<> dist(0,99);
				boost::variate_generator<boost::mt19937&, boost::uniform_int<> > roll(gen, dist);
				
				int random_int = roll();
				//EXPECT_TRUE(false) << "lol";
				if (random_int < probStale)
				{	
					
					typename gc::template Guard node_guard;
					typename std::list<node_type*> stale_nodes;
					node_type* head = node_guard.protect(slot.m_pHead);
					node_type* curr = head;
					node_type* last = curr;
					//EXPECT_TRUE(false) << "head" << node_traits::to_value_ptr(head->m_pNext)->nVal;
					
					while (curr != nullptr && curr->m_removed){
						//if (current_node != PICKET)
						stale_nodes.push_back(curr);
						curr = node_guard.protect(curr->m_pNext);
					}	
					
					if (stale_nodes.size() > 0)
					{
						if (slot.m_pHead.compare_exchange_strong(head, 
																 last, 
																 memory_model::memory_order_relaxed, 
																 memory_model::memory_order_relaxed)){
							stale_nodes.pop_back();
							
							for (typename std::list<node_type*>::iterator it = stale_nodes.begin(); it != stale_nodes.end(); it++){
								//EXPECT_TRUE(false) << "Dis stale" << node_traits::to_value_ptr(*it)->nVal;
								dispose_node(*it);
							}
						}
					}
					
				}
			}
*/
			
        public:
            /// Constructs empty speculative pairing queue
            /**
            */
			void changeProbStale(int prob)
			{
				probStale = prob;
			}
			
            SPQueue()
            {
                m_Queue.store(new Queue, memory_model::memory_order_release);
                PICKET->m_nVer.store(-1, memory_model::memory_order_release);		
            }

            /// Clears priority queue and destructs the object
            ~SPQueue()
            {
                clear();
				//delete PICKET;
            }

            /// Inserts a item into priority queue
            /**

            */
            bool enqueue( value_type& val )
            {
                node_type* DUMMY = nullptr;
                Queue* pQueue;
				typename gc::template Guard queue_guard;
                typename gc::template GuardArray<3>  guards;
                size_t tail;

                while (true){
                    tail = 0;
                    pQueue = queue_guard.protect(m_Queue);
                    if (pQueue->m_Invalid.load(memory_model::memory_order_acquire)) {
                        Queue* pNewQueue = createNewQueue(val);
                        if (m_Queue.compare_exchange_strong(pQueue, pNewQueue, memory_model::memory_order_seq_cst,memory_model::memory_order_seq_cst)) {
							dispose_queue(pQueue);
						    m_Stat.onQueueCreate();
                            ++m_ItemCounter;
                            return true;
                        } else {
                            delete pNewQueue;
                        }

                        m_Stat.onRepeatEnque();
                        continue;
                    }

                    tail = pQueue->m_Tail.load(memory_model::memory_order_seq_cst);

                    size_t idx = tail % C_SIZE;
					
					guards.protect(0, pQueue->m_pair[idx].m_pLast);
					guards.protect(1, pQueue->m_pair[idx].m_pHead);
					
                    node_type* pNode = guards.protect(2, pQueue->m_pair[idx].m_pLast, [](node_type * p) -> value_type * {return node_traits::to_value_ptr(p);});

                    //std::string s = " tail = " + std::to_string(tail) + " idx = " + std::to_string(idx) + "\n";
                    if (tail == idx) {
                        //s += "\t !tail = " + std::to_string(tail) + " !idx = " + std::to_string(idx) + "\n";
                        //std::cerr << s;
                        if (pNode == nullptr) {

                            node_type* pNewNode = node_traits::to_node_ptr( val );
							pNewNode->m_nVer.store(tail, memory_model::memory_order_release);
                            std::string s = " tail_before = " + std::to_string(tail);
                            //std::cerr << s;
                            //std::cerr << pQueue->m_pair[idx].m_pHead.load(memory_model::memory_order_seq_cst) << " \n";
                            const void * address = static_cast<const void*>(pQueue->m_pair[idx].m_pHead.load(memory_model::memory_order_seq_cst));
                            std::stringstream ss;

                            ss << address;
                            s += " Head = " + ss.str();
                            if (pQueue->m_pair[idx].m_pHead.compare_exchange_strong(DUMMY, pNewNode, memory_model::memory_order_seq_cst, memory_model::memory_order_seq_cst)) {
                                s += " tail_after_1 = " + std::to_string(tail) + "\n";
                                std::cerr << s;
								pQueue->m_pair[idx].m_pLast.store(pNewNode, memory_model::memory_order_release);
                                break;
                            }
                            else {
                                s += " tail_after_2 = " + std::to_string(tail) + "\n";
                                std::cerr << s;
                                if (pQueue->m_pair[idx].m_pHead.load(memory_model::memory_order_acquire) == PICKET) {
                                    pQueue->m_Invalid.store(true, memory_model::memory_order_release);
                                } else {

                                    pQueue->m_Tail.compare_exchange_strong(tail,
                                                                           tail + 1,
                                                                           memory_model::memory_order_seq_cst,
                                                                           memory_model::memory_order_seq_cst);
                                }
                                m_Stat.onRepeatEnque();
                                continue;
                            }
                        }
                        else {
                            std::string s = " !!tail_before = " + std::to_string(tail);
                            std::cerr << s;
                            if (pNode == PICKET) {
                                pQueue->m_Invalid.store(true, memory_model::memory_order_release);
                            } else {
                                pQueue->m_Tail.compare_exchange_strong(tail,
                                                                       tail + 1,
                                                                       memory_model::memory_order_seq_cst,
                                                                       memory_model::memory_order_seq_cst);
                            }
							m_Stat.onRepeatEnque();
                            continue;
                        }
                    }

                    if (pNode == nullptr)
					{
						//std::string out = "tail = " + std::to_string(node_traits::to_node_ptr( val )->m_nVer) + "\n";
						//std::cerr << out;
						
                        pNode = guards.protect(2, pQueue->m_pair[idx].m_pHead, [](node_type * p) -> value_type * {return node_traits::to_value_ptr(p);});
						//if (pNode == nullptr)
						//	continue;
					}
                    if (pNode == PICKET) {
                        Queue* pNewQueue = createNewQueue(val);
                        if (m_Queue.compare_exchange_strong(pQueue, pNewQueue,memory_model::memory_order_seq_cst,memory_model::memory_order_acquire))
                        {
                            dispose_queue(pQueue);
                            m_Stat.onQueueCreate();
                            ++m_ItemCounter;
                            return true;
                        } else {
                            delete pNewQueue;
                        }

                        m_Stat.onRepeatEnque();
                        continue;
                    }

                    while (pNode->m_pNext.load(memory_model::memory_order_acquire) != nullptr
                           && pNode->m_nVer.load(memory_model::memory_order_acquire) < tail)
                        pNode = guards.protect(2, pNode->m_pNext, [](node_type * p) -> value_type * {return node_traits::to_value_ptr(p);});

                    if (pNode->m_nVer.load(memory_model::memory_order_acquire) >= tail) {
                        pQueue->m_Tail.compare_exchange_weak(tail, tail + 1,memory_model::memory_order_seq_cst,memory_model::memory_order_acquire);
                        m_Stat.onRepeatEnque();
                        continue;
                    }
					
                    if (pNode != PICKET) {
                        node_type* pNewNode = node_traits::to_node_ptr(val);
						pNewNode->m_nVer.store(tail, memory_model::memory_order_release);
                        if (pNode->m_pNext.compare_exchange_strong(DUMMY, pNewNode,memory_model::memory_order_seq_cst,memory_model::memory_order_acquire)) {
                            pQueue->m_pair[idx].m_pLast.store(pNewNode, memory_model::memory_order_release);
                            break;
                        }
                    }
                    else {
                        pQueue->m_Invalid.store(true, memory_model::memory_order_release);
                    }
                }
                pQueue->m_Tail.compare_exchange_strong(tail, tail + 1,memory_model::memory_order_seq_cst,memory_model::memory_order_seq_cst);
                ++m_ItemCounter;
                m_Stat.onEnqueSuccess();
                return true;
            }

            /// Extracts item with high priority
            /**

            */
            value_type* dequeue()
            {
                node_type* DUMMY = nullptr;
                typename gc::template Guard queue_guard;
				typename gc::template GuardArray<3>  guards;
                
				Queue* pQueue = queue_guard.protect(m_Queue);
                if (pQueue->m_Invalid.load(memory_model::memory_order_acquire)) {
                    m_Stat.onReturnEmpty();
                    return nullptr;
                }
                 
                size_t ticket = pQueue->m_Cntdeq.fetch_add(1, memory_model::memory_order_seq_cst);
                int idx = ticket % C_SIZE;
				guards.protect(0, pQueue->m_pair[idx].m_pRemoved);
				guards.protect(1, pQueue->m_pair[idx].m_pHead);
				
				//dispose_stale_nodes(pQueue->m_pair[idx]);
				
				//guards.protect(1, pQueue->m_pair[idx].m_pHead);
				
                if (ticket >= pQueue->m_Tail.load(memory_model::memory_order_acquire) && ticket == idx) {//Error in article. may be must be >=
					if (pQueue->m_pair[idx].m_pHead.compare_exchange_strong(DUMMY, PICKET, memory_model::memory_order_seq_cst,memory_model::memory_order_acquire)) {
						CloseQueue(pQueue, idx);
                        m_Stat.onCloseQueue();
                        return nullptr;
                    }
                }
				
                node_type* pNode = guards.protect(2, pQueue->m_pair[idx].m_pRemoved, [](node_type * p) -> value_type * {return node_traits::to_value_ptr(p);});
                if (pNode == nullptr)
                    pNode = guards.protect(2, pQueue->m_pair[idx].m_pHead, [](node_type * p) -> value_type * {return node_traits::to_value_ptr(p);});
				
                if (pNode == PICKET) {
                    CloseQueue(pQueue, idx);
                    m_Stat.onCloseQueue();
                    return nullptr;
                }

                if (pNode->m_nVer.load(memory_model::memory_order_acquire) > ticket)
                    pNode = guards.protect(2, pQueue->m_pair[idx].m_pHead, [](node_type * p) -> value_type * {return node_traits::to_value_ptr(p);});
				
                while (pNode->m_nVer.load(memory_model::memory_order_acquire) < ticket) {
                    if (pNode->m_pNext.load(memory_model::memory_order_acquire) == nullptr) {
                        if (pNode->m_pNext.compare_exchange_strong(DUMMY, PICKET, memory_model::memory_order_seq_cst, memory_model::memory_order_acquire)) {
                            CloseQueue(pQueue, idx);
                            m_Stat.onCloseQueue();
                            return nullptr;
                        }
                    }
                    pNode = guards.protect(2, pNode->m_pNext, [](node_type * p) -> value_type * {return node_traits::to_value_ptr(p);});
                    if (pNode == PICKET) {
                        CloseQueue(pQueue, idx);
                        m_Stat.onCloseQueue();
                        return nullptr;
                    }
                }
				
                value_type* x = node_traits::to_value_ptr(pNode);
                pQueue->m_pair[idx].m_pRemoved.store(pNode, memory_model::memory_order_release);
				pNode->m_removed = true;
                --m_ItemCounter;
                m_Stat.onDequeSuccess();
                return x;
            }

            bool push(value_type& val){
                return enqueue(val);
            }

            value_type* pop(){
                return dequeue();
            }
            /// Clears the queue (not atomic)
            /**

            */
            void clear()
            {
				 while ( dequeue() != nullptr);
                 dispose_queue(m_Queue.load(memory_model::memory_order_acquire));
				 m_Queue.store(new Queue(), memory_model::memory_order_release);
            }
			
            /// Checks is the priority queue is empty
            bool empty() const
            {
				Queue* pQueue = m_Queue.load(memory_model::memory_order_acquire);
				return pQueue->m_Tail.load(memory_model::memory_order_acquire) <=
                       pQueue->m_Cntdeq.load(memory_model::memory_order_acquire);
            }

            /// Returns current size of priority queue
            size_t size() const
            {
				return m_ItemCounter.value();
            }

            /// Returns const reference to internal statistics
            stat const& statistics() const
            {
                return m_Stat;
            }

        protected:
            Queue* createNewQueue(value_type& x) {
				Queue* queue = new Queue(1);
				
                node_type* pNewNode = node_traits::to_node_ptr(x);
				pNewNode->m_nVer.store(0, memory_model::memory_order_release);
                queue->m_pair[0].m_pHead.store(pNewNode, memory_model::memory_order_release);
                queue->m_pair[0].m_pLast.store(pNewNode, memory_model::memory_order_release);
				
				return queue;
			}

            void CloseQueue(Queue* q, int idx) {
                q->m_Invalid.store(true, memory_model::memory_order_release);
                q->m_pair[idx].m_pRemoved.store(PICKET, memory_model::memory_order_release);
            }
        };
		
		template <typename GC, typename T, class Traits>
		typename SPQueue<GC, T, Traits>::node_type* SPQueue<GC, T, Traits>::PICKET = new SPQueue<GC, T, Traits>::node_type();
    }} // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_MSPRIORITY_QUEUE_H
