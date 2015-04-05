
:: This sample demonstrates how to build libcds in MinGW (http://tdm-gcc.tdragon.net/).
:: Run TDM-GCC console, change dir to libcds build directory and run this batch file
::
:: BOOST_PATH - environment variable that contains full path to boost root directory
::              for example: set BOOST_PATH=d:\libs\boost_1_56_0
::

set BOOST_PATH=c:/works/libs/boost/boost_1_56_0

:: We use -DBOOST_THREAD_USE_LIB flag since static libboost_thread is used

build.sh -b 64 -x 'g++' -z '-std=c++0x -Wall -Wextra -pedantic -Wno-unused-local-typedefs -DBOOST_THREAD_USE_LIB -D_WIN32_WINNT=0x0500' -l "-L%BOOST_PATH%/stage-mingw64/lib" --with-boost "%BOOST_PATH%" --with-make mingw32-make 2>&1 | tee build-mingw-amd64.log

