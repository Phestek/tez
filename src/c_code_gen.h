#ifndef WAYWARD_C_CODE_GEN_H
#define WAYWARD_C_CODE_GEN_H

#include <vector>

#include "ast.h"

namespace wayward {

std::string generate_c_code(const std::vector<ast_node_ptr>& ast);

}

#endif //WAYWARD_C_CODE_GEN_H

