cmake_minimum_required(VERSION 3.6)
project(tez CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g3 -DTEZ_DEBUG=1")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG
        -DTEZ_DEBUG=0")

if(NOT CMAKE_BUILD_TYPE)
    message(WARNING "Build type not set; using debug.")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(HEADERS
        src/codegen/c_code_generator.h
        src/semantic/semantic_analyzer.h
        src/semantic/symbol_table.h
        src/syntax/ast.h
        src/syntax/lexer.h
        src/syntax/parser.h
        src/syntax/token.h)
set(SOURCES
        src/codegen/c_code_generator.cc
        src/semantic/semantic_analyzer.cc
        src/semantic/symbol_table.cc
        src/syntax/ast.cc
        src/syntax/lexer.cc
        src/syntax/parser.cc
        src/syntax/token.cc)
add_library(tezc_core STATIC ${HEADERS} ${SOURCES})
target_include_directories(tezc_core PUBLIC src)

add_executable(tezc src/main.cc)
target_link_libraries(tezc tezc_core)

find_package(LLVM REQUIRED)
if(LLVM_FOUND)
    target_include_directories(tezc_core PUBLIC LLVM_INCLUDE_DIRS)
    add_definitions(${LLVM_DEFINITIONS})
    llvm_map_components_to_libnames(LLVM_LIBS support core irreader)
    target_link_libraries(tezc_core ${LLVM_LIBS})
endif()

if(NOT DISABLE_TESTS)
    enable_testing()
    set(TEST_FILES
            test/main.cc
            test/lexer_test.cc
            test/parser_test.cc)
    add_executable(tezc_tests ${TEST_FILES})
    target_include_directories(tezc PUBLIC src)
    add_test(
            NAME    test_all_in_tezc
            COMMAND tezc_tests)
    find_package(GTest REQUIRED)
    target_include_directories(tezc_tests PRIVATE src ${GTEST_INCLUDE_DIRS})
    target_link_libraries(tezc_tests tezc_core ${GTEST_BOTH_LIBRARIES})
endif()
