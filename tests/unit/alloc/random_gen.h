//$$CDS-header$$

// Random number geerator

#ifndef CPPUNIT_MEMORY_RANDOM_GEN_H
#define CPPUNIT_MEMORY_RANDOM_GEN_H


#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

namespace memory {
    template <typename T>
    class randomGen
    {
        boost::mt19937   m_rndGen;
    public:
        typedef T   random_type;

        randomGen()
        {}

        random_type gen( random_type nMin, random_type nMax )
        {
            boost::uniform_int<random_type> dist(nMin, nMax);
            return boost::variate_generator<boost::mt19937&, boost::uniform_int<random_type> >(m_rndGen, dist)();
        }

        /*
        random_type operator()()
        {
            return gen( s_nMinBlockSize, s_nMaxBlockSize );
        }
        */

        random_type operator()( random_type nMin, random_type nMax )
        {
            return gen( nMin, nMax );
        }
    };
}   // namespace memory

#endif // CPPUNIT_MEMORY_RANDOM_GEN_H
