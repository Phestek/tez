#include "c_code_gen.h"

#include <ostream>
#include <sstream>

namespace tez {

namespace {

// I know, I know - global variable.
std::size_t indent_level = 0;

std::string print_statement(const Ast_Node& node);
std::ostream& indent(std::ostream& out);
std::ostream& operator<<(std::ostream& out, const Ast_Node& node);
std::ostream& operator<<(std::ostream& out, const Ast_Block& block);
std::ostream& operator<<(std::ostream& out, const Ast_Unary_Operation& unary);
std::ostream& operator<<(std::ostream& out, const Ast_Binary_Operation& bin);
std::ostream& operator<<(std::ostream& out, const Ast_Grouping_Expression& expr);
std::ostream& operator<<(std::ostream& out, const Ast_Boolean& boolean);
std::ostream& operator<<(std::ostream& out, const Ast_Integer& integer);
std::ostream& operator<<(std::ostream& out, const Ast_Real_Number& real_number);
std::ostream& operator<<(std::ostream& out, const Ast_String& string);
std::ostream& operator<<(std::ostream& out, const Ast_Identifier& identifier);
std::ostream& operator<<(std::ostream& out, const Ast_Func_Decl& function);
std::ostream& operator<<(std::ostream& out, const Ast_Func_Call& func_call);
std::ostream& operator<<(std::ostream& out, const Ast_Return& ret);
std::ostream& operator<<(std::ostream& out, const Ast_Var_Decl& var);
std::ostream& operator<<(std::ostream& out, const Ast_If& _if);
std::ostream& operator<<(std::ostream& out, const Ast_While& _while);
std::ostream& operator<<(std::ostream& out, const Ast_Do_While& _while);
std::ostream& operator<<(std::ostream& out, const Ast_For& _for);
std::ostream& operator<<(std::ostream& out, const Ast_Struct& _struct);
std::ostream& operator<<(std::ostream& out, const Ast_Enum& _enum);

std::string print_statement(const Ast_Node& node) {
    std::stringstream ss;
    ss << indent << node;
    if(node.node_type != Ast_Node_Type::BLOCK   // TODO: Find better way to do it.
            && node.node_type != Ast_Node_Type::FUNCTION_DECLARATION
            && node.node_type != Ast_Node_Type::IF
            && node.node_type != Ast_Node_Type::WHILE
            && node.node_type != Ast_Node_Type::FOR) {
        ss << ';';
    }
    ss << '\n';
    return ss.str();
}

std::ostream& indent(std::ostream& out) {
    return out << std::string(indent_level * 4, ' ');
}

std::ostream& operator<<(std::ostream& out, const Ast_Node& node) {
    switch(node.node_type) {
        case Ast_Node_Type::BLOCK:
            return out << dynamic_cast<const Ast_Block&>(node);
        case Ast_Node_Type::UNARY_OPERATION:
            return out << dynamic_cast<const Ast_Unary_Operation&>(node);
        case Ast_Node_Type::BINARY_OPERATION:
            return out << dynamic_cast<const Ast_Binary_Operation&>(node);
        case Ast_Node_Type::GROUPING_EXPRESSION:
            return out << dynamic_cast<const Ast_Grouping_Expression&>(node);
        case Ast_Node_Type::BOOLEAN:
            return out << dynamic_cast<const Ast_Boolean&>(node);
        case Ast_Node_Type::INTEGER:
            return out << dynamic_cast<const Ast_Integer&>(node);
        case Ast_Node_Type::REAL_NUMBER:
            return out << dynamic_cast<const Ast_Real_Number&>(node);
        case Ast_Node_Type::STRING:
            return out << dynamic_cast<const Ast_String&>(node);
        case Ast_Node_Type::IDENTIFIER:
            return out << dynamic_cast<const Ast_Identifier&>(node);
        case Ast_Node_Type::FUNCTION_DECLARATION:
            return out << dynamic_cast<const Ast_Func_Decl&>(node);
        case Ast_Node_Type::FUNCTION_CALL:
            return out << dynamic_cast<const Ast_Func_Call&>(node);
        case Ast_Node_Type::RETURN:
            return out << dynamic_cast<const Ast_Return&>(node);
        case Ast_Node_Type::VARIABLE_DECLARATION:
            return out << dynamic_cast<const Ast_Var_Decl&>(node);
        case Ast_Node_Type::IF:
            return out << dynamic_cast<const Ast_If&>(node);
        case Ast_Node_Type::WHILE:
            return out << dynamic_cast<const Ast_While&>(node);
        case Ast_Node_Type::DO_WHILE:
            return out << dynamic_cast<const Ast_Do_While&>(node);
        case Ast_Node_Type::FOR:
            return out << dynamic_cast<const Ast_For&>(node);
        case Ast_Node_Type::STRUCT:
            return out << dynamic_cast<const Ast_Struct&>(node);
        case Ast_Node_Type::ENUM:
            return out << dynamic_cast<const Ast_Enum&>(node);
        default:
            return out << "<not implemented>";
    }
}

std::ostream& operator<<(std::ostream& out, const Ast_Block& block) {
    out << "{\n";
    ++indent_level;
    for(const auto& statement : block.statements) {
        out << print_statement(*statement);
    }
    --indent_level;
    return out << indent << "}";
}

std::ostream& operator<<(std::ostream& out, const Ast_Unary_Operation& unary) {
    return out << unary.operat << *unary.left;
}

std::ostream& operator<<(std::ostream& out, const Ast_Binary_Operation& bin) {
    return out << *bin.left << ' ' << bin.operat << ' ' << *bin.right;
}

std::ostream& operator<<(std::ostream& out, const Ast_Grouping_Expression& expr) {
    return out << '(' << *expr.expr << ')';
}

std::ostream& operator<<(std::ostream& out, const Ast_Boolean& boolean) {
    return out << boolean.value;
}

std::ostream& operator<<(std::ostream& out, const Ast_Integer& integer) {
    return out << integer.value;
}

std::ostream& operator<<(std::ostream& out, const Ast_Real_Number& real_number) {
    return out << real_number.value;
}

std::ostream& operator<<(std::ostream& out, const Ast_String& string) {
    return out << '"' << string.value << '"';
}

std::ostream& operator<<(std::ostream& out, const Ast_Identifier& identifier) {
    return out << identifier.name;
}

std::ostream& operator<<(std::ostream& out, const Ast_Func_Decl& func) {
    out << func.return_type << ' ' << func.name << '(';
    if(func.params.size() == 0) {
        out << "void";
    } else {
        for(std::size_t i = 0; i < func.params.size(); ++i) {
            if(func.params.at(i).constant) {
                out << "const ";
            }
            out << func.params.at(i).type << ' ' << func.params.at(i).name;
            if(i < func.params.size() - 1) {
                out << ", ";
            }
        }
    }
    return out << ") " << func.body;
}

std::ostream& operator<<(std::ostream& out, const Ast_Func_Call& func_call) {
    out << func_call.name << '(';
    for(std::size_t i = 0; i < func_call.args.size(); ++i) {
        out << *func_call.args.at(i);
        if(i < func_call.args.size() - 1) {
            out << ", ";
        }
    }
    return out << ")";
}

std::ostream& operator<<(std::ostream& out, const Ast_Return& ret) {
    return out << "return " << *ret.value;
}

std::ostream& operator<<(std::ostream& out, const Ast_Var_Decl& var) {
    if(var.constant) {
        out << "const ";
    }
    out << var.type << ' ' << var.name << " = ";
    if(var.initializer == nullptr) {
        out << '0';
    } else {
        out << *var.initializer;
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, const Ast_If& _if) {
    out << "if(" << *_if.condition << ") " << _if.if_block;
    if(_if.else_block == nullptr) {
        return out;
    }
    return out << indent << "else " << *_if.else_block;
}

std::ostream& operator<<(std::ostream& out, const Ast_While& _while) {
    return out << "while(" << *_while.condition << ") " << _while.body;
}

std::ostream& operator<<(std::ostream& out, const Ast_Do_While& do_while) {
    return out << "do " << do_while.body << " while(" << *do_while.condition
            << ')';
}

std::ostream& operator<<(std::ostream& out, const Ast_For& _for) {
    return out << "for(" << *_for.init_statement << "; " << *_for.condition
            << "; " << *_for.iteration_expr << ") "<< _for.body;
}

std::ostream& operator<<(std::ostream& out, const Ast_Struct& _struct) {
    out << "struct " << _struct.name << " {\n";
    ++indent_level;
    for(const auto& field : _struct.fields) {
        out << indent << field.type << ' ' << field.name << ";\n";
    }
    --indent_level;
    return out << '}';
}

std::ostream& operator<<(std::ostream& out, const Ast_Enum& _enum) {
    out << "enum " << _enum.name << " {\n";
    ++indent_level;
    for(const auto& enumeration : _enum.enumerations) {
        out << indent << _enum.name << '_' << enumeration.name << " = "
                << enumeration.value << ",\n";
    }
    --indent_level;
    return out << '}';
}

}

std::string generate_c_code(const std::vector<Ast_Node_Ptr>& ast) {
    std::stringstream output;
    output
            << "#include <stdio.h>\n"
            << "#include <stdlib.h>\n";
    for(const auto& node : ast) {
        output << print_statement(*node);
    }
    output << '\n';
    return output.str();
}

}

