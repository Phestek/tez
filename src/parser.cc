#include "parser.h"

#include <iostream>

#include "ast.h"

namespace wayward {

parser::parser(const std::vector<token>& tokens)
        : _tokens{tokens} {
}

ast_program parser::parse() {
    ast_program program;
    for(token token = next_token(); token.type != token_type::eof;
            token = next_token()) {
        program.nodes.push_back(parse_expression(token));
    }
    return program;
}

token parser::next_token() {
    return _tokens.at(_current_token++);
}

token parser::next_token(token_type type) {
    auto token = _tokens.at(_current_token++);
    if(token.type != type) {
        throw std::invalid_argument{"Token type check failed. Expected: "
                + to_string(type) + ", got " + to_string(token.type) + "."};
    }
    return token;
}

token parser::next_token(token_type type, const std::string& value) {
    auto token = _tokens.at(_current_token++);
    if(token.type != type) {
        throw std::invalid_argument{"Token type check failed. Expected: "
                + to_string(type) + ", got " + to_string(token.type) + "."};
    } else if(token.value != value) {
        throw std::invalid_argument{"Token value check failed. Expected: \""
                + value + "\", got \"" + token.value + "\"."};
    }
    return token;
}

token parser::peek_token(uint depth) {
    return _tokens.at(_current_token + depth);
}

token parser::current_token() const {
    return _tokens.at(_current_token);
}

ast_node_ptr parser::parse_expression(const token& token) {
    ast_node_ptr node;
    switch(token.type) {
        case token_type::kw_func:
            node = parse_function();
            break;
        case token_type::kw_var:
            node = parse_variable(false);
            break;
        case token_type::kw_let:
            node = parse_variable(true);
            break;
        default:
            throw std::invalid_argument{"Unexpected token \"" + token.value
                    + "\" of type " + to_string(token.type) + " at line "
                    + std::to_string(token.line) + "."};
    }
    return node;
}

std::unique_ptr<ast_function_declaration> parser::parse_function() {
    auto func_decl = std::make_unique<ast_function_declaration>();
    func_decl->name = next_token(token_type::identifier).value;
    next_token(token_type::l_paren);
    for(token token = next_token(); token.type != token_type::r_paren;
            token = next_token()) {
    }
    next_token(token_type::arrow);
    func_decl->return_type = next_token(token_type::identifier).value;
    next_token(token_type::l_brace);
    for(auto token = next_token(); token.type != token_type::r_brace;
            token = next_token()) {
        func_decl->body.nodes.push_back(parse_function_parameter(token));
    }
    return func_decl;
}

std::unique_ptr<ast_function_parameter> parser::parse_function_parameter(token token) {
    std::unique_ptr<ast_function_parameter> param;
    param->name = token.value;
    next_token(token_type::colon);
    token = next_token();
    if(token.type == token_type::kw_var) {
        param->constant = false;
        token = next_token(token_type::identifier);
    }
    if(token.type != token_type::identifier) {
        throw std::invalid_argument{"expected id, got shit."};
    }
    param->type = token.value;
    return param;
}

std::unique_ptr<ast_variable_declaration> parser::parse_variable(bool constant) {
    std::unique_ptr<ast_variable_declaration> var_decl;
    var_decl->constant = constant;
    var_decl->name = next_token(token_type::identifier).value;
    next_token(token_type::colon);
    var_decl->type = next_token(token_type::identifier).value;
    next_token(token_type::equals);
    for(auto token = next_token(); token.type != token_type::semicolon;
            token = next_token()) {
    }
    return var_decl;
}

}

