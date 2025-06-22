#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ast.h"
#include "gerador_ts.h"

bool jaDeclarada(VarDecl *lista, const char *nome)
{
    for (VarDecl *v = lista; v; v = v->proximo)
        if (strcmp(v->nome, nome) == 0)
            return true;
    return false;
}

void adicionarDeclarada(VarDecl **lista, const char *nome)
{
    VarDecl *novo = malloc(sizeof(VarDecl));
    strncpy(novo->nome, nome, 63);
    novo->nome[63] = '\0';
    novo->proximo = *lista;
    *lista = novo;
}

void gerarTypeScript(NoAST *no, FILE *saida, VarDecl **decls);
void gerarDeclaracaoNoFor(NoAST *no, FILE *saida, VarDecl **decls);
void gerarExpressaoNoFor(NoAST *no, FILE *saida, VarDecl **decls);

void gerarArgumentos(NoAST *arg, FILE *saida)
{
    while (arg)
    {
        gerarTypeScript(arg, saida, NULL);
        arg = arg->proximo;
        if (arg)
            fprintf(saida, ", ");
    }
}

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

void gerarTypeScript(NoAST *no, FILE *saida, VarDecl **decls)
{
    if (!no)
        return;

    switch (no->tipo_no)
    {
    case NODE_DECLARATION:
        if (!jaDeclarada(*decls, no->data.decl_info.nome_declaracao))
        {
            fprintf(saida, "let %s: %s", no->data.decl_info.nome_declaracao,
                    (no->tipo_dado == TIPO_OBJETO) ? "any" : ts_type(no->tipo_dado));
            if (no->data.decl_info.inicializacao_expr)
            {
                fprintf(saida, " = ");
                gerarTypeScript(no->data.decl_info.inicializacao_expr, saida, decls);
            }
            fprintf(saida, ";\n");
            adicionarDeclarada(decls, no->data.decl_info.nome_declaracao);
        }
        // Não gere atribuição separada para declaração já inicializada
        return;
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
        switch (no->data.op_type)
        {
        case OP_ASSIGN_TYPE:
            gerarTypeScript(no->esquerda, saida, decls);
            fprintf(saida, " = ");
            gerarTypeScript(no->direita, saida, decls);
            fprintf(saida, ";\n");
            return;
        case OP_ADD_ASSIGN_TYPE:
            gerarTypeScript(no->esquerda, saida, decls);
            fprintf(saida, " += ");
            gerarTypeScript(no->direita, saida, decls);
            fprintf(saida, ";\n");
            return;
        case OP_SUB_ASSIGN_TYPE:
            gerarTypeScript(no->esquerda, saida, decls);
            fprintf(saida, " -= ");
            gerarTypeScript(no->direita, saida, decls);
            fprintf(saida, ";\n");
            return;
        case OP_MUL_ASSIGN_TYPE:
            gerarTypeScript(no->esquerda, saida, decls);
            fprintf(saida, " *= ");
            gerarTypeScript(no->direita, saida, decls);
            fprintf(saida, ";\n");
            return;
        case OP_DIV_ASSIGN_TYPE:
            gerarTypeScript(no->esquerda, saida, decls);
            fprintf(saida, " /= ");
            gerarTypeScript(no->direita, saida, decls);
            fprintf(saida, ";\n");
            return;
        case OP_INC_TYPE:
            gerarTypeScript(no->esquerda, saida, decls);
            fprintf(saida, "++;\n");
            return;
        case OP_DEC_TYPE:
            gerarTypeScript(no->esquerda, saida, decls);
            fprintf(saida, "--;\n");
            return;
        case OP_ADD_TYPE:
            gerarTypeScript(no->esquerda, saida, decls);
            fprintf(saida, " + ");
            gerarTypeScript(no->direita, saida, decls);
            break;
        case OP_SUB_TYPE:
            gerarTypeScript(no->esquerda, saida, decls);
            fprintf(saida, " - ");
            gerarTypeScript(no->direita, saida, decls);
            break;
        case OP_MUL_TYPE:
            gerarTypeScript(no->esquerda, saida, decls);
            fprintf(saida, " * ");
            gerarTypeScript(no->direita, saida, decls);
            break;
        case OP_DIV_TYPE:
            gerarTypeScript(no->esquerda, saida, decls);
            fprintf(saida, " / ");
            gerarTypeScript(no->direita, saida, decls);
            break;
        case OP_MOD_TYPE:
            gerarTypeScript(no->esquerda, saida, decls);
            fprintf(saida, " %% ");
            gerarTypeScript(no->direita, saida, decls);
            break;
        case OP_EQ_TYPE:
            gerarTypeScript(no->esquerda, saida, decls);
            fprintf(saida, " == ");
            gerarTypeScript(no->direita, saida, decls);
            break;
        case OP_NE_TYPE:
            gerarTypeScript(no->esquerda, saida, decls);
            fprintf(saida, " != ");
            gerarTypeScript(no->direita, saida, decls);
            break;
        case OP_LT_TYPE:
            gerarTypeScript(no->esquerda, saida, decls);
            fprintf(saida, " < ");
            gerarTypeScript(no->direita, saida, decls);
            break;
        case OP_LE_TYPE:
            gerarTypeScript(no->esquerda, saida, decls);
            fprintf(saida, " <= ");
            gerarTypeScript(no->direita, saida, decls);
            break;
        case OP_GT_TYPE:
            gerarTypeScript(no->esquerda, saida, decls);
            fprintf(saida, " > ");
            gerarTypeScript(no->direita, saida, decls);
            break;
        case OP_GE_TYPE:
            gerarTypeScript(no->esquerda, saida, decls);
            fprintf(saida, " >= ");
            gerarTypeScript(no->direita, saida, decls);
            break;
        case OP_AND_TYPE:
            gerarTypeScript(no->esquerda, saida, decls);
            fprintf(saida, " && ");
            gerarTypeScript(no->direita, saida, decls);
            break;
        case OP_OR_TYPE:
            gerarTypeScript(no->esquerda, saida, decls);
            fprintf(saida, " || ");
            gerarTypeScript(no->direita, saida, decls);
            break;
        case OP_NOT_TYPE:
            fprintf(saida, "!");
            gerarTypeScript(no->esquerda, saida, decls);
            break;
        default:
            fprintf(saida, "/* operador não implementado (%d) */", no->data.op_type);
            break;
        }
        break;
    case NODE_RETURN:
        fprintf(saida, "return ");
        gerarTypeScript(no->esquerda, saida, decls);
        fprintf(saida, ";\n");
        break;
    case NODE_FUNCTION_CALL:
        if (strcmp(no->data.func_name, "printf") == 0)
        {
            if (no->esquerda && no->esquerda->tipo_no == NODE_LITERAL && no->esquerda->tipo_dado == TIPO_STRING)
            {
                // Separa a string de formato dos argumentos
                fprintf(saida, "console.log(");
                gerarTypeScript(no->esquerda, saida, decls); // string
                NoAST *arg = no->esquerda->proximo;
                while (arg)
                {
                    fprintf(saida, ", ");
                    gerarTypeScript(arg, saida, decls);
                    arg = arg->proximo;
                }
                fprintf(saida, ");\n");
            }
            else
            {
                // fallback
                fprintf(saida, "console.log(");
                gerarArgumentos(no->esquerda, saida);
                fprintf(saida, ");\n");
            }
        }
        else if (strcmp(no->data.func_name, "scanf") == 0)
        {
            fprintf(saida, "// scanf não suportado em TypeScript\n");
        }
        else
        {
            fprintf(saida, "%s(", no->data.func_name);
            gerarArgumentos(no->esquerda, saida);
            fprintf(saida, ");\n");
        }
        break;
    case NODE_COMPOUND_STMT:
    {
        VarDecl *escopo_local = NULL;
        fprintf(saida, "{\n");
        for (NoAST *stmt = no->esquerda; stmt; stmt = stmt->proximo)
            gerarTypeScript(stmt, saida, &escopo_local);
        fprintf(saida, "}\n");
        break;
    }
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
        fprintf(saida, "): %s ", ts_type(no->tipo_dado));
        gerarTypeScript(no->direita, saida, decls); // O corpo já imprime as chaves
        fprintf(saida, "\n");
        break;
    case NODE_IF:
        fprintf(saida, "if (");
        gerarTypeScript(no->esquerda, saida, decls);
        fprintf(saida, ") ");
        gerarTypeScript(no->direita, saida, decls); // then block
        if (no->centro)
        {
            fprintf(saida, "else ");
            gerarTypeScript(no->centro, saida, decls); // else block
        }
        break;
    case NODE_FOR:
        fprintf(saida, "for (");
        if (no->esquerda)
            gerarDeclaracaoNoFor(no->esquerda, saida, decls);
        fprintf(saida, "; ");
        if (no->direita)
            gerarExpressaoNoFor(no->direita, saida, decls);
        fprintf(saida, "; ");
        if (no->centro)
            gerarExpressaoNoFor(no->centro, saida, decls);
        fprintf(saida, ") "); // Não coloque \n aqui!
        if (no->proximo && no->proximo->tipo_no == NODE_COMPOUND_STMT)
            gerarTypeScript(no->proximo, saida, decls);
        else
        {
            fprintf(saida, "{\n");
            gerarTypeScript(no->proximo, saida, decls);
            fprintf(saida, "}\n");
        }
        break;
    case NODE_FIELD_ACCESS:
        // Exemplo: p1.x
        if (no->esquerda)
        {
            gerarTypeScript(no->esquerda, saida, decls);
            fprintf(saida, ".");
        }
        fprintf(saida, "%s", no->data.field_info.campo_nome);
        break;

    case NODE_FIELD_ASSIGN:
        // Exemplo: p1.x = valor;
        if (no->esquerda)
        {
            gerarTypeScript(no->esquerda, saida, decls);
            fprintf(saida, ".");
        }
        fprintf(saida, "%s = ", no->data.field_info.campo_nome);
        gerarTypeScript(no->direita, saida, decls);
        fprintf(saida, ";\n");
        break;
    case NODE_SWITCH:
        fprintf(saida, "switch (");
        gerarTypeScript(no->esquerda, saida, decls);
        fprintf(saida, ") ");
        gerarTypeScript(no->direita, saida, decls);
        break;

    case NODE_SWITCH_BODY:
        fprintf(saida, "{\n");
        gerarTypeScript(no->esquerda, saida, decls); // cases
        gerarTypeScript(no->direita, saida, decls);  // default
        fprintf(saida, "}\n");
        break;

    case NODE_CASE:
        fprintf(saida, "case ");
        gerarTypeScript(no->esquerda, saida, decls);
        fprintf(saida, ":\n");
        gerarTypeScript(no->direita, saida, decls);
        break;

    case NODE_DEFAULT:
        fprintf(saida, "default:\n");
        gerarTypeScript(no->esquerda, saida, decls);
        break;
    case NODE_BREAK:
        fprintf(saida, "break;\n");
        break;
    default:
        fprintf(saida, "// [Tipo de nó não implementado: %d]\n", no->tipo_no);
        break;
    }

    if (no->proximo)
        gerarTypeScript(no->proximo, saida, decls);
}

void gerarDeclaracaoNoFor(NoAST *no, FILE *saida, VarDecl **decls)
{
    if (!no)
        return;
    if (no->tipo_no == NODE_DECLARATION)
    {
        if (!jaDeclarada(*decls, no->data.decl_info.nome_declaracao))
        {
            fprintf(saida, "let %s: %s", no->data.decl_info.nome_declaracao,
                    (no->tipo_dado == TIPO_OBJETO) ? "any" : ts_type(no->tipo_dado));
            if (no->data.decl_info.inicializacao_expr)
            {
                fprintf(saida, " = ");
                gerarTypeScript(no->data.decl_info.inicializacao_expr, saida, decls);
            }
            adicionarDeclarada(decls, no->data.decl_info.nome_declaracao);
        }
        // NÃO imprime ;\n aqui!
    }
    else
    {
        gerarTypeScript(no, saida, decls);
    }
}

void gerarExpressaoNoFor(NoAST *no, FILE *saida, VarDecl **decls)
{
    if (!no)
        return;
    // Só imprime a expressão, sem ;\n ao final
    switch (no->tipo_no)
    {
    case NODE_OPERATOR:
        switch (no->data.op_type)
        {
        case OP_ASSIGN_TYPE:
        case OP_ADD_ASSIGN_TYPE:
        case OP_SUB_ASSIGN_TYPE:
        case OP_MUL_ASSIGN_TYPE:
        case OP_DIV_ASSIGN_TYPE:
            gerarTypeScript(no->esquerda, saida, decls);
            switch (no->data.op_type)
            {
            case OP_ASSIGN_TYPE:
                fprintf(saida, " = ");
                break;
            case OP_ADD_ASSIGN_TYPE:
                fprintf(saida, " += ");
                break;
            case OP_SUB_ASSIGN_TYPE:
                fprintf(saida, " -= ");
                break;
            case OP_MUL_ASSIGN_TYPE:
                fprintf(saida, " *= ");
                break;
            case OP_DIV_ASSIGN_TYPE:
                fprintf(saida, " /= ");
                break;
            default:
                break;
            }
            gerarTypeScript(no->direita, saida, decls);
            break;
        default:
            gerarTypeScript(no, saida, decls);
        }
        break;
    default:
        gerarTypeScript(no, saida, decls);
    }
}