#include <gtest/gtest.h>

#include "lexer.h"

TEST(lexer, whitespaces) {
    std::string input = " \t\n\r\v\f";
    std::vector<wayward::token> output;
    
    wayward::lexer lexer{input};
    output = lexer.tokenize();
    
    // All whitespaces should be skipped.
    ASSERT_EQ(output.size(), 0);
}

TEST(lexer, integers) {
    std::string input = "17 43 65";
    std::vector<wayward::token> output;
    
    wayward::lexer lexer{input};
    output = lexer.tokenize();
    
    // All whitespaces should be skipped.
    ASSERT_STREQ(output.at(0).value.c_str(), "17");
    ASSERT_STREQ(output.at(1).value.c_str(), "43");
    ASSERT_STREQ(output.at(2).value.c_str(), "65");
}

TEST(lexer, real_numbers) {
    std::string input = "51.534 823.1 4.54767";
    std::vector<wayward::token> output;
    
    wayward::lexer lexer{input};
    output = lexer.tokenize();
    
    // All whitespaces should be skipped.
    ASSERT_STREQ(output.at(0).value.c_str(), "51.534");
    ASSERT_STREQ(output.at(1).value.c_str(), "823.1");
    ASSERT_STREQ(output.at(2).value.c_str(), "4.54767");
}

TEST(lexer, identifiers) {
    std::string input = "asdf compiler lexer";
    std::vector<wayward::token> output;
    
    wayward::lexer lexer{input};
    output = lexer.tokenize();
    
    // All whitespaces should be skipped.
    ASSERT_STREQ(output.at(0).value.c_str(), "asdf");
    ASSERT_STREQ(output.at(1).value.c_str(), "compiler");
    ASSERT_STREQ(output.at(2).value.c_str(), "lexer");
}

