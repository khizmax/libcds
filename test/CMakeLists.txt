
INCLUDE(CheckCXXSourceCompiles)

if (NOT GTEST_INCLUDE_DIRS)
   find_package(GTest REQUIRED)
endif()
message("GTest: ${GTEST_INCLUDE_DIRS}, lib: ${GTEST_LIBRARIES}")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${GTEST_INCLUDE_DIRS}
)

set(CDS_TEST_LIBRARIES
    ${CDS_SHARED_LIBRARY}
    ${GTEST_LIBRARIES}
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
    ${EXTERNAL_SYSTEM_LIBS}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGTEST_LANG_CXX11") 

# Check if gtest INSTANTIATE_TEST_CASE_P macro supports 4th arg
# The optional last argument to INSTANTIATE_TEST_CASE_P allows the user
# to specify a function or functor that generates custom test name suffixes
# based on the test parameters. The function should accept one argument of
# type testing::TestParamInfo<class ParamType>, and return std::string.
set(CMAKE_REQUIRED_FLAGS "-DGTEST_USE_OWN_TR1_TUPLE ${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_REQUIRED_INCLUDES ${GTEST_INCLUDE_DIRS} )
set(CMAKE_REQUIRED_LIBRARIES ${GTEST_LIBRARIES} ${EXTERNAL_SYSTEM_LIBS} )
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

if( GTEST_INSTANTIATE_TEST_CASE_P_HAS_4TH_ARG )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGTEST_LANG_CXX11 -DCDSTEST_GTEST_INSTANTIATE_TEST_CASE_P_HAS_4TH_ARG") 
endif()
 
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/unit)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/stress)

file(GLOB SANITIZER_OPTION_FILES ${PROJECT_SOURCE_DIR}/tools/tsan-suppression)
file(COPY ${SANITIZER_OPTION_FILES} DESTINATION ${EXECUTABLE_OUTPUT_PATH})
