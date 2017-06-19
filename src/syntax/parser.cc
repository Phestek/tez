#include "parser.h"

#include <iostream>

namespace tez {

Parser::Parser(const std::vector<Token>& tokens)
        : _tokens{tokens} {
}

std::vector<Ast_Node_Ptr> Parser::parse() {
    std::vector<Ast_Node_Ptr> ast;
    _current = 0;
    while(_current < _tokens.size()) {
        ast.push_back(statement());
    }
    return ast;
}

bool Parser::errors_reported() const {
    return _errors_reported;
}

Token Parser::next_token() {
    if(_current < _tokens.size()) {
        return _tokens.at(_current++);
    }
    return _tokens.at(_current);
}

Token Parser::next_token(Token_Type type) {
    if(_current >= _tokens.size()) {    // All tokens were consumed.
        return _tokens.at(_current);
    }
    auto token = _tokens.at(_current++);
    if(token.type != type) {
        report_error("Expected " + to_string(type) + ", got "
                + to_string(token.type));
    }
    return token;
}

Token Parser::peek_token(size_t depth) const {
    if(_current + depth >= _tokens.size()) {
        return _tokens.back();  // Return EOF.
    }
    return _tokens.at(_current + depth);
}

void Parser::report_error(const std::string& message) {
    _errors_reported = true;
    auto token = peek_token(0);
    std::cerr << token.filename << ':' << token.line << ':' << token.column
            << ": " << message << ".\n";
    while(!match_token({Token_Type::SEMICOLON, Token_Type::R_BRACE,
            Token_Type::R_PAREN, Token_Type::COMMA})) {
        next_token();
    }
}

Ast_Node_Ptr Parser::statement() {
    Ast_Node_Ptr node;
    auto token = peek_token(0);
    if(match_token({Token_Type::KW_NAMESPACE})) {
        node = namespace_declaration();
    } else if(match_token({Token_Type::KW_FUNC})) {
        node = function_declaration();
    } else if(match_token({Token_Type::KW_LET})) {
        node = variable_declaration(true);
        next_token(Token_Type::SEMICOLON);
    } else if(match_token({Token_Type::KW_VAR})) {
        node = variable_declaration(false);
        next_token(Token_Type::SEMICOLON);
    } else if(match_token({Token_Type::KW_RETURN})) {
        node = std::make_unique<Ast_Return>();
        dynamic_cast<Ast_Return&>(*node).expr = expression();
        next_token(Token_Type::SEMICOLON);
    } else if(match_token({Token_Type::KW_IF})) {
        node = if_statement();
    } else if(match_token({Token_Type::KW_WHILE})) {
        node = while_statement();
    } else if(match_token({Token_Type::KW_DO})) {
        node = do_while_statement();
    } else if(match_token({Token_Type::KW_FOR})) {
        node = for_statement();
    } else if(match_token({Token_Type::KW_BREAK})) {
        node = std::make_unique<Ast_Break>();
    } else if(match_token({Token_Type::KW_CONTINUE})) {
        node = std::make_unique<Ast_Continue>();
    } else if(match_token({Token_Type::KW_STRUCT})) {
        node = structure();
    } else if(match_token({Token_Type::KW_ENUM})) {
        node = enumeration();
    } else {
        node = expression();
        next_token(Token_Type::SEMICOLON);
    }
    return node;
}

Ast_Node_Ptr Parser::namespace_declaration() {
    auto ns = std::make_unique<Ast_Namespace>();
    ns->name = next_token(Token_Type::IDENTIFIER).value;
    ns->body = block();
    return ns;
}

Ast_Block Parser::block() {
    Ast_Block _block;
    next_token(Token_Type::L_BRACE);
    try {
        while(!match_token({Token_Type::R_BRACE})) {
            _block.statements.push_back(statement());
        }
    } catch(const std::out_of_range& e) {
        report_error("Missing closing '}'");
    }
    return _block;
}

Ast_Node_Ptr Parser::function_declaration() {
    auto func = std::make_unique<Ast_Func_Decl>();
    func->name = next_token(Token_Type::IDENTIFIER).value;
    next_token(Token_Type::L_PAREN);
    if(!check_token(Token_Type::R_PAREN)) {
        do {
            func->params.push_back(function_param());
        } while(match_token({Token_Type::COMMA}));
    }
    next_token(Token_Type::R_PAREN);
    if(match_token({Token_Type::ARROW})) {
        func->return_type = next_token(Token_Type::IDENTIFIER).value;
    } else {
        func->return_type = "void";
    }
    func->body = block();
    return func;
}

Ast_Func_Decl::Param Parser::function_param() {
    Ast_Func_Decl::Param param;
    param.name = next_token(Token_Type::IDENTIFIER).value;
    next_token(Token_Type::COLON);
    if(match_token({Token_Type::KW_VAR})) {
        param.constant = false;
    }
    param.type = type();
    return param;
}

Ast_Node_Ptr Parser::function_call(Ast_Node_Ptr name) {
    auto call = std::make_unique<Ast_Func_Call>();
    call->name = std::move(name);
    if(!match_token({Token_Type::R_PAREN})) {
        do {
            call->args.push_back(expression());
        } while(match_token({Token_Type::COMMA}));
        next_token(Token_Type::R_PAREN);
    }
    return call;
}

Ast_Node_Ptr Parser::variable_declaration(bool constant) {
    auto decl = std::make_unique<Ast_Var_Decl>();
    decl->constant = constant;
    decl->name = next_token(Token_Type::IDENTIFIER).value;
    next_token(Token_Type::COLON);
    decl->type = type();
    if(match_token({Token_Type::EQUALS})) {
        decl->initializer = expression();
    }
    return decl;
}

Ast_Node_Ptr Parser::structure() {
    auto struct_decl = std::make_unique<Ast_Struct>();
    struct_decl->name = next_token(Token_Type::IDENTIFIER).value;
    next_token(Token_Type::L_BRACE);
    while(!match_token({Token_Type::R_BRACE})) {
        Ast_Struct::Field field;
        field.name = next_token(Token_Type::IDENTIFIER).value;
        next_token(Token_Type::COLON);
        field.type = next_token(Token_Type::IDENTIFIER).value;
        next_token(Token_Type::SEMICOLON);
        struct_decl->fields.push_back(field);
    }
    return struct_decl;
}

Ast_Node_Ptr Parser::enumeration() {
    auto enumeration = std::make_unique<Ast_Enum>();
    std::size_t counter = 0;
    enumeration->name = next_token(Token_Type::IDENTIFIER).value;
    next_token(Token_Type::L_BRACE);
    do {
        Ast_Enum::Enumerator enumerator;
        enumerator.name = next_token(Token_Type::IDENTIFIER).value;
        if(match_token({Token_Type::EQUALS})) {
            counter = std::stoi(next_token(Token_Type::INTEGER).value);
        }
        enumerator.value = counter;
        enumeration->enumerations.push_back(enumerator);
        ++counter;
    } while(match_token({Token_Type::COMMA}));
    next_token(Token_Type::R_BRACE);
    return enumeration;
}

Ast_Node_Ptr Parser::if_statement() {
    auto if_stat = std::make_unique<Ast_If>();
    if_stat->condition = expression();
    auto if_block = block();
    if(!match_token({Token_Type::KW_ELSE})) {
        return if_stat;
    }
    Ast_Node_Ptr else_block;                // Else block may also be just if!
    if(match_token({Token_Type::KW_IF})) {  // Because of "else if".
        else_block = if_statement();
    } else {
        else_block = std::make_unique<Ast_Block>(block());
    }
    if_stat->else_block = std::move(else_block);
    return if_stat;
}

Ast_Node_Ptr Parser::while_statement() {
    auto while_loop = std::make_unique<Ast_While>();
    while_loop->condition = expression();
    while_loop->body = block();
    return while_loop;
}

Ast_Node_Ptr Parser::do_while_statement() {
    auto do_while = std::make_unique<Ast_Do_While>();
    do_while->body = block();
    next_token(Token_Type::KW_WHILE);
    do_while->condition = expression();
    next_token(Token_Type::SEMICOLON);
    return do_while;
}

Ast_Node_Ptr Parser::for_statement() {
    auto for_loop = std::make_unique<Ast_For>();
    for_loop->init_statement = statement();
    for_loop->condition      = statement();
    for_loop->iteration_expr = expression();
    for_loop->body           = block();
    return for_loop;
}

Ast_Node_Ptr Parser::expression() {
    return assignment();
}

Ast_Node_Ptr Parser::assignment() {
    auto expr = logical_or();
    while(match_token({Token_Type::EQUALS, Token_Type::PLUS_EQUALS,
            Token_Type::MINUS_EQUALS, Token_Type::MULTIPLY_EQUALS,
            Token_Type::DIVIDE_EQUALS, Token_Type::MODULO_EQUALS})) {
        auto op = std::make_unique<Ast_Binary_Operation>();
        op->left = std::move(expr);
        op->operat = to_string(peek_token(-1).type);
        op->right = logical_or();
        return op;
    }
    return expr;
}

Ast_Node_Ptr Parser::logical_or() {
    auto expr = logical_and();
    while(match_token({Token_Type::LOGICAL_OR})) {
        auto op = std::make_unique<Ast_Binary_Operation>();
        op->left = std::move(expr);
        op->operat = to_string(peek_token(-1).type);
        op->right = logical_and();
        return op;
    }
    return expr;
}

Ast_Node_Ptr Parser::logical_and() {
    auto expr = bitwise_or();
    while(match_token({Token_Type::LOGICAL_AND})) {
        auto op = std::make_unique<Ast_Binary_Operation>();
        op->left = std::move(expr);
        op->operat = to_string(peek_token(-1).type);
        op->right = bitwise_or();
        return op;
    }
    return expr;
}

Ast_Node_Ptr Parser::bitwise_or() {
    auto expr = bitwise_xor();
    while(match_token({Token_Type::BITWISE_OR})) {
        auto op = std::make_unique<Ast_Binary_Operation>();
        op->left = std::move(expr);
        op->operat = to_string(peek_token(-1).type);
        op->right = bitwise_xor();
        return op;
    }
    return expr;
}

Ast_Node_Ptr Parser::bitwise_xor() {
    auto expr = bitwise_and();
    while(match_token({Token_Type::CARET})) {
        auto op = std::make_unique<Ast_Binary_Operation>();
        op->left = std::move(expr);
        op->operat = to_string(peek_token(-1).type);
        op->right = bitwise_and();
        return op;
    }
    return expr;
}

Ast_Node_Ptr Parser::bitwise_and() {
    auto expr = equality();
    while(match_token({Token_Type::AMPERSAND})) {
        auto op = std::make_unique<Ast_Binary_Operation>();
        op->left = std::move(expr);
        op->operat = to_string(peek_token(-1).type);
        op->right = equality();
        return op;
    }
    return expr;
}

Ast_Node_Ptr Parser::equality() {
    auto expr = comparison();
    while(match_token({Token_Type::BANG_EQUALS, Token_Type::EQUALS_EQUALS})) {
        auto op = std::make_unique<Ast_Binary_Operation>();
        op->left = std::move(expr);
        op->operat = to_string(peek_token(-1).type);
        op->right = comparison();
        return op;
    }
    return expr;
}

Ast_Node_Ptr Parser::comparison() {
    auto expr = bitwise_shift();
    while(match_token({Token_Type::GREATER, Token_Type::GREATER_EQUALS,
            Token_Type::LESS, Token_Type::LESS_EQUALS})) {
        auto op = std::make_unique<Ast_Binary_Operation>();
        op->left = std::move(expr);
        op->operat = to_string(peek_token(-1).type);
        op->right = bitwise_shift();
        return op;
    }
    return expr;
}

Ast_Node_Ptr Parser::bitwise_shift() {
    auto expr = term();
    while(match_token({Token_Type::BITWISE_SHIFT_LEFT,
            Token_Type::BITWISE_SHIFT_RIGHT})) {
        auto op = std::make_unique<Ast_Binary_Operation>();
        op->left = std::move(expr);
        op->operat = to_string(peek_token(-1).type);
        op->right = term();
        return op;
    }
    return expr;
}

Ast_Node_Ptr Parser::term() {
    auto expr = factor();
    while(match_token({Token_Type::ASTERISK, Token_Type::SLASH,
            Token_Type::MODULO})) {
        auto op = std::make_unique<Ast_Binary_Operation>();
        op->left = std::move(expr);
        op->operat = to_string(peek_token(-1).type);
        op->right = factor();
        return op;
    }
    return expr;
}

Ast_Node_Ptr Parser::factor() {
    auto expr = cast();
    while(match_token({Token_Type::PLUS, Token_Type::MINUS})) {
        auto op = std::make_unique<Ast_Binary_Operation>();
        op->left = std::move(expr);
        op->operat = to_string(peek_token(-1).type);
        op->right = cast();
        return op;
    }
    return expr;
}

Ast_Node_Ptr Parser::cast() {
    auto expr = prefix_unary();
    while(match_token({Token_Type::KW_AS})) {
        auto op = std::make_unique<Ast_Cast>();
        op->expr = std::move(expr);
        op->to = prefix_unary();
        return op;
    }
    return expr;
}

Ast_Node_Ptr Parser::prefix_unary() {
    if(match_token({Token_Type::BANG, Token_Type::MINUS})) {
        auto op = std::make_unique<Ast_Unary_Operation>();
        op->operat = to_string(peek_token(-1).type);
        op->left = prefix_unary();
        return op;
    }
    return postfix_unary();
}

Ast_Node_Ptr Parser::postfix_unary() {
    auto expr = scope_resolution();
    if(match_token({Token_Type::L_PAREN, Token_Type::L_BRACKET,
            /*Token_Type::L_BRACE,*/ Token_Type::DOT})) {
        if(peek_token(-1).type == Token_Type::L_PAREN) {
            return function_call(std::move(expr));
        }
        if(peek_token(-1).type == Token_Type::L_BRACKET) {
            auto aa = std::make_unique<Ast_Array_Access>();
            aa->array = std::move(expr);
            aa->at = expression();
            next_token(Token_Type::R_BRACKET);
            return aa;
        }
        //if(peek_token(-1).type == Token_Type::L_BRACE) {
            //auto constr = std::make_unique<Ast_Struct_Constructor>();
            //constr->struct_name = std::move(expr);
            //if(!match_token({Token_Type::R_BRACE})) {
                //do {
                    //constr->declaration_list.push_back(expression());
                //} while(match_token({Token_Type::COMMA}));
                //next_token(Token_Type::R_BRACE);
            //}
            //return constr;
        //}
        if(peek_token(-1).type == Token_Type::DOT) {
            auto ma = std::make_unique<Ast_Member_Access>();
            ma->left = std::move(expr);
            ma->right = postfix_unary();
            return ma;
        }
    }
    return expr;
}

Ast_Node_Ptr Parser::scope_resolution() {
    auto expr = primary();
    if(match_token({Token_Type::SCOPE_RESOLUTION})) {
        auto sr = std::make_unique<Ast_Scope_Resolution>();
        sr->left = std::move(expr);
        sr->right = postfix_unary();
        return sr;
    }
    return expr;
}

Ast_Node_Ptr Parser::primary() {
    auto token = peek_token(0);
    if(match_token({Token_Type::KW_TRUE})) {
        auto b = std::make_unique<Ast_Boolean>();
        b->value = true;
        return b;
    }
    if(match_token({Token_Type::KW_FALSE})) {
        auto b = std::make_unique<Ast_Boolean>();
        b->value = false;
        return b;
    }
    //if(match({Token_Type::kw_null})) {
    //    return std::make_unique<Ast_Null>(true);
    //}
    if(match_token({Token_Type::INTEGER})) {
        auto i = std::make_unique<Ast_Integer>();
        i->value = std::stoll(token.value);
        return i;
    }
    if(match_token({Token_Type::REAL_NUMBER})) {
        auto r = std::make_unique<Ast_Real_Number>();
        r->value = std::stod(token.value);
        return r;
    }
    if(match_token({Token_Type::STRING})) {
        auto s = std::make_unique<Ast_String>();
        s->value = token.value;
        return s;
    }
    if(match_token({Token_Type::L_PAREN})) {
        auto expr = std::make_unique<Ast_Grouping_Expression>();
        expr->expr = expression();
        next_token(Token_Type::R_PAREN);
        return expr;
    }
    if(match_token({Token_Type::IDENTIFIER})) {
        auto id = std::make_unique<Ast_Identifier>();
        id->name = token.value;
        return id;
    }
    report_error("Expected primary expression, got " + to_string(token.type));
    return nullptr;
}

Ast_Node_Ptr Parser::type() {
    return type_pointer();
}

Ast_Node_Ptr Parser::type_pointer() {
    if(match_token({Token_Type::CARET})) {
        auto op = std::make_unique<Ast_Pointer>();
        op->expr = type_pointer();
        return op;
    }
    return type_array();
}

Ast_Node_Ptr Parser::type_array() {
    if(match_token({Token_Type::L_BRACKET})) {
        auto array = std::make_unique<Ast_Array>();
        if(!match_token({Token_Type::R_BRACKET})) {
            array->size = expression();
            next_token(Token_Type::R_BRACKET);
        }
        array->expr = type_array();
        return array;
    }
    return type_scope_resolution();
}

Ast_Node_Ptr Parser::type_scope_resolution() {
    auto expr = type_identifier();
    if(match_token({Token_Type::SCOPE_RESOLUTION})) {
        auto sr = std::make_unique<Ast_Scope_Resolution>();
        sr->left = expression();
        sr->right = type_scope_resolution();
        return sr;
    }
    return expr;
}

Ast_Node_Ptr Parser::type_identifier() {
    next_token(Token_Type::IDENTIFIER);
    auto id = std::make_unique<Ast_Identifier>();
    id->name = peek_token(-1).value;
    return id;
}

bool Parser::match_token(const std::initializer_list<Token_Type>& types) {
    const auto token = peek_token(0);
    for(const auto& type : types) {
        if(token.type == type && _current < _tokens.size()) {
            ++_current;
            return true;
        }
    }
    return false;
}

bool Parser::check_token(Token_Type type) const {
    const auto token = peek_token(0);
    if(token.type == type && _current < _tokens.size()) {
        return true;
    }
    return false;
}

}

