#include "lexer.h"

#include <cctype>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>

namespace wayward {

const std::map<std::string, token_type> lexer::_keywords{
    {"func",   token_type::kw_func},
    {"return", token_type::kw_return},
    {"var",    token_type::kw_var},
    {"let",    token_type::kw_let},
    {"struct", token_type::kw_struct},
    {"class",  token_type::kw_class},
    {"if",     token_type::kw_if},
    {"else",   token_type::kw_else},
    {"while",  token_type::kw_while},
    {"for",    token_type::kw_for},
    {"true",   token_type::kw_true},
    {"false",  token_type::kw_false},
    {"null",   token_type::kw_null}
};

const std::map<std::string, token_type> lexer::_operators{
    {"+",  token_type::plus},
    {"-",  token_type::minus},
    {"*",  token_type::multiply},
    {"/",  token_type::divide},
    {"%",  token_type::modulo},
    {"+=", token_type::plus_equals},
    {"-=", token_type::minus_equals},
    {"*=", token_type::multiply_equals},
    {"/=", token_type::divide_equals},
    {"%=", token_type::modulo_equals},
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

namespace {

// Get entire file content, including newlines.
// NOTE: leaves empty line on the end.
std::string read_file_content(const std::string& filename) {
    std::ifstream file{filename};
    if(!file.good()) {
        // TODO: Handle it.
        std::cerr << "Failed to open file \"" + filename + "\".\n";
        throw;
    }
    std::string content;
    while(!file.eof()) {
        std::string line;
        std::getline(file, line);
        if(line != "\n") {
            content += line + '\n';
        }
    }
    return content;
}

}

lexer::lexer(const std::string& filename)
        : _filename{filename} {
    _wayward_source = read_file_content(filename);
}

lexer::lexer(const std::string& wayward_source, bool doesnt_matter) {
    doesnt_matter = false; // Silence this stupid warning.
    _wayward_source = wayward_source;
}

bool lexer::errors_reported() const {
    return _errors_reported;
}

void lexer::report_error(const std::string& message) {
    _errors_reported = true;
    std::cerr << "<filename>:" << std::to_string(_lines_count) << ":"
            << std::to_string(_current_char - _columns_count) << ": "
            << message << ".\n";
}

std::vector<token> lexer::tokenize() {
    _tokens.clear();
    // TODO: Make it cleaner, this is Jonathan Blow style function.
    try {
        while(_current_char < _wayward_source.length()) {
            char c = _wayward_source.at(_current_char);

            if(c == '/' && _wayward_source.at(_current_char + 1) == '/') {
                while(c != '\n') {
                    c = _wayward_source.at(++_current_char);
                }
                continue;
            }
            if(c == '/' && _wayward_source.at(_current_char + 1) == '*') {
                ++_current_char;
                while(c != '*' && _wayward_source.at(_current_char + 1) != '/') {
                    if(_current_char >= _wayward_source.length()) {
                        report_error("Missing closing \"*/\".");
                    }
                    if(_wayward_source.at(_current_char) == '\n') {
                        ++_lines_count;
                    }
                    c = _wayward_source.at(++_current_char);
                }
                ++_current_char;
                ++_current_char;
                continue;
            }
            if(c == '\n') {
                ++_lines_count;
                _columns_count = _current_char;
            }
            if(std::isspace(c)) {
                ++_current_char;
                continue;
            }
            if(c == '"') { // String.
                std::string string;
                c = _wayward_source.at(++_current_char);
                while(c != '"') {
                    string += c;
                    c = _wayward_source.at(++_current_char);
                }
                push_token(token_type::string, string);
                ++_current_char;
                continue;
            }
            if(c == '\'') {
                std::size_t beginning = _current_char - _columns_count;
                std::string character = {_wayward_source.at(++_current_char)};
                if(character == "\\") {
                    character += _wayward_source.at(++_current_char);
                }
                if(c = _wayward_source.at(++_current_char); c != '\'') {
                    report_error("Expected ''', got '" + std::string{c} + "'");
                    while(c != '\'') {
                        c = ++_current_char;
                    }
                }
                ++_current_char;
                push_token(token_type::character, {character}, beginning);
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
            report_error("Unexpected character '" + std::string{c} + "'");
        }
    } catch(const std::out_of_range& e) {
        report_error("Unexpected end of file.");
    }
    return _tokens;
}

char lexer::peek_char(std::size_t depth) const {
    return _wayward_source.at(_current_char + depth);
}

void lexer::push_token(token_type type, const std::string& value) {
    _tokens.push_back({type, value, _filename, _lines_count,
            _current_char - _columns_count});
}

void lexer::push_token(token_type type, const std::string& value,
        unsigned int col) {
    _tokens.push_back({type, value, _filename, _lines_count, col});
}

void lexer::push_operator(char c) {
    switch(c) {
        case '{':
            push_token(token_type::l_brace, "");
            ++_current_char;
            break;
        case '}':
            push_token(token_type::r_brace, "");
            ++_current_char;
            break;
        case '(':
            push_token(token_type::l_paren, "");
            ++_current_char;
            break;
        case ')':
            push_token(token_type::r_paren, "");
            ++_current_char;
            break;
        case '+':
        case '-':
        case '*':
        case '/':
        case '%': {
            if(peek_char() == '=') {
                auto op = std::string{c} + std::string{peek_char()};
                auto o = _operators.find(op);   // It's sure to find.
                push_token(o->second);
                _current_char += 2;
                break;
            }
            if(c == '-') {
                if(peek_char() == '>') {
                    push_token(token_type::arrow);
                    _current_char += 2;
                }
                break;
            }
            auto o = _operators.find(std::string{c});
            if(o != _operators.end()) {
                push_token(o->second, "");
            } else {
                report_error("Unexpected character '" + std::string{c} + "'");
            }
            ++_current_char;
            break;
        }
        case '=':
        case '!':
        case '<':
        case '>': {
            if(peek_char() == '=') {
                std::string op = std::string{c} + std::string{peek_char()};
                auto o = _operators.find(op);   // It's sure to find.
                push_token(o->second);
                _current_char += 2;
            }
            auto o = _operators.find(std::string{c});
            if(o != _operators.end()) {
                push_token(o->second);
            } else {
                report_error("Unexpected character '" + std::string{c} + "'");
            }
            ++_current_char;
            break;
        }
        default:
            auto o = _operators.find(std::string{c});
            if(o != _operators.end()) {
                push_token(o->second);
            } else {
                report_error("Unexpected character '" + std::string{c} + "'");
            }
            ++_current_char;
    }
}

void lexer::push_number(char c) {
    std::string number;
    bool is_real = false;   // False means integer value.
    while(std::isdigit(c) || c == '.') {
        if(c == '.') {
            if(is_real) {   // Error if dot was used before.
        report_error("Too many decimal points in number.");
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
        push_token(token_type::real_number, number);
    } else {
        push_token(token_type::integer, number);
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
        push_token(keyword->second, "");
    } else {
        push_token(token_type::identifier, word);
    }
}

}

