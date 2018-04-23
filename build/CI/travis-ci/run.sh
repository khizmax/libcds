#!/bin/bash

set -e
set -x

CONAN_INSTALL_FLAGS="-s compiler.libcxx=libstdc++11"

if [[ "$(uname -s)" == 'Darwin' ]]; then
    if which pyenv > /dev/null; then
        eval "$(pyenv init -)"
    fi
    pyenv activate conan
    CONAN_INSTALL_FLAGS=""
fi

#export CXX=$CXX_COMPILER
#export CC=$C_COMPILER
mkdir build-test && cd build-test
conan install --build $CONAN_INSTALL_FLAGS -s build_type=$BUILD_TYPE ..
cmake -DCMAKE_PREFIX_PATH="$TRAVIS_BUILD_DIR/build-test/deps" -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_EXE_LINKER_FLAGS=$LINKER_FLAGS -DWITH_TESTS=ON ..
cmake --build . -- -j2 $TARGET

if [[ "$(uname -s)" == 'Darwin' ]]; then
	export DYLD_LIBRARY_PATH=$TRAVIS_BUILD_DIR/build-test/deps/lib
fi
ctest -VV -R $TARGET
