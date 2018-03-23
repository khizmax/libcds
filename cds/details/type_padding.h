// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

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

        using T::T;
    };
    template <typename T, int Align>
    struct type_padding_helper<T, Align, 0>: public T
    {
        enum {
            value = 0
        };

        using T::T;
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
        /// Align factor
        enum {
            align_factor = AlignFactor <= 0 ? 1 : AlignFactor
        };

        /// Result type
        typedef type_padding_helper<T, align_factor, sizeof(T) % align_factor> type;

        /// Padding constant
        enum {
            value = type::value
        };
    };

}}   // namespace cds::details
#endif // #ifndef CDSLIB_DETAILS_TYPE_PADDING_H
