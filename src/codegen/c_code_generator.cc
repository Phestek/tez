#include "c_code_generator.h"

#include <iostream>

namespace tez {

C_Code_Generator::C_Code_Generator(Ast& ast)
        : _ast{ast} {
}

std::string C_Code_Generator::generate() {
    std::string result;
    for(const auto& node : _ast) {
        result += print_statement(*node);
    }
    return result;
}

std::string C_Code_Generator::print_indent() {
    return std::string(_indent_level * 4, ' ');
}

std::string C_Code_Generator::print_statement(const Ast_Node& node) {
    std::string stmt = print_indent() + print(node);
    if(node.node_type != Ast_Node_Type::BLOCK // TODO: Find better way to do it.
            && node.node_type != Ast_Node_Type::NAMESPACE
            && node.node_type != Ast_Node_Type::FUNCTION_DECLARATION
            && node.node_type != Ast_Node_Type::IF
            && node.node_type != Ast_Node_Type::WHILE
            && node.node_type != Ast_Node_Type::FOR) {
        stmt += ';';
    }
    stmt += "\n";
    return stmt;
}

std::string C_Code_Generator::print(const Ast_Node& node) {
    switch(node.node_type) {
        case Ast_Node_Type::UNDEFINED:
        case Ast_Node_Type::NAMESPACE:
            //return print(dynamic_cast<const Ast_Namespace&>(node));
            break;
        case Ast_Node_Type::BLOCK:
            return print(dynamic_cast<const Ast_Block&>(node));
        case Ast_Node_Type::UNARY_OPERATION:
            return print(dynamic_cast<const Ast_Unary_Operation&>(node));
        case Ast_Node_Type::BINARY_OPERATION:
            return print(dynamic_cast<const Ast_Binary_Operation&>(node));
        case Ast_Node_Type::GROUPING_EXPRESSION:
            return print(dynamic_cast<const Ast_Grouping_Expression&>(node));
        case Ast_Node_Type::CAST:
            return print(dynamic_cast<const Ast_Cast&>(node));
        case Ast_Node_Type::BOOLEAN:
            return print(dynamic_cast<const Ast_Boolean&>(node));
        case Ast_Node_Type::INTEGER:
            return print(dynamic_cast<const Ast_Integer&>(node));
        case Ast_Node_Type::REAL_NUMBER:
            return print(dynamic_cast<const Ast_Real_Number&>(node));
        case Ast_Node_Type::STRING:
            return print(dynamic_cast<const Ast_String&>(node));
        case Ast_Node_Type::IDENTIFIER:
            return print(dynamic_cast<const Ast_Identifier&>(node));
        case Ast_Node_Type::FUNCTION_DECLARATION:
            return print(dynamic_cast<const Ast_Func_Decl&>(node));
        case Ast_Node_Type::FUNCTION_CALL:
            return print(dynamic_cast<const Ast_Func_Call&>(node));
        case Ast_Node_Type::RETURN:
            return print(dynamic_cast<const Ast_Return&>(node));
        case Ast_Node_Type::VARIABLE_DECLARATION:
            return print(dynamic_cast<const Ast_Var_Decl&>(node));
        case Ast_Node_Type::IF:
            return print(dynamic_cast<const Ast_If&>(node));
        case Ast_Node_Type::WHILE:
            return print(dynamic_cast<const Ast_While&>(node));
        case Ast_Node_Type::DO_WHILE:
            return print(dynamic_cast<const Ast_Do_While&>(node));
        case Ast_Node_Type::FOR:
            return print(dynamic_cast<const Ast_For&>(node));
        case Ast_Node_Type::STRUCT:
            return print(dynamic_cast<const Ast_Struct&>(node));
        case Ast_Node_Type::STRUCT_CONSTRUCTOR:
            return print(dynamic_cast<const Ast_Struct_Constructor&>(node));
        case Ast_Node_Type::ENUM:
            return print(dynamic_cast<const Ast_Enum&>(node));
        case Ast_Node_Type::BREAK:
            return print_indent() + "break";
        case Ast_Node_Type::CONTINUE:
            return print_indent() + "continue";
        case Ast_Node_Type::ARRAY_ACCESS:
            return print(dynamic_cast<const Ast_Array_Access&>(node));
        default:
            std::cerr << "Error: Tried to print undefined node!\n";
    }
    return "";
}

std::string C_Code_Generator::print([[maybe_unused]]const Ast_Namespace& ns) {
    return "";
}

std::string C_Code_Generator::print(const Ast_Block& block) {
    std::string result = " {\n";
    ++_indent_level;
    for(const auto& stmt : block.statements) {
        result += print_statement(*stmt);
    }
    --_indent_level;
    result += print_indent() + "}";
    return result;
}

std::string C_Code_Generator::print(const Ast_Unary_Operation& unary) {
    return unary.operat + print(*unary.left);
}

std::string C_Code_Generator::print(const Ast_Binary_Operation& bin) {
    return print(*bin.left) + " " + bin.operat + " " + print(*bin.right);
}

std::string C_Code_Generator::print(const Ast_Grouping_Expression& grouping) {
    return "(" + print(*grouping.expr) + ")";
}

std::string C_Code_Generator::print(const Ast_Cast& cast) {
    return "(" + print(*cast.to) + ")" + print(*cast.expr);
}

std::string C_Code_Generator::print(const Ast_Boolean& boolean) {
    return std::to_string(static_cast<int>(boolean.value));
}

std::string C_Code_Generator::print(const Ast_Integer& integer) {
    return std::to_string(integer.value);
}

std::string C_Code_Generator::print(const Ast_Real_Number& real) {
    return std::to_string(real.value);
}

std::string C_Code_Generator::print(const Ast_String& str) {
    return "\"" + str.value + "\"";
}

std::string C_Code_Generator::print(const Ast_Identifier& identifier) {
    return identifier.name;
}

std::string C_Code_Generator::print(const Ast_Func_Decl& func) {
    std::string result = func.return_type + " " + func.name + "(";
    if(func.params.size() == 0) {
        result += "void";
    } else {
        for(std::size_t i = 0; i < func.params.size(); ++i) {
            if(func.params.at(i).constant) {
                result += "const ";
            }
            result += func.params.at(i).type + ' ' + func.params.at(i).name;
            if(i < func.params.size() - 1) {
                result += ", ";
            }
        }
    }
    return result + ")" + print(func.body);
}

std::string C_Code_Generator::print(const Ast_Func_Call& func_call) {
    std::string result = print(*func_call.name) + "(";
    for(std::size_t i = 0; i < func_call.args.size(); ++i) {
        result += print(*func_call.args.at(i));
        if(i < func_call.args.size() - 1) {
            result += ", ";
        }
    }
    return result + ")";
}

std::string C_Code_Generator::print(const Ast_Return& ret) {
    return "return " + print(*ret.expr);
}

std::string C_Code_Generator::print(const Ast_Var_Decl& var) {
    std::string result;
    if(var.constant) {
        result += "const ";
    }
    // TODO:
    result += var.type + " " + var.name + " = ";
    if(var.initializer == nullptr) {
        result += '0';
    } else {
        result += print(*var.initializer);
    }
    return result;
}

std::string C_Code_Generator::print(const Ast_If& if_stmt) {
    std::string result = "if(" + print(*if_stmt.condition) + ")"
            + print(if_stmt.if_block);
    if(if_stmt.else_block == nullptr) {
        return result;
    }
    return result + print_indent() + "else " + print(*if_stmt.else_block);
}

std::string C_Code_Generator::print(const Ast_While& while_loop) {
    return "while(" + print(*while_loop.condition) + ")"
            + print(while_loop.body);
}

std::string C_Code_Generator::print(const Ast_Do_While& do_while_loop) {
    return "do " + print(do_while_loop.body) + " while("
        + print(*do_while_loop.condition) + ")";
}

std::string C_Code_Generator::print(const Ast_For& for_loop) {
    return "for(" + print(*for_loop.init_statement) + "; "
            + print(*for_loop.condition) + "; "
            + print(*for_loop.iteration_expr) + ")"
            + print(for_loop.body);
}

std::string C_Code_Generator::print(const Ast_Struct& struct_decl) {
    std::string result = "typedef struct {\n";
    ++_indent_level;
    for(const auto& field : struct_decl.fields) {
        result += print_indent() + field.type + " " + field.name + ";\n";
    }
    --_indent_level;
    return result + "} " + struct_decl.name;
}

std::string C_Code_Generator::print(const Ast_Struct_Constructor& struct_constr) {
    std::string result = "{";
    for(std::size_t i = 0; i < struct_constr.declaration_list.size(); ++i) {
        result += print(*struct_constr.declaration_list.at(i));
        if(i < struct_constr.declaration_list.size() - 1) {
            result += ", ";
        }
    }
    return result + "}";
}

std::string C_Code_Generator::print(const Ast_Enum& enum_decl) {
    std::string result = "typedef enum {\n";
    ++_indent_level;
    for(const auto& enumeration : enum_decl.enumerations) {
        result += print_indent() + enum_decl.name + "_" + enumeration.name
            + " = " + std::to_string(enumeration.value) + ",\n";
    }
    --_indent_level;
    return result + "} " + enum_decl.name;
}

std::string C_Code_Generator::print(const Ast_Array_Access& array_access) {
    return print(*array_access.array) + "[" + print(*array_access.at) + "]";
}

}

