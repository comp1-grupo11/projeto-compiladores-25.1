// ast.h

#ifndef AST_H
#define AST_H

// Enumerações para tipos de dados
typedef enum
{
    TIPO_INT,
    TIPO_FLOAT,
    TIPO_DOUBLE,
    TIPO_CHAR,
    TIPO_STRING,
    TIPO_VOID,
    TIPO_OBJETO, // Adicionado para suporte a structs
    TIPO_ERRO    // Para representar nós que resultam de erros semânticos/sintáticos
} Tipo;

const char *nomeTipo(Tipo tipo);

// Enumeração para o tipo de nó na AST
typedef enum
{
    NODE_OPERATOR,
    NODE_LITERAL,
    NODE_IDENTIFIER,
    NODE_ERROR,
    NODE_DECLARATION,   // Para declarações de variáveis
    NODE_RETURN,        // Para o comando 'return'
    NODE_IF,            // Para o comando 'if'
    NODE_WHILE,         // Para o comando 'while'
    NODE_FOR,           // Para o comando 'for'
    NODE_SWITCH,        // Para o comando 'switch' (o nó principal)
    NODE_SWITCH_BODY,   // O corpo interno do switch (pode ser o que $5 retorna)
    NODE_CASE,          // Para cada 'case' individual
    NODE_DEFAULT,       // Para o 'default' do switch
    NODE_COMPOUND_STMT, // Para blocos de código ({ ... })
    NODE_BREAK,         // Para o comando 'break'
    NODE_CONTINUE,      // Para o comando 'continue'
    NODE_FUNCTION_CALL, // Para chamadas de função
    NODE_UNARY_OP,      // Para operadores unários (++, --, !)
    NODE_PROGRAM,       // Nó raiz que pode conter uma lista de funções/globais
    NODE_FUNCTION_DEF,  // Definição de função
    NODE_PARAM_LIST,    // Lista de parâmetros de função
    NODE_ARG_LIST,      // Lista de argumentos em chamadas de função
    NODE_FIELD_ASSIGN,  // Para atribuição a campo de struct (ex: p.x = 1)
    NODE_FIELD_ACCESS   // Para acesso a campo de struct (ex: p.x)
} NodeType;

// Enumeração para os operadores
typedef enum
{
    OP_ADD_TYPE,
    OP_SUB_TYPE,
    OP_MUL_TYPE,
    OP_DIV_TYPE,
    OP_MOD_TYPE,
    OP_ASSIGN_TYPE,
    OP_ADD_ASSIGN_TYPE,
    OP_SUB_ASSIGN_TYPE,
    OP_MUL_ASSIGN_TYPE,
    OP_DIV_ASSIGN_TYPE,
    OP_MOD_ASSIGN_TYPE,
    OP_EQ_TYPE,
    OP_NE_TYPE,
    OP_LT_TYPE,
    OP_LE_TYPE,
    OP_GT_TYPE,
    OP_GE_TYPE,
    OP_AND_TYPE,
    OP_OR_TYPE,
    OP_NOT_TYPE,
    OP_INC_TYPE,
    OP_DEC_TYPE,
    OP_INDEX_TYPE, // Adicionado para acesso a arrays
    OP_UNKNOWN_TYPE
} OperatorType;

// Estrutura do nó da Árvore Sintática Abstrata (AST)
typedef struct NoAST
{
    NodeType tipo_no;
    Tipo tipo_dado;
    int linha;

    union
    {
        OperatorType op_type;
        struct
        {
            int val_int;
            float val_float;
            double val_double;
            char val_char;
            char *val_string;
        } literal;

        char nome_id[32];

        struct
        {
            char *nome_declaracao;
            struct NoAST *inicializacao_expr;
        } decl_info;

        char *func_name;

        // Para NODE_FIELD_ASSIGN e NODE_FIELD_ACCESS
        struct
        {
            char campo_nome[32];
        } field_info;

    } data;

    struct NoAST *esquerda;
    struct NoAST *direita;
    struct NoAST *centro;
    struct NoAST *proximo;

} NoAST;

// Funções de criação de nós
NoAST *criarNoOp(OperatorType op, NoAST *esq, NoAST *dir);
NoAST *criarNoUnario(OperatorType op, NoAST *operando);
NoAST *criarNoNum(Tipo tipo, void *valor);
NoAST *criarNoId(char *nome, Tipo tipo);
NoAST *criarNoChar(char val);
NoAST *criarNoString(char *val);
NoAST *criarNoErro();

// Funções para comandos (statements)
NoAST *criarNoDeclaracaoVar(char *nome, Tipo tipo_declarado, NoAST *inicializacao_expr);
NoAST *criarNoDeclaracaoVarArray(char *nome, Tipo tipo_declarado, NoAST *tamanho_expr);
NoAST *criarNoReturn(NoAST *expr_retorno);
NoAST *criarNoIf(NoAST *condicao, NoAST *bloco_then, NoAST *bloco_else); // bloco_else pode ser NULL
NoAST *criarNoWhile(NoAST *condicao, NoAST *bloco);
NoAST *criarNoFor(NoAST *init, NoAST *cond, NoAST *iter, NoAST *bloco);
NoAST *criarNoSwitch(NoAST *expr_controle, NoAST *corpo_switch);
NoAST *criarNoSwitchBody(NoAST *case_list_head, NoAST *default_node);
NoAST *criarNoCase(NoAST *case_expr, NoAST *statement_list);
NoAST *criarNoDefault(NoAST *statement_list);
NoAST *criarNoCompoundStmt(NoAST *lista_statements); // Para blocos {}
NoAST *criarNoBreak();
NoAST *criarNoContinue();
NoAST *criarNoChamadaFuncao(char *nome_func, NoAST *args_list, Tipo tipo_retorno);
NoAST *criarNoAtribuicaoCampo(NoAST *struct_expr, char *campo, NoAST *valor);
NoAST *criarNoAcessoCampo(NoAST *struct_expr, char *campo);
NoAST *criarNoFuncao(char *nome, Tipo tipo_retorno, NoAST *params, NoAST *corpo);

// Funções de manipulação da AST
void imprimirAST(NoAST *no);
void liberarAST(NoAST *no);
int tiposCompativeis(Tipo t1, Tipo t2);

#endif // AST_H