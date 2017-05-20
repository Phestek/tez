#include "token.h"

namespace wayward {

std::string to_string(token_type token_type) {
    std::string result{};
    switch(token_type) {
        case token_type::identifier:
            result = "identifier";
            break;
        case token_type::integer:
            result = "integer";
            break;
        case token_type::real_number:
            result = "real_number";
            break;
        case token_type::l_paren:
            result = "'('";
            break;
        case token_type::r_paren:
            result = "')'";
            break;
        case token_type::l_brace:
            result = "'{'";
            break;
        case token_type::r_brace:
            result = "'}'";
            break;
        case token_type::eof:
            result = "eof";
            break;
        default:
            result = "not implemented yet";
    }
    return result;
}

}

