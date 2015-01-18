/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

// Random number geerator

#ifndef __CPPUNIT_MEMORY_RANDOM_GEN_H
#define __CPPUNIT_MEMORY_RANDOM_GEN_H


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

#endif // __CPPUNIT_MEMORY_RANDOM_GEN_H
