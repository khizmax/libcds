#!/bin/sh
clear

export BOOST_ROOT=~/boost

GCC_ROOT=/usr/local/gcc-433
export PATH=${GCC_ROOT}/bin:${PATH}
export LD_LIBRARY_PATH=${GCC_ROOT}/lib:${LD_LIBRARY_PATH}

./build.sh -b 64 \
--clean \
-x 'g++' \
-z '-std=c++0x -Wall -pedantic' \
-l '-L${BOOST_ROOT}/bin/hpux1123-ia64' \
--with-boost ${BOOST_ROOT} \
--with-make gmake \
--platform-suffix 11.23 \
2>&1 | tee build-hpux11.23-ia64.log

