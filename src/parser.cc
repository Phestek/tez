#include "parser.h"

#include <iostream>

#include "ast.h"

namespace wayward {

parser::parser(const std::vector<token>& tokens)
        : _tokens{tokens} {
}

ast_program parser::parse() {
    ast_program program;
    for(token token; token.type != token_type::eof; token = next_token()) {
        switch(token.type) {
            case token_type::identifier:
                if(token.value == "func") {
                    program.nodes.push_back(parse_function());
                } else if(token.value == "var") {
                } else if(token.value == "let") {
                } else {
                    //throw;
                }
                break;
            default:
                ;//throw;
        }
    }
    return program;
}

token parser::next_token() {
    return _tokens.at(_current_token++);
}

token parser::next_token(token_type type) {
    auto token = _tokens.at(_current_token++);
    if(token.type != type) {
        throw std::invalid_argument{"Token type check failed."};
    }
    return token;
}

token parser::next_token(token_type type, const std::string& value) {
    auto token = _tokens.at(_current_token++);
    if(token.type != type) {
        throw std::invalid_argument{"Token type check failed."};
    } else if(token.value != value) {
        throw std::invalid_argument{"Token value check failed."};
    }
    return token;
}

token parser::peek_token(uint depth) {
    return _tokens.at(_current_token + depth);
}

token parser::current_token() const {
    return _tokens.at(_current_token);
}

std::shared_ptr<ast_function_declaration> parser::parse_function() {
    auto name = next_token(token_type::identifier).value;
    next_token(token_type::parenthesis, "(");
    // TODO: Function parameters.
    next_token(token_type::parenthesis, ")");
    next_token(token_type::operat, "->");
    auto return_type = next_token(token_type::identifier).value;
    next_token(token_type::brace, "{");
    ast_block body;
    while(current_token().type != token_type::brace
            && current_token().value != "}") {
        // Parse, parse, parse!
        next_token();   // TODO: Temporary.
    }
    return std::make_shared<ast_function_declaration>(name, return_type, body);
}

void parser::parse_variable() {
    std::cout << "var\n";
}

void parser::parse_constant() {
    std::cout << "let\n";
}

}

