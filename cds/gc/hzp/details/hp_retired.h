//$$CDS-header$$

#ifndef __CDS_GC_HZP_DETAILS_HP_RETIRED_H
#define __CDS_GC_HZP_DETAILS_HP_RETIRED_H

#include <cds/gc/hzp/details/hp_fwd.h>
#include <cds/gc/hzp/details/hp_type.h>

#include <cds/details/bounded_array.h>

namespace cds {
    namespace gc{ namespace hzp { namespace details {

        /// Retired pointer
        typedef cds::gc::details::retired_ptr   retired_ptr;

        /// Array of retired pointers
        /**
            The vector of retired pointer ready to delete.

            The Hazard Pointer schema is build on thread-static arrays. For each HP-enabled thread the HP manager allocates
            array of retired pointers. The array belongs to the thread: owner thread writes to the array, other threads
            just read it.
        */
        class retired_vector {
            /// Underlying vector implementation
            typedef cds::details::bounded_array<retired_ptr>    retired_vector_impl;

            retired_vector_impl m_arr   ;   ///< the array of retired pointers
            size_t              m_nSize ;   ///< Current size of \p m_arr

        public:
            /// Iterator
            typedef    retired_vector_impl::iterator    iterator;

            /// Constructor
            retired_vector( const cds::gc::hzp::GarbageCollector& HzpMgr )    ;    // inline
            ~retired_vector()
            {}

            /// Vector capacity.
            /**
                The capacity is constant for any thread. It is defined by cds::gc::hzp::GarbageCollector.
            */
            size_t capacity() const     { return m_arr.capacity(); }

            /// Current vector size (count of retired pointers in the vector)
            size_t size() const         { return m_nSize; }

            /// Set vector size. Uses internally
            void size( size_t nSize )
            {
                assert( nSize <= capacity() );
                m_nSize = nSize;
            }

            /// Pushes retired pointer to the vector
            void push( const retired_ptr& p )
            {
                assert( m_nSize < capacity() );
                m_arr[ m_nSize ] = p;
                ++m_nSize;
            }

            /// Checks if the vector is full (size() == capacity() )
            bool isFull() const
            {
                return m_nSize >= capacity();
            }

            /// Begin iterator
            iterator    begin()    { return m_arr.begin(); }
            /// End iterator
            iterator    end()    { return m_arr.begin() +  m_nSize ; }

            /// Clears the vector. After clearing, size() == 0
            void clear()
            {
                m_nSize = 0;
            }
        };

    } } }    // namespace gc::hzp::details
}    // namespace cds

#endif // #ifndef __CDS_GC_HZP_DETAILS_HP_RETIRED_H
