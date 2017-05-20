#ifndef WAYWARD_LEXER_H
#define WAYWARD_LEXER_H

#include <string>
#include <vector>
#include <map>

#include "token.h"

namespace wayward {

class lexer {
public:
    lexer(const std::string working_path, const std::string& filename);
    
    // This constructor is only used for unit tests.
    lexer(const std::string wayward_source);

    // Create tokens from given source code.
    std::vector<token> tokenize();

    bool errors_reported() const;

private:
    void report_error(const std::string& message);
    
    void push_token(token_type type, const std::string& value);

    void push_operator(char c);
    void push_number(char c);
    void push_identifier(char c);
    
    const std::string  _working_path;
    const std::string  _filename;
    std::string        _wayward_source;
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

