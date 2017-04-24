cmake_minimum_required(VERSION 3.6)
project(wayward CXX)

set(CMAKE_CXX_FLAGS "-std=c++1z -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g3 -DDEBUG=1")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DDEBUG=0")

set(SRC_FILES
        src/main.cc)
add_executable(waywardc ${SRC_FILES})
target_include_directories(waywardc PUBLIC src)

enable_testing()
set(TEST_FILES
        ${SRC_FILES}
        test/main.cc)
list(REMOVE_ITEM TEST_FILES src/main.cc)
add_executable(waywardc_tests ${TEST_FILES})
add_test(
        NAME    test_all_in_waywardc
        COMMAND waywardc_tests)
find_package(GTest REQUIRED)
target_include_directories(waywardc_tests PRIVATE src ${GTEST_INCLUDE_DIRS})
target_link_libraries(waywardc_tests ${GTEST_BOTH_LIBRARIES})

