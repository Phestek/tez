#ifndef WAYWARD_PARSER_H
#define WAYWARD_PARSER_H

#include <vector>

#include "ast.h"
#include "token.h"

namespace wayward {

class parser {
public:
    parser(const std::vector<token>& tokens);

    std::vector<ast_node_ptr> parse();

    bool errors_reported() const;

private:
    // Tokens iteration helpers.
    token next_token();
    token next_token(token_type type);
    token peek_token(size_t depth = 1) const; // depth = 0 returns current token.

    // Report error for current token.
    void report_error(const std::string& message);
    
    ast_node_ptr statement();

    // Declarations.
    ast_node_ptr function_declaration();
    ast_func_param function_param();
    ast_node_ptr variable_declaration(bool constant);

    // Recursive descent parsing (for rvalues).
    ast_node_ptr expression();      // Base.
    ast_node_ptr equality();        // == !=
    ast_node_ptr comparison();      // >= <= > <
    ast_node_ptr term();            // * / %
    ast_node_ptr factor();          // + -
    ast_node_ptr unary();           // ! - TODO: & ^ (for pointers)
    ast_node_ptr primary();         // int, double, bool, identifier
    
    // Helper functions.
    bool match_token(const std::initializer_list<token_type>& types);
    bool check_token(token_type type) const;    // Validate current token type.

    const std::vector<token> _tokens;
    unsigned int             _current;

    bool _errors_reported = false;
};

}

#endif //WAYWARD_PARSER_H

