#include "ast.h"

#include <sstream>

namespace tez {

namespace {

std::string print_c_statement(const Ast_Node_Ptr& node, C_Codegen_Data& codegen_data) {
    std::stringstream result;
    result << codegen_data.print_indent() << node->generate_c(codegen_data);
    if(node->node_type != Ast_Node_Type::BLOCK
            && node->node_type != Ast_Node_Type::NAMESPACE
            && node->node_type != Ast_Node_Type::FUNCTION_DECLARATION
            && node->node_type != Ast_Node_Type::IF
            && node->node_type != Ast_Node_Type::WHILE
            && node->node_type != Ast_Node_Type::FOR) {
        result << ';';
    }
    result << '\n';
    return result.str();
}

std::string print_c_array(const Ast_Node& node, C_Codegen_Data& codegen_data) {
    std::stringstream code;
    if(node.node_type == Ast_Node_Type::POINTER) {
        const auto& ptr = dynamic_cast<const Ast_Pointer&>(node);
        code << print_c_array(*ptr.expr, codegen_data);
    }
    if(node.node_type == Ast_Node_Type::ARRAY) {
        const auto& array = dynamic_cast<const Ast_Array&>(node);
        code << "[";
        if(array.size != nullptr) {
            code << array.size->generate_c(codegen_data);
        }
        code << "]" << print_c_array(*array.type, codegen_data);
    }
    return code.str();
}

}

std::string Ast_File::generate_c(C_Codegen_Data& codegen_data) const {
    std::stringstream code;
    for(const auto& statement : statements) {
        code << print_c_statement(statement, codegen_data);
    }
    return code.str();
}

std::string Ast_Block::generate_c(C_Codegen_Data& codegen_data) const {
    std::stringstream code;
    code << " {\n";
    ++codegen_data.indent_level;
    for(const auto& stmt : statements) {
        code << print_c_statement(stmt, codegen_data);
    }
    --codegen_data.indent_level;
    code << codegen_data.print_indent() << "}";
    return code.str();
}

std::string Ast_Boolean::generate_c([[maybe_unused]] C_Codegen_Data& codegen_data) const {
    return std::to_string(value);
}

llvm::Value* Ast_Boolean::generate_llvm(LLVM_Codegen_Data& codegen_data) const {
    return llvm::ConstantInt::get(codegen_data.context, llvm::APInt{8, static_cast<uint64_t>(value), false});
}

std::string Ast_Integer::generate_c([[maybe_unused]] C_Codegen_Data& codegen_data) const {
    return std::to_string(value);
}

llvm::Value* Ast_Integer::generate_llvm(LLVM_Codegen_Data& codegen_data) const {
    return llvm::ConstantInt::get(codegen_data.context, llvm::APInt{64, static_cast<uint64_t>(value), true});
}

std::string Ast_Real_Number::generate_c([[maybe_unused]] C_Codegen_Data& codegen_data) const {
    return std::to_string(value);
}

llvm::Value* Ast_Real_Number::generate_llvm(LLVM_Codegen_Data& codegen_data) const {
    return llvm::ConstantFP::get(codegen_data.context, llvm::APFloat{value});
}

std::string Ast_String::generate_c([[maybe_unused]] C_Codegen_Data& codegen_data) const {
    return "\"" + value + "\"";
}

std::string Ast_Identifier::generate_c([[maybe_unused]] C_Codegen_Data& codegen_data) const {
    return name;
}

std::string Ast_Unary_Operation::generate_c(C_Codegen_Data& codegen_data) const {
    return operat + left->generate_c(codegen_data);
}

std::string Ast_Binary_Operation::generate_c(C_Codegen_Data& codegen_data) const {
    return left->generate_c(codegen_data) + " " + operat + " " + right->generate_c(codegen_data);
}

llvm::Value* Ast_Binary_Operation::generate_llvm(LLVM_Codegen_Data& codegen_data) const {
    auto l = left->generate_llvm(codegen_data);
    auto r = right->generate_llvm(codegen_data);
    if(l == nullptr || r == nullptr) {
        // TODO: Return nullptr smells bad.
        return nullptr;
    }
    if(operat == "+") {
        return codegen_data.builder.CreateFAdd(l, r, "tmpadd");
    }
    if(operat == "-") {
        return codegen_data.builder.CreateFSub(l, r, "tmpsub");
    }
    if(operat == "*") {
        return codegen_data.builder.CreateFMul(l, r, "tmpmul");
    }
    if(operat == "/") {
        return codegen_data.builder.CreateFDiv(l, r, "tmpdiv");
    }
    // We won't ever reach this point, because parser guarantees operator
    // correctness.
    return nullptr;
}

std::string Ast_Grouping_Expression::generate_c(C_Codegen_Data& codegen_data) const {
    return "(" + expr->generate_c(codegen_data) + ")";
}

std::string Ast_Cast::generate_c(C_Codegen_Data& codegen_data) const {
    return "(" + to->generate_c(codegen_data) + ")" + expr->generate_c(codegen_data);
}

std::string Ast_Address_Of::generate_c(C_Codegen_Data& codegen_data) const {
    return "&" + expr->generate_c(codegen_data);
}

std::string Ast_Func_Decl::generate_c(C_Codegen_Data& codegen_data) const {
    std::stringstream code;
    code << return_type->generate_c(codegen_data) << ' ' << name << '(';
    for(std::size_t i = 0; i < params.size(); ++i) {
        const auto& param = params[i];
        code << param.type->generate_c(codegen_data) << ' ' << param.name;
        if(i < params.size() - 1) {
            code << ", ";
        }
    }
    code << ')' << body.generate_c(codegen_data);
    return code.str();
}

llvm::Value* Ast_Func_Decl::generate_llvm([[maybe_unused]] LLVM_Codegen_Data& codegen_data) const {
    return nullptr;
}

std::string Ast_Return::generate_c(C_Codegen_Data& codegen_data) const {
    return "return " + expr->generate_c(codegen_data);
}

std::string Ast_Func_Call::generate_c(C_Codegen_Data& codegen_data) const {
    std::stringstream code;
    code << name << '(';
    for(std::size_t i = 0; i < args.size(); ++i) {
        code << args[i]->generate_c(codegen_data);
        if(i < args.size() - 1) {
            code << ", ";
        }
    }
    code << ')';
    return code.str();
}

llvm::Value* Ast_Func_Call::generate_llvm(LLVM_Codegen_Data& codegen_data) const {
    auto func = codegen_data.module->getFunction(name);
    if(func == nullptr) {
        return nullptr;
    }
    if(func->arg_size() != args.size()) {
        return nullptr;
    }
    std::vector<llvm::Value*> gen_args;
    for(const auto& arg : args) {
        gen_args.push_back(arg->generate_llvm(codegen_data));
        if(!args.back()) {
            return nullptr;
        }
    }
    return codegen_data.builder.CreateCall(func, gen_args, "calltmp");
}

std::string Ast_Var_Decl::generate_c(C_Codegen_Data& codegen_data) const {
    std::stringstream code;
    if(constant) {
        code << "const ";
    }
    code << type->generate_c(codegen_data) << ' ' << name << print_c_array(*type, codegen_data);
    if(initializer != nullptr) {
        code << " = " << initializer->generate_c(codegen_data);
    }
    return code.str();
}

std::string Ast_If::generate_c(C_Codegen_Data& codegen_data) const {
    std::stringstream code;
    code << "if(" << condition->generate_c(codegen_data) << ')' << if_block.generate_c(codegen_data);
    if(else_block != nullptr) {
        code << " else" << else_block->generate_c(codegen_data);
    }
    return code.str();
}

std::string Ast_While::generate_c(C_Codegen_Data& codegen_data) const {
    return "while(" + condition->generate_c(codegen_data) + ")" + body.generate_c(codegen_data);
}

std::string Ast_Do_While::generate_c(C_Codegen_Data& codegen_data) const {
    return "do" + body.generate_c(codegen_data) + " while(" + condition->generate_c(codegen_data) + ")";
}

std::string Ast_For::generate_c(C_Codegen_Data& codegen_data) const {
    return "for(" + init_statement->generate_c(codegen_data) + "; " + condition->generate_c(codegen_data) + "; "
            + iteration_expr->generate_c(codegen_data) + ")" + body.generate_c(codegen_data);
}

std::string Ast_Break::generate_c([[maybe_unused]] C_Codegen_Data& codegen_data) const {
    return "break";
}

std::string Ast_Continue::generate_c([[maybe_unused]] C_Codegen_Data& codegen_data) const {
    return "continue";
}

std::string Ast_Struct::generate_c(C_Codegen_Data& codegen_data) const {
    std::stringstream code;
    code << "typedef struct {\n";
    ++codegen_data.indent_level;
    for(const auto& member : fields) {
        code << codegen_data.print_indent() << member.type->generate_c(codegen_data) << ' ' << member.name
                << print_c_array(*member.type, codegen_data) << ";\n";
    }
    --codegen_data.indent_level;
    code << "} " << name;
    return code.str();
}

std::string Ast_Enum::generate_c(C_Codegen_Data& codegen_data) const {
    std::stringstream code;
    code << "typedef enum {\n";
    ++codegen_data.indent_level;
    for(const auto& en : enumerations) {
        code << codegen_data.print_indent() << en.name << " = " << en.value << ";\n";
    }
    --codegen_data.indent_level;
    code << "} " << name;
    return code.str();
}

std::string Ast_Union_Decl::generate_c(C_Codegen_Data& codegen_data) const {
    std::stringstream code;
    code << "typedef union {\n";
    ++codegen_data.indent_level;
    for(const auto& member : members) {
        code << codegen_data.print_indent() << member.type->generate_c(codegen_data) << ' ' << member.name
                << print_c_array(*member.type, codegen_data) << ";\n";
    }
    --codegen_data.indent_level;
    code << "} " << name;
    return code.str();
}

std::string Ast_Array_Access::generate_c(C_Codegen_Data& codegen_data) const {
    return array->generate_c(codegen_data) + "[" + at->generate_c(codegen_data) + "]";
}

std::string Ast_Member_Access::generate_c(C_Codegen_Data& codegen_data) const {
    return left->generate_c(codegen_data) + "." + right->generate_c(codegen_data);
}

std::string Ast_Scope_Resolution::generate_c([[maybe_unused]] C_Codegen_Data& codegen_data) const {
    return "";
}

std::string Ast_Ptr_Dereference::generate_c([[maybe_unused]] C_Codegen_Data& codegen_data) const {
    return "(*" + expr->generate_c(codegen_data) + ")";
}

std::string Ast_New::generate_c(C_Codegen_Data& codegen_data) const {
    return "malloc(sizeof(" + type->generate_c(codegen_data) + ")";
}

std::string Ast_Free::generate_c(C_Codegen_Data& codegen_data) const {
    return "free(" + what->generate_c(codegen_data) + ")";
}

std::string Ast_Null::generate_c([[maybe_unused]] C_Codegen_Data& codegen_data) const {
    return "NULL";
}

std::string Ast_Array_Initializer::generate_c(C_Codegen_Data& codegen_data) const {
    std::stringstream code;
    code << '{';
    for(const auto& value : values) {
        code << value->generate_c(codegen_data) << ", ";
    }
    code << '}';
    return code.str();
}

std::string Ast_Pointer::generate_c([[maybe_unused]] C_Codegen_Data& codegen_data) const {
    return expr->generate_c(codegen_data) + "*";
}

std::string Ast_Array::generate_c([[maybe_unused]] C_Codegen_Data& codegen_data) const {
    return type->generate_c(codegen_data);
}

std::string Ast_Inline_Asm::generate_c([[maybe_unused]] C_Codegen_Data& codegen_data) const {
    std::stringstream code{"asm(\n"};
    ++codegen_data.indent_level;
    for(const auto& op : operations) {
        code << codegen_data.print_indent() << "\"" << op << "\"" << "\n";
    }
    --codegen_data.indent_level;
    code << codegen_data.print_indent() << ')';
    return code.str();
}

}

