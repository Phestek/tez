#include "symbol_table.h"

#include <algorithm>

namespace tez {

Symbol_Table::Symbol_Table() {
    // Create global scope.
    _scopes.emplace_back();
    _scopes.back().push_back(make_built_in("Void"));
    _scopes.back().push_back(make_built_in("Bool"));
    _scopes.back().push_back(make_built_in("Char"));
    _scopes.back().push_back(make_built_in("Rune"));
    _scopes.back().push_back(make_built_in("Int"));
    _scopes.back().push_back(make_built_in("Uint"));
    _scopes.back().push_back(make_built_in("Int8"));
    _scopes.back().push_back(make_built_in("Int16"));
    _scopes.back().push_back(make_built_in("Int32"));
    _scopes.back().push_back(make_built_in("Int64"));
    _scopes.back().push_back(make_built_in("Uint8"));
    _scopes.back().push_back(make_built_in("Uint16"));
    _scopes.back().push_back(make_built_in("Uint32"));
    _scopes.back().push_back(make_built_in("Uint64"));
    _scopes.back().push_back(make_built_in("Float"));
    _scopes.back().push_back(make_built_in("Double"));
}

void Symbol_Table::push_scope() {
    _scopes.emplace_back();
}

void Symbol_Table::pop_scope() {
    _scopes.pop_back();
}

void Symbol_Table::push_symbol(Symbol&& symbol) {
    _scopes.back().push_back(std::move(symbol));
}

Symbol make_built_in(std::string&& name) {
    return Symbol{Symbol_Type::BUILT_IN, Built_In{std::move(name)}};
}

}

