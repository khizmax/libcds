#!/bin/sh
clear

export BOOST_ROOT=~/boost

GCC_ROOT=/usr/local/gcc-4.3.3
export PATH=${GCC_ROOT}/bin:${PATH}
export LD_LIBRARY_PATH=${GCC_ROOT}/lib:${LD_LIBRARY_PATH}

./build.sh -b 64 \
--clean \
-x 'g++' \
-z '-std=c++0x -Wall -pedantic' \
-l '-L${BOOST_ROOT}/bin/linux-amd64' \
--with-boost ${BOOST_ROOT} \
--amd64-use-128bit \
2>&1 | tee build-linux-amd64.log

