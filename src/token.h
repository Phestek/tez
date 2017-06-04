#ifndef WAYWARD_TOKEN_H
#define WAYWARD_TOKEN_H

#include <string>

namespace wayward {

enum class token_type {
    IDENTIFIER,

    INTEGER,
    REAL_NUMBER,
    CHARACTER,
    STRING,

    KW_FUNC,
    KW_RETURN,
    KW_VAR,
    KW_LET,
    KW_STRUCT,
    KW_CLASS,
    KW_IF,
    KW_ELSE,
    KW_WHILE,
    KW_DO,
    KW_FOR,
    KW_TRUE,
    KW_FALSE,
    KW_NULL,
    KW_BREAK,
    KW_CONTINUE,

    L_BRACE,
    R_BRACE,
    L_PAREN,
    R_PAREN,

    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    MODULO,
    PLUS_EQUALS,
    MINUS_EQUALS,
    MULTIPLY_EQUALS,
    DIVIDE_EQUALS,
    MODULO_EQUALS,

    BANG,               // !
    EQUALS,
    BANG_EQUALS,
    EQUALS_EQUALS,
    GREATER,
    GREATER_EQUALS,
    LESS,
    LESS_EQUALS,

    SEMICOLON,
    COLON,
    COMMA,
    DOT,
    CIRCUMFLEX,
    ARROW,              // ->

    END_OF_FILE         // Stupid macro, can't use just "EOF".
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

