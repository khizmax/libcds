// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_OPT_PERMUTATION_H
#define CDSLIB_OPT_PERMUTATION_H

#include <cstdlib> // rand, srand
#include <random>
#include <algorithm> // std::shuffle
#include <numeric>   // std::iota

#include <cds/opt/options.h>

namespace cds { namespace opt {

    /// [type-option] Random permutation generator option setter
    /**
        The class represents permutation generator of unique integers from <tt> [0, nLength) </tt>
        (\p nLenght is not included) in random order.

        The generator interface is:
        \code
        class generator {
            // Initializes the generator of length nLength
            generator( size_t nLength );

            // Returns current value
            operator int();

            // Goes to next value. Returns false if the permutation is exchausted
            bool next();

            // Resets the generator to produce the new sequence
            void reset();
        };
        \endcode

        <b>Usage example</b>

        The permutation generator is intended for <tt>do {} while</tt> loop:
        \code
        permutation_generator gen( 16 );
        int i = gen.begin();
        do {
            int i = gen;
            //...
        } while ( gen.next());

        // Get other permutation
        gen.reset();
        do {
            int i = gen;
            //...
        } while ( gen.next());
        \endcode

        The following \p Generator defined:
        - \p opt::v::random_permutation
        - \p opt::v::random2_permutation
        - \p opt::v::random_shuffle_permutation
        - \p opt::v::skew_permutation
    */
    template <typename Generator>
    struct permutation_generator {
        //@cond
        template <typename Base> struct pack: public Base
        {
            typedef Generator permutation_generator;
        };
        //@endcond
    };

    namespace v {

        /// Permutation generator of arbitrary length based on \p rand()
        /**
            The class is suitable for \p opt::permutation_generator option.

            The generator calculates <tt>n = rand()</tt> and produces the sequence
            <tt>[n % nLen, (n + 1) % nLen, ..., (n + nLen - 1) % nLen]</tt>.
            The generator does not allocate any memory.

            \p Int template argument specifies the type of generated value, it should be any integer.
        */
        template <typename Int=int>
        class random_permutation
        {
        public:
            typedef Int     integer_type;   ///< Type of generated value
        protected:
            //@cond
            integer_type        m_nCur;
            integer_type        m_nStart;
            integer_type const  m_nMod;
            //@endcond

        public:
            /// Initializes the generator of arbitrary length \p nLength
            random_permutation( size_t nLength )
                : m_nCur(0)
                , m_nStart(0)
                , m_nMod( integer_type(nLength))
            {
                reset();
            }

            /// Returns the curent value
            operator integer_type() const
            {
                return m_nCur % m_nMod;
            }

            /// Goes to next value. Returns \p false if the sequence is exhausted
            bool next()
            {
                return (++m_nCur % m_nMod) != m_nStart;
            }

            /// Resets the generator to produce new sequence
            void reset()
            {
                m_nCur = m_nStart = integer_type( std::rand()) % m_nMod;
            }
        };

        /// Permutation generator of power-of-2 length based on \p rand()
        /**
            The class is suitable for \p opt::permutation_generator option.

            The generator calculates <tt>n = rand()</tt> and produces the sequence
            <tt>[n % nLen, (n + 1) % nLen, ..., (n + nLen - 1) % nLen]</tt>.
            The generator does not allocate any memory.
            \p nLen must be power of two.

            \p Int template argument specifies the type of generated value, it should be any integer.
        */
        template <typename Int=int>
        class random2_permutation
        {
        public:
            typedef Int     integer_type;   ///< Type of generated value

        protected:
            //@cond
            integer_type        m_nCur;
            integer_type        m_nStart;
            integer_type const  m_nMask;
            //@endcond

        public:
            /// Initializes the generator of length \p nLength
            /**
                An assertion is raised if \p nLength is not a power of two.
            */
            random2_permutation( size_t nLength )
                : m_nCur(0)
                , m_nStart(0)
                , m_nMask( integer_type(nLength) - 1 )
            {
                // nLength must be power of two
                assert( (nLength & (nLength - 1)) == 0 );
                reset();
            }

            /// Returns the current value
            operator integer_type() const
            {
                return m_nCur & m_nMask;
            }

            /// Goes to next value. Returns \p false if the sequence is exhausted
            bool next()
            {
                return (++m_nCur & m_nMask) != m_nStart;
            }

            /// Resets the generator to produce new sequence
            void reset()
            {
                m_nCur = m_nStart = integer_type( std::rand()) & m_nMask;
            }
        };

        /// Permutation generator based on \p std::shuffle
        /**
            The class is suitable for \p opt::permutation_generator option.

            The generator produces a permutation of <tt>[0, nLen)</tt> sequence.
            The generator instance allocates a memory block.

            Template parameters:
            - \p Int - specifies the type of generated value, it should be any integer.
            - \p RandomGenerator - random generator, default is \p std::mt19937
            - \p RandomDevice - random device, default is \p std::random_device
        */
        template <typename Int=int, class RandomGenerator = std::mt19937, class RandomDevice = std::random_device >
        class random_shuffle_permutation
        {
        public:
            typedef Int     integer_type;             ///< Type of generated value
            typedef RandomGenerator random_generator; ///< random generator
            typedef RandomDevice random_device;       ///< random device

        protected:
            //@cond
            integer_type *      m_pCur;
            integer_type *      m_pFirst;
            integer_type *      m_pLast;
            //@endcond

        public:
            /// Initializes the generator of arbitrary length \p nLength
            random_shuffle_permutation( size_t nLength )
                : m_pCur( nullptr )
            {
                m_pFirst = new integer_type[nLength];
                m_pLast = m_pFirst + nLength;
                std::iota(m_pFirst, m_pLast, integer_type{0} );
                reset();
            }

            ~random_shuffle_permutation()
            {
                delete [] m_pFirst;
            }

            /// Returns the current value
            operator integer_type() const
            {
                return *m_pCur;
            }

            /// Goes to next value. Returns \p false if the sequence is exhausted
            bool next()
            {
                return ++m_pCur < m_pLast;
            }

            /// Resets the generator to produce new sequence
            void reset()
            {
                std::shuffle( m_pFirst, m_pLast, random_generator{ random_device{}() });
                m_pCur = m_pFirst;
            }
        };

        /// Skew permutation generator
        /**
            This generator produces offset permutation based on \p Generator:
                <tt>int(Generator) + nOffset</tt>
            where \p Generator - a permutation generator.

            The class is suitable for \p opt::permutation_generator option
            if the goal sequence should be a permutation of <tt>[nOffset, nOffset + nLength)</tt>
        */
        template <typename Generator>
        class skew_permutation
        {
        public:
            typedef Generator base_generator;   ///< Original permutation generator
            typedef typename base_generator::integer_type   integer_type; ///< Type of generated value

        protected:
            //@cond
            base_generator      m_Gen;
            integer_type const  m_nOffset;
            //@endcond

        public:
            /// Initializes the generator
            skew_permutation(
                integer_type nOffset,   ///< The offset, i.e. first value of generated sequence
                size_t nLength          ///< The length of sequence
                )
                : m_Gen( nLength )
                , m_nOffset( nOffset )
            {}

            /// Returns the current value
            operator integer_type() const
            {
                return integer_type(m_Gen) + m_nOffset;
            }

            /// Goes to next value. Returns \p false if the sequence is exhausted
            bool next()
            {
                return m_Gen.next();
            }

            /// Resets the generator to produce new sequence
            void reset()
            {
                m_Gen.reset();
            }
        };

    } // namespace v

}} // namespace cds::opt

#endif // #ifndef CDSLIB_OPT_PERMUTATION_H
