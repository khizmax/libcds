/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

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

#ifndef CDSLIB_DETAILS_BIT_REVERSE_COUNTER_H
#define CDSLIB_DETAILS_BIT_REVERSE_COUNTER_H

#include <cds/algo/bitop.h>

//@cond
namespace cds { namespace bitop {

    template <typename Counter = size_t>
    class bit_reverse_counter
    {
    public:
        typedef Counter     counter_type;

    private:
        counter_type        m_nCounter;
        counter_type        m_nReversed;
        int                 m_nHighBit;

    public:
        bit_reverse_counter()
            : m_nCounter(0)
            , m_nReversed(0)
            , m_nHighBit(-1)
        {}

        counter_type inc()
        {
            ++m_nCounter;
            int nBit;
            for ( nBit = m_nHighBit - 1; nBit >= 0; --nBit ) {
                if ( !cds::bitop::complement( m_nReversed, nBit ))
                    break;
            }
            if ( nBit < 0 ) {
                m_nReversed = m_nCounter;
                ++m_nHighBit;
            }
            return m_nReversed;
        }

        counter_type dec()
        {
            counter_type ret = m_nReversed;
            --m_nCounter;
            int nBit;
            for ( nBit = m_nHighBit - 1; nBit >= 0; --nBit ) {
                if ( cds::bitop::complement( m_nReversed, nBit ))
                    break;
            }
            if ( nBit < 0 ) {
                m_nReversed = m_nCounter;
                --m_nHighBit;
            }
            return ret;
        }

        counter_type    value() const
        {
            return m_nCounter;
        }

        counter_type    reversed_value() const
        {
            return m_nReversed;
        }

        int high_bit() const
        {
            return m_nHighBit;
        }
    };

}}   // namespace cds::bitop
//@endcond

#endif // #ifndef CDSLIB_DETAILS_BIT_REVERSE_COUNTER_H
