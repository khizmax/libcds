#!/bin/sh

BOOST_ROOT=~/works/libs/boost_1_53_0 $NDK/ndk-build -j 1 2>&1 |tee arm-build.log
