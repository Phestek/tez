#include "symbol_table.h"

#include <algorithm>

namespace tez {

Symbol_Table::Symbol_Table() {
    push_scope();   // Create global scope.
}

void Symbol_Table::push_scope() {
    _declarations.emplace_back();
}

void Symbol_Table::pop_scope() {
    _declarations.pop_back();
}

bool Symbol_Table::push_declaration(Declaration&& declaration) {
    _declarations.back().push_back(declaration);
}

bool Symbol_Table::declaration_exists(const std::string& name) {
    for(const auto& scope : _declarations) {
        for(const auto decl : scope) {
            if(decl.name == name) {
                return true;
            }
        }
    }
    return false;
}

std::optional<Symbol_Table::Declaration_Ref> Symbol_Table::get_declaration(
        const std::string& name) {
    //for(const auto& scope : _declarations) {
        //auto result = std::find_if(scope.begin(), scope.end(),
                //[&](const Declaration& decl) {
                    //return decl.name == name ? true : false;
                //});
        //if(result != scope.end()) {
            //return {*result};
        //}
    //}
    return {};
}

}

