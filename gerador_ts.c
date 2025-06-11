#include <stdio.h>
#include "ast.h"

void imprimirIndentacaoTs(int indent)
{
    for (int i = 0; i < indent; i++)
        printf("  ");
}

void gerarCodigoTs(NoAST *no, int indent)
{
    if (!no)
        return;

    switch (no->tipo_no)
    {
    case NODE_DECLARATION:
        imprimirIndentacaoTs(indent);
        printf("let %s", no->data.decl_info.nome_declaracao);
        if (no->data.decl_info.inicializacao_expr)
        {
            printf(" = ");
            gerarCodigoTs(no->data.decl_info.inicializacao_expr, 0);
        }
        printf(";\n");
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
            printf("undefined");
            break;
        }
        break;

    case NODE_IDENTIFIER:
        printf("%s", no->data.nome_id);
        break;

    case NODE_OPERATOR:
        printf("(");
        gerarCodigoTs(no->esquerda, 0);
        printf(" %s ",
               (no->data.op_type == OP_ADD_TYPE) ? "+" : (no->data.op_type == OP_SUB_TYPE)  ? "-"
                                                     : (no->data.op_type == OP_MUL_TYPE)    ? "*"
                                                     : (no->data.op_type == OP_DIV_TYPE)    ? "/"
                                                     : (no->data.op_type == OP_ASSIGN_TYPE) ? "="
                                                                                            : "?");
        gerarCodigoTs(no->direita, 0);
        printf(")");
        break;

    case NODE_RETURN:
        imprimirIndentacaoTs(indent);
        printf("return ");
        gerarCodigoTs(no->esquerda, 0);
        printf(";\n");
        break;

    case NODE_COMPOUND_STMT:
        imprimirIndentacaoTs(indent);
        printf("{\n");
        gerarCodigoTs(no->esquerda, indent + 1);
        imprimirIndentacaoTs(indent);
        printf("}\n");
        break;

    case NODE_IF:
        imprimirIndentacaoTs(indent);
        printf("if (");
        gerarCodigoTs(no->esquerda, 0);
        printf(")\n");
        gerarCodigoTs(no->direita, indent + 1);
        if (no->centro)
        {
            imprimirIndentacaoTs(indent);
            printf("else\n");
            gerarCodigoTs(no->centro, indent + 1);
        }
        break;

    default:
        imprimirIndentacaoTs(indent);
        printf("// [Tipo de nó não implementado: %d]\n", no->tipo_no);
        break;
    }

    if (no->proximo)
        gerarCodigoTs(no->proximo, indent);
}