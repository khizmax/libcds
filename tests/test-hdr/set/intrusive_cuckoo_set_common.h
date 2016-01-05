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

#ifndef CDSTEST_HDR_INTRUSIVE_CUCKOO_SET_COMMON_H
#define CDSTEST_HDR_INTRUSIVE_CUCKOO_SET_COMMON_H

namespace set {
    namespace {

        typedef IntrusiveCuckooSetHdrTest::hash_int    hash1;
        struct hash2: private hash1
        {
            typedef hash1 base_class;

            size_t operator()( int i ) const
            {
                size_t h = ~( base_class::operator()(i));
                return ~h + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            template <typename Item>
            size_t operator()( const Item& i ) const
            {
                size_t h = ~( base_class::operator()(i));
                return ~h + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            size_t operator()( IntrusiveCuckooSetHdrTest::find_key const& i) const
            {
                size_t h = ~( base_class::operator()(i));
                return ~h + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
        };

        template <typename T>
        struct equal_to
        {
            bool operator ()(const T& v1, const T& v2 ) const
            {
                return v1.key() == v2.key();
            }

            template <typename Q>
            bool operator ()(const T& v1, const Q& v2 ) const
            {
                return v1.key() == v2;
            }

            template <typename Q>
            bool operator ()(const Q& v1, const T& v2 ) const
            {
                return v1 == v2.key();
            }
        };

    }
} // namespace set

#endif // #ifndef CDSTEST_HDR_INTRUSIVE_CUCKOO_SET_COMMON_H
