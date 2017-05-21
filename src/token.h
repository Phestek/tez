#ifndef WAYWARD_TOKEN_H
#define WAYWARD_TOKEN_H

#include <string>

namespace wayward {

enum class token_type {
    identifier,

    integer,
    real_number,
    character,
    string,

    kw_func,
    kw_var,
    kw_let,
    kw_struct,
    kw_class,
    kw_if,
    kw_else,
    kw_while,
    kw_for,

    l_brace,
    r_brace,
    l_paren,
    r_paren,

    plus,
    minus,
    multiply,
    divide,
    modulo,
    plus_equals,
    minus_equals,
    multiply_equals,
    divide_equals,
    modulo_equals,

    bang,       // !
    equals,
    bang_equals,
    equals_equals,
    greater,
    greater_equals,
    less,
    less_equals,

    semicolon,
    colon,
    comma,
    dot,
    arrow,      // ->

    eof
};

struct token {
    token_type   type;
    std::string  value;
    std::string  filename;
    unsigned int line;
    unsigned int column;
};

std::string to_string(token_type token_type);

}

#endif //WAYWARD_TOKEN_H

