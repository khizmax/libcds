//$$CDS-header$$

#ifndef CDSLIB_COMPILER_GCC_IA64_BITOP_H
#define CDSLIB_COMPILER_GCC_IA64_BITOP_H

//@cond none
namespace cds {
    namespace bitop { namespace platform { namespace gcc { namespace ia64 {

        // MSB - return index (1..32) of most significant bit in x. If x == 0 return 0
#        define cds_bitop_msb32_DEFINED
        static inline int msb32( uint32_t nArg )
        {
            if ( !nArg )
                return 0;
            atomic64u_t x = nArg;
            x |= x >> 1;
            x |= x >> 2;
            x |= x >> 4;
            x |= x >> 8;
            x |= x >> 16;

            atomic64u_t    nRes;
            asm __volatile__( "popcnt %0=%1\n\t" : "=r" (nRes) : "r" (x) );
            return (int) nRes;
        }

        // It is not compiled on HP-UX. Why?..
#if CDS_OS_TYPE != CDS_OS_HPUX
        // MSB - return index (0..31) of most significant bit in nArg.
        // !!! nArg != 0
#        define cds_bitop_msb32nz_DEFINED
        static inline int msb32nz( uint32_t nArg )
        {
            assert( nArg != 0 );
            long double d = nArg;
            long nExp;
            asm __volatile__("getf.exp %0=%1\n\t" : "=r"(nExp) : "f"(d));
            return (int) (nExp - 0xffff);
        }

        // MSB - return index (0..63) of most significant bit in nArg.
        // !!! nArg != 0
#        define cds_bitop_msb64nz_DEFINED
        static inline int msb64nz( atomic64u_t nArg )
        {
            assert( nArg != 0 );
            long double d = nArg;
            long nExp;
            asm __volatile__("getf.exp %0=%1\n\t" : "=r" (nExp) : "f" (d));
            return (int) (nExp - 0xffff);
        }
#endif    // #if CDS_OS_TYPE != CDS_OS_HPUX

    }} // namespace gcc::ia64

    using namespace gcc::ia64;

}}}    // namespace cds::bitop::platform
//@endcond

#endif // #ifndef CDSLIB_COMPILER_GCC_IA64_BITOP_H
