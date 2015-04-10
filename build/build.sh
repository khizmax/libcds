#!/bin/sh

# cds library build script
# Maxim Khiszinsky 04.01.2009

# The following variables are defined and exported at the end of this script.
#
# LDFLAGS
# CFLAGS
# CXXFLAGS
# CXX
# CC
# BITSTOBUILD

usage()
{
    echo "Build helper script for one of the supported platforms"
    echo "Usage: build.sh \"options\""
    echo "       where options may be any of the following:"
    echo "       -t <target> make target"
    echo "       -c <C compiler name> Possible values are: gcc,clang,icc"
    echo "       -x <C++ compiler name> (e.g. g++, CC)"
    echo "       -p <Processor architecture> Possible values are:"
    echo "             x86, amd64 (x86_64), sparc, ia64"
    echo "       -o <OS family> Possible values are:"
    echo "             linux, sunos (solaris), hpux, darwin"
    echo "       -D <define> define"
    echo "       -b <bitsToBuild> (accepts '64', '32')"
    echo "       -l <extra linker options>"
    echo "       -z <extra compiler options>"
    echo "       -j <number of make jobs. default 2>"
    echo "       -h (to get help on the above commands)"
    echo "       --with-boost <path to boost include>"
    echo "       --debug-cxx-options <extra compiler options for debug target>"
    echo "       --debug-ld-options <extra linker options for debug target>"
    echo "       --release-cxx-options <extra compiler options for release target>"
    echo "       --release-ld-options <extra linker options for release target>"
    echo "       --clean clean all before building"
    echo "       --debug-test make unit test in debug mode"
    echo "       --amd64-use-128bit use 128bit (16byte) CAS on amd64"
    echo "       --arch-tune march flag (only for x86/amd64), default = native"
    echo "       --nodefaultlibs - no default libs (pthread, stdc++)"
    echo "       --optimize-flags - optimization level flags for release target, default -O3"
}

ERROR_EXIT_CODE=1

MAKE=make

# Set up the default values for each parameter
debug=off                # by default debug is off
bitsToBuild=0            # unknown
makejobs=2
cppcompiler=g++
ccompiler=gcc
processor_arch=unknown
OS_FAMILY=unknown
ArchFlag=native
ld_nodefaultlibs=off

BOOST_INCLUDE_PATH=
makeclean=off

MAKE_DEBUG_TEST=0
ld_libs="-lpthread -ldl -lstdc++"

cxx_debug_options=
ld_debug_options=

cxx_release_options=
ld_release_options=

cxx_test_release_options=
ls_test_release_options=

cxx_release_optimization="-fno-strict-aliasing"
cxx_release_optimization_level="-O3"

amd64_cxx_options=

OS_VERSION=
TOOLSET_SUFFIX=

target=test

while [ $# -gt 0 ]
   do
   case $1 in
   -t)
		target=$2
		shift 2
		;;
   -c)
        ccompiler=$2
		shift 2
		;;
   -x)
        cppcompiler=$2
		shift 2
		;;
   -o)
		OS_FAMILY=$2
		shift 2
		;;
   -p)
        processor_arch=$2; shift 2
		;;
   -b)
        bitsToBuild=$2
		shift 2
		;;
   -l)
        linkeroptions="$linkeroptions $2"
		shift 2
		;;
   -z)
        compileroptions="$compileroptions $2"
		shift 2
		;;
   -j)
        makejobs=$2
		shift 2
		;;
   -D)
	EXTRA_CXXFLAGS="$EXTRA_CXXFLAGS -D$2"
		shift 2
		;;
   -h)
        usage
        exit $ERROR_EXIT_CODE;; 
	
    --clean)
	makeclean=on
	shift
	;;		
    --with-boost)
	BOOST_INCLUDE_PATH=$2
	shift 2
	;;	
    --debug-cxx-options)
	cxx_debug_options=$2
	shift 2
	;;
    --debug-ld-options)
	ld_debug_options=$2
	shift 2
	;;
    --release-cxx-options)
	cxx_release_options=$2
	shift 2
	;;
    --optimize-flags)
	cxx_release_optimization_level=$2
	shift 2
	;;
    --release-ld-options)
	ld_release_options=$2
	shift 2
	;;
    --nodefaultlibs)
	ld_libs=" "
	shift
	;;
    --with-make)
        MAKE=$2
	shift 2
	;;
    --platform-suffix)
        OS_VERSION=$2
	shift 2
	;;
    --toolset-suffix)
	TOOLSET_SUFFIX=$2
	shift 2
	;;
    --debug-test)
	MAKE_DEBUG_TEST=1
	if test $target = 'test'; then
		target=test_debug
	fi
	shift 1
	;;
   --amd64-use-128bit)
	amd64_cxx_options='-mcx16'
	shift 1
	;;
   --arch-tune)
	ArchFlag=$2
	shift 2
	;;
   --)
	shift; break;; 

   *)
       echo "unknown option $1"
       usage
       exit $ERROR_EXIT_CODE;;
   esac
done

cxx_release_optimization="$cxx_release_optimization_level $cxx_release_optimization"

# Determine compiler
case $ccompiler in
	gcc)
		if test $cppcompiler = ''; then
			cppcompiler=g++
		fi
		;;
	clang)
		if test $cppcompiler = ''; then
			cppcompiler=clang++
		fi
		;;
	icc)
		if test $cppcompiler = ''; then
			cppcompiler=icc
		fi
		;;
	*)
		echo "ERROR: Unknown compiler: $ccompiler"
		exit $ERROR_EXIT_CODE
		;;
esac

# Determine OS family
if test $OS_FAMILY = 'unknown'; then
	OS_FAMILY=`uname |tr [A-Z] [a-z]|sed "s/-//"`
fi
case $OS_FAMILY in
    hp-ux)
	OS_FAMILY=hpux
	;;
    solaris)
	OS_FAMILY=sunos
	;;
    mingw*)
	OS_FAMILY=mingw
	;;
    linux|sunos|hpux|aix|freebsd|mingw|darwin)
	;;
    *)
	echo "Warning: Unknown operation system: $OS_FAMILY"
	#exit $ERROR_EXIT_CODE
	;;
esac


# Determine processor architecture
if test $processor_arch = 'unknown'; then
	processor_arch=`uname -m|tr [A-Z] [a-z]`
fi
case $processor_arch in
	x86_64)
	    if test $bitsToBuild = 64; then
	        processor_arch='amd64'
	    else
		processor_arch='x86'
	    fi;
	    ;;
	x86|i686)
		if test $bitsToBuild = 64; then
			processor_arch='amd64'
		else
			processor_arch='x86'
		fi
		;;
	sparc64)
		processor_arch='sparc'
		;;
	amd64|x86|ia64|sparc)
		;;
	*)
		processor_arch=`uname -p|tr [A-Z] [a-z]`
		case $processor_arch in
		    sparc|powerpc)
			;;
		    *)
			echo "Warning: Unknown processor architecture: $processor_arch"
			#exit ${ERROR_EXIT_CODE}
			;;
		esac			
		1;;
esac	

# Determine compiler flags
case $ccompiler in
    gcc|clang)
	case $processor_arch in
	amd64)
	    case $OS_FAMILY in
	    linux|freebsd|darwin)
		buildCXXflags="-m64 -fPIC -march=$ArchFlag $amd64_cxx_options"
		buildCflags="-m64 -fPIC -march=$ArchFlag $amd64_cxx_options"
		buildLDflags="-m64 -fPIC"
		buildTestLDflags="-m64 -fPIC"
		;;
	    mingw)
		buildCXXflags="-m64 -march=$ArchFlag $amd64_cxx_options"
		buildCflags="-m64 -march=$ArchFlag $amd64_cxx_options"
		buildLDflags="-m64"
		buildTestLDflags="-m64"
                ld_libs=""
		;;
	    *)
		echo "Warning: cannot determine compiler flags for processor $processor_arch, OS $OS_FAMILY, and compiler $ccompiler"
		#exit ${ERROR_EXIT_CODE}
		;;
	    esac
	    ;;
	x86)
	    case $OS_FAMILY in
		linux|freebsd|darwin)
		    buildCXXflags="-m32 -fPIC -march=$ArchFlag"
		    buildCflags="-m32 -fPIC -march=$ArchFlag"
		    buildLDflags="-m32 -fPIC"
		    buildTestLDflags="-m32 -fPIC"
		    ;;
		mingw)
		    buildCXXflags="-m32 -march=$ArchFlag"
		    buildCflags="-m32 -march=$ArchFlag"
		    buildLDflags="-m32"
		    buildTestLDflags="-m32"
		    ld_libs=""
		    ;;
		*)
		    echo "Warning: cannot determine compiler flags for processor $processor_arch, OS $OS_FAMILY, and compiler $ccompiler"
		    #exit ${ERROR_EXIT_CODE}
		    ;;
	    esac
	    ;;
	ia64)
	    bitsToBuild=64
	    case $OS_FAMILY in
		linux|freebsd)
		    buildCXXflags="-mtune=itanium2 -fPIC"
		    buildCflags="-mtune=itanium2 -fPIC"
		    buildLDflags="-mtune=itanium2 -fPIC"
		    buildTestLDflags="-mtune=itanium2 -fPIC"
		    ;;
		hpux)
		    buildCXXflags="-mlp64 -mtune=itanium2 -fPIC"
		    buildCflags="-mlp64 -mtune=itanium2 -fPIC"
		    buildLDflags="-mlp64 -mtune=itanium2 -fPIC"
		    buildTestLDflags="-mlp64 -mtune=itanium2 -fPIC"
		    ;;
		*)
		    echo "Warning: cannot determine compiler flags for processor $processor_arch, OS $OS_FAMILY, and compiler $ccompiler"
		    #exit ${ERROR_EXIT_CODE}
		    ;;
	    esac
	    ;;
	sparc)
	    bitsToBuild=64
	    case $OS_FAMILY in
		sunos)
		    buildCXXflags="-mcpu=v9 -mtune=ultrasparc3 -m64 -fPIC -pthreads"
		    buildCflags="-mcpu=v9 -mtune=ultrasparc3 -m64 -fPIC -pthreads"
		    buildLDflags="-mcpu=v9 -mtune=ultrasparc3 -m64 -fPIC -pthreads"
		    buildTestLDflags="-mcpu=v9 -mtune=ultrasparc3 -m64 -fPIC -pthreads"
		    cxx_test_release_options="-fPIC"
		    ld_test_release_options="-fPIC"
		    ;;
		linux)
		    buildCXXflags="-mcpu=v9 -mtune=ultrasparc3 -m64 -fPIC"
		    buildCflags="-mcpu=v9 -mtune=ultrasparc3 -m64 -fPIC"
		    buildLDflags="-mcpu=v9 -mtune=ultrasparc3 -m64 -fPIC"
		    buildTestLDflags="-mcpu=v9 -mtune=ultrasparc3 -m64 -fPIC"
		    cxx_test_release_options="-fPIC"
		    ld_test_release_options="-fPIC"
		    ;;
		*)
		    echo "Warning: cannot determine compiler flags for processor $processor_arch, OS $OS_FAMILY, and compiler $ccompiler"
		    #exit ${ERROR_EXIT_CODE}
		    ;;
	    esac
	    ;;
	powerpc)
	    bitsToBuild=64
	    case $OS_FAMILY in
		aix)
		    buildCXXflags="-maix64 -pthread -fPIC"
		    buildCflags="-maix64 -pthread -fPIC"
		    buildLDflags="-maix64 -pthread -fPIC"
		    buildTestLDflags="-maix64 -pthread -fPIC"
		    cxx_test_release_options="-fPIC"
		    ld_test_release_options="-fPIC"
		    ;;
		*)
		    echo "Warning: cannot determine compiler flags for processor $processor_arch, OS $OS_FAMILY, and compiler $ccompiler"
		    #exit ${ERROR_EXIT_CODE}
		    ;;
	    esac
	    ;;
	*)
	    echo "Warning: cannot determine compiler flags for processor $processor_arch and compiler $ccompiler"
	    #exit ${ERROR_EXIT_CODE}
	    ;;
	esac

	#cppcompiler_version=`$cppcompiler -dumpversion`
	#echo compiler version=$cppcompiler $cppcompiler_version

	# Setup target options
	# buildCXXflags="-std=gnu++0x $buildCXXflags"
	#cxx_debug_options="-D_DEBUG -O0 -g $cxx_debug_options"
	#cxx_release_options="-DNDEBUG $cxx_release_optimization $cxx_release_options"
	;;
    icc)
	case $processor_arch in
	amd64)
	    case $OS_FAMILY in
	    linux)
		buildCXXflags="-fPIC -march=$ArchFlag $amd64_cxx_options"
		buildCflags="-fPIC -march=$ArchFlag $amd64_cxx_options"
		buildLDflags="-fPIC"
		buildTestLDflags="-fPIC"
		;;
	    *)
		echo "Warning: cannot determine compiler flags for processor $processor_arch, OS $OS_FAMILY, and compiler $ccompiler"
		#exit ${ERROR_EXIT_CODE}
		;;
	    esac
	    ;;
	x86)
	    case $OS_FAMILY in
		linux)
		    buildCXXflags="-fPIC -march=$ArchFlag"
		    buildCflags="-fPIC -march=$ArchFlag"
		    buildLDflags="-fPIC"
		    buildTestLDflags="-fPIC"
		    ;;
		*)
		    echo "Warning: cannot determine compiler flags for processor $processor_arch, OS $OS_FAMILY, and compiler $ccompiler"
		    #exit ${ERROR_EXIT_CODE}
		    ;;
	    esac
	    ;;
	*)
	    echo "Warning: cannot determine compiler flags for processor $processor_arch and compiler $ccompiler"
	    #exit ${ERROR_EXIT_CODE}
	    ;;
	esac
	;;
    *)
	echo "ERROR: Unknown compiler: $ccompiler"
	exit ${ERROR_EXIT_CODE}
	;;
esac

cppcompiler_version=`$cppcompiler -dumpversion`
echo compiler version=$cppcompiler $cppcompiler_version

# Setup target options
# buildCXXflags="-std=gnu++0x $buildCXXflags"
cxx_debug_options="-D_DEBUG -O0 -g $cxx_debug_options"
cxx_release_options="-DNDEBUG $cxx_release_optimization $cxx_release_options"


if test "x$BOOST_INCLUDE_PATH" != "x"; then
	buildCXXflags="$buildCXXflags -I$BOOST_INCLUDE_PATH"
fi

if test "x$buildTestLDflags" = "x"; then
	buildTestLDflags=$buildLDflags
fi


EXTRA_CXXFLAGS="$buildCXXflags $EXTRA_CXXFLAGS"
EXTRA_CFLAGS="$buildCflags $EXTRA_CFLAGS"
EXTRA_LDFLAGS="$buildLDflags $EXTRA_LDFLAGS"

EXTRA_TEST_LDFLAGS="$buildTestLDflags $EXTRA_TEST_LDFLAGS"


echo "Building with the following options ..."
echo "Processor: $processor_arch"
echo "Platform: $OS_FAMILY"
echo "C Compiler: $ccompiler"
echo "C++ Compiler: $cppcompiler"
echo "C++ Compiler version: $cppcompiler_version"
echo "Bits to build: $bitsToBuild"
echo "Compile options: $compileroptions $EXTRA_CXXFLAGS"
echo "Link options: $linkeroptions $EXTRA_LDFLAGS"
echo "Link options (for test cds-unit app): $linkeroptions $EXTRA_TEST_LDFLAGS"

BITSTOBUILD=$bitsToBuild
export BITSTOBUILD

#
# Set the C compiler and C++ compiler environment variables
#

CC="$ccompiler"
export CC

CXX="$cppcompiler"
export CXX

ROOT_DIR=..

GOAL_DIR=$ccompiler$TOOLSET_SUFFIX-$processor_arch-$OS_FAMILY$OS_VERSION-$bitsToBuild
BIN_PATH=$ROOT_DIR/bin/$GOAL_DIR
mkdir -p $BIN_PATH

OBJ_PATH=$ROOT_DIR/obj/$GOAL_DIR
mkdir -p $OBJ_PATH

echo PATH=$PATH
echo LD_LIBRARY_PATH=$LD_LIBRARY_PATH
echo BIN_PATH=$BIN_PATH
echo OBJ_PATH=$OBJ_PATH
echo `${CXX} --version | head -1`
echo Build started

makegoals=
if test $makeclean = 'on'; then
   echo Clean all
   $MAKE -f Makefile clean platform=$OS_FAMILY BIN_PATH=$BIN_PATH OBJ_PATH=$OBJ_PATH
fi

echo ---------------------------------
echo Make debug library
#CXXFLAGS="$compileroptions $cxx_debug_options $EXTRA_CXXFLAGS"
#export CXXFLAGS
#CFLAGS="$compileroptions $cxx_debug_options $EXTRA_CFLAGS $debugflag "
#export CFLAGS
#LDFLAGS="$linkeroptions -shared $ld_debug_options $EXTRA_LDFLAGS "
#export LDFLAGS

mkdir -p $OBJ_PATH/debug

CXXFLAGS="$compileroptions $cxx_debug_options $EXTRA_CXXFLAGS" \
CFLAGS="$compileroptions $cxx_debug_options $EXTRA_CFLAGS $debugflag " \
LDLIBS="$ld_libs" \
LDFLAGS="$linkeroptions -shared $ld_debug_options $EXTRA_LDFLAGS " \
$MAKE -f Makefile \
     platform=$OS_FAMILY \
     BIN_PATH=$BIN_PATH \
     OBJ_PATH=$OBJ_PATH/debug \
     debug \
  || exit $?

echo ---------------------------------
echo Make release library

#CXXFLAGS="$compileroptions $cxx_release_options $EXTRA_CXXFLAGS "
#export CXXFLAGS
#CFLAGS="$compileroptions $cxx_release_options $EXTRA_CFLAGS "
#export CFLAGS
#LDFLAGS="$linkeroptions -shared $ld_resease_options $ld_libs $EXTRA_LDFLAGS "
#export LDFLAGS

mkdir -p $OBJ_PATH/release

CXXFLAGS="$compileroptions $cxx_release_options $EXTRA_CXXFLAGS " \
CFLAGS="$compileroptions $cxx_release_options $EXTRA_CFLAGS " \
LDFLAGS="$linkeroptions -shared $ld_resease_options $EXTRA_LDFLAGS " \
LDLIBS="$ld_libs" \
$MAKE -f Makefile \
     platform=$OS_FAMILY \
     BIN_PATH=$BIN_PATH \
     OBJ_PATH=$OBJ_PATH/release \
     release \
  || exit $?


echo ---------------------------------
echo Make tests

if test $MAKE_DEBUG_TEST = '0'; then
    CXXFLAGS="$compileroptions $cxx_release_options $cxx_test_release_options $EXTRA_CXXFLAGS "
    export CXXFLAGS
    CFLAGS="$compileroptions $cxx_release_options $EXTRA_CFLAGS "
    export CFLAGS
    LDFLAGS="$linkeroptions $ld_release_options $ld_test_release_options $ld_libs $EXTRA_TEST_LDFLAGS "
    export LDFLAGS

    $MAKE -f Makefile -j $makejobs \
        platform=$OS_FAMILY \
        BIN_PATH=$BIN_PATH \
        OBJ_PATH=$OBJ_PATH/test \
        $target \
     || exit $?
fi    

echo ---------------------------------
echo Make tests debug

if test $MAKE_DEBUG_TEST = '1'; then
    CXXFLAGS="$compileroptions $cxx_debug_options $cxx_test_release_options $EXTRA_CXXFLAGS "
    export CXXFLAGS
    CFLAGS="$compileroptions $cxx_debug_options $EXTRA_CFLAGS "
    export CFLAGS
    LDFLAGS="$linkeroptions $ld_debug_options $ld_test_release_options $ld_libs $EXTRA_TEST_LDFLAGS "
    export LDFLAGS

    $MAKE -f Makefile -j $makejobs \
        platform=$OS_FAMILY \
        BIN_PATH=$BIN_PATH \
        OBJ_PATH=$OBJ_PATH/test-debug \
        $target \
     || exit $?
fi   
 
