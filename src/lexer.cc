#include "lexer.h"

#include <cctype>

#include <stdexcept>
#include <algorithm>

namespace wayward {

const std::map<std::string, token_type> lexer::_keywords{
    {"func",   token_type::kw_func},
    {"var",    token_type::kw_var},
    {"let",    token_type::kw_let},
    {"struct", token_type::kw_struct},
    {"class",  token_type::kw_class},
};

const std::map<std::string, token_type> lexer::_operators{
    {"+",  token_type::plus},
    {"-",  token_type::minus},
    {"*",  token_type::multiply},
    {"/",  token_type::divide},
    {"+=", token_type::plus_equals},
    {"-=", token_type::minus_equals},
    {"*=", token_type::multiply_equals},
    {"/=", token_type::divide_equals},
    {"!",  token_type::bang},
    {"=",  token_type::equals},
    {"!=", token_type::bang_equals},
    {"==", token_type::equals_equals},
    {">",  token_type::greater},
    {">=", token_type::greater_equals},
    {"<",  token_type::less},
    {"<=", token_type::less_equals},
    {";",  token_type::semicolon},
    {":",  token_type::colon},
    {",",  token_type::comma},
    {".",  token_type::dot},
    {"->", token_type::arrow},
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
    
        if(c == '\"') {
            // TODO: Implement this.
            continue;
        }
        
        if(c == '\'') {
            // TODO: Implement this.
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
            _tokens.push_back({token_type::l_brace, "", _lines_count});
            ++_current_char;
            return;
        case '}':
            _tokens.push_back({token_type::r_brace, "", _lines_count});
            ++_current_char;
            return;
        case '(':
            _tokens.push_back({token_type::l_paren, "", _lines_count});
            ++_current_char;
            return;
        case ')':
            _tokens.push_back({token_type::r_paren, "", _lines_count});
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
    auto o = _operators.find(operat);
    if(o != _operators.end()) {
        _tokens.push_back({o->second, "", _lines_count});
    } else {
        throw std::invalid_argument{"Unknown operator \"" + operat
                + "\" at line " + std::to_string(_lines_count) + "."};
    }
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
    auto keyword = _keywords.find(word);
    if(keyword != _keywords.end()) {
        _tokens.push_back({keyword->second, "", _lines_count});
    } else {
        _tokens.push_back({token_type::identifier, word, _lines_count});
    }
}

std::string to_string(token_type token_type) {
    std::string result{};
    switch(token_type) {
        case token_type::identifier:
            result = "identifier";
            break;
        case token_type::integer:
            result = "integer";
            break;
        case token_type::real_number:
            result = "real_number";
            break;
        case token_type::l_paren:
            result = "l_paren";
            break;
        case token_type::r_paren:
            result = "r_paren";
            break;
        case token_type::l_brace:
            result = "l_brace";
            break;
        case token_type::r_brace:
            result = "r_brace";
            break;
        case token_type::eof:
            result = "eof";
            break;
        default:
            result = "not implemented yet";
    }
    return result;
}

}

