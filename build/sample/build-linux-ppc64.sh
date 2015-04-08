#!/bin/sh
clear

export BOOST_ROOT=~/boost/boost_1_57_0

GCC_ROOT=/opt/cfarm/release/4.9.2
export PATH=${GCC_ROOT}/bin:${PATH}
export LD_LIBRARY_PATH=${GCC_ROOT}/lib:${LD_LIBRARY_PATH}

./build.sh -b 64 -j 8 \
--clean \
-x 'g++' \
-z '-std=c++0x -Wall -Wextra -pedantic -m64' \
-l '-L${BOOST_ROOT}/stage64/lib' \
--with-boost ${BOOST_ROOT} \
2>&1 | tee build-linux-ppc64.log

