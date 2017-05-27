#include "c_code_gen.h"

#include <ostream>
#include <sstream>

namespace wayward {

namespace {

// I know, I know - global variable.
std::size_t indent_level = 0;

std::ostream& indent(std::ostream& out);
std::ostream& operator<<(std::ostream& out, const ast_node& node);
std::ostream& operator<<(std::ostream& out, const ast_binary_operation& bin);
std::ostream& operator<<(std::ostream& out, const ast_boolean& boolean);
std::ostream& operator<<(std::ostream& out, const ast_integer& integer);
std::ostream& operator<<(std::ostream& out, const ast_real_number& real_number);
std::ostream& operator<<(std::ostream& out, const ast_string& string);
std::ostream& operator<<(std::ostream& out, const ast_identifier& identifier);
std::ostream& operator<<(std::ostream& out, const ast_function_declaration& function);
std::ostream& operator<<(std::ostream& out, const ast_function_call& func_call);
std::ostream& operator<<(std::ostream& out, const ast_function_return& func_ret);
std::ostream& operator<<(std::ostream& out, const ast_variable_declaration& var);

std::ostream& indent(std::ostream& out) {
    return out << ";\n" << std::string(indent_level * 4, ' ');
}

std::ostream& operator<<(std::ostream& out, const ast_node& node) {
    switch(node.node_type) {
        case ast_node_type::binary_operation:
            return out << dynamic_cast<const ast_binary_operation&>(node);
        case ast_node_type::boolean:
            return out << dynamic_cast<const ast_boolean&>(node);
        case ast_node_type::integer:
            return out << dynamic_cast<const ast_integer&>(node);
        case ast_node_type::real_number:
            return out << dynamic_cast<const ast_real_number&>(node);
        case ast_node_type::string:
            return out << dynamic_cast<const ast_string&>(node);
        case ast_node_type::identifier:
            return out << dynamic_cast<const ast_identifier&>(node);
        case ast_node_type::function_declaration:
            return out << dynamic_cast<const ast_function_declaration&>(node);
        case ast_node_type::function_call:
            return out << dynamic_cast<const ast_function_call&>(node);
        case ast_node_type::function_return:
            return out << dynamic_cast<const ast_function_return&>(node);
        case ast_node_type::variable_declaration:
            return out << dynamic_cast<const ast_variable_declaration&>(node);
        default:
            return out << "<not implemented>";
    }
}

std::ostream& operator<<(std::ostream& out, const ast_binary_operation& bin) {
    return out << *bin.left << ' ' << bin.operat << ' ' << *bin.right;
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
    return out << identifier.value;
}

std::ostream& operator<<(std::ostream& out, const ast_function_declaration& function) {
    out << function.return_type << ' ' << function.name << '(';
    if(function.params.size() == 0) {
        out << "void";
    } else {
        for(std::size_t i = 0; i < function.params.size(); ++i) {
            if(function.params.at(i).constant) {
                out << "const ";
            }
            out << function.params.at(i).type << ' ' << function.params.at(i).name;
            if(i < function.params.size() - 1) {
                out << ", ";
            }
        }
    }
    out << ") {";
    ++indent_level;
    for(const auto& node : function.body) {
        out << indent << *node;
    }
    --indent_level;
    return out << ";\n}\n";
}

std::ostream& operator<<(std::ostream& out, const ast_function_call& func_call) {
    out << func_call.name << '(';
    for(std::size_t i = 0; i < func_call.params.size(); ++i) {
        out << *func_call.params.at(i);
        if(i < func_call.params.size() - 1) {
            out << ", ";
        }
    }
    return out << ")";
}

std::ostream& operator<<(std::ostream& out, const ast_function_return& func_ret) {
    return out << "return " << *func_ret.value;
}

std::ostream& operator<<(std::ostream& out, const ast_variable_declaration& var) {
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

}

std::string generate_c_code(const std::vector<ast_node_ptr>& ast) {
    std::stringstream output;
    output
            << "#include <stdio.h>\n"
            << "#include <stdlib.h>\n";
    for(const auto& node : ast) {
        output << *node;
    }
    return output.str();
}

}

