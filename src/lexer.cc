#include "lexer.h"

#include <cctype>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>

namespace tez {

const std::map<std::string, Token_Type> Lexer::KEYWORDS{
    {"func",   Token_Type::KW_FUNC},
    {"return", Token_Type::KW_RETURN},
    {"var",    Token_Type::KW_VAR},
    {"let",    Token_Type::KW_LET},
    {"struct", Token_Type::KW_STRUCT},
    {"class",  Token_Type::KW_CLASS},
    {"if",     Token_Type::KW_IF},
    {"else",   Token_Type::KW_ELSE},
    {"while",  Token_Type::KW_WHILE},
    {"do",     Token_Type::KW_DO},
    {"for",    Token_Type::KW_FOR},
    {"true",   Token_Type::KW_TRUE},
    {"false",  Token_Type::KW_FALSE},
    {"null",   Token_Type::KW_NULL},
    {"struct", Token_Type::KW_STRUCT},
    {"enum",   Token_Type::KW_ENUM},
};

const std::map<std::string, Token_Type> Lexer::OPERATORS{
    {"{",  Token_Type::L_BRACE},
    {"}",  Token_Type::R_BRACE},
    {"(",  Token_Type::L_PAREN},
    {")",  Token_Type::R_PAREN},
    {"+",  Token_Type::PLUS},
    {"-",  Token_Type::MINUS},
    {"*",  Token_Type::ASTERISK},
    {"/",  Token_Type::SLASH},
    {"&&", Token_Type::LOGICAL_AND},
    {"||", Token_Type::LOGICAL_OR},
    {"&",  Token_Type::AMPERSAND},
    {"|",  Token_Type::BITWISE_OR},
    {"^",  Token_Type::CARET},
    {"<<", Token_Type::BITWISE_SHIFT_LEFT},
    {">>", Token_Type::BITWISE_SHIFT_RIGHT},
    {"%",  Token_Type::MODULO},
    {"+=", Token_Type::PLUS_EQUALS},
    {"-=", Token_Type::MINUS_EQUALS},
    {"*=", Token_Type::MULTIPLY_EQUALS},
    {"/=", Token_Type::DIVIDE_EQUALS},
    {"%=", Token_Type::MODULO_EQUALS},
    {"!",  Token_Type::BANG},
    {"=",  Token_Type::EQUALS},
    {"!=", Token_Type::BANG_EQUALS},
    {"==", Token_Type::EQUALS_EQUALS},
    {">",  Token_Type::GREATER},
    {">=", Token_Type::GREATER_EQUALS},
    {"<",  Token_Type::LESS},
    {"<=", Token_Type::LESS_EQUALS},
    {";",  Token_Type::SEMICOLON},
    {":",  Token_Type::COLON},
    {",",  Token_Type::COMMA},
    {".",  Token_Type::DOT},
    {"->", Token_Type::ARROW},
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

Lexer::Lexer(const std::string& filename)
        : _filename{filename} {
    _wayward_source = read_file_content(filename);
}

Lexer::Lexer(const std::string& wayward_source,
        [[maybe_unused]] bool doesnt_matter) {
    _wayward_source = wayward_source;
}

bool Lexer::errors_reported() const {
    return _errors_reported;
}

void Lexer::report_error(const std::string& message) {
    _errors_reported = true;
    std::cerr << "<filename>:" << std::to_string(_lines_count) << ":"
            << std::to_string(_current_char - _columns_count + 1) << ": "
            << message << ".\n";
}

std::vector<Token> Lexer::tokenize() {
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
                push_token(Token_Type::STRING, string);
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
                push_token(Token_Type::CHARACTER, {character}, beginning);
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

char Lexer::peek_char(std::size_t depth) const {
    return _wayward_source.at(_current_char + depth);
}

void Lexer::push_token(Token_Type type, const std::string& value) {
    _tokens.push_back({type, value, _filename, _lines_count,
            _current_char - _columns_count + 1});
}

void Lexer::push_token(Token_Type type, const std::string& value,
        unsigned int col) {
    _tokens.push_back({type, value, _filename, _lines_count, col});
}

void Lexer::push_operator(char c) {
    switch(c) {
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case '=':
        case '!':
        case '<':
        case '>': {
            if(peek_char() == '=') {
                auto op = std::string{c} + std::string{peek_char()};
                auto o = OPERATORS.find(op);   // It's sure to find.
                push_token(o->second);
                _current_char += 2;
                break;
            }
            if(c == '-') {  // Arrow is special case.
                if(peek_char() == '>') {
                    push_token(Token_Type::ARROW);
                    _current_char += 2;
                    break;
                }
            }
            if(c == '<') {
                if(peek_char() == '<') {
                    push_token(Token_Type::BITWISE_SHIFT_LEFT);
                    _current_char += 2;
                    break;
                }
            }
            if(c == '>') {
                if(peek_char() == '>') {
                    push_token(Token_Type::BITWISE_SHIFT_RIGHT);
                    _current_char += 2;
                    break;
                }
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

void Lexer::push_number(char c) {
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
        push_token(Token_Type::REAL_NUMBER, number);
    } else {
        push_token(Token_Type::INTEGER, number);
    }
}

void Lexer::push_identifier(char c) {
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
        push_token(Token_Type::IDENTIFIER, word);
    }
}

}

