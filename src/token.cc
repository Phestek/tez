#include "token.h"

namespace wayward {

std::string to_string(token_type token_type) {
    std::string result{};
    switch(token_type) {
        case token_type::identifier:     result = "identifier";  break;
        case token_type::integer:        result = "integer";     break;
        case token_type::real_number:    result = "real_number"; break;
        case token_type::l_paren:        result = "(";           break;
        case token_type::r_paren:        result = ")";           break;
        case token_type::l_brace:        result = "{";           break;
        case token_type::r_brace:        result = "}";           break;
        case token_type::bang_equals:    result = "!=";          break;
        case token_type::equals_equals:  result = "==";          break;
        case token_type::greater:        result = ">";           break;
        case token_type::greater_equals: result = ">=";          break;
        case token_type::less:           result = "<";           break;
        case token_type::less_equals:    result = "<=";          break;
        case token_type::plus:           result = "+";           break;
        case token_type::minus:          result = "-";           break;
        case token_type::multiply:       result = "*";           break;
        case token_type::divide:         result = "/";           break;
        case token_type::eof:            result = "eof";         break;
        default:
            result = "not implemented yet";
    }
    return result;
}

}

