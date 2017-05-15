#ifndef WAYWARD_LEXER_H
#define WAYWARD_LEXER_H

#include <string>
#include <vector>
#include <map>

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
    unsigned int line;
};

class lexer {
public:
    // Default constructor.
    lexer(const std::string& wayward_source);

    // Create tokens from given source code.
    std::vector<token> tokenize();

    bool errors_reported() const;

private:
    void report_error(unsigned int line, unsigned int column,
            const std::string& message);

    void push_operator(char c);
    void push_number(char c);
    void push_identifier(char c);

    const std::string& _wayward_source;
    std::vector<token> _tokens;

    unsigned int _current_char = 0;
    unsigned int _lines_count = 1;
    unsigned int _columns_count = 1;
    
    bool _errors_reported = false;

    static const std::map<std::string, token_type> _keywords;
    static const std::map<std::string, token_type> _operators;
};

std::string to_string(token_type token_type);

}

#endif //WAYWARD_LEXER_H

