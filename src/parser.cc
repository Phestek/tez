#include "parser.h"

namespace wayward {

parser::parser(const std::vector<token>& tokens)
        : _tokens{tokens} {
}

std::vector<ast_node_ptr> parser::parse() {
    std::vector<ast_node_ptr> ast;
    _current = 0;
    while(_current < _tokens.size()) {
        ast.push_back(expression());
    }
    return ast;
}

bool parser::errors_reported() const {
    return _errors_reported;
}

token parser::next_token() {
    if(_tokens.at(_current).type == token_type::eof) {
        return _tokens.at(_current);
    }
    return _tokens.at(_current++);
}

token parser::peek_token(size_t depth) const {
    if(_current + depth >= _tokens.size()) {
        return _tokens.back();  // Return EOF.
    }
    return _tokens.at(_current + depth);
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
    //report_error("");
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

}

