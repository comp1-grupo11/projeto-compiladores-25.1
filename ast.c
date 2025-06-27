// Função utilitária para copiar um nó isolado da AST (sem filhos)
// (deve ser definida após os typedefs e enums)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include <stddef.h>

// Protótipo da função auxiliar para alocar um nó base
static NoAST *alocarNoAST(NodeType tipo_no);

extern int yylineno; // Para obter o número da linha atual

// Função auxiliar para imprimir indentação
void imprimirIndentacao(int indent)
{
    for (int i = 0; i < indent; i++)
        printf("    ");
}

// Nova função de criação de nó de declaração (modelo sugerido)
NoAST *criarNoDeclaracao(Tipo tipo, char *nome, NoAST *init)
{
    NoAST *no = alocarNoAST(NODE_DECLARATION);
    no->tipo_dado = tipo;
    no->data.decl_info.nome_declaracao = strdup(nome);
    if (init)
    {
        no->direita = init;
    }
    return no;
}
// Versão indentada da impressão da AST
void imprimirASTComIndent(NoAST *no, int indent)
{
    if (no == NULL)
    {
        return;
    }

    switch (no->tipo_no)
    {
    case NODE_OPERATOR:
        if (no->data.op_type == OP_NOT_TYPE || no->data.op_type == OP_INC_TYPE || no->data.op_type == OP_DEC_TYPE)
        {
            imprimirIndentacao(indent);
            printf("(%s",
                   (no->data.op_type == OP_NOT_TYPE) ? "!" : (no->data.op_type == OP_INC_TYPE) ? "++"
                                                         : (no->data.op_type == OP_DEC_TYPE)   ? "--"
                                                                                               : "?");
            imprimirASTComIndent(no->esquerda, indent);
            printf(")");
        }
        else
        {
            imprimirIndentacao(indent);
            printf("(");
            imprimirASTComIndent(no->esquerda, indent);
            printf(" %s ",
                   (no->data.op_type == OP_ADD_TYPE) ? "+" : (no->data.op_type == OP_SUB_TYPE)      ? "-"
                                                         : (no->data.op_type == OP_MUL_TYPE)        ? "*"
                                                         : (no->data.op_type == OP_DIV_TYPE)        ? "/"
                                                         : (no->data.op_type == OP_ASSIGN_TYPE)     ? "="
                                                         : (no->data.op_type == OP_EQ_TYPE)         ? "=="
                                                         : (no->data.op_type == OP_NE_TYPE)         ? "!="
                                                         : (no->data.op_type == OP_LT_TYPE)         ? "<"
                                                         : (no->data.op_type == OP_LE_TYPE)         ? "<="
                                                         : (no->data.op_type == OP_GT_TYPE)         ? ">"
                                                         : (no->data.op_type == OP_GE_TYPE)         ? ">="
                                                         : (no->data.op_type == OP_AND_TYPE)        ? "&&"
                                                         : (no->data.op_type == OP_OR_TYPE)         ? "||"
                                                         : (no->data.op_type == OP_ADD_ASSIGN_TYPE) ? "+="
                                                         : (no->data.op_type == OP_SUB_ASSIGN_TYPE) ? "-="
                                                         : (no->data.op_type == OP_MUL_ASSIGN_TYPE) ? "*="
                                                         : (no->data.op_type == OP_DIV_ASSIGN_TYPE) ? "/="
                                                         : (no->data.op_type == OP_MOD_ASSIGN_TYPE) ? "%="
                                                                                                    : "?");
            imprimirASTComIndent(no->direita, indent);
            printf(")");
        }
        break;

    case NODE_LITERAL:
        imprimirIndentacao(indent);
        switch (no->tipo_dado)
        {
        case TIPO_INT:
            printf("%d", no->data.literal.val_int);
            break;
        case TIPO_FLOAT:
            printf("%f", no->data.literal.val_float);
            break;
        case TIPO_DOUBLE:
            printf("%lf", no->data.literal.val_double);
            break;
        case TIPO_CHAR:
            printf("'%c'", no->data.literal.val_char);
            break;
        case TIPO_STRING:
            printf("\"%s\"", no->data.literal.val_string);
            break;
        default:
            printf("[LITERAL DESCONHECIDO]");
            break;
        }
        break;

    case NODE_IDENTIFIER:
        imprimirIndentacao(indent);
        printf("%s", no->data.nome_id);
        break;

    case NODE_DECLARATION:
        imprimirIndentacao(indent);
        printf("%s %s",
               (no->tipo_dado == TIPO_INT) ? "int" : (no->tipo_dado == TIPO_FLOAT) ? "float"
                                                 : (no->tipo_dado == TIPO_DOUBLE)  ? "double"
                                                 : (no->tipo_dado == TIPO_CHAR)    ? "char"
                                                 : (no->tipo_dado == TIPO_STRING)  ? "string"
                                                                                   : "tipo_desconhecido",
               no->data.decl_info.nome_declaracao);
        if (no->data.decl_info.inicializacao_expr)
        {
            printf(" = ");
            imprimirASTComIndent(no->data.decl_info.inicializacao_expr, indent);
        }
        printf(";");
        break;

    case NODE_RETURN:
        imprimirIndentacao(indent);
        printf("return ");
        imprimirASTComIndent(no->esquerda, indent);
        printf(";");
        break;

    case NODE_IF:
        imprimirIndentacao(indent);
        printf("if (");
        imprimirASTComIndent(no->esquerda, indent);
        printf(")\n");
        imprimirASTComIndent(no->direita, indent + 1);
        if (no->centro)
        {
            printf("\n");
            imprimirIndentacao(indent);
            printf("else\n");
            imprimirASTComIndent(no->centro, indent + 1);
        }
        break;

    case NODE_WHILE:
        imprimirIndentacao(indent);
        printf("while (");
        imprimirASTComIndent(no->esquerda, indent);
        printf(")\n");
        imprimirASTComIndent(no->direita, indent + 1);
        break;

    case NODE_FOR:
        imprimirIndentacao(indent);
        printf("for (");
        if (no->esquerda)
            imprimirASTComIndent(no->esquerda, 0);
        printf("; ");
        if (no->direita)
            imprimirASTComIndent(no->direita, 0);
        printf("; ");
        if (no->centro)
            imprimirASTComIndent(no->centro, 0);
        printf(")\n");
        if (no->proximo)
            imprimirASTComIndent(no->proximo, indent + 1);
        break;

    case NODE_PROGRAM:
        imprimirASTComIndent(no->esquerda, indent);
        if (no->proximo)
            imprimirASTComIndent(no->proximo, indent);
        break;

    case NODE_PARAM_LIST:
    case NODE_ARG_LIST:
        imprimirASTComIndent(no->esquerda, indent);
        if (no->proximo)
            imprimirASTComIndent(no->proximo, indent);
        break;

    case NODE_FIELD_ASSIGN:
        imprimirIndentacao(indent);
        if (no->esquerda)
        {
            imprimirASTComIndent(no->esquerda, 0);
            printf(".");
        }
        printf("%s = ", no->data.field_info.campo_nome);
        imprimirASTComIndent(no->direita, 0);
        printf(";");
        break;

    case NODE_FIELD_ACCESS:
        imprimirIndentacao(indent);
        if (no->esquerda)
        {
            imprimirASTComIndent(no->esquerda, 0);
            printf(".");
        }
        printf("%s", no->data.field_info.campo_nome);
        break;

    case NODE_COMPOUND_STMT:
        imprimirIndentacao(indent);
        printf("{\n");
        for (NoAST *stmt = no->esquerda; stmt; stmt = stmt->proximo)
        {
            imprimirASTComIndent(stmt, indent + 1);
            printf("\n");
        }
        imprimirIndentacao(indent);
        printf("}\n");
        break;

    case NODE_BREAK:
        imprimirIndentacao(indent);
        printf("break;");
        break;

    case NODE_CONTINUE:
        imprimirIndentacao(indent);
        printf("continue;");
        break;

    case NODE_FUNCTION_CALL:
        imprimirIndentacao(indent);
        printf("%s(", no->data.func_name);
        imprimirASTComIndent(no->esquerda, 0);
        printf(")");
        break;

    case NODE_FUNCTION_DEF:
        imprimirIndentacao(indent);
        printf("function %s(", no->data.func_name);
        NoAST *param = no->esquerda;
        while (param)
        {
            printf("%s", param->data.decl_info.nome_declaracao);
            if (param->proximo)
                printf(", ");
            param = param->proximo;
        }
        printf(")\n");
        imprimirASTComIndent(no->direita, indent + 1);
        break;

    case NODE_SWITCH:
        imprimirIndentacao(indent);
        printf("switch (");
        imprimirASTComIndent(no->esquerda, 0);
        printf(")\n");
        imprimirASTComIndent(no->direita, indent + 1);
        break;

    case NODE_SWITCH_BODY:
        imprimirASTComIndent(no->esquerda, indent);
        if (no->direita)
        {
            imprimirASTComIndent(no->direita, indent);
        }
        break;

    case NODE_CASE:
        imprimirIndentacao(indent);
        printf("case ");
        imprimirASTComIndent(no->esquerda, 0);
        printf(":\n");
        imprimirASTComIndent(no->direita, indent + 1);
        break;

    case NODE_DEFAULT:
        imprimirIndentacao(indent);
        printf("default:\n");
        imprimirASTComIndent(no->esquerda, indent + 1);
        break;

    case NODE_ERROR:
        imprimirIndentacao(indent);
        printf("[NÓ DE ERRO]");
        break;

    default:
        imprimirIndentacao(indent);
        printf("[TIPO DE NÓ DESCONHECIDO]");
        break;
    }
}

// Implementação da função auxiliar para alocar um nó base
static NoAST *alocarNoAST(NodeType tipo_no)
{
    NoAST *no = (NoAST *)malloc(sizeof(NoAST));
    if (no == NULL)
    {
        fprintf(stderr, "Erro de alocacao de memoria para NoAST (tipo %d) na linha %d.\n", tipo_no, yylineno);
        exit(EXIT_FAILURE);
    }
    memset(no, 0, sizeof(NoAST)); // Inicializa todos os campos com zero/NULL
    no->tipo_no = tipo_no;
    no->linha = yylineno;
    return no;
}

// Implementação da função de criação de nó de declaração de variável
NoAST *criarNoDeclaracaoVar(char *nome, Tipo tipo_declarado, NoAST *inicializacao_expr)
{
    NoAST *no = alocarNoAST(NODE_DECLARATION);
    no->tipo_dado = tipo_declarado;
    no->data.decl_info.nome_declaracao = strdup(nome);
    if (no->data.decl_info.nome_declaracao == NULL)
    {
        fprintf(stderr, "Erro de alocacao de memoria para nome da variavel '%s' na declaracao na linha %d.\n", nome, yylineno);
        liberarAST(no); // Libera o nó parcialmente alocado
        exit(EXIT_FAILURE);
    }
    no->data.decl_info.inicializacao_expr = inicializacao_expr;
    return no;
}

// Implementação da função de criação de nó de declaração de variável array
NoAST *criarNoDeclaracaoVarArray(char *nome, Tipo tipo_declarado, NoAST *tamanho_expr)
{
    NoAST *no = alocarNoAST(NODE_DECLARATION);
    no->tipo_dado = tipo_declarado;
    no->data.decl_info.nome_declaracao = strdup(nome);
    if (no->data.decl_info.nome_declaracao == NULL)
    {
        fprintf(stderr, "Erro de alocacao de memoria para nome do array '%s' na declaracao na linha %d.\n", nome, yylineno);
        liberarAST(no);
        exit(EXIT_FAILURE);
    }
    no->data.decl_info.inicializacao_expr = tamanho_expr; // Aqui você pode guardar o tamanho ou NULL
    return no;
}

// Implementação da função de criação de nó de retorno
NoAST *criarNoReturn(NoAST *expr_retorno)
{
    NoAST *no = alocarNoAST(NODE_RETURN);
    no->esquerda = expr_retorno;                                          // A expressão de retorno é o filho esquerdo
    no->tipo_dado = (expr_retorno) ? expr_retorno->tipo_dado : TIPO_VOID; // Tipo do retorno
    return no;
}

// Exemplo para if
NoAST *criarNoIf(NoAST *condicao, NoAST *bloco_then, NoAST *bloco_else)
{
    NoAST *no = alocarNoAST(NODE_IF);
    no->esquerda = condicao;
    no->direita = bloco_then;
    no->centro = bloco_else;
    no->tipo_dado = TIPO_VOID;
    if (bloco_then && bloco_then->tipo_no == NODE_COMPOUND_STMT)
        bloco_then->pai_controlador = no;
    if (bloco_else && bloco_else->tipo_no == NODE_COMPOUND_STMT)
        bloco_else->pai_controlador = no;
    return no;
}

// Exemplo para while
NoAST *criarNoWhile(NoAST *condicao, NoAST *bloco)
{
    NoAST *no = alocarNoAST(NODE_WHILE);
    no->esquerda = condicao;
    no->direita = bloco;
    no->tipo_dado = TIPO_VOID;
    if (bloco && bloco->tipo_no == NODE_COMPOUND_STMT)
        bloco->pai_controlador = no;
    return no;
}

// Exemplo para switch
NoAST *criarNoSwitch(NoAST *expr_controle, NoAST *corpo_switch)
{
    NoAST *no = alocarNoAST(NODE_SWITCH);
    no->esquerda = expr_controle;
    no->direita = corpo_switch;
    no->tipo_dado = TIPO_VOID;
    if (corpo_switch && corpo_switch->tipo_no == NODE_COMPOUND_STMT)
        corpo_switch->pai_controlador = no;
    return no;
}

NoAST *criarNoFor(NoAST *init, NoAST *cond, NoAST *inc, NoAST *corpo)
{
    NoAST *no = alocarNoAST(NODE_FOR);
    no->esquerda = init;
    no->direita = cond;
    no->centro = inc;
    no->proximo = corpo;
    if (corpo && corpo->tipo_no == NODE_COMPOUND_STMT)
        corpo->pai_controlador = no;
    return no;
}

// Implementação da função de criação de nó de bloco de comando (compound statement)
NoAST *criarNoCompoundStmt(NoAST *lista_statements)
{
    NoAST *no = alocarNoAST(NODE_COMPOUND_STMT);
    no->esquerda = lista_statements; // A cabeça da lista de statements dentro do bloco
    no->tipo_dado = TIPO_VOID;
    return no;
}

// Implementação da função de criação de nó break
NoAST *criarNoBreak()
{
    NoAST *no = alocarNoAST(NODE_BREAK);
    no->tipo_dado = TIPO_VOID;
    return no;
}

// Implementação da função de criação de nó continue
NoAST *criarNoContinue()
{
    NoAST *no = alocarNoAST(NODE_CONTINUE);
    no->tipo_dado = TIPO_VOID;
    return no;
}

// Implementação da função de criação de nó de chamada de função
NoAST *criarNoChamadaFuncao(char *nome_func, NoAST *args_list, Tipo tipo_retorno)
{
    NoAST *no = alocarNoAST(NODE_FUNCTION_CALL);
    no->data.func_name = strdup(nome_func);
    if (no->data.func_name == NULL)
    {
        fprintf(stderr, "Erro de alocacao de memoria para nome da funcao '%s' na chamada na linha %d.\n", nome_func, yylineno);
        liberarAST(no);
        exit(EXIT_FAILURE);
    }
    no->esquerda = args_list;
    no->tipo_dado = tipo_retorno; // Agora usa o tipo correto!
    return no;
}

// Função para criar um nó de operador
NoAST *criarNoOp(OperatorType op, NoAST *esq, NoAST *dir)
{
    NoAST *no = (NoAST *)malloc(sizeof(NoAST));
    if (no == NULL)
    {
        fprintf(stderr, "Erro de alocacao de memoria para NoAST (Operador).\n");
        exit(EXIT_FAILURE);
    }
    no->tipo_no = NODE_OPERATOR;
    no->data.op_type = op;
    no->esquerda = esq;
    no->direita = dir;
    no->linha = yylineno;

    if (esq == NULL || dir == NULL || esq->tipo_dado == TIPO_ERRO || dir->tipo_dado == TIPO_ERRO)
    {
        no->tipo_dado = TIPO_ERRO;
    }
    else
    {
        // Lógica de inferência de tipo (pode ser mais complexa com coerção)
        no->tipo_dado = (esq->tipo_dado == dir->tipo_dado) ? esq->tipo_dado : TIPO_ERRO;
    }
    return no;
}

NoAST *criarNoUnario(OperatorType op, NoAST *operando)
{
    NoAST *no = (NoAST *)malloc(sizeof(NoAST));
    if (no == NULL)
    {
        fprintf(stderr, "Erro de alocacao de memoria para NoAST (Unario).\n");
        exit(EXIT_FAILURE);
    }
    no->tipo_no = NODE_OPERATOR;
    no->data.op_type = op;
    no->esquerda = operando;
    no->direita = NULL;
    no->linha = yylineno;

    if (operando == NULL || operando->tipo_dado == TIPO_ERRO)
    {
        no->tipo_dado = TIPO_ERRO;
    }
    else
    {
        // Lógica de tipo para operadores unários (geralmente mantêm o tipo do operando)
        no->tipo_dado = operando->tipo_dado;
    }
    return no;
}

// Função para criar um nó numérico (INT, FLOAT, DOUBLE)
NoAST *criarNoNum(Tipo tipo, void *valor)
{
    NoAST *no = (NoAST *)malloc(sizeof(NoAST));
    if (no == NULL)
    {
        fprintf(stderr, "Erro de alocacao de memoria para NoAST (Numero).\n");
        exit(EXIT_FAILURE);
    }
    no->tipo_no = NODE_LITERAL;
    no->tipo_dado = tipo;
    no->esquerda = NULL;
    no->direita = NULL;
    no->linha = yylineno;

    switch (tipo)
    {
    case TIPO_INT:
        no->data.literal.val_int = *(int *)valor;
        break;
    case TIPO_FLOAT:
        no->data.literal.val_float = *(float *)valor;
        break;
    case TIPO_DOUBLE:
        no->data.literal.val_double = *(double *)valor;
        break;
    default:
        fprintf(stderr, "Tipo numerico invalido para criarNoNum.\n");
        free(no);
        return NULL; // Deve ser tratado pelo chamador
    }
    return no;
}

// Função para criar um nó de identificador
NoAST *criarNoId(char *nome, Tipo tipo)
{
    NoAST *no = (NoAST *)malloc(sizeof(NoAST));
    if (no == NULL)
    {
        fprintf(stderr, "Erro de alocacao de memoria para NoAST (Identificador).\n");
        exit(EXIT_FAILURE);
    }
    no->tipo_no = NODE_IDENTIFIER;
    no->tipo_dado = tipo;
    strncpy(no->data.nome_id, nome, sizeof(no->data.nome_id) - 1);
    no->data.nome_id[sizeof(no->data.nome_id) - 1] = '\0';
    no->esquerda = NULL;
    no->direita = NULL;
    no->linha = yylineno;
    return no;
}

// Função para criar um nó de caractere
NoAST *criarNoChar(char val)
{
    NoAST *no = (NoAST *)malloc(sizeof(NoAST));
    if (no == NULL)
    {
        fprintf(stderr, "Erro de alocacao de memoria para NoAST (Char).\n");
        exit(EXIT_FAILURE);
    }
    no->tipo_no = NODE_LITERAL;
    no->tipo_dado = TIPO_CHAR;
    no->data.literal.val_char = val;
    no->esquerda = NULL;
    no->direita = NULL;
    no->linha = yylineno;
    return no;
}

// Função para criar um nó de string
NoAST *criarNoString(char *val)
{
    NoAST *no = (NoAST *)malloc(sizeof(NoAST));
    if (no == NULL)
    {
        fprintf(stderr, "Erro de alocacao de memoria para NoAST (String).\n");
        exit(EXIT_FAILURE);
    }
    no->tipo_no = NODE_LITERAL;
    no->tipo_dado = TIPO_STRING;
    no->data.literal.val_string = strdup(val);
    if (no->data.literal.val_string == NULL)
    {
        fprintf(stderr, "Erro de alocacao de memoria para string literal.\n");
        free(no);
        exit(EXIT_FAILURE);
    }
    no->esquerda = NULL;
    no->direita = NULL;
    no->linha = yylineno;
    return no;
}

// Função para criar um nó de erro
NoAST *criarNoErro()
{
    NoAST *no = (NoAST *)malloc(sizeof(NoAST));
    if (no == NULL)
    {
        fprintf(stderr, "Erro de alocacao de memoria para NoAST (Erro).\n");
        exit(EXIT_FAILURE);
    }
    no->tipo_no = NODE_ERROR;
    no->tipo_dado = TIPO_ERRO;
    no->esquerda = NULL;
    no->direita = NULL;
    no->linha = yylineno;
    return no;
}

// Impressão da AST em formato C-like para uso em arquivos (por exemplo, geração de código/intermediário)
void imprimirNoAST(NoAST *no, FILE *out)
{
    if (no == NULL)
        return;
    switch (no->tipo_no)
    {
    case NODE_OPERATOR:
        if (no->data.op_type == OP_NOT_TYPE || no->data.op_type == OP_INC_TYPE || no->data.op_type == OP_DEC_TYPE)
        {
            fprintf(out, "(%s", (no->data.op_type == OP_NOT_TYPE) ? "!" : (no->data.op_type == OP_INC_TYPE) ? "++"
                                                                      : (no->data.op_type == OP_DEC_TYPE)   ? "--"
                                                                                                            : "?");
            imprimirNoAST(no->esquerda, out);
            fprintf(out, ")");
        }
        else
        {
            fprintf(out, "(");
            imprimirNoAST(no->esquerda, out);
            fprintf(out, " %s ",
                    (no->data.op_type == OP_ADD_TYPE) ? "+" : (no->data.op_type == OP_SUB_TYPE)      ? "-"
                                                          : (no->data.op_type == OP_MUL_TYPE)        ? "*"
                                                          : (no->data.op_type == OP_DIV_TYPE)        ? "/"
                                                          : (no->data.op_type == OP_ASSIGN_TYPE)     ? "="
                                                          : (no->data.op_type == OP_EQ_TYPE)         ? "=="
                                                          : (no->data.op_type == OP_NE_TYPE)         ? "!="
                                                          : (no->data.op_type == OP_LT_TYPE)         ? "<"
                                                          : (no->data.op_type == OP_LE_TYPE)         ? "<="
                                                          : (no->data.op_type == OP_GT_TYPE)         ? ">"
                                                          : (no->data.op_type == OP_GE_TYPE)         ? ">="
                                                          : (no->data.op_type == OP_AND_TYPE)        ? "&&"
                                                          : (no->data.op_type == OP_OR_TYPE)         ? "||"
                                                          : (no->data.op_type == OP_ADD_ASSIGN_TYPE) ? "+="
                                                          : (no->data.op_type == OP_SUB_ASSIGN_TYPE) ? "-="
                                                          : (no->data.op_type == OP_MUL_ASSIGN_TYPE) ? "*="
                                                          : (no->data.op_type == OP_DIV_ASSIGN_TYPE) ? "/="
                                                          : (no->data.op_type == OP_MOD_ASSIGN_TYPE) ? "%="
                                                                                                     : "?");
            imprimirNoAST(no->direita, out);
            fprintf(out, ")");
        }
        break;
    case NODE_LITERAL:
        switch (no->tipo_dado)
        {
        case TIPO_INT:
            fprintf(out, "%d", no->data.literal.val_int);
            break;
        case TIPO_FLOAT:
            fprintf(out, "%f", no->data.literal.val_float);
            break;
        case TIPO_DOUBLE:
            fprintf(out, "%lf", no->data.literal.val_double);
            break;
        case TIPO_CHAR:
            fprintf(out, "'%c'", no->data.literal.val_char);
            break;
        case TIPO_STRING:
            fprintf(out, "\"%s\"", no->data.literal.val_string);
            break;
        default:
            fprintf(out, "[LITERAL DESCONHECIDO]");
            break;
        }
        break;
    case NODE_IDENTIFIER:
        fprintf(out, "%s", no->data.nome_id);
        break;
    case NODE_DECLARATION:
        fprintf(out, "%s %s",
                (no->tipo_dado == TIPO_INT) ? "int" : (no->tipo_dado == TIPO_FLOAT) ? "float"
                                                  : (no->tipo_dado == TIPO_DOUBLE)  ? "double"
                                                  : (no->tipo_dado == TIPO_CHAR)    ? "char"
                                                  : (no->tipo_dado == TIPO_STRING)  ? "string"
                                                                                    : "tipo_desconhecido",
                no->data.decl_info.nome_declaracao);
        if (no->data.decl_info.inicializacao_expr)
        {
            fprintf(out, " = ");
            imprimirNoAST(no->data.decl_info.inicializacao_expr, out);
        }
        fprintf(out, ";");
        break;
    case NODE_RETURN:
        fprintf(out, "return ");
        imprimirNoAST(no->esquerda, out);
        fprintf(out, ";");
        break;
    case NODE_IF:
        fprintf(out, "if (");
        imprimirNoAST(no->esquerda, out);
        fprintf(out, ") ");
        imprimirNoAST(no->direita, out);
        if (no->centro)
        {
            fprintf(out, " else ");
            imprimirNoAST(no->centro, out);
        }
        break;
    case NODE_WHILE:
        fprintf(out, "while (");
        imprimirNoAST(no->esquerda, out);
        fprintf(out, ") ");
        imprimirNoAST(no->direita, out);
        break;

    case NODE_FOR:
        fprintf(out, "for (");
        if (no->esquerda)
            imprimirNoAST(no->esquerda, out);
        fprintf(out, "; ");
        if (no->direita)
            imprimirNoAST(no->direita, out);
        fprintf(out, "; ");
        if (no->centro)
            imprimirNoAST(no->centro, out);
        fprintf(out, ") ");
        if (no->proximo)
            imprimirNoAST(no->proximo, out);
        break;
    case NODE_COMPOUND_STMT:
        fprintf(out, "{\n");
        imprimirNoAST(no->esquerda, out);
        fprintf(out, "}\n");
        break;
    case NODE_BREAK:
        fprintf(out, "break;");
        break;
    case NODE_CONTINUE:
        fprintf(out, "continue;");
        break;
    case NODE_FUNCTION_CALL:
        fprintf(out, "%s(", no->data.func_name);
        imprimirNoAST(no->esquerda, out);
        fprintf(out, ")");
        break;
    case NODE_SWITCH:
        fprintf(out, "switch (");
        imprimirNoAST(no->esquerda, out);
        fprintf(out, ") ");
        imprimirNoAST(no->direita, out);
        break;
    case NODE_SWITCH_BODY:
        imprimirNoAST(no->esquerda, out);
        if (no->direita)
            imprimirNoAST(no->direita, out);
        break;
    case NODE_CASE:
        fprintf(out, "case ");
        imprimirNoAST(no->esquerda, out);
        fprintf(out, ": ");
        imprimirNoAST(no->direita, out);
        break;
    case NODE_DEFAULT:
        fprintf(out, "default: ");
        imprimirNoAST(no->esquerda, out);
        break;
    case NODE_ERROR:
        fprintf(out, "[NÓ DE ERRO]");
        break;
    default:
        fprintf(out, "[TIPO DE NÓ DESCONHECIDO]");
        break;
    }
}

NoAST *criarNoAtribuicaoCampo(NoAST *struct_expr, char *campo, NoAST *valor)
{
    NoAST *no = alocarNoAST(NODE_FIELD_ASSIGN);
    no->esquerda = struct_expr;
    no->direita = valor;
    no->tipo_dado = (valor) ? valor->tipo_dado : TIPO_ERRO;
    strncpy(no->data.field_info.campo_nome, campo, sizeof(no->data.field_info.campo_nome) - 1);
    no->data.field_info.campo_nome[sizeof(no->data.field_info.campo_nome) - 1] = '\0';
    return no;
}

NoAST *criarNoAcessoCampo(NoAST *struct_expr, char *campo)
{
    NoAST *no = alocarNoAST(NODE_FIELD_ACCESS);
    no->esquerda = struct_expr;
    no->direita = NULL;
    // Tenta inferir o tipo do campo a partir do struct (simples)
    if (struct_expr && struct_expr->tipo_dado == TIPO_OBJETO)
    {
        // Aqui você pode buscar o tipo real do campo na tabela de símbolos do struct
        // Por enquanto, marque como int (ou outro tipo padrão)
        no->tipo_dado = TIPO_INT; // Troque por busca real se possível
    }
    else
    {
        no->tipo_dado = TIPO_INT; // fallback para int
    }
    strncpy(no->data.field_info.campo_nome, campo, sizeof(no->data.field_info.campo_nome) - 1);
    no->data.field_info.campo_nome[sizeof(no->data.field_info.campo_nome) - 1] = '\0';
    return no;
}

void imprimirAST(NoAST *no)
{
    if (no == NULL)
    {
        return;
    }

    switch (no->tipo_no)
    {
    case NODE_OPERATOR:
        if (no->data.op_type == OP_NOT_TYPE || no->data.op_type == OP_INC_TYPE || no->data.op_type == OP_DEC_TYPE)
        {
            printf("(%s",
                   (no->data.op_type == OP_NOT_TYPE) ? "!" : (no->data.op_type == OP_INC_TYPE) ? "++"
                                                         : (no->data.op_type == OP_DEC_TYPE)   ? "--"
                                                                                               : "?");
            imprimirAST(no->esquerda);
            printf(")");
        }
        else
        {
            printf("(");
            imprimirAST(no->esquerda);
            printf(" %s ",
                   (no->data.op_type == OP_ADD_TYPE) ? "+" : (no->data.op_type == OP_SUB_TYPE)      ? "-"
                                                         : (no->data.op_type == OP_MUL_TYPE)        ? "*"
                                                         : (no->data.op_type == OP_DIV_TYPE)        ? "/"
                                                         : (no->data.op_type == OP_ASSIGN_TYPE)     ? "="
                                                         : (no->data.op_type == OP_EQ_TYPE)         ? "=="
                                                         : (no->data.op_type == OP_NE_TYPE)         ? "!="
                                                         : (no->data.op_type == OP_LT_TYPE)         ? "<"
                                                         : (no->data.op_type == OP_LE_TYPE)         ? "<="
                                                         : (no->data.op_type == OP_GT_TYPE)         ? ">"
                                                         : (no->data.op_type == OP_GE_TYPE)         ? ">="
                                                         : (no->data.op_type == OP_AND_TYPE)        ? "&&"
                                                         : (no->data.op_type == OP_OR_TYPE)         ? "||"
                                                         : (no->data.op_type == OP_ADD_ASSIGN_TYPE) ? "+="
                                                         : (no->data.op_type == OP_SUB_ASSIGN_TYPE) ? "-="
                                                         : (no->data.op_type == OP_MUL_ASSIGN_TYPE) ? "*="
                                                         : (no->data.op_type == OP_DIV_ASSIGN_TYPE) ? "/="
                                                         : (no->data.op_type == OP_MOD_ASSIGN_TYPE) ? "%="
                                                                                                    : "?");
            imprimirAST(no->direita);
            printf(")");
        }
        break;

    case NODE_LITERAL:
        switch (no->tipo_dado)
        {
        case TIPO_INT:
            printf("%d", no->data.literal.val_int);
            break;
        case TIPO_FLOAT:
            printf("%f", no->data.literal.val_float);
            break;
        case TIPO_DOUBLE:
            printf("%lf", no->data.literal.val_double);
            break;
        case TIPO_CHAR:
            printf("'%c'", no->data.literal.val_char);
            break;
        case TIPO_STRING:
            printf("\"%s\"", no->data.literal.val_string);
            break;
        default:
            printf("[LITERAL DESCONHECIDO]");
            break;
        }
        break;

    case NODE_IDENTIFIER:
        printf("%s", no->data.nome_id);
        break;

    case NODE_DECLARATION:
        printf("%s %s",
               (no->tipo_dado == TIPO_INT) ? "int" : (no->tipo_dado == TIPO_FLOAT) ? "float"
                                                 : (no->tipo_dado == TIPO_DOUBLE)  ? "double"
                                                 : (no->tipo_dado == TIPO_CHAR)    ? "char"
                                                 : (no->tipo_dado == TIPO_STRING)  ? "string"
                                                                                   : "tipo_desconhecido",
               no->data.decl_info.nome_declaracao);
        if (no->data.decl_info.inicializacao_expr)
        {
            printf(" = ");
            imprimirAST(no->data.decl_info.inicializacao_expr);
        }
        printf(";");
        break;

    case NODE_RETURN:
        printf("return ");
        imprimirAST(no->esquerda);
        printf(";");
        break;

    case NODE_IF:
        printf("if (");
        imprimirAST(no->esquerda);
        printf(") ");
        imprimirAST(no->direita);
        if (no->centro)
        {
            printf(" else ");
            imprimirAST(no->centro);
        }
        break;

    case NODE_WHILE:
        printf("while (");
        imprimirAST(no->esquerda);
        printf(") ");
        imprimirAST(no->direita);
        break;

    case NODE_FOR:
        printf("for (");
        if (no->esquerda)
            imprimirAST(no->esquerda);
        printf("; ");
        if (no->direita)
            imprimirAST(no->direita);
        printf("; ");
        if (no->centro)
            imprimirAST(no->centro);
        printf(") ");
        if (no->proximo)
            imprimirAST(no->proximo);
        break;

    case NODE_COMPOUND_STMT:
        printf("{\n");
        imprimirAST(no->esquerda);
        printf("}\n");
        break;

    case NODE_BREAK:
        printf("break;");
        break;

    case NODE_CONTINUE:
        printf("continue;");
        break;

    case NODE_FUNCTION_CALL:
        printf("%s(", no->data.func_name);
        imprimirAST(no->esquerda);
        printf(")");
        break;

    case NODE_SWITCH:
        printf("switch (");
        imprimirAST(no->esquerda);
        printf(") ");
        imprimirAST(no->direita);
        break;

    case NODE_SWITCH_BODY:
        imprimirAST(no->esquerda);
        if (no->direita)
        {
            imprimirAST(no->direita);
        }
        break;

    case NODE_CASE:
        printf("case ");
        imprimirAST(no->esquerda);
        printf(": ");
        imprimirAST(no->direita);
        break;

    case NODE_DEFAULT:
        printf("default: ");
        imprimirAST(no->esquerda);
        break;

    case NODE_ERROR:
        printf("[NÓ DE ERRO]");
        break;

    default:
        printf("[TIPO DE NÓ DESCONHECIDO]");
        break;
    }
}

void liberarAST(NoAST *no)
{
    if (no == NULL)
        return;

    switch (no->tipo_no)
    {
    case NODE_DECLARATION:
        free(no->data.decl_info.nome_declaracao);
        liberarAST(no->data.decl_info.inicializacao_expr);
        break;

    case NODE_FUNCTION_CALL:
        free(no->data.func_name);
        break;

    case NODE_LITERAL:
        if (no->tipo_dado == TIPO_STRING && no->data.literal.val_string != NULL)
        {
            free(no->data.literal.val_string);
        }
        break;

    default:
        break;
    }

    liberarAST(no->esquerda);
    liberarAST(no->direita);
    liberarAST(no->centro);
    liberarAST(no->proximo);

    free(no);
}

int tiposCompativeis(Tipo t1, Tipo t2)
{
    // Permite atribuição entre tipos numéricos compatíveis
    if (t1 == t2)
        return 1;
    // Permite promoção: int -> float/double, float -> double
    if ((t1 == TIPO_DOUBLE && (t2 == TIPO_FLOAT || t2 == TIPO_INT)) ||
        (t1 == TIPO_FLOAT && t2 == TIPO_INT))
        return 1;
    return 0;
}

NoAST *criarNoFuncao(char *nome, Tipo tipo_retorno, NoAST *params, NoAST *corpo)
{
    NoAST *no = alocarNoAST(NODE_FUNCTION_DEF);
    no->data.func_name = strdup(nome);
    no->tipo_dado = tipo_retorno;
    no->esquerda = params; // já é uma lista encadeada
    no->direita = corpo;
    no->proximo = NULL;
    return no;
}
const char *nomeTipo(Tipo tipo)
{
    switch (tipo)
    {
    case TIPO_INT:
        return "int";
    case TIPO_FLOAT:
        return "float";
    case TIPO_DOUBLE:
        return "double";
    case TIPO_CHAR:
        return "char";
    case TIPO_STRING:
        return "string";
    case TIPO_VOID:
        return "void";
    case TIPO_OBJETO:
        return "struct";
    case TIPO_ERRO:
        return "erro";
    default:
        return "desconhecido";
    }
}

NoAST *criarNoSwitchBody(NoAST *case_list_head, NoAST *default_node)
{
    NoAST *no = alocarNoAST(NODE_SWITCH_BODY);
    no->esquerda = case_list_head;
    no->direita = default_node;
    return no;
}

NoAST *criarNoCase(NoAST *case_expr, NoAST *statement_list)
{
    NoAST *no = alocarNoAST(NODE_CASE);
    no->esquerda = case_expr;
    no->direita = statement_list;
    return no;
}

NoAST *criarNoDefault(NoAST *statement_list)
{
    NoAST *no = alocarNoAST(NODE_DEFAULT);
    no->esquerda = statement_list;
    return no;
}

NoAST *criarNoDeclaracaoVarArrayComInicializacao(char *nome, Tipo tipo, NoAST *tamanho, NoAST *valores)
{
    NoAST *no = alocarNoAST(NODE_DECLARATION);
    no->tipo_dado = tipo;
    no->data.decl_info.nome_declaracao = strdup(nome);
    if (no->data.decl_info.nome_declaracao == NULL)
    {
        fprintf(stderr, "Erro de alocacao de memoria para nome do array '%s' na declaracao com inicializacao na linha %d.\n", nome, yylineno);
        liberarAST(no);
        exit(EXIT_FAILURE);
    }
    no->data.decl_info.inicializacao_expr = valores;
    no->data.decl_info.tamanho_array = (tamanho != NULL) ? tamanho : NULL;
    return no;
}

NoAST *criarNoArrayLiteral(NoAST *valores)
{
    NoAST *no = alocarNoAST(NODE_ARRAY_LITERAL);
    no->tipo_dado = TIPO_INT; // tipo default do array, pode ajustar depois se quiser
    no->esquerda = valores;   // lista de valores
    no->direita = NULL;
    no->proximo = NULL;
    no->pai_controlador = NULL;
    return no;
}

NoAST *removerNoDaLista(NoAST *head, NoAST *alvo)
{
    if (!head)
        return NULL;
    if (head == alvo)
        return head->proximo;
    NoAST *atual = head;
    while (atual->proximo)
    {
        if (atual->proximo == alvo)
        {
            atual->proximo = alvo->proximo;
            break;
        }
        atual = atual->proximo;
    }
    return head;
}

NoAST *criarNoAcessoArray(NoAST *array, NoAST *indice)
{
    NoAST *no = alocarNoAST(NODE_ARRAY_ACCESS);
    no->esquerda = array;     // o array
    no->direita = indice;     // o índice de acesso
    no->tipo_dado = TIPO_INT; // tipo padrão, ou pode tentar inferir depois
    return no;
}

// Função utilitária para copiar um nó isolado da AST (sem filhos)
NoAST *copiarNoAST(const NoAST *orig)
{
    if (!orig)
        return NULL;
    NoAST *novo = (NoAST *)malloc(sizeof(NoAST));
    memset(novo, 0, sizeof(NoAST));
    novo->tipo_no = orig->tipo_no;
    novo->tipo_dado = orig->tipo_dado;
    switch (orig->tipo_no)
    {
    case NODE_IDENTIFIER:
        strncpy(novo->data.nome_id, orig->data.nome_id, sizeof(novo->data.nome_id));
        novo->data.nome_id[sizeof(novo->data.nome_id) - 1] = '\0';
        break;
    case NODE_LITERAL:
        if (orig->tipo_dado == TIPO_STRING && orig->data.literal.val_string)
        {
            novo->data.literal.val_string = strdup(orig->data.literal.val_string);
        }
        else if (orig->tipo_dado == TIPO_INT)
        {
            novo->data.literal.val_int = orig->data.literal.val_int;
        }
        else if (orig->tipo_dado == TIPO_FLOAT)
        {
            novo->data.literal.val_float = orig->data.literal.val_float;
        }
        else if (orig->tipo_dado == TIPO_DOUBLE)
        {
            novo->data.literal.val_double = orig->data.literal.val_double;
        }
        else if (orig->tipo_dado == TIPO_CHAR)
        {
            novo->data.literal.val_char = orig->data.literal.val_char;
        }
        break;
    case NODE_OPERATOR:
        novo->data.op_type = orig->data.op_type;
        break;
    case NODE_FUNCTION_CALL:
        if (orig->data.func_name)
            novo->data.func_name = strdup(orig->data.func_name);
        break;
    default:
        // Para outros tipos, copie campos relevantes se necessário
        break;
    }
    // Todos os ponteiros para filhos ficam NULL (nó isolado)
    novo->esquerda = NULL;
    novo->direita = NULL;
    novo->centro = NULL;
    novo->proximo = NULL;
    novo->parametros = NULL;
    novo->pai_controlador = NULL;
    return novo;
}