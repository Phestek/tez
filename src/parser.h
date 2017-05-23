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
    token peek_token(size_t depth = 1) const; // depth = 0 returns current token.

    // Recursive descent parsing.
    ast_node_ptr expression();
    ast_node_ptr equality();
    ast_node_ptr comparison();
    ast_node_ptr term();
    ast_node_ptr factor();
    ast_node_ptr unary();
    ast_node_ptr primary();
    
    // Helper functions.
    bool match_token(const std::initializer_list<token_type>& types);

    const std::vector<token> _tokens;
    unsigned int             _current;

    bool _errors_reported = false;
};

}

#endif //WAYWARD_PARSER_H

