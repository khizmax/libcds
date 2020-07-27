
INCLUDE(CheckCXXSourceCompiles)

if(TARGET boost::system AND TARGET boost::thread)
    link_libraries(boost::system boost::thread)
else()
    find_package(Boost 1.50 COMPONENTS system thread)
endif()

if(Boost_FOUND)
    message("Boost: ${Boost_LIB_VERSION} in ${Boost_INCLUDE_DIRS}, lib ${Boost_LIBRARY_DIRS}")
else()
    message("Boost: Using CMake-fied boost")
endif()

include_directories(${Boost_INCLUDE_DIRS})


if (NOT GTEST_INCLUDE_DIRS)
   find_package(GTest)

    if (NOT GTEST_FOUND)
        # Download and make gtest library
        # Download and unpack googletest at configure time
        configure_file(CMakeLists.txt.gtest googletest-download/CMakeLists.txt)

        execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
          RESULT_VARIABLE result
          WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )

        if(result)
          message(FATAL_ERROR "CMake step for googletest failed: ${result}")
        endif()

        execute_process(COMMAND ${CMAKE_COMMAND} --build .
          RESULT_VARIABLE result
          WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )

        if(result)
          message(FATAL_ERROR "Build step for googletest failed: ${result}")
        endif()

        # Prevent overriding the parent project's compiler/linker
        # settings on Windows
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

        # Add googletest directly to our build. This defines
        # the gtest and gtest_main targets.
        add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                         ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                         EXCLUDE_FROM_ALL)
                         
        set(GTEST_INCLUDE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/googletest-src/googletest/include")
        set(GTEST_LIBRARIES "gtest_main")
    endif()
endif()

message("GTest: ${GTEST_INCLUDE_DIRS}, lib: ${GTEST_LIBRARIES}")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${GTEST_INCLUDE_DIRS}
)

if(LIBCDS_WITH_HPX)
    if(LIBCDS_INSIDE_HPX)
        set(HPX_MAIN_TARGET hpx_wrap)
    endif()
endif()

set(CDS_TEST_LIBRARIES
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARIES}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
    ${EXTERNAL_SYSTEM_LIBS}
    ${HPX_MAIN_TARGET}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGTEST_LANG_CXX11")

if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "8.0.0")
    #   gcc 4.8 - 6: disable noise -Wunused-local-typedefs
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs")
endif()


# Check if gtest INSTANTIATE_TEST_CASE_P macro supports 4th arg
# The optional last argument to INSTANTIATE_TEST_CASE_P allows the user
# to specify a function or functor that generates custom test name suffixes
# based on the test parameters. The function should accept one argument of
# type testing::TestParamInfo<class ParamType>, and return std::string.
set(OLD_CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})
set(OLD_CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES})
set(OLD_CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES})
set(CMAKE_REQUIRED_FLAGS "-DGTEST_USE_OWN_TR1_TUPLE ${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_REQUIRED_INCLUDES ${GTEST_INCLUDE_DIRS} )
set(CMAKE_REQUIRED_LIBRARIES ${GTEST_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${EXTERNAL_SYSTEM_LIBS} )
CHECK_CXX_SOURCE_COMPILES("
#include <gtest/gtest.h>
class fixture: public ::testing::Test, public ::testing::WithParamInterface< int >
{
public:
    static std::vector< int > get_test_parameters()
    {
        std::vector< int > v{ 1, 2 };
        return v;
    }
    static std::string get_test_parameter_name( ::testing::TestParamInfo<int> const& )
    {
        return std::string();
    }
};

INSTANTIATE_TEST_CASE_P( a,
    fixture,
    ::testing::ValuesIn( fixture::get_test_parameters()),
    fixture::get_test_parameter_name );

int main( int argc, char **argv ) 
{
    ::testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}
" GTEST_INSTANTIATE_TEST_CASE_P_HAS_4TH_ARG )
set(CMAKE_REQUIRED_FLAGS ${OLD_CMAKE_REQUIRED_FLAGS})
set(CMAKE_REQUIRED_INCLUDES ${OLD_CMAKE_REQUIRED_INCLUDES})
set(CMAKE_REQUIRED_LIBRARIES ${OLD_CMAKE_REQUIRED_LIBRARIES})

if(GTEST_INSTANTIATE_TEST_CASE_P_HAS_4TH_ARG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGTEST_LANG_CXX11 -DCDSTEST_GTEST_INSTANTIATE_TEST_CASE_P_HAS_4TH_ARG") 
endif()

# Check if there is byteswap.h (needed for cityhash)
CHECK_CXX_SOURCE_COMPILES("
#include <byteswap.h>
int main( int argc, char **argv ) 
{
    return 0;
}
" CDSTEST_HAVE_BYTESWAP_H )

if(CDSTEST_HAVE_BYTESWAP_H)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCDSTEST_HAVE_BYTESWAP_H") 
endif()

if(LIBCDS_ENABLE_UNIT_TEST)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/unit)
endif()
if(LIBCDS_ENABLE_STRESS_TEST)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/stress)
endif()
if(ENABLE_PERFORMANCE_TEST)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/performance)
endif()

file(GLOB SANITIZER_OPTION_FILES ${PROJECT_SOURCE_DIR}/tools/tsan-suppression)
file(COPY ${SANITIZER_OPTION_FILES} DESTINATION ${EXECUTABLE_OUTPUT_PATH})
