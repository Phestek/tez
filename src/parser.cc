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
        node = std::make_unique<ast_return>(); 
        dynamic_cast<ast_return&>(*node).value = expression();
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
    ast_block _block;
    next_token(token_type::L_BRACE);
    try {
        while(!match_token({token_type::R_BRACE})) {
            _block.statements.push_back(statement());
        }
    } catch(const std::out_of_range& e) {
        report_error("Missing closing '}'");
    }
    return _block;
}

ast_node_ptr parser::function_declaration() {
    auto func = std::make_unique<ast_func_decl>();
    func->name = next_token(token_type::IDENTIFIER).value;
    next_token(token_type::L_PAREN);
    if(!check_token(token_type::R_PAREN)) {
        do {
            func->params.push_back(function_param());
        } while(match_token({token_type::COMMA}));
    }
    next_token(token_type::R_PAREN);
    next_token(token_type::ARROW);
    func->return_type = next_token(token_type::IDENTIFIER).value;
    func->body = block();
    return func;
}

ast_func_decl::param parser::function_param() {
    ast_func_decl::param param;
    param.name = next_token(token_type::IDENTIFIER).value;
    next_token(token_type::COLON);
    if(match_token({token_type::KW_VAR})) {
        param.constant = false;
    }
    param.type = next_token(token_type::IDENTIFIER).value;
    return param;
}

ast_node_ptr parser::function_call(const std::string& name) {
    auto call = std::make_unique<ast_func_call>();
    call->name = name;
    if(!match_token({token_type::R_PAREN})) {
        do {
            call->args.push_back(expression());
        } while(match_token({token_type::COMMA}));
    }
    next_token(token_type::R_PAREN);
    return call;
}

ast_node_ptr parser::variable_declaration(bool constant) {
    auto decl = std::make_unique<ast_var_decl>();
    decl->constant = constant;
    decl->name = next_token(token_type::IDENTIFIER).value;
    next_token(token_type::COLON);
    decl->type = next_token(token_type::IDENTIFIER).value;
    if(match_token({token_type::EQUALS})) {
        decl->initializer = expression();
    }
    return decl;
}

ast_node_ptr parser::structure() {
    auto struct_decl = std::make_unique<ast_struct>();
    struct_decl->name = next_token(token_type::IDENTIFIER).value;
    next_token(token_type::L_BRACE);
    while(!match_token({token_type::R_BRACE})) {
        ast_struct::field field;
        field.name = next_token(token_type::IDENTIFIER).value;
        next_token(token_type::COLON);
        field.type = next_token(token_type::IDENTIFIER).value;
        next_token(token_type::SEMICOLON);
        struct_decl->fields.push_back(field);
    }
    return struct_decl;
}

ast_node_ptr parser::enumeration() {
    auto enumeration = std::make_unique<ast_enum>();
    std::size_t counter = 0;
    enumeration->name = next_token(token_type::IDENTIFIER).value;
    next_token(token_type::L_BRACE);
    do {
        ast_enum::enumerator enumerator;
        enumerator.name = next_token(token_type::IDENTIFIER).value;
        if(match_token({token_type::EQUALS})) {
            counter = std::stoi(next_token(token_type::INTEGER).value);
        }
        enumerator.value = counter;
        enumeration->enumerations.push_back(enumerator);
        ++counter;
    } while(match_token({token_type::COMMA}));
    next_token(token_type::R_BRACE);
    return enumeration;
}

ast_node_ptr parser::if_statement() {
    auto if_stat = std::make_unique<ast_if>();
    if_stat->condition = expression();
    auto if_block = block();
    if(!match_token({token_type::KW_ELSE})) {
        return if_stat;
    }
    ast_node_ptr else_block;                // Else block may also be just if!
    if(match_token({token_type::KW_IF})) {  // Because of "else if".
        else_block = if_statement();
    } else {
        else_block = std::make_unique<ast_block>(block());
    }
    if_stat->else_block = std::move(else_block);
    return if_stat;
}

ast_node_ptr parser::while_statement() {
    auto while_loop = std::make_unique<ast_while>();
    while_loop->condition = expression();
    while_loop->body = block();
    return while_loop;
}

ast_node_ptr parser::do_while_statement() {
    auto do_while = std::make_unique<ast_do_while>();
    do_while->body = block();
    next_token(token_type::KW_WHILE);
    do_while->condition = expression();
    next_token(token_type::SEMICOLON);
    return do_while;
}

ast_node_ptr parser::for_statement() {
    auto for_loop = std::make_unique<ast_for>();
    for_loop->init_statement = statement();
    for_loop->condition      = statement();
    for_loop->iteration_expr = expression();
    for_loop->body           = block();
    return for_loop;
}

ast_node_ptr parser::expression() {
    return assignment();
}

ast_node_ptr parser::assignment() {
    auto expr = logical_or();
    while(match_token({token_type::EQUALS, token_type::PLUS_EQUALS,
            token_type::MINUS_EQUALS, token_type::MULTIPLY_EQUALS,
            token_type::DIVIDE_EQUALS, token_type::MODULO_EQUALS})) {
        auto op = std::make_unique<ast_binary_operation>();
        op->left = std::move(expr);
        op->operat = to_string(peek_token(-1).type);
        op->right = logical_or();
        return op;
    }
    return expr;
}

ast_node_ptr parser::logical_or() {
    auto expr = logical_and();
    while(match_token({token_type::LOGICAL_OR})) {
        auto op = std::make_unique<ast_binary_operation>();
        op->left = std::move(expr);
        op->operat = to_string(peek_token(-1).type);
        op->right = logical_and();
        return op;
    }
    return expr;
}

ast_node_ptr parser::logical_and() {
    auto expr = bitwise_or();
    while(match_token({token_type::LOGICAL_AND})) {
        auto op = std::make_unique<ast_binary_operation>();
        op->left = std::move(expr);
        op->operat = to_string(peek_token(-1).type);
        op->right = bitwise_or();
        return op;
    }
    return expr;
}

ast_node_ptr parser::bitwise_or() {
    auto expr = bitwise_xor();
    while(match_token({token_type::BITWISE_OR})) {
        auto op = std::make_unique<ast_binary_operation>();
        op->left = std::move(expr);
        op->operat = to_string(peek_token(-1).type);
        op->right = bitwise_xor();
        return op;
    }
    return expr;
}

ast_node_ptr parser::bitwise_xor() {
    auto expr = bitwise_and();
    while(match_token({token_type::CARET})) {
        auto op = std::make_unique<ast_binary_operation>();
        op->left = std::move(expr);
        op->operat = to_string(peek_token(-1).type);
        op->right = bitwise_and();
        return op;
    }
    return expr;
}

ast_node_ptr parser::bitwise_and() {
    auto expr = equality();
    while(match_token({token_type::AMPERSAND})) {
        auto op = std::make_unique<ast_binary_operation>();
        op->left = std::move(expr);
        op->operat = to_string(peek_token(-1).type);
        op->right = equality();
        return op;
    }
    return expr;
}

ast_node_ptr parser::equality() {
    auto expr = comparison();
    while(match_token({token_type::BANG_EQUALS, token_type::EQUALS_EQUALS})) {
        auto op = std::make_unique<ast_binary_operation>();
        op->left = std::move(expr);
        op->operat = to_string(peek_token(-1).type);
        op->right = comparison();
        return op;
    }
    return expr;
}

ast_node_ptr parser::comparison() {
    auto expr = bitwise_shift();
    while(match_token({token_type::GREATER, token_type::GREATER_EQUALS,
            token_type::LESS, token_type::LESS_EQUALS})) {
        auto op = std::make_unique<ast_binary_operation>();
        op->left = std::move(expr);
        op->operat = to_string(peek_token(-1).type);
        op->right = bitwise_shift();
        return op;
    }
    return expr;
}

ast_node_ptr parser::bitwise_shift() {
    auto expr = term();
    while(match_token({token_type::BITWISE_SHIFT_LEFT,
            token_type::BITWISE_SHIFT_RIGHT})) {
        auto op = std::make_unique<ast_binary_operation>();
        op->left = std::move(expr);
        op->operat = to_string(peek_token(-1).type);
        op->right = term();
        return op;
    }
    return expr;
}

ast_node_ptr parser::term() {
    auto expr = factor();
    while(match_token({token_type::ASTERISK, token_type::SLASH,
            token_type::MODULO})) {
        auto op = std::make_unique<ast_binary_operation>();
        op->left = std::move(expr);
        op->operat = to_string(peek_token(-1).type);
        op->right = factor();
        return op;
    }
    return expr;
}

ast_node_ptr parser::factor() {
    auto expr = unary();
    while(match_token({token_type::PLUS, token_type::MINUS})) {
        auto op = std::make_unique<ast_binary_operation>();
        op->left = std::move(expr);
        op->operat = to_string(peek_token(-1).type);
        op->right = unary();
        return op;
    }
    return expr;
}

ast_node_ptr parser::unary() {
    if(match_token({token_type::BANG, token_type::MINUS})) {
        auto op = std::make_unique<ast_unary_operation>();
        op->operat = to_string(peek_token(-1).type);
        op->left = unary();
        return op;
    }
    return primary();
}

ast_node_ptr parser::primary() {
    auto token = peek_token(0);
    if(match_token({token_type::KW_TRUE})) {
        auto b = std::make_unique<ast_boolean>();
        b->value = true;
        return b;
    }
    if(match_token({token_type::KW_FALSE})) {
        auto b = std::make_unique<ast_boolean>();
        b->value = false;
        return b;
    }
    //if(match({token_type::kw_nullptr})) {
    //    return std::make_unique<ast_boolean>(true);
    //}
    if(match_token({token_type::INTEGER})) {
        auto i = std::make_unique<ast_integer>();
        i->value = false;
        return i;
    }
    if(match_token({token_type::REAL_NUMBER})) {
        auto r = std::make_unique<ast_integer>();
        r->value = false;
        return r;
    }
    if(match_token({token_type::STRING})) {
        auto s = std::make_unique<ast_string>();
        s->value = token.value;
        return s;
    }
    if(match_token({token_type::L_PAREN})) {
        auto expr = std::make_unique<ast_grouping_expression>();
        expr->expr = expression();
        next_token(token_type::R_PAREN);
        return expr;
    }
    if(match_token({token_type::IDENTIFIER})) {
        if(match_token({token_type::L_PAREN})) {
            return function_call(token.value);
        } else {
            auto id = std::make_unique<ast_identifier>();
            id->name = token.value;
            return id;
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

