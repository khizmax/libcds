#!/bin/bash

set -e
set -x

if [[ "$(uname -s)" == 'Darwin' ]]; then
    if which pyenv > /dev/null; then
        eval "$(pyenv init -)"
    fi
    pyenv activate conan
fi

#export CXX=$CXX_COMPILER
#export CC=$C_COMPILER
mkdir build-test && cd build-test
conan install --build=missing -s build_type=$BUILD_TYPE ..
cmake -DCMAKE_PREFIX_PATH="$TRAVIS_BUILD_DIR/build-test/deps" -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_EXE_LINKER_FLAGS=$LINKER_FLAGS -DWITH_TESTS=ON ..
cmake --build . -- -j2 $TARGET

if [[ "$(uname -s)" == 'Darwin' ]]; then
	export DYLD_LIBRARY_PATH=$TRAVIS_BUILD_DIR/build-test/deps/lib
fi
ctest -VV -R $TARGET
