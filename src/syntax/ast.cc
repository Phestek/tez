#include "ast.h"

namespace tez {

llvm::Value* Ast_Boolean::llvm_codegen(LLVM_Codegen_Data& codegen_data) const {
    return llvm::ConstantInt::get(
            codegen_data.context,
            llvm::APInt{8, static_cast<uint64_t>(value), false});
}

llvm::Value* Ast_Integer::llvm_codegen(LLVM_Codegen_Data& codegen_data) const {
    return llvm::ConstantInt::get(
            codegen_data.context,
            llvm::APInt{64, static_cast<uint64_t>(value), true});
}

llvm::Value* Ast_Real_Number::llvm_codegen(
        LLVM_Codegen_Data& codegen_data) const {
    return llvm::ConstantFP::get(codegen_data.context, llvm::APFloat{value});
}

llvm::Value* Ast_Binary_Operation::llvm_codegen(
        LLVM_Codegen_Data& codegen_data) const {
    auto l = dynamic_cast<Ast_LLVM_Generable&>(*left).llvm_codegen(codegen_data);
    auto r = dynamic_cast<Ast_LLVM_Generable&>(*right).llvm_codegen(codegen_data);
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

llvm::Value* Ast_Func_Decl::llvm_codegen(
        [[maybe_unused]] LLVM_Codegen_Data& codegen_data) const {
    return nullptr;
}

llvm::Value* Ast_Func_Call::llvm_codegen(
        LLVM_Codegen_Data& codegen_data) const {
    auto func = codegen_data.module->getFunction(name);
    if(func == nullptr) {
        return nullptr;
    }
    if(func->arg_size() != args.size()) {
        return nullptr;
    }
    std::vector<llvm::Value*> gen_args;
    for(const auto& arg : args) {
        gen_args.push_back(static_cast<Ast_LLVM_Generable&>(*arg)
                .llvm_codegen(codegen_data));
        if(!args.back()) {
            return nullptr;
        }
    }
    return codegen_data.builder.CreateCall(func, gen_args, "calltmp");
}

}
