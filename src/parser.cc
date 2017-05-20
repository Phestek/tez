#include "parser.h"

#include <iostream>

#include "ast.h"

namespace wayward {

const std::map<token_type, int> parser::_operator_persistence {
    {token_type::plus,     20},
    {token_type::minus,    20},
    {token_type::multiply, 40},
    {token_type::divide,   40},
    {token_type::modulo,   40},
};

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

bool parser::errors_reported() const {
    return _errors_reported;
}

void parser::report_error(const std::string message) {
    _errors_reported = true;
    auto token = current_token();
    std::cerr << token.filename << ':' << token.line << ':' << token.column
            << ": " << message << ".\n";
    while(token.type != token_type::semicolon) {
        token = next_token();
    }
}

token parser::next_token() {
    return _tokens.at(_current_token++);
}

token parser::next_token(token_type type) {
    auto token = _tokens.at(_current_token++);
    if(token.type != type) {
        report_error("Expected " + to_string(type) + ", got "
                + to_string(token.type) + ".");
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
            report_error("Unexpected token of type \"" + to_string(token.type)
                    + "\" and value \"" + token.value + "\"");
    }
    return node;
}

std::unique_ptr<ast_function_declaration> parser::parse_function() {
    auto func_decl = std::make_unique<ast_function_declaration>();
    func_decl->name = next_token(token_type::identifier).value;
    next_token(token_type::l_paren);
    for(token token = next_token(); token.type != token_type::r_paren;
            token = next_token()) {
        func_decl->params.push_back(parse_function_parameter(token));
    }
    next_token(token_type::arrow);
    func_decl->return_type = next_token(token_type::identifier).value;
    next_token(token_type::l_brace);
    for(auto token = next_token(); token.type != token_type::r_brace;
            token = next_token()) {
        func_decl->body.nodes.push_back(parse_expression(token));
    }
    return func_decl;
}

ast_function_parameter parser::parse_function_parameter(token token) {
    ast_function_parameter param;
    param.name = token.value;
    next_token(token_type::colon);
    token = next_token();
    if(token.type == token_type::kw_var) {
        param.constant = false;
        token = next_token();
    }
    if(token.type != token_type::identifier) {
        report_error("Expected " + to_string(token_type::identifier) + ", got "
                + to_string(token.type) + ".");
    }
    param.type = token.value;
    if(current_token().type == token_type::comma) {
        next_token();
    }
    return param;
}

std::unique_ptr<ast_variable_declaration> parser::parse_variable(bool constant) {
    auto var_decl = std::make_unique<ast_variable_declaration>();
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

