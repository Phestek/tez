#ifndef WAYWARD_PARSER_H
#define WAYWARD_PARSER_H

#include <string>
#include <map>

#include "ast.h"
#include "lexer.h"

namespace wayward {

class parser {
public:
    parser(const std::vector<token>& tokens);

    ast_program parse();

    bool errors_reported() const;

private:
    void report_error(const std::string message);

    token next_token();
    token next_token(token_type type);
    token current_token() const;

    token peek_token(unsigned int depth = 1);

    ast_node_ptr parse_expression(const token& token);
    std::unique_ptr<ast_function_declaration> parse_function();
    ast_function_parameter parse_function_parameter(token token);
    std::unique_ptr<ast_variable_declaration> parse_variable(bool constant);
    ast_node_ptr parse_rvalue();

    const std::vector<token>& _tokens;  // Vector of tokens to parse.
    unsigned int _current_token = 0;    // Current token counter.

    static const std::map<token_type, int> _operator_persistence;

    bool _errors_reported = false;
};

}

#endif //WAYWARD_PARSER_H

