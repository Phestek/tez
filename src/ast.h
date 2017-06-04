#ifndef WAYWARD_AST_H
#define WAYWARD_AST_H

#include <string>
#include <memory>

namespace wayward {

enum class ast_node_type {
    UNDEFINED,
    BLOCK,                  // Represents a block of code - { ... } .
    BOOLEAN,                // True / false value.
    INTEGER,                // Integer value.
    REAL_NUMBER,            // Floating point value.
    STRING,                 // Sequence of characters.
    IDENTIFIER,             // Variable / constant name.
    UNARY_OPERATION,        // Operation with single operand.
    BINARY_OPERATION,       // Operation with two operands.
    GROUPING_EXPRESSION,    // Expression inside parenthesis.
    FUNC_PARAM,             // Function parameter.
    FUNCTION_DECLARATION,   // Function declaration.
    FUNCTION_RETURN,        // 'return'.
    FUNCTION_CALL,          // Call to (previously declared) function.
    TYPE,                   // Type representation. Just type, pointer or array.
    VARIABLE_DECLARATION,   // Used both for 'var' and 'let'.
    IF,                     // 'if else' statement.
    WHILE,                  // 'while' loop.
    DO_WHILE,               // 'do { ... } while' loop.
    FOR,                    // 'for' loop.
    BREAK,                  // Breaks current loop.
    CONTINUE,               // Goes to top of current loop.
};

struct ast_node {
    virtual ~ast_node() {}
    ast_node_type node_type = ast_node_type::UNDEFINED;
};
using ast_node_ptr = std::unique_ptr<ast_node>;

struct ast_block final : ast_node {
    ast_block(std::vector<ast_node_ptr>& statements)
            : statements{std::move(statements)} {
        node_type = ast_node_type::BLOCK;
    }
    std::vector<ast_node_ptr> statements;
};

struct ast_boolean final : ast_node {
    ast_boolean(bool value)
            : value{value} {
        node_type = ast_node_type::BOOLEAN;
    }
    bool value;
};

struct ast_integer final : ast_node {
    ast_integer(long long value)
            : value{value} {
        node_type = ast_node_type::INTEGER;
    }
    long long value;
};

struct ast_real_number final : ast_node {
    ast_real_number(double value)
            : value{value} {
        node_type = ast_node_type::REAL_NUMBER;
    }
    double value;
};

struct ast_string final : ast_node {
    ast_string(const std::string& value)
            : value{value} {
        node_type = ast_node_type::STRING;
    }
    std::string value;
};

struct ast_identifier final : ast_node {
    ast_identifier(const std::string& value)
            : value{value} {
        node_type = ast_node_type::IDENTIFIER;
    }
    std::string value;
};

struct ast_unary_operation final : ast_node {
    ast_unary_operation(ast_node_ptr left, const std::string& operat)
            : left{std::move(left)}, operat{operat} {
        node_type = ast_node_type::UNARY_OPERATION;
    }
    ast_node_ptr left;
    std::string  operat;
};

struct ast_binary_operation final : ast_node {
    ast_binary_operation(ast_node_ptr left, ast_node_ptr right,
            const std::string& operat)
            : left{std::move(left)}, right{std::move(right)}, operat{operat} {
        node_type = ast_node_type::BINARY_OPERATION;
    }
    ast_node_ptr left;
    ast_node_ptr right;
    std::string  operat;
};

struct ast_grouping_expression final : ast_node {
    ast_grouping_expression(ast_node_ptr expr)
            : expr{std::move(expr)} {
        node_type = ast_node_type::GROUPING_EXPRESSION;
    }
    ast_node_ptr expr;
};

struct ast_func_param final : ast_node {
    ast_func_param(const std::string& name, bool constant,
            const std::string& type)
            : name{name}, constant{constant}, type{type} {
        node_type = ast_node_type::FUNC_PARAM;
    }
    std::string name;
    bool        constant;
    std::string type;
};

struct ast_function_declaration final : ast_node {
    ast_function_declaration(
            const std::string& name,
            const std::vector<ast_func_param>& params,
            const std::string& return_type,
            ast_block body)
            : name{name}, params{params}, return_type{return_type},
              body{std::move(body)} {
        node_type = ast_node_type::FUNCTION_DECLARATION;
    }
    std::string                 name;
    std::vector<ast_func_param> params;
    std::string                 return_type;
    ast_block                   body;
};

struct ast_function_return final : ast_node {
    ast_function_return(ast_node_ptr value)
            : value{std::move(value)} {
        node_type = ast_node_type::FUNCTION_RETURN;
    }
    ast_node_ptr value;
};

struct ast_function_call final : ast_node {
    ast_function_call(const std::string& name, std::vector<ast_node_ptr>& params)
            : name{name}, params{std::move(params)} {
        node_type = ast_node_type::FUNCTION_CALL;
    }
    std::string               name;
    std::vector<ast_node_ptr> params;
};

struct ast_variable_declaration final : ast_node {
    ast_variable_declaration(const std::string& name, bool constant,
            const std::string& type, ast_node_ptr initializer)
            : name{name}, constant{constant}, type{type},
              initializer{std::move(initializer)} {
        node_type = ast_node_type::VARIABLE_DECLARATION;
    }
    std::string  name;
    bool         constant;
    std::string  type;
    ast_node_ptr initializer;
};

struct ast_if final : ast_node {
    ast_if(ast_node_ptr condition, ast_block if_block, ast_node_ptr else_block)
            : condition{std::move(condition)}, if_block{std::move(if_block)},
              else_block{std::move(else_block)} {
        node_type = ast_node_type::IF;
    }
    ast_node_ptr condition;
    ast_block    if_block;
    ast_node_ptr else_block;
};

struct ast_while final : ast_node {
    ast_while(ast_node_ptr condition, ast_block body)
            : condition{std::move(condition)}, body{std::move(body)} {
        node_type = ast_node_type::WHILE;
    }
    ast_node_ptr condition;
    ast_block    body;
};

struct ast_do_while final : ast_node {
    ast_do_while(ast_node_ptr condition, ast_block body)
            : condition{std::move(condition)}, body{std::move(body)} {
        node_type = ast_node_type::DO_WHILE;
    }
    ast_node_ptr condition;
    ast_block    body;
};

struct ast_for final : ast_node {
    ast_for(ast_node_ptr init_statement, ast_node_ptr condition,
            ast_node_ptr iteration_expr, ast_block body)
            : init_statement{std::move(init_statement)},
              condition{std::move(condition)},
              iteration_expr{std::move(iteration_expr)}, body{std::move(body)} {
        node_type = ast_node_type::FOR;
    }
    ast_node_ptr init_statement;
    ast_node_ptr condition;
    ast_node_ptr iteration_expr;
    ast_block    body;
};

}

#endif //WAYWARD_AST_H

