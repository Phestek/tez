#include "token.h"

namespace tez {

std::string to_string(Token_Type token_type) {
    std::string result{};
    switch(token_type) {
        case Token_Type::IDENTIFIER:          result = "identifier";  break;
        case Token_Type::INTEGER:             result = "integer";     break;
        case Token_Type::REAL_NUMBER:         result = "real_number"; break;
        case Token_Type::L_PAREN:             result = "(";           break;
        case Token_Type::R_PAREN:             result = ")";           break;
        case Token_Type::L_BRACE:             result = "{";           break;
        case Token_Type::R_BRACE:             result = "}";           break;
        case Token_Type::BANG_EQUALS:         result = "!=";          break;
        case Token_Type::EQUALS_EQUALS:       result = "==";          break;
        case Token_Type::GREATER:             result = ">";           break;
        case Token_Type::GREATER_EQUALS:      result = ">=";          break;
        case Token_Type::LESS:                result = "<";           break;
        case Token_Type::LESS_EQUALS:         result = "<=";          break;
        case Token_Type::BITWISE_SHIFT_LEFT:  result = "<<";          break;
        case Token_Type::BITWISE_SHIFT_RIGHT: result = ">>";          break;
        case Token_Type::LOGICAL_OR:          result = "||";          break;
        case Token_Type::LOGICAL_AND:         result = "&&";          break;
        case Token_Type::BITWISE_OR:          result = "|";           break;
        case Token_Type::AMPERSAND:           result = "&";           break;
        case Token_Type::CARET:               result = "^";           break;
        case Token_Type::PLUS:                result = "+";           break;
        case Token_Type::MINUS:               result = "-";           break;
        case Token_Type::ASTERISK:            result = "*";           break;
        case Token_Type::SLASH:               result = "/";           break;
        case Token_Type::MODULO:              result = "%";           break;
        case Token_Type::EQUALS:              result = "=";           break;
        case Token_Type::PLUS_EQUALS:         result = "+=";          break;
        case Token_Type::MINUS_EQUALS:        result = "-=";          break;
        case Token_Type::MULTIPLY_EQUALS:     result = "*=";          break;
        case Token_Type::DIVIDE_EQUALS:       result = "/=";          break;
        case Token_Type::MODULO_EQUALS:       result = "%=";          break;
        case Token_Type::SEMICOLON:           result = ";";           break;
        case Token_Type::COLON:               result = ":";           break;
        case Token_Type::ARROW:               result = "->";          break;
        case Token_Type::DOT:                 result = ".";           break;
        case Token_Type::COMMA:               result = ",";           break;
        case Token_Type::KW_FUNC:             result = "func";        break;
        case Token_Type::KW_RETURN:           result = "return";      break;
        case Token_Type::KW_VAR:              result = "var";         break;
        case Token_Type::KW_IF:               result = "if";          break;
        case Token_Type::KW_STRUCT:           result = "if";          break;
        case Token_Type::KW_ENUM:             result = "if";          break;
        default:                              result = "not implemented yet";
    }
    return result;
}

}

