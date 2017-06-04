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
    /** Just get next token. */
    token next_token();
    /** Validates token type and returns it. Error is reported if check fails. */
    token next_token(token_type type);
    /** Peek token; depth = 0 returns current token. */
    token peek_token(size_t depth = 1) const;
    /** Check next token type. If it matches any of given types, return it. */
    bool match_token(const std::initializer_list<token_type>& types);
    /** Check token type; returns false if eof. */
    bool check_token(token_type type) const;

    void report_error(const std::string& message);
    
    ast_node_ptr statement();

    // Declarations.
    ast_node_ptr function_declaration();
    ast_func_param function_param();
    ast_node_ptr function_call(const std::string& name);
    ast_node_ptr variable_declaration(bool constant);

    ast_node_ptr if_statement();
    ast_node_ptr while_statement();
    ast_node_ptr do_while_statement();
    ast_node_ptr for_statement();

    // Recursive descent parsing (for rvalues).
    ast_node_ptr expression();      // Base.
    ast_node_ptr assignment();      // = += -= *= /= %=
    ast_node_ptr equality();        // == !=
    ast_node_ptr comparison();      // >= <= > <
    ast_node_ptr term();            // * / %
    ast_node_ptr factor();          // + -
    ast_node_ptr unary();           // ! -  & ^
    ast_node_ptr primary();         // int, double, bool, identifier

    const std::vector<token> _tokens;
    unsigned int             _current = 0;

    bool _errors_reported = false;
};

}

#endif //WAYWARD_PARSER_H

