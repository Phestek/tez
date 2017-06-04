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
    if(peek_token(0).type == token_type::END_OF_FILE) {
        return _tokens.at(_current);
    }
    return _tokens.at(_current++);
}

token parser::next_token(token_type type) {
    if(peek_token(0).type == token_type::END_OF_FILE) {
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
    while(!match_token({token_type::SEMICOLON, token_type::R_BRACE,
            token_type::R_PAREN, token_type::COMMA, token_type::END_OF_FILE})) {
        next_token();
    }
}

ast_node_ptr parser::statement() {
    ast_node_ptr node;
    auto token = peek_token(0);
    if(match_token({token_type::KW_FUNC})) {
        node = function_declaration();
    } else if(match_token({token_type::KW_LET})) {
        node = variable_declaration(true);
        next_token(token_type::SEMICOLON);
    } else if(match_token({token_type::KW_VAR})) {
        node = variable_declaration(false);
        next_token(token_type::SEMICOLON);
    } else if(match_token({token_type::KW_RETURN})) {
        node = std::make_unique<ast_function_return>(expression());
        next_token(token_type::SEMICOLON);
    } else if(match_token({token_type::KW_IF})) {
        node = if_statement();
    } else if(match_token({token_type::KW_WHILE})) {
        node = while_statement();
    } else if(match_token({token_type::KW_DO})) {
        node = do_while_statement();
    } else if(match_token({token_type::KW_FOR})) {
        node = for_statement();
    } else if(match_token({token_type::KW_BREAK})) {
        node = std::make_unique<ast_break>();
    } else if(match_token({token_type::KW_CONTINUE})) {
        node = std::make_unique<ast_continue>();
    } else if(match_token({token_type::KW_STRUCT})) {
        node = structure();
    } else if(match_token({token_type::KW_ENUM})) {
        node = enumeration();
    } else {
        node = expression();
        next_token(token_type::SEMICOLON);
    }
    return node;
}

ast_block parser::block() {
    next_token(token_type::L_BRACE);
    std::vector<ast_node_ptr> statements;
    try {
        while(!match_token({token_type::R_BRACE})) {
            statements.push_back(statement());
        }
    } catch(const std::out_of_range& e) {
        report_error("Missing closing '}'");
    }
    return ast_block{statements};
}

ast_node_ptr parser::function_declaration() {
    // Function name.
    auto name = next_token(token_type::IDENTIFIER).value;
    // Function parameters.
    next_token(token_type::L_PAREN);
    std::vector<ast_func_param> params;
    if(!check_token(token_type::R_PAREN)) {
        do {
            params.push_back(function_param());
        } while(match_token({token_type::COMMA}));
    }
    next_token(token_type::R_PAREN);
    // Return type.
    next_token(token_type::ARROW);
    auto return_type = next_token(token_type::IDENTIFIER).value;
    // Body.
    auto body = block();
    return std::make_unique<ast_function_declaration>(name, params, return_type,
            std::move(body));
}

ast_func_param parser::function_param() {
    auto name = next_token(token_type::IDENTIFIER).value;
    next_token(token_type::COLON);
    bool constant = true;
    if(match_token({token_type::KW_VAR})) {
        constant = false;
    }
    auto type = next_token(token_type::IDENTIFIER).value;
    return ast_func_param{name, constant, type};
}

ast_node_ptr parser::function_call(const std::string& name) {
    std::vector<ast_node_ptr> params;
    if(!match_token({token_type::R_PAREN})) {
        do {
            params.push_back(expression());
        } while(match_token({token_type::COMMA}));
    }
    next_token(token_type::R_PAREN);
    return std::make_unique<ast_function_call>(name, params);
}

ast_node_ptr parser::variable_declaration(bool constant) {
    /* Ways to declare variable:
     * 1) var a: int;      <- explicit type, default initializer (aka zero)
     * 2) var a: int = 10; <- explicit type + explicit initializer.
     * these 2 are super easy to implement. the problem is 3rd one:
     * 3) var a = 10;      <- implicit type
     * it requires some kind of resolver to deduct type. */
    auto name = next_token(token_type::IDENTIFIER).value;
    next_token(token_type::COLON);
    auto type = next_token(token_type::IDENTIFIER).value;
    if(match_token({token_type::EQUALS})) {
        return std::make_unique<ast_variable_declaration>(name, constant, type,
                expression());
    }
    return std::make_unique<ast_variable_declaration>(name, constant, type, nullptr);
}

ast_node_ptr parser::structure() {
    auto name = next_token(token_type::IDENTIFIER).value;
    next_token(token_type::L_BRACE);
    std::vector<ast_struct::field> fields;
    while(!match_token({token_type::R_BRACE})) {
        ast_struct::field field;
        field.name = next_token(token_type::IDENTIFIER).value;
        next_token(token_type::COLON);
        field.type = next_token(token_type::IDENTIFIER).value;
        next_token(token_type::SEMICOLON);
        fields.push_back(field);
    }
    return std::make_unique<ast_struct>(name, fields);
}

ast_node_ptr parser::enumeration() {
    return std::make_unique<ast_enum>();
}

ast_node_ptr parser::if_statement() {
    auto condition = expression();
    auto if_block = block();
    if(!match_token({token_type::KW_ELSE})) {
        return std::make_unique<ast_if>(std::move(condition),
                std::move(if_block), nullptr);
    }
    ast_node_ptr else_block;                // Else block may also be just if!
    if(match_token({token_type::KW_IF})) {  // Because of "else if".
        else_block = if_statement();
    } else {
        else_block = std::make_unique<ast_block>(block());
    }
    return std::make_unique<ast_if>(std::move(condition), std::move(if_block),
            std::move(else_block));
}

ast_node_ptr parser::while_statement() {
    auto condition = expression();
    auto body = block();
    return std::make_unique<ast_while>(std::move(condition), std::move(body));
}

ast_node_ptr parser::do_while_statement() {
    auto body = block();
    next_token(token_type::KW_WHILE);
    auto condition = expression();
    next_token(token_type::SEMICOLON);
    return std::make_unique<ast_do_while>(std::move(condition), std::move(body));
}

ast_node_ptr parser::for_statement() {
    auto init_statement = statement();
    auto condition      = statement();
    auto iteration_expr = expression();
    auto body           = block();
    return std::make_unique<ast_for>(std::move(init_statement),
            std::move(condition), std::move(iteration_expr), std::move(body));
}

ast_node_ptr parser::expression() {
    return assignment();
}

ast_node_ptr parser::assignment() {
    auto expr = logical_or();
    while(match_token({token_type::EQUALS, token_type::PLUS_EQUALS,
            token_type::MINUS_EQUALS, token_type::MULTIPLY_EQUALS,
            token_type::DIVIDE_EQUALS, token_type::MODULO_EQUALS})) {
        const auto op = peek_token(-1);
        auto right = logical_or();
        expr = std::make_unique<ast_binary_operation>(std::move(expr),
                std::move(right), to_string(op.type));
    }
    return expr;
}

ast_node_ptr parser::logical_or() {
    auto expr = logical_and();
    while(match_token({token_type::LOGICAL_OR})) {
        const auto op = peek_token(-1);
        auto right = logical_and();
        expr = std::make_unique<ast_binary_operation>(std::move(expr),
                std::move(right), to_string(op.type));
    }
    return expr;
}

ast_node_ptr parser::logical_and() {
    auto expr = bitwise_or();
    while(match_token({token_type::LOGICAL_AND})) {
        const auto op = peek_token(-1);
        auto right = bitwise_or();
        expr = std::make_unique<ast_binary_operation>(std::move(expr),
                std::move(right), to_string(op.type));
    }
    return expr;
}

ast_node_ptr parser::bitwise_or() {
    auto expr = bitwise_xor();
    while(match_token({token_type::BITWISE_OR})) {
        const auto op = peek_token(-1);
        auto right = bitwise_xor();
        expr = std::make_unique<ast_binary_operation>(std::move(expr),
                std::move(right), to_string(op.type));
    }
    return expr;
}

ast_node_ptr parser::bitwise_xor() {
    auto expr = bitwise_and();
    while(match_token({token_type::CARET})) {
        const auto op = peek_token(-1);
        auto right = bitwise_and();
        expr = std::make_unique<ast_binary_operation>(std::move(expr),
                std::move(right), to_string(op.type));
    }
    return expr;
}

ast_node_ptr parser::bitwise_and() {
    auto expr = equality();
    while(match_token({token_type::AMPERSAND})) {
        const auto op = peek_token(-1);
        auto right = equality();
        expr = std::make_unique<ast_binary_operation>(std::move(expr),
                std::move(right), to_string(op.type));
    }
    return expr;
}

ast_node_ptr parser::equality() {
    auto expr = comparison();
    while(match_token({token_type::BANG_EQUALS, token_type::EQUALS_EQUALS})) {
        const auto operat = peek_token(-1);
        auto right = comparison();
        expr = std::make_unique<ast_binary_operation>(std::move(expr),
                std::move(right), to_string(operat.type));
    }
    return expr;
}

ast_node_ptr parser::comparison() {
    auto expr = bitwise_shift();
    while(match_token({token_type::GREATER, token_type::GREATER_EQUALS,
            token_type::LESS, token_type::LESS_EQUALS})) {
        const auto operat = peek_token(-1);
        auto right = bitwise_shift();
        expr = std::make_unique<ast_binary_operation>(std::move(expr),
                std::move(right), to_string(operat.type));
    }
    return expr;
}

ast_node_ptr parser::bitwise_shift() {
    auto expr = term();
    while(match_token({token_type::BITWISE_SHIFT_LEFT,
            token_type::BITWISE_SHIFT_RIGHT})) {
        const auto operat = peek_token(-1);
        auto right = term();
        expr = std::make_unique<ast_binary_operation>(std::move(expr),
                std::move(right), to_string(operat.type));
    }
    return expr;
}

ast_node_ptr parser::term() {
    auto expr = factor();
    while(match_token({token_type::ASTERISK, token_type::SLASH,
            token_type::MODULO})) {
        const auto operat = peek_token(-1);
        auto right = factor();
        expr = std::make_unique<ast_binary_operation>(std::move(expr),
                std::move(right), to_string(operat.type));
    }
    return expr;
}

ast_node_ptr parser::factor() {
    auto expr = unary();
    while(match_token({token_type::PLUS, token_type::MINUS})) {
        const auto operat = peek_token(-1);
        auto right = unary();
        expr = std::make_unique<ast_binary_operation>(std::move(expr),
                std::move(right), to_string(operat.type));
    }
    return expr;
}

ast_node_ptr parser::unary() {
    if(match_token({token_type::BANG, token_type::MINUS})) {
        auto operat = peek_token(-1);
        auto right = unary();
        return std::make_unique<ast_unary_operation>(std::move(right),
                to_string(operat.type));
    }
    return primary();
}

ast_node_ptr parser::primary() {
    auto token = peek_token(0);
    if(match_token({token_type::KW_TRUE})) {
        return std::make_unique<ast_boolean>(true);
    }
    if(match_token({token_type::KW_FALSE})) {
        return std::make_unique<ast_boolean>(false);
    }
    //if(match({token_type::kw_nullptr})) {
    //    return std::make_unique<ast_boolean>(true);
    //}
    if(match_token({token_type::INTEGER})) {
        return std::make_unique<ast_integer>(std::stoi(token.value));
    }
    if(match_token({token_type::REAL_NUMBER})) {
        return std::make_unique<ast_real_number>(std::stod(token.value));
    }
    if(match_token({token_type::STRING})) {
        return std::make_unique<ast_string>(token.value);
    }
    if(match_token({token_type::L_PAREN})) {
        auto expr = std::make_unique<ast_grouping_expression>(expression());
        next_token(token_type::R_PAREN);
        return expr;
    }
    if(match_token({token_type::IDENTIFIER})) {
        if(match_token({token_type::L_PAREN})) {
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
        if(token.type == type && peek_token().type != token_type::END_OF_FILE) {
            ++_current;
            return true;
        }
    }
    return false;
}

bool parser::check_token(token_type type) const {
    const auto token = peek_token(0);
    if(token.type == type && peek_token().type != token_type::END_OF_FILE) {
        return true;
    }
    return false;
}

}

