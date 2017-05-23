#ifndef WAYWARD_AST_H
#define WAYWARD_AST_H

#include <string>
#include <memory>

namespace wayward {

enum class ast_node_type {
    undefined,
    boolean,
    integer,
    real_number,
    identifier,
    unary_operation,
    binary_operation,
};

struct ast_node {
    virtual ~ast_node() {}
    ast_node_type node_type = ast_node_type::undefined;
};
using ast_node_ptr = std::unique_ptr<ast_node>;

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

}

#endif //WAYWARD_AST_H

