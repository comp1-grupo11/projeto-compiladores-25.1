#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ast.h"
#include "gerador_ts.h"
#include "tabela.h"

#define MAX_STRUCTS 16
static char structs_usados[MAX_STRUCTS][32];
static int qtd_structs_usados = 0;

static void registrar_struct_usado(const char *nome)
{
    for (int i = 0; i < qtd_structs_usados; ++i)
    {
        if (strcmp(structs_usados[i], nome) == 0)
            return;
    }
    if (qtd_structs_usados < MAX_STRUCTS)
    {
        strncpy(structs_usados[qtd_structs_usados], nome, 31);
        structs_usados[qtd_structs_usados][31] = '\0';
        qtd_structs_usados++;
    }
}

// Função auxiliar para obter o nome do tipo struct (ex: Point)
static const char *struct_type_name(const char *var_name)
{
    Simbolo *s = buscarSimbolo((char *)var_name);
    if (s && s->tipo == TIPO_OBJETO && s->nome_struct[0] != '\0')
    {
        registrar_struct_usado(s->nome_struct);
        return s->nome_struct;
    }
    return "any";
}

// Função auxiliar para inicializar struct Point
static void inicializar_struct_padrao(FILE *saida, const char *tipo)
{
    if (strcmp(tipo, "Point") == 0 || strcmp(tipo, "Ponto") == 0)
    {
        fprintf(saida, "{ x: 0, y: 0 }");
    }
    else
    {
        fprintf(saida, "{} ");
    }
}

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

void gerarTypeScript(NoAST *no, FILE *saida, VarDecl **decls, int ident, int isExpr);
void gerarDeclaracaoNoFor(NoAST *no, FILE *saida, VarDecl **decls);
void gerarExpressaoNoFor(NoAST *no, FILE *saida, VarDecl **decls);

void gerarArgumentos(NoAST *arg, FILE *saida)
{
    while (arg)
    {
        gerarTypeScript(arg, saida, NULL, 0, 1);
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

static void gerar_cabecalhos_structs(FILE *saida)
{
    for (int i = 0; i < qtd_structs_usados; ++i)
    {
        if (strcmp(structs_usados[i], "Point") == 0 || strcmp(structs_usados[i], "Ponto") == 0)
        {
            fprintf(saida, "type %s = {\n  x: number;\n  y: number;\n};\n\n", structs_usados[i]);
        }
        else
        {
            fprintf(saida, "type %s = {\n  // campos\n};\n\n", structs_usados[i]);
        }
    }
}

// Garante que o type Point seja emitido no início do arquivo
void gerarTypeScript(NoAST *no, FILE *saida, VarDecl **decls, int ident, int isExpr)
{
    static int ja_gerou_cabecalhos = 0;
    if (!ja_gerou_cabecalhos)
    {
        gerar_cabecalhos_structs(saida);
        ja_gerou_cabecalhos = 1;
    }

    while (no)
    {

        switch (no->tipo_no)
        {
        // Mantém apenas UM bloco para cada tipo abaixo:
        case NODE_DECLARATION:
            if (!jaDeclarada(*decls, no->data.decl_info.nome_declaracao))
            {
                if (no->tipo_dado == TIPO_OBJETO)
                {
                    const char *tipo = struct_type_name(no->data.decl_info.nome_declaracao);
                    fprintf(saida, "let %s: %s = ", no->data.decl_info.nome_declaracao, tipo);
                    NoAST *init = no->data.decl_info.inicializacao_expr;
                    if (init && init->tipo_no == NODE_FIELD_ASSIGN && strcmp(tipo, "Point") == 0)
                    {
                        // Busca os campos x e y na lista, independente da ordem
                        NoAST *field = init;
                        NoAST *fx = NULL, *fy = NULL;
                        while (field)
                        {
                            if (field->tipo_no == NODE_FIELD_ASSIGN)
                            {
                                if (strcmp(field->data.field_info.campo_nome, "x") == 0)
                                    fx = field;
                                if (strcmp(field->data.field_info.campo_nome, "y") == 0)
                                    fy = field;
                            }
                            field = field->proximo;
                        }
                        fprintf(saida, "{ x: ");
                        if (fx)
                            gerarTypeScript(fx->direita, saida, decls, ident, 1);
                        else
                            fprintf(saida, "0");
                        fprintf(saida, ", y: ");
                        if (fy)
                            gerarTypeScript(fy->direita, saida, decls, ident, 1);
                        else
                            fprintf(saida, "0");
                        fprintf(saida, " };");
                    }
                    else if (no->data.decl_info.inicializacao_expr)
                    {
                        gerarTypeScript(no->data.decl_info.inicializacao_expr, saida, decls, ident, 1);
                        fprintf(saida, ";");
                    }
                    else
                    {
                        inicializar_struct_padrao(saida, tipo);
                        fprintf(saida, ";");
                    }
                    fprintf(saida, "\n");
                }
                else
                {
                    // Detecta atribuição composta do tipo i = i + 2; e transforma em i += 2;
                    NoAST *init = no->data.decl_info.inicializacao_expr;
                    if (init && init->tipo_no == NODE_OPERATOR &&
                        ((init->data.op_type == OP_ADD_TYPE || init->data.op_type == OP_SUB_TYPE || init->data.op_type == OP_MUL_TYPE || init->data.op_type == OP_DIV_TYPE) &&
                         init->esquerda && init->esquerda->tipo_no == NODE_IDENTIFIER &&
                         strcmp(no->data.decl_info.nome_declaracao, init->esquerda->data.nome_id) == 0))
                    {
                        fprintf(saida, "let %s: %s = 0;\n", no->data.decl_info.nome_declaracao, ts_type(no->tipo_dado));
                        fprintf(saida, "%s ", no->data.decl_info.nome_declaracao);
                        switch (init->data.op_type)
                        {
                        case OP_ADD_TYPE:
                            fprintf(saida, "+= ");
                            break;
                        case OP_SUB_TYPE:
                            fprintf(saida, "-= ");
                            break;
                        case OP_MUL_TYPE:
                            fprintf(saida, "*= ");
                            break;
                        case OP_DIV_TYPE:
                            fprintf(saida, "/= ");
                            break;
                        default:
                            break;
                        }
                        gerarTypeScript(init->direita, saida, decls, ident, 1);
                        fprintf(saida, ";\n");
                    }
                    else
                    {
                        fprintf(saida, "let %s: %s", no->data.decl_info.nome_declaracao, ts_type(no->tipo_dado));
                        if (no->data.decl_info.inicializacao_expr)
                        {
                            fprintf(saida, " = ");
                            gerarTypeScript(no->data.decl_info.inicializacao_expr, saida, decls, ident, 1);
                        }
                        fprintf(saida, ";\n");
                    }
                }
                adicionarDeclarada(decls, no->data.decl_info.nome_declaracao);
            }
            return;
        case NODE_RETURN:
            if (!isExpr)
            {
                static int ja_teve_return = 0;
                if (!ja_teve_return)
                {
                    fprintf(saida, "return ");
                    gerarTypeScript(no->esquerda, saida, decls, ident, 1);
                    fprintf(saida, ";\n");
                    ja_teve_return = 1;
                }
            }
            else
            {
                gerarTypeScript(no->esquerda, saida, decls, ident, 1);
            }
            return;
        case NODE_FUNCTION_CALL:
            if (strcmp(no->data.func_name, "printf") == 0)
            {
                // Se for printf com múltiplos argumentos, converte para template string
                NoAST *fmt = no->esquerda;
                if (fmt && fmt->tipo_no == NODE_LITERAL && fmt->tipo_dado == TIPO_STRING && fmt->proximo)
                {
                    // Constrói template string
                    const char *format = fmt->data.literal.val_string;
                    fprintf(saida, "console.log(`");
                    const char *p = format;
                    int arg_idx = 0;
                    NoAST *arg = fmt->proximo;
                    while (*p)
                    {
                        if (*p == '%' && (*(p + 1) == 'd' || *(p + 1) == 'f' || *(p + 1) == 'c' || *(p + 1) == 's'))
                        {
                            fprintf(saida, "${");
                            if (arg)
                            {
                                gerarTypeScript(arg, saida, decls, ident, 1);
                                arg = arg->proximo;
                            }
                            fprintf(saida, "}");
                            p += 2;
                        }
                        else
                        {
                            fputc(*p, saida);
                            p++;
                        }
                    }
                    fprintf(saida, "`);\n");
                }
                else if (fmt && fmt->tipo_no == NODE_LITERAL && fmt->tipo_dado == TIPO_STRING)
                {
                    fprintf(saida, "console.log(");
                    gerarTypeScript(fmt, saida, decls, ident, 1);
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
            // Só gera bloco { ... } se NÃO for filho de comando de controle
            if (no->pai_controlador == NULL)
            {
                VarDecl *escopo_local = NULL;
                fprintf(saida, "{\n");
                for (NoAST *stmt = no->esquerda; stmt; stmt = stmt->proximo)
                    gerarTypeScript(stmt, saida, &escopo_local, ident, 0);
                fprintf(saida, "}\n");
            }
            else
            {
                // Se for filho de comando de controle, só gera os statements internos
                for (NoAST *stmt = no->esquerda; stmt; stmt = stmt->proximo)
                    gerarTypeScript(stmt, saida, decls, ident, 0);
            }
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
            gerarTypeScript(no->direita, saida, decls, ident, 0);
            fprintf(saida, "\n\n"); // Adiciona espaço extra entre funções
            break;
        case NODE_IF:
            fprintf(saida, "if (");
            gerarTypeScript(no->esquerda, saida, decls, ident, 1);
            fprintf(saida, ") {\n");
            gerarTypeScript(no->direita, saida, decls, ident, 0);
            fprintf(saida, "}\n");
            if (no->centro)
            {
                fprintf(saida, "else {\n");
                gerarTypeScript(no->centro, saida, decls, ident, 0);
                fprintf(saida, "}\n");
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
            fprintf(saida, ") {\n");
            if (no->proximo)
                gerarTypeScript(no->proximo, saida, decls, ident, 0);
            fprintf(saida, "}\n");
            break;
        case NODE_SWITCH:
            fprintf(saida, "switch (");
            if (no->esquerda && no->esquerda->tipo_no == NODE_OPERATOR && no->esquerda->data.op_type == OP_MOD_TYPE)
            {
                gerarTypeScript(no->esquerda->esquerda, saida, decls, ident, 1);
                fprintf(saida, " %% ");
                gerarTypeScript(no->esquerda->direita, saida, decls, ident, 1);
            }
            else
            {
                gerarTypeScript(no->esquerda, saida, decls, ident, 1);
            }
            fprintf(saida, ") {\n");
            // Suporte a NODE_SWITCH_BODY (lista de cases/default)
            NoAST *body = no->direita;
            if (body && body->tipo_no == NODE_SWITCH_BODY)
            {
                NoAST *cases = body->esquerda;
                while (cases)
                {
                    if (cases->tipo_no == NODE_CASE)
                    {
                        fprintf(saida, "  case ");
                        gerarTypeScript(cases->esquerda, saida, decls, ident, 1);
                        fprintf(saida, ":\n");
                        gerarTypeScript(cases->direita, saida, decls, ident + 2, 0);
                    }
                    else if (cases->tipo_no == NODE_DEFAULT)
                    {
                        fprintf(saida, "  default:\n");
                        gerarTypeScript(cases->esquerda, saida, decls, ident + 2, 0);
                    }
                    cases = cases->proximo;
                }
                // Se houver default separado
                if (body->direita && body->direita->tipo_no == NODE_DEFAULT)
                {
                    fprintf(saida, "  default:\n");
                    gerarTypeScript(body->direita->esquerda, saida, decls, ident + 2, 0);
                }
            }
            else
            {
                while (body)
                {
                    if (body->tipo_no == NODE_CASE)
                    {
                        fprintf(saida, "  case ");
                        gerarTypeScript(body->esquerda, saida, decls, ident, 1);
                        fprintf(saida, ":\n");
                        gerarTypeScript(body->direita, saida, decls, ident + 2, 0);
                    }
                    else if (body->tipo_no == NODE_DEFAULT)
                    {
                        fprintf(saida, "  default:\n");
                        gerarTypeScript(body->esquerda, saida, decls, ident + 2, 0);
                    }
                    body = body->proximo;
                }
            }
            fprintf(saida, "}\n");
            break;
        case NODE_CASE:
            fprintf(saida, "  case ");
            gerarTypeScript(no->esquerda, saida, decls, ident, 1);
            fprintf(saida, ":\n");
            gerarTypeScript(no->direita, saida, decls, ident, 0);
            break;
        case NODE_DEFAULT:
            fprintf(saida, "  default:\n");
            gerarTypeScript(no->esquerda, saida, decls, ident, 0);
            break;
        case NODE_FIELD_ASSIGN:
            if (no->esquerda)
            {
                gerarTypeScript(no->esquerda, saida, decls, ident, 1);
                fprintf(saida, ".");
            }
            fprintf(saida, "%s = ", no->data.field_info.campo_nome);
            gerarTypeScript(no->direita, saida, decls, ident, 1);
            fprintf(saida, ";\n");
            break;
        case NODE_FIELD_ACCESS:
            if (no->esquerda)
            {
                gerarTypeScript(no->esquerda, saida, decls, ident, 1);
                fprintf(saida, ".");
            }
            fprintf(saida, "%s", no->data.field_info.campo_nome);
            break;
        case NODE_LITERAL:
            if (no->tipo_dado == TIPO_FLOAT)
                fprintf(saida, "%g", no->data.literal.val_float);
            else
                switch (no->tipo_dado)
                {
                case TIPO_INT:
                    fprintf(saida, "%d", no->data.literal.val_int);
                    break;
                case TIPO_DOUBLE:
                    fprintf(saida, "%lf", no->data.literal.val_double);
                    break;
                case TIPO_CHAR:
                    fprintf(saida, "\"%c\"", no->data.literal.val_char);
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
            // Detecta operadores compostos do tipo i = i + 2; e transforma em i += 2; em comandos também
            if (!isExpr && no->data.op_type == OP_ASSIGN_TYPE &&
                no->direita && no->direita->tipo_no == NODE_OPERATOR &&
                (no->direita->data.op_type == OP_ADD_TYPE || no->direita->data.op_type == OP_SUB_TYPE || no->direita->data.op_type == OP_MUL_TYPE || no->direita->data.op_type == OP_DIV_TYPE) &&
                no->direita->esquerda && no->direita->esquerda->tipo_no == NODE_IDENTIFIER &&
                no->esquerda && no->esquerda->tipo_no == NODE_IDENTIFIER &&
                strcmp(no->esquerda->data.nome_id, no->direita->esquerda->data.nome_id) == 0)
            {
                gerarTypeScript(no->esquerda, saida, decls, ident, 1);
                switch (no->direita->data.op_type)
                {
                case OP_ADD_TYPE:
                    fprintf(saida, " += ");
                    break;
                case OP_SUB_TYPE:
                    fprintf(saida, " -= ");
                    break;
                case OP_MUL_TYPE:
                    fprintf(saida, " *= ");
                    break;
                case OP_DIV_TYPE:
                    fprintf(saida, " /= ");
                    break;
                default:
                    break;
                }
                gerarTypeScript(no->direita->direita, saida, decls, ident, 1);
                fprintf(saida, ";\n");
                return;
            }
            // Operadores binários comuns
            if (no->esquerda && no->direita)
            {
                gerarTypeScript(no->esquerda, saida, decls, ident, 1);
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
                case OP_MOD_TYPE:
                    fprintf(saida, " % ");
                    break;
                case OP_EQ_TYPE:
                    fprintf(saida, " == ");
                    break;
                case OP_NE_TYPE:
                    fprintf(saida, " != ");
                    break;
                case OP_LT_TYPE:
                    fprintf(saida, " < ");
                    break;
                case OP_LE_TYPE:
                    fprintf(saida, " <= ");
                    break;
                case OP_GT_TYPE:
                    fprintf(saida, " > ");
                    break;
                case OP_GE_TYPE:
                    fprintf(saida, " >= ");
                    break;
                case OP_AND_TYPE:
                    fprintf(saida, " && ");
                    break;
                case OP_OR_TYPE:
                    fprintf(saida, " || ");
                    break;
                default:
                    break;
                }
                gerarTypeScript(no->direita, saida, decls, ident, 1);
                if (!isExpr)
                    fprintf(saida, ";\n");
            }
            else if (no->esquerda)
            {
                // Operadores unários
                switch (no->data.op_type)
                {
                case OP_INC_TYPE:
                    gerarTypeScript(no->esquerda, saida, decls, ident, 1);
                    fprintf(saida, "++");
                    if (!isExpr)
                        fprintf(saida, ";\n");
                    break;
                case OP_DEC_TYPE:
                    gerarTypeScript(no->esquerda, saida, decls, ident, 1);
                    fprintf(saida, "--");
                    if (!isExpr)
                        fprintf(saida, ";\n");
                    break;
                case OP_NOT_TYPE:
                    fprintf(saida, "!");
                    gerarTypeScript(no->esquerda, saida, decls, ident, 1);
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            if (!isExpr)
                fprintf(saida, "// [Tipo de nó não implementado: %d]\n", no->tipo_no);
            break;
        }

        // Move para o próximo nó irmão
        no = no->proximo;
    }
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
                gerarTypeScript(no->data.decl_info.inicializacao_expr, saida, decls, 0, 1);
            }
            adicionarDeclarada(decls, no->data.decl_info.nome_declaracao);
        }
        // NÃO imprime ;\n aqui!
    }
    else
    {
        gerarTypeScript(no, saida, decls, 0, 1);
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
            gerarTypeScript(no->esquerda, saida, decls, 0, 1);
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
            gerarTypeScript(no->direita, saida, decls, 0, 1);
            break;
        default:
            gerarTypeScript(no, saida, decls, 0, 1);
        }
        break;
    default:
        gerarTypeScript(no, saida, decls, 0, 1);
    }
}