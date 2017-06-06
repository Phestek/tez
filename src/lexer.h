#ifndef WAYWARD_LEXER_H
#define WAYWARD_LEXER_H

#include <string>
#include <vector>
#include <map>

#include "token.h"

namespace wayward {

class lexer {
public:
    lexer(const std::string& filename);
    
    // This constructor is only used for unit tests.
    // Second parameter is workaround for ambigious call (1st constructor
    // also takes only 1 std::string parameter).
    lexer(const std::string& wayward_source, bool doesnt_matter);

    // Create tokens from given source code.
    std::vector<token> tokenize();

    bool errors_reported() const;

private:
    void report_error(const std::string& message);
    
    char peek_char(std::size_t depth = 1) const;

    void push_token(token_type type, const std::string& value = "");
    void push_token(token_type type, const std::string& value,
            unsigned int col);

    void push_operator(char c);
    void push_number(char c);
    void push_identifier(char c);
    
    const std::string  _working_path;
    const std::string  _filename;
    std::string        _wayward_source;
    std::vector<token> _tokens;

    unsigned int _current_char = 0;
    unsigned int _lines_count = 1;
    unsigned int _columns_count = 0;
    
    bool _errors_reported = false;

    static const std::map<std::string, token_type> KEYWORDS;
    static const std::map<std::string, token_type> OPERATORS;
};

std::string to_string(token_type token_type);

}

#endif //WAYWARD_LEXER_H

