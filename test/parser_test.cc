#include <gtest/gtest.h>

#include "syntax/lexer.h"
#include "syntax/parser.h"

TEST(Parser, Namespaces) {
    std::string input = "namespace qwer {}";
    tez::Lexer lexer{input, true};
    tez::Parser parser{lexer.tokenize()};
    auto file_unit = parser.parse();

    ASSERT_EQ(file_unit.statements.size(), 1);
    ASSERT_EQ(file_unit.statements.back()->node_type, tez::Ast_Node_Type::NAMESPACE);
}

