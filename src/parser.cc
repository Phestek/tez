#include "parser.h"

#include <iostream>

namespace wayward {

parser::parser(const std::vector<token>& tokens)
        : _tokens{tokens} {
}

std::vector<ast_node_ptr> parser::parse() {
    std::vector<ast_node_ptr> ast;
    _current = 0;
    while(_current < _tokens.size()) {
        ast.push_back(statement());
    }
    return ast;
}

bool parser::errors_reported() const {
    return _errors_reported;
}

token parser::next_token() {
    if(peek_token(0).type == token_type::eof) {
        return _tokens.at(_current);
    }
    return _tokens.at(_current++);
}

token parser::next_token(token_type type, token_type skip_until) {
    if(peek_token(0).type == token_type::eof) {
        return _tokens.at(_current);
    }
    auto token = _tokens.at(_current++);
    if(token.type != type) {
        report_error("Expected " + to_string(type) + ", got "
                + to_string(token.type), skip_until);
    }
    return token;
}

token parser::peek_token(size_t depth) const {
    if(_current + depth >= _tokens.size()) {
        return _tokens.back();  // Return EOF.
    }
    return _tokens.at(_current + depth);
}

void parser::report_error(const std::string& message, token_type skip_until) {
    _errors_reported = true;
    auto token = peek_token(0);
    std::cerr << token.filename << ':' << token.line << ':' << token.column
            << ": " << message << ".\n";
    while(!match_token({skip_until})) {}
}

ast_node_ptr parser::statement() {
    ast_node_ptr node;
    auto token = peek_token(0);
    if(match_token({token_type::kw_func})) {
        node = function_declaration();
    } else {
        node = expression();
        next_token(token_type::semicolon);
        //report_error("Unexpected token");
    }
    return node;
}

ast_func_param parser::function_param() {
    auto name = next_token(token_type::identifier).value;
    next_token(token_type::colon);
    bool constant = true;
    if(match_token({token_type::kw_var})) {
        constant = false;
    }
    auto type = next_token(token_type::identifier).value;
    return ast_func_param{name, constant, type};
}

ast_node_ptr parser::function_declaration() {
    // Function name.
    auto name = next_token(token_type::identifier).value;
    // Function parameters.
    next_token(token_type::l_paren);
    std::vector<ast_func_param> params;
    if(!check_token(token_type::r_paren)) {
        do {
            params.push_back(function_param());
        } while(match_token({token_type::comma}));
    }
    next_token(token_type::r_paren);
    // Return type.
    next_token(token_type::arrow);
    auto return_type = next_token(token_type::identifier).value;
    next_token(token_type::l_brace);
    // Body.
    std::vector<ast_node_ptr> body;
    while(!match_token({token_type::r_brace})) {
        body.push_back(statement());
    }
    return std::make_unique<ast_function>(name, params, return_type, body);
}

ast_node_ptr parser::expression() {
    return equality();
}

ast_node_ptr parser::equality() {
    auto expr = comparison();
    while(match_token({token_type::bang_equals, token_type::equals_equals})) {
        const auto operat = peek_token(-1);
        auto right = comparison();
        expr = std::make_unique<ast_binary_operation>(std::move(expr),
                std::move(right), to_string(operat.type));
    }
    return expr;
}

ast_node_ptr parser::comparison() {
    auto expr = term();
    while(match_token({token_type::greater, token_type::greater_equals,
            token_type::less, token_type::less_equals})) {
        const auto operat = peek_token(-1);
        auto right = term();
        expr = std::make_unique<ast_binary_operation>(std::move(expr),
                std::move(right), to_string(operat.type));
    }
    return expr;
}

ast_node_ptr parser::term() {
    auto expr = factor();
    while(match_token({token_type::multiply, token_type::divide,
            token_type::modulo})) {
        const auto operat = peek_token(-1);
        auto right = factor();
        expr = std::make_unique<ast_binary_operation>(std::move(expr),
                std::move(right), to_string(operat.type));
    }
    return expr;
}

ast_node_ptr parser::factor() {
    auto expr = unary();
    while(match_token({token_type::plus, token_type::minus})) {
        const auto operat = peek_token(-1);
        auto right = unary();
        expr = std::make_unique<ast_binary_operation>(std::move(expr),
                std::move(right), to_string(operat.type));
    }
    return expr;
}

ast_node_ptr parser::unary() {
    if(match_token({token_type::bang, token_type::minus})) {
        auto operat = peek_token(-1);
        auto right = unary();
        return std::make_unique<ast_unary_operation>(std::move(right),
                to_string(operat.type));
    }
    return primary();
}

ast_node_ptr parser::primary() {
    auto token = peek_token(0);
    if(match_token({token_type::kw_true})) {
        return std::make_unique<ast_boolean>(true);
    }
    if(match_token({token_type::kw_false})) {
        return std::make_unique<ast_boolean>(false);
    }
    //if(match({token_type::kw_nullptr})) {
    //    return std::make_unique<ast_boolean>(true);
    //}
    if(match_token({token_type::integer})) {
        return std::make_unique<ast_integer>(std::stoi(token.value));
    }
    if(match_token({token_type::real_number})) {
        return std::make_unique<ast_real_number>(std::stod(token.value));
    }
    if(match_token({token_type::identifier})) {
        return std::make_unique<ast_identifier>(token.value);
    }
    report_error("Unexpected token");
    return nullptr;
}

bool parser::match_token(const std::initializer_list<token_type>& types) {
    const auto token = peek_token(0);
    for(const auto& type : types) {
        if(token.type == type && peek_token().type != token_type::eof) {
            ++_current;
            return true;
        }
    }
    return false;
}

bool parser::check_token(token_type type) const {
    const auto token = peek_token(0);
    if(token.type == type && peek_token().type != token_type::eof) {
        return true;
    }
    return false;
}

}

