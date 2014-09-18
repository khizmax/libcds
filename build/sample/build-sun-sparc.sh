#!/bin/sh
clear

BOOST_ROOT=~/boost
export BOOST_ROOT

GCC_ROOT=/usr/local/gcc433
PATH=${GCC_ROOT}/bin:${PATH}
export PATH
LD_LIBRARY_PATH=${GCC_ROOT}/lib:/usr/local/lib/mpfr-2.4.1/lib:/usr/local/lib/gmp-4.2.4/lib:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH

./build.sh -b 64 \
--clean \
-x 'g++' \
-z '-std=c++0x -Wall -pedantic' \
-l '-L${BOOST_ROOT}/bin/sun-sparc64' \
--with-boost ${BOOST_ROOT} \
2>&1 | tee build-sun-sparc.log


