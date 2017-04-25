#include "lexer.h"

#include <cctype>
#include <stdexcept>

namespace wayward {

lexer::lexer(const std::string& wayward_source)
        : _wayward_source{wayward_source} {
}

std::vector<token> lexer::tokenize() {
    std::vector<token> tokens;
    
    while(_current_char < _wayward_source.length()) {
        char c = _wayward_source.at(_current_char);
        
        if(c == '\n') {
            ++_lines_count;
        }
        if(std::isspace(c)) {
            ++_current_char;
            continue;
        }

        switch(c) {
            case '{':
                tokens.push_back({token_type::brace, "{", _lines_count});
                ++_current_char;
                continue;
            case '}':
                tokens.push_back({token_type::brace, "}", _lines_count});
                ++_current_char;
                continue;
            case '(':
                tokens.push_back({token_type::parenthesis, "(", _lines_count});
                ++_current_char;
                continue;
            case ')':
                tokens.push_back({token_type::parenthesis, ")", _lines_count});
                ++_current_char;
                continue;
        }
        
        if(std::ispunct(c)) {
            std::string operat;
            while(std::ispunct(c)) {
                try {
                    operat += c;
                    c = _wayward_source.at(++_current_char);
                } catch(const std::out_of_range& e) {
                    break;
                }
            }
            // TODO: Check if is correct operator.
            tokens.push_back({token_type::operat, operat, _lines_count});
            continue;
        }

        if(std::isdigit(c)) {
            std::string number;
            bool is_real = false;   // False means integer value.
            while(std::isdigit(c) || c == '.') {
                if(c == '.') {
                    if(is_real) {   // Error if dot was used before.
                        throw;
                    }
                    is_real = true;
                }
                try {
                    number += c;
                    c = _wayward_source.at(++_current_char);
                } catch(const std::out_of_range& e) {
                    break;
                }
            }
            if(is_real) {
                tokens.push_back({token_type::real_number, number, _lines_count});
            } else {
                tokens.push_back({token_type::integer, number, _lines_count});
            }
            continue;
        }

        if(std::isalpha(c) || c == '_') {
            std::string word;
            while(std::isalpha(c) || c == '_') {
                try {
                    word += c;
                    c = _wayward_source.at(++_current_char);
                } catch(const std::out_of_range& e) {
                    break;
                }
            }
            tokens.push_back({token_type::identifier, word, _lines_count});
            continue;
        }

        throw std::invalid_argument{"Unexpected character \""
                + std::string{c} + "\" at line " + std::to_string(_lines_count)
                    + "."};
    }

    return tokens;
}

}

