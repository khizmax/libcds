#!/bin/sh
clear

BOOST_ROOT=~/boost/boost_1_49_0
export BOOST_ROOT

./build.sh -b 64 -j 2 \
-x 'g++' \
-z '-std=c++0x -Wall -pedantic' \
-l '-L/opt/cfarm/release/4.4.1-64/lib64 -L${BOOST_ROOT}/stage64/lib' \
--with-boost ${BOOST_ROOT} \
2>&1 | tee build-linux-sparc.log


