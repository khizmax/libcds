#! /bin/sh

BOOST_ROOT=~/works/libs/boost_1_55_0
MACOS_SDK_INCLUDE=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk/usr/include

./build.sh \
    --clean \
    -c gcc \
    -x gcc-4.8 \
    -b 64 \
    -z "--std=c++11 -I$BOOST_ROOT -I$MACOS_SDK_INCLUDE" \
    -l "-L$BOOST_ROOT/stage64/lib" \
    --with-boost $BOOST_ROOT \
    --arch-tune core2 \
    2>&1 |tee build.log
    
