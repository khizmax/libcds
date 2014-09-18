//$$CDS-header$$

#ifndef __CDS_GC_GUARDED_PTR_H
#define __CDS_GC_GUARDED_PTR_H

#include <cds/details/defs.h>

namespace cds { namespace gc {

    /// Guarded pointer
    /**
        A guarded pointer is a pair of the pointer and GC's guard.
        Usually, it is used for returning a pointer to the item from an lock-free container.
        The guard prevents the pointer to be early disposed (freed) by GC.
        After destructing \p %guarded_ptr object the pointer can be automatically disposed (freed) at any time.

        Template arguments:
        - \p GC - a garbage collector type like cds::gc::HP and any other from cds::gc namespace
        - \p GuardedType - a type which the guard stores
        - \p ValueType - a value type
        - \p Cast - a functor for converting <tt>GuardedType*</tt> to <tt>ValueType*</tt>. Default is \p void (no casting).

        For intrusive containers, \p GuardedType is the same as \p ValueType and no casting is needed.
        In such case the \p %guarded_ptr is:
        @code
        typedef cds::gc::guarded_ptr< cds::gc::HP, foo > intrusive_guarded_ptr;
        @endcode

        For standard (non-intrusive) containers \p GuardedType is not the same as \p ValueType and casting is needed.
        For example:
        @code
        struct foo {
            int const   key;
            std::string value;
        };

        struct value_accessor {
            std::string* operator()( foo* pFoo ) const
            {
                return &(pFoo->value);
            }
        };

        // Guarded ptr
        typedef cds::gc::guarded_ptr< cds::gc::HP, Foo, std::string, value_accessor > nonintrusive_guarded_ptr;
        @endcode

        Many set/map container classes from \p libcds declare the typedef for \p %guarded_ptr with appropriate casting functor.
    */
    template <class GC, typename GuardedType, typename ValueType=GuardedType, typename Cast=void >
    class guarded_ptr
    {
        //TODO: use moce semantics and explicit operator bool!
    public:
        typedef GC          gc         ;   ///< Garbage collector like cds::gc::HP and any other from cds::gc namespace
        typedef GuardedType guarded_type;  ///< Guarded type
        typedef ValueType   value_type ;   ///< Value type
        typedef Cast        value_cast ;   ///< Functor for casting \p guarded_type to \p value_type

    private:
        //@cond
        typename gc::Guard  m_guard;
        //@endcond

    public:
        /// Creates empty guarded pointer
        guarded_ptr() CDS_NOEXCEPT
        {}

        /// Initializes guarded pointer with \p p
        guarded_ptr( guarded_type * p ) CDS_NOEXCEPT
        {
            m_guard.assign( p );
        }

        /// Copy constructor
        guarded_ptr( guarded_ptr const& gp ) CDS_NOEXCEPT
        {
            m_guard.copy( gp.m_guard );
        }

        /// Clears the guarded pointer
        /**
            \ref release is called if guarded pointer is not \ref empty
        */
        ~guarded_ptr() CDS_NOEXCEPT
        {
            release();
        }

        /// Assignment operator
        guarded_ptr& operator=( guarded_ptr const& gp ) CDS_NOEXCEPT
        {
            m_guard.copy( gp.m_guard );
            return *this;
        }

        /// Returns a pointer to guarded value
        value_type * operator ->() const CDS_NOEXCEPT
        {
            return value_cast()( m_guard.template get<guarded_type>() );
        }

        /// Returns a reference to guarded value
        value_type& operator *() CDS_NOEXCEPT
        {
            assert( !empty());
            return *value_cast()( m_guard.template get<guarded_type>() );
        }

        /// Returns const reference to guarded value
        value_type const& operator *() const CDS_NOEXCEPT
        {
            assert( !empty());
            return *value_cast()( m_guard.template get<guarded_type>() );
        }

        /// Checks if the guarded pointer is \p NULL
        bool empty() const CDS_NOEXCEPT
        {
            return m_guard.template get<guarded_type>() == null_ptr<guarded_type *>();
        }

        /// Clears guarded pointer
        /**
            If the guarded pointer has been released, the pointer can be disposed (freed) at any time.
            Dereferncing the guarded pointer after \p release() is dangerous.
        */
        void release() CDS_NOEXCEPT
        {
            m_guard.clear();
        }

        //@cond
        // For internal use only!!!
        typename gc::Guard& guard() CDS_NOEXCEPT
        {
            return m_guard;
        }
        //@endcond
    };


    //@cond
    // Intrusive specialization
    template <class GC, typename T>
    class guarded_ptr< GC, T, T, void >
    {
    public:
        typedef GC  gc         ;   ///< Garbage collector like cds::gc::HP
        typedef T   guarded_type;  ///< Guarded type
        typedef T   value_type ;   ///< Value type

    private:
        typename gc::Guard  m_guard;

    public:
        guarded_ptr() CDS_NOEXCEPT
        {}

        guarded_ptr( value_type * p ) CDS_NOEXCEPT
        {
            m_guard.assign( p );
        }

        guarded_ptr( guarded_ptr const& gp ) CDS_NOEXCEPT
        {
            m_guard.copy( gp.m_guard );
        }

        ~guarded_ptr() CDS_NOEXCEPT
        {
            release();
        }

        guarded_ptr& operator=( guarded_ptr const& gp ) CDS_NOEXCEPT
        {
            m_guard.copy( gp.m_guard );
            return *this;
        }

        value_type * operator ->() const CDS_NOEXCEPT
        {
            return m_guard.template get<value_type>();
        }

        value_type& operator *() CDS_NOEXCEPT
        {
            assert( !empty());
            return *m_guard.template get<value_type>();
        }

        value_type const& operator *() const CDS_NOEXCEPT
        {
            assert( !empty());
            return *m_guard.template get<value_type>();
        }

        bool empty() const CDS_NOEXCEPT
        {
            return m_guard.template get<guarded_type>() == null_ptr<guarded_type *>();
        }

        void release() CDS_NOEXCEPT
        {
            m_guard.clear();
        }

        typename gc::Guard& guard() CDS_NOEXCEPT
        {
            return m_guard;
        }
    };
    //@endcond

}} // namespace cds::gc

#endif // #ifndef __CDS_GC_GUARDED_PTR_H
