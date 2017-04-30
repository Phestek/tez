#ifndef WAYWARD_AST_H
#define WAYWARD_AST_H

#include <vector>
#include <string>
#include <memory>

namespace wayward {

enum class ast_node_type {
    undefined,
    
    program,
    block,

    integer_literal,
    real_number_literal,
    operator_literal,

    binary_operation,
    unary_operation,

    function_declaration,
    variable_declaration    // Used for both 'var' and 'let'.
};

struct ast_node {
    virtual ~ast_node() {}
    const ast_node_type node_type = ast_node_type::undefined;
};
using ast_node_ptr = std::shared_ptr<ast_node>;

struct ast_program final : ast_node {
    const ast_node_type       node_type = ast_node_type::program;
    std::vector<ast_node_ptr> nodes;
};

struct ast_block final : ast_node {
    const ast_node_type       node_type = ast_node_type::block;
    std::vector<ast_node_ptr> nodes;
};

struct ast_integer_literal final : ast_node {
    ast_integer_literal(int value) : value{value} {}
    const ast_node_type node_type = ast_node_type::integer_literal;
    int                 value;
};

struct ast_binary_operation final : ast_node {
    ast_binary_operation(ast_node_ptr left, ast_node_ptr right,
            const std::string& operat)
            : left{left}, right{right}, operat(operat) {}
    const ast_node_type node_type = ast_node_type::binary_operation;
    ast_node_ptr        left;
    ast_node_ptr        right;
    std::string         operat;
};

struct ast_function_declaration : ast_node {
    ast_function_declaration(const std::string& name,
            const std::string& return_type, ast_block body)
            : name{name}, return_type{return_type}, body{body} {}
    const ast_node_type node_type = ast_node_type::function_declaration;
    std::string         name;
    std::string         return_type;
    ast_block           body;
};

/*struct ast_variable_declaration final : ast_node {
    std::string  name;
    std::string  type;
    ast_node_ptr initializer;
};*/

}

#endif //WAYWARD_AST_H

