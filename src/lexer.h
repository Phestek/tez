#ifndef WAYWARD_LEXER_H
#define WAYWARD_LEXER_H

#include <string>
#include <vector>

namespace wayward {

enum class token_type {
    identifier,
    
    integer,
    real_number,
    unary_operator,
    binary_operator,

    parenthesis,
    brace
};

struct token {
    token_type  type;
    std::string value;
};

class lexer {
public:
    // Default constructor.
    lexer(const std::string& wayward_source);
    
    // Create tokens from given source code.
    std::vector<token> tokenize();

private:
    const std::string& _wayward_source;

    unsigned int _current_char = 0;
    unsigned int _lines_count = 1;
    unsigned int _columns_count = 1;
};

}

#endif //WAYWARD_LEXER_H

