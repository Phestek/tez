#include "c_code_gen.h"

#include <ostream>
#include <sstream>

namespace wayward {

namespace {

// I know, I know - global variable.
std::size_t indent_level = 0;

std::string print_statement(const ast_node& node);
std::ostream& indent(std::ostream& out);
std::ostream& operator<<(std::ostream& out, const ast_node& node);
std::ostream& operator<<(std::ostream& out, const ast_block& block);
std::ostream& operator<<(std::ostream& out, const ast_binary_operation& bin);
std::ostream& operator<<(std::ostream& out, const ast_grouping_expression& expr);
std::ostream& operator<<(std::ostream& out, const ast_boolean& boolean);
std::ostream& operator<<(std::ostream& out, const ast_integer& integer);
std::ostream& operator<<(std::ostream& out, const ast_real_number& real_number);
std::ostream& operator<<(std::ostream& out, const ast_string& string);
std::ostream& operator<<(std::ostream& out, const ast_identifier& identifier);
std::ostream& operator<<(std::ostream& out, const ast_func_decl& function);
std::ostream& operator<<(std::ostream& out, const ast_func_call& func_call);
std::ostream& operator<<(std::ostream& out, const ast_return& ret);
std::ostream& operator<<(std::ostream& out, const ast_var_decl& var);
std::ostream& operator<<(std::ostream& out, const ast_if& _if);
std::ostream& operator<<(std::ostream& out, const ast_while& _while);
std::ostream& operator<<(std::ostream& out, const ast_do_while& _while);
std::ostream& operator<<(std::ostream& out, const ast_for& _for);
std::ostream& operator<<(std::ostream& out, const ast_struct& _struct);
std::ostream& operator<<(std::ostream& out, const ast_enum& _enum);

std::string print_statement(const ast_node& node) {
    std::stringstream ss;
    ss << indent << node;
    if(node.node_type != ast_node_type::BLOCK   // TODO: Find better way to do it.
            && node.node_type != ast_node_type::FUNCTION_DECLARATION
            && node.node_type != ast_node_type::IF
            && node.node_type != ast_node_type::WHILE
            && node.node_type != ast_node_type::FOR) {
        ss << ';';
    }
    ss << '\n';
    return ss.str();
}

std::ostream& indent(std::ostream& out) {
    return out << std::string(indent_level * 4, ' ');
}

std::ostream& operator<<(std::ostream& out, const ast_node& node) {
    switch(node.node_type) {
        case ast_node_type::BLOCK:
            return out << dynamic_cast<const ast_block&>(node);
        case ast_node_type::BINARY_OPERATION:
            return out << dynamic_cast<const ast_binary_operation&>(node);
        case ast_node_type::GROUPING_EXPRESSION:
            return out << dynamic_cast<const ast_grouping_expression&>(node);
        case ast_node_type::BOOLEAN:
            return out << dynamic_cast<const ast_boolean&>(node);
        case ast_node_type::INTEGER:
            return out << dynamic_cast<const ast_integer&>(node);
        case ast_node_type::REAL_NUMBER:
            return out << dynamic_cast<const ast_real_number&>(node);
        case ast_node_type::STRING:
            return out << dynamic_cast<const ast_string&>(node);
        case ast_node_type::IDENTIFIER:
            return out << dynamic_cast<const ast_identifier&>(node);
        case ast_node_type::FUNCTION_DECLARATION:
            return out << dynamic_cast<const ast_func_decl&>(node);
        case ast_node_type::FUNCTION_CALL:
            return out << dynamic_cast<const ast_func_call&>(node);
        case ast_node_type::RETURN:
            return out << dynamic_cast<const ast_return&>(node);
        case ast_node_type::VARIABLE_DECLARATION:
            return out << dynamic_cast<const ast_var_decl&>(node);
        case ast_node_type::IF:
            return out << dynamic_cast<const ast_if&>(node);
        case ast_node_type::WHILE:
            return out << dynamic_cast<const ast_while&>(node);
        case ast_node_type::DO_WHILE:
            return out << dynamic_cast<const ast_do_while&>(node);
        case ast_node_type::FOR:
            return out << dynamic_cast<const ast_for&>(node);
        case ast_node_type::STRUCT:
            return out << dynamic_cast<const ast_struct&>(node);
        case ast_node_type::ENUM:
            return out << dynamic_cast<const ast_enum&>(node);
        default:
            return out << "<not implemented>";
    }
}

std::ostream& operator<<(std::ostream& out, const ast_block& block) {
    out << "{\n";
    ++indent_level;
    for(const auto& statement : block.statements) {
        out << print_statement(*statement);
    }
    --indent_level;
    return out << indent << "}";
}

std::ostream& operator<<(std::ostream& out, const ast_binary_operation& bin) {
    return out << *bin.left << ' ' << bin.operat << ' ' << *bin.right;
}

std::ostream& operator<<(std::ostream& out, const ast_grouping_expression& expr) {
    return out << '(' << *expr.expr << ')';
}

std::ostream& operator<<(std::ostream& out, const ast_boolean& boolean) {
    return out << boolean.value;
}

std::ostream& operator<<(std::ostream& out, const ast_integer& integer) {
    return out << integer.value;
}

std::ostream& operator<<(std::ostream& out, const ast_real_number& real_number) {
    return out << real_number.value;
}

std::ostream& operator<<(std::ostream& out, const ast_string& string) {
    return out << '"' << string.value << '"';
}

std::ostream& operator<<(std::ostream& out, const ast_identifier& identifier) {
    return out << identifier.name;
}

std::ostream& operator<<(std::ostream& out, const ast_func_decl& func) {
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

std::ostream& operator<<(std::ostream& out, const ast_func_call& func_call) {
    out << func_call.name << '(';
    for(std::size_t i = 0; i < func_call.args.size(); ++i) {
        out << *func_call.args.at(i);
        if(i < func_call.args.size() - 1) {
            out << ", ";
        }
    }
    return out << ")";
}

std::ostream& operator<<(std::ostream& out, const ast_return& ret) {
    return out << "return " << *ret.value;
}

std::ostream& operator<<(std::ostream& out, const ast_var_decl& var) {
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

std::ostream& operator<<(std::ostream& out, const ast_if& _if) {
    out << "if(" << *_if.condition << ") " << _if.if_block;
    if(_if.else_block == nullptr) {
        return out;
    }
    return out << indent << "else " << *_if.else_block;
}

std::ostream& operator<<(std::ostream& out, const ast_while& _while) {
    return out << "while(" << *_while.condition << ") " << _while.body;
}

std::ostream& operator<<(std::ostream& out, const ast_do_while& do_while) {
    return out << "do " << do_while.body << " while(" << *do_while.condition
            << ')';
}

std::ostream& operator<<(std::ostream& out, const ast_for& _for) {
    return out << "for(" << *_for.init_statement << "; " << *_for.condition
            << "; " << *_for.iteration_expr << ") "<< _for.body;
}

std::ostream& operator<<(std::ostream& out, const ast_struct& _struct) {
    out << "struct " << _struct.name << " {\n";
    ++indent_level;
    for(const auto& field : _struct.fields) {
        out << indent << field.type << ' ' << field.name << ";\n";
    }
    --indent_level;
    return out << '}';
}

std::ostream& operator<<(std::ostream& out, const ast_enum& _enum) {
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

std::string generate_c_code(const std::vector<ast_node_ptr>& ast) {
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

