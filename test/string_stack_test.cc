#include <gtest/gtest.h>

#include "utils/string_stack.h"

TEST(String_Stack, Append) {
    tez::String_Stack<> sstack{};
    sstack.append("test1");
    sstack.append("test2");
    auto result = sstack.get();
    ASSERT_EQ(result, "test1_test2");
}

TEST(String_Stack, Pop) {
    tez::String_Stack<> sstack{};
    sstack.append("test1");
    sstack.append("test2");
    sstack.pop();
    auto result = sstack.get();
    ASSERT_EQ(result, "test1");
}

TEST(String_Stack, Pop_Everything) {
    tez::String_Stack<> sstack{};
    sstack.append("test1");
    sstack.append("test2");
    sstack.pop();
    sstack.pop();
    auto result = sstack.get();
    ASSERT_EQ(result, "");
}

TEST(String_Stack, Clear) {
    tez::String_Stack<> sstack{};
    sstack.append("test1");
    sstack.append("test2");
    sstack.clear();
    auto result = sstack.get();
    ASSERT_EQ(result, "");
}

TEST(String_Stack, Pop_Everything_With_Trailing_Delimiter) {
    tez::String_Stack<> sstack{};
    sstack.use_trailing_delimiter(true);
    sstack.append("test1");
    sstack.append("test2");
    sstack.pop();
    sstack.pop();
    auto result = sstack.get();
    ASSERT_EQ(result, "");
}

TEST(String_Stack, Use_Trailing_Delimiter) {
    tez::String_Stack<> sstack{};
    sstack.use_trailing_delimiter(true);
    sstack.append("test1");
    sstack.append("test2");
    auto result = sstack.get();
    ASSERT_EQ(result, "test1_test2_");
}
