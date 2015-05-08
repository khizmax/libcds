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
    void notify(ExtendedPublicationRecord* pRec){}
};
//===================================================================
template<typename UserPublicationRecord, typename Traits>
struct WaitStartegyBasedOnSingleLocalMutexAndCondVar
{
	struct ExtendedPublicationRecord: public UserPublicationRecord
	{
		boost::mutex _waitMutex;
		boost::condition_variable _condVar;
	};

    void wait(ExtendedPublicationRecord * pRec){
        boost::unique_lock<boost::mutex> lock(pRec->_waitMutex);
    	if (pRec->nRequest.load( Traits::memory_model::memory_order_acquire ) >= req_Operation)
    		pRec->_condVar.timed_wait(lock, static_cast<boost::posix_time::seconds>(1));
    }

    void notify(ExtendedPublicationRecord* pRec){
    	boost::unique_lock<boost::mutex> lock(pRec->_waitMutex);
    	pRec->nRequest.store( req_Response, Traits::memory_model::memory_order_acquire);
    	pRec->_condVar.notify_one();
    }
};

//==================================================================
    class publication_record;
    template<typename PublicationRecord>
    class WaitEmptyStrategy{
        typedef PublicationRecord   publication_record_type;   ///< publication record type
    public:
        void wait(publication_record_type * pRec){}
        void notify(publication_record& rec){}
    };
//====================================================================
    template<typename PublicationRecord>
    class WaitBakkOffStrategy{
        typedef PublicationRecord   publication_record_type;   ///< publication record type
        cds::backoff::delay_of<2>   back_off;   ///< Back-off strategy
    public:
        void wait(publication_record_type * pRec){
            back_off();
        }
        void notify(publication_record& rec){}
    };
//====================================================================
    template<typename PublicationRecord>
    class WaitOneMutexOneCondVarStrategy{
        typedef PublicationRecord   publication_record_type;   ///< publication record type
        boost::mutex              _globalMutex;
        boost::condition_variable _globalCondVar;
    public:
        void wait(publication_record_type * pRec){
            boost::unique_lock<boost::mutex> lock(_globalMutex);
            _globalCondVar.timed_wait(lock, static_cast<boost::posix_time::seconds>(5));
        }

        void notify(publication_record& rec){
            _globalCondVar.notify_all();
        }
    };
//====================================================================
/*    template<typename PublicationRecord>
    class WaitMultMutexMultCondVarStrategy{
        typedef PublicationRecord   publication_record_type;   ///< publication record type
    public:
        void wait(publication_record_type * pRec){
            boost::unique_lock<boost::mutex> lock(pRec->_waitMutex);
            pRec->_condVar.wait(lock);
        }

        void notify(publication_record& rec){
            rec._condVar.notify_one();
        }
    };
*/
}}}//end namespace cds::algo::flat_combining

#endif //CDSLIB_ALGO_FLAT_COMBINING_WAIT_STRATEGY_H
