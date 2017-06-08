#ifndef TEZ_C_CODE_GENERATOR_H
#define TEZ_C_CODE_GENERATOR_H

#include "ast.h"

namespace tez {

class C_Code_Generator {
public:
    C_Code_Generator(Ast& ast);

    std::string generate();

private:
    std::string print_indent();
    std::string print_statement(const Ast_Node& node);
    std::string print(const Ast_Node& node);
    std::string print(const Ast_Block& block);
    std::string print(const Ast_Namespace& ns);
    std::string print(const Ast_Unary_Operation& unary);
    std::string print(const Ast_Binary_Operation& bin);
    std::string print(const Ast_Grouping_Expression& grouping);
    std::string print(const Ast_Boolean& boolean);
    std::string print(const Ast_Integer& integer);
    std::string print(const Ast_Real_Number& real);
    std::string print(const Ast_String& str);
    std::string print(const Ast_Identifier& identifier);
    std::string print(const Ast_Func_Decl& func);
    std::string print(const Ast_Func_Call& func_call);
    std::string print(const Ast_Return& ret);
    std::string print(const Ast_Var_Decl& var);
    std::string print(const Ast_If& if_stat);
    std::string print(const Ast_While& while_loop);
    std::string print(const Ast_Do_While& do_while_loop);
    std::string print(const Ast_For& for_loop);
    std::string print(const Ast_Struct& struct_decl);
    std::string print(const Ast_Enum& enum_decl);

    Ast& _ast;

    std::size_t _indent_level = 0;

    std::vector<std::string> _namespaces;
};

}

#endif //TEZ_C_CODE_GENERATOR_H

