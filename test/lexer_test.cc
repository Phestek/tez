#include <gtest/gtest.h>

#include "lexer.h"

TEST(lexer, whitespaces) {
    std::string input = " \t\n\r\v\f";
    wayward::lexer lexer{input};
    auto output = lexer.tokenize();
    
    // All whitespaces should be skipped.
    ASSERT_EQ(output.size(), 0);
}

TEST(lexer, braces_and_parentheses) {
    std::string input = "({)}";
    wayward::lexer lexer{input};
    auto output = lexer.tokenize();

    ASSERT_EQ(output.at(0).type, wayward::token_type::l_paren);
    ASSERT_EQ(output.at(1).type, wayward::token_type::l_brace);
    ASSERT_EQ(output.at(2).type, wayward::token_type::r_paren);
    ASSERT_EQ(output.at(3).type, wayward::token_type::r_brace);
}

TEST(lexer, operators) {
    std::string input = "-> == != >=";
    wayward::lexer lexer{input};
    auto output = lexer.tokenize();

    ASSERT_EQ(output.at(0).type, wayward::token_type::arrow);
    ASSERT_EQ(output.at(1).type, wayward::token_type::equals_equals);
    ASSERT_EQ(output.at(2).type, wayward::token_type::bang_equals);
    ASSERT_EQ(output.at(3).type, wayward::token_type::greater_equals);
}

TEST(lexer, integers) {
    std::string input = "17 43 65";
    wayward::lexer lexer{input};
    auto output = lexer.tokenize();
    
    // All whitespaces should be skipped.
    ASSERT_STREQ(output.at(0).value.c_str(), "17");
    ASSERT_STREQ(output.at(1).value.c_str(), "43");
    ASSERT_STREQ(output.at(2).value.c_str(), "65");
}

TEST(lexer, real_numbers) {
    std::string input = "51.534 823.1 4.54767";
    wayward::lexer lexer{input};
    auto output = lexer.tokenize();
    
    // All whitespaces should be skipped.
    ASSERT_STREQ(output.at(0).value.c_str(), "51.534");
    ASSERT_STREQ(output.at(1).value.c_str(), "823.1");
    ASSERT_STREQ(output.at(2).value.c_str(), "4.54767");
}

TEST(lexer, identifiers) {
    std::string input = "asdf compiler lexer";
    wayward::lexer lexer{input};
    auto output = lexer.tokenize();
    
    // All whitespaces should be skipped.
    ASSERT_STREQ(output.at(0).value.c_str(), "asdf");
    ASSERT_STREQ(output.at(1).value.c_str(), "compiler");
    ASSERT_STREQ(output.at(2).value.c_str(), "lexer");
}

