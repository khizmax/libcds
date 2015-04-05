//$$CDS-header$$

#ifndef CDSLIB_OPT_BUFFER_H
#define CDSLIB_OPT_BUFFER_H

#include <cds/details/defs.h>
#include <cds/user_setup/allocator.h>
#include <cds/details/allocator.h>
#include <cds/algo/int_algo.h>

namespace cds { namespace opt {

    /// [type-option] Option setter for user-provided plain buffer
    /**
        This option is used by some container as a random access array for storing
        container's item; for example, a bounded queue may use
        this option to define underlying buffer implementation.

        The template parameter \p Type should be rebindable.

        Implementations:
            - opt::v::static_buffer
            - opt::v::dynamic_buffer
    */
    template <typename Type>
    struct buffer {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef Type buffer;
        };
        //@endcond
    };

    namespace v {

        /// Static buffer (see \p cds::opt::buffer option)
        /**
            One of available type for opt::buffer type-option.

            This buffer maintains static array. No dynamic memory allocation performed.

            \par Template parameters:
                - \p T - item type the buffer stores
                - \p Capacity - the capacity of buffer. The value must be power of two if \p Exp2 is \p true
                - \p Exp2 - a boolean flag. If it is \p true the buffer capacity must be power of two.
                    Otherwise it can be any positive number. Usually, it is required that the buffer has
                    size of a power of two.
        */
        template <typename T, size_t Capacity, bool Exp2 = true>
        class static_buffer
        {
        public:
            typedef T   value_type  ;   ///< value type
            static const size_t c_nCapacity = Capacity ;    ///< Capacity
            static const bool c_bExp2 = Exp2; ///< \p Exp2 flag

            /// Rebind buffer for other template parameters
            template <typename Q, size_t Capacity2 = c_nCapacity, bool Exp22 = c_bExp2>
            struct rebind {
                typedef static_buffer<Q, Capacity2, Exp22> other   ;   ///< Rebind result type
            };
        private:
            //@cond
            value_type  m_buffer[c_nCapacity];
            //@endcond
        public:
            /// Construct static buffer
            static_buffer()
            {
                // Capacity must be power of 2
                static_assert( !c_bExp2 || (c_nCapacity & (c_nCapacity - 1)) == 0, "Capacity must be power of two" );
            }
            /// Construct buffer of given capacity
            /**
                This ctor ignores \p nCapacity argument. The capacity of static buffer
                is defined by template argument \p Capacity
            */
            static_buffer( size_t nCapacity )
            {
                CDS_UNUSED( nCapacity );
                // Capacity must be power of 2
                static_assert( !c_bExp2 || (c_nCapacity & (c_nCapacity - 1)) == 0,  "Capacity must be power of two");
                //assert( c_nCapacity == nCapacity );
            }

            /// Get item \p i
            value_type& operator []( size_t i )
            {
                assert( i < capacity() );
                return m_buffer[i];
            }

            /// Get item \p i, const version
            const value_type& operator []( size_t i ) const
            {
                assert( i < capacity() );
                return m_buffer[i];
            }

            /// Returns buffer capacity
            CDS_CONSTEXPR size_t capacity() const CDS_NOEXCEPT
            {
                return c_nCapacity;
            }

            /// Zeroize the buffer
            void zeroize()
            {
                memset( m_buffer, 0, capacity() * sizeof(m_buffer[0]) );
            }

            /// Returns pointer to buffer array
            value_type * buffer()
            {
                return m_buffer;
            }

            /// Returns pointer to buffer array (const version)
            value_type * buffer() const
            {
                return m_buffer;
            }

        private:
            //@cond
            // non-copyable
            static_buffer(const static_buffer&);
            void operator =(const static_buffer&);
            //@endcond
        };


        /// Dynamically allocated buffer
        /**
            One of available \p cds::opt::buffer type-option.

            This buffer maintains dynamically allocated array.
            Allocation is performed at construction time.

            \par Template parameters:
                - \p T - item type storing in the buffer
                - \p Alloc - an allocator used for allocating internal buffer (\p std::allocator interface)
                - \p Exp2 - a boolean flag. If it is \p true the buffer capacity must be power of two.
                    Otherwise it can be any positive number. Usually, it is required that the buffer has
                    size of a power of two.
        */
        template <typename T, class Alloc = CDS_DEFAULT_ALLOCATOR, bool Exp2 = true>
        class dynamic_buffer
        {
        public:
            typedef T   value_type  ;   ///< Value type
            static CDS_CONSTEXPR const bool c_bExp2 = Exp2; ///< \p Exp2 flag

            /// Rebind buffer for other template parameters
            template <typename Q, typename Alloc2=Alloc, bool Exp22 = c_bExp2>
            struct rebind {
                typedef dynamic_buffer<Q, Alloc2, Exp22> other   ;  ///< Rebinding result type
            };

            //@cond
            typedef cds::details::Allocator<value_type, Alloc>   allocator_type;
            //@endcond

        private:
            //@cond
            value_type *    m_buffer;
            size_t const    m_nCapacity;
            //@endcond
        public:
            /// Allocates dynamic buffer of given \p nCapacity
            /**
                If \p Exp2 class template parameter is \p true then actual capacity
                of allocating buffer is nearest upper to \p nCapacity power of two.
            */
            dynamic_buffer( size_t nCapacity )
                : m_nCapacity( c_bExp2 ? beans::ceil2(nCapacity) : nCapacity )
            {
                assert( m_nCapacity >= 2 );
                // Capacity must be power of 2
                assert( !c_bExp2 || (m_nCapacity & (m_nCapacity - 1)) == 0 );

                allocator_type a;
                m_buffer = a.NewArray( m_nCapacity );
            }

            /// Destroys dynamically allocated buffer
            ~dynamic_buffer()
            {
                allocator_type a;
                a.Delete( m_buffer, m_nCapacity );
            }

            /// Get item \p i
            value_type& operator []( size_t i )
            {
                assert( i < capacity() );
                return m_buffer[i];
            }

            /// Get item \p i, const version
            const value_type& operator []( size_t i ) const
            {
                assert( i < capacity() );
                return m_buffer[i];
            }

            /// Returns buffer capacity
            size_t capacity() const CDS_NOEXCEPT
            {
                return m_nCapacity;
            }

            /// Zeroize the buffer
            void zeroize()
            {
                memset( m_buffer, 0, capacity() * sizeof(m_buffer[0]) );
            }

            /// Returns pointer to buffer array
            value_type * buffer()
            {
                return m_buffer;
            }

            /// Returns pointer to buffer array (const version)
            value_type * buffer() const
            {
                return m_buffer;
            }

        private:
            //@cond
            // non-copyable
            dynamic_buffer(const dynamic_buffer&);
            void operator =(const dynamic_buffer&);
            //@endcond
        };

    }   // namespace v

}}  // namespace cds::opt

#endif // #ifndef CDSLIB_OPT_BUFFER_H
