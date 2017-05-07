#include "lexer.h"

#include <cctype>

#include <stdexcept>
#include <algorithm>

namespace wayward {

const std::array<std::string, 3> lexer::_keywords {
    "func", "var", "let"
};

lexer::lexer(const std::string& wayward_source)
        : _wayward_source{wayward_source} {
}

std::vector<token> lexer::tokenize() {
    _tokens.clear();
    while(_current_char < _wayward_source.length()) {
        char c = _wayward_source.at(_current_char);

        if(c == '\n') {
            ++_lines_count;
            //_columns_count = 1;
        }
        if(std::isspace(c)) {
            ++_current_char;
            continue;
        }

        if(std::ispunct(c)) {
            push_operator(c);
            continue;
        }

        if(std::isdigit(c)) {
            push_number(c);
            continue;
        }

        if(std::isalpha(c) || c == '_') {
            push_identifier(c);
            continue;
        }

        throw std::invalid_argument{"Unexpected character \"" + std::string{c}
                + "\" at line " + std::to_string(_lines_count) + "."};
    }
    _tokens.push_back({token_type::eof, "", _lines_count});
    return _tokens;
}

void lexer::push_operator(char c) {
    switch(c) {
        case '{':
            _tokens.push_back({token_type::brace, "{", _lines_count});
            ++_current_char;
            return;
        case '}':
            _tokens.push_back({token_type::brace, "}", _lines_count});
            ++_current_char;
            return;
        case '(':
            _tokens.push_back({token_type::parenthesis, "(", _lines_count});
            ++_current_char;
            return;
        case ')':
            _tokens.push_back({token_type::parenthesis, ")", _lines_count});
            ++_current_char;
            return;
    }
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
    _tokens.push_back({token_type::operat, operat, _lines_count});
}

void lexer::push_number(char c) {
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
        _tokens.push_back({token_type::real_number, number, _lines_count});
    } else {
        _tokens.push_back({token_type::integer, number, _lines_count});
    }
}

void lexer::push_identifier(char c) {
    std::string word;
    while(std::isalpha(c) || c == '_') {
        try {
            word += c;
            c = _wayward_source.at(++_current_char);
        } catch(const std::out_of_range& e) {
            break;
        }
    }
    if(std::find(_keywords.begin(), _keywords.end(), word) != _keywords.end()) {
        _tokens.push_back({token_type::keyword, word, _lines_count});
    } else {
        _tokens.push_back({token_type::identifier, word, _lines_count});
    }
}

std::string to_string(token_type token_type) {
    std::string result{};
    switch(token_type) {
        case token_type::identifier:
            result = "identifier";
        case token_type::keyword:
            result = "keyword";
        case token_type::integer:
            result = "integer";
        case token_type::real_number:
            result = "real_number";
        case token_type::operat:
            result = "operator";
        case token_type::parenthesis:
            result = "parenthesis";
        case token_type::brace:
            result = "brace";
        case token_type::eof:
            result = "eof";
    }
    return result;
}

}

