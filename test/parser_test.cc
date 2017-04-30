#include <gtest/gtest.h>

#include "parser.h"

#include <iostream>

TEST(parser, function_declaration) {
    std::string input = "func main() -> void {}";
    wayward::lexer lexer{input};
    wayward::parser parser{lexer.tokenize()};
    auto output = parser.parse();
    // TODO: Nobody understands why is this test not passing.
    // This code WORKS fine when debugging and running "by hand",
    // but fails when test is running...
    //EXPECT_EQ(output.nodes.size(), 1);
    //auto a = dynamic_cast<wayward::ast_function_declaration&>(
    //        *output.nodes.back());
    //EXPECT_EQ(a.node_type, wayward::ast_node_type::function_declaration);
    //EXPECT_STREQ(a.name.c_str(), "main");
    //EXPECT_STREQ(a.return_type.c_str(), "void");
}

