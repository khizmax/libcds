// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_ALGO_SPLIT_BITSTRING_H
#define CDSLIB_ALGO_SPLIT_BITSTRING_H

#include <cds/algo/base.h>

namespace cds { namespace algo {

    /// Cuts a bit sequence from fixed-size bit-string
    /**
        The splitter can be used as an iterator over bit-string.
        Each call of \p cut() or \p safe_cut() cuts the bit count specified
        and keeps the position inside bit-string for the next call.

        The splitter stores a const reference to bit-string, not a copy.
        The maximum count of bits that can be cut in a single call is <tt> sizeof(UInt) * 8 </tt>

        The splitter keeps byte order.

        Template parameters:
        - \p BitString - a fixed-sized type that interprets as bit string
        - \p BitStringSize - the size of \p BitString in bytes, default is <tt>sizeof( BitString )</tt>.
             You can specify 0 for default.
        - \p UInt - an unsigned integer, return type for \p cut(), default is \p unsigned

        There are specialized splitters:
        - a simplified \p byte_splitter algorithm that is suitable when count is multiple of 8.
        - \p number_splitter algorithm is suitable for a number
    */
    template <typename BitString, size_t BitStringSize = sizeof( BitString ), typename UInt = unsigned >
    class split_bitstring
    {
    public:
        typedef BitString bitstring;    ///< Bit-string type
        typedef UInt      uint_type;    ///< Result type of \p cut() function
        static constexpr size_t const c_bitstring_size = BitStringSize ? BitStringSize : sizeof( BitString ); ///< size of \p BitString in bytes

        //@cond
        static constexpr unsigned const c_nBitPerByte = 8;
        //@endcond

    public:
        /// Initializises the splitter with reference to \p h and zero start bit offset
        explicit split_bitstring( bitstring const& h )
            : cur_( reinterpret_cast<uint8_t const*>( &h ))
            , offset_( 0 )
            , first_( cur_ )
            , last_( cur_ + c_bitstring_size )
        {}

        /// Initializises the splitter with reference to \p h and start bit offset \p nBitOffset
        split_bitstring( bitstring const& h, size_t nBitOffset )
            : cur_( reinterpret_cast<uint8_t const*>( &h ) + nBitOffset / c_nBitPerByte )
            , offset_( nBitOffset % c_nBitPerByte  )
            , first_( reinterpret_cast<uint8_t const*>( &h ))
            , last_( first_ + c_bitstring_size )
        {}

        /// Returns \p true if end-of-string is not reached yet
        explicit operator bool() const
        {
            return !eos();
        }

        /// Returns \p true if end-of-stream encountered
        bool eos() const
        {
            return cur_ >= last_;
        }

        /// Cuts next \p count bits from bit-string
        /**
            For performance reason, the function does not manage out-of-bound condition.
            To control that use \p safe_cut().
        */
        uint_type cut( unsigned count )
        {
            assert( !eos());

            uint_type result = 0;
#       if defined( CDS_ARCH_LITTLE_ENDIAN )
            for ( unsigned done = 0; done < count; ) {
                assert( cur_ < last_ );
                unsigned bits = count - done;
                if ( bits > c_nBitPerByte - offset_ )
                    bits = c_nBitPerByte - offset_;

                result |= static_cast<uint_type>(( *cur_ >> offset_ ) & (( 1 << bits ) - 1 )) << done;

                offset_ += bits;
                assert( offset_ <= c_nBitPerByte );
                if ( offset_ == c_nBitPerByte ) {
                    offset_ = 0;
                    ++cur_;
                }
                done += bits;
            }
#       else
            while ( count ) {
                assert( cur_ < last_ );

                unsigned bits = count <= ( c_nBitPerByte - offset_ ) ? count : c_nBitPerByte - offset_;

                result = ( result << bits ) | (( *cur_ >> offset_ ) & ( ( 1 << bits ) - 1 ));

                offset_ += bits;
                assert( offset_ <= c_nBitPerByte );
                if ( offset_ == c_nBitPerByte ) {
                    offset_ = 0;
                    ++cur_;
                }
                count -= bits;
            }
#       endif

            return result;
        }

        /// Cuts up to \p count from the bit-string
        /**
            Safe analog of \p cut() but if \p count is more than the rest of bit-string,
            only the rest is returned.
            When \p eos() condition is met the function returns 0.
        */
        uint_type safe_cut( unsigned count )
        {
            if ( eos())
                return 0;

            unsigned const rest = static_cast<unsigned>( last_ - cur_ - 1 ) * c_nBitPerByte + ( c_nBitPerByte - offset_ );
            if ( rest < count )
                count = rest;
            return count ? cut( count ) : 0;
        }

        /// Resets the splitter
        void reset() noexcept
        {
            cur_ = first_;
            offset_ = 0;
        }

        /// Returns pointer to source bitstring
        bitstring const * source() const
        {
            return reinterpret_cast<bitstring const *>( first_ );
        }

        /// Returns current bit offset from beginning of bit-string
        size_t bit_offset() const
        {
            return offset_ + (cur_ - first_) * c_nBitPerByte;
        }

        /// Returns how many bits remain
        size_t rest_count() const
        {
            return c_bitstring_size * c_nBitPerByte - bit_offset();
        }

        /// Returns \p true for any argument
        static constexpr bool is_correct( unsigned /*count*/ )
        {
            return true;
        }

    private:
        //@cond
        uint8_t const*  cur_;
        unsigned        offset_;
        uint8_t const* const    first_;
        uint8_t const* const    last_;
        //@endcond
    };

    /// Simplified \p split_bitstring algorithm when \p count is multiple of 8
    template <typename BitString, size_t BitStringSize = sizeof( BitString ), typename UInt = unsigned >
    class byte_splitter
    {
    public:
        typedef BitString bitstring;    ///< Bit-string type
        typedef UInt      uint_type;    ///< Result type of \p cut() function
        static constexpr size_t const c_bitstring_size = BitStringSize ? BitStringSize : sizeof( BitString ); ///< size of \p BitString in bytes

        //@cond
        static constexpr unsigned const c_nBitPerByte = 8;
        //@endcond

    public:
        /// Initializises the splitter with reference to \p h and zero start bit offset
        explicit byte_splitter( bitstring const& h )
            : cur_( reinterpret_cast<uint8_t const*>( &h ))
            , first_( cur_ )
            , last_( cur_ + c_bitstring_size )
        {}

        /// Initializises the splitter with reference to \p h and start bit offset \p nBitOffset
        byte_splitter( bitstring const& h, size_t nBitOffset )
            : cur_( reinterpret_cast<uint8_t const*>( &h ) + nBitOffset / c_nBitPerByte )
            , first_( reinterpret_cast<uint8_t const*>( &h ))
            , last_( first_ + c_bitstring_size )
        {
            assert( is_correct( static_cast<unsigned>( nBitOffset )));
            assert( !eos());
        }

        /// Returns \p true if end-of-string is not reached yet
        explicit operator bool() const
        {
            return !eos();
        }

        /// Returns \p true if end-of-stream encountered
        bool eos() const
        {
            return cur_ >= last_;
        }

        /// Cuts next \p count bits (must be multiplier of 8) from bit-string
        /**
            For performance reason, the function does not manage out-of-bound condition.
            To control that use \p safe_cut().
        */
        uint_type cut( unsigned count )
        {
            assert( !eos());
            assert( is_correct( count ));

            uint_type result = 0;

#       if defined( CDS_ARCH_LITTLE_ENDIAN )
            for ( unsigned i = 0; i < count; i += c_nBitPerByte ) {
                result |= static_cast<uint_type>( *cur_ ) << i;
                ++cur_;
            }
#       else
            for ( ; count; count -= c_nBitPerByte ) {
                result = ( result << c_nBitPerByte ) | *cur_;
                ++cur_;
            }
#       endif

            return result;
        }

        /// Cuts up to \p count from the bit-string
        /**
            Safe analog of \p cut(): if \p count is more than the rest of bit-string,
            only the rest is returned.
            When \p eos() condition is met the function returns 0.
        */
        uint_type safe_cut( unsigned count )
        {
            if ( eos())
                return 0;

            unsigned const rest = static_cast<unsigned>( last_ - cur_ - 1 ) * c_nBitPerByte;
            if ( rest < count )
                count = rest;
            return count ? cut( count ) : 0;
        }

        /// Resets the splitter
        void reset() noexcept
        {
            cur_ = first_;
        }

        /// Returns pointer to source bitstring
        bitstring const* source() const
        {
            return reinterpret_cast<bitstring const *>( first_ );
        }

        /// Returns current bit offset from beginning of bit-string
        size_t bit_offset() const
        {
            return (cur_ - first_) * c_nBitPerByte;
        }

        /// Returns how many bits remain
        size_t rest_count() const
        {
            return c_bitstring_size * c_nBitPerByte - bit_offset();
        }

        /// Checks if \p count is multiple of 8
        static constexpr bool is_correct( unsigned count )
        {
            return count % 8 == 0;
        }

    private:
        //@cond
        uint8_t const*  cur_;
        uint8_t const* const    first_;
        uint8_t const* const    last_;
        //@endcond
    };


    /// Cuts a bit sequence from a number
    /**
        The splitter can be used as an iterator over bit representation of the number of type \p Int.
        Each call of \p cut() or \p safe_cut() cuts the bit count specified
        and keeps the position inside the number for the next call.
    */
    template <typename Int>
    class number_splitter
    {
    public:
        typedef Int       int_type;     ///< Number type
        typedef Int       uint_type;    ///< Result type of \p cut() function

        //@cond
        static constexpr unsigned const c_nBitPerByte = 8;
        //@endcond

    public:
        /// Initalizes the splitter with nymber \p n and initial bit offset 0
        explicit number_splitter( int_type n )
            : number_( n )
            , shift_( 0 )
        {}

        /// Initalizes the splitter with nymber \p n and initial bit offset \p initial_offset
        number_splitter( int_type n, size_t initial_offset )
            : number_( n )
            , shift_( static_cast<unsigned>( initial_offset ))
        {
            assert( initial_offset < sizeof( int_type ) * c_nBitPerByte );
        }

        /// Returns \p true if end-of-string is not reached yet
        explicit operator bool() const
        {
            return !eos();
        }

        /// Returns \p true if end-of-stream encountered
        bool eos() const
        {
            return shift_ >= sizeof( int_type ) * c_nBitPerByte;
        }

        /// Cuts next \p count bits (must be multiplier of 8) from the number
        /**
            For performance reason, the function does not manage out-of-bound condition.
            To control that use \p safe_cut().
        */
        int_type cut( unsigned count )
        {
            assert( !eos());
            assert( is_correct( count ));

            int_type result = ( number_ >> shift_ ) & (( 1 << count ) - 1 );
            shift_ += count;

            return result;
        }

        /// Cuts up to \p count from the bit-string
        /**
            Safe analog of \p cut(): if \p count is more than the rest of \p int_type,
            only the rest is returned.
            When \p eos() condition is met the function returns 0.
        */
        int_type safe_cut( unsigned count )
        {
            if ( eos())
                return 0;

            unsigned rest = static_cast<unsigned>( rest_count());
            if ( rest < count )
                count = rest;
            return count ? cut( count ) : 0;
        }

        /// Resets the splitter
        void reset() noexcept
        {
            shift_ = 0;
        }

        /// Returns initial number
        int_type source() const
        {
            return number_;
        }

        /// Returns current bit offset from beginning of the number
        size_t bit_offset() const
        {
            return shift_;
        }

        /// Returns how many bits remain
        size_t rest_count() const
        {
            return sizeof( int_type ) * c_nBitPerByte - shift_;
        }

        /// Checks if \p count is multiple of 8
        static constexpr bool is_correct( unsigned count )
        {
            return count < sizeof( int_type ) * c_nBitPerByte;
        }

    private:
        //@cond
        int_type const  number_;
        unsigned        shift_;
        //@endcond
    };

    /// Metafunctin to select a most suitable splitter for type \p BitString of size \p BitStringSize
    template <typename BitString, size_t BitStringSize >
    struct select_splitter
    {
        typedef split_bitstring< BitString, BitStringSize > type; ///< metafunction result
    };

    //@cond
#   define CDS_SELECT_NUMBER_SPLITTER( num_type ) \
        template <> struct select_splitter<num_type, sizeof(num_type)> { typedef number_splitter<num_type> type; }

    CDS_SELECT_NUMBER_SPLITTER( int );
    CDS_SELECT_NUMBER_SPLITTER( unsigned );
    CDS_SELECT_NUMBER_SPLITTER( short );
    CDS_SELECT_NUMBER_SPLITTER( unsigned short );
    CDS_SELECT_NUMBER_SPLITTER( long );
    CDS_SELECT_NUMBER_SPLITTER( unsigned long );
    CDS_SELECT_NUMBER_SPLITTER( long long );
    CDS_SELECT_NUMBER_SPLITTER( unsigned long long );

#   undef CDS_SELECT_NUMBER_SPLITTER
    //@endcond

}} // namespace cds::algo

#endif // #ifndef CDSLIB_ALGO_SPLIT_BITSTRING_H
