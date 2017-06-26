#ifndef TEZ_SYMBOL_TABLE_H
#define TEZ_SYMBOL_TABLE_H

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace tez {

struct Declaration {
    enum class Type { NONE, BUILT_IN, FUNCTION, VARIABLE, STRUCT, ENUM };
    std::string  name = "";
    Type         type = Type::NONE;
};


enum class Scope_Type {
    NAMESPACE, FUNCTION, IF, DO_WHILE, WHILE, FOR, STRUCT
};

class Symbol_Table {
public:
    /** Default constructor. Creates global namespace and adds primitive types
     *  to declarations table. */
    Symbol_Table();

    /** Push new scope. */
    void push_scope(Scope_Type type);

    /** Pops scope and all declarations in this scope. */
    void pop_scope();

    /** Check if given scope type exists in current context. */
    bool scope_exists(Scope_Type type);

    /** Add new declaration to current scope. */
    bool push_declaration(Declaration&& declaration);

    /** Check if declaration with given name exists in current context. */
    bool declaration_exists(const std::string& name) const;
    bool declaration_exists(const std::string& name, Scope_Type type) const;

    /** Get declaration from current scope. */
    std::optional<Declaration> get_declaration(const std::string& name) const;

private:
    std::vector<std::vector<Declaration>> _declarations;
    std::vector<Scope_Type>               _scope_names;
};

}

#endif // TEZ_SYMBOL_TABLE_H

