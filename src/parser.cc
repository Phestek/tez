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
        case token_type::keyword:
            if(token.value == "func") {
                node = parse_function();
            } else if(token.value == "var") {
            } else if(token.value == "let") {
            } else {
                throw std::invalid_argument{"Unexpected keyword."};
            }
            break;
        default:
            throw std::invalid_argument{"Unexpected token at line "
                    + std::to_string(token.line) + "."};
    }
    return node;
}

std::unique_ptr<ast_function_declaration> parser::parse_function() {
    auto func_decl = std::make_unique<ast_function_declaration>();
    func_decl->name = next_token(token_type::identifier).value;
    next_token(token_type::parenthesis, "(");
    while(true) {
        auto token = next_token();
        if(token.type == token_type::parenthesis) {
            if(token.value == ")") {
                break;
            } else {
                token = next_token();
            }
        }
    }
    next_token(token_type::operat, "->");
    func_decl->return_type = next_token(token_type::identifier).value;
    next_token(token_type::brace, "{");
    for(auto token = next_token(); token.value != "}"; token = next_token()) {
        func_decl->body.nodes.push_back(parse_expression(token));
    }
    return func_decl;
}

}

