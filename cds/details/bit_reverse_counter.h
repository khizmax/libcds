// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

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
