#ifndef CDSLIB_ALGO_FLAT_COMBINING_WAIT_STRATEGY_H
#define CDSLIB_ALGO_FLAT_COMBINING_WAIT_STRATEGY_H

#include <boost/thread.hpp>
#include <cds/algo/backoff_strategy.h>
//#include <cds/algo/flat_combining.h>

namespace cds {  namespace algo {  namespace flat_combining {

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

    template<typename UserPublicationRecord, typename Traits>
    struct DefautlWaitStartegy
    {
        struct ExtendedPublicationRecord: public UserPublicationRecord
        {
        };

        void wait(ExtendedPublicationRecord * pRec){}
        void notify(ExtendedPublicationRecord* pRec){
            pRec->nRequest.store( req_Response, Traits::memory_model::memory_order_release);
        }
    };
    //===================================================================
    template<typename UserPublicationRecord, typename Traits>
    struct WaitStartegyBasedOnSingleLocalMutexAndCondVar
    {
        struct ExtendedPublicationRecord: public UserPublicationRecord
        {
            boost::mutex                _waitMutex;
            boost::condition_variable   _condVar;
        };

        void wait(ExtendedPublicationRecord * pRec){
            boost::unique_lock<boost::mutex> lock(pRec->_waitMutex);
            if (pRec->nRequest.load( Traits::memory_model::memory_order_acquire ) >= req_Operation)
                //pRec->_condVar.timed_wait(lock, static_cast<boost::posix_time::seconds>(1));
				pRec->_condVar.wait(lock);
        }

        void notify(ExtendedPublicationRecord* pRec){
            boost::unique_lock<boost::mutex> lock(pRec->_waitMutex);
            pRec->nRequest.store( req_Response, Traits::memory_model::memory_order_release);
            pRec->_condVar.notify_one();
        }
    };

    //====================================================================
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
    //====================================================================
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
				pRec->_condVar.wait(lock);
		}

		void notify(ExtendedPublicationRecord* pRec){
			boost::unique_lock<boost::mutex> lock(_globalMutex);
			pRec->nRequest.store(req_Response, Traits::memory_model::memory_order_release);
			pRec->_condVar.notify_one();
		}
	};
}}}//end namespace cds::algo::flat_combining

#endif //CDSLIB_ALGO_FLAT_COMBINING_WAIT_STRATEGY_H
