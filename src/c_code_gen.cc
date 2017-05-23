#include "c_code_gen.h"

#include <ostream>
#include <sstream>

namespace wayward {

namespace {

std::ostream& operator<<(std::ostream& out, const ast_node& node);
std::ostream& operator<<(std::ostream& out, const ast_binary_operation& bin);
std::ostream& operator<<(std::ostream& out, const ast_integer& integer);

std::ostream& operator<<(std::ostream& out, const ast_node& node) {
    switch(node.node_type) {
        case ast_node_type::binary_operation:
            return out << dynamic_cast<const ast_binary_operation&>(node);
        case ast_node_type::integer:
            return out << dynamic_cast<const ast_integer&>(node);
        default:
            return out << "<not implemented>";
    }
}

std::ostream& operator<<(std::ostream& out, const ast_binary_operation& bin) {
    return out << *bin.left << ' ' << bin.operat << ' ' << *bin.right;
}

std::ostream& operator<<(std::ostream& out, const ast_integer& integer) {
    return out << integer.value;
}

}

std::string generate_c_code(const std::vector<ast_node_ptr>& ast) {
    std::stringstream output;
    for(const auto& node : ast) {
        output << *node << ";\n";
    }
    return output.str();
}

}

