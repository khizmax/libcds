#ifndef CDSLIB_ALGO_FLAT_COMBINING_WAIT_STRATEGY_H
#define CDSLIB_ALGO_FLAT_COMBINING_WAIT_STRATEGY_H

#include <iostream>

#include <boost/thread.hpp>
#include <cds/algo/backoff_strategy.h>

namespace cds {  namespace algo {  namespace flat_combining {
	/// Waiting strategy for flat combining

    // Special values of publication_record::nRequest
    enum request_value
    {
        req_EmptyRecord,    ///< Publication record is empty
        req_Response,       ///< Operation is done

        req_Operation       ///< First operation id for derived classes
    };

    /// publication_record state
    enum record_state {
        inactive,       ///< Record is inactive
        active,         ///< Record is active
        removed         ///< Record should be removed
    };

    /// do-nothing strategy
    template<typename UserPublicationRecord, typename Traits>
    struct BareWaitStartegy
    {
        struct ExtendedPublicationRecord: public UserPublicationRecord
        {
        };

        void wait(ExtendedPublicationRecord * pRec){}
        void notify(ExtendedPublicationRecord* pRec){
            pRec->nRequest.store( req_Response, Traits::memory_model::memory_order_release);
        }
    };

    /// Wait/notify strategy based on thread-local mutex and thread-local condition variable
    template<typename UserPublicationRecord, typename Traits>
    struct WaitStartegyMultMutexMultCondVar
    {
        struct ExtendedPublicationRecord: public UserPublicationRecord
        {
            boost::mutex                _waitMutex;
            boost::condition_variable   _condVar;
        };

        void wait(ExtendedPublicationRecord * pRec){
            boost::unique_lock<boost::mutex> lock(pRec->_waitMutex);
            if (pRec->nRequest.load( Traits::memory_model::memory_order_acquire ) >= req_Operation)
				pRec->_condVar.wait(lock);
        }

        void notify(ExtendedPublicationRecord* pRec){
            boost::unique_lock<boost::mutex> lock(pRec->_waitMutex);
            pRec->nRequest.store( req_Response, Traits::memory_model::memory_order_release);
            pRec->_condVar.notify_one();
        }
    };

    /// Back-off strategy
    /**
     * When N threads compete for the critical resource that can be accessed with the help of CAS-operations,
     * only one of them gets an access. Other N–1 threads interrupt each other and consume process time in vain.
     */
    template<typename UserPublicationRecord, typename Traits>
	struct WaitBakkOffStrategy
	{
        struct ExtendedPublicationRecord: public UserPublicationRecord
        {
        };
        void wait(ExtendedPublicationRecord * pRec){
			      cds::backoff::delay_of<2>   back_off;
            back_off();
        }
        void notify(ExtendedPublicationRecord* pRec){
            pRec->nRequest.store( req_Response, Traits::memory_model::memory_order_release);
        }
    };

    /// Wait/notify strategy based on the global mutex and the condition variable
    /**
     *  The strategy is based on the usage of synchronization primitives of
     *  the FC core which are shared by all threads.
     */
    template<typename UserPublicationRecord, typename Traits>
    struct WaitOneMutexOneCondVarStrategy
	{
        boost::mutex              _globalMutex;
        boost::condition_variable _globalCondVar;

        struct ExtendedPublicationRecord: public UserPublicationRecord
        {
        };
        void wait(ExtendedPublicationRecord * pRec){
            boost::unique_lock<boost::mutex> lock(_globalMutex);
            if (pRec->nRequest.load( Traits::memory_model::memory_order_acquire ) >= req_Operation)
				//_globalCondVar.timed_wait(lock, static_cast<boost::posix_time::seconds>(1));
				_globalCondVar.wait(lock);
        }

        void notify(ExtendedPublicationRecord* pRec){
            boost::unique_lock<boost::mutex> lock(_globalMutex);
            pRec->nRequest.store( req_Response, Traits::memory_model::memory_order_release);
            _globalCondVar.notify_all();
        }
    };

    /// Wait/notify strategy based on the global mutex and the thread-local condition variable
    /**
     * It uses one extra mutex aggregated in the FC core and a condition variable for every
     * thread aggregated in thread's publication record in the publication list
     */
	template<typename UserPublicationRecord, typename Traits>
	struct WaitStratygyBaseOnSingleMutexMutlLocalCondVars
	{
		boost::mutex              _globalMutex;

		struct ExtendedPublicationRecord : public UserPublicationRecord
		{
			boost::condition_variable _globalCondVar;
		};
		void wait(ExtendedPublicationRecord * pRec){
			boost::unique_lock<boost::mutex> lock(_globalMutex);
			if (pRec->nRequest.load(Traits::memory_model::memory_order_acquire) >= req_Operation)
				//_globalCondVar.timed_wait(lock, static_cast<boost::posix_time::seconds>(1));
				pRec->_globalCondVar.wait(lock);
		}

		void notify(ExtendedPublicationRecord* pRec){
			boost::unique_lock<boost::mutex> lock(_globalMutex);
			pRec->nRequest.store(req_Response, Traits::memory_model::memory_order_release);
			pRec->_globalCondVar.notify_one();
		}
	};

	//===================================================================
    template<typename UserPublicationRecord, typename Traits>
    struct TimedWaitMultMutexMultCondVar
    {
        struct ExtendedPublicationRecord: public UserPublicationRecord
        {
            boost::mutex                _waitMutex;
            boost::condition_variable   _condVar;
        };

        void wait(ExtendedPublicationRecord * pRec){
            boost::unique_lock<boost::mutex> lock(pRec->_waitMutex);
            if (pRec->nRequest.load( Traits::memory_model::memory_order_acquire ) >= req_Operation)
                pRec->_condVar.timed_wait(lock, boost::posix_time::millisec(2));
        }

        void notify(ExtendedPublicationRecord* pRec){
            pRec->nRequest.store(req_Response, Traits::memory_model::memory_order_release);
            pRec->_condVar.notify_one();
        }
    };
    //===================================================================
    template<typename UserPublicationRecord, typename Traits>
    struct TimedWaitGlobalMutexAndCondVar
    {
        boost::mutex              _globalMutex;
        boost::condition_variable _globalCondVar;

        struct ExtendedPublicationRecord: public UserPublicationRecord
        {
        };
        void wait(ExtendedPublicationRecord * pRec){
            boost::unique_lock<boost::mutex> lock(_globalMutex);
            if (pRec->nRequest.load( Traits::memory_model::memory_order_acquire ) >= req_Operation)
                _globalCondVar.timed_wait(lock, boost::posix_time::millisec(2));
        }

        void notify(ExtendedPublicationRecord* pRec){
            pRec->nRequest.store( req_Response, Traits::memory_model::memory_order_release);
            _globalCondVar.notify_all();
        }
    };
    //===================================================================
    template <int v>
    struct Int2Type{
        enum {value = v};
    };

    ///Adaptive strategy
    /**
     * It works like “back-off”-strategy with “light” elements with small size
     * and like Wait/notify strategy based on thread-local mutex and thread-local condition variable
     * with “heavy” elements.
     */
    template<typename UserPublicationRecord, typename Traits>
    struct AutoWaitStrategy
    {
        struct ExtendedPublicationRecord: public UserPublicationRecord
        {
            boost::mutex                _waitMutex;
            boost::condition_variable   _condVar;
        };

        void wait(ExtendedPublicationRecord * pRec){
            doWait(pRec, Int2Type<sizeof(typename UserPublicationRecord::value_type) <= 4*sizeof(int) >());
        }

        void notify(ExtendedPublicationRecord* pRec){
            doNotify(pRec, Int2Type<sizeof(typename UserPublicationRecord::value_type) <= 4*sizeof(int) >());
        }

     private:
        //The container consists a small data
        void doWait(ExtendedPublicationRecord * pRec, Int2Type<true>){
            cds::backoff::delay_of<2>   back_off;
            back_off();
        }

        void doNotify(ExtendedPublicationRecord * pRec, Int2Type<true>){
            pRec->nRequest.store( req_Response, Traits::memory_model::memory_order_release);
        }

        //The container consists a big data
        void doWait(ExtendedPublicationRecord * pRec, Int2Type<false>){
            boost::unique_lock<boost::mutex> lock(pRec->_waitMutex);
            if (pRec->nRequest.load( Traits::memory_model::memory_order_acquire ) >= req_Operation)
                pRec->_condVar.timed_wait(lock, boost::posix_time::millisec(2));
        }

        void doNotify(ExtendedPublicationRecord * pRec, Int2Type<false>){
            pRec->nRequest.store(req_Response, Traits::memory_model::memory_order_release);
            pRec->_condVar.notify_one();
        }
    };//class AutoWaitStrategy
}}}//end namespace cds::algo::flat_combining

#endif //CDSLIB_ALGO_FLAT_COMBINING_WAIT_STRATEGY_H
