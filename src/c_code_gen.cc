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
std::ostream& operator<<(std::ostream& out, const ast_identifier& identifier);
std::ostream& operator<<(std::ostream& out, const ast_function& function);

std::ostream& indent(std::ostream& out) {
    return out << std::string(indent_level * 4, ' ');
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
        case ast_node_type::identifier:
            return out << dynamic_cast<const ast_identifier&>(node);
        case ast_node_type::function:
            return out << dynamic_cast<const ast_function&>(node);
        default:
            return out << "<not implemented>";
    }
}

std::ostream& operator<<(std::ostream& out, const ast_binary_operation& bin) {
    return out << *bin.left << ' ' << bin.operat << ' ' << *bin.right << ";\n";
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

std::ostream& operator<<(std::ostream& out, const ast_identifier& identifier) {
    return out << identifier.value;
}

std::ostream& operator<<(std::ostream& out, const ast_function& function) {
    out << function.return_type << ' ' << function.name << "() {\n";
    ++indent_level;
    for(const auto& node : function.body) {
        out << indent << *node;
    }
    --indent_level;
    return out << "}\n";
}

}

std::string generate_c_code(const std::vector<ast_node_ptr>& ast) {
    std::stringstream output;
    for(const auto& node : ast) {
        output << indent << *node;
    }
    return output.str();
}

}

