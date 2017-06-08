#include <gtest/gtest.h>

#include "lexer.h"
#include "parser.h"

TEST(Parser, Namespaces) {
    std::string input = "namespace qwer {}";
    tez::Lexer lexer{input, true};
    tez::Parser parser{lexer.tokenize()};
    auto ast = parser.parse();

    ASSERT_EQ(ast.size(), 1);
    ASSERT_EQ(ast.back()->node_type, tez::Ast_Node_Type::NAMESPACE);
}

