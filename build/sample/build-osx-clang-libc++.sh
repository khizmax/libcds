#! /bin/sh

BOOST_ROOT=~/works/libs/boost_1_55_0
./build.sh \
    --clean \
    -c clang \
    -x clang \
    -b 64 \
    -z "--std=c++11 -DCDS_USE_LIBCDS_ATOMIC -I/usr/lib/c++/v1 -I$BOOST_ROOT" \
    -l "-L$BOOST_ROOT/stage64-clang/lib -lc++ -lc++abi -lpthread -ldl" \
    --nodefaultlibs \
    --with-boost $BOOST_ROOT \
    --arch-tune core2 \
    2>&1 |tee build.log
    
#     -z "--std=c++11 -DCDS_USE_LIBCDS_ATOMIC -I/usr/lib/c++/v1 -I/usr/include -I$BOOST_ROOT"
