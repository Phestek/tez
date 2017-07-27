#ifndef TEZ_SYMBOL_TABLE_H
#define TEZ_SYMBOL_TABLE_H

#include <string>
#include <variant>
#include <vector>

namespace tez {

struct Built_In {
    std::string name;
};

struct Function {
    struct Param {
        std::string name;
        //Type        type;
    };
    std::string        name;
    std::vector<Param> params;
    //Type        return_type;
};

enum class Symbol_Type {
    BUILT_IN, FUNCTION, STRUCT, UNION, ENUM, VARIABLE
};

using Symbol_Data = std::variant<Built_In>;

struct Symbol {
    Symbol_Type type;
    Symbol_Data data;
};

// It doesn't support namespaces yet.
class Symbol_Table {
    Symbol_Table();

    void push_scope();
    void pop_scope();

    // Add new symbol to current scope.
    void push_symbol(Symbol&& symbol);

public:
private:
    std::vector<std::vector<Symbol>> _scopes;
};

Symbol make_built_in(std::string&& name);
Symbol make_function(std::string&& name);

}

#endif // TEZ_SYMBOL_TABLE_H

