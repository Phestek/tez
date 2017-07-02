#ifndef TEZ_AST_H
#define TEZ_AST_H

#include <string>
#include <memory>
#include <vector>

namespace tez {

enum class Ast_Node_Type {
    UNDEFINED,
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
    UNION,  // Not implemented yet.
    CLASS,  // Not implemented yet.

    ARRAY_ACCESS,
    MEMBER_ACCESS,
    SCOPE_RESOLUTION,
    PTR_DEREFERENCE,
    ADDRESS_OF,

    NEW,
    FREE,
    NULL_,
    
    ARRAY_INITIALIZER,
    STRUCT_CONSTRUCTOR,

    // These 2 are only used for types.
    POINTER,
    ARRAY,

    INLINE_ASM,
};

struct Ast_Node {
    virtual ~Ast_Node() = default;
    Ast_Node_Type node_type = Ast_Node_Type::UNDEFINED;
};
using Ast_Node_Ptr = std::unique_ptr<Ast_Node>;
using Ast          = std::vector<Ast_Node_Ptr>;

struct Ast_Block final : Ast_Node {
    Ast_Block() { node_type = Ast_Node_Type::BLOCK; }
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
    bool value;
};

struct Ast_Integer final : Ast_Node {
    Ast_Integer() { node_type = Ast_Node_Type::INTEGER; }
    long long value;
};

struct Ast_Real_Number final : Ast_Node {
    Ast_Real_Number() { node_type = Ast_Node_Type::REAL_NUMBER; }
    double value;
};

struct Ast_String final : Ast_Node {
    Ast_String() { node_type = Ast_Node_Type::STRING; }
    std::string value;
};

struct Ast_Identifier final : Ast_Node {
    Ast_Identifier() { node_type = Ast_Node_Type::IDENTIFIER; }
    std::string name;
};

struct Ast_Unary_Operation final : Ast_Node {
    Ast_Unary_Operation() { node_type = Ast_Node_Type::UNARY_OPERATION; }
    Ast_Node_Ptr left;
    std::string  operat;
};

struct Ast_Binary_Operation final : Ast_Node {
    Ast_Binary_Operation() { node_type = Ast_Node_Type::BINARY_OPERATION; }
    Ast_Node_Ptr left;
    Ast_Node_Ptr right;
    std::string  operat;
};

struct Ast_Grouping_Expression final : Ast_Node {
    Ast_Grouping_Expression() { node_type = Ast_Node_Type::GROUPING_EXPRESSION; }
    Ast_Node_Ptr expr;
};

struct Ast_Cast final : Ast_Node {
    Ast_Cast() { node_type = Ast_Node_Type::CAST; }
    Ast_Node_Ptr expr;      // Expression to cast.
    Ast_Node_Ptr to;        // Type to cast to.
};

struct Ast_Address_Of final : Ast_Node {
    Ast_Address_Of() { node_type = Ast_Node_Type::ADDRESS_OF; }
    Ast_Node_Ptr expr;
};

struct Ast_Func_Decl : Ast_Node {
    struct Param {
        std::string  name;
        bool         constant;
        Ast_Node_Ptr type;
    };
    Ast_Func_Decl() { node_type = Ast_Node_Type::FUNCTION_DECLARATION; }
    std::string        parent;  // Free function if parent == "".
    std::string        name;
    std::vector<Param> params;
    Ast_Node_Ptr       return_type;
    Ast_Block          body;
};

struct Ast_Return final : Ast_Node {
    Ast_Return() { node_type = Ast_Node_Type::RETURN; }
    Ast_Node_Ptr expr;
};

struct Ast_Func_Call final : Ast_Node {
    Ast_Func_Call() { node_type = Ast_Node_Type::FUNCTION_CALL; }
    Ast_Node_Ptr              name;
    std::vector<Ast_Node_Ptr> args;
};

struct Ast_Var_Decl final : Ast_Node {
    Ast_Var_Decl() { node_type = Ast_Node_Type::VARIABLE_DECLARATION; }
    std::string  name;
    bool         constant;
    Ast_Node_Ptr type;
    Ast_Node_Ptr initializer;
};

struct Ast_If final : Ast_Node {
    Ast_If() { node_type = Ast_Node_Type::IF; }
    Ast_Node_Ptr condition;
    Ast_Block    if_block;
    Ast_Node_Ptr else_block;
};

struct Ast_While final : Ast_Node {
    Ast_While() { node_type = Ast_Node_Type::WHILE; }
    Ast_Node_Ptr condition;
    Ast_Block    body;
};

struct Ast_Do_While final : Ast_Node {
    Ast_Do_While() { node_type = Ast_Node_Type::DO_WHILE; }
    Ast_Node_Ptr condition;
    Ast_Block    body;
};

struct Ast_For final : Ast_Node {
    Ast_For() { node_type = Ast_Node_Type::FOR; }
    Ast_Node_Ptr init_statement;
    Ast_Node_Ptr condition;
    Ast_Node_Ptr iteration_expr;
    Ast_Block    body;
};

struct Ast_Break final : Ast_Node {
    Ast_Break() { node_type = Ast_Node_Type::BREAK; }
};

struct Ast_Continue final : Ast_Node {
    Ast_Continue() { node_type = Ast_Node_Type::CONTINUE; }
};

struct Ast_Struct final : Ast_Node {
    struct Field {
        std::string  name;
        Ast_Node_Ptr type;
    };
    Ast_Struct() { node_type = Ast_Node_Type::STRUCT; }
    std::string        name;
    std::vector<Field> fields;
};

struct Ast_Enum final : Ast_Node {
    struct Enumerator {
        std::string name;
        int         value;
    };
    Ast_Enum() { node_type = Ast_Node_Type::ENUM; }
    std::string             name;
    std::vector<Enumerator> enumerations;
};

struct Ast_Union_Decl final : Ast_Node {
    struct Member {
        std::string  name;
        Ast_Node_Ptr type;
    };
    Ast_Union_Decl() { node_type = Ast_Node_Type::UNION; }
    std::string         name;
    std::vector<Member> members;
};

struct Ast_Array_Access final : Ast_Node {
    Ast_Array_Access() { node_type = Ast_Node_Type::ARRAY_ACCESS; }
    Ast_Node_Ptr array;
    Ast_Node_Ptr at;
};

struct Ast_Member_Access final : Ast_Node {
    Ast_Member_Access() { node_type = Ast_Node_Type::MEMBER_ACCESS; }
    Ast_Node_Ptr left;
    Ast_Node_Ptr right;
};

struct Ast_Scope_Resolution final : Ast_Node {
    Ast_Scope_Resolution() { node_type = Ast_Node_Type::SCOPE_RESOLUTION; }
    Ast_Node_Ptr left;
    Ast_Node_Ptr right;
};

struct Ast_Ptr_Dereference final : Ast_Node {
    Ast_Ptr_Dereference() { node_type = Ast_Node_Type::PTR_DEREFERENCE; }
    Ast_Node_Ptr expr;
};

struct Ast_New final : Ast_Node {
    Ast_New() { node_type = Ast_Node_Type::NEW; }
    Ast_Node_Ptr type;
};

struct Ast_Free final : Ast_Node {
    Ast_Free() { node_type = Ast_Node_Type::FREE; }
    Ast_Node_Ptr what;
};

struct Ast_Null final : Ast_Node {
    Ast_Null() { node_type = Ast_Node_Type::NULL_; }
};

struct Ast_Array_Initializer final : Ast_Node {
    Ast_Array_Initializer() { node_type = Ast_Node_Type::ARRAY_INITIALIZER; }
    std::vector<Ast_Node_Ptr> values;
};

struct Ast_Struct_Constructor final : Ast_Node {
    Ast_Struct_Constructor() { node_type = Ast_Node_Type::STRUCT_CONSTRUCTOR; }
    Ast_Node_Ptr              struct_name;
    std::vector<Ast_Node_Ptr> declaration_list;
};

struct Ast_Pointer final : Ast_Node {
    Ast_Pointer() { node_type = Ast_Node_Type::POINTER; };
    Ast_Node_Ptr expr;
};

struct Ast_Array final : Ast_Node {
    Ast_Array() { node_type = Ast_Node_Type::ARRAY; }
    Ast_Node_Ptr type;
    Ast_Node_Ptr size;
};

struct Ast_Inline_Asm final : Ast_Node {
    Ast_Inline_Asm() { node_type = Ast_Node_Type::INLINE_ASM; }
    std::vector<std::string> operations;
};

}

#endif //TEZ_AST_H

