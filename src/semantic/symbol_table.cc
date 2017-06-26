#include "symbol_table.h"

#include <algorithm>

namespace tez {

Symbol_Table::Symbol_Table() {
    // Create global namespace.
    push_scope(Scope_Type::NAMESPACE);

    // Declare primitive types.
    push_declaration({"void",    Declaration::Type::BUILT_IN});
    push_declaration({"int",     Declaration::Type::BUILT_IN});
    push_declaration({"uint",    Declaration::Type::BUILT_IN});
    push_declaration({"char",    Declaration::Type::BUILT_IN});
    push_declaration({"int8",    Declaration::Type::BUILT_IN});
    push_declaration({"int16",   Declaration::Type::BUILT_IN});
    push_declaration({"int32",   Declaration::Type::BUILT_IN});
    push_declaration({"int64",   Declaration::Type::BUILT_IN});
    push_declaration({"uint8",   Declaration::Type::BUILT_IN});
    push_declaration({"uint16",  Declaration::Type::BUILT_IN});
    push_declaration({"uint32",  Declaration::Type::BUILT_IN});
    push_declaration({"uint64",  Declaration::Type::BUILT_IN});
    push_declaration({"float32", Declaration::Type::BUILT_IN});
    push_declaration({"float64", Declaration::Type::BUILT_IN});
}

void Symbol_Table::push_scope(Scope_Type type) {
    _declarations.emplace_back();
    _scope_names.push_back(type);
}

void Symbol_Table::pop_scope() {
    _declarations.pop_back();
    _scope_names.pop_back();
}

bool Symbol_Table::scope_exists(Scope_Type type) {
    // We search from end to begin, because something inside my head tells me
    // its just better idea...
    for(auto it = _scope_names.crbegin(); it < _scope_names.crend(); ++it) {
        if(*it == type) {
            return true;
        }
    }
    return false;
}

bool Symbol_Table::push_declaration(Declaration&& declaration) {
    if(declaration_exists(declaration.name)) {
        return false;
    }
    _declarations.back().push_back(declaration);
    return true;
}

bool Symbol_Table::declaration_exists(const std::string& name) const {
    for(const auto& scope : _declarations) {
        for(const auto decl : scope) {
            if(decl.name == name) {
                return true;
            }
        }
    }
    return false;
}

bool Symbol_Table::declaration_exists(const std::string& name,
        Scope_Type type) const {
    for(const auto& scope : _declarations) {
        for(const auto decl : scope) {
            if(decl.name == name && decl.type == type) {
                return true;
            }
        }
    }
    return false;
}

std::optional<Declaration> Symbol_Table::get_declaration(
        const std::string& name) const {
    for(const auto& scope : _declarations) {
        auto result = std::find_if(scope.begin(), scope.end(),
                [&](const Declaration& decl) {
                    return decl.name == name ? true : false;
                });
        if(result != scope.end()) {
            return {*result};
        }
    }
    return {};
}

}

