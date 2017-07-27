#ifndef TEZ_SEMANTIC_ANALYSER_H
#define TEZ_SEMANTIC_ANALYSER_H

#include "semantic/symbol_table.h"
#include "syntax/ast.h"

namespace tez {

class Code_Manager {
public:
    Code_Manager(Ast& ast);

    int errors_reported() const;
    int warnings_reported() const;

private:
    Ast& _ast;

    Symbol_Table _symbol_table;

    int _errors_reported = 0;
    int _warnings_reported = 0;
};

}

#endif //TEZ_SEMANTIC_ANALYSER_H

