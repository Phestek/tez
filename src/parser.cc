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

token parser::next_token(token_type type) {
    if(peek_token(0).type == token_type::eof) {
        return _tokens.at(_current);
    }
    auto token = _tokens.at(_current++);
    if(token.type != type) {
        report_error("Expected " + to_string(type) + ", got "
                + to_string(token.type));
    }
    return token;
}

token parser::peek_token(size_t depth) const {
    if(_current + depth >= _tokens.size()) {
        return _tokens.back();  // Return EOF.
    }
    return _tokens.at(_current + depth);
}

void parser::report_error(const std::string& message) {
    _errors_reported = true;
    auto token = peek_token(0);
    std::cerr << token.filename << ':' << token.line << ':' << token.column
            << ": " << message << ".\n";
    while(!match_token({token_type::semicolon, token_type::r_brace,
            token_type::r_paren, token_type::comma, token_type::eof})) {
        next_token();
    }
}

ast_node_ptr parser::statement() {
    ast_node_ptr node;
    auto token = peek_token(0);
    if(match_token({token_type::kw_func})) {
        node = function_declaration();
    } else if(match_token({token_type::kw_let})) {
        node = variable_declaration(true);
        next_token(token_type::semicolon);
    } else if(match_token({token_type::kw_var})) {
        node = variable_declaration(false);
        next_token(token_type::semicolon);
    } else if(match_token({token_type::identifier})) {
        if(match_token({token_type::l_paren})) {
            node = function_call(token.value);
            next_token(token_type::semicolon);
        }
    } else if(match_token({token_type::kw_return})) {
        node = std::make_unique<ast_function_return>(expression());
        next_token(token_type::semicolon);
    } else if(match_token({token_type::kw_if})) {
        node = if_statement();
    } else if(match_token({token_type::kw_while})) {
        node = while_statement();
    } else {
        node = expression();
    }
    return node;
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
    return std::make_unique<ast_function_declaration>(name, params, return_type,
            body);
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

ast_node_ptr parser::function_call(const std::string& name) {
    std::vector<ast_node_ptr> params;
    if(!match_token({token_type::r_paren})) {
        do {
            params.push_back(expression());
        } while(match_token({token_type::comma}));
    }
    next_token(token_type::r_paren);
    return std::make_unique<ast_function_call>(name, params);
}

ast_node_ptr parser::variable_declaration(bool constant) {
    /* Ways to declare variable:
     * 1) var a: int;      <- explicit type, default initializer (aka zero)
     * 2) var a: int = 10; <- explicit type + explicit initializer.
     * these 2 are super easy to implement. the problem is 3rd one:
     * 3) var a = 10;      <- implicit type
     * it requires some kind of resolver to deduct type. */
    auto name = next_token(token_type::identifier).value;
    next_token(token_type::colon);
    auto type = next_token(token_type::identifier).value;
    if(match_token({token_type::equals})) {
        return std::make_unique<ast_variable_declaration>(name, constant, type,
                expression());
    }
    return std::make_unique<ast_variable_declaration>(name, constant, type, nullptr);
}

ast_node_ptr parser::if_statement() {
    auto condition = expression();
    next_token(token_type::l_brace);
    std::vector<ast_node_ptr> if_block;
    while(!match_token({token_type::r_brace})) {
        if_block.push_back(statement());
    }
    if(!match_token({token_type::kw_else})) {
        return std::make_unique<ast_if>(std::move(condition), if_block, nullptr);
    }
    ast_node_ptr else_block;                // Else block may also be just if!
    if(match_token({token_type::kw_if})) {  // Because of "else if".
        else_block = if_statement();
    } else {
        std::vector<ast_node_ptr> else_body;
        next_token(token_type::l_brace);
        while(!match_token({token_type::r_brace})) {
            else_body.push_back(statement());
        }
        else_block = std::make_unique<ast_block>(else_body);
    }
    return std::make_unique<ast_if>(std::move(condition), if_block,
            std::move(else_block));
}

ast_node_ptr parser::while_statement() {
    auto condition = expression();
    next_token(token_type::l_brace);
    std::vector<ast_node_ptr> body;
    while(!match_token({token_type::r_brace})) {
        body.push_back(statement());
    }
    return std::make_unique<ast_while>(std::move(condition), body);
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
    if(match_token({token_type::string})) {
        return std::make_unique<ast_string>(token.value);
    }
    if(match_token({token_type::identifier})) {
        if(match_token({token_type::l_paren})) {
            return function_call(token.value);
        } else {
            return std::make_unique<ast_identifier>(token.value);
        }
    }
    report_error("Expected primary expression, got " + to_string(token.type));
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

