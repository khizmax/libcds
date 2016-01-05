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

#ifndef CDSLIB_DETAILS_TYPE_PADDING_H
#define CDSLIB_DETAILS_TYPE_PADDING_H

namespace cds { namespace details {

    //@cond none
    template <typename T, int Align, int Modulo>
    struct type_padding_helper: public T
    {
        enum {
            value = Modulo
        };
        char _[Align - Modulo]   ;   // padding

        type_padding_helper() CDS_NOEXCEPT_( noexcept( T() ))
        {}
    };
    template <typename T, int Align>
    struct type_padding_helper<T, Align, 0>: public T
    {
        enum {
            value = 0
        };

        type_padding_helper() CDS_NOEXCEPT_( noexcept( T()) )
        {}
    };
    //@endcond

    /// Automatic alignment type \p T to \p AlignFactor
    /**
        The class adds appropriate bytes to type T that the following condition is true:
        \code
        sizeof( type_padding<T,AlignFactor>::type ) % AlignFactor == 0
        \endcode
        It is guaranteed that count of padding bytes no more than AlignFactor - 1.

        \b Applicability: type \p T must not have constructors another that default ctor.
        For example, \p T may be any POD type.
    */
    template <typename T, int AlignFactor>
    class type_padding {
    public:
        /// Result type
        typedef type_padding_helper<T, AlignFactor, sizeof(T) % AlignFactor>    type;

        /// Padding constant
        enum {
            value = type::value
        };
    };

}}   // namespace cds::details
#endif // #ifndef CDSLIB_DETAILS_TYPE_PADDING_H
