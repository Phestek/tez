#ifndef WAYWARD_AST_H
#define WAYWARD_AST_H

#include <string>
#include <memory>

namespace wayward {

enum class ast_node_type {
    undefined,
    block,                  // Represents a block of code - { ... } .
    boolean,                // True / false value.
    integer,                // Integer value.
    real_number,            // Floating point value.
    string,                 // Sequence of characters.
    identifier,             // Variable / constant name.
    unary_operation,        // Operation with single operand.
    binary_operation,       // Operation with two operands.
    grouping_expression,    // Expression inside parenthesis.
    func_param,             // Function parameter.
    function_declaration,   // Function declaration.
    function_return,        // 'return'.
    function_call,          // Call to (previously declared) function.
    type,                   // Type representation. Just type, pointer or array.
    variable_declaration,   // Used both for 'var' and 'let'.
    _if,                    // 'if else' statement.
    _while,                 // 'while' loop.
    _for,                   // 'for' loop.
};

struct ast_node {
    virtual ~ast_node() {}
    ast_node_type node_type = ast_node_type::undefined;
};
using ast_node_ptr = std::unique_ptr<ast_node>;

struct ast_block final : ast_node {
    ast_block(std::vector<ast_node_ptr>& statements)
            : statements{std::move(statements)} {
        node_type = ast_node_type::block;
    }
    std::vector<ast_node_ptr> statements;
};

struct ast_boolean final : ast_node {
    ast_boolean(bool value)
            : value{value} {
        node_type = ast_node_type::boolean;
    }
    bool value;
};

struct ast_integer final : ast_node {
    ast_integer(long long value)
            : value{value} {
        node_type = ast_node_type::integer;
    }
    long long value;
};

struct ast_real_number final : ast_node {
    ast_real_number(double value)
            : value{value} {
        node_type = ast_node_type::real_number;
    }
    double value;
};

struct ast_string final : ast_node {
    ast_string(const std::string& value)
            : value{value} {
        node_type = ast_node_type::string;
    }
    std::string value;
};

struct ast_identifier final : ast_node {
    ast_identifier(const std::string& value)
            : value{value} {
        node_type = ast_node_type::identifier;
    }
    std::string value;
};

struct ast_unary_operation final : ast_node {
    ast_unary_operation(ast_node_ptr left, const std::string& operat)
            : left{std::move(left)}, operat{operat} {
        node_type = ast_node_type::unary_operation;
    }
    ast_node_ptr left;
    std::string  operat;
};

struct ast_binary_operation final : ast_node {
    ast_binary_operation(ast_node_ptr left, ast_node_ptr right,
            const std::string& operat)
            : left{std::move(left)}, right{std::move(right)}, operat{operat} {
        node_type = ast_node_type::binary_operation;
    }
    ast_node_ptr left;
    ast_node_ptr right;
    std::string  operat;
};

struct ast_grouping_expression final : ast_node {
    ast_grouping_expression(ast_node_ptr expr)
            : expr{std::move(expr)} {
        node_type = ast_node_type::grouping_expression;
    }
    ast_node_ptr expr;
};

struct ast_func_param final : ast_node {
    ast_func_param(const std::string& name, bool constant,
            const std::string& type)
            : name{name}, constant{constant}, type{type} {
        node_type = ast_node_type::func_param;
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
        node_type = ast_node_type::function_declaration;
    }
    std::string                 name;
    std::vector<ast_func_param> params;
    std::string                 return_type;
    ast_block                   body;
};

struct ast_function_return final : ast_node {
    ast_function_return(ast_node_ptr value)
            : value{std::move(value)} {
        node_type = ast_node_type::function_return;
    }
    ast_node_ptr value;
};

struct ast_function_call final : ast_node {
    ast_function_call(const std::string& name, std::vector<ast_node_ptr>& params)
            : name{name}, params{std::move(params)} {
        node_type = ast_node_type::function_call;
    }
    std::string               name;
    std::vector<ast_node_ptr> params;
};

struct ast_variable_declaration final : ast_node {
    ast_variable_declaration(const std::string& name, bool constant,
            const std::string& type, ast_node_ptr initializer)
            : name{name}, constant{constant}, type{type},
              initializer{std::move(initializer)} {
        node_type = ast_node_type::variable_declaration;
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
        node_type = ast_node_type::_if;
    }
    ast_node_ptr condition;
    ast_block    if_block;
    ast_node_ptr else_block;
};

struct ast_while final : ast_node {
    ast_while(ast_node_ptr condition, ast_block body)
            : condition{std::move(condition)}, body{std::move(body)} {
        node_type = ast_node_type::_while;
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
        node_type = ast_node_type::_for;
    }
    ast_node_ptr init_statement;
    ast_node_ptr condition;
    ast_node_ptr iteration_expr;
    ast_block    body;
};

}

#endif //WAYWARD_AST_H

