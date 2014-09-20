//$$CDS-header$$

#ifndef __CDS_GC_HRC_SCHEMA_RETIRED_H
#define __CDS_GC_HRC_SCHEMA_RETIRED_H

#include <cds/gc/hrc/details/hrc_fwd.h>
#include <cds/gc/details/retired_ptr.h>
#include <cds/cxx11_atomic.h>
#include <cds/details/bounded_array.h>

namespace cds { namespace gc { namespace hrc {
    namespace details {

        /// Pointer to function to free (destruct and deallocate) retired pointer of specific type
        typedef gc::details::free_retired_ptr_func free_retired_ptr_func;

        /// Retired node descriptor
        struct retired_node {
            atomics::atomic<ContainerNode *> m_pNode        ;    ///< node to destroy
            free_retired_ptr_func               m_funcFree     ;    ///< pointer to the destructor function
            size_t                              m_nNextFree    ;    ///< Next free item in retired array
            atomics::atomic<unsigned int>    m_nClaim       ;    ///< Access to reclaimed node
            atomics::atomic<bool>            m_bDone        ;    ///< the record is in work (concurrent access flag)

            /// Default ctor
            retired_node()
                : m_pNode( nullptr )
                , m_funcFree( nullptr )
                , m_nNextFree(0)
                , m_nClaim(0)
                , m_bDone( false )
            {}

            /// Assignment ctor
            retired_node(
                ContainerNode * pNode           ///< Node to retire
                ,free_retired_ptr_func func     ///< Destructor function
                )
                : m_pNode( pNode )
                , m_funcFree( func )
                , m_nClaim(0)
                , m_bDone( false )
            {}

            /// Compares two \ref retired_node
            static bool Less( const retired_node& p1, const retired_node& p2 )
            {
                return p1.m_pNode.load( atomics::memory_order_relaxed ) < p2.m_pNode.load( atomics::memory_order_relaxed );
            }

            /// Assignment operator
            retired_node& set( ContainerNode * pNode, free_retired_ptr_func func )
            {
                m_bDone.store( false, atomics::memory_order_relaxed );
                m_nClaim.store( 0, atomics::memory_order_relaxed );
                m_funcFree = func;
                m_pNode.store( pNode, atomics::memory_order_release );
                CDS_COMPILER_RW_BARRIER;
                return *this;
            }

            /// Invokes destructor function for the pointer
            void free()
            {
                assert( m_funcFree != nullptr );
                m_funcFree( m_pNode.load( atomics::memory_order_relaxed ));
            }
        };

        /// Compare two retired node
        /**
            This comparison operator is needed for sorting pointers on
            deallocation step
        */
        static inline bool operator <( const retired_node& p1, const retired_node& p2 )
        {
            return retired_node::Less( p1, p2 );
        }

        /// Array of ready for destroying pointers
        /**
            The array object is belonged to one thread: only owner thread may write to this array,
            any other thread can read one.
        */
        class retired_vector
        {
            typedef cds::details::bounded_array<retired_node> vector_type  ;   ///< type of vector of retired pointer (implicit CDS_DEFAULT_ALLOCATOR dependency)

            //@cond
            static const size_t m_nEndFreeList = size_t(0) -  1 ;    ///< End of free list
            //@endcond
            size_t          m_nFreeList ; ///< Index of first free item in m_arr
            vector_type     m_arr       ; ///< Array of retired pointers (implicit \ref CDS_DEFAULT_ALLOCATOR dependence)

        public:
            /// Iterator over retired pointer vector
            typedef vector_type::iterator                       iterator;
            /// Const iterator type
            typedef vector_type::const_iterator                 const_iterator;

        public:
            /// Ctor
            retired_vector( const GarbageCollector& mgr )    ;    // inline
            ~retired_vector()
            {}

            ///@anchor hrc_gc_retired_vector_capacity Capacity (max available size) of array
            size_t capacity() const
            {
                return m_arr.capacity();
            }

            /// Returns count of retired node in array. This function is intended for debug purposes only
            size_t retiredNodeCount() const
            {
                size_t nCount = 0;
                const size_t nCapacity = capacity();
                for ( size_t i = 0; i < nCapacity; ++i ) {
                    if ( m_arr[i].m_pNode.load( atomics::memory_order_relaxed ) != nullptr )
                        ++nCount;
                }
                return nCount;
            }

            /// Push a new item into the array
            void push( ContainerNode * p, free_retired_ptr_func pFunc )
            {
                assert( !isFull());

                size_t n = m_nFreeList;
                assert( m_arr[n].m_pNode.load( atomics::memory_order_relaxed ) == nullptr );
                m_nFreeList = m_arr[n].m_nNextFree;
                CDS_DEBUG_DO( m_arr[n].m_nNextFree = m_nEndFreeList ; )
                m_arr[n].set( p, pFunc );
            }

            /// Pops the item by index \p n from the array
            void pop( size_t n )
            {
                assert( n < capacity() );
                m_arr[n].m_pNode.store( nullptr, atomics::memory_order_release );
                m_arr[n].m_nNextFree = m_nFreeList;
                m_nFreeList = n;
            }

            /// Checks if array is full
            bool isFull() const
            {
                return m_nFreeList == m_nEndFreeList;
            }

            /// Get the item by index \p i
            retired_node& operator []( size_t i )
            {
                assert( i < capacity() );
                return m_arr[i];
            }

            /// Returns a random-access iterator to the first element in the retired pointer vector
            /**
                If the vector is empty, end() == begin().
            */
            iterator begin()
            {
                return m_arr.begin();
            }

            /// Const version of begin()
            const_iterator begin() const
            {
                return m_arr.begin();
            }

            /// A random-access iterator to the end of the vector object.
            /**
                If the vector is empty, end() == begin().
            */
            iterator end()
            {
                return m_arr.end();
            }

            /// Const version of end()
            const_iterator end() const
            {
                return m_arr.end();
            }
        };

    }    // namespace details
}}}    // namespace cds::gc::hrc

#endif // #ifndef __CDS_GC_HRC_SCHEMA_RETIRED_H
