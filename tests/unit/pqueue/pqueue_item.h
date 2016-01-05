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

#ifndef CDSUNIT_PQUEUE_ITEM_H
#define CDSUNIT_PQUEUE_ITEM_H

namespace pqueue {
    struct SimpleValue {
        size_t      key;

        typedef size_t  key_type;

        struct key_extractor {
            void operator()( key_type& k, SimpleValue const& s ) const
            {
                k = s.key;
            }
        };

        SimpleValue(): key(0) {}
        SimpleValue( size_t n ): key(n) {}
    };
}

namespace std {
    template <>
    struct less<pqueue::SimpleValue>
    {
        bool operator()( pqueue::SimpleValue const& k1, pqueue::SimpleValue const& k2 ) const
        {
            return k1.key < k2.key;
        }

        bool operator()( pqueue::SimpleValue const& k1, size_t k2 ) const
        {
            return k1.key < k2;
        }

        bool operator()( size_t k1, pqueue::SimpleValue const& k2 ) const
        {
            return k1 < k2.key;
        }

        bool operator()( size_t k1, size_t k2 ) const
        {
            return k1 < k2;
        }
    };

    template <>
    struct greater<pqueue::SimpleValue>
    {
        bool operator()( pqueue::SimpleValue const& k1, pqueue::SimpleValue const& k2 ) const
        {
            return k1.key > k2.key;
        }

        bool operator()( pqueue::SimpleValue const& k1, size_t k2 ) const
        {
            return k1.key > k2;
        }

        bool operator()( size_t k1, pqueue::SimpleValue const& k2 ) const
        {
            return k1 > k2.key;
        }

        bool operator()( size_t k1, size_t k2 ) const
        {
            return k1 > k2;
        }
    };

} // namespace std

#endif // #ifndef CDSUNIT_PQUEUE_ITEM_H
