#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

static const char *ts_type(Tipo tipo)
{
    switch (tipo)
    {
    case TIPO_INT:
    case TIPO_FLOAT:
    case TIPO_DOUBLE:
        return "number";
    case TIPO_CHAR:
    case TIPO_STRING:
        return "string";
    case TIPO_VOID:
        return "void";
    default:
        return "any";
    }
}

void gerarTypeScript(NoAST *no, FILE *saida)
{
    if (!no)
        return;

    switch (no->tipo_no)
    {
    case NODE_DECLARATION:
        fprintf(saida, "let %s: %s", no->data.decl_info.nome_declaracao, ts_type(no->tipo_dado));
        if (no->data.decl_info.inicializacao_expr)
        {
            fprintf(saida, " = ");
            gerarTypeScript(no->data.decl_info.inicializacao_expr, saida);
        }
        fprintf(saida, ";\n");
        break;
    case NODE_LITERAL:
        switch (no->tipo_dado)
        {
        case TIPO_INT:
            fprintf(saida, "%d", no->data.literal.val_int);
            break;
        case TIPO_FLOAT:
            fprintf(saida, "%f", no->data.literal.val_float);
            break;
        case TIPO_DOUBLE:
            fprintf(saida, "%lf", no->data.literal.val_double);
            break;
        case TIPO_CHAR:
            fprintf(saida, "'%c'", no->data.literal.val_char);
            break;
        case TIPO_STRING:
            fprintf(saida, "\"%s\"", no->data.literal.val_string);
            break;
        default:
            fprintf(saida, "undefined");
        }
        break;
    case NODE_IDENTIFIER:
        fprintf(saida, "%s", no->data.nome_id);
        break;
    case NODE_OPERATOR:
        // Exemplo para operadores binários
        gerarTypeScript(no->esquerda, saida);
        switch (no->data.op_type)
        {
        case OP_ADD_TYPE:
            fprintf(saida, " + ");
            break;
        case OP_SUB_TYPE:
            fprintf(saida, " - ");
            break;
        case OP_MUL_TYPE:
            fprintf(saida, " * ");
            break;
        case OP_DIV_TYPE:
            fprintf(saida, " / ");
            break;
        case OP_ASSIGN_TYPE:
            fprintf(saida, " = ");
            break;
        // Adicione outros operadores conforme necessário
        default:
            fprintf(saida, " /*op*/ ");
            break;
        }
        gerarTypeScript(no->direita, saida);
        break;
    case NODE_RETURN:
        fprintf(saida, "return ");
        gerarTypeScript(no->esquerda, saida);
        fprintf(saida, ";\n");
        break;
    case NODE_FUNCTION_CALL:
        fprintf(saida, "%s(", no->data.func_name);
        for (NoAST *arg = no->esquerda; arg; arg = arg->proximo)
        {
            gerarTypeScript(arg, saida);
            if (arg->proximo)
                fprintf(saida, ", ");
        }
        fprintf(saida, ")");
        break;
    case NODE_COMPOUND_STMT:
        fprintf(saida, "{\n");
        for (NoAST *stmt = no->esquerda; stmt; stmt = stmt->proximo)
            gerarTypeScript(stmt, saida);
        fprintf(saida, "}\n");
        break;
    case NODE_FUNCTION_DEF:
        fprintf(saida, "function %s(", no->data.func_name);
        // Gere parâmetros
        for (NoAST *param = no->esquerda; param; param = param->proximo)
        {
            fprintf(saida, "%s: %s", param->data.decl_info.nome_declaracao, ts_type(param->tipo_dado));
            if (param->proximo)
                fprintf(saida, ", ");
        }
        fprintf(saida, "): %s ", ts_type(no->tipo_dado));
        gerarTypeScript(no->direita, saida); // corpo da função
        fprintf(saida, "\n");
        break;
    // Adicione outros casos conforme necessário
    default:
        break;
    }
    if (no->proximo)
        gerarTypeScript(no->proximo, saida);
}

void imprimeAst(NoAST *raiz_ast)
{
    printf("AST gerada? %p\n", (void *)raiz_ast);
    // Resto da função de impressão da AST...
}