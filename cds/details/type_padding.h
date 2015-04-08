//$$CDS-header$$

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
