#include "lexer.h"

#include <cctype>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>

namespace wayward {

const std::map<std::string, token_type> lexer::KEYWORDS{
    {"func",   token_type::KW_FUNC},
    {"return", token_type::KW_RETURN},
    {"var",    token_type::KW_VAR},
    {"let",    token_type::KW_LET},
    {"struct", token_type::KW_STRUCT},
    {"class",  token_type::KW_CLASS},
    {"if",     token_type::KW_IF},
    {"else",   token_type::KW_ELSE},
    {"while",  token_type::KW_WHILE},
    {"do",     token_type::KW_DO},
    {"for",    token_type::KW_FOR},
    {"true",   token_type::KW_TRUE},
    {"false",  token_type::KW_FALSE},
    {"null",   token_type::KW_NULL}
};

const std::map<std::string, token_type> lexer::OPERATORS{
    {"{",  token_type::L_BRACE},
    {"}",  token_type::R_BRACE},
    {"(",  token_type::L_PAREN},
    {")",  token_type::R_PAREN},
    {"+",  token_type::PLUS},
    {"-",  token_type::MINUS},
    {"*",  token_type::MULTIPLY},
    {"/",  token_type::DIVIDE},
    {"%",  token_type::MODULO},
    {"+=", token_type::PLUS_EQUALS},
    {"-=", token_type::MINUS_EQUALS},
    {"*=", token_type::MULTIPLY_EQUALS},
    {"/=", token_type::DIVIDE_EQUALS},
    {"%=", token_type::MODULO_EQUALS},
    {"!",  token_type::BANG},
    {"=",  token_type::EQUALS},
    {"!=", token_type::BANG_EQUALS},
    {"==", token_type::EQUALS_EQUALS},
    {">",  token_type::GREATER},
    {">=", token_type::GREATER_EQUALS},
    {"<",  token_type::LESS},
    {"<=", token_type::LESS_EQUALS},
    {";",  token_type::SEMICOLON},
    {":",  token_type::COLON},
    {",",  token_type::COMMA},
    {".",  token_type::DOT},
    {"->", token_type::ARROW},
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
            << std::to_string(_current_char - _columns_count + 1) << ": "
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
                push_token(token_type::STRING, string);
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
                push_token(token_type::CHARACTER, {character}, beginning);
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
            _current_char - _columns_count + 1});
}

void lexer::push_token(token_type type, const std::string& value,
        unsigned int col) {
    _tokens.push_back({type, value, _filename, _lines_count, col});
}

void lexer::push_operator(char c) {
    switch(c) {
        case '+':
        case '-':
        case '*':
        case '/':
        case '%': {
            if(peek_char() == '=') {
                auto op = std::string{c} + std::string{peek_char()};
                auto o = OPERATORS.find(op);   // It's sure to find.
                push_token(o->second);
                _current_char += 2;
                break;
            }
            if(c == '-') {
                if(peek_char() == '>') {
                    push_token(token_type::ARROW);
                    _current_char += 2;
                }
                break;
            }
            auto o = OPERATORS.find(std::string{c});
            if(o != OPERATORS.end()) {
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
                auto o = OPERATORS.find(op);   // It's sure to find.
                push_token(o->second);
                _current_char += 2;
                break;
            }
            auto o = OPERATORS.find(std::string{c});
            if(o != OPERATORS.end()) {
                push_token(o->second);
            } else {
                report_error("Unexpected character '" + std::string{c} + "'");
            }
            ++_current_char;
            break;
        }
        default:
            auto o = OPERATORS.find(std::string{c});
            if(o != OPERATORS.end()) {
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
        push_token(token_type::REAL_NUMBER, number);
    } else {
        push_token(token_type::INTEGER, number);
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
    auto keyword = KEYWORDS.find(word);
    if(keyword != KEYWORDS.end()) {
        push_token(keyword->second, "");
    } else {
        push_token(token_type::IDENTIFIER, word);
    }
}

}

