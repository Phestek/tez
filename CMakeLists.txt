cmake_minimum_required(VERSION 3.6)
project(tez CXX)

set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g3 -DTEZ_DEBUG=1")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG -DDEBUG=0")

if(NOT CMAKE_BUILD_TYPE)
    message(WARNING "Build type not set; using debug.")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(SRC_FILES
        src/codegen/c_code_generator.cc src/codegen/c_code_generator.h
        src/syntax/ast.h
        src/syntax/lexer.cc src/syntax/lexer.h
        src/syntax/parser.cc src/syntax/parser.h
        src/syntax/token.cc src/syntax/token.h)
add_library(tezc_core STATIC ${SRC_FILES})
target_include_directories(tezc_core PUBLIC src)

add_executable(tezc src/main.cc)
target_link_libraries(tezc tezc_core)

enable_testing()
set(TEST_FILES
        test/main.cc
        test/lexer_test.cc
        test/parser_test.cc)

add_executable(tezc_tests ${TEST_FILES})
target_include_directories(tezc PRIVATE src)
add_test(
        NAME    test_all_in_tezc
        COMMAND tezc_tests)
find_package(GTest REQUIRED)
target_include_directories(tezc_tests PRIVATE src ${GTEST_INCLUDE_DIRS})
target_link_libraries(tezc_tests tezc_core ${GTEST_BOTH_LIBRARIES})

