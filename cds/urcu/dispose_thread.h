//$$CDS-header$$1

#ifndef CDSLIB_URCU_DISPOSE_THREAD_H
#define CDSLIB_URCU_DISPOSE_THREAD_H

#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cds/details/aligned_type.h>

namespace cds { namespace urcu {

    /// Reclamation thread for \p general_threaded and \p signal_threaded URCU
    /**
        The object of this class contains a reclamation thread object and
        necessary synchronization object(s). The object manages reclamation thread
        and defines a set of messages (i.e. methods) to communicate with the thread.

        Template argument \p Buffer defines the buffer type of \ref general_threaded (or \ref signal_threaded) URCU.
    */
    template <class Buffer>
    class dispose_thread
    {
    public:
        typedef Buffer  buffer_type ;   ///< Buffer type
    private:
        //@cond
        typedef std::thread                 thread_type;
        typedef std::mutex              mutex_type;
        typedef std::condition_variable condvar_type;
        typedef std::unique_lock< mutex_type >  unique_lock;

        class dispose_thread_starter: public thread_type
        {
            static void thread_func( dispose_thread * pThis )
            {
                pThis->execute();
            }

        public:
            dispose_thread_starter( dispose_thread * pThis )
                : thread_type( thread_func, pThis )
            {}
        };

        typedef char thread_placeholder[ sizeof(dispose_thread_starter) ];
        typename cds::details::aligned_type< thread_placeholder, alignof( dispose_thread_starter ) >::type  m_threadPlaceholder;
        dispose_thread_starter *                m_DisposeThread;

        // synchronization with disposing thread
        mutex_type      m_Mutex;
        condvar_type    m_cvDataReady;

        // Task for thread (dispose cycle)
        buffer_type * volatile  m_pBuffer;
        uint64_t volatile       m_nCurEpoch;

        // Quit flag
        bool volatile           m_bQuit;

        // disposing pass sync
        condvar_type            m_cvReady;
        bool volatile           m_bReady;
        //@endcond

    private: // methods called from disposing thread
        //@cond
        void execute()
        {
            buffer_type *   pBuffer;
            uint64_t        nCurEpoch;
            bool            bQuit = false;

            while ( !bQuit ) {
                {
                    unique_lock lock( m_Mutex );

                    // signal that we are ready to dispose
                    m_bReady = true;
                    m_cvReady.notify_one();

                    // wait new data portion
                    while ( !m_pBuffer )
                        m_cvDataReady.wait( lock );

                    // New work is ready
                    m_bReady = false ;   // we are busy

                    bQuit = m_bQuit;
                    nCurEpoch = m_nCurEpoch;
                    pBuffer = m_pBuffer;
                    m_pBuffer = nullptr;
                }

                if ( pBuffer )
                    dispose_buffer( pBuffer, nCurEpoch );
            }
        }

        void dispose_buffer( buffer_type * pBuf, uint64_t nCurEpoch )
        {
            epoch_retired_ptr p;
            while ( pBuf->pop( p ) ) {
                if ( p.m_nEpoch <= nCurEpoch )
                    p.free();
                else {
                    pBuf->push( p );
                    break;
                }
            }
        }
        //@endcond

    public:
        //@cond
        dispose_thread()
            : m_pBuffer( nullptr )
            , m_nCurEpoch(0)
            , m_bQuit( false )
            , m_bReady( false )
        {}
        //@endcond

    public: // methods called from any thread
        /// Start reclamation thread
        /**
            This function is called by \ref general_threaded object to start
            internal reclamation thread.
        */
        void start()
        {
            m_DisposeThread = new (m_threadPlaceholder) dispose_thread_starter( this );
        }

        /// Stop reclamation thread
        /**
            This function is called by \ref general_threaded object to
            start reclamation cycle and then to terminate reclamation thread.

            \p buf buffer contains retired objects ready to free.
        */
        void stop( buffer_type& buf, uint64_t nCurEpoch )
        {
            {
                unique_lock lock( m_Mutex );

                // wait while retiring pass done
                while ( !m_bReady )
                    m_cvReady.wait( lock );

                // give a new work and set stop flag
                m_pBuffer = &buf;
                m_nCurEpoch = nCurEpoch;
                m_bQuit = true;
            }
            m_cvDataReady.notify_one();

            m_DisposeThread->join();
        }

        /// Start reclamation cycle
        /**
            This function is called by \ref general_threaded object
            to notify the reclamation thread about new work.
            \p buf buffer contains retired objects ready to free.
            The reclamation thread should free all \p buf objects
            \p m_nEpoch field of which is no more than \p nCurEpoch.

            If \p bSync parameter is \p true the calling thread should
            wait until disposing done.
        */
        void dispose( buffer_type& buf, uint64_t nCurEpoch, bool bSync )
        {
            unique_lock lock( m_Mutex );

            // wait while disposing pass done
            while ( !m_bReady )
                m_cvReady.wait( lock );

            if ( bSync )
                m_bReady = false;

            // new work
            m_nCurEpoch = nCurEpoch;
            m_pBuffer = &buf;

            m_cvDataReady.notify_one();

            if ( bSync ) {
                while ( !m_bReady )
                    m_cvReady.wait( lock );
            }
        }
    };
}} // namespace cds::urcu

#endif // #ifdef CDSLIB_URCU_DISPOSE_THREAD_H
