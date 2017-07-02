#ifndef TEZ_TOKEN_H
#define TEZ_TOKEN_H

#include <string>

namespace tez {

enum class Token_Type {
    IDENTIFIER,

    INTEGER,
    REAL_NUMBER,
    CHARACTER,
    STRING,

    KW_NAMESPACE,
    KW_USING,
    KW_AS,
    KW_FUNC,
    KW_RETURN,
    KW_LET,
    KW_VAR,
    KW_ENUM,
    KW_STRUCT,
    KW_UNION,
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
    KW_NEW,
    KW_FREE,
    KW_ASM,

    L_BRACE,
    R_BRACE,
    L_PAREN,
    R_PAREN,
    L_BRACKET,
    R_BRACKET,

    PLUS,
    MINUS,
    ASTERISK,
    SLASH,
    MODULO,
    BANG,
    EQUALS,
    BANG_EQUALS,
    EQUALS_EQUALS,
    GREATER,
    GREATER_EQUALS,
    LESS,
    LESS_EQUALS,
    BITWISE_OR,
    BITWISE_XOR,
    BITWISE_SHIFT_LEFT,
    BITWISE_SHIFT_RIGHT,
    LOGICAL_OR,
    LOGICAL_AND,
    PLUS_EQUALS,
    MINUS_EQUALS,
    MULTIPLY_EQUALS,
    DIVIDE_EQUALS,
    MODULO_EQUALS,
    AND_EQUALS,
    OR_EQUALS,
    XOR_EQUALS,
    LEFT_SHIFT_EQUALS,
    RIGHT_SHIFT_EQUALS,

    SCOPE_RESOLUTION,
    SEMICOLON,
    COLON,
    COMMA,
    DOT,
    AMPERSAND,
    CARET,
    ARROW,
};

struct Token {
    Token_Type   type;
    std::string  value;
    std::string  filename;
    unsigned int line;
    unsigned int column;
};

std::string to_string(Token_Type token_type);

}

#endif //TEZ_TOKEN_H

