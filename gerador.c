#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

static int temp_count = 0;
static int label_count = 0;

char *novaTemp()
{
    char *temp = (char *)malloc(16);
    sprintf(temp, "t%d", temp_count++);
    return temp;
}

char *novoLabel()
{
    char *label = (char *)malloc(16);
    sprintf(label, "L%d", label_count++);
    return label;
}

char *gerarIR(NoAST *no, FILE *saida)
{
    if (no == NULL)
        return NULL;

    switch (no->tipo_no)
    {
    case NODE_LITERAL:
    {
        char *temp = novaTemp();
        switch (no->tipo_dado)
        {
        case TIPO_INT:
            fprintf(saida, "%s = %d\n", temp, no->data.literal.val_int);
            break;
        case TIPO_FLOAT:
            fprintf(saida, "%s = %f\n", temp, no->data.literal.val_float);
            break;
        case TIPO_DOUBLE:
            fprintf(saida, "%s = %lf\n", temp, no->data.literal.val_double);
            break;
        default:
            fprintf(stderr, "Literal com tipo não suportado.\n");
        }
        return temp;
    }
    case NODE_IDENTIFIER:
        return strdup(no->data.nome_id);

    case NODE_OPERATOR:
    {
        char *left = gerarIR(no->esquerda, saida);
        char *right = gerarIR(no->direita, saida);
        char *temp = novaTemp();

        const char *op_str;
        switch (no->data.op_type)
        {
        case OP_ADD_TYPE:
            op_str = "+"; break;
        case OP_SUB_TYPE:
            op_str = "-"; break;
        case OP_MUL_TYPE:
            op_str = "*"; break;
        case OP_DIV_TYPE:
            op_str = "/"; break;
        case OP_EQ_TYPE:
            op_str = "=="; break;
        case OP_NE_TYPE:
            op_str = "!="; break;
        case OP_LT_TYPE:
            op_str = "<"; break;
        case OP_LE_TYPE:
            op_str = "<="; break;
        case OP_GT_TYPE:
            op_str = ">"; break;
        case OP_GE_TYPE:
            op_str = ">="; break;

        default:
            op_str = "??"; break;
        }

        fprintf(saida, "%s = %s %s %s\n", temp, left, op_str, right);
        free(left);
        free(right);
        return temp;
    }

    case NODE_DECLARATION:
    {
        char *valor = gerarIR(no->data.decl_info.inicializacao_expr, saida);
        if (valor != NULL)
        {
            fprintf(saida, "%s = %s\n", no->data.decl_info.nome_declaracao, valor);
            free(valor);
        }
        else
        {
            fprintf(saida, "%s = 0\n", no->data.decl_info.nome_declaracao);
        }
        return NULL;
    }

    case NODE_RETURN:
    {
        char *expr = gerarIR(no->esquerda, saida);
        fprintf(saida, "return %s\n", expr);
        free(expr);
        return NULL;
    }

    case NODE_IF:
    {
        char *cond = gerarIR(no->esquerda, saida);
        char *label_else = novoLabel();
        char *label_end = novoLabel();

        fprintf(saida, "ifFalse %s goto %s\n", cond, label_else);
        gerarIR(no->direita, saida); 
        fprintf(saida, "goto %s\n", label_end);
        fprintf(saida, "%s:\n", label_else);
        if (no->centro)
            gerarIR(no->centro, saida); 
        else
        fprintf(saida, "%s:\n", label_end);
        free(cond);
        return NULL;
    }

    case NODE_WHILE:
    {
        char *label_inicio = novoLabel();
        char *label_fim = novoLabel();

        fprintf(saida, "%s:\n", label_inicio);
        char *cond = gerarIR(no->esquerda, saida);
        fprintf(saida, "ifFalse %s goto %s\n", cond, label_fim);
        gerarIR(no->direita, saida);
        fprintf(saida, "goto %s\n", label_inicio);
        fprintf(saida, "%s:\n", label_fim);
        free(cond);
        return NULL;
    }

    case NODE_FOR:
    {
        char *label_inicio = novoLabel();
        char *label_fim = novoLabel();

        gerarIR(no->esquerda, saida); 
        fprintf(saida, "%s:\n", label_inicio);

        char *cond = gerarIR(no->direita, saida); 
        fprintf(saida, "ifFalse %s goto %s\n", cond, label_fim);
        gerarIR(no->proximo, saida);
        gerarIR(no->centro, saida);  
        fprintf(saida, "goto %s\n", label_inicio);
        fprintf(saida, "%s:\n", label_fim);
        free(cond);
        return NULL;
    }

    case NODE_FUNCTION_CALL:
    {
        NoAST *arg = no->esquerda;
        int arg_count = 0;
        while (arg)
        {
            char *valor = gerarIR(arg, saida);
            fprintf(saida, "param %s\n", valor);
            free(valor);
            arg = arg->proximo;
            arg_count++;
        }
        char *ret = novaTemp();
        fprintf(saida, "%s = call %s, %d\n", ret, no->data.func_name, arg_count);
        return ret;
    }

    case NODE_COMPOUND_STMT:
    {
        NoAST *stmt = no->esquerda;
        while (stmt)
        {
            gerarIR(stmt, saida);
            stmt = stmt->proximo;
        }
        return NULL;
    }

    case NODE_BREAK:
        fprintf(saida, "break\n");
        return NULL;

    case NODE_CONTINUE:
        fprintf(saida, "continue\n");
        return NULL;

    default:
        fprintf(stderr, "Tipo de nó AST não suportado: %d\n", no->tipo_no);
        return NULL;
    }
}
