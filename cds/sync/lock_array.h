// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_SYNC_LOCK_ARRAY_H
#define CDSLIB_SYNC_LOCK_ARRAY_H

#include <mutex>    //unique_lock
#include <cds/details/allocator.h>
#include <cds/algo/int_algo.h>

namespace cds { namespace sync {

    /// Trivial lock \ref lock_array selection policy
    struct trivial_select_policy
    {
        /// Returns \p nWhat
        size_t operator()( size_t nWhat, size_t nCapacity ) const
        {
            assert( nWhat < nCapacity );
            CDS_UNUSED( nCapacity );
            return nWhat;
        }

        /// Checks if \p nCapacity is acceptable by policy. For trivial policy, any \p nCapacity is accepted.
        static bool is_capacity_accepted( size_t nCapacity )
        {
            CDS_UNUSED( nCapacity );
            return true;
        }
    };

    /// The lock \ref lock_array cell selection policy "division by modulo"
    struct mod_select_policy
    {
        /// Returns <tt> nWhat % nCapacity </tt>
        size_t operator()( size_t nWhat, size_t nCapacity ) const
        {
            return nWhat % nCapacity;
        }

        /// Checks if \p nCapacity is acceptable by policy. For modulo policy, any positive \p nCapacity is accepted.
        static bool is_capacity_accepted( size_t nCapacity )
        {
            return nCapacity > 0;
        }
    };

    /// The lock \ref lock_array cell selection policy "division by modulo of power of 2"
    /**
        This policy may be used if the size of lock array is equal to power of two.
    */
    struct pow2_select_policy
    {
        //@cond
        const size_t    m_nMask;
        //@endcond

        /// Ctor. \p nCapacity must be power of two.
        pow2_select_policy( size_t nCapacity )
            : m_nMask( nCapacity - 1 )
        {
            assert( is_capacity_accepted( nCapacity ));
        }

        /// Copy constructor
        pow2_select_policy( pow2_select_policy const& src )
            : m_nMask( src.m_nMask )
        {}

        /// Move constructor
        pow2_select_policy( pow2_select_policy&& src )
            : m_nMask( src.m_nMask )
        {}

        /// Returns <tt>nWhat & (nPow2 - 1)</tt>
        size_t operator()( size_t nWhat, size_t ) const
        {
            return nWhat & m_nMask;
        }

        /// Checks if \p nCapacity is acceptable by policy. \p nCapacity must be power of two
        static bool is_capacity_accepted( size_t nCapacity )
        {
            return cds::beans::is_power2( nCapacity );
        }
    };

    /// Array of locks
    /**
        The lock array is useful for building fine-grained lock-based data structure
        based on striping technique. Instead of locking access to data struct (a hash map, for example)
        at whole, the striping locks only part of the map (a bucket). So, access to different buckets
        can be simultaneous.

        Template arguments:
        - \p Lock - lock type, for example, \p std::mutex, \p cds::sync::spin_lock
        - \p SelectPolicy - array cell selection policy, the default is \ref mod_select_policy
             Available policies: \ref trivial_select_policy, \ref pow2_select_policy, \ref mod_select_policy.
        - \p Alloc - memory allocator for array

        To determine array's cell the selection policy \p SelectPolicy functor is used. Two arguments
        are passed to the policy:
        \code size_t operator()( size_t nHint, size_t nCapacity ) const \endcode
        - \p nHint - a hint to calculate cell index in the lock array. Usually, it is a hash value.
        - \p nCapacity - the size of the lock array
        The functor should return the index in the lock array.

        Note that the type of \p nHint parameter can be any.
    */
    template <typename Lock
        , typename SelectPolicy = mod_select_policy
        , class Alloc = CDS_DEFAULT_ALLOCATOR
    >
    class lock_array
    {
        //@cond
        typedef ::cds::details::Allocator< Lock, Alloc > cxx_allocator;
        //@endcond
    public:
        typedef Lock            lock_type           ;   ///< lock type
        typedef SelectPolicy    select_cell_policy  ;   ///< Cell selection policy functor
        static size_t const     c_nUnspecifiedCell = (size_t) -1 ;  ///< failed \ref try_lock call result

    protected:
        lock_type *         m_arrLocks  ;   ///< lock array
        size_t const        m_nCapacity ;   ///< array capacity

        select_cell_policy  m_SelectCellPolicy  ;   ///< Cell selection policy

    protected:
        //@cond
        static lock_type * create_lock_array( size_t nCapacity )
        {
            return cxx_allocator().NewArray( nCapacity );
        }
        static void delete_lock_array( lock_type * pArr, size_t nCapacity )
        {
            if ( pArr )
                cxx_allocator().Delete( pArr, nCapacity );
        }

        // Only for internal use!!!
        lock_array()
            : m_arrLocks( nullptr )
            , m_nCapacity(0)
        {}

        lock_array( select_cell_policy const& policy )
            : m_arrLocks( nullptr )
            , m_nCapacity(0)
            , m_SelectCellPolicy( policy )
        {}
        //@endcond

    public:
        /// Constructs array of locks
        /**
            Allocates the array and initializes all locks as unlocked.
        */
        lock_array(
            size_t nCapacity        ///< [in] Array size
        )
        : m_arrLocks( nullptr )
        , m_nCapacity( nCapacity )
        {
            m_arrLocks = create_lock_array( nCapacity );
        }

        /// Constructs array of lock and copy cell selection policy
        /**
            Allocates the array and initializes all locks as unlocked.
        */
        lock_array(
            size_t nCapacity,       ///< [in] Array size
            select_cell_policy const& policy    ///< Cell selection policy (copy-constructible)
        )
        : m_arrLocks( nullptr )
        , m_nCapacity( nCapacity )
        , m_SelectCellPolicy( policy )
        {
            m_arrLocks = create_lock_array( m_nCapacity );
        }

        /// Constructs array of lock and move cell selection policy
        /**
            Allocates the array and initializes all locks as unlocked.
        */
        lock_array(
            size_t nCapacity,       ///< [in] Array size
            select_cell_policy&& policy    ///< Cell selection policy (move-constructible)
        )
        : m_arrLocks( nullptr )
        , m_nCapacity( nCapacity )
        , m_SelectCellPolicy( std::forward<select_cell_policy>( policy ))
        {
            m_arrLocks = create_lock_array( m_nCapacity );
        }

        /// Destructs array of locks and frees used memory
        ~lock_array()
        {
            delete_lock_array( m_arrLocks, m_nCapacity );
        }

        /// Locks a lock at cell \p hint
        /**
            To define real array's cell which should be locked, \ref select_cell_policy is used.
            The target cell is a result of <tt>select_cell_policy( hint, size())</tt>.

            Returns the index of locked lock.
        */
        template <typename Q>
        size_t lock( Q const& hint )
        {
            size_t nCell = m_SelectCellPolicy( hint, size());
            assert( nCell < size());
            m_arrLocks[nCell].lock();
            return nCell;
        }

        /// Try lock a lock at cell \p hint
        /**
            To define real array's cell which should be locked, \ref select_cell_policy is used.
            The target cell is a result of <tt>select_cell_policy( hint, size())</tt>.

            Returns the index of locked lock if success, \ref c_nUnspecifiedCell constant otherwise.
        */
        template <typename Q>
        size_t try_lock( Q const& hint )
        {
            size_t nCell = m_SelectCellPolicy( hint, size());
            assert( nCell < size());
            if ( m_arrLocks[nCell].try_lock())
                return nCell;
            return c_nUnspecifiedCell;
        }

        /// Unlock the lock specified by index \p nCell
        void unlock( size_t nCell )
        {
            assert( nCell < size());
            m_arrLocks[nCell].unlock();
        }

        /// Lock all
        void lock_all()
        {
            lock_type * pLock = m_arrLocks;
            for ( lock_type * pEnd = m_arrLocks + size(); pLock != pEnd; ++pLock )
                pLock->lock();
        }

        /// Unlock all
        void unlock_all()
        {
            lock_type * pLock = m_arrLocks;
            for ( lock_type * pEnd = m_arrLocks + size(); pLock != pEnd; ++pLock )
                pLock->unlock();
        }

        /// Get lock at cell \p nCell.
        /**
            Precondition: <tt>nCell < size()</tt>
        */
        lock_type& at( size_t nCell ) const
        {
            assert( nCell < size());
            return m_arrLocks[ nCell ];
        }

        /// Size of lock array.
        size_t size() const
        {
            return m_nCapacity;
        }
    };

}} // namespace cds::sync

//@cond
namespace std {

    /// Specialization \p std::unique_lock for \p sync::lock_array
    template <typename Lock, typename SelectPolicy, class Alloc>
    class unique_lock< cds::sync::lock_array< Lock, SelectPolicy, Alloc > >
    {
    public:
        typedef cds::sync::lock_array< Lock, SelectPolicy, Alloc >   lock_array_type;   ///< Lock array type

    private:
        lock_array_type&    m_arrLocks;
        size_t              m_nLockGuarded;

        static const size_t c_nLockAll = ~size_t( 0 );

    public:
        /// Onws the lock array \p arrLocks and locks a cell determined by \p hint parameter
        template <typename Q>
        unique_lock( lock_array_type& arrLocks, Q const& hint )
            : m_arrLocks( arrLocks )
            , m_nLockGuarded( arrLocks.lock( hint ))
        {}

        /// Locks all from \p arrLocks array
        unique_lock( lock_array_type& arrLocks )
            : m_arrLocks( arrLocks )
            , m_nLockGuarded( c_nLockAll )
        {
            arrLocks.lock_all();
        }

        unique_lock() = delete;
        unique_lock( unique_lock const& ) = delete;

        ~unique_lock()
        {
            if ( m_nLockGuarded == c_nLockAll )
                m_arrLocks.unlock_all();
            else
                m_arrLocks.unlock( m_nLockGuarded );
        }
    };
} // namespace std
//@endcond

#endif // #ifndef CDSLIB_SYNC_LOCK_ARRAY_H
