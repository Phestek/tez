#include "token.h"

namespace wayward {

std::string to_string(token_type token_type) {
    std::string result{};
    switch(token_type) {
        case token_type::IDENTIFIER:          result = "identifier";  break;
        case token_type::INTEGER:             result = "integer";     break;
        case token_type::REAL_NUMBER:         result = "real_number"; break;
        case token_type::L_PAREN:             result = "(";           break;
        case token_type::R_PAREN:             result = ")";           break;
        case token_type::L_BRACE:             result = "{";           break;
        case token_type::R_BRACE:             result = "}";           break;
        case token_type::BANG_EQUALS:         result = "!=";          break;
        case token_type::EQUALS_EQUALS:       result = "==";          break;
        case token_type::GREATER:             result = ">";           break;
        case token_type::GREATER_EQUALS:      result = ">=";          break;
        case token_type::LESS:                result = "<";           break;
        case token_type::LESS_EQUALS:         result = "<=";          break;
        case token_type::BITWISE_SHIFT_LEFT:  result = "<<";          break;
        case token_type::BITWISE_SHIFT_RIGHT: result = ">>";          break;
        case token_type::LOGICAL_OR:          result = "||";          break;
        case token_type::LOGICAL_AND:         result = "&&";          break;
        case token_type::BITWISE_OR:          result = "|";           break;
        case token_type::AMPERSAND:           result = "&";           break;
        case token_type::CARET:               result = "^";           break;
        case token_type::PLUS:                result = "+";           break;
        case token_type::MINUS:               result = "-";           break;
        case token_type::ASTERISK:            result = "*";           break;
        case token_type::SLASH:               result = "/";           break;
        case token_type::MODULO:              result = "%";           break;
        case token_type::EQUALS:              result = "=";           break;
        case token_type::PLUS_EQUALS:         result = "+=";          break;
        case token_type::MINUS_EQUALS:        result = "-=";          break;
        case token_type::MULTIPLY_EQUALS:     result = "*=";          break;
        case token_type::DIVIDE_EQUALS:       result = "/=";          break;
        case token_type::MODULO_EQUALS:       result = "%=";          break;
        case token_type::SEMICOLON:           result = ";";           break;
        case token_type::COLON:               result = ":";           break;
        case token_type::ARROW:               result = "->";          break;
        case token_type::DOT:                 result = ".";           break;
        case token_type::COMMA:               result = ",";           break;
        case token_type::KW_FUNC:             result = "func";        break;
        case token_type::KW_RETURN:           result = "return";      break;
        case token_type::KW_VAR:              result = "var";         break;
        case token_type::KW_IF:               result = "if";          break;
        case token_type::KW_STRUCT:           result = "if";          break;
        case token_type::KW_ENUM:             result = "if";          break;
        default:                              result = "not implemented yet";
    }
    return result;
}

}

