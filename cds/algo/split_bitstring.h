/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.     
*/

#ifndef CDSLIB_ALGO_SPLIT_BITSTRING_H
#define CDSLIB_ALGO_SPLIT_BITSTRING_H

#include <cds/algo/base.h>

namespace cds { namespace algo {

    /// Cuts a bit sequence from fixed-size bit-string
    /**
        The splitter can be used as iterator over bit-string.
        Each call of \p cut() or \p safe_cut() cuts the bit count specified
        and keeps the position inside bit-string for the next call.

        The splitter stores a const reference to bit-string, not a copy.
        The maximum count of bits that can be cut in a single call is <tt> sizeof(UInt) * 8 </tt>
    */
    template <typename BitString, typename UInt = size_t >
    class split_bitstring
    {
    public:
        typedef BitString bitstring;    ///< Bit-string type
        typedef UInt      uint_type;    ///< Bit-string portion type

        //@cond
        static CDS_CONSTEXPR size_t const c_nHashSize   = (sizeof(bitstring) + sizeof(uint_type) - 1) / sizeof(uint_type);
        static CDS_CONSTEXPR size_t const c_nBitPerByte = 8;
        static CDS_CONSTEXPR size_t const c_nBitPerHash = sizeof(bitstring) * c_nBitPerByte;
        static CDS_CONSTEXPR size_t const c_nBitPerInt  = sizeof(uint_type) * c_nBitPerByte;
        //@endcond

    public:
        /// Initializises the splitter with reference to \p h and zero start bit offset
        explicit split_bitstring( bitstring const& h )
            : m_ptr(reinterpret_cast<uint_type const*>( &h ))
            , m_offset(0)
            , m_first( m_ptr )
#   ifdef _DEBUG
            , m_last( m_ptr + c_nHashSize )
#   endif
        {}

        /// Initializises the splitter with reference to \p h and start bit offset \p nBitOffset
        split_bitstring( bitstring const& h, size_t nBitOffset )
            : m_ptr( reinterpret_cast<uint_type const*>( &h ) + nBitOffset / c_nBitPerInt )
            , m_offset( nBitOffset )
            , m_first( reinterpret_cast<uint_type const*>(&h))
#   ifdef _DEBUG
            , m_last( m_first + c_nHashSize )
#   endif
        {}


        /// Returns \p true if end-of-string is not reached yet
        explicit operator bool() const
        {
            return !eos();
        }

        /// Returns \p true if end-of-stream encountered
        bool eos() const
        {
            return m_offset >= c_nBitPerHash;
        }

        /// Cuts next \p nBits from bit-string
        /**
            Precondition: <tt>nBits <= sizeof(uint_type) * 8</tt>

            This function does not manage out-of-bound condition.
            To control that use \p safe_cut().
        */
        uint_type cut( size_t nBits )
        {
            assert( !eos() );
            assert( nBits <= c_nBitPerInt );
            assert( m_offset + nBits <= c_nBitPerHash );
#   ifdef _DEBUG
            assert( m_ptr < m_last );
#   endif
            uint_type result;

            uint_type const nRest = c_nBitPerInt - m_offset % c_nBitPerInt;
            m_offset += nBits;
            if ( nBits < nRest ) {
                result = *m_ptr << ( nRest - nBits );
                result = result >> ( c_nBitPerInt - nBits );
            }
            else if ( nBits == nRest ) {
                result = *m_ptr >> ( c_nBitPerInt - nRest );
                ++m_ptr;
                assert( m_offset % c_nBitPerInt == 0 );
            }
            else {
                uint_type const lsb = *m_ptr >> ( c_nBitPerInt - nRest );
                nBits -= nRest;
                ++m_ptr;

                result = *m_ptr << ( c_nBitPerInt - nBits );
                result = result >> ( c_nBitPerInt - nBits );
                result = (result << nRest) + lsb;
            }

            assert( m_offset <= c_nBitPerHash );
#   ifdef _DEBUG
            assert( m_ptr <= m_last );
#   endif
            return result;
        }

        /// Cuts up to \p nBits from the bit-string
        /**
            Analog of \p cut() but if \p nBits is more than the rest of bit-string,
            only the rest is returned.
            If \p eos() is \p true the function returns 0.
        */
        uint_type safe_cut( size_t nBits )
        {
            if ( eos() )
                return 0;

            assert( nBits <= sizeof(uint_type) * c_nBitPerByte );

            if ( m_offset + nBits > c_nBitPerHash )
                nBits = c_nBitPerHash - m_offset;
            return nBits ? cut( nBits ) : 0;
        }

        /// Resets the splitter
        void reset() CDS_NOEXCEPT
        {
            m_ptr = m_first;
            m_offset = 0;
        }

        /// Returns pointer to source bitstring
        bitstring const * source() const
        {
            return reinterpret_cast<bitstring const *>( m_first );
        }

        /// Returns current bit offset from beginning of bit-string
        size_t bit_offset() const
        {
            return m_offset;
        }

    private:
        //@cond
        uint_type const* m_ptr;     ///< current position in the hash
        size_t           m_offset;  ///< current bit offset in bit-string
        uint_type const* m_first;   ///< first position
#   ifdef _DEBUG
        uint_type const* m_last;    ///< last position
#   endif
        //@endcond
    };

}} // namespace cds::algo

#endif // #ifndef CDSLIB_ALGO_SPLIT_BITSTRING_H
