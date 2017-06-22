#ifndef TEZ_SEMANTIC_ANALYSER_H
#define TEZ_SEMANTIC_ANALYSER_H

#include "semantic/symbol_table.h"
#include "syntax/ast.h"

namespace tez {

class Semantic_Analyser {
public:
    void analyse(Ast& ast);

    bool errors_reported();

private:
    void collect_top_level_declarations(const Ast& ast);

    bool _errors_reported = false;

    Symbol_Table _symbol_table;
};

}

#endif //TEZ_SEMANTIC_ANALYSER_H

