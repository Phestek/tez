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
    func_param,
    function_declaration,
    variable_declaration
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
            std::vector<ast_node_ptr>& _body)
            : name{name}, params{params}, return_type{return_type} {
        node_type = ast_node_type::function_declaration;
        // Stupid and naive, but works (I hope).
        body.reserve(_body.size());
        for(std::size_t i = 0; i < _body.size(); ++i) {
            body.push_back(std::move(_body[i]));
        }
    }
    std::string                 name;
    std::vector<ast_func_param> params;
    std::string                 return_type;
    std::vector<ast_node_ptr>   body;
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

}

#endif //WAYWARD_AST_H

