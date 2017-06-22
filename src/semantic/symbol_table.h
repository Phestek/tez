#ifndef TEZ_SYMBOL_TABLE_H
#define TEZ_SYMBOL_TABLE_H

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace tez {

struct Declaration {
    enum class Type { NONE, FUNCTION, VARIABLE, STRUCT };
    std::string  name = "";
    Type         type = Type::NONE;
};

class Symbol_Table {
public:
    using Declaration_Ref = std::reference_wrapper<Declaration>;

    Symbol_Table();

    /** Push new scope. */
    void push_scope();

    /** Pops scope and all declarations in this scope. */
    void pop_scope();

    /** Add new declaration to current scope. */
    bool push_declaration(Declaration&& declaration);

    /** Check if declaration with given name exists in current context. */
    bool declaration_exists(const std::string& name);

    /** Get declaration from current scope. */
    std::optional<Declaration_Ref> get_declaration(const std::string& name);

private:
    std::vector<std::vector<Declaration>> _declarations;
};

}

#endif // TEZ_SYMBOL_TABLE_H

