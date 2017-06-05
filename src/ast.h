#ifndef WAYWARD_AST_H
#define WAYWARD_AST_H

#include <string>
#include <memory>

namespace wayward {

enum class ast_node_type {
    UNDEFINED,

    BLOCK,
    
    BOOLEAN,
    INTEGER,
    REAL_NUMBER,
    STRING,
    
    IDENTIFIER,
    
    UNARY_OPERATION,
    BINARY_OPERATION,
    GROUPING_EXPRESSION,
    
    FUNCTION_DECLARATION,
    FUNCTION_CALL,
    RETURN,
    VARIABLE_DECLARATION,
    
    IF,
    WHILE,
    DO_WHILE,
    FOR,
    BREAK,
    CONTINUE,
    
    STRUCT,
    ENUM,
    UNION,  // Not implemented yet.
    CLASS,  // Not implemented yet.
};

struct ast_node {
    virtual ~ast_node() = default;
    ast_node_type node_type = ast_node_type::UNDEFINED;
};
using ast_node_ptr = std::unique_ptr<ast_node>;

struct ast_block final : ast_node {
    ast_block() { node_type = ast_node_type::BLOCK; }
    std::vector<ast_node_ptr> statements;
};

struct ast_boolean final : ast_node {
    ast_boolean() {node_type = ast_node_type::BOOLEAN; }
    bool value;
};

struct ast_integer final : ast_node {
    ast_integer() { node_type = ast_node_type::INTEGER; }
    long long value;
};

struct ast_real_number final : ast_node {
    ast_real_number() { node_type = ast_node_type::REAL_NUMBER; }
    double value;
};

struct ast_string final : ast_node {
    ast_string() { node_type = ast_node_type::STRING; }
    std::string value;
};

struct ast_identifier final : ast_node {
    ast_identifier() { node_type = ast_node_type::IDENTIFIER; }
    std::string name;
};

struct ast_unary_operation final : ast_node {
    ast_unary_operation() { node_type = ast_node_type::UNARY_OPERATION; }
    ast_node_ptr left;
    std::string  operat;
};

struct ast_binary_operation final : ast_node {
    ast_binary_operation() { node_type = ast_node_type::BINARY_OPERATION; }
    ast_node_ptr left;
    ast_node_ptr right;
    std::string  operat;
};

struct ast_grouping_expression final : ast_node {
    ast_grouping_expression() {node_type = ast_node_type::GROUPING_EXPRESSION; }
    ast_node_ptr expr;
};

struct ast_func_decl final : ast_node {
    struct param {
        std::string name;
        bool        constant;
        std::string type;
    };
    ast_func_decl() { node_type = ast_node_type::FUNCTION_DECLARATION; }
    std::string        name;
    std::vector<param> params;
    std::string        return_type;
    ast_block          body;
};

struct ast_return final : ast_node {
    ast_return() { node_type = ast_node_type::RETURN; }
    ast_node_ptr value;
};

struct ast_func_call final : ast_node {
    ast_func_call() { node_type = ast_node_type::FUNCTION_CALL; }
    std::string               name;
    std::vector<ast_node_ptr> args;
};

struct ast_var_decl final : ast_node {
    ast_var_decl() { node_type = ast_node_type::VARIABLE_DECLARATION; }
    std::string  name;
    bool         constant;
    std::string  type;
    ast_node_ptr initializer;
};

struct ast_if final : ast_node {
    ast_if() { node_type = ast_node_type::IF; }
    ast_node_ptr condition;
    ast_block    if_block;
    ast_node_ptr else_block;
};

struct ast_while final : ast_node {
    ast_while() { node_type = ast_node_type::WHILE; }
    ast_node_ptr condition;
    ast_block    body;
};

struct ast_do_while final : ast_node {
    ast_do_while() { node_type = ast_node_type::DO_WHILE; }
    ast_node_ptr condition;
    ast_block    body;
};

struct ast_for final : ast_node {
    ast_for() { node_type = ast_node_type::FOR; }
    ast_node_ptr init_statement;
    ast_node_ptr condition;
    ast_node_ptr iteration_expr;
    ast_block    body;
};

struct ast_break final : ast_node {
    ast_break() { node_type = ast_node_type::BREAK; }
};

struct ast_continue final : ast_node {
    ast_continue() { node_type = ast_node_type::CONTINUE; }
};

struct ast_struct final : ast_node {
    struct field {
        std::string name;
        std::string type;
    };
    ast_struct() { node_type = ast_node_type::STRUCT; }
    std::string        name;
    std::vector<field> fields;
};

struct ast_enum final : ast_node {
    struct enumerator {
        std::string name;
        int         value;
    };
    ast_enum() { node_type = ast_node_type::ENUM; }
    std::string             name;
    std::vector<enumerator> enumerations;
};

}

#endif //WAYWARD_AST_H

