//$$CDS-header$$

#ifndef __CDS_NUMERIC_TRAITS_H
#define __CDS_NUMERIC_TRAITS_H

/*
    Filename: numtraits.h
    Created 2007.04.22 by Maxim.Khiszinsky

    Description:
        Various numeric constants and algorithms
        Many algorithms are static (compile-time)
        Result of static algorithm is the constant (enum) called "result".

    Editions:
        2007.04.22  Maxim.Khiszinsky    Created
        2007.07.20  Maxim.Khiszinsky    Added functions: exponent2, exp2Ceil
*/

namespace cds {
    /// Some helper compile-time tricks
    namespace beans {

        // @cond details
        namespace details {
            template <int N, int MOD> struct Exponent2Helper;
            template <int N> struct Exponent2Helper< N, 0 > {
                enum { result = Exponent2Helper< N / 2, N % 2 >::result + 1 };
            };
            template <> struct Exponent2Helper< 1, 0 > {
                enum { result = 0 };
            };
        }
        // @endcond

        /*! Compile-time computing of log2(N)

            If N = 2**k for some natural k then Exponent2<N>::result = k
            If N != 2**k for any natural k then compile-time error has been encountered
        */
        template <int N> struct Exponent2 {
            enum {
                native    = N,
                base    = 2,
                result    = details::Exponent2Helper< N / 2, N % 2 >::result + 1
            };
        };
        //@cond details
        template <> struct Exponent2<1> {
            enum {
                native    = 1,
                base    = 2,
                result    = 0
            };
        };
        //@endcond

        /// Returns @a N: 2**N is nearest to @p nNumber, 2**N < nNumber
        static inline size_t exp2Ceil( size_t nNumber )
        {
            static_assert( sizeof(size_t) == (CDS_BUILD_BITS / 8), "Internal assumption error" );

            size_t nExp = 0;
            size_t nBit = CDS_BUILD_BITS - 1;
#if CDS_BUILD_BITS == 32
            size_t nMask = 0x80000000;
#else
            size_t nMask = 0x8000000000000000;
#endif
            while ( nMask != 0 ) {
                if ( nNumber & nMask ) {
                    nExp = nBit;
                    break;
                }
                nMask = nMask >> 1;
                --nBit;
            }
            if ( ( nNumber % ( ((size_t) 1) << nExp )) > ( ((size_t) 1) << (nExp - 1)) )
                ++nExp;
            return nExp;
        }

        /* ExponentN< int BASE, int N >
            Exponent
            If N = BASE**k then the algorithm returns k
            Else compile-time error is encountered
        */
        //@cond details
        namespace details {
            template <int N, int BASE, int MOD> struct ExponentNHelper;
            template <int N, int BASE> struct ExponentNHelper< N, BASE, 0 > {
                enum { result = ExponentNHelper< N / BASE, BASE, N % BASE >::result + 1 };
            };
            template <int BASE> struct ExponentNHelper< 1, BASE, 0 > {
                enum { result = 0 };
            };
        }
        //@endcond

        /// Compile-time computing log(@p N) based @p BASE. Result in @a Exponent<BASE, N>::result
        template <int BASE, int N> struct ExponentN {
            enum {
                native    = N,
                base    = BASE,
                result    = details::ExponentNHelper< N / BASE, BASE, N % BASE >::result + 1
            };
        };
        //@cond
        template <int BASE> struct ExponentN< BASE, 1 > {
            enum {
                native    = 1,
                base    = BASE,
                result    = 0
            };
        };
        template <int BASE> struct ExponentN< BASE, 0 >;
        //@endcond

        //@cond none
        template <int N> struct Power2 {
            enum {
                exponent = N,
                result = 1 << N
            };
        };
        template <> struct Power2<0> {
            enum {
                exponent = 0,
                result = 1
            };
        };
        //@endcond

        //@cond none
        template <int BASE, int N > struct PowerN {
            enum {
                exponent = N,
                base     = BASE,
                result = PowerN< BASE, N - 1 >::result * BASE
            };
        };
        template <int BASE> struct PowerN<BASE, 0> {
            enum {
                exponent = 0,
                base     = BASE,
                result     = 1
            };
        };
        //@endcond

        //@cond none
        namespace details {
            template <int N, int ALIGN, int MOD> struct NearestCeilHelper {
                enum { result = N + ALIGN - MOD };
            };
            template <int N, int ALIGN> struct NearestCeilHelper< N, ALIGN, 0> {
                enum { result = N };
            };
        }
        template <int N, int ALIGN> struct NearestCeil {
            enum {
                native    = N,
                align    = ALIGN,
                result    = details::NearestCeilHelper< N, ALIGN, N % ALIGN >::result
            };
        };
        //@endcond

        //@cond none
        template <typename T, int ALIGN> struct AlignedSize {
            typedef T    NativeType;
            enum {
                nativeSize    = sizeof(T),
                result        = NearestCeil< sizeof(T), ALIGN >::result,
                alignBytes    = result - nativeSize,
                alignedSize = result
            };
        };
        //@endcond

        //@cond none
        namespace details {
            template < int N1, int N2, bool LESS > struct Max;
            template < int N1, int N2 >
            struct Max< N1, N2, true > {
                enum { result = N2  };
            };

            template < int N1, int N2 >
            struct Max< N1, N2, false > {
                enum { result = N1  };
            };

            template < int N1, int N2, bool LESS > struct Min;
            template < int N1, int N2 >
            struct Min< N1, N2, true > {
                enum { result = N1  };
            };

            template < int N1, int N2 >
            struct Min< N1, N2, false > {
                enum { result = N2  };
            };
        }
        //@endcond

        /// Returns max(N1, N2) as Max<N1,N2>::result
        template <int N1, int N2>
        struct Max {
            enum { result = details::Max< N1, N2, N1 < N2 >::result  };
        };

        /// Returns min(N1, N2) as Min<N1,N2>::result
        template <int N1, int N2>
        struct Min {
            enum { result = details::Min< N1, N2, N1 < N2 >::result  };
        };

    }    // namespace beans
}    // namespace cds

#endif    // __CDS_NUMERIC_TRAITS_H
