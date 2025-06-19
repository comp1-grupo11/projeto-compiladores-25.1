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
        case OP_EQ_TYPE:
            fprintf(saida, " == ");
            break;
        case OP_NE_TYPE:
            fprintf(saida, " != ");
            break;
        case OP_GT_TYPE:
            fprintf(saida, " > ");
            break;
        case OP_LT_TYPE:
            fprintf(saida, " < ");
            break;
        case OP_GE_TYPE:
            fprintf(saida, " >= ");
            break;
        case OP_LE_TYPE:
            fprintf(saida, " <= ");
            break;
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
        NoAST *paramAtual = no->esquerda;
        while (paramAtual)
        {
            fprintf(saida, "%s: %s", paramAtual->data.decl_info.nome_declaracao, ts_type(paramAtual->tipo_dado));
            if (paramAtual->proximo)
                fprintf(saida, ", ");
            paramAtual = paramAtual->proximo;
        }
        fprintf(saida, "): %s {\n", ts_type(no->tipo_dado));
        gerarTypeScript(no->direita, saida);
        fprintf(saida, "}\n");
        break;
    case NODE_IF:
        fprintf(saida, "if (");
        gerarTypeScript(no->esquerda, saida);
        fprintf(saida, ") {\n");
        gerarTypeScript(no->direita, saida); // then block
        fprintf(saida, "}");
        if (no->centro)
        {
            fprintf(saida, " else {\n");
            gerarTypeScript(no->centro, saida); // else block
            fprintf(saida, "}");
        }
        fprintf(saida, "\n");
        break;
    case NODE_FOR:
        fprintf(saida, "for (");
        if (no->esquerda)
            gerarTypeScript(no->esquerda, saida); // init
        fprintf(saida, "; ");
        if (no->direita)
            gerarTypeScript(no->direita, saida); // condition
        fprintf(saida, "; ");
        if (no->centro)
            gerarTypeScript(no->centro, saida); // iteration
        fprintf(saida, ") {\n");
        if (no->proximo && no->proximo->tipo_no != NODE_COMPOUND_STMT)
        {
            gerarTypeScript(no->proximo, saida);
        }
        else if (no->proximo)
        {
            gerarTypeScript(no->proximo, saida);
        }
        break;
    default:
        fprintf(saida, "// [Tipo de nó não implementado: %d]\n", no->tipo_no);
        break;
    }

    if (no->proximo)
        gerarTypeScript(no->proximo, saida);
}