Building library with CMake
===============

CDS suports both in-source and out-of-source cmake build types. Now project uses:

- CMake: general cross-platform building
- CTest: all unit tests can be run in a standard way by *ctest* command
- CPack: for making rpm/deb/nsys etc. packages

Compiling and testing
----------
**Building out-of-source in "RELEASE" mode ("DEBUG" is default)**

- Wherever create empty directory for building, for instance `libcds-debug`
- Prepare: `cmake -DCMAKE_BUILD_TYPE=RELEASE <path to the project's root directory with CMakeLists.txt>`
- Compile: `make -j4`
- As a result you'll see shared and static cds libraries in the build directory

**Warning**: We strongly recommend not to use static cds library. Static library is not tested and not maintained. You can use it on own risk.

After using command cmake -L <path to the project's root directory with CMakeLists.txt> one can see some additional variables, that can activate additional features:

- `WITH_TESTS:BOOL=OFF`: if you want to build library with unit testing support use *-DWITH_TESTS=ON* on prepare step. Be careful with this flag, because compile time will dramatically increase
- `WITH_TESTS_COVERAGE:BOOL=OFF`: Analyze test coverage using gcov (only for gcc)
- `WITH_BOOST_ATOMIC:BOOL=OFF`: Use boost atomics (only for boost >= 1.54)
- `WITH_ASAN:BOOL=OFF`: compile libcds with AddressSanitizer instrumentation
- `WITH_TSAN:BOOL=OFF`: compile libcds with ThreadSanitizer instrumentation

Additional gtest hints (for unit and stress tests only):
- `GTEST_INCLUDE_DIRS=path`: gives full `path` to gtest include dir. 
- `GTEST_LIBRARY=path`: gives full `path` to `libgtest.a`.


Packaging
----------

In order to package library *CPack* is used, command *cpack -G <Generator>* should create correspondent packages for particular operating system. Now the project supports building the following package types:

- *RPM*: redhat-based linux distribs        
- *DEB*: debian-based linux distribs
- *TGZ*: simple "*tgz*" archive with library and headers
- *NSYS*: windows installer package (NSYS should be installed)   
  
"Live" building and packaging example
----------
- `git clone https://github.com/khizmax/libcds.git`
- `mkdir libcds-release`
- `cd libcds-release`
- `cmake -DWITH\_TESTS=ON -DCMAKE\_BUILD_TYPE=RELEASE ../libcds`
```
    -- The C compiler identification is GNU 4.8.3
    -- The CXX compiler identification is GNU 4.8.3
    ...
    -- Found Threads: TRUE
    -- Boost version: 1.54.0
    -- Found the following Boost libraries:
    --   system
    --   thread
    Build type -- RELEASE
    -- Configuring done
    -- Generating done
    -- Build files have been written to: <...>/libcds-release
``` 
- `make -j4`
```
    Scanning dependencies of target cds
    Scanning dependencies of target test-common
    Scanning dependencies of target cds-s
    Scanning dependencies of target test-hdr-offsetof
    [  1%] Building CXX object CMakeFiles/cds-s.dir/src/hp_gc.cpp.o
    ...
    [100%] Built target test-hdr
```

- `ctest`
```
    Test project /home/kel/projects_cds/libcds-debug
        Start 1: test-hdr
    1/7 Test #1: test-hdr .........................   Passed  1352.24 sec
        Start 2: cdsu-misc
    2/7 Test #2: cdsu-misc ........................   Passed    0.00 sec
        Start 3: cdsu-map
    ...
```

- `cpack -G RPM`
```
    CPack: Create package using RPM
    CPack: Install projects
    CPack: - Run preinstall target for: cds
    CPack: - Install project: cds
    CPack: -   Install component: devel
    CPack: -   Install component: lib
    CPack: Create package
    CPackRPM:Debug: Adding /usr/local to builtin omit list.
    CPackRPM: Will use GENERATED spec file: /home/kel/projects_cds/libcds-debug/_CPack_Packages/Linux/RPM/SPECS/cds-devel.spec
    CPackRPM: Will use GENERATED spec file: /home/kel/projects_cds/libcds-debug/_CPack_Packages/Linux/RPM/SPECS/cds-lib.spec
    CPack: - package: /home/kel/projects_cds/libcds-debug/cds-2.1.0-1-devel.rpm generated.
    CPack: - package: /home/kel/projects_cds/libcds-debug/cds-2.1.0-1-lib.rpm generated.
```

Future development
----------
- CDash: use CI system