#include <gtest/gtest.h>

#include "lexer.h"

TEST(Lexer, Whitespaces) {
    std::string input = " \t\n\r\v\f";
    tez::Lexer lexer{input, true};
    auto output = lexer.tokenize();
    
    // All whitespaces should be skipped.
    ASSERT_EQ(output.size(), 0);
}

TEST(Lexer, Braces_And_Parentheses) {
    std::string input = "({)}";
    tez::Lexer lexer{input, true};
    auto output = lexer.tokenize();

    ASSERT_EQ(output.at(0).type, tez::Token_Type::L_PAREN);
    ASSERT_EQ(output.at(1).type, tez::Token_Type::L_BRACE);
    ASSERT_EQ(output.at(2).type, tez::Token_Type::R_PAREN);
    ASSERT_EQ(output.at(3).type, tez::Token_Type::R_BRACE);
}

TEST(Lexer, Operators) {
    std::string input = "-> == != >=";
    tez::Lexer lexer{input, true};
    auto output = lexer.tokenize();

    ASSERT_EQ(output.at(0).type, tez::Token_Type::ARROW);
    ASSERT_EQ(output.at(1).type, tez::Token_Type::EQUALS_EQUALS);
    ASSERT_EQ(output.at(2).type, tez::Token_Type::BANG_EQUALS);
    ASSERT_EQ(output.at(3).type, tez::Token_Type::GREATER_EQUALS);
}

TEST(Lexer, Integers) {
    std::string input = "17 43 65";
    tez::Lexer lexer{input, true};
    auto output = lexer.tokenize();
    
    ASSERT_STREQ(output.at(0).value.c_str(), "17");
    ASSERT_STREQ(output.at(1).value.c_str(), "43");
    ASSERT_STREQ(output.at(2).value.c_str(), "65");
}

TEST(Lexer, Real_numbers) {
    std::string input = "51.534 823.1 4.54767";
    tez::Lexer lexer{input, true};
    auto output = lexer.tokenize();
    ASSERT_STREQ(output.at(0).value.c_str(), "51.534");
    ASSERT_STREQ(output.at(1).value.c_str(), "823.1");
    ASSERT_STREQ(output.at(2).value.c_str(), "4.54767");
}

TEST(Lexer, Identifiers) {
    std::string input = "asdf compiler lexer";
    tez::Lexer lexer{input, true};
    auto output = lexer.tokenize();
    
    ASSERT_STREQ(output.at(0).value.c_str(), "asdf");
    ASSERT_STREQ(output.at(1).value.c_str(), "compiler");
    ASSERT_STREQ(output.at(2).value.c_str(), "lexer");
}

TEST(Lexer, Keywords) {
    std::string input = "func break namespace struct";
    tez::Lexer lexer{input, true};
    auto output = lexer.tokenize();
    
    ASSERT_EQ(output.size(), 4);
    ASSERT_EQ(output.at(0).type, tez::Token_Type::KW_FUNC);
    ASSERT_EQ(output.at(1).type, tez::Token_Type::KW_BREAK);
    ASSERT_EQ(output.at(2).type, tez::Token_Type::KW_NAMESPACE);
    ASSERT_EQ(output.at(3).type, tez::Token_Type::KW_STRUCT);
}

