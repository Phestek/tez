#ifndef TEZ_SEMANTIC_ANALYSER_H
#define TEZ_SEMANTIC_ANALYSER_H

#include "semantic/symbol_table.h"
#include "syntax/ast.h"

namespace tez {

class Semantic_Analyzer {
public:
    void analyse(Ast& ast);

    bool errors_reported();

private:
    void report_error(const std::string&& msg);

    /** Collects all declarations from global scope. */
    void collect_top_level_declarations(const Ast& ast);

    void go_deeper(std::vector<Ast_Node_Ptr>& block);

    /** Check if type name exists in current context. Works for pointers and arrays. */
    bool check_type(Ast_Node_Ptr& type) const;

    /** Extract type identifier from pointer or array. */
    std::string get_type_name(const Ast_Node_Ptr& type) const;

    bool validate_statement(Ast_Node_Ptr& stmt) const;

    bool _errors_reported = false;

    Symbol_Table _symbol_table;
};

}

#endif //TEZ_SEMANTIC_ANALYSER_H

