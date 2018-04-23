// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_OPT_BUFFER_H
#define CDSLIB_OPT_BUFFER_H

#include <memory.h>
#include <cds/details/defs.h>
#include <cds/user_setup/allocator.h>
#include <cds/details/allocator.h>
#include <cds/algo/int_algo.h>
#include <memory>

namespace cds { namespace opt {

    /// [type-option] Option setter for user-provided plain buffer
    /**
        This option is used by some container as a random access array for storing
        container's item; for example, a bounded queue may use
        this option to define underlying buffer implementation.

        The template parameter \p Type should be rebindable.

        Implementations:
            - \p opt::v::initialized_static_buffer
            - \p opt::v::uninitialized_static_buffer
            - \p opt::v::initialized_dynamic_buffer
            - \p opt::v::uninitialized_dynamic_buffer

        Uninitialized buffer is just an array of uninitialized elements.
        Each element should be manually constructed, for example with a placement new operator.
        When the uninitialized buffer is destroyed the destructor of its element is not called.

        Initialized buffer contains default-constructed elements. Element destructor is called automatically
        when the buffer is destroyed.

        @note Usually, initialized and uninitialized buffers are not interchangeable.
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

        /// Static uninitialized buffer
        /**
            One of available type for \p opt::buffer option.

            This buffer maintains static array of uninitialized elements.
            You should manually construct each element when needed.
            No dynamic memory allocation performed.

            \par Template parameters:
                - \p T - item type the buffer stores
                - \p Capacity - the capacity of buffer. The value must be power of two if \p Exp2 is \p true
                - \p Exp2 - a boolean flag. If it is \p true the buffer capacity must be power of two.
                    Otherwise it can be any positive number. Usually, it is required that the buffer has
                    size of a power of two.
        */
        template <typename T, size_t Capacity, bool Exp2 = true>
        class uninitialized_static_buffer
        {
        public:
            typedef T   value_type;   ///< value type
            static constexpr const size_t c_nCapacity = Capacity;    ///< Capacity
            static constexpr const bool c_bExp2 = Exp2; ///< \p Exp2 flag

            /// Rebind buffer for other template parameters
            template <typename Q, size_t Capacity2 = c_nCapacity, bool Exp22 = c_bExp2>
            struct rebind {
                typedef uninitialized_static_buffer<Q, Capacity2, Exp22> other;   ///< Rebind result type
            };

            // Capacity must be power of 2
            static_assert(!c_bExp2 || (c_nCapacity & (c_nCapacity - 1)) == 0, "Capacity must be power of two");

        private:
            //@cond
            union element {
                value_type v;
                char       c;

                element()
                {}
            };

            element  m_buffer[c_nCapacity];
            //@endcond
        public:
            /// Construct static buffer
            uninitialized_static_buffer() noexcept
            {}

            /// Construct buffer of given capacity
            /**
                This ctor ignores \p nCapacity argument. The capacity of static buffer
                is defined by template argument \p Capacity
            */
            uninitialized_static_buffer( size_t nCapacity ) noexcept
            {
                CDS_UNUSED( nCapacity );
            }

            uninitialized_static_buffer( const uninitialized_static_buffer& ) = delete;
            uninitialized_static_buffer& operator =( const uninitialized_static_buffer& ) = delete;

            /// Get item \p i
            value_type& operator []( size_t i )
            {
                assert( i < capacity());
                return m_buffer[i].v;
            }

            /// Get item \p i, const version
            const value_type& operator []( size_t i ) const
            {
                assert( i < capacity());
                return m_buffer[i].v;
            }

            /// Returns buffer capacity
            constexpr size_t capacity() const noexcept
            {
                return c_nCapacity;
            }

            /// Zeroize the buffer
            void zeroize()
            {
                memset( m_buffer, 0, capacity() * sizeof(m_buffer[0]));
            }

            /// Returns pointer to buffer array
            value_type * buffer() noexcept
            {
                return &( m_buffer[0].v );
            }

            /// Returns pointer to buffer array
            value_type * buffer() const noexcept
            {
                return &( m_buffer[0].v );
            }

            /// Returns <tt> idx % capacity() </tt>
            /**
                If the buffer size is a power of two, binary arithmethics is used
                instead of modulo arithmetics
            */
            size_t mod( size_t idx )
            {
                constexpr_if ( c_bExp2 )
                    return idx & ( capacity() - 1 );
                else
                    return idx % capacity();
            }

            //@cond
            template <typename I>
            typename std::enable_if< sizeof(I) != sizeof(size_t), size_t >::type mod( I idx )
            {
                constexpr_if ( c_bExp2 )
                    return static_cast<size_t>( idx & static_cast<I>( capacity() - 1 ));
                else
                    return static_cast<size_t>( idx % capacity());
            }
            //@endcond
        };

        /// Static initialized buffer
        /**
            One of available type for \p opt::buffer option.

            This buffer maintains static array of default-constructed elements.
            No dynamic memory allocation performed.

            \par Template parameters:
                - \p T - item type the buffer stores
                - \p Capacity - the capacity of buffer. The value must be power of two if \p Exp2 is \p true
                - \p Exp2 - a boolean flag. If it is \p true the buffer capacity must be power of two.
                    Otherwise it can be any positive number. Usually, it is required that the buffer has
                    size of a power of two.
        */
        template <typename T, size_t Capacity, bool Exp2 = true>
        class initialized_static_buffer
        {
        public:
            typedef T   value_type;   ///< value type
            static constexpr const size_t c_nCapacity = Capacity;    ///< Capacity
            static constexpr const bool c_bExp2 = Exp2; ///< \p Exp2 flag

            /// Rebind buffer for other template parameters
            template <typename Q, size_t Capacity2 = c_nCapacity, bool Exp22 = c_bExp2>
            struct rebind {
                typedef initialized_static_buffer<Q, Capacity2, Exp22> other;   ///< Rebind result type
            };

            // Capacity must be power of 2
            static_assert(!c_bExp2 || (c_nCapacity & (c_nCapacity - 1)) == 0, "Capacity must be power of two");

        private:
            //@cond
            value_type  m_buffer[c_nCapacity];
            //@endcond
        public:
            /// Construct static buffer
            initialized_static_buffer() noexcept
            {}

            /// Construct buffer of given capacity
            /**
                This ctor ignores \p nCapacity argument. The capacity of static buffer
                is defined by template argument \p Capacity
            */
            initialized_static_buffer( size_t nCapacity ) noexcept
            {
                CDS_UNUSED( nCapacity );
            }

            initialized_static_buffer( const initialized_static_buffer& ) = delete;
            initialized_static_buffer& operator =( const initialized_static_buffer& ) = delete;

            /// Get item \p i
            value_type& operator []( size_t i )
            {
                assert( i < capacity());
                return m_buffer[i];
            }

            /// Get item \p i, const version
            const value_type& operator []( size_t i ) const
            {
                assert( i < capacity());
                return m_buffer[i];
            }

            /// Returns buffer capacity
            constexpr size_t capacity() const noexcept
            {
                return c_nCapacity;
            }

            /// Zeroize the buffer
            void zeroize()
            {
                memset( m_buffer, 0, capacity() * sizeof(m_buffer[0]));
            }

            /// Returns pointer to buffer array
            value_type * buffer() noexcept
            {
                return m_buffer;
            }

            /// Returns pointer to buffer array
            value_type * buffer() const noexcept
            {
                return m_buffer;
            }

            /// Returns <tt> idx % capacity() </tt>
            /**
            If the buffer size is a power of two, binary arithmethics is used
            instead of modulo arithmetics
            */
            size_t mod( size_t idx )
            {
                constexpr_if ( c_bExp2 )
                    return idx & ( capacity() - 1 );
                else
                    return idx % capacity();
            }

            //@cond
            template <typename I>
            typename std::enable_if< sizeof( I ) != sizeof( size_t ), size_t >::type mod( I idx )
            {
                constexpr_if ( c_bExp2 )
                    return static_cast<size_t>( idx & static_cast<I>( capacity() - 1 ));
                else
                    return static_cast<size_t>( idx % capacity());
            }
            //@endcond
        };

        /// Dynamically allocated uninitialized buffer
        /**
            One of available type for \p opt::buffer option.

            This buffer maintains dynamically allocated array of uninitialized elements.
            You should manually construct each element when needed.
            Allocation is performed at construction time.

            \par Template parameters:
                - \p T - item type storing in the buffer
                - \p Alloc - an allocator used for allocating internal buffer (\p std::allocator interface)
                - \p Exp2 - a boolean flag. If it is \p true the buffer capacity must be power of two.
                    Otherwise it can be any positive number. Usually, it is required that the buffer has
                    size of a power of two.
        */
        template <typename T, class Alloc = CDS_DEFAULT_ALLOCATOR, bool Exp2 = true>
        class uninitialized_dynamic_buffer
        {
        public:
            typedef T     value_type;   ///< Value type
            typedef Alloc allocator;    ///< Allocator type;
            static constexpr const bool c_bExp2 = Exp2; ///< \p Exp2 flag

            /// Rebind buffer for other template parameters
            template <typename Q, typename Alloc2= allocator, bool Exp22 = c_bExp2>
            struct rebind {
                typedef uninitialized_dynamic_buffer<Q, Alloc2, Exp22> other;  ///< Rebinding result type
            };

            //@cond
            typedef typename std::allocator_traits<allocator>::template rebind_alloc<value_type> allocator_type;
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
            uninitialized_dynamic_buffer( size_t nCapacity )
                : m_nCapacity( c_bExp2 ? beans::ceil2(nCapacity) : nCapacity )
            {
                assert( m_nCapacity >= 2 );
                // Capacity must be power of 2
                assert( !c_bExp2 || (m_nCapacity & (m_nCapacity - 1)) == 0 );

                m_buffer = allocator_type().allocate( m_nCapacity );
            }

            /// Destroys dynamically allocated buffer
            ~uninitialized_dynamic_buffer()
            {
                allocator_type().deallocate( m_buffer, m_nCapacity );
            }

            uninitialized_dynamic_buffer( const uninitialized_dynamic_buffer& ) = delete;
            uninitialized_dynamic_buffer& operator =( const uninitialized_dynamic_buffer& ) = delete;

            /// Get item \p i
            value_type& operator []( size_t i )
            {
                assert( i < capacity());
                return m_buffer[i];
            }

            /// Get item \p i, const version
            const value_type& operator []( size_t i ) const
            {
                assert( i < capacity());
                return m_buffer[i];
            }

            /// Returns buffer capacity
            size_t capacity() const noexcept
            {
                return m_nCapacity;
            }

            /// Zeroize the buffer
            void zeroize()
            {
                memset( m_buffer, 0, capacity() * sizeof(m_buffer[0]));
            }

            /// Returns pointer to buffer array
            value_type * buffer() noexcept
            {
                return m_buffer;
            }

            /// Returns pointer to buffer array
            value_type * buffer() const noexcept
            {
                return m_buffer;
            }

            /// Returns <tt> idx % capacity() </tt>
            /**
                If the buffer size is a power of two, binary arithmethics is used
                instead of modulo arithmetics
            */
            size_t mod( size_t idx )
            {
                constexpr_if ( c_bExp2 )
                    return idx & ( capacity() - 1 );
                else
                    return idx % capacity();
            }

            //@cond
            template <typename I>
            typename std::enable_if< sizeof( I ) != sizeof( size_t ), size_t >::type mod( I idx )
            {
                constexpr_if ( c_bExp2 )
                    return static_cast<size_t>( idx & static_cast<I>( capacity() - 1 ));
                else
                    return static_cast<size_t>( idx % capacity());
            }
            //@endcond
        };


        /// Dynamically allocated initialized buffer
        /**
            One of available type for \p opt::buffer option.

            This buffer maintains dynamically allocated array of initialized default-constructed elements.
            Allocation is performed at construction time.

            \par Template parameters:
                - \p T - item type storing in the buffer
                - \p Alloc - an allocator used for allocating internal buffer (\p std::allocator interface)
                - \p Exp2 - a boolean flag. If it is \p true the buffer capacity must be power of two.
                    Otherwise it can be any positive number. Usually, it is required that the buffer has
                    size of a power of two.
        */
        template <typename T, class Alloc = CDS_DEFAULT_ALLOCATOR, bool Exp2 = true>
        class initialized_dynamic_buffer
        {
        public:
            typedef T     value_type;   ///< Value type
            typedef Alloc allocator;    ///< Allocator type
            static constexpr const bool c_bExp2 = Exp2; ///< \p Exp2 flag

            /// Rebind buffer for other template parameters
            template <typename Q, typename Alloc2= allocator, bool Exp22 = c_bExp2>
            struct rebind {
                typedef initialized_dynamic_buffer<Q, Alloc2, Exp22> other;  ///< Rebinding result type
            };

            //@cond
            typedef cds::details::Allocator<value_type, allocator>   allocator_type;
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
            initialized_dynamic_buffer( size_t nCapacity )
                : m_nCapacity( c_bExp2 ? beans::ceil2(nCapacity) : nCapacity )
            {
                assert( m_nCapacity >= 2 );
                // Capacity must be power of 2
                assert( !c_bExp2 || (m_nCapacity & (m_nCapacity - 1)) == 0 );

                allocator_type a;
                m_buffer = a.NewArray( m_nCapacity );
            }

            /// Destroys dynamically allocated buffer
            ~initialized_dynamic_buffer()
            {
                allocator_type a;
                a.Delete( m_buffer, m_nCapacity );
            }

            initialized_dynamic_buffer( const initialized_dynamic_buffer& ) = delete;
            initialized_dynamic_buffer& operator =( const initialized_dynamic_buffer& ) = delete;

            /// Get item \p i
            value_type& operator []( size_t i )
            {
                assert( i < capacity());
                return m_buffer[i];
            }

            /// Get item \p i, const version
            const value_type& operator []( size_t i ) const
            {
                assert( i < capacity());
                return m_buffer[i];
            }

            /// Returns buffer capacity
            size_t capacity() const noexcept
            {
                return m_nCapacity;
            }

            /// Zeroize the buffer
            void zeroize()
            {
                memset( m_buffer, 0, capacity() * sizeof(m_buffer[0]));
            }

            /// Returns pointer to buffer array
            value_type * buffer() noexcept
            {
                return m_buffer;
            }

            /// Returns pointer to buffer array
            value_type * buffer() const noexcept
            {
                return m_buffer;
            }

            /// Returns <tt> idx % capacity() </tt>
            /**
            If the buffer size is a power of two, binary arithmethics is used
            instead of modulo arithmetics
            */
            size_t mod( size_t idx )
            {
                constexpr_if ( c_bExp2 )
                    return idx & ( capacity() - 1 );
                else
                    return idx % capacity();
            }

            //@cond
            template <typename I>
            typename std::enable_if< sizeof( I ) != sizeof( size_t ), size_t >::type mod( I idx )
            {
                constexpr_if ( c_bExp2 )
                    return static_cast<size_t>( idx & static_cast<I>( capacity() - 1 ));
                else
                    return static_cast<size_t>( idx % capacity());
            }
            //@endcond
        };

    }   // namespace v

}}  // namespace cds::opt

#endif // #ifndef CDSLIB_OPT_BUFFER_H
