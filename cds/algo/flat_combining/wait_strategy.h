// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_ALGO_FLAT_COMBINING_WAIT_STRATEGY_H
#define CDSLIB_ALGO_FLAT_COMBINING_WAIT_STRATEGY_H

#include <cds/algo/flat_combining/defs.h>
#include <cds/algo/backoff_strategy.h>
#include <mutex>
#include <condition_variable>
#include <boost/thread/tss.hpp>  // thread_specific_ptr


namespace cds { namespace opt {

    /// Wait strategy option for \p flat_combining::kernel
    template <typename Strategy>
    struct wait_strategy {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef Strategy wait_strategy;
        };
        //@endcond
    };

}} // namespace cds::opt

namespace cds { namespace algo { namespace flat_combining {

    /// Wait strategies for \p flat_combining technique
    /**
        Wait strategy specifies how a thread waits until its request is performed by the combiner.
        See \p wait_strategy::empty wait strategy to explain the interface.
    */
    namespace wait_strategy {

        /// Empty wait strategy
        /**
            Empty wait strategy is just spinning on request field.
            All functions are empty.
        */
        struct empty
        {
            /// Metafunction for defining a publication record for flat combining technique
            /**
                Any wait strategy may expand the publication record for storing
                its own private data.
                \p PublicationRecord is the type specified by \p flat_combining::kernel.
                - If the strategy has no thread-private data, it should typedef \p PublicationRecord
                  as a return \p type of metafunction.
                - Otherwise, if the strategy wants to store anything in thread-local data,
                  it should expand \p PublicationRecord, for example:
                  \code
                  template <typename PublicationRecord>
                  struct make_publication_record {
                    struct type: public PublicationRecord
                    {
                        int strategy_data;
                    };
                  };
                  \endcode
            */
            template <typename PublicationRecord>
            struct make_publication_record {
                typedef PublicationRecord type; ///< Metafunction result
            };

            /// Prepares the strategy
            /**
                This function is called before enter to waiting cycle.
                Some strategies need to prepare its thread-local data in \p rec.

                \p PublicationRecord is thread's publication record of type \p make_publication_record::type
            */
            template <typename PublicationRecord>
            void prepare( PublicationRecord& rec )
            {
                CDS_UNUSED( rec );
            }

            /// Waits for the combiner
            /**
                The thread calls this function to wait for the combiner process
                the request.
                The function returns \p true if the thread was waked up by the combiner,
                otherwise it should return \p false.

                \p FCKernel is a \p flat_combining::kernel object,
                \p PublicationRecord is thread's publication record of type \p make_publication_record::type
            */
            template <typename FCKernel, typename PublicationRecord>
            bool wait( FCKernel& fc, PublicationRecord& rec )
            {
                CDS_UNUSED( fc );
                CDS_UNUSED( rec );
                return false;
            }

            /// Wakes up the thread
            /**
                The combiner calls \p %notify() when it has been processed the request.

                \p FCKernel is a \p flat_combining::kernel object,
                \p PublicationRecord is thread's publication record of type \p make_publication_record::type
            */
            template <typename FCKernel, typename PublicationRecord>
            void notify( FCKernel& fc, PublicationRecord& rec )
            {
                CDS_UNUSED( fc );
                CDS_UNUSED( rec );
            }

            /// Moves control to other thread
            /**
                This function is called when the thread becomes the combiner
                but the request of the thread is already processed.
                The strategy may call \p fc.wakeup_any() instructs the kernel
                to wake up any pending thread.

                \p FCKernel is a \p flat_combining::kernel object,
            */
            template <typename FCKernel>
            void wakeup( FCKernel& fc )
            {
                CDS_UNUSED( fc );
            }
        };

        /// Back-off wait strategy
        /**
            Template argument \p Backoff specifies back-off strategy, default is cds::backoff::delay_of<2>
        */
        template <typename BackOff = cds::backoff::delay_of<2>>
        struct backoff
        {
            typedef BackOff back_off;   ///< Back-off strategy

            /// Incorporates back-off strategy into publication record
            template <typename PublicationRecord>
            struct make_publication_record
            {
                //@cond
                struct type: public PublicationRecord
                {
                    back_off bkoff;
                };
                //@endcond
            };

            /// Resets back-off strategy in \p rec
            template <typename PublicationRecord>
            void prepare( PublicationRecord& rec )
            {
                rec.bkoff.reset();
            }

            /// Calls back-off strategy
            template <typename FCKernel, typename PublicationRecord>
            bool wait( FCKernel& /*fc*/, PublicationRecord& rec )
            {
                rec.bkoff();
                return false;
            }

            /// Does nothing
            template <typename FCKernel, typename PublicationRecord>
            void notify( FCKernel& /*fc*/, PublicationRecord& /*rec*/ )
            {}

            /// Does nothing
            template <typename FCKernel>
            void wakeup( FCKernel& )
            {}
        };

        /// Wait strategy based on the single mutex and the condition variable
        /**
            The strategy shares the mutex and conditional variable for all thread.

            Template parameter \p Milliseconds specifies waiting duration;
            the minimal value is 1.
        */
        template <int Milliseconds = 2>
        class single_mutex_single_condvar
        {
        //@cond
            std::mutex  m_mutex;
            std::condition_variable m_condvar;
            bool        m_wakeup;

            typedef std::unique_lock< std::mutex > unique_lock;
        //@endcond

        public:
            enum {
                c_nWaitMilliseconds = Milliseconds < 1 ? 1 : Milliseconds ///< Waiting duration
            };

            /// Empty metafunction
            template <typename PublicationRecord>
            struct make_publication_record {
                typedef PublicationRecord type; ///< publication record type
            };

            /// Default ctor
            single_mutex_single_condvar()
                : m_wakeup( false )
            {}

            /// Does nothing
            template <typename PublicationRecord>
            void prepare( PublicationRecord& /*rec*/ )
            {}

            /// Sleeps on condition variable waiting for notification from combiner
            template <typename FCKernel, typename PublicationRecord>
            bool wait( FCKernel& fc, PublicationRecord& rec )
            {
                if ( fc.get_operation( rec ) >= req_Operation ) {
                    unique_lock lock( m_mutex );
                    if ( fc.get_operation( rec ) >= req_Operation ) {
                        if ( m_wakeup ) {
                            m_wakeup = false;
                            return true;
                        }

                        bool ret = m_condvar.wait_for( lock, std::chrono::milliseconds( c_nWaitMilliseconds )) == std::cv_status::no_timeout;
                        m_wakeup = false;
                        return ret;
                    }
                }
                return false;
            }

            /// Calls condition variable function \p notify_all()
            template <typename FCKernel, typename PublicationRecord>
            void notify( FCKernel& fc, PublicationRecord& /*rec*/ )
            {
                wakeup( fc );
            }

            /// Calls condition variable function \p notify_all()
            template <typename FCKernel>
            void wakeup( FCKernel& /*fc*/ )
            {
                unique_lock lock( m_mutex );
                m_wakeup = true;
                m_condvar.notify_all();
            }
        };

        /// Wait strategy based on the single mutex and thread-local condition variables
        /**
            The strategy shares the mutex, but each thread has its own conditional variable

            Template parameter \p Milliseconds specifies waiting duration;
            the minimal value is 1.
        */
        template <int Milliseconds = 2>
        class single_mutex_multi_condvar
        {
        //@cond
            std::mutex  m_mutex;
            bool        m_wakeup;

            typedef std::unique_lock< std::mutex > unique_lock;
        //@endcond

        public:
            enum {
                c_nWaitMilliseconds = Milliseconds < 1 ? 1 : Milliseconds  ///< Waiting duration
            };

            /// Incorporates a condition variable into \p PublicationRecord
            template <typename PublicationRecord>
            struct make_publication_record {
                /// Metafunction result
                struct type: public PublicationRecord
                {
                    //@cond
                    std::condition_variable m_condvar;
                    //@endcond
                };
            };

            /// Default ctor
            single_mutex_multi_condvar()
                : m_wakeup( false )
            {}

            /// Does nothing
            template <typename PublicationRecord>
            void prepare( PublicationRecord& /*rec*/ )
            {}

            /// Sleeps on condition variable waiting for notification from combiner
            template <typename FCKernel, typename PublicationRecord>
            bool wait( FCKernel& fc, PublicationRecord& rec )
            {
                if ( fc.get_operation( rec ) >= req_Operation ) {
                    unique_lock lock( m_mutex );

                    if ( fc.get_operation( rec ) >= req_Operation ) {
                        if ( m_wakeup ) {
                            m_wakeup = false;
                            return true;
                        }

                        bool ret = rec.m_condvar.wait_for( lock, std::chrono::milliseconds( c_nWaitMilliseconds )) == std::cv_status::no_timeout;
                        m_wakeup = false;
                        return ret;
                    }
                }
                return false;
            }

            /// Calls condition variable function \p notify_one()
            template <typename FCKernel, typename PublicationRecord>
            void notify( FCKernel& /*fc*/, PublicationRecord& rec )
            {
                unique_lock lock( m_mutex );
                m_wakeup = true;
                rec.m_condvar.notify_one();
            }

            /// Calls \p fc.wakeup_any() to wake up any pending thread
            template <typename FCKernel>
            void wakeup( FCKernel& fc )
            {
                fc.wakeup_any();
            }
        };

        /// Wait strategy where each thread has a mutex and a condition variable
        /**
            Template parameter \p Milliseconds specifies waiting duration;
            the minimal value is 1.
        */
        template <int Milliseconds = 2>
        class multi_mutex_multi_condvar
        {
        //@cond
            typedef std::unique_lock< std::mutex > unique_lock;
        //@endcond
        public:
            enum {
                c_nWaitMilliseconds = Milliseconds < 1 ? 1 : Milliseconds   ///< Waiting duration
            };

            /// Incorporates a condition variable and a mutex into \p PublicationRecord
            template <typename PublicationRecord>
            struct make_publication_record {
                /// Metafunction result
                struct type: public PublicationRecord
                {
                    //@cond
                    std::mutex              m_mutex;
                    std::condition_variable m_condvar;
                    bool                    m_wakeup;

                    type()
                        : m_wakeup( false )
                    {}
                    //@endcond
                };
            };

            /// Does nothing
            template <typename PublicationRecord>
            void prepare( PublicationRecord& /*rec*/ )
            {}

            /// Sleeps on condition variable waiting for notification from combiner
            template <typename FCKernel, typename PublicationRecord>
            bool wait( FCKernel& fc, PublicationRecord& rec )
            {
                if ( fc.get_operation( rec ) >= req_Operation ) {
                    unique_lock lock( rec.m_mutex );

                    if ( fc.get_operation( rec ) >= req_Operation ) {
                        if ( rec.m_wakeup ) {
                            rec.m_wakeup = false;
                            return true;
                        }

                        bool ret = rec.m_condvar.wait_for( lock, std::chrono::milliseconds( c_nWaitMilliseconds )) == std::cv_status::no_timeout;
                        rec.m_wakeup = false;
                        return ret;
                    }
                }
                return false;
            }

            /// Calls condition variable function \p notify_one()
            template <typename FCKernel, typename PublicationRecord>
            void notify( FCKernel& /*fc*/, PublicationRecord& rec )
            {
                unique_lock lock( rec.m_mutex );
                rec.m_wakeup = true;
                rec.m_condvar.notify_one();
            }

            /// Calls \p fc.wakeup_any() to wake up any pending thread
            template <typename FCKernel>
            void wakeup( FCKernel& fc )
            {
                fc.wakeup_any();
            }
        };

    } // namespace wait_strategy
}}} // namespace cds::algo::flat_combining

#endif //CDSLIB_ALGO_FLAT_COMBINING_WAIT_STRATEGY_H
