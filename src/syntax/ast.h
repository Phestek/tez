#ifndef TEZ_AST_H
#define TEZ_AST_H

#include <sstream>
#include <memory>
#include <vector>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

namespace tez {

struct C_Codegen_Data {
    std::string print_indent() const { return std::string(indent_level, '\t'); }
    std::size_t indent_level = 0;
};

struct LLVM_Codegen_Data {
    LLVM_Codegen_Data() : builder{context} {}
    llvm::LLVMContext             context;
    llvm::IRBuilder<>             builder{context};
    std::unique_ptr<llvm::Module> module{std::make_unique<llvm::Module>("Main_Module", context)};
};

enum class Ast_Node_Type {
    UNDEFINED,

    FILE,

    BLOCK,

    NAMESPACE,
    USING,

    BOOLEAN,
    INTEGER,
    REAL_NUMBER,
    STRING,

    IDENTIFIER,

    UNARY_OPERATION,
    BINARY_OPERATION,
    GROUPING_EXPRESSION,
    CAST,

    FUNCTION_DECLARATION,
    METHOD_DECLARATION,
    FUNCTION_CALL,
    RETURN,
    VARIABLE_DECLARATION,

    IF,
    WHILE,
    DO_WHILE,
    FOR,
    BREAK,
    CONTINUE,

    STRUCT,
    ENUM,
    UNION,

    ARRAY_ACCESS,
    MEMBER_ACCESS,
    SCOPE_RESOLUTION,
    PTR_DEREFERENCE,
    ADDRESS_OF,

    NEW,
    FREE,
    NULL_,
    
    ARRAY_INITIALIZER,

    // These 2 are only used for types.
    POINTER,
    ARRAY,

    INLINE_ASM,
};

// Base class for all AST nodes.
struct Ast_Node {
    virtual ~Ast_Node() = default;
    Ast_Node_Type node_type = Ast_Node_Type::UNDEFINED;
    // This 2 methods should be pure virtual, but because they are implemented yet in all of the nodes, it's the only
    // way to force compiler to compile it.
    virtual std::string generate_c([[maybe_unused]] C_Codegen_Data& codegen_data) const { return ""; };
    virtual llvm::Value* llvm_codegen([[maybe_unused]] LLVM_Codegen_Data& codegen_data) const { return nullptr; };
};
using Ast_Node_Ptr = std::unique_ptr<Ast_Node>;

struct Ast_File final : Ast_Node {
    Ast_File() { node_type = Ast_Node_Type::FILE; }
    std::string generate_c(C_Codegen_Data& data) const override;
    std::vector<Ast_Node_Ptr> statements;
};
using Ast = std::vector<Ast_File>;

struct Ast_Block final : Ast_Node {
    Ast_Block() { node_type = Ast_Node_Type::BLOCK; }
    std::string generate_c(C_Codegen_Data& codegen_data) const;
    std::vector<Ast_Node_Ptr> statements;
};

struct Ast_Namespace final : Ast_Node {
    Ast_Namespace() { node_type = Ast_Node_Type::NAMESPACE; }
    std::string name;
    Ast_Block   body;
};

struct Ast_Using final : Ast_Node {
    Ast_Using() { node_type = Ast_Node_Type::USING; }
    std::string nspace;
    std::string alias;
};

struct Ast_Boolean final : Ast_Node {
    Ast_Boolean() { node_type = Ast_Node_Type::BOOLEAN; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    llvm::Value* llvm_codegen(LLVM_Codegen_Data& codegen_data) const override;
    bool value;
};

struct Ast_Integer final : Ast_Node {
    Ast_Integer() { node_type = Ast_Node_Type::INTEGER; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    llvm::Value* llvm_codegen(LLVM_Codegen_Data& codegen_data) const override;
    int64_t value;
};

struct Ast_Real_Number final : Ast_Node {
    Ast_Real_Number() { node_type = Ast_Node_Type::REAL_NUMBER; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    llvm::Value* llvm_codegen(LLVM_Codegen_Data& codegen_data) const override;
    double value;
};

struct Ast_String final : Ast_Node {
    Ast_String() { node_type = Ast_Node_Type::STRING; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    std::string value;
};

struct Ast_Identifier final : Ast_Node {
    Ast_Identifier() { node_type = Ast_Node_Type::IDENTIFIER; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    std::string name;
};

struct Ast_Unary_Operation final : Ast_Node {
    Ast_Unary_Operation() { node_type = Ast_Node_Type::UNARY_OPERATION; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    Ast_Node_Ptr left;
    std::string  operat;
};

struct Ast_Binary_Operation final : Ast_Node {
    Ast_Binary_Operation() { node_type = Ast_Node_Type::BINARY_OPERATION; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    llvm::Value* llvm_codegen(LLVM_Codegen_Data& codegen_data) const override;
    Ast_Node_Ptr left;
    Ast_Node_Ptr right;
    std::string  operat;
};

struct Ast_Grouping_Expression final : Ast_Node {
    Ast_Grouping_Expression() { node_type = Ast_Node_Type::GROUPING_EXPRESSION; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    Ast_Node_Ptr expr;
};

struct Ast_Cast final : Ast_Node {
    Ast_Cast() { node_type = Ast_Node_Type::CAST; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    Ast_Node_Ptr expr;      // Expression to cast.
    Ast_Node_Ptr to;        // Type to cast to.
};

struct Ast_Address_Of final : Ast_Node {
    Ast_Address_Of() { node_type = Ast_Node_Type::ADDRESS_OF; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    Ast_Node_Ptr expr;
};

struct Ast_Func_Decl : Ast_Node {
    struct Param {
        std::string  name;
        bool         constant;
        Ast_Node_Ptr type;
    };
    Ast_Func_Decl() { node_type = Ast_Node_Type::FUNCTION_DECLARATION; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    llvm::Value* llvm_codegen(LLVM_Codegen_Data& codegen_data) const override;
    std::string        parent;  // Free function if parent == "".
    std::string        name;
    std::vector<Param> params;
    Ast_Node_Ptr       return_type;
    Ast_Block          body;
};

struct Ast_Return final : Ast_Node {
    Ast_Return() { node_type = Ast_Node_Type::RETURN; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    Ast_Node_Ptr expr;
};

struct Ast_Func_Call final : Ast_Node {
    Ast_Func_Call() { node_type = Ast_Node_Type::FUNCTION_CALL; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    llvm::Value* llvm_codegen(LLVM_Codegen_Data& codegen_data) const override;
    std::string               name;
    std::vector<Ast_Node_Ptr> args;
};

struct Ast_Var_Decl final : Ast_Node {
    Ast_Var_Decl() { node_type = Ast_Node_Type::VARIABLE_DECLARATION; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    std::string  name;
    bool         constant;
    Ast_Node_Ptr type;
    Ast_Node_Ptr initializer;
};

struct Ast_If final : Ast_Node {
    Ast_If() { node_type = Ast_Node_Type::IF; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    Ast_Node_Ptr condition;
    Ast_Block    if_block;
    Ast_Node_Ptr else_block;
};

struct Ast_While final : Ast_Node {
    Ast_While() { node_type = Ast_Node_Type::WHILE; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    Ast_Node_Ptr condition;
    Ast_Block    body;
};

struct Ast_Do_While final : Ast_Node {
    Ast_Do_While() { node_type = Ast_Node_Type::DO_WHILE; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    Ast_Node_Ptr condition;
    Ast_Block    body;
};

struct Ast_For final : Ast_Node {
    Ast_For() { node_type = Ast_Node_Type::FOR; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    Ast_Node_Ptr init_statement;
    Ast_Node_Ptr condition;
    Ast_Node_Ptr iteration_expr;
    Ast_Block    body;
};

struct Ast_Break final : Ast_Node {
    Ast_Break() { node_type = Ast_Node_Type::BREAK; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
};

struct Ast_Continue final : Ast_Node {
    Ast_Continue() { node_type = Ast_Node_Type::CONTINUE; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
};

struct Ast_Struct final : Ast_Node {
    struct Field {
        std::string  name;
        Ast_Node_Ptr type;
    };
    Ast_Struct() { node_type = Ast_Node_Type::STRUCT; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    std::string        name;
    std::vector<Field> fields;
};

struct Ast_Enum final : Ast_Node {
    struct Enumerator {
        std::string name;
        int         value;
    };
    Ast_Enum() { node_type = Ast_Node_Type::ENUM; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    std::string             name;
    std::vector<Enumerator> enumerations;
};

struct Ast_Union_Decl final : Ast_Node {
    struct Member {
        std::string  name;
        Ast_Node_Ptr type;
    };
    Ast_Union_Decl() { node_type = Ast_Node_Type::UNION; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    std::string         name;
    std::vector<Member> members;
};

struct Ast_Array_Access final : Ast_Node {
    Ast_Array_Access() { node_type = Ast_Node_Type::ARRAY_ACCESS; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    Ast_Node_Ptr array;
    Ast_Node_Ptr at;
};

struct Ast_Member_Access final : Ast_Node {
    Ast_Member_Access() { node_type = Ast_Node_Type::MEMBER_ACCESS; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    Ast_Node_Ptr left;
    Ast_Node_Ptr right;
};

struct Ast_Scope_Resolution final : Ast_Node {
    Ast_Scope_Resolution() { node_type = Ast_Node_Type::SCOPE_RESOLUTION; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    Ast_Node_Ptr left;
    Ast_Node_Ptr right;
};

struct Ast_Ptr_Dereference final : Ast_Node {
    Ast_Ptr_Dereference() { node_type = Ast_Node_Type::PTR_DEREFERENCE; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    Ast_Node_Ptr expr;
};

struct Ast_New final : Ast_Node {
    Ast_New() { node_type = Ast_Node_Type::NEW; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    Ast_Node_Ptr type;
};

struct Ast_Free final : Ast_Node {
    Ast_Free() { node_type = Ast_Node_Type::FREE; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    Ast_Node_Ptr what;
};

struct Ast_Null final : Ast_Node {
    Ast_Null() { node_type = Ast_Node_Type::NULL_; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
};

struct Ast_Array_Initializer final : Ast_Node {
    Ast_Array_Initializer() { node_type = Ast_Node_Type::ARRAY_INITIALIZER; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    std::vector<Ast_Node_Ptr> values;
};

struct Ast_Pointer final : Ast_Node {
    Ast_Pointer() { node_type = Ast_Node_Type::POINTER; };
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    Ast_Node_Ptr expr;
};

struct Ast_Array final : Ast_Node {
    Ast_Array() { node_type = Ast_Node_Type::ARRAY; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    Ast_Node_Ptr type;
    Ast_Node_Ptr size;
};

struct Ast_Inline_Asm final : Ast_Node {
    Ast_Inline_Asm() { node_type = Ast_Node_Type::INLINE_ASM; }
    std::string generate_c(C_Codegen_Data& codegen_data) const override;
    std::vector<std::string> operations;
};

}

#endif //TEZ_AST_H

