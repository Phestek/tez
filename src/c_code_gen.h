#ifndef TEZ_C_CODE_GEN_H
#define TEZ_C_CODE_GEN_H

#include <vector>

#include "ast.h"

namespace tez {

std::string generate_c_code(const std::vector<Ast_Node_Ptr>& ast);

}

#endif //TEZ_C_CODE_GEN_H

