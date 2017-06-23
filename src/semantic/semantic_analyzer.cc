#include "semantic_analyzer.h"

#include <iostream>

namespace tez {

void Semantic_Analyzer::analyse(Ast& ast) {
    collect_top_level_declarations(ast);
    for(const auto& stmt : ast) {
        switch(stmt->node_type) {
            case Ast_Node_Type::FUNCTION_DECLARATION:
            case Ast_Node_Type::VARIABLE_DECLARATION:
            case Ast_Node_Type::STRUCT:
            case Ast_Node_Type::ENUM:
                // TODO: Validate them.
                break;
            default: {
                report_error("Statement invalid in current context");
            }
        }
    }
}

bool Semantic_Analyzer::errors_reported() {
    return _errors_reported;
}

void Semantic_Analyzer::report_error(const std::string&& msg) {
    _errors_reported = true;
    std::cerr << "Error: " << msg << ".\n";
}

void Semantic_Analyzer::collect_top_level_declarations(const Ast& ast) {
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
            case Ast_Node_Type::ENUM: {
                const auto& var = dynamic_cast<Ast_Enum&>(*stmt);
                _symbol_table.push_declaration(
                        Declaration{var.name, Declaration::Type::ENUM});
                break;
            }
            default:
                break;
        }
    }
}

void Semantic_Analyzer::go_deeper(std::vector<Ast_Node_Ptr>& block) {
    for(const auto& stmt : block) {
        switch(stmt->node_type) {
            case Ast_Node_Type::VARIABLE_DECLARATION: {
                auto& var = dynamic_cast<Ast_Var_Decl&>(*stmt);
                if(!check_type(var.type)) {
                    report_error("Type " + get_type_name(var.type)
                            + " was not found in current context.");
                }
            }
            default:
                break;
        }
    }
    _symbol_table.pop_scope();
}

bool Semantic_Analyzer::check_type(Ast_Node_Ptr& type) const {
    if(type->node_type == Ast_Node_Type::POINTER) {
        return check_type(dynamic_cast<Ast_Pointer&>(*type).expr);
    }
    if(type->node_type == Ast_Node_Type::ARRAY) {
        return check_type(dynamic_cast<Ast_Array&>(*type).expr);
    }
    return _symbol_table.declaration_exists(
            dynamic_cast<Ast_Identifier&>(*type).name);
}

std::string Semantic_Analyzer::get_type_name(const Ast_Node_Ptr& type) const {
    if(type->node_type == Ast_Node_Type::POINTER) {
        return get_type_name(dynamic_cast<Ast_Pointer&>(*type).expr);
    }
    if(type->node_type == Ast_Node_Type::ARRAY) {
        return get_type_name(dynamic_cast<Ast_Array&>(*type).expr);
    }
    return dynamic_cast<Ast_Identifier&>(*type).name;
}

}

