cmake_minimum_required(VERSION 3.6)
project(wayward CXX)

set(CMAKE_CXX_FLAGS "-std=c++1z -Wall -Wextra -Weffc++ -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g3 -DDEBUG=1")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG -DDEBUG=0")

if(NOT CMAKE_BUILD_TYPE)
    message(WARNING "Build type not set; using debug.")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(SRC_FILES
        src/ast.h
        src/c_code_gen.cc src/c_code_gen.h
        src/lexer.cc src/lexer.h
        src/parser.cc src/parser.h
        src/token.cc src/token.h
        src/utilities.h)
add_library(waywardc_core STATIC ${SRC_FILES})

add_executable(waywardc src/main.cc)
target_include_directories(waywardc PUBLIC src)
target_link_libraries(waywardc waywardc_core)

IF(DISABLE_TESTING)
        enable_testing()
        set(TEST_FILES
                test/main.cc
                test/lexer_test.cc)

        add_executable(waywardc_tests ${TEST_FILES})
        target_include_directories(waywardc PRIVATE src)
        add_test(
                NAME    test_all_in_waywardc
                COMMAND waywardc_tests)
        find_package(GTest REQUIRED)
        target_include_directories(waywardc_tests PRIVATE src ${GTEST_INCLUDE_DIRS})
        target_link_libraries(waywardc_tests waywardc_core ${GTEST_BOTH_LIBRARIES})
ENDIF(DISABLE_TESTING)

