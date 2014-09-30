//$$CDS-header$$-2

#ifndef __CDS_LOCK_SCOPED_LOCK_H
#define __CDS_LOCK_SCOPED_LOCK_H

#include <cds/details/defs.h>
#include <cds/details/noncopyable.h>

namespace cds { namespace lock {

    /// Scoped lock
    /**

        An object of type \p scoped_lock controls the ownership of a lockable object within a scope.
        A \p scoped_lock object maintains ownership of a lockable object throughout the \p scoped_lock object’s lifetime.
        The behavior of a program is undefined if the lockable object does not exist for the entire lifetime
        of the \p scoped_lock object.
        The supplied \p Lock type shall have two methods: \p lock and \p unlock.

        The constructor locks the wrapped lock object, the destructor unlocks it.

        Scoped lock is not copy-constructible and not default-constructible.

        This class is similar to \p std::lock_quard
    */
    template <class Lock>
    class scoped_lock: public cds::details::noncopyable
    {
    public:
        typedef Lock lock_type ;    ///< Lock type

    protected:
        lock_type&  m_Lock ;        ///< Owned lock object

    protected:
        //@cond
        // Only for internal use!!!
        scoped_lock()
        {}
        //@endcond
    public:
        /// Get ownership of lock object \p l and calls <tt>l.lock()</tt>
        scoped_lock( lock_type& l )
            : m_Lock( l )
        {
            l.lock();
        }

        /// Get ownership of lock object \p l and conditionally locks it
        /**
            The constructor calls <tt>l.lock()</tt> only if \p bAlreadyLocked is \p false.
            If \p bAlreadyLocked is \p true, no locking is performed.

            In any case, the destructor of \p scoped_lock object invokes <tt>l.unlock()</tt>.
        */
        scoped_lock( lock_type& l, bool bAlreadyLocked )
            : m_Lock( l )
        {
            if ( !bAlreadyLocked )
                l.lock();
        }

        /// Unlock underlying lock object and release ownership
        ~scoped_lock()
        {
            m_Lock.unlock();
        }
    };
}}  // namespace cds::lock


#endif // #ifndef __CDS_LOCK_SCOPED_LOCK_H
