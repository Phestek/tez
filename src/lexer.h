#ifndef WAYWARD_LEXER_H
#define WAYWARD_LEXER_H

#include <string>
#include <vector>

namespace wayward {

enum class token_type {
    identifier,
    
    integer,
    real_number,
    operat,         // operator is reserved keyword NotLikeThis

    parenthesis,
    brace,

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

private:
    void push_operator(char c);
    void push_number(char c);
    void push_identifier(char c);

    const std::string& _wayward_source;
    std::vector<token> _tokens;

    unsigned int _current_char = 0;
    unsigned int _lines_count = 1;
    // TODO: unsigned int _columns_count = 1;
};

std::string to_string(token_type token_type);

}

#endif //WAYWARD_LEXER_H

