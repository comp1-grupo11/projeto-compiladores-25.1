#ifndef AST_H
#define AST_H

typedef enum {
    // Declarações
    NODE_FUNCTION,
    NODE_STRUCT_DEF,
    NODE_VARIABLE_DECL,
    
    // Expressões
    NODE_LITERAL,
    NODE_VARIABLE,
    NODE_BINOP,
    NODE_ASSIGN,
    NODE_CALL,
    
    // Controle de Fluxo
    NODE_IF,
    NODE_SWITCH,
    NODE_CASE,
    NODE_FOR,
    NODE_WHILE,
    NODE_RETURN,
    NODE_BREAK,
    NODE_CONTINUE,
    
    // Estruturas
    NODE_MEMBER_ACCESS
} NodeType;

typedef enum {
    DT_NUMBER,    // int/float -> number
    DT_STRING,    // char/char[] -> string
    DT_BOOLEAN,   // bool -> boolean
    DT_STRUCT,    // struct -> type
    DT_VOID       // void
} DataType;

typedef struct ASTNode ASTNode;

struct ASTNode {
    NodeType type;
    DataType data_type;
    char *type_name;  // Para structs
    int line;         // Linha no código fonte

    union {
        // Função
        struct {
            char *name;
            ASTNode *params;
            ASTNode *body;
        } function;

        // Struct
        struct {
            char *name;
            ASTNode *fields;
        } struct_def;

        // Variável
        struct {
            char *name;
            ASTNode *init_value;
        } variable;

        // Literal
        struct {
            union {
                int int_val;
                float float_val;
                char *str_val;
            };
        } literal;

        // Operação Binária
        struct {
            char *op;
            ASTNode *left;
            ASTNode *right;
        } binop;

        // Chamada de Função
        struct {
            char *func_name;
            ASTNode *args;
        } call;

        // Switch
        struct {
            ASTNode *expr;
            ASTNode *cases;
            ASTNode *default_case;
        } switch_stmt;

        // Case
        struct {
            ASTNode *value;
            ASTNode *body;
        } case_stmt;

        // Acesso a membro (struct.member)
        struct {
            ASTNode *object;
            char *member;
        } member_access;
    };
};

// Funções de Criação
ASTNode* create_function_node(char *name, ASTNode *params, ASTNode *body);
ASTNode* create_struct_def_node(char *name, ASTNode *fields);
ASTNode* create_variable_node(char *name, DataType dtype, char *type_name, ASTNode *init_value);
ASTNode* create_literal_number(int value);
ASTNode* create_literal_string(char *value);
ASTNode* create_binop_node(char *op, ASTNode *left, ASTNode *right);
ASTNode* create_call_node(char *func_name, ASTNode *args);
ASTNode* create_switch_node(ASTNode *expr, ASTNode *cases, ASTNode *default_case);
ASTNode* create_case_node(ASTNode *value, ASTNode *body);
ASTNode* create_member_access_node(ASTNode *object, char *member);

// Utilitários
void free_ast(ASTNode *node);
void print_ast(ASTNode *node, int indent);

#endif