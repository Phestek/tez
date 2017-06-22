#include "semantic_analyser.h"

namespace tez {

void Semantic_Analyser::analyse(Ast& ast) {
    collect_top_level_declarations(ast);
}

bool Semantic_Analyser::errors_reported() {
    return _errors_reported;
}

void Semantic_Analyser::collect_top_level_declarations(const Ast& ast) {
    for(const auto& stmt : ast) {
        switch(stmt->node_type) {
            case Ast_Node_Type::FUNCTION_DECLARATION: {
                const auto& func = dynamic_cast<Ast_Func_Decl&>(*stmt);
                _symbol_table.push_declaration(
                        Declaration{func.name, Declaration::Type::FUNCTION});
                break;
            }
            case Ast_Node_Type::VARIABLE_DECLARATION: {
                const auto& var = dynamic_cast<Ast_Var_Decl&>(*stmt);
                _symbol_table.push_declaration(
                        Declaration{var.name, Declaration::Type::VARIABLE});
                break;
            }
            case Ast_Node_Type::STRUCT: {
                const auto& var = dynamic_cast<Ast_Struct&>(*stmt);
                _symbol_table.push_declaration(
                        Declaration{var.name, Declaration::Type::STRUCT});
                break;
            }
            default:
                break;
        }
    }
}

}

