#ifndef TEZ_LEXER_H
#define TEZ_LEXER_H

#include <string>
#include <vector>
#include <map>

#include "syntax/token.h"

namespace tez {

using File_Tokens = std::vector<Token>;

class Lexer {
public:
    Lexer(const std::string& filename);
    
    // This constructor is only used for unit tests.
    // Second parameter is workaround for ambigious call (1st constructor
    // also takes only 1 std::string parameter).
    Lexer(const std::string& tez_source, bool doesnt_matter);

    // Create tokens from given source code.
    std::vector<Token> tokenize();

    bool errors_reported() const;

private:
    void report_error(const std::string& message);
    
    char peek_char(std::size_t depth = 1) const;
    char peek_string(const std::string&& to_peek) const;

    void push_token(Token_Type type, const std::string& value = "");
    void push_token(Token_Type type, const std::string& value,
            unsigned int col);

    void handle_whitespace();
    void handle_comment();

    void push_operator();
    void push_number(char c);
    void push_identifier(char c);
    void push_string();
    void push_character();
    
    const std::string  _working_path;
    const std::string  _filename;
    std::string        _tez_source;
    File_Tokens        _tokens;

    unsigned int _current_char = 0;
    unsigned int _lines_count = 1;
    unsigned int _columns_count = 0;
    
    bool _errors_reported = false;

    static const std::map<std::string, Token_Type> _keywords;
    static const std::map<std::string, Token_Type> _single_char_operators;
    static const std::map<std::string, Token_Type> _double_char_operators;
    static const std::map<std::string, Token_Type> _triple_char_operators;
};

}

#endif //TEZ_LEXER_H

